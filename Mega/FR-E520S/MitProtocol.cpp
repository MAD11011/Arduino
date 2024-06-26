
#include <Arduino.h>
#include "RS485comms.h"

struct ControlCode{
   const char STX = 0x02;
   const char ETX = 0x03;
   const char ENQ = 0x05;
   const char ACK = 0x06;
   const char LF  = 0x0A;
   const char CR  = 0x0D;
   const char NAK = 0x15;
}CCode;

const char EndOL = CCode.CR;

enum DataLength {
    DATA2BYTES = 2,
    DATA4BYTES = 4,
    DATA6BYTES = 6
};

int WaitingTime = 1;

void toASCII(char* dest,int source){

    sprintf(dest,"%X",source);

}

bool IsValidString(char* str,uint8_t size){
    return (str[strlen(str)] == EndOL);
}

void MChecksum(char* dest,char* source){

    int  checkSum = 0;
    char sCheckSum[4] = {'\0'};
    for(int c=0;c<strlen(source);c++){
        // printf("%d- %c | %02X\n",c,source[c],source[c]);
        checkSum +=  source[c];
    }
    toASCII(sCheckSum,checkSum);
    int bytesToSkip = 0;
    bytesToSkip = (strlen(sCheckSum) > 2) ?  strlen(sCheckSum) - 2 : 0;
    strcpy(dest,sCheckSum+bytesToSkip);
    //printf("Sum = %X , \"%s\"\n",checkSum,dest);

}

void DataWrite(const char sNum[3],const char ins[3],const char data[7],DataLength dPacketL){

    char* packetFrame = (char*) calloc(9+dPacketL+1,1);
    char wTime[3]= {'\0'};
    char cSum[3] = {'\0'};
    unsigned long bytesW;
    toASCII(wTime,WaitingTime);
    snprintf(packetFrame,9+dPacketL+1,"%c%s%s%s%s\0",CCode.ENQ
                                                ,sNum
                                                ,ins
                                                ,wTime
                                                ,data);
    //skip first byte
    MChecksum(cSum,packetFrame+1);
    strcat(packetFrame,cSum);
    strncat(packetFrame,&EndOL,1);
    SWrite(packetFrame,9+dPacketL+1,&bytesW);
    free(packetFrame);
}


void DataRead(const char sNum[3],const char ins[3]){

    char* packetFrame = (char*) calloc(10,1);
    char wTime[3]= {'\0'};
    char cSum[3] = {'\0'};
    unsigned long bytesW;
    toASCII(wTime,WaitingTime);
    snprintf(packetFrame,9,"%c%s%s%s\0",CCode.ENQ
                                       ,sNum
                                       ,ins
                                       ,wTime);
    //skip first byte
    MChecksum(cSum,packetFrame+1);
    strcat(packetFrame,cSum);
    strncat(packetFrame,&EndOL,1);
    SWrite(packetFrame,strlen(packetFrame),&bytesW);
    Serial.println(packetFrame);
    free(packetFrame);
}

int WriteIReplyData(char* buf,char* error){

    if(buf[0] == CCode.ACK){
        return 0;
    }
    else{
        *error = buf[3];
        return 1;
    }

}

int ReadIReplyData(char* buf,char* dataBuf,int dataBufSize,char* error){

    if(buf[0] == CCode.NAK){
        *error = buf[3];
        return 1;
    }
    char* endOfData = strchr(buf,CCode.ETX);
    char* newBuf = (char*) calloc(endOfData-buf+1,1);
    memcpy(newBuf,buf,endOfData-buf);
    char cSum[3] = {'\0'};
    MChecksum(cSum,newBuf+1);
    if(strncmp(cSum,buf+8,2) != 0){
        *error = 2;
        return 1;
    }
    strncpy(dataBuf,buf+3,endOfData-(buf+3));
    return 0;
}

void ReadPReplyData(char Ccode,const char sNum[3]){

    char packetFrame[5] = {'\0'};
    unsigned long bytesW;
    sprintf(packetFrame,"%c%s%c",Ccode,sNum,EndOL);
    SWrite(packetFrame,sizeof(packetFrame),&bytesW);
}

void InverterReset(const char sNum[3]){
    DataWrite(sNum,"FD","9696",DATA4BYTES);
}

void ParamaterWrite(const char sNum[3],const char par[3],const char val[7]){
    enum DataLength length;
    switch (strlen(val)) {
        case 2:
            length = DATA2BYTES;
            break;
        case 4:
            length = DATA4BYTES;
            break;
        case 6:
            length = DATA6BYTES;
            break;
        default:
            //fprintf(stderr,"Pr write legth is not valid\n");
            return;
    }
    char response[32] = {'\0'};
    unsigned long bytesRead = 0;
    char error = 0;
    DataWrite(sNum,par,val,length);
    SRead(response,sizeof(response),&bytesRead);
    if(!IsValidString(response,sizeof(response))){
       Serial.println("Got Invalid string"); 
    }
    if(WriteIReplyData(response,&error)){
        //fprintf(stderr,"Inverter Responded with NAK error = %c\n",error);
        return;
    }
    //printf("Pr written successfully\n");
}

double ParameterRead(const char sNum[3],const char par[3]){
    char buffer[32] = {0};
    char dataBuf[16]= {0};
    char error = 0;
    unsigned long bytesRead = 0;
    DataRead(sNum,par);
    SRead(buffer,sizeof(buffer),&bytesRead);
    if(!IsValidString(buffer,sizeof(buffer))){
       Serial.println("Got Invalid string"); 
       Serial.println(buffer);
    }
    if(ReadIReplyData(buffer,dataBuf,sizeof(dataBuf),&error)){
        if(error != 2){
            //fprintf(stderr,"Data read error, Recevied NAK\n");
            return -1.0f;
        }
        else{
            //fprintf(stderr,"Data read error, Recevied mismatched checksum\n");
            return -2.0f;
        }
    }
    ReadPReplyData(CCode.ACK,sNum);
    //printf("Value of Pr %s is %.2f\n",par,((float)strtol(dataBuf,NULL,16))/100);
    
    Serial.println(dataBuf);
    return 0.0f;
    //return (double)strtol(dataBuf,NULL,16);


}
void InvStatus(const char sNum[3],char *buf){

    char buffer[32] = {0};
    char dataBuf[16]= {0};
    char error = 0;
    unsigned long bytesRead = 0;
    DataRead(sNum,"7A");
    Serial.println("Got Invalid string"); 
    SRead(buffer,sizeof(buffer),&bytesRead);
    if(!IsValidString(buffer,sizeof(buffer))){
       Serial.println("Got Invalid string"); 
    }
    if(ReadIReplyData(buffer,dataBuf,sizeof(dataBuf),&error)){
        if(error != 2){
            Serial.println("Err"); 
            return;
        }
        else{
            Serial.println("Err"); 
            return;
        }
    }
    ReadPReplyData(CCode.ACK,sNum);
    strncpy(buf,dataBuf,16);

}
