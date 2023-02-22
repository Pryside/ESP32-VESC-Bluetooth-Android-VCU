#include "FlashSave.h"

void FlashSave::init(){ //get saved data and validate
    EEPROM.begin(EEPROM_SIZE);

    stats ReadStats = {0,0,0,0, 0};
    EEPROM.get(EEPROM_STAT_ADRR, ReadStats);
    if (createChecksum(ReadStats) == ReadStats.checksum){
        initialStats = ReadStats;
        currentStats = ReadStats;
    }
}

int FlashSave::createChecksum(stats ValidateStats){
    return (int)(ValidateStats.BatteryPercent + ValidateStats.DistanceKm
    + ValidateStats.EnergyWh + ValidateStats.EnergyWhC);
}

stats FlashSave::getStats(){
    return currentStats;
}

void FlashSave::setStats(float percent, float km, float wh, float whc){
    currentStats.BatteryPercent = initialStats.BatteryPercent + percent;
    currentStats.DistanceKm = initialStats.DistanceKm + km;
    currentStats.EnergyWh = initialStats.EnergyWh + wh;
    currentStats.EnergyWhC = initialStats.EnergyWhC + whc;
}


void FlashSave::SaveOnShutdown(float voltage, float voltage_threshhold){
    if (voltage < voltage_threshhold && StatsSaved == false){
        currentStats.checksum = createChecksum(currentStats);
        EEPROM.put(EEPROM_STAT_ADRR, currentStats);
        StatsSaved = true;
    }
}