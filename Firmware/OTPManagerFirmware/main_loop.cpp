#include <Arduino.h>
#include <LowPower.h>
#include "configuration.h"
#include "debug.h"
#include "main_loop.h"
#include "comm_helper.h"
#include "gui_ssd1306_i2c.h"
#include "clock_helper_DS3231.h"

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

static CommHelper commHelper(&SERIAL_PORT);
static ClockHelperDS3231 clockHelper(RTC_POWER_PIN);
static GuiSSD1306_I2C gui(DISPLAY_POWER_PIN,DISPLAY_ADDR,(ClockHelperBase*)(&clockHelper));

static volatile bool buttonPressed=false;
static unsigned long lastTime=0;
static MenuItem curMenuItem(MenuItemType::MainMenu,0);

void send_resync()
{
  commHelper.SendAnswer(AnsType::Resync,nullptr,0);
}

void resync()
{
  send_resync();
  uint8_t resyncState=0;
  while(true)
  {
    SYNC_ERR();
    gui.ShowCDScr();
    //read request
    auto request=commHelper.ReceiveRequest();
    //check header
    if(request.reqType==ReqType::Invalid)
    {
      send_resync();
      resyncState=0;
      continue;
    }
    if(resyncState==1)
    {
      //if request is ResyncComplete
      if(request.reqType==ReqType::ResyncComplete)
      {
        //send ANS_OK
        if(!commHelper.SendAnswer(AnsType::Ok,nullptr,0))
        {
          send_resync();
          resyncState=0;
          continue;
        }
        //resync complete!
        SYNC_OK();
        gui.ShowCEScr();
        return;
      }
      resyncState=0;
    }
    //final resync-sequence
    if(request.reqType!=ReqType::Resync || !commHelper.SendAnswer(AnsType::Resync,request.payload,request.plLen))
    {
      send_resync();
      continue;
    }
    resyncState=1;
  }
}

void conn_loop()
{
  LOG(F("Entering connection handling loop"));
  //TODO: activate watchdog for handling connection-loss and stalled resync
  resync();
  while(true)
  {
    //read request
    auto request=commHelper.ReceiveRequest();
    uint8_t result=1;
    //perform action
    switch (request.reqType)
    {
      default:
      case ReqType::Invalid:
      case ReqType::Resync:
      case ReqType::ResyncComplete:
        result=0;
        //TODO: ping watchdog
        break;
    }
    if(!result)
      resync();
  }
}

void wakeup()
{
  //TODO: setup power state of MCU components
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

void update_menu()
{
  //TODO: reload menu items
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
  //TODO: read settings
  //TODO: deactivate watchdog
  SYNC_LED_PREP();
  SYNC_ERR();
  commHelper.Init(SERIAL_PORT_SPEED);
  RX_PIN_PREP(); // enable pullup on serial RX-pin
  commHelper.FlushInput(); //flush incoming input
  //pre-init external low-powered devices
  gui.InitPre();
  clockHelper.InitPre();
  delay(10);
  //post-init external low-powered devices
  gui.InitPost();
  clockHelper.InitPost();
  update_menu();
  //install button interrupts
  attachInterrupt(digitalPinToInterrupt(BUTTON_NEXT_PIN),next_button_handler, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON_SELECT_PIN),select_button_handler, CHANGE);
  if(commHelper.DataAvailable())
  {
    conn_loop();
    //TODO: deactivate watchdog
    //TODO: reset button status
    update_menu();
  }
  //update current-time
  clockHelper.Update();
  curMenuItem=gui.ResetToMainScr();
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
    //TODO: deactivate watchdog
    //TODO: reset button status
    update_menu();
    curMenuItem=gui.ResetToMainScr();
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
    //TODO: perform action
    //reset last-time
    lastTime=millis();
    return;
  }

  if (button==1)
  {
    STATUS();
    LOG(F("Select button pressed"));
    //TODO: perform action
    //reset last-time
    lastTime=millis();
    return;
  }

  if((timeDiff>DEFAULT_IDLE_TIMEOUT && curMenuItem.itemType==MenuItemType::MainMenu) ||
    (timeDiff>DEFAULT_CODE_TIMEOUT && curMenuItem.itemType==MenuItemType::ProfileItem))
  {
    //activate powersave mode
    descend();
    //<< execution will be paused here >>
    wakeup();
    clockHelper.Update();
    curMenuItem=gui.ResetToMainScr();
    //reset last-time
    lastTime=millis();
  }
}
