#include <Si114.h>

PulsePlug<> pulse(Wire);
//PulsePlug<TwoWire> acc(Wire); // TwoWire is the default class, so this is the same as above

void setup()
{
   Serial.begin(9600);
   Serial.println(__FILE__);

   while (pulse.isPresent() == false)
   {
      Serial.println(F("No SI114x found"));
      delay(1000);
   }
   
   Serial.println(F("SI114x Pulse Sensor found"));
   pulse.init();
}

void loop()
{
   pulse.readSensor(3);   // sensorIdx = 1(HeartRate),2(Ambiant),3(Both)
   Serial.print(F("Active LEDs [red, ir1, ir2] = ["));
   Serial.print(pulse.led_red); 
   Serial.print(F(", "));
   Serial.print(pulse.led_ir1);
   Serial.print(F(", "));
   Serial.print(pulse.led_ir2);
   Serial.println(F("]"));
   Serial.print(F("Ambiant LEDs [visible, ir] = ["));
   Serial.print(pulse.als_vis);
   Serial.print(F(", "));
   Serial.print(pulse.als_ir); 
   Serial.println(F("]"));
   delay(1000);
}
