/*
General Idea:
Reset Battery in 2 cases: No visible current draw when full or empty

Save Part:
Save to EEPROM when Battery Voltage dips below possible range
-> make sure only possible once per Device cycle


*/

#include "BatteryModel.h"


// Gives Back % of Battery Charge
float BatteryModel::getSOC(float V, float A, float Wh, float WhC, float flashPercent){
    if (BatteryWasShutdown){
        BatteryWasShutdown = false;

        float guessPercent = PercentFromVolt(V);
        if (abs(guessPercent - flashPercent) > 10){
            SOC = guessPercent;
            return guessPercent;
        }
        else{
            SOC = flashPercent;
            return flashPercent;
            }
    }
    float factor = 1 + (A*A*BATTERY_IR)/(V*A);
    
    realBatWhDelta = -(Wh-LastWh)*factor + (WhC-LastWhC)*factor;
    LastWh = Wh;
    LastWhC = WhC;

    return (SOC - (realBatWhDelta / BATTERY_FULL_WH));
    }

void BatteryModel::calcIR(){

}

float BatteryModel::PercentFromVolt(float V){
    float Percent = map(V, BATTERY_V_EMPTY, BATTERY_V_FULL, 0, 100);
    if (Percent > 100.F) return 100.0F;
    if (Percent < 0.0F) return 0.0F;
    return Percent;
}