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

        void GetInputs(int &throttlein, int &brakein);

        void recognizeLock(int inp_throttle, int inp_brake);

        void ReadAnalogs();

    private:

        int throttle=0, brake=0;

        const float Kp = KP_PAS,
                    Ki = KI_PAS * ((float)T_CTRL/1000.0F),
                    Kd = KD_PAS * (1000.0F/(float)T_CTRL);

        float VehicleSpeed = 0.0F;

        bool lockdownmode = true;
        int lock_counter = 0;

        int last_throttle = THROTTLELOW;
        const float ratioPulseDistanceMeter = GEARRATIO * WHEELCIRCUM / ( (float)POLEPAIRS * (float)CORRECTION);

        unsigned long last_PAS;
        unsigned long PAS_times[PAS_SMOOTHING] = {};

        float I_PAS = 0.0;

};