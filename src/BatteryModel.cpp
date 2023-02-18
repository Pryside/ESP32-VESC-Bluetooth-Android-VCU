/*
General Idea:
Reset Battery in 2 cases: No visible current draw when full or empty

Save Part:
Save to EEPROM when Battery Voltage dips below possible range
-> make sure only possible once per Device cycle


*/

#include "BatteryModel.h"


// Gives Back % of Battery Charge
float BatteryModel::getSOC(){ return SOC; }

void BatteryModel::calcIR(){
    //Start timer when Battery Current draw is low

    //when current is 0 for time, set 0A_Voltage

    //Start 2nd timer when Battery Current has value
    //restart timer when IR measurement is crooked or 0A

    //when timer is done, set new IR
}
