#include <Arduino.h>
#include "VescUart.h"
#include "VehicleData.h"


class VehicleControl{
    public:
        void ControlVesc(VescUart &lVesc);
    private:
        int last_throttle = THROTTLELOW;
        const float ratioPulseDistanceMeter = GEARRATIO * WHEELCIRCUM / ( (float)POLEPAIRS * (float)CORRECTION);

    
};