
#define MAX_EASING_SERVOS 6
#define USE_PCA9685_SERVO_EXPANDER
#define DISABLE_COMPLEX_FUNCTIONS
#define USE_SERVO_LIB
#include <ServoEasing.hpp>
#include <Wire.h>


ServoEasing Servo1AtPCA9685(0x40);
ServoEasing Servo2AtPCA9685(0x40);
ServoEasing Servo3AtPCA9685(0x40);
ServoEasing Servo4AtPCA9685(0x40);
ServoEasing Servo5AtPCA9685(0x40);
ServoEasing Servo6AtPCA9685(0x40);


int Angles [6] = {90, 120, 180, 90, 90, 180};
String command;
bool ON = false;
const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];
boolean newData = false;
boolean State = true;
int LeftSideForward = 6;
int LeftSideBackward = 7;
int RightSideForward = 4;
int RightSideBackward = 5;
int SpeedRight = 11;
int SpeedLeft = 10;
unsigned long Currentmilis ;
unsigned long Millis;
String MonitorMsg;
bool func_called = false;
bool incremented = false;
bool Grabber = false ;



void setup() {
  Serial.begin(9600);
  Servo1AtPCA9685.InitializeAndCheckI2CConnection(&Serial);
  Servo1AtPCA9685.attach(7, 90);
  Servo2AtPCA9685.attach(1, 120);
  Servo3AtPCA9685.attach(2, 180);
  Servo4AtPCA9685.attach(3, 90);
  Servo5AtPCA9685.attach(4, 90);
  Servo6AtPCA9685.attach(5, 180);

  delay(3000);

  pinMode(RightSideForward, OUTPUT);
  pinMode(LeftSideForward, OUTPUT);
  pinMode(LeftSideBackward, OUTPUT);
  pinMode(RightSideBackward, OUTPUT);

}

void loop() {

  recv();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    parseCommand(tempChars);
    newData = false;
  }
  Millis = millis();
  //Serial.print("*C");
  //Serial.println(Millis);
  //Serial.print("*");
  if ((Millis > Currentmilis + 200) & func_called == false) {
    Release();
  }
  /*
    Servo1AtPCA9685.write(Angles[0]);
    Servo2AtPCA9685.write(Angles[1]);
    Servo3AtPCA9685.write(Angles[2]);
    Servo4AtPCA9685.write(Angles[3]);
    Servo5AtPCA9685.write(Angles[4]);
    Servo6AtPCA9685.write(Angles[5]); */
  func_called = false;
}

void recv() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

