#include "configuration.h"

#ifdef STANDALONE_APP

#include "standalone_config.h"
#include "arduino-defines.h"
#include "serial.h"

#define SYNC_OK() ({})
#define SYNC_ERR() ({})
#define SYNC_LED_PREP() ({})
#define RTC_POWER_PREP() ({})
#define RTC_POWER_ON() ({})
#define RTC_POWER_OFF() ({})
#define RX_PIN_PREP() ({})

#else

#include <Arduino.h>
#define LOG(...) ({})
#define SYNC_OK() ({digitalWrite(LED_SYNC, HIGH);})
#define SYNC_ERR() ({digitalWrite(LED_SYNC, LOW);})
#define SYNC_LED_PREP() ({pinMode(LED_SYNC, OUTPUT);})
#define RTC_POWER_PREP() ({pinMode(RTC_POWER_PIN, OUTPUT);})
#define RTC_POWER_ON() ({digitalWrite(RTC_POWER_PIN, HIGH);})
#define RTC_POWER_OFF() ({digitalWrite(RTC_POWER_PIN, LOW);})

//If SERIAL_RX_PIN defined, define macro to enable pullup on serial rx-pin
//We need this in order to prevent false incoming connection events when device enabled and not connected to PC
#ifdef SERIAL_RX_PIN
#define RX_PIN_PREP() ({pinMode(SERIAL_RX_PIN,INPUT_PULLUP);})
#else
#define RX_PIN_PREP() ({})
#endif

#endif

#include "main_loop.h"
#include "comm_helper.h"
#include "gui.h"

static CommHelper commHelper(&SERIAL_PORT);
static GuiU8G2 gui;

void send_resync()
{
  LOG("send_resync: sending resync-pending notification");
  commHelper.SendAnswer(AnsType::Resync,nullptr,0);
}

void resync()
{
  LOG("Starting resync");
  send_resync();
  uint8_t resyncState=0;
  while(true)
  {
    SYNC_ERR();
    gui.ShowCDScr();
    //read request
    auto request=commHelper.ReceiveRequest();
    LOG("Resync in progress");
    //check header
    if(request.reqType==ReqType::Invalid)
    {
      LOG("resync: invalid request received");
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
          LOG("resync: failed to send Ok answer to ResyncComplete request");
          send_resync();
          resyncState=0;
          continue;
        }
        //resync complete!
        LOG("Resync complete!");
        SYNC_OK();
        gui.ShowCEScr();
        return;
      }
      resyncState=0;
    }
    //final resync-sequence
    if(request.reqType!=ReqType::Resync || !commHelper.SendAnswer(AnsType::Resync,request.payload,request.plLen))
    {
      LOG("resync: incorrect request type or final resync-sequence send-failure");
      send_resync();
      continue;
    }
    resyncState=1;
  }
}

void conn_loop()
{
  LOG("Entering connection handling loop");
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
        LOG("Incorrect request type");
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
  //power-on RTC, wait for a while
  RTC_POWER_ON();
  //power-on display
  gui.Powersave(0);
}

void descend()
{
  //power-off RTC
  RTC_POWER_OFF();
  //TODO: power-off display
  gui.Powersave(1);
  //TODO: set MCU to sleep state
}

void update_menu()
{
  //TODO: reload menu items
}

void setup()
{
  //TODO: deactivate watchdog
  RTC_POWER_PREP();
  RTC_POWER_ON();
  SYNC_LED_PREP();
  SYNC_ERR();
  commHelper.Init(38400);
  RX_PIN_PREP(); // enable pullup on serial RX-pin
  gui.Init(DISPLAY_ADDR);
  wakeup();
  //TODO: install button interrupts
  update_menu();
  if(commHelper.DataAvailable())
  {
    conn_loop();
    //TODO: deactivate watchdog
    //TODO: reset button status
    update_menu();
  }
  gui.ResetToMainScr();
}

void loop()
{
  if(commHelper.DataAvailable())
  {
    conn_loop();
    //TODO: deactivate watchdog
    //TODO: reset button status
    update_menu();
    gui.ResetToMainScr();
  }
  //TODO: detect button event, perform action
  //TODO: calculate time after previous button event
  //TODO: after some idle time -> activate powersave mode (call descend)
}

