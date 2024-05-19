#include <Wire.h>

uint8_t slave0 = 0x26;

void setup(){

    Wire.begin();
    Serial.begin(9600);
    for(int i=0;i<8;i++){
        Serial.print("P");
        Serial.print(i);
        Serial.print(" ");

    }
    Serial.write('\n');
    Wire.requestFrom((int)slave0,1,false);
}

uint8_t port_byte = 0;
void loop(){
    while(Wire.available()){
        byte msg = 0;
        msg = Wire.read();
        port_byte = msg;
    }
    for(int i=0;i<8;i++){
        Serial.print(port_byte & (1 << i));
        Serial.print("  ");
    }
    for(int i=0;i<8*3;i++){
        Serial.write('\b');
    }



}

void OnRequest(){
}
