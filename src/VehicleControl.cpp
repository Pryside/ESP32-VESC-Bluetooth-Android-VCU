#include "VehicleControl.h"

//Multisampling Function to get more Accurate Analog Read (also use 0.1uF Cap)
void VehicleControl::ReadAnalogs(){
  static int brake_vals[IN_MSPL] = {0,0,0,0,0};
  static int throttle_vals[IN_MSPL] = {0,0,0,0,0};
  for(int u = 1; u < IN_MSPL; u++){
    throttle_vals[u-1] = throttle_vals [u];
    brake_vals[u-1] = brake_vals [u];
  }
  throttle_vals[IN_MSPL-1] = analogRead(THROTTLEPIN);
  brake_vals[IN_MSPL-1] = analogRead(BRAKEPIN);

  int temp_throttle=0, temp_brake=0;
  //calculate avg
  for(int u = 0; u < IN_MSPL; u++){
    temp_throttle += throttle_vals[u];
    temp_brake += brake_vals[u];
  }
  throttle = temp_throttle/IN_MSPL;
  brake = temp_brake/IN_MSPL;
}


//Interprets inputs and sets vesc parameters
void VehicleControl::ControlVesc(VescUart &lVesc){
  //get all needed input data
  int output_PAS = getPASoutput();
  int input_throttle = throttle;
  int input_brake = brake;
  int output = 127;


  recognizeLock(input_throttle, input_brake);

  //  //for initializing and testing the inputs
  //  Serial.print("PAS: ");
  //  Serial.print(output_PAS);
  //  Serial.print("throttleIn: ");
  //  Serial.print(input_throttle);
  //   Serial.print("brakeIn: ");
  //  Serial.print(input_brake);

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
    if (output_throttle == 127){
      output_throttle = output_PAS;
    }
  }

  if(input_brake > BRAKELOW){
     output = output_brake;
     I_PAS = 0;
  }
  else output = output_throttle;

  //  Serial.print("output: ");
  //   Serial.println(output);

  lVesc.nunchuck.lowerButton = 0;
  lVesc.nunchuck.upperButton = 0;
  lVesc.nunchuck.valueX = 0;
  lVesc.nunchuck.valueY = output;
  lVesc.setNunchuckValues();

}

//Counts when PAS Sensor gets a Signal
void VehicleControl::PasTriggered(){
  //debouce
  unsigned long thistime = millis();
  unsigned long diff = thistime - last_PAS;
  //debouce should be 10-20ms (use small 100pf cap & 4.7k extra pullup)
  if (diff > 10){
    for (int i = 1; i<PAS_SMOOTHING; i++){
      PAS_times[i-1] = PAS_times[i];
    }
    PAS_times[PAS_SMOOTHING-1] = thistime;
  }
  last_PAS = thistime;
}

//Gives back the current Pedal Cadence
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
      int avg_time = ( (PAS_times[PAS_SMOOTHING-1] - PAS_times[0]) / (PAS_SMOOTHING-1)  );
      //convert avg time to cadence (rpm)
      // 60s * 1000ms / Time ms * 6(Magnets per rotiation)
      if (avg_time == 0) return 0;
      return ( (60 * 1000) / (avg_time * PAS_MAGNETS) );
    }
  }
}

//Gives back PID output the Control the VESC
int VehicleControl::getPASoutput(){
  int cadence = getavgPASspeed();
  // Serial.println(cadence);
  // Serial.println();
  
  int pas_maxout = PAS_MAXOUTPUT;
  int pas_minout = PAS_MINOUTPUT;

  if(lockdownmode){
    pas_maxout = PAS_MAXOUTPUTLOCKED;
    pas_minout = PAS_MINOUTPUT;
    if (VehicleSpeed > MAX_SPEED_LOCK){
        pas_maxout = map(VehicleSpeed, MAX_SPEED_LOCK, MAX_SPEED_LOCK_END, PAS_MAXOUTPUTLOCKED, PAS_NOOUTPUT);
      }
  }
  
    //only when there is cadence
    if (cadence > 1){
      static float last_error = 0.0;
      int output = 0;

      float error = PAS_SETSPEED - cadence;
      float P_PAS = error * Kp;
            I_PAS += error * Ki;
      float D_PAS = (error - last_error) * Kd;
      last_error = error;

      //Serial.println(cadence);
      //Serial.print(P_PAS);
      //Serial.print(" ");
      //Serial.println(I_PAS);

      output = P_PAS + I_PAS + D_PAS + PAS_NOOUTPUT;

      if (output > pas_maxout){
        I_PAS -= error * Ki;
        output = pas_maxout;
      }
      else if (output < pas_minout){
        I_PAS -= error * Ki;
        output = pas_minout;
      }
      return output;
    }
    else{
      I_PAS = 0;
      return PAS_NOOUTPUT;
    }

}

//Recognizes Low Speed Mode Code
void VehicleControl::recognizeLock(int inp_throttle, int inp_brake){
  if (inp_brake >= BRAKEHIGH){
    if(inp_throttle >= THROTTLEHIGH && lock_counter % 2 == 0){
      lock_counter++;
    }
    else if (inp_throttle <= THROTTLELOW && lock_counter % 2 != 0){
      lock_counter++;
      Serial.println(lock_counter);
    }

    if (!lockdownmode && lock_counter >= LOCKDOWN_EN){
      lockdownmode = true;
      lock_counter = 0;
      //Serial.println("lockdown on");
    }
    else if (lockdownmode && lock_counter >= LOCKDOWN_DIS){
      lockdownmode = false;
      lock_counter = 0;
      //Serial.println("lockdown off");
    }
  }
  else lock_counter = 0;
}


//Set Low Speed Mode - only use when starting
void VehicleControl::setLockdown(bool lockme){
  lockdownmode = lockme;
}
bool VehicleControl::getLockdown(){
  return lockdownmode;
}

void VehicleControl::giveSpeed(float speed){
  VehicleSpeed = speed;
}