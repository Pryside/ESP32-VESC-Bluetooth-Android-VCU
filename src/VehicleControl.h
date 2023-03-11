#include <Arduino.h>
#include "VescUart.h"
#include "VehicleData.h"


class VehicleControl{
    public:
        void ControlVesc(VescUart &lVesc);

        void setLockdown(bool lockme);
        bool getLockdown();

        void PasTriggered();

        int getAvgPASspeed(); // BUILD AVG OF PAS SPEEDS


    private:

        bool lockdownmode = true;

        int last_throttle = THROTTLELOW;
        const float ratioPulseDistanceMeter = GEARRATIO * WHEELCIRCUM / ( (float)POLEPAIRS * (float)CORRECTION);

        unsigned long last_PAS;
        int time_between[PAS_SMOOTHING];

};