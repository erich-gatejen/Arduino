#include <FastLED.h>
#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>

#define __DEBUG__
#define __HALLOWEEN_COLORS__

#define STRING_NUM_LEDS       50
#define STRING_RAMP_TARGET    25

#define PIN__CONTROL_LEFT 3
#define PIN__CONTROL_RIGHT 4

#define DECAY_RATE                      100
#define DIMBRIGHT_INTERVAL_MS           30
#define FADE_STEP                       20
#define FADE_DROP_START                 255

#define SLOT_RETRIES                    8

#define RAMP_LOWER_LIVE_TIME  8000
#define RAMP_UPPER_LIVE_TIME  25000
#define LOWER_LIVE_TIME  2000
#define UPPER_LIVE_TIME  10000

#define RAMP_TAG_TIME_LOWER   200
#define RAMP_TAG_TIME_UPPER   500
#define TAG_TIME_LOWER   2000
#define TAG_TIME_UPPER   10000


#ifdef __HALLOWEEN_COLORS__
     CRGB    pallate[] = {
          CRGB::DarkOrange,
          CRGB::Orange,
          CRGB::Gold,
          CRGB::Chocolate     
     };
     #define NUM_COLORS
#endif

enum LEDState
{
     LS_OFF,
     LS_INCREASING,
     LS_ON,
     LS_DECREASING
};

template <int CONTROL_PIN, int NUM_LEDS, int RAMP_TARGET>
class LEDString
{
     CRGB leds[NUM_LEDS];
     CRGB color[NUM_LEDS];
     long killBarrier[NUM_LEDS];
     uint8_t   fade[NUM_LEDS];
     uint8_t   state[NUM_LEDS];
     
     uint8_t   pin;
     long      nextTag;
     int       tagLower;
     int       tagUpper;
     int       liveLower;
     int       liveUpper;
     int       lit;
     uint8_t   changed;
     uint8_t   retries;
     uint8_t   choice;

public:
     void set_next_tag()
     {
          nextTag = millis() + random(tagLower, tagUpper);
     }

     void init(uint8_t   pin_)
     {
          pin = pin_;
          FastLED.addLeds<WS2811, CONTROL_PIN, RGB>(leds, NUM_LEDS);  
          for (int i = 0; i < NUM_LEDS; i++)
          {
                killBarrier[i] = 0;  
                color[i] = CRGB::Black;
                fade[i] = 0;
                state[i] = LS_OFF;
                
                pinMode(pin, OUTPUT);
                digitalWrite(pin, 0);
          }
          FastLED.show();

          lit = 0;

          tagLower = RAMP_TAG_TIME_LOWER;
          tagUpper = RAMP_TAG_TIME_UPPER;
          liveLower = RAMP_LOWER_LIVE_TIME;
          liveUpper = RAMP_UPPER_LIVE_TIME;
          set_next_tag();
     }

     void tag_light()
     {

          retries = SLOT_RETRIES;
          while (retries > 0)
          {

               choice  = random(0, NUM_LEDS);
               #ifdef __DEBUG__
                    Serial.print("Choice: ");
                    Serial.println(choice);
               #endif
          
               if ((state[choice] == LS_OFF) && (lit <= RAMP_TARGET))
               {
                    killBarrier[choice] = millis() + random(liveLower, liveUpper);       

                    int colorchoice = random(NUM_COLORS);  
                    //leds[choice] = CRGB::Red;
                    leds[choice] = *pallate[colorchoice];
                    state[choice] = LS_ON;
     
                    if (lit >= RAMP_TARGET)
                    {
                         tagLower = TAG_TIME_LOWER;
                         tagUpper = TAG_TIME_UPPER; 
                         liveLower = LOWER_LIVE_TIME;
                         liveUpper = UPPER_LIVE_TIME;                   
                    }
     
                    #ifdef __DEBUG__
                         Serial.print("Total lit: ");
                         Serial.println(lit);
                    #endif

                    lit++;
                    break;
               }
               else
               {
                    retries--;
               }
          }
     }

     void clear_killed()
     {
          for (int index = 0; index < NUM_LEDS; index++)
          {
               if ((killBarrier[index] < millis()) && (state[index] == LS_ON))
               {
                    killBarrier[index] = 0;
                    leds[index] = CRGB::Black;
                    lit--; 
               }
          }
          FastLED.show();
     }

     void run()
     {
          
          if (nextTag < millis())
          {
               tag_light();
               set_next_tag();          
          }

          clear_killed();
     }
  
};

LEDString      <PIN__CONTROL_LEFT, STRING_NUM_LEDS, STRING_RAMP_TARGET>        left;

void setup() 
{
     #ifdef __DEBUG__
          Serial.begin(9600); // Prepare for Serial debugging
     #endif
     randomSeed(analogRead(0));
     left.init(PIN__CONTROL_LEFT);      
}

void loop() 
{
     left.run();            
}


