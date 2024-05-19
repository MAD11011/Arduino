#include "QTRSensors.h"
#include "tokenizer.h"
#define USE_TIMER_1 true
#define USE_TIMER_2 true
#include "TimerInterrupt.h"
#include <arduino-timer.h>
#define TIMER_INTERVAL_MS 62


uint8_t const pins[] = {A7,A6,A5,A4,A3,A2,A1,A0};
const uint8_t num_sensors = sizeof(pins)/sizeof(uint8_t);
uint16_t sensor_data[num_sensors]= {0};
float padding_precent = 0.3;
uint16_t blackline = 0;
const uint8_t read_interval_ms = 10;
QTRSensors sensors = QTRSensors();
float Kp = 0.05, Ki = 0.00001 , Kd = 0.08;
enum Signal { BInter = 1 };

struct {
    uint8_t msg_type = 5;
    float KP = 0;
    float KI = 0;
    float KD = 0;
    uint16_t point = 0;
    int32_t out = 0;
}PIDmsg;

 struct {
     uint8_t in1 = 7;
     uint8_t in2 = 8;
     uint8_t in3 = 9;
     uint8_t in4 = 10;
     uint8_t a = 5;
     uint8_t b = 6;
     void SetSpeed(uint8_t l,uint8_t r){
         analogWrite(a,l);
         analogWrite(b,r);
     }
     void init(){
         pinMode(in1,OUTPUT);
         pinMode(in2,OUTPUT);
         pinMode(in3,OUTPUT);
         pinMode(in4,OUTPUT);
         pinMode(a,OUTPUT);
         pinMode(b,OUTPUT);
         SetSpeed(0,0);
         digitalWrite(in1,HIGH);
         digitalWrite(in3,HIGH);
         digitalWrite(in2,LOW);
         digitalWrite(in4,LOW);
     }
     void Forward(){
         digitalWrite(in3,HIGH);
         digitalWrite(in1,HIGH);
         digitalWrite(in4,LOW);
         digitalWrite(in2,LOW);

     }
     void Right(){
         digitalWrite(in3,HIGH);
         digitalWrite(in1,LOW);
         digitalWrite(in4,LOW);
         digitalWrite(in2,HIGH);
     }
     void Left(){
         digitalWrite(in3,LOW);
         digitalWrite(in1,HIGH);
         digitalWrite(in4,HIGH);
         digitalWrite(in2,LOW);
     }
     
}driver;



struct {
    uint8_t  count = num_sensors;
    uint8_t  pins[num_sensors];
}sensor_info;

struct {
    bool run:1;
    bool read:1;
    bool serial:1;
}state;

double t0 = 0;


void InitSerialT60();

Timer<1,millis> readTimer ;

void setup(){

    ITimer1.init();
    driver.init();
    delay(100);
    state.run = 0;
    state.read = 0;
    state.serial = 0;
    sensors.setSensorPins(pins,num_sensors); 
    sensors.setTypeAnalog();
    sensors.setEmitterPin(2);
    sensors.emittersOn();
    memcpy(sensor_info.pins,pins,sizeof(sensor_info.pins));
    Serial.begin(115200,SERIAL_8O1);
    pinMode(LED_BUILTIN, OUTPUT);
}

char out_msg [256] = {0};

