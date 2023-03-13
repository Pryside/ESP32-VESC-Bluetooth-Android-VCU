#include <Arduino.h>
#include "VescUart.h"
#include "VehicleData.h"


class VehicleControl{
    public:
        void ControlVesc(VescUart &lVesc);

        void setLockdown(bool lockme);
        bool getLockdown();

        void PasTriggered();

        int getavgPASspeed();
        int getPASoutput();

        void giveSpeed(float speed);


        void recognizeLock(int inp_throttle, int inp_brake);

    private:

        float VehicleSpeed = 0.0F;

        bool lockdownmode = true;
        int lock_counter = 0;

        int last_throttle = THROTTLELOW;
        const float ratioPulseDistanceMeter = GEARRATIO * WHEELCIRCUM / ( (float)POLEPAIRS * (float)CORRECTION);

        unsigned long last_PAS;
        unsigned long PAS_times[PAS_SMOOTHING] = {0,0,0,0};

        float I_PAS = 0;

};