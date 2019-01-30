#include <Si114.h>
#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library

#ifdef _VARIANT_ID107HR_
  #ifdef ACCEL_HWI2C
  #define USE_SOFTWAREI2C
  #endif
#else
  #define USE_SOFTWAREI2C
  #define PIN_WIRE1_SDA 22
  #define PIN_WIRE1_SCL 23
#endif

//#define USE_SOFTWAREI2C
#ifdef USE_SOFTWAREI2C
#include <SoftwareI2C.h>
SoftwareI2C sWire(PIN_WIRE1_SDA, PIN_WIRE1_SCL);
PulsePlug<SoftwareI2C> pulse(sWire);
#else
PulsePlug<> pulse(Wire);
//PulsePlug<TwoWire> pulse(Wire); // TwoWire is the default class, so this is the same as above
#endif

#define OLED_WIDTH 64
#define OLED_HEIGHT 32
MicroOLED oled(OLED_RST, OLED_DC, OLED_CS);

uint32_t tPage;
bool button_wasPressed = false;
uint8_t page_num = 0;
const uint8_t page_count = 2;

void draw_page(uint8_t idx = 0);
void page_startup();
void page_active(uint8_t idx = 0);

void padded_oled_print(uint8_t row, String title, uint32_t value);

void setup()
{
   Serial.begin(9600);
   Serial.println(__FILE__);

   pinMode(PIN_BUTTON1, INPUT_PULLUP);

   while (pulse.isPresent() == false)
   {
      Serial.println(F("No SI114x found"));
      delay(1000);
   }
   
   Serial.println(F("SI114x Pulse Sensor found"));
   pulse.init();

  oled.setScreenSize(OLED_WIDTH, OLED_HEIGHT);
  oled.begin();
  oled.display();
  oled.setFontType(0);  // Set font to type 0
  page_startup();
  delay(2000); // show splash for 2s
  page_active();
  tPage = millis();
}

void loop()
{
  if (Button_isPressed(PIN_BUTTON1))
  {
    page_num = (page_num + 1 < page_count)?page_num+1:0;
  }
  
  if (millis() - tPage > 20) // 20ms = 50Hz
  {
    tPage = millis();
    draw_page(page_num);
  }
  yield();
}

// debounce the button
bool Button_isPressed(uint8_t ButtonPin)
{
  bool button_isPressed = (digitalRead(ButtonPin) == 0);

  bool result = (button_wasPressed == false) & (button_isPressed == true);
  button_wasPressed = button_isPressed;
  return result;
}

void draw_page(uint8_t idx)
{
  switch(idx)
  {
    case 0:
      page_active(0); break;
    default:
      page_active(1);
    break;
  }
}

void page_startup()
{
  oled.setCursor(0, 0);
  oled.print("github.com");
  oled.setCursor(0, 10);
  oled.print("/micooke");
  oled.setCursor(0, 20);
  oled.print(__TIME__);
  oled.display();
  oled.clear(SFEOLED_PAGE);
}

void padded_oled_print(uint8_t row, String title, uint32_t value)
{
  oled.setCursor(0, row); oled.print(title);
  uint8_t xIncrement = ((value < 10000) + (value < 1000) + title.length()) * 5;
  oled.setCursor(xIncrement, row);
  oled.print(value);
}

void page_active(uint8_t idx)
{
    pulse.readSensor(idx + 1);   // sensorIdx = 1(HeartRate),2(Ambiant),3(Both)

    if (idx == 0)
    {
      padded_oled_print( 0, "RED    ", pulse.led_red);
      padded_oled_print(10, "IR1    ", pulse.led_ir1);
      padded_oled_print(20, "IR2    ", pulse.led_ir2);
    }
    else
    {
      oled.setCursor(0, 0); oled.print("ALS");
      #ifdef USE_SOFTWAREI2C
      oled.setCursor(35, 0); oled.print("SWI2C");
      #else
      oled.setCursor(35, 0); oled.print("HWI2C");
      #endif

      padded_oled_print(10, "VIS    ", pulse.als_vis);
      padded_oled_print(20, "IR     ", pulse.als_ir);
    }
    
    oled.display();
    oled.clear(SFEOLED_PAGE);
}
