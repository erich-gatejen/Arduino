#include <FastLED.h>

// =========================================================================
// ===== DESIGN ============================================================
/*
 red pin   ->  100R -> red led 6.0v  -> 5v
 green pin ->  60R  -> green led 9.6v  -> 5v  
 blue pin  ->  60R  -> blue led 9.6v  -> 5v  
 
 

 */

// =========================================================================
// ===== CONFIG ============================================================

// Uncomment to get debug output to serial
//#define __DEBUG__

//                                       Twisted pair assignments the same for all players
#define PIN__PLAYER1__RED     2       // TP : solid orange
#define PIN__PLAYER1__GREEN   3       // TP : solid green
#define PIN__PLAYER1__BLUE    4       // TP : solid blue
#define PIN__PLAYER1__BUZZER  22      // TP : white-blue    - 5v output pin on controller
#define PIN__PLAYER1__BUTTON  23      // TP : white-green   - attach to ground in box

#define PIN__PLAYERX__V5      0       // TP : solid brown   - common (black in box)
#define PIN__PLAYERX__GND     0       // TP : white-orange   

#define PIN__PLAYER2__RED     5
#define PIN__PLAYER2__GREEN   6
#define PIN__PLAYER2__BLUE    7
#define PIN__PLAYER2__BUZZER  24
#define PIN__PLAYER2__BUTTON  25

#define PIN__PLAYER3__RED     8
#define PIN__PLAYER3__GREEN   9
#define PIN__PLAYER3__BLUE    10
#define PIN__PLAYER3__BUZZER  26
#define PIN__PLAYER3__BUTTON  27

#define PIN__BASE__BUTTON_RESET     28    
#define PIN__BASE__BUTTON_CORRECT   29
#define PIN__BASE__BUTTON_INCORRECT 30
// WARNING!  If input pins are added after this, the value of MAX_TRACKED_PIN must match the last pin added.

#define PIN__BASE__BUZZER           31

#define PIN__BASE__LEDS       11
#define NUM_LEDS              50  // ????



// =========================================================================
// ===== INTERNAL CONFIG ===================================================

#define DEBOUNCE_TIME    50
#define BRIGHTNESS       255

#define MAX_TRACKED_PIN   PIN__BASE__BUTTON_INCORRECT

#define PLAYER1     0
#define PLAYER2     1
#define PLAYER3     2

#define BUTTON_HIT_DELAY  1200

#define CORRECT_BUZZ_TIME     750
#define CORRECT_SILENT_TIME   750
#define CORRECT_FLASH_TIMES   3

#define INCORRECT_BUZZ_TIME   1500

// =========================================================================
// ===== DATA ==============================================================

unsigned long lasttime[MAX_TRACKED_PIN + 1];
bool pinstate[MAX_TRACKED_PIN + 1];

bool player1Disabled;
bool player2Disabled;
bool player3Disabled;

bool perndingAnswer;

CRGB boardLeds[NUM_LEDS];

int  lastPlayerButton;


// =========================================================================
// ===== FUNCTION ==========================================================

bool checkbutton(int pin) 
{
     //debounce
     if ((lasttime[pin] + DEBOUNCE_TIME) < millis()) 
     {
          lasttime[pin] = millis();          
          int val = digitalRead(pin);     // read the input pin
          if (val == 0) {
               // On, since it is on a pullup resistor 
               pinstate[pin] = true;  
          } else {
               // Off
               pinstate[pin] = false;        
          } 
     }
     return pinstate[pin];
}

void check_all_buttons()
{
     checkbutton(PIN__PLAYER1__BUTTON); 
     checkbutton(PIN__PLAYER2__BUTTON);   
     checkbutton(PIN__PLAYER3__BUTTON);  
     
     checkbutton(PIN__BASE__BUTTON_RESET);  
     checkbutton(PIN__BASE__BUTTON_CORRECT); 
     checkbutton(PIN__BASE__BUTTON_INCORRECT); 
}

void set_all_led(const CRGB value)
{
     for (int i = 0; i < NUM_LEDS; i++)
     {
          boardLeds[i] = value;        
     }
     FastLED.show(); 
}

void player_led_pin(const int pin, const uint8_t intensity)
{
     analogWrite(pin, ~intensity);       
}

void player_color(const int player, const CRGB value)
{
     switch(player)
     {
          case PLAYER1:
               player_led_pin(PIN__PLAYER1__RED, value.red);
               player_led_pin(PIN__PLAYER1__GREEN, value.green);
               player_led_pin(PIN__PLAYER1__BLUE, value.blue);
               break;     
          
          case PLAYER2:
               player_led_pin(PIN__PLAYER2__RED, value.red);
               player_led_pin(PIN__PLAYER2__GREEN, value.green);
               player_led_pin(PIN__PLAYER2__BLUE, value.blue);
               break;
                         
          case PLAYER3: 
               player_led_pin(PIN__PLAYER3__RED, value.red);
               player_led_pin(PIN__PLAYER3__GREEN, value.green);
               player_led_pin(PIN__PLAYER3__BLUE, value.blue);
               break;      
     }
}

