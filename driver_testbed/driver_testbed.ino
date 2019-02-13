#include <MultiStepper.h>
#include <AccelStepper.h>
#include <Keypad.h>


// =========================================================================
// ===== DESIGN ============================================================

/*
  
  -- STEPPER ----------------------------

  Pump stepper is home at max intake.




  
 */

// =========================================================================
// ===== CONFIG ============================================================

#define DEBUG

#define PIN__INDICATOR__RED        7
#define PIN__INDICATOR__GREEN      6
#define PIN__INDICATOR__BLUE       5

#define PIN__LCD__IN1              23
#define PIN__LCD__IN2              25
#define PIN__LCD__IN3              27
#define PIN__LCD__IN4              29
#define PIN__LCD__RS               2

#define PIN__BUTTON__RESET         38
#define PIN__BUTTON__HOME_PUMP     39
#define PIN__BUTTON__HOME_AIR      40

#define MAX_TRACKED_PIN            PIN__BUTTON__HOME_AIR

#define PIN__KEYPAD__IN1           22
#define PIN__KEYPAD__IN2           24
#define PIN__KEYPAD__IN3           26
#define PIN__KEYPAD__IN4           28
#define PIN__KEYPAD__IN5           30
#define PIN__KEYPAD__IN6           32
#define PIN__KEYPAD__IN7           34
#define PIN__KEYPAD__IN8           36

#define PIN__PUMP__IN1             50
#define PIN__PUMP__IN2             51
#define PIN__PUMP__IN3             52
#define PIN__PUMP__IN4             53

#define PIN__AIR__IN1              46
#define PIN__AIR__IN2              45
#define PIN__AIR__IN3              44
#define PIN__AIR__IN4              43


// =========================================================================
// ===== INTERNAL CONFIG ===================================================

#define STEPPER__MIN     0
#define STEPPER__MAX     100       // 1.8 degrees a step = 200 for 360 = 100 for 180.

#define STEPPER__PUMP__HOME   STEPPER__MIN
#define STEPPER__AIR__HOME    STEPPER__MAX / 2

#define DEBOUNCE_TIME         50

// Keys

#define KEY__1      '1'
#define KEY__2      '2'
#define KEY__3      '3'
#define KEY__4      '4'
#define KEY__5      '5'
#define KEY__6      '6'
#define KEY__7      '7'
#define KEY__8      '8'
#define KEY__9      '9'
#define KEY__0      '0'
#define KEY__UP     '*'
#define KEY__DOWN   '#'
#define KEY__A      'A'
#define KEY__B      'B'
#define KEY__C      'C'
#define KEY__ENTER  'D'

// =========================================================================
// ===== DATA ==============================================================

// -- IO ----------------------------

unsigned long lasttime[MAX_TRACKED_PIN + 1];
int pinstate[MAX_TRACKED_PIN + 1];


// -- KEYPAD ------------------------

const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {KEY__1,KEY__2,KEY__3,KEY__A},
  {KEY__4,KEY__5,KEY__6,KEY__B},
  {KEY__7,KEY__8,KEY__9,KEY__C},
  {KEY__UP,KEY__0,KEY__DOWN,KEY__ENTER}
};

byte keypodRowPins[KEYPAD_ROWS] = {PIN__KEYPAD__IN1, PIN__KEYPAD__IN2, PIN__KEYPAD__IN3, PIN__KEYPAD__IN4};
byte keypadColPins[KEYPAD_COLS] = {PIN__KEYPAD__IN5, PIN__KEYPAD__IN6, PIN__KEYPAD__IN7, PIN__KEYPAD__IN8};
Keypad keypad = Keypad( makeKeymap(keys), keypodRowPins, keypadColPins, KEYPAD_ROWS, KEYPAD_COLS );


// -- STEPPER ------------------------

AccelStepper pumpStepper = AccelStepper(AccelStepper::FULL4WIRE, PIN__PUMP__IN1, PIN__PUMP__IN2, PIN__PUMP__IN3, PIN__PUMP__IN4);
int pumpStepperTarget;


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

void player_led_pin(const int pin, const uint8_t intensity)
{
     analogWrite(pin, ~intensity);       
}


// =========================================================================
// ===== LOGIC =============================================================

void reset() 
{

     while ( !checkbutton(PIN__BUTTON__HOME_PUMP) && !checkbutton(PIN__BUTTON__HOME_AIR))
     {
          if (!checkbutton(PIN__BUTTON__HOME_PUMP))
          {
               
          }
     }
     
}


void setup()
{
     #ifdef DEBUG
          Serial.begin(9600);
     #endif

     pumpStepper.setCurrentPosition(0);
     stepper.setMaxSpeed(300);
     stepper.setAcceleration(200);
}

void loop()
{
     while (stepper.currentPosition() != pumpStepperTarget) stepper.run();
     
     char key = keypad.getKey();
     if (key)
     {
          #ifdef DEBUG
               Serial.println(key);
          #endif
          switch(key)
          {
               case KEY__UP:
                    pumpStepper.setCurrentPosition(0);
                    stepper.moveTo(100);
                    pumpStepperTarget = 100;
                    break;

               case KEY__DOWN:
                    pumpStepper.setCurrentPosition(0);
                    stepper.moveTo(-100);
                    pumpStepperTarget = -100;
                    break;

               default:
               break;
          }
     }
     
}
