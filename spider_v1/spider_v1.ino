

#define IPIN__BUTTON      26

#define OPIN__ACTIVE_LED  13
#define OPIN__RED_LED     10
#define OPIN__GREEN_LED   11

#define OPIN__DIRECTION_A 36 
#define OPIN__DIRECTION_B 38
#define OPIN__SPEED       8

#define OPIN__DEAD        46

bool button;
bool setDirection;
bool directionGreen;

void setup() {

  pinMode(IPIN__BUTTON, INPUT_PULLUP); 
  
  pinMode(OPIN__ACTIVE_LED, OUTPUT); 
  pinMode(OPIN__RED_LED, OUTPUT); 
  pinMode(OPIN__GREEN_LED, OUTPUT); 
  
  pinMode(OPIN__DIRECTION_A, OUTPUT); 
  pinMode(OPIN__DIRECTION_B, OUTPUT); 
  pinMode(OPIN__SPEED, OUTPUT); 

  button = false;
  digitalWrite(OPIN__ACTIVE_LED,LOW);
  digitalWrite(OPIN__RED_LED,LOW); 
  digitalWrite(OPIN__GREEN_LED,LOW); 
  digitalWrite(OPIN__SPEED,LOW);

  setDirection = false;
  directionGreen = false;

  // DEAD
  pinMode(OPIN__DEAD, OUTPUT);
  digitalWrite(OPIN__DEAD,LOW);
}

void loop() {

  int val = digitalRead(IPIN__BUTTON);

  if (button) {
    
    if (val == 0) {

        if (setDirection)
        {
          if (directionGreen) {
           
            digitalWrite(OPIN__GREEN_LED,HIGH); 
            
            digitalWrite(OPIN__DIRECTION_A,LOW); 
            digitalWrite(OPIN__DIRECTION_B,HIGH); 
            directionGreen = false;
            
          } else {
            
            digitalWrite(OPIN__RED_LED,HIGH); 
            
            digitalWrite(OPIN__DIRECTION_A,HIGH); 
            digitalWrite(OPIN__DIRECTION_B,LOW);  
            directionGreen = true;  
                    
          }
          setDirection = false;
          
        }
        
        analogWrite(OPIN__SPEED,200);
        button = true;
      
    } else {
        button = false;
        digitalWrite(OPIN__ACTIVE_LED,LOW);  
        digitalWrite(OPIN__RED_LED,LOW); 
        digitalWrite(OPIN__GREEN_LED,LOW); 
        //digitalWrite(OPIN__SPEED,LOW);
        analogWrite(OPIN__SPEED,0);        
    }
    delay(50);   // Force a debounce.     
    
  } else {
      if (val == 0 ) 
      {
        button = true;
        setDirection = true;
        digitalWrite(OPIN__ACTIVE_LED,HIGH);      
      }
  }

}
