#include <EEPROM.h>
#define EEPROM_SIZE 50
#define EEPROM_STAT_ADRR 0

struct stats
        {
            float BatteryPercent;
            float DistanceKm;
            float EnergyWh;
            float EnergyWhC;
            int checksum;
        };

class FlashSave{
    public:
        void init();
        void SaveOnShutdown(float voltage, float voltage_threshhold);
        stats getStats();
        void setStats(float percent, float km, float wh, float whc);
    
    private:
        stats initialStats = {0, 0, 0, 0, 0};
        stats currentStats = {0, 0, 0, 0, 0};

        bool StatsSaved = false;
        int createChecksum(stats validateStats);
};