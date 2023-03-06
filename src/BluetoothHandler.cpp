#include "BluetoothHandler.h"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      g_centralConnected = true;
      Serial.println("connected...");
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      g_centralConnected = false;
      Serial.println("disconnect...");
    }
};

class MyCharPrintingCallbacks: public BLECharacteristicCallbacks
{
public:
    explicit MyCharPrintingCallbacks(const char* name) : m_name(name) {}

private:
    void PrintEvent(const char* event, const char* value)
    {
        Serial.print(event);
        Serial.print("(");
        Serial.print(m_name.c_str());
        Serial.print(")");
        if (value)
        {
            Serial.print(" value='");
            Serial.print(value);
            Serial.print("'");
        }
        Serial.println();
    }

private:
    void onRead(BLECharacteristic* pCharacteristic) override
    {
        PrintEvent("onRead", pCharacteristic->getValue().c_str());
    }

    void onWrite(BLECharacteristic* pCharacteristic) override
    {
        PrintEvent("onWrite", pCharacteristic->getValue().c_str());
    }

    void onNotify(BLECharacteristic* pCharacteristic) override
    {
        PrintEvent("onNotify", pCharacteristic->getValue().c_str());
    }

    void onStatus(BLECharacteristic* pCharacteristic, Status status, uint32_t code) override
    {
        std::string event("onStatus:");
        switch (status)
        {
        case SUCCESS_INDICATE: event += "SUCCESS_INDICATE"; break;
        case SUCCESS_NOTIFY: event += "SUCCESS_NOTIFY"; break;
        case ERROR_INDICATE_DISABLED: event += "ERROR_INDICATE_DISABLED"; break;
        case ERROR_NOTIFY_DISABLED: event += "ERROR_NOTIFY_DISABLED"; break;
        case ERROR_GATT: event += "ERROR_GATT"; break;
        case ERROR_NO_CLIENT: event += "ERROR_NO_CLIENT"; break;
        case ERROR_INDICATE_TIMEOUT: event += "ERROR_INDICATE_TIMEOUT"; break;
        case ERROR_INDICATE_FAILURE: event += "ERROR_INDICATE_FAILURE"; break;
        }
        event += ":";
        event += String(code).c_str();
        PrintEvent(event.c_str(), nullptr);
    }

private:
    std::string m_name;
};




void BluetoothHandler::CalculateData(VescUart getUart){
    
  SendData[kmh] = (float)(getUart.data.rpm) * ratioSpeedKmh;
  SendData[km] = (float)getUart.data.tachometerAbs*ratioPulseDistanceKm;

  //old way of calcuating wh
  //ldata.wattHours += (getUart.data.ampHours-lastAmpHours)*((getUart.data.inpVoltage-lastInputVoltage)*0.5 + lastInputVoltage);
  //ldata.wattHoursC += (getUart.data.ampHoursCharged-lastAmpHoursCharged)*((getUart.data.inpVoltage-lastInputVoltage)*0.5 + lastInputVoltage);

  SendData[wattHours] = getUart.data.wattHours;
  SendData[wattHoursC] = getUart.data.wattHoursCharged;

  lastAmpHours = getUart.data.ampHours;
  lastAmpHoursCharged = getUart.data.ampHoursCharged;
  lastInputVoltage = getUart.data.inpVoltage;

  SendData[ampHours]  = getUart.data.ampHours;
  SendData[ampHoursC] = getUart.data.ampHoursCharged;
  SendData[batCurr]   = getUart.data.avgInputCurrent;
  SendData[inpVolt]   = getUart.data.inpVoltage;
  SendData[motorCurr] = getUart.data.avgMotorCurrent;
  SendData[tempMotor] = getUart.data.tempMotor;

  SendData[batPercent] = BatModel.getSOC(SendData[inpVolt], SendData[batCurr], SendData[wattHours],
                                     SendData[wattHoursC], Flash.getStats().BatteryPercent);


  
  Flash.setStats(SendData[batPercent],SendData[km], SendData[wattHours], SendData[wattHoursC]);
  SendData[totalWh] = Flash.getStats().EnergyWh;
  SendData[totalkm] = Flash.getStats().DistanceKm;
  SendData[totalWhC] = Flash.getStats().EnergyWhC;

  Flash.SaveOnShutdown(SendData[inpVolt], SHUTDOWN_THESH_V);
}


void BluetoothHandler::SendNotifyBTData(){
    if(g_centralConnected){
        if (notify_counter >= BT_NOTIFYS) notify_counter = 0;
        
        g_pCharNotify[notify_counter]->setValue(SendData[notify_counter]);
        g_pCharNotify[notify_counter]->notify();

        notify_counter++;
    }

}


void BluetoothHandler::SetReadBTData(){
    if(g_centralConnected){
        if (read_counter >= BT_READS+BT_NOTIFYS) read_counter = 0+BT_NOTIFYS;
        g_pCharRead[read_counter]->setValue(SendData[read_counter]);
        read_counter++;
    }

}



bool BluetoothHandler::GetBTData(uint8_t *buffer){
  return 0;
}

float BluetoothHandler::getBTValue(varnames name){
    return SendData[name];
}


void BluetoothHandler::init(){
    
    lastAmpHours = 0;
    lastInputVoltage = 0;
    lastAmpHoursCharged = 0;

    BLEDevice::init("ESP32");
    g_pServer = BLEDevice::createServer();
    g_pServer->setCallbacks(new MyServerCallbacks());
    
    BLEService* pService = g_pServer->createService(BLEUUID(SERVICE_UUID),50,0);
    //BLEService* pServiceRead = g_pServer->createService((BLEUUID)SERVICE_READ_UUID,BT_VARS,0);

    //Create all notifys
        for(int u = 0; u < BT_NOTIFYS; u++){
            
            uint32_t propertyFlags = BLECharacteristic::PROPERTY_NOTIFY;
            char tempUUID[37] = {  };
            //char tempName[20] = {  };
            Notify_UUIDs[u][0].toCharArray(tempUUID, 37);
            //Notify_UUIDs[u][1].toCharArray(tempName, sizeof(Notify_UUIDs[u][1]));
            //g_pCharNotifyDesc[u] = new BLE2902();
            //g_pCharNotifyDesc[u]->setValue(tempName);
            g_pCharNotify[u] = pService->createCharacteristic(tempUUID,propertyFlags);
            //g_pCharNotify[u]->addDescriptor(g_pCharNotifyDesc[u]);
            g_pCharNotify[u]->addDescriptor(new BLE2902());

            delay(10);

        }

    //Create all reads
    for(int u = 0; u < BT_READS; u++){
            
            uint32_t propertyFlags = BLECharacteristic::PROPERTY_READ;
            char tempUUID[37] = {  };
            //char tempName[20] = {  };
            Read_UUIDs[u][0].toCharArray(tempUUID, 37);
            //Read_UUIDs[u][1].toCharArray(tempName, sizeof(Read_UUIDs[u][1]));
            g_pCharRead[u] = pService->createCharacteristic(tempUUID,propertyFlags);
            g_pCharRead[u]->addDescriptor(new BLE2902());

            delay(10);
        }


    //pServiceRead->start();
    pService->start();
    delay(50);
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    //pAdvertising->addServiceUUID(SERVICE_READ_UUID);
    pAdvertising->setScanResponse(true);
    // fix for later iphone connectons
    // {
    //     pAdvertising->setMinPreferred(0x06);
    //     pAdvertising->setMinPreferred(0x12);
    // }
    pAdvertising->setMinPreferred(0x0);
    BLEDevice::startAdvertising();
    delay(50);

    Flash.init();
}



