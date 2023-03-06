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

int dir = 1;

void setup() {
  Serial.begin(115200);
  
  
  Serial.println("The device started, now you can pair it with bluetooth!");
  
  BT.init();
  
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  while (!Serial2) {;}
  UART.setSerialPort(&Serial2);

  DataTimer.init();
  DataTimer.setInterval(50);

  ControlTimer.init();
  ControlTimer.setInterval(50);

  BtTimerNotifys.init();
  BtTimerNotifys.setInterval(18);

  BtTimerReads.init();
  BtTimerReads.setInterval(1000);

  pinMode(THROTTLEPIN,ANALOG);
  pinMode(BRAKEPIN,ANALOG);

  #ifdef DEBUG
    UART.data.avgInputCurrent = 15.0F;
    UART.data.rpm = 1600.0F;
    UART.data.inpVoltage = 50.0F;
    UART.data.ampHours = 18.0F;
    UART.data.ampHoursCharged = 19.0F;
    UART.data.wattHours = 20.0F;
    UART.data.wattHoursCharged = 21.0F;
    UART.data.tachometer = 2200.0F;
    UART.data.tachometerAbs = 2300.0F;
    UART.data.tempMosfet = 24.0F;
    UART.data.tempMotor = 25.0F;

    pinMode(2, OUTPUT);
  #endif
}

void loop() {
    digitalWrite(2,1);

    if(DataTimer.isdone()){
      #ifndef DEBUG
      if ( UART.getVescValues() ){
        BT.CalculateData(UART, BT.SendData);
      }
      #else
      BT.CalculateData(UART);

      if (UART.data.inpVoltage > 50.00)
        dir = -1;
        if (UART.data.inpVoltage < 30.00)
        dir = 1;

        UART.data.inpVoltage += dir;

      if(Serial.available()){
        String tempinp = Serial.readStringUntil('\n');
        UART.data.wattHours = tempinp.toInt();

        Serial.println(BT.getBTValue(totalkm));
        Serial.println(BT.getBTValue(totalWh));
        Serial.println(BT.getBTValue(totalWhC));
        Serial.println(BT.getBTValue(batPercent));

        Serial.println();
      }
      #endif
    }

    if(ControlTimer.isdone()){
      VCU.ControlVesc(UART);
    }

    if(BtTimerNotifys.isdone()){
      BT.SendNotifyBTData();
    }
    if(BtTimerReads.isdone()){
      BT.SetReadBTData();
    }


    digitalWrite(2,0);

}