// SI114.h
// Code for the Modern Device Pulse Sensor
// Based on the SI1143 chip
// Heavily updated by Toby Corkindale to use the Wire library,
// amongst other changes. February 2016.
// https://github.com/TJC
// Original version by:
// paul@moderndevice.com 6-27-2012
//
// Modified @micooke (https://github.com/micooke) 08-08-2017
#ifndef SI114_h
#define SI114_h

#include "Si114x_defs.h"
#include <Arduino.h>
#include <Wire.h>   // See https://www.arduino.cc/en/Reference/Wire
#include <stdint.h>

#ifndef _DEBUG
#define _DEBUG 0
#endif

// #define SEND_TOTAL_TO_PROCESSING   // Use this option exclusive of other options
// for sending data to HeartbeatGraph in Processing
// samples on an integral of a power line period [eg 1/60 sec]
// See http://www.worldstandards.eu/electricity/plug-voltage-by-country/ for your AC frequency
// US : 60 Hz - use 16 (1 cycle) or 33 (two cycles)
// Australia,Europe : 50 Hz - use 20 or 40
// #define POWERLINE_SAMPLING 16

//#define AMBIENT_LIGHT_SAMPLING   // also samples ambient slight (slightly slower)
//#define GET_PULSE_READING        // prints HB and signal size

#if (_DEBUG > 0)
#define debugPrint(...) Serial.print(__VA_ARGS__)
#define debugPrintln(...) Serial.println(__VA_ARGS__)
#else
#define debugPrint(...)
#define debugPrintln(...)
#endif

/* Toby Notes:
  In the original library, pins were as follows:
  digiPin() = SDA
  digiPin2() = SCL
  digiPin3() = unused, but assigned to 3 in most cases
  anaPin() = 0 or -1 (???)
  */

/*
// simple smoothing function for heartbeat detection and processing
static float smooth(float data, float filterVal, float smoothedVal)
{
   if (filterVal > 1)
   {   // check to make sure param's are within range
      filterVal = .99;
   }
   else if (filterVal <= 0.0)
   {
      filterVal = 0.01;
   }

   smoothedVal = (data * (1.0 - filterVal)) + (smoothedVal * filterVal);
   return smoothedVal;
}
*/
// simple smoothing function for heartbeat detection and processing
// filterVal = [1 - 99]
uint32_t smooth(uint32_t data, uint8_t filterVal, uint32_t smoothedVal)
{
   filterVal = constrain(filterVal, 1, 99);

   uint32_t returnVal = (data / 100 * (100 - filterVal)) + (smoothedVal / 100 * filterVal);
   return returnVal;
}

class PulsePlug {
   private:
   uint8_t i2cAddr;
   uint8_t samples_to_average;

   uint8_t measurementCount;
   int16_t valley, peak, smoothPeak, smoothValley;
   uint32_t lastTotal, lastBeat, lastValleyTime, lastPeakTime;
   uint32_t baseline, IR_baseline, red_baseline;
   uint32_t beat, hysterisis, LFoutput, HFoutput;
   bool lastBinOut;
   uint32_t cumulativeTotal;
   int32_t signalSize;

   uint16_t* fetchALSData();
   uint16_t* fetchLedData();

   uint8_t getReg(uint8_t reg);
   void setReg(uint8_t reg, uint8_t val);

   void beginTransmission() const { Wire.beginTransmission(i2cAddr); }
   // Note, could optionally take a boolean(false) to say don't release the bus
   uint8_t endTransmission() const { return Wire.endTransmission(); }

   uint8_t readParam(uint8_t addr);
   void writeParam(uint8_t addr, uint8_t val);

   public:
   uint16_t led_red, led_ir1, led_ir2;
   uint16_t als_ir, als_vis;

