#include "VehicleControl.h"


void VehicleControl::ControlVesc(VescUart &lVesc){

  int input_throttle = analogRead(THROTTLEPIN);
  int input_brake = analogRead(BRAKEPIN);
  int output = 127;


  if(input_throttle > (last_throttle+THROTTLERATE)){
    input_throttle = last_throttle + THROTTLERATE;
  }
  last_throttle = input_throttle;
  

  if(input_throttle > THROTTLEHIGH) input_throttle = THROTTLEHIGH;
  if(input_throttle < THROTTLELOW) input_throttle = THROTTLELOW;
  int output_throttle = map(input_throttle, THROTTLELOW, THROTTLEHIGH, 127, 254);
  
  if(input_brake < BRAKELOW) input_brake = BRAKELOW;
  if(input_brake > BRAKEHIGH) input_brake = BRAKEHIGH;
  int output_brake = map(input_brake, BRAKELOW, BRAKEHIGH, 127, 0);

  if(input_brake < BRAKEHIGH) output = output_brake;
  else output = output_throttle;

  //Serial.println(UART.nunchuck.valueY);
  lVesc.nunchuck.lowerButton = 0;
  lVesc.nunchuck.upperButton = 0;
  lVesc.nunchuck.valueX = 0;
  lVesc.nunchuck.valueY = output;
  lVesc.setNunchuckValues();

}

void VehicleControl::PasTriggered(){
  //debouce
  unsigned long diff = millis() - last_PAS;
  if (diff > 5 && diff < 1000){
    for (int i = 0; i<(PAS_SMOOTHING-1); i++){
      time_between[i] = time_between[i+1];
    }
    time_between[PAS_SMOOTHING] = diff;
  }
  last_PAS = millis();
}



void VehicleControl::setLockdown(bool lockme){
  lockdownmode = lockme;
}
bool VehicleControl::getLockdown(){
  return lockdownmode;
}
