#include <LiquidCrystal_I2C.h>
#include <stdlib.h>
const uint8_t ir_pin = 2;

double rpm = 0.0f;
double last_ms = 0.0f;
double delta_ms = 0.0f;
bool interrupted = false;

LiquidCrystal_I2C lcd(0x27,16,2);

void setup(){

    pinMode(ir_pin,INPUT);
    attachInterrupt(digitalPinToInterrupt(ir_pin),Count,FALLING);

    Serial.begin(9600);

    //lcd.begin(16,2);
    lcd.init();
    lcd.backlight();
    lcd.leftToRight();
    lcd.setCursor(0,0);
    lcd.print("RPM:");
    lcd.setCursor(4,0);
    lcd.print("0");
    lcd.setCursor(4,0);
};


void loop(){

    //String rpm_s = String((int)floor(rpm)); 
    static char msg[8] = {0};
    static uint8_t spaces = 0;
    spaces = sprintf(msg,"%d",((int)roundf(rpm)));
    memset(msg+spaces,' ',2);
    
    lcd.print(msg);
    lcd.setCursor(4,0);
    if(interrupted){
        //rpm = 0.0f;
    }

};


void Count(){
    interrupted = true; 
    double curr_ms = millis();
    delta_ms = (curr_ms - last_ms);
    rpm = 1/( delta_ms  / 60000);
    last_ms = curr_ms;
    
    Serial.print("RPM =");
    Serial.println(rpm);

};
