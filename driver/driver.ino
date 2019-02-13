

// =========================================================================
// ===== DESIGN ============================================================

/*
  
  -- STEPPER ----------------------------

  Pump stepper is home at max intake.




  
 */

// =========================================================================
// ===== CONFIG ============================================================

#define PIN__INDICATOR__RED        D7
#define PIN__INDICATOR__GREEN      D6
#define PIN__INDICATOR__BLUE       D5

#define PIN__LCD__IN1              25
#define PIN__LCD__IN2              24
#define PIN__LCD__IN3              23
#define PIN__LCD__IN4              22
#define PIN__LCD__RS               2

#define PIN__BUTTON__RESET         27
#define PIN__BUTTON__HOME_PUMP     28
#define PIN__BUTTON__HOME_AIR      29

#define PIN__KEYPAD__IN1           30
#define PIN__KEYPAD__IN2           31
#define PIN__KEYPAD__IN3           32
#define PIN__KEYPAD__IN4           33
#define PIN__KEYPAD__IN5           34
#define PIN__KEYPAD__IN6           35
#define PIN__KEYPAD__IN7           36
#define PIN__KEYPAD__IN8           37

#define PIN__PUMP__IN1             50
#define PIN__PUMP__IN2             51
#define PIN__PUMP__IN3             52
#define PIN__PUMP__IN4             54

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



// =========================================================================
// ===== DATA ==============================================================


unsigned long lasttime[MAX_TRACKED_PIN + 1];




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

void reset() {

     while ( !checkbutton(PIN__BUTTON__HOME_PUMP) && !checkbutton(PIN__BUTTON__HOME_AIR))
     {
          if (!checkbutton(PIN__BUTTON__HOME_PUMP)
          {
               
          }
     }
     
}

void setup() {

     // reset

}

void loop() {


}
