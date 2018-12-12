#include <Arduino.h>
#include <LowPower.h>
#include "debug.h"
#include "configuration.h"
#include "main_loop.h"
#include "comm_helper.h"
#include "cmd_processor.h"
#include "lcgen.h"
#include "watchdog.h"
#include "watchdog_AVR.h"
#include "cipher.h"
#include "aes128.h"
#include "settings_manager.h"
#include "eeprom_settings_manager.h"
#include "profile_manager.h"
#include "eeprom_profile_manager.h"
#include "clock_helper.h"
#include "clock_helper_DS3231.h"
#include "gui.h"
#include "gui_ssd1306_i2c.h"


//If SERIAL_RX_PIN defined, define macro to enable pullup on serial rx-pin
//We need this in order to prevent false incoming connection events when device enabled and not connected to PC
#ifdef SERIAL_RX_PIN
#define RX_PIN_PREP() ({pinMode(SERIAL_RX_PIN,INPUT_PULLUP);})
#else
#define RX_PIN_PREP() ({})
#endif

#ifdef LED_SYNC
#define SYNC_OK() ({digitalWrite(LED_SYNC, HIGH);})
#define SYNC_ERR() ({digitalWrite(LED_SYNC, LOW);})
#define SYNC_LED_PREP() ({pinMode(LED_SYNC, OUTPUT);})
#else
#define SYNC_OK() ({})
#define SYNC_ERR() ({})
#define SYNC_LED_PREP() ({})
#endif

//create main logic blocks and perform "poor man's" dependency injection
static CommHelper commHelper(SERIAL_PORT);
static LCGen reqLCG(0);
static LCGen ansLCG(0);
static uint8_t encKey[ENCRYPTION_KEY_LEN] = ENCRYPTION_KEY;
static uint8_t encTweak[ENCRYPTION_TWEAK_LEN] = ENCRYPTION_TWEAK;
static AES128 cipher;
static EEPROMSettingsManager settingsManager(EEPROM_SETTINGS_ADDR, EEPROM_SETTINGS_LEN, cipher, encKey, encTweak);
static EEPROMProfileManager profileManager(EEPROM_PROFILE_STORAGE_ADDR, EEPROM_PROFILE_STORAGE_LEN, cipher, encKey, encTweak);
static ClockHelperDS3231 clockHelper(RTC_POWER_PIN, settingsManager);
static GuiSSD1306_I2C gui(DISPLAY_POWER_PIN, DISPLAY_ADDR, clockHelper, profileManager);
static CmdProcessor cmdProcessor(clockHelper);
static WatchdogAVR watchdog(SYNC_WATCHDOG_TIMEOUT);

//stuff needed for requests, commands and responses parsing
static uint8_t commandBuffer[COMMAND_BUFFER_SIZE];
static uint8_t responseBuffer[RESPONSE_BUFFER_SIZE];
static CMDRSP_BUFF_TYPE cmdBuffPos=0;
static CMDRSP_BUFF_TYPE rspBuffPos=0;
static CMDRSP_BUFF_TYPE rspSize=0;
static uint8_t cmdType=0;
static uint8_t rspType=0;

static volatile bool buttonPressed=false;
static unsigned long lastTime=0;

