#include <Preferences.h>
#define PREFERENCE_NAME "stats"

struct stats
        {
            float BatteryPercent = 0.0F;
            float DistanceKm = 0.0F;
            float EnergyWh = 0.0F;
            float EnergyWhC = 0.0F;
            int checksum = 0;
            bool lockdown = true;
        };

class FlashSave{
    
    
    public:
        void init();
        void SaveOnShutdown(float voltage, float voltage_threshhold);
        stats getStats();
        void setStats(float percent, float km, float wh, float whc);

        bool getLockdown();
        void setLockdown(bool lockit);
    
    private:
        Preferences prefs;

        stats initialStats;
        stats currentStats;

        bool StatsSaved = false;
        int createChecksum(stats validateStats);
};