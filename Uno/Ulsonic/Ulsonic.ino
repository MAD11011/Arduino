#include <NewPing.h>

const uint8_t echoPin = 8;
const uint8_t trigPin = 9;
const uint8_t ledPin  = 10;

NewPing sonar(trigPin,echoPin,70);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ledPin,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int dist = sonar.ping_cm();
  if(dist < 10){
    digitalWrite(ledPin,HIGH);
  }
  else{
    digitalWrite(ledPin,LOW);
  }
  Serial.print(dist);
  Serial.println(" cm");
}
