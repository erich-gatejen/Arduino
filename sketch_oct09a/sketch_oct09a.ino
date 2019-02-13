
#define PIN__EYES_RED      3
#define PIN__EYES_GREEN    5
#define PIN__EYES_BLUE     6

#define PIN__SKULL_RED     9
#define PIN__SKULL_GREEN   10
#define PIN__SKULL_BLUE    11


void setup_led_pin(int pin) {
   pinMode(pin, OUTPUT); 
   digitalWrite(pin,255);  
}

void setup() {
   setup_led_pin(PIN__EYES_RED);
   setup_led_pin(PIN__EYES_GREEN);  
   setup_led_pin(PIN__EYES_BLUE);
   setup_led_pin(PIN__SKULL_RED);
   setup_led_pin(PIN__SKULL_GREEN);  
   setup_led_pin(PIN__SKULL_BLUE);
}

void loop() {

  for (int i = 0; i < 256; i++) {
    analogWrite(PIN__EYES_RED, i);
    analogWrite(PIN__EYES_GREEN, i);
    analogWrite(PIN__EYES_BLUE, i);
    analogWrite(PIN__SKULL_RED, i);
    analogWrite(PIN__SKULL_GREEN, i);
    analogWrite(PIN__SKULL_BLUE, i);
    delay(12);            
  }

  for (int i = 255; i >=0; i--) {
    analogWrite(PIN__EYES_RED, i);
    analogWrite(PIN__EYES_GREEN, i);
    analogWrite(PIN__EYES_BLUE, i);
    analogWrite(PIN__SKULL_RED, i);
    analogWrite(PIN__SKULL_GREEN, i);
    analogWrite(PIN__SKULL_BLUE, i);
    delay(12);            
  }
  
}
