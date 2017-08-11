/* SI114_Pulse_Demo.ino
 * demo code for the Modern Device SI1143-based pulse sensor
 * http://moderndevice.com/product/pulse-heartbeat-sensor-pulse-sensor-1x/
 * Paul Badger 2013 with plenty of coding help from Jean-Claude Wippler
 * Modified by Toby Corkindale Feb 2016 to use Wire library.
 * Hardware setup - please read the chart below and set the appropriate options
 */

#define _DEBUG 1
#include <Si114.h>

/*
 For Arduino users, use the SDA and SCL pins on your controller.
 For Teensy 3.x/LC users, likewise.
 Typically pin 18 is SCL, and 19 is SDA.

 The original docs here said to use 10k resisters in series. (Why?)
 I note that you should additionally have 5k pull-up resistors going to a 3V3
 source.


 JeeNode users just set the appropriate port

 JeeNode Port  SCL ('duino pin)  SDA ('duino pin)
 0             18 (A5)       19 (A4)
 1             4             14 (A0)
 2             5             15 (A1)
 3             6             16 (A2)
 4             7             17 (A3)
 */

PulsePlug pulse;

void setup()
{
   Serial.begin(57600);
   Serial.println("pulse_demo");

   while (pulse.isPresent() == false)
   {
      Serial.println("No SI114x found");
      delay(1000);
   }
   
   Serial.println("SI114x Pulse Sensor found");
   pulse.init();
}

void loop()
{
   pulse.getHeartRate();
   delay(1);
}