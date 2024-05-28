#include <SoftwareSerial.h>

SoftwareSerial bSerial(2,3);

const int led_pin = 6;

void setup(){
   bSerial.begin(9600); 
   pinMode(led_pin,OUTPUT);
}


void loop(){

    char msg;
    if(bSerial.available()){
        msg = bSerial.read();
        if(msg == 'A'){
            digitalWrite(led_pin,HIGH);
        }
        else if(msg == 'a'){
            digitalWrite(led_pin,LOW);
        }

    }

}