void loop(){
    
    if(state.read){
        readTimer.tick();
    }
    char msg[32] = {0};
    if(Serial.available()){
        if(Serial.peek() == 0x02){
            //Serial.println("Cmd");
            Serial.read();
            Serial.readBytesUntil(0x03,msg,sizeof(msg));
            //Serial.println(msg);
            tokens t;
            t = Tokenize(msg);
            /*
            for(int i=0;i<t.num_tokens;i++){
                Serial.println(t.tokens[i].str);
            }
            */
            if(t.num_tokens >= 1){
                if(strcmp(t.tokens[0].str,"CALIBRATE") == 0){
                    if(strcmp(t.tokens[1].str,"START") == 0){
                        Calibrate();
                        SendCValues();
                    }
                    else if(strcmp(t.tokens[1].str,"INFO") == 0){
                        SendCValues();
                    }
                }
                else if(strcmp(t.tokens[0].str,"DATA") == 0){
                    if(strcmp(t.tokens[1].str,"UPDATE") == 0){
                        if(strcmp(t.tokens[2].str,"ONCE") == 0){
                            sensors.read(sensor_data);
                            SendSData();
                        }
                        else if(strcmp(t.tokens[2].str,"60") == 0 && !state.serial){
                            InitSerialT60();
                        }
                        else if(strcmp(t.tokens[2].str,"STOP") == 0){
                            StopSerialT();
                        }
                    }
                    else if(strcmp(t.tokens[1].str,"INFO") == 0){
                        memset(out_msg,0,sizeof(out_msg));
                        out_msg[0] = 1;
                        memcpy(out_msg+1,&sensor_info,sizeof(sensor_info));
                        //Serial.write((char*)(&sensor_info)+1,sizeof(sensor_info)-1);
                        //SendEncoded((char*)(&sensor_info)+1,sizeof(sensor_info)-1);
                        SendEncoded(out_msg,sizeof(sensor_info)+1);
                        SendLog("Sup' PC",7);
                        
                    }

                }
                else if(strcmp(t.tokens[0].str,"RUN") == 0){
                    state.run = 1;
                }
                else if(strcmp(t.tokens[0].str,"TEST") == 0){
                    //SpeedTest();
                }
                else if(strcmp(t.tokens[0].str,"STOP") == 0){
                    state.run = 0;
                }
                else if(strcmp(t.tokens[0].str,"PID") == 0){
                    if(strcmp(t.tokens[1].str,"SET") == 0){
                        const char *s = t.tokens[2].str;
                        char *eq = NULL;
                        if((eq = strchr(s,'=')) != NULL){
                            if(isdigit(*(eq+1))){
                                float v = atof(eq+1);
                                char gain_chr = *(eq-1);
                                if(strchr("PID",gain_chr) != NULL){
                                    switch(gain_chr){
                                        case 'P':
                                            Kp = v;
                                            break;
                                        case 'I':
                                            Ki = v;
                                            break;
                                        case 'D':
                                            Kd = v;
                                            break;


                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


void SendCValues(){
    memset(out_msg,0,sizeof(out_msg));
    out_msg[0] = 3;
    out_msg[1] = sensors.calibrationOn.initialized;
    memcpy(out_msg+2,sensors.calibrationOn.minimum,sizeof(uint16_t)*8);
    memcpy(out_msg+2+16,sensors.calibrationOn.maximum,sizeof(uint16_t)*8);
    SendEncoded(out_msg,32+2);
}

void Calibrate(){
    sensors.resetCalibration();
    digitalWrite(LED_BUILTIN,HIGH);
    double t0 = millis();
    sensors.setSamplesPerSensor(2);
    while(millis() - t0 < 6000){
        sensors.calibrate();
    }
    sensors.setSamplesPerSensor(4);
    digitalWrite(LED_BUILTIN,LOW);
}


void SendSData(){
    static uint8_t buf[sizeof(uint16_t)*num_sensors+1] = {0} ;
    memset(buf,0,sizeof(buf)+1);
    buf[0] = 2;
    memcpy(buf+1,sensor_data,sizeof(buf));
    SendEncoded(buf,sizeof(buf)+1);
    if(state.run){
        static uint8_t buf1[sizeof(uint16_t)+sizeof(bool)+1];
        //memset(buf1,0,sizeof(uint16_t)+1);
        buf1[0] = 4;
        *((uint16_t*)(buf1+1)) = blackline;
        buf1[3] = IsOnBlack();
        SendEncoded(buf1,sizeof(buf1));
        SendEncoded((char*)&PIDmsg,sizeof(PIDmsg));
        }
    
    //COBSencode(buf,sizeof(sensor_data)+2);
    //Serial.write((char*)buf,sizeof(buf));
    //Serial.write((char*)sensor_data,sizeof(sensor_data));
};

void SendEncoded(char* buffer,int size){
    uint8_t buf[size+2] = {0};
    memcpy(buf+1,buffer,size);
    COBSencode(buf,size+2);
    Serial.write((char*)buf,size+2);
}

void SendLog(char* buffer,int size){
    uint8_t buf[size+3] = {0};
    buf[0] = 7;
    buf[1] = 02;
    buf[size+3-1] = 03;
    memcpy(buf+2,buffer,size);
    SendEncoded(buf,size+3);
}

void SendLog(const char* buffer){
    int size = sizeof(buffer);
    uint8_t buf[size-1+3] = {0};
    buf[0] = 7;
    buf[1] = 02;
    buf[size+3-1] = 03;
    memcpy(buf+2,buffer,size);
    SendEncoded(buf,size+3);
}

void InitSerialT60(){
    StartSensorRead();
    ITimer1.restartTimer();
    state.serial = 1;
    ITimer1.attachInterruptInterval(TIMER_INTERVAL_MS,SendSData);
};

void StopSerialT(){
    state.serial = 0;
    ITimer1.detachInterrupt();
}

void COBSencode(char *buf,size_t size){

    buf[0] = 0;
    buf[size-1] = 0;
    uint8_t *arr = (uint8_t*) buf+1;
    uint8_t *last_zero = (uint8_t*) buf;
    for(int idx=0;idx<size-1;idx++){
       if(arr[idx] == 0){
           *last_zero = &arr[idx]-last_zero;
           last_zero = &(arr[idx]);
       }

    }
};

void PID(uint16_t line){
    static const int16_t scale = 10000;
    static const int16_t set_point = 3600; 
    static int16_t perr = 0;
    static int16_t Dt = read_interval_ms;
    static int32_t It = 0;
    int16_t err = line - set_point;
    int16_t P = err * Kp ;
    int16_t D = (Kd * (err - perr) / Dt) ;
    int16_t I = Ki * err * Dt ;
    if(IsOnBlack()){
        It += I;
    }
    int32_t out = P + It + D;

    if(state.serial){
        PIDmsg.KP = Kp;
        PIDmsg.KI = Ki;
        PIDmsg.KD = Kd;
        PIDmsg.point = set_point;
        PIDmsg.out = out;
    }

    uint8_t speed = constrain(out,110,255);
    uint8_t speed_offset = 5;
    //driver.Forward();
    driver.SetSpeed(speed,speed);
    if(out > 0){
        //#Go Right
        driver.Right();
        //driver.SetSpeed(255-speed-speed_offset,speed-speed_offset);

    }else{
        //#Go Left
        driver.Left();
        //driver.SetSpeed(speed-speed_offset,255-speed-speed_offset);
    }

    perr = err;
}

     

bool SensorRead(){
    digitalWrite(LED_BUILTIN,LOW);    
    static uint16_t buffer[num_sensors];
    sensors.read(buffer);
    uint8_t zeros = 0;
    for(int idx=0;idx<num_sensors;idx++){
        if(buffer[idx] == 0)zeros++;
    };
    if(zeros<2){
        memcpy(sensor_data,buffer,sizeof(sensor_data));
    }
    if(state.run){
        static uint16_t line = 0 ;
        static uint8_t intersections = 0;
        static bool on_inter = 0;
        
        if(IsOnBlack() ){
            blackline = BlackLine();
            digitalWrite(LED_BUILTIN,HIGH);    
        }
        if(!on_inter && IsAllBlack()){
            on_inter = true;
            intersections++;
        }
        if (!IsAllBlack()){
            on_inter = false;
        }


        PID(blackline);
        /*
        if(intersections == 1 ){
            if(on_inter == true){
                driver.SetSpeed(165,175);
                driver.Forward();
            }
            else{
            }
        }
        else if(intersections == 2){
            driver.SetSpeed(0,0); 
        }
        */


           
    }
    return true;
}

void StartSensorRead(){
    state.read=1;
    readTimer.every(read_interval_ms,SensorRead);
};

void StopSensorRead(){
    state.read=0;
    readTimer.cancel();
};

void SpeedTest(){
    bool start_test = false;
    uint8_t test_speed = 125;
    char log_msg[64];
    uint8_t len;
    unsigned long t0 = 0;
    unsigned long t_start = 0;
    while(true){
        if(start_test == false && IsAllBlack()){
            start_test = true; 
            len = sprintf(log_msg,"Starting Test At Speed %d",log_msg);
            SendLog(log_msg,len);

            t_start = millis();
            t0 = t_start;
            driver.SetSpeed(test_speed,test_speed);
            driver.Forward();
        }
        if(start_test){
          if(!IsAllBlack()){
            driver.SetSpeed(0,0);
            break;
          }
          if(millis() > t0 + 10){
            t0 = millis();
            len = sprintf(log_msg,"Line = %d At = %ld",BlackLine(),t_start - t0);
            SendLog(log_msg,len);
          }
          

        }
    }

}


uint8_t GetCalibrated(size_t idx){
    if(idx > num_sensors-1){
        return 0;
    }
    uint16_t v = sensor_data[idx];
    uint16_t range = sensors.calibrationOn.maximum[idx] - sensors.calibrationOn.minimum[idx];
    uint16_t padding = range * padding_precent + sensors.calibrationOn.minimum[idx];
    v = constrain(v,padding,sensors.calibrationOn.maximum[idx]);
    v -= padding;
    return 255 * ((float)v / range);
}

bool IsOnBlack(){
    if(BlackLine() == 0 && GetCalibrated(0) == 0){
        return false;
    }
    return true;
}

bool IsAllBlack(){
    for(int i=0;i<num_sensors;i++){
        if(GetCalibrated(i) == 0)return false;
    }
    return true;
}

uint16_t BlackLine(){
    uint16_t blackline_x = 0;
    static uint8_t processed_data[num_sensors] ;
    
    memset(processed_data,0,sizeof(uint16_t)*num_sensors);
    uint8_t* maximum = processed_data;
    for(int a=0;a<num_sensors;a++){
        processed_data[a] = GetCalibrated(a);
        if(processed_data[a] > *maximum){
            maximum = &processed_data[a];
        }
    }
    uint8_t left = (maximum != processed_data)? *(maximum-1) :  0;
    uint8_t right = (maximum != processed_data+num_sensors)? *(maximum+1) : 0;

    int offset = maximum - processed_data   ;
    blackline_x = offset*1000;
    blackline_x = blackline_x - (float)left/255*500 + (float)right/255*500;
    return blackline_x;
}