bool reset()
{
     player1Disabled = false;
     player2Disabled = false;
     player3Disabled = false;  
     perndingAnswer = false;  
  
     set_all_led(CRGB::Black);  
     check_all_buttons();
     
     player_color(PLAYER1, CRGB::Black);
     player_color(PLAYER2, CRGB::Black);
     player_color(PLAYER3, CRGB::Black);     
}

void pressed_button(const int sound_pin, const int player)
{
          
     set_all_led(CRGB::Blue);
     player_color(player, CRGB::White);
     delay(BUTTON_HIT_DELAY);
     digitalWrite(sound_pin, LOW);
}

void last_player_correct()
{
     player1Disabled = true;
     player2Disabled = true;
     player3Disabled = true;  

     player_color(lastPlayerButton, CRGB::Green);    

     for (int index = 0; index < CORRECT_FLASH_TIMES; index++)
     {
          set_all_led(CRGB::Green);           
          digitalWrite(PIN__BASE__BUZZER, HIGH);    
          delay(CORRECT_BUZZ_TIME);
          set_all_led(CRGB::White);    
          digitalWrite(PIN__BASE__BUZZER, LOW);
          delay(CORRECT_SILENT_TIME);             
     }
     set_all_led(CRGB::Green);     
}

void last_player_incorrect()
{
     player1Disabled = true;
     player2Disabled = true;
     player3Disabled = true;  

     player_color(lastPlayerButton, CRGB::Red);    

     set_all_led(CRGB::Red);           
     digitalWrite(PIN__BASE__BUZZER, HIGH);    
     delay(CORRECT_BUZZ_TIME);
     digitalWrite(PIN__BASE__BUZZER, LOW);       
}


// =========================================================================
// ===== MAIN ==============================================================

void setup() 
{
     #ifdef __DEBUG__
          Serial.begin(9600); // Prepare for Serial debugging
     #endif

     pinMode(PIN__PLAYER1__RED, OUTPUT); 
     pinMode(PIN__PLAYER1__GREEN, OUTPUT); 
     pinMode(PIN__PLAYER1__BLUE, OUTPUT); 
     pinMode(PIN__PLAYER1__BUZZER, OUTPUT); 
     pinMode(PIN__PLAYER1__BUTTON, INPUT_PULLUP); 
     
     pinMode(PIN__PLAYER2__RED, OUTPUT); 
     pinMode(PIN__PLAYER2__GREEN, OUTPUT); 
     pinMode(PIN__PLAYER2__BLUE, OUTPUT); 
     pinMode(PIN__PLAYER2__BUZZER, OUTPUT); 
     pinMode(PIN__PLAYER2__BUTTON, INPUT_PULLUP); 
     
     pinMode(PIN__PLAYER3__RED, OUTPUT); 
     pinMode(PIN__PLAYER3__GREEN, OUTPUT); 
     pinMode(PIN__PLAYER3__BLUE, OUTPUT); 
     pinMode(PIN__PLAYER3__BUZZER, OUTPUT); 
     pinMode(PIN__PLAYER3__BUTTON, INPUT_PULLUP); 
     
     pinMode(PIN__BASE__BUTTON_RESET, INPUT_PULLUP); 
     pinMode(PIN__BASE__BUTTON_CORRECT, INPUT_PULLUP); 
     pinMode(PIN__BASE__BUTTON_INCORRECT, INPUT_PULLUP); 

     FastLED.setBrightness(BRIGHTNESS);    
     pinMode(PIN__BASE__LEDS, OUTPUT);
     FastLED.addLeds<WS2811, PIN__BASE__LEDS, RGB>(boardLeds, NUM_LEDS);

     reset();
}

void loop() 
{

     if (checkbutton(PIN__BASE__BUTTON_RESET))
     {
          reset();      
     }
     else if (!player1Disabled && !perndingAnswer && checkbutton(PIN__PLAYER1__BUTTON))
     {
          pressed_button(PIN__PLAYER1__BUTTON, PLAYER1); 
          lastPlayerButton = PLAYER1;
          player1Disabled = true;  
          perndingAnswer = true;
     }
     else if (!player2Disabled && !perndingAnswer && checkbutton(PIN__PLAYER2__BUTTON))
     {
          pressed_button(PIN__PLAYER2__BUTTON, PLAYER2); 
          lastPlayerButton = PLAYER2;
          player2Disabled = true;  
          perndingAnswer = true;       
     }
     else if (!player3Disabled && !perndingAnswer && checkbutton(PIN__PLAYER3__BUTTON))
     {
          pressed_button(PIN__PLAYER3__BUTTON, PLAYER3); 
          lastPlayerButton = PLAYER3;
          player2Disabled = true; 
          perndingAnswer = true;  
     }
     else if (perndingAnswer && checkbutton(PIN__BASE__BUTTON_CORRECT))
     {
          last_player_correct();
          perndingAnswer = true;
     }
     else if (perndingAnswer && checkbutton(PIN__BASE__BUTTON_INCORRECT))
     {
          last_player_incorrect();   
          perndingAnswer = true;            
     }

}



