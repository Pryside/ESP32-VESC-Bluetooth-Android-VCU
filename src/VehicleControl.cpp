#include "VehicleControl.h"


void VehicleControl::ControlVesc(VescUart &lVesc){
  //get all needed input data
  int output_PAS = getPASoutput();
  int input_throttle = analogRead(THROTTLEPIN);
  int input_brake = analogRead(BRAKEPIN);
  int output = 127;

  recognizeLock(input_throttle, input_brake);

  /* //for initializing and testing the inputs
  Serial.println(output_PAS + "PAS, " + input_throttle + "throttle, "
  + input_brake + "brake. Inputs");*/

  //limit instant throttle input
  if(input_throttle > (last_throttle+THROTTLERATE)){
    input_throttle = last_throttle + THROTTLERATE;
  }
  last_throttle = input_throttle;
  
  //limit input throttle and detect possible errors
  if(input_throttle > THROTTLEHIGH) input_throttle = THROTTLEHIGH;
  if(input_throttle > THROTTLEHIGH_ERROR) input_throttle = THROTTLELOW;
  if(input_throttle < THROTTLELOW) input_throttle = THROTTLELOW;
  int output_throttle = map(input_throttle, THROTTLELOW, THROTTLEHIGH, 127, 254);
  
    //brake input limit and map
  if(input_brake < BRAKELOW) input_brake = BRAKELOW;
  if(input_brake > BRAKEHIGH) input_brake = BRAKEHIGH;
  int output_brake = map(input_brake, BRAKELOW, BRAKEHIGH, 127, 0);


  // integrate PAS
  //check if lockdown is enabled
  if (lockdownmode){
    output_throttle = output_PAS;
  }
  else{
    //in full power only use PAS when no throttle is applied
    if (output_throttle = 127){
      output_throttle = output_PAS;
    }
  }


  if(input_brake > BRAKELOW) output = output_brake;  //double check this
  //this was botched to fit the escooter throttle!!
  else output = output_throttle;

  lVesc.nunchuck.lowerButton = 0;
  lVesc.nunchuck.upperButton = 0;
  lVesc.nunchuck.valueX = 0;
  lVesc.nunchuck.valueY = output;
  lVesc.setNunchuckValues();

}

void VehicleControl::PasTriggered(){
  //debouce
  unsigned long diff = millis() - last_PAS;
  if (millis() - last_PAS > 50){
    for (int i = 0; i<(PAS_SMOOTHING-1); i++){
      PAS_times[i] = PAS_times[i+1];
    }
    PAS_times[PAS_SMOOTHING-1] = millis();
  }
  last_PAS = millis();
}

int VehicleControl::getavgPASspeed(){
  //check if we are not in motion
  if (millis() - PAS_times[PAS_SMOOTHING-1] > PAS_MAXTIME){
    return 0;
  }
  else{
    //when in motion, check if we are not too slow
    if( (PAS_times[PAS_SMOOTHING-1] - PAS_times[0]) > (PAS_MAXTIME*(PAS_SMOOTHING-1))  ){
      return 0;
    }
    //if we are in motion, and fast enough
    //get the avg time between intervals
    //and convert to cadence and return
    else{
      int avg_time = ( (PAS_times[PAS_SMOOTHING-1] - PAS_times[0]) /
                    (PAS_MAXTIME*(PAS_SMOOTHING-1))  );
      //convert avg time to cadence (rpm)
      // 60s * 1000ms / Time ms * 6(Magnets per rotiation)
      return ( (60 * 1000) / (avg_time * 6) );
    }
  }
}

int VehicleControl::getPASoutput(){
  int cadence = getavgPASspeed();

  //when in full power mode
  if(!lockdownmode){
    //only when there is cadence
    if (cadence > 1){
      int output = 0;
      float error = PAS_SETSPEED - cadence;
      float P_PAS = error * KP_PAS;
            I_PAS += error * KI_PAS;
      if (I_PAS > I_PAS_MAX){
        I_PAS = I_PAS_MAX;
      }
      else if (I_PAS < -I_PAS_MAX){
        I_PAS = -I_PAS_MAX;
      }

      output = P_PAS = I_PAS;

      if (output > PAS_MAXOUTPUT){
        output = PAS_MAXOUTPUT;
      }
      else if (output < PAS_MINOUTPUT){
        output = PAS_MINOUTPUT;
      }
      return output;
    }
    else{
      return PAS_NOOUTPUT;
    }
  }
  //when in lockdown mode
  else{
    //only when there is cadence
    if (cadence > 1){
      int output = 0;
      float error = PAS_SETSPEED - cadence;
      ///SPEED DIFF EINFÜGEN!!!
      if (VehicleSpeed > MAX_SPEED_LOCK){
        return PAS_NOOUTPUT;
      }

      float P_PAS = error * KP_PAS;
            I_PAS += error * KI_PAS;
      if (I_PAS > I_PAS_MAX){
        I_PAS = I_PAS_MAX;
      }
      else if (I_PAS < -I_PAS_MAX){
        I_PAS = -I_PAS_MAX;
      }

      output = P_PAS = I_PAS;

      if (output > PAS_MAXOUTPUT){
        output = PAS_MAXOUTPUT;
      }
      else if (output < PAS_MINOUTPUT){
        output = PAS_MINOUTPUT;
      }
      return output;
    }
    else{
      return PAS_NOOUTPUT;
    }
  }

}


void VehicleControl::recognizeLock(int inp_throttle, int inp_brake){
  if (inp_brake >= BRAKEHIGH){
    if(inp_throttle >= THROTTLEHIGH && lock_counter % 2 == 0){
      lock_counter++;
    }
    else if (inp_throttle >= THROTTLEHIGH && lock_counter % 2 != 0){
      lock_counter++;
    }

    if (!lockdownmode && lock_counter >= LOCKDOWN_ENA){
      lockdownmode = true;
    }
    else if (lockdownmode && lock_counter >= LOCKDOWN_DIS){
      lockdownmode = false;
    }
  }
  else lock_counter = 0;
}



void VehicleControl::setLockdown(bool lockme){
  lockdownmode = lockme;
}
bool VehicleControl::getLockdown(){
  return lockdownmode;
}

void VehicleControl::giveSpeed(float speed){
  VehicleSpeed = speed;
}