   PulsePlug()
      : i2cAddr(0x5A), samples_to_average(5), measurementCount(0), valley(0), peak(0), smoothPeak(0), smoothValley(0),
        lastTotal(0), lastBeat(0), lastValleyTime(0), lastPeakTime(0), baseline(0), IR_baseline(0), red_baseline(0),
        hysterisis(0), LFoutput(0), HFoutput(0), lastBinOut(false), cumulativeTotal(0), signalSize(0)
   {
      Wire.begin();
   }
   ~PulsePlug() { Wire.end(); }

   // Note, can also take a 3rd parameter; set false to say don't release the bus
   void requestData(uint8_t count) const { Wire.requestFrom(i2cAddr, count); }

   bool isPresent();
   void init();
   void setAddress(uint8_t _i2cAddr = 0x5A) { i2cAddr = _i2cAddr; }
   uint8_t getAddress() { return i2cAddr; }
   void id();

   void setLEDcurrents(uint8_t LED1, uint8_t LED2, uint8_t LED3);
   void setLEDdrive(uint8_t LED1pulse, uint8_t LED2pulse, uint8_t LED3pulse);

   // samples for smoothing 1 to 10 seem useful 5 is default
   // increase for smoother waveform (with less resolution - slower!)
   void setSamplesToAverage(uint8_t& N_Samples) { samples_to_average = N_Samples; }

   uint32_t readSensor(uint8_t sensorIdx = 1);   // sensorIdx = 1(HeartRate),2(Ambiant),3(Both)
   uint32_t readPulseSensor();
   uint32_t readAmbientSensor();

   uint8_t getHeartRate(bool insertNewlineAtEnd = true);
   uint8_t getPSO2(bool usePrevReading = true);
};

bool PulsePlug::isPresent()
{
   beginTransmission();
   uint8_t result = endTransmission();
   if (result == 0)
   {
      return true;
   }
   else
   {
      debugPrint("isPresent() error code = ");
      debugPrintln(result);
      return false;
   }
}

uint8_t PulsePlug::readParam(uint8_t addr)
{
   // read from parameter ram
   beginTransmission();
   Wire.write(Si114x::COMMAND);
   Wire.write(0x80 | addr);   // PARAM_QUERY
   endTransmission();
   delay(10);   // NOTE: Nothing in datasheet indicates this is required - in original code.
   return getReg(Si114x::PARAM_RD);
}

uint8_t PulsePlug::getReg(uint8_t reg)
{
   // get a register
   beginTransmission();
   Wire.write(reg);
   endTransmission();
   requestData(1);
   uint8_t result = Wire.read();
   delay(10);   // NOTE: Nothing in datasheet indicates this is required - in original code.
   return result;
}

void PulsePlug::setReg(uint8_t reg, uint8_t val)
{
   // set a register
   beginTransmission();
   Wire.write(reg);
   Wire.write(val);
   endTransmission();
   delay(10);   // NOTE: Nothing in datasheet indicates this is required - in original code.
}

void PulsePlug::id()
{
   Serial.print("PART: ");
   Serial.print(PulsePlug::getReg(Si114x::PART_ID));
   Serial.print(" REV: ");
   Serial.print(PulsePlug::getReg(Si114x::REV_ID));
   Serial.print(" SEQ: ");
   Serial.println(PulsePlug::getReg(Si114x::SEQ_ID));
}

