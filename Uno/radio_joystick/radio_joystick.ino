#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

uint8_t x_axis = A0, y_axis = A1;
uint16_t last_x = 0, last_y = 0;
RF24 radio(9,10);

const byte address[6] = "00012";

struct joyEvent{
    uint16_t x;
    uint16_t y;
    uint16_t x_delta;
    uint16_t y_delta;
}joy1;

void setup(){

    Serial.begin(9600);
    radio.begin();
    radio.openReadingPipe(0,address);
    radio.openWritingPipe(address);
    radio.startListening();
     
}

void loop(){

    if(radio.available()){
        char msg[8] = {0}; 
        Serial.println(msg);
        radio.read(msg,sizeof(msg));
        if(strcmp(msg,"JOY") == 0){
            radio.stopListening(); 
            joy1.x_delta = joy1.x - last_x;
            joy1.y_delta = joy1.y - last_y;
            radio.write(&joy1,sizeof(joy1));
            last_x = joy1.x;
            last_y = joy1.y;
            radio.startListening();
        };
    }
    joy1.x = analogRead(x_axis);
    joy1.y = analogRead(y_axis);

}
