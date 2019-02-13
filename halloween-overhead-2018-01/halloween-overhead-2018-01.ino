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
//#define __DEBUG__

#define PROG_SOLID__MIN_HOLD  15000
#define PROG_SOLID__MAX_HOLD  27000

#define PROG_ALT__MIN_CYCLES  4
#define PROG_ALT__MAX_CYCLES  11
#define PROG_ALT__MIN_HOLD    300
#define PROG_ALT__MAX_HOLD    1000

#define PROG_2XCOLOR__MIN_HOLD     8000
#define PROG_2XCOLOR__MAX_HOLD     13000

#define PROG_RUNNER__MIN_SEED     12
#define PROG_RUNNER__MAX_SEED     30
#define PROG_RUNNER__NEXT_SEED    7
#define PROG_RUNNER__DELAY_MS     43         

// Only one of these uncommented
#define __HALLOWEEN_COLORS__
//#define __CHRISTMAS_COLORS__

#define PIN__CONTROL_STRING1  4

#define NUM_LEDS              91

#define BRIGHTNESS            255


// =========================================================================
// ===== INTERNAL CONFIG ===================================================

#define COLOR__HOLLOWEEN_ORANGE    0xee2500

#ifdef __HALLOWEEN_COLORS__
#define PICK_COLOR(_V_)  switch(random(3)) { \
          case 0: _V_ = CRGB::Red; break; \
          case 1: _V_ = COLOR__HOLLOWEEN_ORANGE; break; \
          case 2: _V_ = CRGB::Gold; break;   \
     }

#endif

#ifdef __CHRISTMAS_COLORS__
#define PICK_COLOR(_V_) switch(random(3)) { \
          case 0: _V_ = CRGB::Green; break; \
          case 1: _V_ = CRGB::Red; break; \
          case 2: _V_ = CRGB::White; break; \
     }

#endif

CRGB leds[NUM_LEDS];
CRGB pick;
CRGB pick2;

void setup() 
{
     FastLED.setBrightness(BRIGHTNESS);    
     #ifdef __DEBUG__
          Serial.begin(9600); // Prepare for Serial debugging
     #endif
     randomSeed(analogRead(0));
     pinMode(PIN__CONTROL_STRING1, OUTPUT);

     FastLED.addLeds<WS2811, PIN__CONTROL_STRING1, RGB>(leds, NUM_LEDS);
}

void set_all()
{
     for (int i = 0; i < NUM_LEDS; i++)
     {
          leds[i] = pick;        
     }     
     FastLED.show(); 
}

void program_solid()
{
     #ifdef __DEBUG__
          Serial.println("program_solid");
     #endif
     
     pick_one();
     set_all();
     delay(random(PROG_SOLID__MIN_HOLD,PROG_SOLID__MAX_HOLD));
}

void alternate_leds(bool setFirst)
{
     bool first = setFirst;
     for (int i = 0; i < NUM_LEDS; i++)
     {
          if (first) 
          {
               leds[i] = pick; 
               first = false;  
          }
          else
          {
               leds[i] = pick2; 
               first = true;      
          }
     } 
     FastLED.show(); 
}

void pick_one()
{
     PICK_COLOR(pick);       
}

void pick_two()
{
     pick_one();
     do 
     {
          PICK_COLOR(pick2);
     } while ( (pick2.r == pick.r) && (pick2.g == pick.g) && (pick2.b == pick.b) );        
}

void program_alternate()
{
     #ifdef __DEBUG__
          Serial.println("program_alternate");
     #endif
    
     long cycleHold = random(PROG_ALT__MIN_HOLD, PROG_ALT__MAX_HOLD); 
     int numberCycles = random(PROG_ALT__MIN_CYCLES,PROG_ALT__MAX_CYCLES);
     pick_two();

     bool head = true;
     for (int cycle = 0; cycle < numberCycles; cycle++)
     {
          alternate_leds(head);   
          delay(cycleHold);
          if (head) head = false;
          else head = true;
     }
}

void program_two_color()
{
     #ifdef __DEBUG__
          Serial.println("program_two_color");
     #endif  
     pick_two();
     
     alternate_leds(true);  
     delay(random(PROG_2XCOLOR__MIN_HOLD,PROG_2XCOLOR__MAX_HOLD));
}


void program_runner()
{
     #ifdef __DEBUG__
          Serial.println("program_runner");
     #endif  
     
     int nextSeed = PROG_RUNNER__NEXT_SEED;
     int seedCountDown = NUM_LEDS;
     int seedRemaining = random(PROG_RUNNER__MIN_SEED,PROG_RUNNER__MAX_SEED);
     
     pick = CRGB::Black;
     set_all();

     pick_one();
     leds[0] = pick;
     FastLED.show(); 

     pick_one();
     leds[0] = pick; 
     while (seedCountDown > 0)
     {
          for (int index = (NUM_LEDS-0); index > 0 ; index--)
          {
               leds[index].red = leds[index - 1].red;
               leds[index].green = leds[index - 1].green;
               leds[index].blue = leds[index - 1].blue;
          }

          leds[0] = CRGB::Black;
          if ((nextSeed == 0) && (seedRemaining > 0))
          {
               pick_one();
               leds[0] = pick; 
               seedCountDown = NUM_LEDS; 
               nextSeed = PROG_RUNNER__NEXT_SEED;
               seedRemaining--;                                          
          }
          else
          {
               nextSeed--; 
               seedCountDown--;  
          }
          FastLED.show();  
          delay(PROG_RUNNER__DELAY_MS);         
     }  
}

void loop() 
{
     switch(random(20))
     {
          case 0: case 1: program_runner(); break;   
          case 2: case 3: case 4: program_alternate(); break;
          case 5: case 6: case 7: case 8: program_two_color(); break;
          default: program_solid(); break;     
     }      
      
}