void resync()
{
	SYNC_ERR();
	gui.ShowCDScr();
	commHelper.SendAnswer(AnsType::Resync,0,nullptr,0);
	watchdog.Ping();
	uint8_t resyncState=0;
	while(true)
	{
		//read request
		auto request=commHelper.ReceiveRequest();
		//we have received something, resync process is not dead
		watchdog.Ping();
		//check header
		if(request.reqType==ReqType::Invalid)
		{
			commHelper.SendAnswer(AnsType::Resync,0,nullptr,0);
			resyncState=0;
			continue;
		}
		if(resyncState==1)
		{
			//if request is ResyncComplete, TODO: check req seq-number
			if(request.reqType==ReqType::ResyncComplete && request.seq==reqLCG.GenerateValue())
			{
				//send ANS_OK with current sequence number from ans-LCG
				if(!commHelper.SendAnswer(AnsType::Ok,ansLCG.GenerateValue(),nullptr,0))
				{
					commHelper.SendAnswer(AnsType::Resync,0,nullptr,0);
					resyncState=0;
					continue;
				}
				//resync complete!
				SYNC_OK();
				watchdog.Ping();
				gui.ShowCEScr();
				watchdog.Ping();
				return;
			}
			resyncState=0;
		}
		//final resync-sequence
		if(request.reqType!=ReqType::Resync || request.seq==0 || !commHelper.SendAnswer(AnsType::Resync,request.seq,request.payload,request.plLen))
		{
			commHelper.SendAnswer(AnsType::Resync,0,nullptr,0);
			continue;
		}
		//TODO: set req and ans LCGs
		reqLCG=LCGen(request.seq);
		ansLCG=LCGen(request.seq);
		resyncState=1;
	}
}

void conn_loop()
{
	LOG(F("Entering connection handling loop"));
	//activate watchdog for handling connection-loss and stalled resync
	watchdog.Enable();
	resync();
	while(true)
	{
		//read request
		auto request=commHelper.ReceiveRequest();
		uint8_t result=1;
		//ping watchdog
		watchdog.Ping();
		//disable watchdog while performing requested action
		watchdog.Disable();
		//check sequence number
		if(request.seq!=reqLCG.GenerateValue())
			result=0;
		else
			switch (request.reqType)
			{
				case ReqType::Ping:
					result=commHelper.SendAnswer(AnsType::Pong,ansLCG.GenerateValue(),nullptr,0);
					break;
				case ReqType::Command:
					if(request.payload[0]==0)
					{
						//commit command
						auto rspParams=cmdProcessor.ProcessCommand(cmdType,commandBuffer,cmdBuffPos,responseBuffer);
						//fillup results
						rspType=rspParams.rspType;
						rspSize=rspParams.rspLen;
						//reset cmdType;
						cmdType=0;
					}
					else
						//save command type
						cmdType=request.payload[0];
					//reset command buffer pos
					cmdBuffPos=0;
					//send ok
					result=commHelper.SendAnswer(AnsType::Ok,ansLCG.GenerateValue(),nullptr,0);
					break;
				case ReqType::CommandData:
					//copy command data
					for(uint8_t cnt=0;cnt<request.plLen;++cnt)
						*(commandBuffer+cmdBuffPos++)=*(request.payload+cnt);
					//send ok
					result=commHelper.SendAnswer(AnsType::Ok,ansLCG.GenerateValue(),nullptr,0);
					break;
				case ReqType::DataRequest:
					if(request.payload[0]==0)
					{
						//reset response position
						rspBuffPos=0;
						//send answer with response type
						uint8_t rspTypeTmp[1];
						rspTypeTmp[0]=rspType;
						result=commHelper.SendAnswer(AnsType::DataMarker,ansLCG.GenerateValue(),rspTypeTmp,1);
					}
					else
					{
						//get next response-data chunk
						CMDRSP_BUFF_TYPE rspChunkSz=CMD_MAX_PLSZ;
						auto availBuffSz=rspSize-rspBuffPos;
						if(rspChunkSz>availBuffSz)
							rspChunkSz=static_cast<CMDRSP_BUFF_TYPE>(availBuffSz);
						//send chunk to client
						result=commHelper.SendAnswer(AnsType::Data,ansLCG.GenerateValue(),responseBuffer+rspBuffPos,rspChunkSz);
						rspBuffPos+=rspChunkSz;
					}
					break;
				default:
					result=0; // we will run resync
					break;
			}
		//reenable watchdog
		watchdog.Enable();
		if(!result)
		{
			commHelper.FlushInput(); //flush incoming data-buffer
			resync();
		}
	}
}

