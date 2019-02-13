#include <FastLED.h>


// =========================================================================
// ===== CONFIGURATION =====================================================

#define PIN__EYES_RED      5
#define PIN__EYES_GREEN    6
#define PIN__EYES_BLUE     9

#define PIN__SKELETON_RED     10
#define PIN__SKELETON_GREEN   11
#define PIN__SKELETON_BLUE    12  // NOT PWM

#define PICK_COLOR(__TARGET__) switch(random(6)) { \
          case 0: __TARGET__ = CRGB::CadetBlue; break; \
          case 1: __TARGET__ = CRGB::White; break; \
          case 2: __TARGET__ = CRGB::White; break; \
          case 3: __TARGET__ = CRGB::DodgerBlue; break; \
          case 4: __TARGET__ = CRGB::Cyan; break; \
          case 5: __TARGET__ = CRGB::AliceBlue; break; \
     }

// Uncomment to get debug output to serial
#define __DEBUG__

// =========================================================================
// ===== INTERNAL CONFIG ===================================================

#define START_DELAY_MS             500
#define STEP_INTERVAL_EYES_MS      70
#define STEP_INTERVAL_SKELETON_MS     100

#define EYES_ON_MIN_MS    2000
#define EYES_ON_MAX_MS    9000
#define EYES_WAIT_MIN_MS  500
#define EYES_WAIT_MAX_MS  2000

#define EYES_PULSE_CYCLES_MIN     2
#define EYES_PULSE_CYCLES_MAX     5

#define EYES_PULSE_LOW_TARGET_MIN    190
#define EYES_PULSE_LOW_TARGET_MAX    255
#define EYES_PULSE_HIGH_TARGET_MIN   0 
#define EYES_PULSE_HIGH_TARGET_MAX   100

#define EYES_PULSE_STEP_SIZE         5

#define SKELETON_TRANSITION_MIN_MS      4000
#define SKELETON_TRANSITION_MAX_MS      18000 

#define SCALE_OFFSET     3

// =========================================================================
// ===== IMPLEMENTATION ====================================================

enum eyes_state
{
     EYES_WAIT = -1,
     EYES_OFF = 0,
     EYES_ON = 1,
     EYES_PULSE_UP = 2,
     EYES_PULSE_DOWN = 3
};

int eyesState; 
long eyesStateTimeBarrier;

long skeletonNextTimeBarrier;

int pulseCyclesRemaining;
int pulseStep;
int pulseTarget;

CRGB SKELETON;
CRGB SKELETONTarget;

void setup_led_pin(int pin) {
   pinMode(pin, OUTPUT); 
   analogWrite(pin,255);  
}

int pick_new_eyes_state()
{
     int result = eyesState;
     while(result == eyesState)
     {
          result = random(EYES_OFF, EYES_PULSE_DOWN + 1);
          if ((result = EYES_PULSE_UP) && (random(2) == 1)) result = EYES_ON;
     }
     return result;
}

bool is_new_target()
{
     if (millis() > skeletonNextTimeBarrier)
     {
          skeletonNextTimeBarrier = millis() + random(SKELETON_TRANSITION_MIN_MS, SKELETON_TRANSITION_MAX_MS);  
          return true;
     }
     return false;
}

void pick_new_target()
{
     PICK_COLOR(SKELETONTarget);          
}

void transition(uint8_t & existing, uint8_t target)
{
     if (existing == target) return;
     
     if (existing < target) 
     {
          uint8_t difference = target - existing;
          difference = scale8_video(difference, SCALE_OFFSET);
          existing += difference;
     } 
     else 
     {
          uint8_t difference = existing - target;
          difference = scale8_video(difference, SCALE_OFFSET);
          existing -= difference;
     }
}

uint8_t invert(const uint8_t value)
{
     return ~value;
}

void showSKELETON()
{
     analogWrite(PIN__SKELETON_RED,invert(SKELETON.red));  
     analogWrite(PIN__SKELETON_GREEN,invert(SKELETON.green));  
     analogWrite(PIN__SKELETON_BLUE,invert(SKELETON.blue));              
}

