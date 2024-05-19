
#include <Stepper.h>

uint8_t const stepperPins[] = {A0,A1,A2,A3};

const int16_t oneRev = 2048;

Stepper stepper(oneRev,A0,A2,A1,A3);

void setup(){

    //pinMode(stepperPins[0],OUTPUT);
    //pinMode(stepperPins[1],OUTPUT);
    //pinMode(stepperPins[2],OUTPUT);
    //pinMode(stepperPins[3],OUTPUT);

    stepper.setSpeed(10);

};


void loop(){

    stepper.step(oneRev);
    delay(1000);
    stepper.step(-oneRev);
    delay(1000);

};
