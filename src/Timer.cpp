#include "Timer.h"

void Timer::init(){
    lastset = millis();
}

void Timer::setInterval(int frequency){
    frequence = frequency;
}

bool Timer::isdone(){
    if(millis()<lastset) init();
    
    if((millis()-lastset) > frequence) {
        init();
        return true;
    }
    else return false;
}