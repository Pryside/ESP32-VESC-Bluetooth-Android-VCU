#include <Arduino.h>
#include "VehicleData.h"
//#include "VescUart.h"

#define IR_MEASUREMENT_T 100.0F //the time for a stable IR measurement
#define IR_MEASUREMENT_D 0.1F //the % of jitter allowed for a measurement

class BatteryModel{
    public:
        float getSOC();
        void calcSOC();
        void calcIR();
    private:
        float SOC = 0;
        float IR = 0;
        boolean BatteryWasShutdown = false;



};