void setup() {
     #ifdef __DEBUG__
          Serial.begin(9600); // Prepare for Serial debugging
     #endif
     randomSeed(analogRead(0));
          
     setup_led_pin(PIN__EYES_RED);
     setup_led_pin(PIN__EYES_GREEN);  
     setup_led_pin(PIN__EYES_BLUE);
     setup_led_pin(PIN__SKELETON_RED);
     setup_led_pin(PIN__SKELETON_GREEN);  
     setup_led_pin(PIN__SKELETON_BLUE);
     
     eyesState = EYES_WAIT;  
     eyesStateTimeBarrier = millis() + START_DELAY_MS;

     pick_new_target();
     SKELETON = CRGB::Black;

     skeletonNextTimeBarrier = millis();
     
     // Blink startup
     analogWrite(PIN__EYES_RED,0);
     delay(3000);     
     analogWrite(PIN__EYES_RED,255);
}

void loop() {

     EVERY_N_MILLISECONDS(STEP_INTERVAL_EYES_MS) 
     { 

          // Manage EYES first.
          switch(eyesState)
          {
               case EYES_OFF:
               {
                    eyesState = pick_new_eyes_state();
                    switch(eyesState)
                    {
                         case EYES_ON:
                              #ifdef __DEBUG__
                                   Serial.println("Turning eyes on.");
                              #endif
                              
                              eyesStateTimeBarrier = millis() + random(EYES_ON_MIN_MS, EYES_ON_MAX_MS);  
                              analogWrite(PIN__EYES_RED, 0);
                              break; 

                         case EYES_PULSE_UP:
                         case EYES_PULSE_DOWN:
                              #ifdef __DEBUG__
                                   Serial.println("Starting eyes pulse.");
                              #endif
                         
                              eyesState = EYES_PULSE_UP;
                              pulseStep = 255;
                              pulseTarget = random(EYES_PULSE_HIGH_TARGET_MIN, EYES_PULSE_HIGH_TARGET_MAX + 1);
                              pulseCyclesRemaining = random(EYES_PULSE_CYCLES_MIN, EYES_PULSE_CYCLES_MAX);
                              break;                                                           
                    }
               }
               break;                 
               
               case EYES_ON:
                    if (eyesStateTimeBarrier < millis())
                    {
                         analogWrite(PIN__EYES_RED, 255);     
                         eyesState = EYES_WAIT;
                         eyesStateTimeBarrier = millis() + random(EYES_WAIT_MIN_MS, EYES_WAIT_MAX_MS);
                    }
                    break;
               
               case EYES_PULSE_UP:
                    pulseStep -= EYES_PULSE_STEP_SIZE;
                    if (pulseStep <= pulseTarget)
                    {
                         pulseStep = pulseTarget;
                         eyesState = EYES_PULSE_DOWN;  
                         pulseTarget = random(EYES_PULSE_LOW_TARGET_MIN, EYES_PULSE_LOW_TARGET_MAX + 1);          
                    }
                    analogWrite(PIN__EYES_RED, pulseStep); 
                    break;
               
               case EYES_PULSE_DOWN:
                    pulseStep += EYES_PULSE_STEP_SIZE;
                    if (pulseStep > pulseTarget)
                    {
                         pulseStep = pulseTarget; 
                         pulseCyclesRemaining--;
                         #ifdef __DEBUG__
                              Serial.print("Pulse cycles remaining: ");  Serial.println(pulseCyclesRemaining);
                         #endif
                         
                         if (pulseCyclesRemaining < 1)
                         {
                              eyesStateTimeBarrier = millis() + random(EYES_WAIT_MIN_MS, EYES_WAIT_MAX_MS);
                              eyesState = EYES_WAIT;      
                         }
                         else
                         {
                           eyesState = EYES_PULSE_UP;  
                         pulseTarget = random(EYES_PULSE_HIGH_TARGET_MIN, EYES_PULSE_HIGH_TARGET_MAX + 1);    
                         }           
                    }
                    analogWrite(PIN__EYES_RED, pulseStep); 
                    break;                   

               case EYES_WAIT:
                    if (eyesStateTimeBarrier < millis()) eyesState = EYES_OFF;
                    break;                          
          }
          
     } // end EVERY_N_MILLISECONDS eyes

     EVERY_N_MILLISECONDS(STEP_INTERVAL_SKELETON_MS) 
     { 
          // SKELETON color                 
          if (SKELETON == SKELETONTarget)
          {
               if (is_new_target()) pick_new_target();     
          }
          else
          {
               transition(SKELETON.red, SKELETONTarget.red);     
               transition(SKELETON.green, SKELETONTarget.green);
               transition(SKELETON.blue, SKELETONTarget.blue);  
               showSKELETON();  
          }
          
     } // end EVERY_N_MILLISECONDS skill
}
