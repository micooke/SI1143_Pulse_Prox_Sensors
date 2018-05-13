/* SI1143_proximity_demo.ino
 * http://moderndevice.com/product/si1143-proximity-sensors/
 * Reads the Proximity Sensor and either prints raw LED data or angular data
 * depending the options chosen in the #define's below
 * Toby Corkindale, Paul Badger, Gavin Atkinson, Jean-Claude Wippler  2013-2016
 */

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

#define _DEBUG 1
#include <Si114.h>

float Avect = 0.0;
float Bvect = 0.0;
float Cvect = 0.0;
float Tvect, x, y, angle = 0;

// some printing options for experimentation (sketch is about the same)
//#define SEND_TO_PROCESSING_SKETCH
#define PRINT_RAW_LED_VALUES           // prints Raw LED values for debug or experimenting
#define PRINT_AMBIENT_LIGHT_SAMPLING   // also samples ambient slight (slightly slower)
                                       // good for ambient light experiments, comparing output with ambient

unsigned long lastMillis, red, IR1, IR2;

PulsePlug pulse;

void setup()
{
   Serial.begin(57600);
   Serial.println("proximity_demo");

   if (pulse.isPresent())
   {
      Serial.println("SI114x Pulse Sensor found");
      pulse.id();
   }
   else
   {
      while (1)
      {
         Serial.println("No SI114x found");
         delay(1000);
      }
   }

   pulse.init();

   pulse.setLEDcurrents(6, 6, 6);
   // void PulsePlug.setLEDCurrent( byte LED1, byte LED2, byte LED3)
   // 0 to 15 are valid
   // 1 = 5.6mA, 2 = 11.2mA, 3 = 22.4mA, 4 = 45mA; 5 = 67mA, 6 =90mA, 7 = 112mA, 8 = 135mA, 9 = 157mA,
   // 10 = 180mA, 11 = 202mA, 12 = 224mA, 13 = 269mA, 14 = 314mA, 15 = 359mA
   // It may be possible to damage the chip and or LEDs with some current / time settings - check that datasheet!

   pulse.setLEDdrive(1, 2, 4);
   // void PulsePlug::setLEDdrive(byte LED1pulse, byte LED2pulse, byte LED3pulse){
   // this sets which LEDs are active on which pulses
   // any or none of the LEDs may be active on each PulsePlug
   // 000: NO LED DRIVE
   // xx1: LED1 Drive Enabled
   // x1x: LED2 Drive Enabled (Si1142 and Si1143 only. Clear for Si1141)
   // 1xx: LED3 Drive Enabled (Si1143 only. Clear for Si1141 and Si1142)
   // example setLEDdrive(1, 2, 5); sets LED1 on pulse 1, LED2 on pulse 2, LED3, LED1 on pulse 3
}

void loop()
{
   pulse.readSensor(3);
   float x, y, Tvect, angle;
   uint32_t total = pulse.led_red + pulse.led_ir1 + pulse.led_ir2;

#ifdef SEND_TO_PROCESSING_SKETCH
   /* Add LED values as vectors - treat each vector as a force vector at
    * 0 deg, 120 deg, 240 deg respectively
    * parse out x and y components of each vector
    * y = sin(angle) * Vect , x = cos(angle) * Vect
    * add vectors then use atan2() to get angle
    * vector quantity from pythagorean theorem
    */

   float Avect = pulse.led_ir1;
   float Bvect = pulse.led_red;
   float Cvect = pulse.led_ir2;

   // cut off reporting if total reported from LED pulses is less than the ambient measurement
   // eliminates noise when no signal is present
   if (total > 900)
   {   // determined empirically, you may need to adjust for your sensor or lighting conditions

      x = (float)Avect - (.5 * (float)Bvect) - (.5 * (float)Cvect);   // integer math
      y = (.866 * (float)Bvect) - (.866 * (float)Cvect);

      angle = atan2((float)y, (float)x) * 57.296 + 180;   // convert to degrees and lose the neg values
      Tvect = (long)sqrt(x * x + y * y);
   }
   else
   {   // report 0's if no signal (object) present above sensor
      angle = 0;
      Tvect = 0;
      total = 900;
   }

   // angle is the resolved angle from vector addition of the three LED values
   // Tvect is the vector amount from vector addition of the three LED values
   // Basically a ratio of differences of LED values to each other
   // total is just the total of raw LED amounts returned, proportional to the distance of objects from the sensor.

   debugPrint(angle);
   debugPrint("\t");
   debugPrint(Tvect);
   debugPrint("\t");
   debugPrintln(total);
#endif

   delay(1);
}
