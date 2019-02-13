void setup() {
  // put your setup code here, to run once:
   pinMode(15, OUTPUT); 
   pinMode(16, OUTPUT);
   pinMode(17, OUTPUT);  
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(15, 200);
  analogWrite(16, 200);  
  analogWrite(17, 200);
  delay(2000);
  analogWrite(15, 0);
  analogWrite(16,0);  
  analogWrite(17, 0);
}
