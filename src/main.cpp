#include <Arduino.h>
#include "VescUart.h"
#include "Timer.h"
#include "BluetoothHandler.h"
#include "VehicleControl.h"


//##############################################################################
//          VERSION: 0.92 eBIKE_TEST
//##############################################################################



#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif


VescUart UART;

BluetoothHandler BT;
VehicleControl VCU;

Timer DataTimer;
Timer ControlTimer;
Timer BtTimerNotifys;
Timer BtTimerReads;


void IRAM_ATTR isr(){
  VCU.PasTriggered();
}

void setup() {
  Serial.begin(115200);
  //Serial.println("ESP32 STARTED");

  BT.init();
  
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  while (!Serial2) {;}
  UART.setSerialPort(&Serial2);

  DataTimer.init();
  DataTimer.setInterval(50);

  ControlTimer.init();
  ControlTimer.setInterval(50);

  BtTimerNotifys.init();
  BtTimerNotifys.setInterval(50);

  BtTimerReads.init();
  BtTimerReads.setInterval(1000);

  pinMode(THROTTLEPIN,ANALOG);
  pinMode(BRAKEPIN,ANALOG);
  pinMode(PAS_PIN, INPUT_PULLUP);

  VCU.setLockdown(BT.Flash.getStats().lockdown);

  attachInterrupt(PAS_PIN, isr, FALLING);
}

void loop() {
    digitalWrite(2,1);

    if(DataTimer.isdone()){
      if ( UART.getVescValues() ){
        BT.CalculateData(UART);
      }
    }

    if(ControlTimer.isdone()){
      VCU.ControlVesc(UART);
    }

    if(BtTimerNotifys.isdone()){
      BT.SendNotifyBTData();
    }

    if(BtTimerReads.isdone()){
      BT.SetReadBTData();
      // VCU lockdown check only here because of low frequency
      if (VCU.getLockdown() != BT.Flash.getStats().lockdown){
        BT.Flash.setLockdown(VCU.getLockdown());
      }
    }

}