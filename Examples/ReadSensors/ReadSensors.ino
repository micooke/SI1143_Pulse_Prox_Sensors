#include <Si114.h>

PulsePlug<> pulse(Wire);
//PulsePlug<TwoWire> acc(Wire); // TwoWire is the default class, so this is the same as above

void setup()
{
   Serial.begin(9600);
   Serial.println("pulse_PSO2");

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
   pulse.readSensor(1);   // sensorIdx = 1(HeartRate),2(Ambiant),3(Both)
   Serial.print("Active LEDs : ");
   Serial.print(pulse.led_red); 
   Serial.print(",");
   Serial.print(pulse.led_ir1);
   Serial.print(",");
   Serial.println(pulse.led_ir2);
   delay(500);
   pulse.readSensor(2);   // sensorIdx = 1(HeartRate),2(Ambiant),3(Both)
   Serial.print("Ambiant LEDs : ");
   Serial.print(pulse.als_ir); 
   Serial.print(",");
   Serial.println(pulse.als_vis);
   delay(500);
}