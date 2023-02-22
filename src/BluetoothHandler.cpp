#include "BluetoothHandler.h"


//#include "BatteryModel.h" not used yet


void BluetoothHandler::IntoBytes(float inp, uint8_t* output, uint8_t index) {
    splitbytes.input = inp;
    for (int i = 0; i < 4; i++)
        output[index + i] = splitbytes.bytes[i];
}

void BluetoothHandler::ConvertToData(float* dataIn, uint8_t* btdata) {
    for (int i = 0; i < 13; i++)
    {
        IntoBytes((dataIn[i]), btdata, 1 + i * 5);
        btdata[i * 5] = 0xFF;
    }
    char a[] = "ENDOF\n";
    for (int u = 0; u < 6; u++) {
        btdata[65 + u] = a[u];
    }
}



void BluetoothHandler::CalculateData(VescUart getUart, BTdata &ldata){
  ldata.kmh = (float)(getUart.data.rpm) * ratioSpeedKmh;
  ldata.km = (float)getUart.data.tachometerAbs*ratioPulseDistanceKm;

  ldata.wattHours += (getUart.data.ampHours-lastAmpHours)*((getUart.data.inpVoltage-lastInputVoltage)*0.5 + lastInputVoltage);
  ldata.wattHoursC += (getUart.data.ampHoursCharged-lastAmpHoursCharged)*((getUart.data.inpVoltage-lastInputVoltage)*0.5 + lastInputVoltage);

  lastAmpHours = getUart.data.ampHours;
  lastAmpHoursCharged = getUart.data.ampHoursCharged;
  lastInputVoltage = getUart.data.inpVoltage;

  ldata.ampHours  = getUart.data.ampHours;
  ldata.ampHoursC = getUart.data.ampHoursCharged;
  ldata.batCurr   = getUart.data.avgInputCurrent;
  ldata.inpVolt   = getUart.data.inpVoltage;
  ldata.motorCurr = getUart.data.avgMotorCurrent;
  ldata.tempMotor = getUart.data.tempMotor;

  Flash.setStats(0,ldata.km, ldata.wattHours, ldata.wattHoursC);
  ldata.totalWh = Flash.getStats().EnergyWh;
  ldata.totalkm = Flash.getStats().DistanceKm;
  ldata.totalWhC = Flash.getStats().EnergyWhC;

  Flash.SaveOnShutdown(ldata.inpVolt, SHUTDOWN_THESH_V);
}

void BluetoothHandler::FitToArray(BTdata lbtdata, float* ldata){
  ldata[0]  = lbtdata.tempMotor;
  ldata[1]  = lbtdata.motorCurr;
  ldata[2]  = lbtdata.batCurr;
  ldata[3]  = lbtdata.kmh;
  ldata[4]  = lbtdata.inpVolt;
  ldata[5]  = lbtdata.ampHours;
  ldata[6]  = lbtdata.ampHoursC;
  ldata[7]  = lbtdata.km;
  ldata[8]  = lbtdata.wattHours;
  ldata[9]  = lbtdata.wattHoursC;
  ldata[10] = lbtdata.totalWh;
  ldata[11] = lbtdata.totalWhC;
  ldata[12] = lbtdata.totalkm;
  
}

void BluetoothHandler::SendBTData(){
  uint8_t btdata[71] = {};
  float data[13] = {};
  FitToArray(SendData, data);
  ConvertToData(data,btdata);
  if (SerialBT.connected())
  SerialBT.write(btdata,sizeof(btdata));
}

bool BluetoothHandler::GetBTData(uint8_t *buffer){
  if(SerialBT.connected() && SerialBT.available())
  {
  //Serial.write(SerialBT.read());
  
  if(SerialBT.readBytesUntil('\n',buffer,SIZEOF_BT_RECIEVE)>0){
    bool found = true;
    const int start = SIZEOF_BT_RECIEVE-ENDOF_RECIEVE_LEN;
    for(int i = 0; i < ENDOF_RECIEVE_LEN; i++ ){
      if(buffer[start+i] != endof_r[i])found = false;
    }
    if (found){
      //Serial.println("found data");
      //correct data found, trigger the set function
      return true;
    }
  }
  }
  return false;
}


void BluetoothHandler::init(){
    SerialBT.begin(BLUETOOTH_NAME); //Bluetooth device name
    lastAmpHours = 0;
    lastInputVoltage = 0;
    lastAmpHoursCharged = 0;

    Flash.init();
}