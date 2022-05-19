#include "VehicleControl.h"

//Adafruit_NeoPixel pixels(LEDSTRIP_SIZE, LEDSTRIP_PIN, NEO_GRB + NEO_KHZ800);
CRGB leds[LEDSTRIP_SIZE];

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

void VehicleControl::init_ledstrip()
{
  LEDS.addLeds<WS2812, LEDSTRIP_PIN, RGB>(leds, LEDSTRIP_SIZE);
  LEDS.setBrightness(84);
  // pixels.begin();
}

void fadeall()
{
  for (int i = 0; i < LEDSTRIP_SIZE; i++)
  {
    leds[i].nscale8(250);
  }
}

void rainbow_wave(uint8_t thisSpeed, uint8_t deltaHue)
{ // The fill_rainbow call doesn't support brightness levels.

  // uint8_t thisHue = beatsin8(thisSpeed,0,255);                // A simple rainbow wave.
  uint8_t thisHue = beat8(thisSpeed, 255); // A simple rainbow march.

  fill_rainbow(leds, LEDSTRIP_SIZE, thisHue, deltaHue); // Use FastLED's fill_rainbow routine.

} // rainbow_wave()

void VehicleControl::set_speed(long tacho, float lspeed)
{
  speed = lspeed;
  long thistacho = (tacho - last_tacho)/2;
  pos = (float)(thistacho) * -ratioPulseDistanceMeter;
  //Serial.println(tacho);
  ledpos = pos * LEDSTRIP_SIZE * wavelength / LEDSTRIP_LEN;
  last_tacho = tacho-thistacho;
}

void VehicleControl::refresh_ledstrip()
{
  if (strip_enabled)
  {
    switch (mode)
    {
    case 1:
      wavelength = 0.5;
      rainbow_effect(wavelength);
      break;

    case 2:
      wavelength = 1.0;
      rainbow_effect(wavelength);
      break;

    case 3:
      if (speed < 5)
        wavelength = 5;
      else
        wavelength = 1.0 / (speed / 10.0);
      rainbow_effect(wavelength);
      break;

    case 4:
      wavelength = 1.0;
      stripe_effect(wavelength);
      break;

    case 5:
      wavelength = 0.5;
      stripe_effect(0.5);
      break;
    }

    FastLED.show();
  }
  else
  {
    if (!strip_disabled)
    {
      for (int i = 0; i < LEDSTRIP_SIZE; i++)
      { // For each pixel...
        leds[i].setRGB(0, 0, 0);
      }
      FastLED.show();
      strip_disabled = true;
    }
  }
}

void VehicleControl::set_ledstrip(uint8_t *buffer)
{
  if (buffer[0] == 49)
    strip_enabled = 1;
  else
    strip_enabled = 0;

  mode = buffer[1] - 0x30;

  if (buffer[2] == 49)
    frontbeam_en = 1;
  else
    frontbeam_en = 0;

  digitalWrite(FRONTBEAM_PIN,frontbeam_en);
}

void VehicleControl::rainbow_effect(float lwavelength)
{
  strip_disabled = false;
  j = j + ledpos;
  if (j >= 255)
    j = j - 255;
  if (j < 0)
    j = j + 255;

  for (int i = 0; i < LEDSTRIP_SIZE; i++)
  { // For each pixel...
    leds[i].setHue(i * lwavelength - j);
  }
}

void VehicleControl::stripe_effect(float lwavelength)
{
  strip_disabled = false;
  j = j - ledpos;
  j = j%LEDSTRIP_SIZE;
  if (j >= LEDSTRIP_SIZE)
    j = j - LEDSTRIP_SIZE;
  if (j < 0)
    j = j + LEDSTRIP_SIZE;

  for (int i = 0; i < LEDSTRIP_SIZE; i++)
  {
    if ((i + j) % LEDSTRIP_SIZE > lwavelength * (LEDSTRIP_SIZE / 2))
      leds[i].setHue(22);
    else
      leds[i].setRGB(0, 0, 0);
  }
}