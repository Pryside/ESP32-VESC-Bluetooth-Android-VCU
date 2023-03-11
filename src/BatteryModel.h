#include <Arduino.h>
#include "VehicleData.h"

class BatteryModel{
    public:
        float getSOC(float V, float A, float Wh, float WhC, float flashPercent);
        //void calcSOC();
        void calcIR();
    private:
        float SOC_wh = 0;
        float IR = BATTERY_IR;
        float LastWh = 0;
        float LastWhC = 0;
        float realBatWhDelta = 0;

        bool BatteryWasShutdown = true;

        float PercentFromVolt(float V);
};