void PulsePlug::init()
{
   PulsePlug::setReg(Si114x::HW_KEY, 0x17);
   // pulsePlug.setReg(Si114x::COMMAND, Si114x::RESET_cmd);
   //
   setReg(Si114x::INT_CFG, 0x03);      // turn on interrupts
   setReg(Si114x::IRQ_ENABLE, 0x10);   // turn on interrupt on PS3
   setReg(Si114x::IRQ_MODE2, 0x01);    // interrupt on ps3 measurement
   setReg(Si114x::MEAS_RATE, 0x84);    // 10ms measurement rate
   setReg(Si114x::ALS_RATE, 0x08);     // ALS 1:1 with MEAS
   setReg(Si114x::PS_RATE, 0x08);      // PS 1:1 with MEAS

   // Current setting for LEDs pulsed while taking readings
   // PS_LED21  Setting for LEDs 1 & 2. LED 2 is high nibble
   // each LED has 16 possible (0-F in hex) possible settings
   // see the SI114x datasheet.

   // These settings should really be automated with feedback from output
   // On my todo list but your patch is appreciated :)
   // support at moderndevice dot com.
   setReg(Si114x::PS_LED21, 0x39);   // LED current for 2 (led_ir1 - high nibble) &
   // LEDs 1 (led_red - low nibble)
   setReg(Si114x::PS_LED3, 0x02);   // LED current for LED 3 (led_ir2)

   writeParam(Si114x::PARAM_CH_LIST, 0x77);   // all measurements on

   // increasing PARAM_PS_ADC_GAIN will increase the LED on time and ADC window
   // you will see increase in brightness of visible LED's, ADC output, & noise
   // datasheet warns not to go beyond 4 because chip or LEDs may be damaged
   writeParam(Si114x::PARAM_PS_ADC_GAIN, 0x00);

   // You can select which LEDs are energized for each reading.
   // The settings below (in the comments)
   // turn on only the LED that "normally" would be read
   // ie LED1 is pulsed and read first, then LED2 & LED3.
   writeParam(Si114x::PARAM_PSLED12_SELECT, 0x21);   // 21 select LEDs 2 & 1 (led_red) only
   writeParam(Si114x::PARAM_PSLED3_SELECT, 0x04);    // 4 = LED 3 only

   // Sensors for reading the three LEDs
   // 0x03: Large IR Photodiode
   // 0x02: Visible Photodiode - cannot be read with LEDs on - just for ambient
   // measurement
   // 0x00: Small IR Photodiode
   writeParam(Si114x::PARAM_PS1_ADCMUX, 0x03);   // PS1 photodiode select
   writeParam(Si114x::PARAM_PS2_ADCMUX, 0x03);   // PS2 photodiode select
   writeParam(Si114x::PARAM_PS3_ADCMUX, 0x03);   // PS3 photodiode select

   writeParam(Si114x::PARAM_PS_ADC_COUNTER, B01110000);   // B01110000 is default
   setReg(Si114x::COMMAND, Si114x::PSALS_AUTO_cmd);       // starts an autonomous read loop
}

void PulsePlug::setLEDcurrents(uint8_t _LED1, uint8_t _LED2, uint8_t _LED3)
{
   /*
   VLEDn = 1 V, PS_LEDn = 0001    5.6
   VLEDn = 1 V, PS_LEDn = 0010   11.2
   VLEDn = 1 V, PS_LEDn = 0011   22.4
   VLEDn = 1 V, PS_LEDn = 0100   45
   VLEDn = 1 V, PS_LEDn = 0101   67
   VLEDn = 1 V, PS_LEDn = 0110   90
   VLEDn = 1 V, PS_LEDn = 0111   112
   VLEDn = 1 V, PS_LEDn = 1000   135
   VLEDn = 1 V, PS_LEDn = 1001   157
   VLEDn = 1 V, PS_LEDn = 1010   180
   VLEDn = 1 V, PS_LEDn = 1011   202
   VLEDn = 1 V, PS_LEDn = 1100   224
   VLEDn = 1 V, PS_LEDn = 1101   269
   VLEDn = 1 V, PS_LEDn = 1110   314
   VLEDn = 1 V, PS_LEDn = 1111   359
   */

   _LED1 = constrain(_LED1, 0, 15);
   _LED2 = constrain(_LED2, 0, 15);
   _LED3 = constrain(_LED3, 0, 15);

   PulsePlug::setReg(Si114x::PS_LED21, (_LED2 << 4) | _LED1);
   PulsePlug::setReg(Si114x::PS_LED3, _LED3);
}

