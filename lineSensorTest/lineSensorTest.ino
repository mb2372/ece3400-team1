void setup() {
  Serial.begin(9600);
}

void loop() {
  double sensorValue=analogRead(A0);
  Serial.println(sensorValue);
}
