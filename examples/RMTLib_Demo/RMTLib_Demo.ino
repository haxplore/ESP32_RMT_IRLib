/*
 * RMTLib Demo
 *
 *
 */
#include <RMTLib.h>

RMTLib RMTLib;

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  Serial.println("sending command");

  // Roku Home 0x5743C03F
  RMTLib.sendNEC(0x5743C03F);
  delay(5000);
  
  // Mute 0xE0E0F00F
  RMTLib.sendSAMSUNG(0xE0E0F00F);
  delay(5000);

  // Mute 4D
  RMTLib.sendRC5(0x4D);
  delay(5000);
}