#include "FlashSave.h"
#define DEBUG


void FlashSave::init(){ //get saved data and validate
    //EEPROM.begin(EEPROM_SIZE);
    prefs.begin(PREFERENCE_NAME);

    stats ReadStats;
    prefs.getBytes(PREFERENCE_NAME, &ReadStats, sizeof(ReadStats));
    int tmp_checksum = createChecksum(ReadStats);

    if (tmp_checksum == ReadStats.checksum){ //when checksum fits
        initialStats = ReadStats;
        currentStats = ReadStats;
    }
    else{ //else resetting the flash
        currentStats.checksum = createChecksum(currentStats);
        prefs.putBytes(PREFERENCE_NAME, &currentStats, sizeof(currentStats));
    }

    
}

int FlashSave::createChecksum(stats ValidateStats){
    return (int)(ValidateStats.BatteryPercent + ValidateStats.DistanceKm
    + ValidateStats.EnergyWh + ValidateStats.EnergyWhC);
}

stats FlashSave::getStats(){
    return currentStats;
}
void FlashSave::setLockdown(bool lockit){
    currentStats.lockdown = lockit;
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
        prefs.putBytes(PREFERENCE_NAME, &currentStats, sizeof(currentStats));
        StatsSaved = true;
    }
}