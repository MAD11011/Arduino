#include <NewPing.h>

const uint8_t echoPin = 8;
const uint8_t trigPin = 9;

NewPing sonar(trigPin,echoPin,70);

uint8_t const leds[] = {3,5,6}; // G Y R 

void setup() {
  // put your setup code here, to run once:
  pinMode(leds[0],OUTPUT);
  pinMode(leds[1],OUTPUT);
  pinMode(leds[2],OUTPUT);
  Serial.begin(9600);
}

uint8_t max = 40;

void loop() {
  // put your main code here, to run repeatedly:
  int dist = sonar.ping_cm();
  dist = (dist > max) ? max : dist;
  float range = ((float)dist)/max;
  float green_range  =  InRange(range-0.6,0.0,0.4) / 0.4 ;
  float yellow_range =  InRange(range-0.3,0.0,0.3) / 0.3 ;
  float red_range    =  InRange(1.0-range,0.7,1.0) / 0.3 ;
  analogWrite(leds[0],(uint8_t) (green_range  *255));
  analogWrite(leds[1],(uint8_t) (yellow_range * 255 ) );
  analogWrite(leds[2],(uint8_t) (red_range    * 255) );
  
}

float InRange(float val,float min,float max){
  float res = 0.0f;
  res = ((val >= min) && (val <= max)) ? val : 0.0f;
  return res;
}
