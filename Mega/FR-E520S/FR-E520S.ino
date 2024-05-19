#include "RS485comms.h"
#include "MitProtocol.h"

uint8_t dir = 0;
double freq = 0.0f;

void setup(){

    Serial.begin(115200);
    RS485Serial1Begin();
    delay(1000);
    InverterReset("00");
    
    //freq = ParameterRead("00","2A");
    //Serial.println(freq);
    //ParamaterWrite("00","B4","3031");
    //ParameterRead("00","34");
Serial.println("Hello");
};

void loop(){

if(Serial.available()){

    char msg[32] = {0};
    uint8_t len = Serial.readBytesUntil('\r',msg,sizeof(msg));
    long unsigned int p;

    if(strstr(msg,"RESET") != NULL){
        InverterReset("00");
    }
    else if(strstr(msg,"STATUS") != NULL){
        Serial.println("Reading Status");
        char data[16] = {0};
        InvStatus("00",data);
        Serial.println(data);
    }

};


};



    
