#include "BluetoothSerial.h"
#include <Arduino.h>
#include "VescUart.h"
#include "VehicleData.h"



class BluetoothHandler{

private:

    BluetoothSerial SerialBT;
    const uint8_t endof_r[ENDOF_RECIEVE_LEN] = {'s','e','t'};
    const float ratioPulseDistanceKm = GEARRATIO * WHEELCIRCUM / ( (float)POLEPAIRS * (float)CORRECTION)/1000.0;
    const float ratioSpeedKmh = 60 / ((float)(POLEPAIRS)*1000.0) * GEARRATIO * WHEELCIRCUM;

    union memory
    {
        float input;
        uint8_t bytes[3];
    };
    union memory splitbytes;

public:

    void IntoBytes(float inp, uint8_t* output, uint8_t index);

    struct BTdata
    {
    float tempMotor, motorCurr, batCurr, kmh, inpVolt, ampHours, ampHoursC, km, wattHours, wattHoursC, totalWh, totalWhC, totalkm;
    };

    BTdata SendData;

    void ConvertToData(float* dataIn, uint8_t* btdata);

    float lastAmpHours, lastInputVoltage, lastAmpHoursCharged;

    void CalculateData(VescUart getUart, BTdata &ldata);

    void FitToArray(BTdata lbtdata, float* ldata);

    void SendBTData();

    void init();

    bool GetBTData(uint8_t *buffer);


};
