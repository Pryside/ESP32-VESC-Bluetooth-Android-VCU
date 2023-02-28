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

        Serial.print("bat was shutdown");
    Serial.print("flashpercent: ");
    Serial.println(flashPercent);

        BatteryWasShutdown = false;

        float guessPercent = PercentFromVolt(V);
        Serial.println(guessPercent);
        if (abs(guessPercent - flashPercent) > 10){
            Serial.println("TOOK A GUESS!");
            
            SOC_wh = (guessPercent/100.0F) * BATTERY_FULL_WH;
            return guessPercent;
        }
        else{
            
            SOC_wh = (flashPercent/100.0F) * BATTERY_FULL_WH;
            return flashPercent;
            }
    }
    
    //float factor = 1 + (A*A*BATTERY_IR)/(V*A);
    float factorout = 1.00F + (A*BATTERY_IR)/V;
    float factorin = 1.00F - (A*BATTERY_IR)/V;

    realBatWhDelta = -(Wh-LastWh)*factorout + (WhC-LastWhC)*factorin;
    SOC_wh += realBatWhDelta;

    if(abs(Wh-LastWh) > 0 || abs(WhC-LastWhC) > 0){
        Serial.println("battery update:");
        Serial.println(factorin);
        Serial.println(factorout);
        Serial.println((Wh-LastWh));
        Serial.println((WhC-LastWhC));
        Serial.println(SOC_wh / BATTERY_FULL_WH);
    }
    
    LastWh = Wh;
    LastWhC = WhC;

    
    
    return ((SOC_wh / BATTERY_FULL_WH)*100.0F);
    }

void BatteryModel::calcIR(){

}

float BatteryModel::PercentFromVolt(float V){
    float Percent = map(V, BATTERY_V_EMPTY, BATTERY_V_FULL, 0, 100);
    if (Percent > 100.F) return 100.0F;
    if (Percent < 0.0F) return 0.0F;
    return Percent;
}