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

// Uncomment to get debug output to serial
#define __DEBUG__

// Only one of these uncommented
//#define __HALLOWEEN_COLORS__
#define __CHRISTMAS_COLORS__

// Uncomment to enable.
// String 1 is controlled by pin 3
// String 2 is controlled by pin 4
#define __ENABLE_STRING_1__
//#define __ENABLE_STRING_2__      // Micro and Nano can't handle two strings.  It takes too much memory.

#define PIN__CONTROL_STRING1       3
#define PIN__CONTROL_STRING2       4

#define STRING_NUM_LEDS       100

#define DECAY_RATE                      100
#define DIMBRIGHT_INTERVAL_MS           50
#define FADE_STEP                       30

#define LOWER_LIVE_TIME       5000
#define UPPER_LIVE_TIME       15000
#define TAG_TIME_LOWER        500
#define TAG_TIME_UPPER        2000
#define TTL_BIAS              1000

#define MINIMUM_LIT           40
#define MAXIMUM_LIT           60

#define BRIGHTNESS            80

// =========================================================================
// ===== INTERNAL CONFIG ===================================================
#define FADE_DROP_START   255
#define SLOT_RETRIES      8
#define MAX_OFF_AT_ONCE   2

//#define TTL_FORMULA ttl =  (MAXIMUM_LIT - lit) * TTL_BIAS; ttl = random(LOWER_LIVE_TIME + ttl, UPPER_LIVE_TIME + ttl);
#define TTL_FORMULA ttl =  (MAXIMUM_LIT - lit) * TTL_BIAS; ttl = random(LOWER_LIVE_TIME + ttl, UPPER_LIVE_TIME + ttl);

#define TAG_FORMULA random(TAG_TIME_LOWER - ((MAXIMUM_LIT - lit) * 25), TAG_TIME_UPPER - ((MAXIMUM_LIT - lit) * 98))

#ifdef __HALLOWEEN_COLORS__
#define PICK_COLOR switch(random(5)) { \
          case 0: leds[choice] = CRGB::DarkOrange; break; \
          case 1: leds[choice] = CRGB::Orange; break; \
          case 2: leds[choice] = CRGB::Gold; break; \
          case 3: leds[choice] = CRGB::SaddleBrown; break; \     
          case 4: leds[choice] = CRGB::Maroon; break; \  
     }

#endif

#ifdef __CHRISTMAS_COLORS__
#define PICK_COLOR switch(random(3)) { \
          case 0: leds[choice] = CRGB::Green; break; \
          case 1: leds[choice] = CRGB::Red; break; \
          case 2: leds[choice] = CRGB::White; break; \
     }

#endif

enum LEDState
{
     LS_OFF,
     LS_INCREASING,
     LS_ON,
     LS_DECREASING
};

template <int CONTROL_PIN, int NUM_LEDS>
class LEDString
{
     CRGB leds[NUM_LEDS];
     CRGB color[NUM_LEDS];
     long killBarrier[NUM_LEDS];
     uint8_t   fade[NUM_LEDS];
     uint8_t   state[NUM_LEDS];
     
     uint8_t   pin;
     long      nextTag;
     int       lit;
     uint8_t   changed;
     uint8_t   retries;
     uint8_t   choice;

     long      ttl;

public:

     void set_next_tag()
     {
          #ifdef __DEBUG__
               long tag_time = TAG_FORMULA;
               Serial.print("Next tag: ");
               Serial.println(tag_time);               
               nextTag = tag_time + millis();  
          #else
               nextTag = TAG_FORMULA + millis();  
          #endif          
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
          while(lit < MINIMUM_LIT)
          {
               tag_light();
          }
          FastLED.show();
          
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
          
               if (state[choice] == LS_OFF)
               {                 
                    // Bias upper based on how many lit.
                    long ttl = TTL_FORMULA;
                    #ifdef __DEBUG__
                         long ms  = millis();
                         killBarrier[choice] = millis() + ttl; 
                         Serial.print(ms); Serial.print(" : NEW #"); Serial.print(choice); Serial.print(" . TTL= "); Serial.print(ttl); 
                         Serial.print("  Barrier= "); Serial.println( killBarrier[choice]); 
                    #else
                         killBarrier[choice] = millis() + ttl;   
                    #endif                           
                    
                    PICK_COLOR;
                    color[choice] = leds[choice];
                    leds[choice].fadeLightBy(FADE_DROP_START);  
                    
                    lit++;
                    fade[choice] = FADE_DROP_START;
                    state[choice] = LS_INCREASING;
               
                    #ifdef __DEBUG__
                         Serial.print("Total lit: ");
                         Serial.println(lit);
                    #endif                    
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
          
          if (lit > MINIMUM_LIT)
          {
               changed = 0;
               for (int index = 0; index < NUM_LEDS; index++)
               {
                    if ((killBarrier[index] < millis()) && (state[index] == LS_ON))
                    {
                         #ifdef __DEBUG__
                              Serial.print(millis()); Serial.print(" : KILL #= "); Serial.print(index); 
                              Serial.print("  Barrier= "); Serial.println( killBarrier[index]); 
                         #endif
                         
                         killBarrier[index] = 0;
                         state[index] = LS_DECREASING;   
                         lit--; 
                         changed++;
                    }
                    if (changed > MAX_OFF_AT_ONCE) break;
               }
          }

     }

     void run()
     {
          EVERY_N_MILLISECONDS(DIMBRIGHT_INTERVAL_MS) 
          { 
               changed = 0;
               for (int i = 0; i < NUM_LEDS; i++)
               {
                    switch(state[i])
                    {
                    case  LS_INCREASING:
                         {
                              int newFade = fade[i] - FADE_STEP; 
                              leds[i] = color[i];
                              if (newFade <= 0) 
                              {
                                   leds[i] = color[i];
                                   state[i] = LS_ON;
                              } 
                              else
                              {
                                   fade[i] = newFade;                                   
                                   leds[i].fadeLightBy(newFade);                                   
                              }
                         }
                         changed++;
                         break;

                    case LS_DECREASING:
                         {
                              int newFade = fade[i] + FADE_STEP; 
                              leds[i] = color[i];
                              if (newFade >= 255) 
                              {
                                   leds[i] = CRGB::Black;
                                   state[i] = LS_OFF;
                              } 
                              else
                              {
                                   fade[i] = newFade;                                   
                                   leds[i].fadeLightBy(newFade);                                   
                              }
                         }
                         changed++;
                         break;

                    default:
                         break;
                    }
               }

               if (changed > 0) FastLED.show();
          }
          
          if ((lit < MAXIMUM_LIT) && (nextTag < millis()))
          {
               tag_light();
               set_next_tag();          
          }

          clear_killed();
     }
  
};

#ifdef __ENABLE_STRING_1__
     LEDString      <PIN__CONTROL_STRING1, STRING_NUM_LEDS>        string1;
#endif
#ifdef __ENABLE_STRING_2__
     LEDString      <PIN__CONTROL_STRING2, STRING_NUM_LEDS>        string2;
#endif
void setup() 
{
     FastLED.setBrightness(BRIGHTNESS);
     
     #ifdef __DEBUG__
          Serial.begin(9600); // Prepare for Serial debugging
     #endif
     randomSeed(analogRead(0));
     #ifdef __ENABLE_STRING_1__
          string1.init(PIN__CONTROL_STRING1); 
     #endif
     #ifdef __ENABLE_STRING_2__
          string2.init(PIN__CONTROL_STRING2); 
     #endif        
}

void loop() 
{
     #ifdef __ENABLE_STRING_1__
          string1.run(); 
     #endif 
     #ifdef __ENABLE_STRING_2__
          string2.run(); 
     #endif           
}


