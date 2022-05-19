#include <Arduino.h>

class Timer{
    public:
        void init();
        void setInterval(int frequency);
        bool isdone();
    private:
        unsigned long lastset;
        unsigned int frequence;
};