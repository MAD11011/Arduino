const uint8_t red = 10;
const uint8_t green = 11;
const uint8_t blue = 12;

struct RgbColor
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} rgbColor;

struct HsvColor
{
    unsigned char h;
    unsigned char s;
    unsigned char v;
    HsvColor(uint8_t _h,uint8_t _s,uint8_t _v){
      h = _h;
      s = _s;
      v = _v;
    }
} ;


unsigned int val = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(red,OUTPUT);
  pinMode(green,OUTPUT);
  pinMode(blue,OUTPUT);

  analogWrite(red,0);
  analogWrite(green,0);
  analogWrite(blue,0);

  
}

void loop() {
  // put your main code here, to run repeatedly:
    

  
  val = (val == 255) ? 0 : val;
  delay(50);
  val++;
  rgbColor = HsvToRgb(HsvColor(val,200,255));

  analogWrite(red,rgbColor.r);
  analogWrite(green,rgbColor.g);
  analogWrite(blue,rgbColor.b);
   
  
}



RgbColor HsvToRgb(HsvColor hsv)
{
    RgbColor rgb;
    unsigned char region, remainder, p, q, t;
    
    if (hsv.s == 0)
    {
        rgb.r = hsv.v;
        rgb.g = hsv.v;
        rgb.b = hsv.v;
        return rgb;
    }
    
    region = hsv.h / 43;
    remainder = (hsv.h - (region * 43)) * 6; 
    
    p = (hsv.v * (255 - hsv.s)) >> 8;
    q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
    t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;
    
    switch (region)
    {
        case 0:
            rgb.r = hsv.v; rgb.g = t; rgb.b = p;
            break;
        case 1:
            rgb.r = q; rgb.g = hsv.v; rgb.b = p;
            break;
        case 2:
            rgb.r = p; rgb.g = hsv.v; rgb.b = t;
            break;
        case 3:
            rgb.r = p; rgb.g = q; rgb.b = hsv.v;
            break;
        case 4:
            rgb.r = t; rgb.g = p; rgb.b = hsv.v;
            break;
      
    }
    return rgb;
}



