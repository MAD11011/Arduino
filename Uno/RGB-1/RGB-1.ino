const uint8_t red = 10;
const uint8_t green = 11;
const uint8_t blue = 12;

void setup() {
  // put your setup code here, to run once:
  pinMode(red,OUTPUT);
  pinMode(green,OUTPUT);
  pinMode(blue,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(red,HIGH);
  delay(1000);
  digitalWrite(red,LOW);
  digitalWrite(green,HIGH);
  delay(1000);
  digitalWrite(green,LOW);
  digitalWrite(blue,HIGH);
  delay(1000);
  digitalWrite(blue,LOW);




}
