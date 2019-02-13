void setup() {
  // put your setup code here, to run once:

  pinMode(3, OUTPUT);
  digitalWrite(3, 255);

}

void loop() {
  // put your main code here, to run repeatedly:

  analogWrite(3, 10);
  delay(1200);
  analogWrite(3, 250);
  delay(1200);  

}