void PulsePlug::setLEDdrive(uint8_t LED1pulse, uint8_t LED2pulse, uint8_t LED3pulse)
{
   // this sets which LEDs are active on which pulses
   // any or none of the LEDs may be active on each PulsePlug
   // 000: NO LED DRIVE
   // xx1: LED1 Drive Enabled
   // x1x: LED2 Drive Enabled (Si1142 and Si1143 only. Clear for Si1141)
   // 1xx: LED3 Drive Enabled (Si1143 only. Clear for Si1141 and Si1142)
   // example setLEDdrive(1, 2, 5); sets LED1 on pulse 1, LED2 on pulse 2, LED3,
   // LED1 on pulse 3

   PulsePlug::writeParam(
      Si114x::PARAM_PSLED12_SELECT,
      (LED1pulse << 4) | LED2pulse);   // select LEDs on for readings see datasheet
   PulsePlug::writeParam(Si114x::PARAM_PSLED3_SELECT, LED3pulse);
}

// Returns ambient light values as an array
// First item is visual light, second is IR light.
uint16_t* PulsePlug::fetchALSData()
{
   static uint16_t als_data[2];
   static uint16_t tmp;
   // read out all result registers as lsb-msb pairs of bytes
   beginTransmission();
   Wire.write(Si114x::ALS_VIS_DATA0);
   endTransmission();
   requestData(4);

   for (uint8_t i = 0; i <= 1; ++i)
   {
      als_data[i] = Wire.read();
      tmp         = Wire.read();

      als_data[i] += (tmp << 8);
   }

   return als_data;
}

// Fetch data from the PS1, PS2 and PS3 registers.
// They are stored as LSB-MSB pairs of bytes there; convert them to 16bit ints
// here.
uint16_t* PulsePlug::fetchLedData()
{
   static uint16_t ps[3];
   static uint16_t tmp;

   beginTransmission();
   Wire.write(Si114x::PS1_DATA0);
   endTransmission();
   requestData(6);

   for (uint8_t i = 0; i <= 2; ++i)
   {
      ps[i] = Wire.read();
      tmp   = Wire.read();

      ps[i] += (tmp << 8);
   }

   return ps;
}

void PulsePlug::writeParam(uint8_t addr, uint8_t val)
{
   // write to parameter ram
   beginTransmission();
   Wire.write(Si114x::PARAM_WR);
   Wire.write(val);
   // auto-increments into Si114x::COMMAND
   Wire.write(0xA0 | addr);   // PARAM_SET
   endTransmission();
   delay(10);   // XXX Nothing in datasheet indicates this is required; was in
                // original code.
}

void PulsePlug::readSensor(uint8_t sensorIdx)
{
   uint32_t _sensor[5] = {0, 0, 0, 0, 0};
   uint8_t count       = 0;

#ifdef POWERLINE_SAMPLING
   uint32_t start = millis();
   while (millis() - start < POWERLINE_SAMPLING)
#else
   while (count < samples_to_average)
#endif
   {
      if (sensorIdx & 0x01)
      {
         uint16_t* ledSensor = fetchLedData();
         _sensor[0] += ledSensor[0];
         _sensor[1] += ledSensor[1];
         _sensor[2] += ledSensor[2];
      }
      if (sensorIdx & 0x02)
      {
         uint16_t* alsSensor = fetchALSData();
         _sensor[3] += alsSensor[0];
         _sensor[4] += alsSensor[1];
      }

      ++count;
   }

   // get averages
   if (sensorIdx & 0x01)
   {
      led_red = _sensor[0] / count;
      led_ir1 = _sensor[1] / count;
      led_ir2 = _sensor[2] / count;
   }
   if (sensorIdx & 0x02)
   {
      als_vis = _sensor[3] / count;
      als_ir  = _sensor[4] / count;
   }
}

uint32_t PulsePlug::readPulseSensor()
{
   readSensor(1);
   return led_red + led_ir1 + led_ir2;
}

uint32_t PulsePlug::readAmbientSensor()
{
   readSensor(2);
   return als_vis + als_ir;
}

