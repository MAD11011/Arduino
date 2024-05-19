
#include <Arduino.h>

const uint8_t DEPin = 3;
char _buffer[64] = {0};

void RS485Serial1Begin(){
    pinMode(DEPin,OUTPUT); 
    digitalWrite(DEPin,LOW);
    Serial1.begin(19200,SERIAL_8E2);  
    Serial1.setTimeout(15);

}

int SWrite(char* buf,int size,long unsigned int* bytesWritten){

    digitalWrite(DEPin,HIGH);
    delay(50);
    *bytesWritten =  Serial1.write(buf,size);
    delay(50);
    digitalWrite(DEPin,LOW);

    return 0;
};


int SRead(char* buf,int size,long unsigned int* bytesRead){

    digitalWrite(DEPin,LOW);

    *bytesRead = Serial1.readBytes(_buffer,sizeof(_buffer));
    if(*bytesRead > size || *bytesRead == 0){
        *bytesRead == 0;
        return 1;
    }
    
    memcpy(buf,_buffer,*bytesRead);

    return 0;
};

int SAwaitEOF(){

    if(Serial1.find('\r')){
       return 0; 
    }
    return 1;
};

