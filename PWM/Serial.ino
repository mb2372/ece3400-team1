void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  float value=analogRead(0);
  Serial.println(value);
  delay(100);
  analogWrite(10, 255);
}
