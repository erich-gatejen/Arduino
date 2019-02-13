#include <Mouse.h>

#define DEBOUNCE_TIME 50 

#define COLLECT__DELAY       300
#define TICKET__DELAY        3000
#define TICKET__PARK_DELAY   1500
#define TICKET__MOVE_DELAY   4

#define TICKET__BRONZE_X     100
#define TICKET__BRONZE_Y     -100
#define TICKET__SILVER_X     0
#define TICKET__SILVER_Y     -100
#define TICKET__GOLD_X       -100
#define TICKET__GOLD_Y       -100

#define DEBUG_PIN       LED_BUILTIN

#define IPIN__ONOFF     2
#define IPIN__COLLECT   3
#define IPIN__BRONZE    4
#define IPIN__SILVER    5
#define IPIN__GOLD      6

#define IBITMASK__ONOFF     1
#define IBITMASK__COLLECT   2
#define IBITMASK__BRONZE    4
#define IBITMASK__SILVER    8
#define IBITMASK__GOLD      16

byte  currentPins;
byte  readPins;

#define PINLED__ONOFF   7
#define PINLED__COLLECT 8
#define PINLED__BRONZE  9
#define PINLED__SILVER  10
#define PINLED__GOLD    11

long lastDebounceTime;
long lastClickTime;
bool clicked;
bool lit13;

void setup()
{
  pinMode(DEBUG_PIN, OUTPUT);
  digitalWrite(DEBUG_PIN, LOW);
  
  pinMode(IPIN__ONOFF, INPUT_PULLUP);
  pinMode(IPIN__COLLECT, INPUT_PULLUP);
  pinMode(IPIN__BRONZE, INPUT_PULLUP);
  pinMode(IPIN__SILVER, INPUT_PULLUP);
  pinMode(IPIN__GOLD, INPUT_PULLUP);

  pinMode(PINLED__ONOFF, OUTPUT);
  pinMode(PINLED__COLLECT, OUTPUT);
  pinMode(PINLED__BRONZE, OUTPUT);
  pinMode(PINLED__SILVER, OUTPUT);
  pinMode(PINLED__GOLD, OUTPUT);

  currentPins = 0;
  lastDebounceTime = millis();
}

void loop()
{
  
  checkPins();
  if (readPins & IBITMASK__ONOFF) {
    currentPins = IBITMASK__ONOFF;
    digitalWrite(PINLED__ONOFF, HIGH);
    Mouse.begin();
    runIt();
    Mouse.end();
    digitalWrite(PINLED__ONOFF, LOW);
    
  }
  
}

void runIt() 
{
  while (true)
  {
    checkPins();

    // Turned off?
    if (! (readPins & IBITMASK__ONOFF))
    {
      unlight();
      return;
    }

    // State change?
    if ((readPins != IBITMASK__ONOFF) && (currentPins != readPins)) 
    {
      unlight();
      currentPins = readPins;
      light();
      clicked = false;
    }    

    if (currentPins == (IBITMASK__ONOFF + IBITMASK__COLLECT))
    {
      if (millis() > (lastClickTime + COLLECT__DELAY)) clickMouse();     
    } 
    else if (currentPins == (IBITMASK__ONOFF + IBITMASK__BRONZE)) 
    {
      rewardCheck(TICKET__BRONZE_X, TICKET__BRONZE_Y);      
    } 
    else if (currentPins == (IBITMASK__ONOFF + IBITMASK__SILVER)) 
    {
      rewardCheck(TICKET__SILVER_X, TICKET__SILVER_Y); 
    } 
    else if (currentPins == (IBITMASK__ONOFF + IBITMASK__GOLD)) 
    {
      rewardCheck(TICKET__GOLD_X, TICKET__GOLD_Y); 
    }
    
  } // end while
}

void unlight()
{
  digitalWrite(PINLED__COLLECT, LOW);
  digitalWrite(PINLED__BRONZE,  LOW);
  digitalWrite(PINLED__SILVER,  LOW);
  digitalWrite(PINLED__GOLD, LOW);  
}

void clickMouse()
{
  Mouse.click();
  clicked = true;
  lastClickTime = millis();  
}

void rewardCheck(signed char x, signed char y)
{
  if (clicked)
  {
      if (millis() > (lastClickTime + TICKET__DELAY)) 
      {

        // Move, click, move back to park
        Mouse.move(x, y);
        delay(TICKET__MOVE_DELAY);
        clickMouse();
        delay(1000);
        Mouse.move(-x, -y);
        delay(TICKET__MOVE_DELAY);
        lastClickTime = millis();
        clicked = false;
                     
      } 
  } 
  else 
  {
    if (millis() > (lastClickTime + TICKET__PARK_DELAY))
    { 
      clickMouse();
    }
  }
}

void light()
{
  if (currentPins == (IBITMASK__ONOFF + IBITMASK__COLLECT))
  {
    digitalWrite(PINLED__COLLECT, HIGH);  
    lastClickTime = millis() - COLLECT__DELAY;
  } 
  else if (currentPins == (IBITMASK__ONOFF + IBITMASK__BRONZE)) 
  {
    primeTickets(PINLED__BRONZE);  
  } 
  else if (currentPins == (IBITMASK__ONOFF + IBITMASK__SILVER)) 
  {
    primeTickets(PINLED__SILVER);
  } 
  else if (currentPins == (IBITMASK__ONOFF + IBITMASK__GOLD)) 
  {
    primeTickets(PINLED__GOLD);
  }
    
}

void primeTickets(int pin)
{
    digitalWrite(pin, HIGH);
    lastClickTime = millis() - TICKET__PARK_DELAY;  
    clicked = false;
}

void checkPins() 
{ 
  if ((lastDebounceTime + DEBOUNCE_TIME) > millis()) {
    return;
  }
  lastDebounceTime = millis();

  byte pins = 0;
  int pin = digitalRead(IPIN__ONOFF);
  if (pin == LOW) pins += IBITMASK__ONOFF;
  pin = digitalRead(IPIN__COLLECT);
  if (pin == LOW) pins += IBITMASK__COLLECT;
  pin = digitalRead(IPIN__BRONZE);
  if (pin == LOW) pins += IBITMASK__BRONZE;
  pin = digitalRead(IPIN__SILVER);
  if (pin == LOW) pins += IBITMASK__SILVER;
   pin = digitalRead(IPIN__GOLD);
  if (pin == LOW) pins += IBITMASK__GOLD;

  readPins = pins; 
}

