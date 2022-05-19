#include <Arduino.h>
#include "VescUart.h"
#include "VehicleData.h"

#include <FastLED.h>

//#include <Adafruit_NeoPixel.h>
//#include <HSBColor.h>

class VehicleControl{
    private:
        int last_throttle = THROTTLELOW;

        bool strip_enabled = true;
        bool strip_disabled = true;
        int mode = 0;
        bool frontbeam_en = false;
        int i=0;
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        int hue, j=0;
        int saturation=100, lighting=50, value;
        float pos;
        long last_tacho = 0;
        int ledpos;
        float wavelength = 0.5;
        float speed;

        const float ratioPulseDistanceMeter = GEARRATIO * WHEELCIRCUM / ( (float)POLEPAIRS * (float)CORRECTION);
        uint8_t rgb[3];

    public:
        void ControlVesc(VescUart &lVesc);
        void init_ledstrip();
        void refresh_ledstrip();
        void set_speed(long, float);
        void set_ledstrip(uint8_t* buffer);
    private:
        void rainbow_effect(float);
        void stripe_effect(float);
};