uint8_t PulsePlug::getHeartRate(bool insertNewlineAtEnd)
{
   uint32_t tNow = millis();
   bool binOut;   // 1 or 0 depending on state of heartbeat
   uint32_t BPM = 0;

   uint32_t total = readPulseSensor();

   // except this one for Processing heartbeat monitor
   // comment out all the bottom print lines
   if (total > 20000L)
   {
      // new measurement
      if (lastTotal < 20000L)
      {
         measurementCount = 1;
         cumulativeTotal  = 0;
         debugPrintln("new measurement");
      }

      // if found a new finger prime filters first 20 times through the loop
      if (measurementCount < 20)
      {
         cumulativeTotal += total;
         baseline = (cumulativeTotal / measurementCount);   // baseline the smooth filter to the total average
         ++measurementCount;
      }

      // main running function
      // the idea here is to keep track of a high frequency signal, HFoutput and a low frequency signal, LFoutput
      // The HF signal is shifted downward slightly (heartbeats are negative peaks)
      // The high freq signal has some hysterisis added.
      // When the HF signal is less than the shifted LF signal, we have found a heartbeat.

      // baseline is the moving average of the signal - the middle of the waveform
      baseline = smooth(total, 99, baseline);
      HFoutput = smooth((total - baseline), 20, HFoutput);   // recycling output - filter to slow down response
      uint32_t HFoutput2 = HFoutput + hysterisis;
      LFoutput           = smooth((total - baseline), 95, LFoutput);

      // heartbeat signal is inverted - we are looking for negative peaks
      uint32_t shiftedOutput = LFoutput - 5 * (signalSize / 100);

      // We need to be able to keep track of peaks and valleys to scale the output for
      // user convenience. Hysterisis is also scaled.
      if (HFoutput > peak)
         peak = HFoutput;
      peak    = constrain(peak, 0, 1500);

      // reset peak detector slower than lowest human HB
      if (tNow - lastPeakTime > 1800)   // TODO: understand why 1800?
      {
         smoothPeak   = smooth(peak, 60, smoothPeak);   // smooth peaks
         peak         = 0;
         lastPeakTime = tNow;
      }

      if (HFoutput < valley)
         valley = HFoutput;
      valley    = constrain(peak, -1500, 0);

      if (tNow - lastValleyTime > 1800)                       // TODO: why are valleys and peaks reset at the same time?
      {                                                       // reset valleys detector slower than lowest human HB
         smoothValley   = smooth(valley, 60, smoothValley);   // smooth valleys
         valley         = 0;
         lastValleyTime = tNow;
      }

      // this the size of the smoothed HF heartbeat signal
      signalSize = smoothPeak - smoothValley;

      // debugPrint(" T  ");
      // debugPrint(signalSize);

      lastBinOut = binOut;
      binOut     = (HFoutput2 < shiftedOutput);

      // you might want to divide by smaller number if you start getting "double bumps"
      hysterisis = (1 - 2 * binOut) * constrain((signalSize / 15), 35, 120);

      debugPrint(binOut);

      if ((lastBinOut == false) && binOut)
      {
         lastBeat = beat;
         beat     = tNow;
         BPM      = 60000 / (beat - lastBeat);
         debugPrint("\t");
         debugPrint(BPM);
         debugPrint("\t");
         debugPrint(signalSize);
         if (insertNewlineAtEnd)
         {
            debugPrint("\n");
         }
      }
   }
   lastTotal = total;

   return static_cast<uint8_t>(BPM);
}

uint8_t PulsePlug::getPSO2(bool usePrevReading)
{
   uint32_t total;
   bool binOut;   // 1 or 0 depending on state of heartbeat

   if (usePrevReading == false)
   {
      readPulseSensor();
   }

   // except this one for Processing heartbeat monitor
   // comment out all the bottom print lines
   if (total > 20000L)
   {
      IR_baseline  = smooth(led_ir1 + led_ir2, 99, IR_baseline);
      red_baseline = smooth(led_red, 99, red_baseline);
   }

   uint8_t PSO2 = red_baseline / (IR_baseline / 2);
   debugPrint("\t");
   debugPrintln(PSO2);

   return static_cast<uint8_t>(PSO2);
}

#endif
