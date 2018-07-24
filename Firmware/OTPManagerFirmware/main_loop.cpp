#ifdef STANDALONE_APP

#include "standalone_config.h"
#include "arduino-defines.h"
#include "serial.h"

#define SYNC_OK() ({})
#define SYNC_ERR() ({})
#define SYNC_LED_PREP() ({})

#else

#include <Arduino.h>
#define LOG(...) ({})
#define SYNC_OK() ({digitalWrite(LED_SYNC, HIGH);})
#define SYNC_ERR() ({digitalWrite(LED_SYNC, LOW);})
#define SYNC_LED_PREP() ({pinMode(LED_SYNC, OUTPUT);;})

#endif

#include "configuration.h"
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
  //TODO: power-on RTC, wait for a while
  //TODO: power-on display
}

void descend()
{
  //TODO: power-off RTC
  //TODO: power-off display
  //TODO: set MCU to sleep state
}

void update_menu()
{
  //TODO: reload menu items
}

void setup()
{
  //TODO: deactivate watchdog
  SYNC_LED_PREP();
  SYNC_ERR();
  commHelper.Init(38400);
#ifdef SERIAL_RX_PIN
  //Enable pullup on serial rx-pin.
  //We need this in order to prevent false incoming connection events when device enabled and not connected to PC
  pinMode(SERIAL_RX_PIN,INPUT_PULLUP);
#endif
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
  //TODO: show main screen
}

void loop()
{
  if(commHelper.DataAvailable())
  {
    conn_loop();
    //TODO: deactivate watchdog
    //TODO: reset button status
    update_menu();
    //TODO: show main screen
  }
  //TODO: detect button event, perform action
  //TODO: calculate time after previous button event
  //TODO: after some idle time -> activate powersave mode (call descend)
}

