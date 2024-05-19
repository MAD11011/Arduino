#include <Arduino.h>

const uint8_t dirPin = 2;
const uint8_t stepPin = 3;
const uint8_t stepsPerRevolution = 200;

 
void setup(){
   pinMode(dirPin,OUTPUT); 
   pinMode(stepPin,OUTPUT); 
}

bool STF = 1;

void loop(){
    digitalWrite(dirPin,HIGH); 
    while(STF){
        digitalWrite(stepPin,HIGH); 
        delayMicroseconds(1000);
        digitalWrite(stepPin,LOW); 
        delayMicroseconds(1000);
    }

}

