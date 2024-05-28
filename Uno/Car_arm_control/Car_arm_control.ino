
#define MAX_EASING_SERVOS 6
#define USE_PCA9685_SERVO_EXPANDER
#define DISABLE_COMPLEX_FUNCTIONS
#define USE_SERVO_LIB
#include <ServoEasing.hpp>
#include <SoftwareSerial.h>
#include <Wire.h>





ServoEasing Servo1AtPCA9685(0x40);
ServoEasing Servo2AtPCA9685(0x40);
ServoEasing Servo3AtPCA9685(0x40);
ServoEasing Servo4AtPCA9685(0x40);
ServoEasing Servo5AtPCA9685(0x40);
ServoEasing Servo6AtPCA9685(0x40);

SoftwareSerial blue_serial(2,3);

int Angles [6] = {90, 120, 180, 90, 90, 180};
String command;
int LeftSideForward = 6;
int LeftSideBackward = 7;
int RightSideForward = 4;
int RightSideBackward = 5;
int SpeedRight = 11;
int SpeedLeft = 10;
unsigned long Currentmilis ;
unsigned long Millis;
String MonitorMsg;
bool Grabber = false ;
int base_speed = 125;
uint8_t timeToMax_s = 1 ;


void setup() {
    Servo1AtPCA9685.InitializeAndCheckI2CConnection(&blue_serial);
    Servo1AtPCA9685.attach(9, 90);
    Servo2AtPCA9685.attach(10, 120);
    Servo3AtPCA9685.attach(11, 180);
    Servo4AtPCA9685.attach(13, 90);
    Servo5AtPCA9685.attach(14, 20);
    Servo6AtPCA9685.attach(15, 90);


    pinMode(RightSideForward, OUTPUT);
    pinMode(LeftSideForward, OUTPUT);
    pinMode(LeftSideBackward, OUTPUT);
    pinMode(RightSideBackward, OUTPUT);

}

void loop() {

}

enum Dirs {NONE = '0',UP = '1',DOWN = '3',RIGHT = '3',LEFT = '4'};

enum Dirs curr_dir = NONE;

static double delta_ms = millis();

void UpdateState(enum Dirs dir){

    if(curr_dir == NONE){
        if(dir)curr_dir = UP;
        else if(dir)curr_dir = DOWN;
        else if(dir)curr_dir = LEFT;
        else if(dir)curr_dir = RIGHT;
    }else{
        if(dir == NONE)curr_dir = NONE;
    }

}



void UpdateWheels(double delta){
    static double in_speed = 0;
    uiny16_t ms_per_tick = ((base_speed - 255) / TimeToMax_s*1000)*1000;
    uint8_t int_interval = 1/((base_speed - 255) / TimeToMax_s*1000) ;
    uint8_t ticks =ms_per_tick / delta;
    if(curr_dir != NONE && in_speed != 255){
        in_speed += (ticks + in_speed <= 255)? ticks : ticks + in_speed - 255;
    }
    else if(in_speed != 0){
        in_speed -= (in_speed - ticks >= 0)? ticks : in_speed;
    }
    switch(curr_dir){
        case UP:
            moveForward(speed,speed); 
            break;
        case DOWN:
            moveBackward(speed,speed); 
            break;
        case LEFT:
            turnLeft(speed,speed); 
            break;
        case RIGHT:
            turnRight(speed,speed); 
            break;
            Release();
    }

}




void moveForward(int L, int R) {
    digitalWrite(LeftSideForward, LOW);
    digitalWrite(LeftSideBackward, HIGH);
    digitalWrite(RightSideForward, LOW);
    digitalWrite(RightSideBackward, HIGH);
    analogWrite(SpeedRight, R);
    analogWrite(SpeedLeft, L);
}

void moveBackward(int L, int R) {
    digitalWrite(LeftSideForward, HIGH);
    digitalWrite(LeftSideBackward, LOW);
    digitalWrite(RightSideForward, HIGH);
    digitalWrite(RightSideBackward, LOW);
    analogWrite(SpeedRight, R);
    analogWrite(SpeedLeft, L);
}
void turnRight(int L, int R) {
    digitalWrite(LeftSideForward, HIGH);
    digitalWrite(LeftSideBackward, LOW);
    digitalWrite(RightSideForward, LOW);
    digitalWrite(RightSideBackward, HIGH);
    analogWrite(SpeedRight, R);
    analogWrite(SpeedLeft, L);
}
void turnLeft(int L, int R) {
    blue_serial.println("Left");
    digitalWrite(LeftSideForward, LOW);
    digitalWrite(LeftSideBackward, HIGH);
    digitalWrite(RightSideForward, HIGH);
    digitalWrite(RightSideBackward, LOW);
    analogWrite(SpeedRight, R);
    analogWrite(SpeedLeft, L);
}
void Release() {
    digitalWrite(LeftSideForward, LOW);
    digitalWrite(LeftSideBackward, LOW);
    digitalWrite(RightSideForward, LOW);
    digitalWrite(RightSideBackward, LOW);
}