void parseCommand(String com)
{
  String part1;
  String part2;
  String part3;
  String part4;
  String Key;

  int spaceIndex = com.indexOf(" ");
  part1 = com.substring(0, spaceIndex);
  switch (part1[0]) {

    case ('K'):
      part2 = com.substring(spaceIndex + 1);
      ButtonHandler(part2);
      break;
    case ('P'):
      part2 = com.substring(spaceIndex + 1, 5);
      int X = com.indexOf('X');
      int Y = com.indexOf('Y');
      part3 = com.substring(X + 1, Y);
      part4 = com.substring(Y + 1);
      PadHandler(part2.toInt(), part3.toInt(), part4.toInt());
      break;
    case ('S'):
      part2 = com.substring(spaceIndex + 1, 5);
      part3 = com.substring(spaceIndex + 2);
      break;
    case ('T'):
      part2 = com.substring(spaceIndex + 1, 5);
      part3 = com.substring(com.indexOf('R'), com.indexOf('A', 4));
      part4 = com.substring(com.indexOf('A', 4));
      break;
  }
  Serial.print("*T");
  Serial.println(part2);
  Serial.println(part3);
  Serial.println(part4);
  Serial.print("*");

}
void ButtonHandler(String KeyID) {
  switch (KeyID[0]) {
    case ('B'):
      if (KeyID[1]  == '+') {
        State = true;
        Serial.print("*LR0G255B0");
      }
      else if (KeyID[1]  == '-') {
        State = false;
        Serial.print("*LR255G0B0");
      }
      break;
    case ('G'):
      if (KeyID[1]  == '+') {
        if (Grabber == false) {
          Servo6AtPCA9685.setSpeed(70);
          Servo6AtPCA9685.startEaseTo(80);
          Grabber = true;
        }
        else{
         Servo6AtPCA9685.setSpeed(70);
         Servo6AtPCA9685.startEaseTo(160);
         Grabber = false; 
        }
      }
  }
}
void PadHandler(int PadID, int X, int Y) {

  Currentmilis = millis();
  func_called = true;
  int SpeedX, SpeedY, NewAngle;

  Serial.print("*D");
  Serial.println(Currentmilis);
  Serial.print("*");
  Serial.print("*T");
  Serial.println("///PAD///");
  if (State == true) {
    switch (PadID) {
      case (0):
        Serial.print("*T");
        if (Y > 70) {
          SpeedY = map(Y, 70, 100, 50, 120);
          if (X > 50) {
            SpeedX = map(X, 50, 100, 10, 70);
            Serial.print(SpeedX);
            Serial.print("///");
            Serial.println(SpeedY);
            moveForward(SpeedY, SpeedY - SpeedX);
          }
          else if (X < 50) {
            SpeedX = map(X, 50, 0, 10, 70);
            Serial.print(SpeedX);
            Serial.print("///");
            Serial.println(SpeedY);
            moveForward(SpeedY - SpeedX, SpeedY);
          }
        }
        else if (Y < 30) {
          SpeedY = map(Y, 30, 0, 50, 120);
          if (X > 50) {
            SpeedX = map(X, 50, 100, 10, 70);
            Serial.print(SpeedX);
            Serial.print("///");
            Serial.println(SpeedY);
            moveBackward(SpeedY, SpeedY - SpeedX );
          }
          else if (X < 50) {
            SpeedX = map(X, 50, 0, 10, 70);
            Serial.print(SpeedX);
            Serial.print("///");
            Serial.println(SpeedY);
            moveBackward(SpeedY - SpeedX, SpeedY);
          }
        }
        else if ( Y < 30 & Y > 50) {
          Release();
        }
        Serial.print("*");
        break;

      case (1):
        if (X > 50) {
          int Speed = map(X, 50, 100, 50, 80);
          Serial.println(Speed);
          turnLeft(Speed, Speed);
        }
        else if (X < 50) {
          int Speed = map(X, 50, 0, 50, 80);
          Serial.println(Speed);
          turnRight(Speed, Speed);
        }
        else {
          Release();
        }

        break;
    }
  }
  else {

    switch (PadID) {
      case 0:
        NewAngle = map(X, 100, 0, -9, 9);
        if(!(Angles[0] + NewAngle > 180 | Angles[0] + NewAngle < 0 )){
        Angles[0] = Angles[0]+NewAngle;
        }
        Servo1AtPCA9685.setSpeed(80);
        Servo1AtPCA9685.startEaseTo(Angles[0]);
        NewAngle = map(Y, 0, 100, -9, 9);
        if(!(Angles[1] + NewAngle > 180 | Angles[1] + NewAngle < 0 )){
        Angles[1] = Angles[1]+NewAngle;
        }
        Servo2AtPCA9685.setSpeed(50);
        Servo2AtPCA9685.startEaseTo(Angles[1]);
        break;
      case 1:
        NewAngle = map(X, 100, 0, -9, 9);
        if(!(Angles[4] + NewAngle > 180 | Angles[0] + NewAngle < 0 )){
        Angles[4] = Angles[4]+NewAngle;
        }
        Servo4AtPCA9685.setSpeed(80);
        Servo4AtPCA9685.startEaseTo(Angles[3]);
        NewAngle = map(Y, 100, 0, -9, 9);
        if(!(Angles[2] + NewAngle > 180 | Angles[0] + NewAngle < 0 )){
        Angles[2] = Angles[2]+NewAngle;
        }
        Servo3AtPCA9685.setSpeed(80);
        Servo3AtPCA9685.startEaseTo(Angles[2]);
        break;
        //updateAndWaitForAllServosToStop();
    }

  }
  Serial.print("*");
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
  Serial.println("Left");
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
