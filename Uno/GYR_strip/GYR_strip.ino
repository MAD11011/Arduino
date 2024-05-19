
const uint8_t green = 3;
const uint8_t yellow = 5;
const uint8_t red = 6;



void setup() {
  // put your setup code here, to run once:
  pinMode(green,OUTPUT);
  pinMode(yellow,OUTPUT);
  pinMode(red,OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(green,HIGH);
  delay(1000);
  digitalWrite(green,LOW);
  digitalWrite(yellow,HIGH);
  delay(1000);
  digitalWrite(yellow,LOW);
  digitalWrite(red,HIGH);
  delay(1000);
  digitalWrite(red,LOW);

}
