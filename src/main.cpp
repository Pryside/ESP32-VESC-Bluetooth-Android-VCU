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
Timer BluetoothTimer;

void setup() {
  Serial.begin(115200);
  BT.init();
  
  Serial.println("The device started, now you can pair it with bluetooth!");
  
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  while (!Serial2) {;}
  UART.setSerialPort(&Serial2);

  DataTimer.init();
  DataTimer.setInterval(50);

  ControlTimer.init();
  ControlTimer.setInterval(50);

  BluetoothTimer.init();
  BluetoothTimer.setInterval(50);

  pinMode(THROTTLEPIN,ANALOG);
  pinMode(BRAKEPIN,ANALOG);
}

void loop() {
    if(DataTimer.isdone()){
      UART.getVescValues();
      BT.CalculateData(UART, BT.SendData);
    }

    if(ControlTimer.isdone()){
      VCU.ControlVesc(UART);
    }

    if(BluetoothTimer.isdone()){
      BT.SendBTData();
      uint8_t buffer[SIZEOF_BT_RECIEVE] = {};
    }
}