void wakeup()
{
	//flush incoming data on the serial line
	commHelper.FlushInput();
	//pre-wakeup, enable power on all devices
	gui.WakeupPre();
	clockHelper.WakeupPre();
	//wait for stable power
	delay(10);
	//post-wakeup, reinitialize devices
	gui.WakeupPost();
	clockHelper.WakeupPost();
}

void descend()
{
	//pre-descend, prepare devices to powering-off
	gui.DescendPre();
	clockHelper.DescendPre();
	//post-descend, physically power-off devices, no device-communication on this stage is allowed
	gui.DescendPost();
	clockHelper.DescendPost();
	//set MCU to sleep state
	LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}

void next_button_handler()
{
	buttonPressed=true;
}

void select_button_handler()
{
	buttonPressed=true;
}

void setup()
{
	//read settings
	settingsManager.Init();
	//deactivate watchdog
	watchdog.Disable();
	SYNC_LED_PREP();
	SYNC_ERR();
	SERIAL_PORT.begin(SERIAL_PORT_SPEED);
	SERIAL_PORT.setTimeout(CMD_TIMEOUT);
	RX_PIN_PREP(); // enable pullup on serial RX-pin
	commHelper.FlushInput(); //flush incoming input
	//pre-init external low-powered devices
	gui.InitPre();
	clockHelper.InitPre();
	delay(10);
	//post-init external low-powered devices
	gui.InitPost();
	clockHelper.InitPost();
	gui.Reseed();
	//install button interrupts
	attachInterrupt(digitalPinToInterrupt(BUTTON_NEXT_PIN),next_button_handler, CHANGE);
	attachInterrupt(digitalPinToInterrupt(BUTTON_SELECT_PIN),select_button_handler, CHANGE);
	if(commHelper.DataAvailable())
	{
		conn_loop();
		watchdog.Disable();
		buttonPressed=false;
	}
	//update current-time
	gui.ResetToMainScr();
	//reset last-time
	lastTime=millis();
	STATUS();
	LOG(F("Setup complete!"));
}

void loop()
{
	//initialize resync routine, if any incoming data detected on serial
	if(commHelper.DataAvailable())
	{
		conn_loop();
		watchdog.Disable();
		buttonPressed=false;
		gui.ResetToMainScr();
	}

	//read current time
	auto currentTime=millis();
	//fix timer overflow
	if(currentTime<lastTime)
		lastTime=currentTime;
	auto timeDiff=currentTime-lastTime;
	//delay between buttons press
	if(timeDiff<BUTTON_DELAY_MS)
		return;

	//get current state of the buttons only once
	uint8_t button=0;
	if(buttonPressed)
	{
		if(digitalRead(BUTTON_NEXT_PIN))
			button=2;
		else if(digitalRead(BUTTON_SELECT_PIN))
			button=1;
		else
			lastTime=millis();
		buttonPressed=false;
	}
	else if(digitalRead(BUTTON_NEXT_PIN) || digitalRead(BUTTON_SELECT_PIN))
		lastTime=millis();

	//detect button event, perform action
	if(button==2)
	{
		STATUS();
		LOG(F("Next button pressed"));
		gui.MenuNext();
		//reset last-time
		lastTime=millis();
		return;
	}

	if (button==1)
	{
		STATUS();
		LOG(F("Select button pressed"));
		gui.MenuSelect();
		//reset last-time
		lastTime=millis();
		return;
	}

	if((timeDiff>DEFAULT_IDLE_TIMEOUT && gui.GetCurItem().itemType==MenuItemType::MainScreen) ||
	   (timeDiff>DEFAULT_CODE_TIMEOUT && gui.GetCurItem().itemType==MenuItemType::ProfileItem))
	{
		//activate powersave mode
		descend();
		//<< execution will be paused here >>
		wakeup();
		gui.ResetToMainScr();
		//reset last-time
		lastTime=millis();
		return;
	}

	if(timeDiff>DEFAULT_MENU_TIMEOUT && gui.GetCurItem().itemType==MenuItemType::ProfileMenu)
	{
		gui.ResetToMainScr();
		//reset last-time
		lastTime=millis();
		return;
	}
}
