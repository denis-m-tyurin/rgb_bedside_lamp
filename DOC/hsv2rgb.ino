char iii;
void loop(){}

void setup()
{
  iii = 4;
  uint8_t r,g,b;
  UBRR0L = 16;UBRR0H = 0; //115200
  //UBRR0L = 34;UBRR0H = 0; //57600
  UCSR0A = ( 1<<U2X0 ); //UART_DOUBLESPEED
  UCSR0C =((1<<UCSZ01) | (1<<UCSZ00)); //Set Frame Format
  UCSR0B = ((1<<TXEN0) | (1<<RXEN0)); //Enable The receiver and transmitter 
  uint32_t tstart;
  
  
  tstart = millis();
  for(uint8_t j=0;j<50;j++)
  for(uint16_t i=0;i<360;i++)
  {
    HSV2RGB_Adv2(r,g,b,i,239,221);
  }
  Serial.println(millis()-tstart,DEC);
 
  tstart = millis();
  for(uint8_t j=0;j<50;j++)
  for(uint16_t i=0;i<1536;i++)
  {
    HSV2RGB_Adv1(r,g,b,i,239,221);
  }
  Serial.println(millis()-tstart,DEC);

  tstart = millis();
  for(uint16_t j=0;j<50*iii;j++)
  for(uint16_t i=0;i<1536;i+=iii)
  {
    HSV2RGB_Adv1(r,g,b,i,239,221);
  }
  Serial.println(millis()-tstart,DEC);

  tstart = millis();
//  uint32_t divider = pow(255,2.2)/255;
  for(uint8_t j=0;j<50;j++)
  for(uint16_t i=0;i<360;i++)
  {
    HSV2RGB_Binary(r,g,b,i,239,221);
  }
  Serial.println(millis()-tstart,DEC);
  tstart = millis();

}


void HSV2RGB_360 (unsigned char &r, unsigned char &g, unsigned char &b,
unsigned int i_hue, unsigned char sat, unsigned char val)
/* Вход:
* 0 <= hue < 360 градусов - оттенок. Основные цвета:
* 0 - красный, 60 - желтый,
* 120 - зеленый, 180 - голубой
* 240 - синий, 300 - пурпурный
* hue == 360 - неопределён !!!
* Остальные цвета между ними
* 0 <= sat <= 255 - Saturation - насыщенность
* 0 <= val <= 255 - Value - светлота
*
* Выход:
* 0 <= r,g,b <= 255 - значения красного, зеленого, синего
*/
  {
    int ii;
    double fr,hue ;
    unsigned char c1, c2, c3;
    if (!sat)
      r = g = b = val;
    else
      { /* Хроматический цвет */
        while (i_hue >= 360)
          i_hue -= 360;
        hue = i_hue;
        ii = (int)(hue /= 60.0);
        //ii = (hue >>6);
        fr = hue - ii;
        c1 = (val * (255 - sat)) / 255;
        c2 = (val * (255 - sat * fr)) / 255;
        c3 = (val * (255 - sat * (1.0 - fr))) / 255;
        switch (ii)
          {
            case 0: r = val; g = c3; b = c1; break;
            case 1: r = c2; g = val; b = c1; break;
            case 2: r = c1; g = val; b = c3; break;
            case 3: r = c1; g = c2; b = val; break;
            case 4: r = c3; g = c1; b = val; break;
            case 5: r = val; g = c1; b = c2; break;
          } // switch
      } // if (!sat)
  } // HSV 2 RGB

void HSV2RGB_384 (unsigned char &r, unsigned char &g, unsigned char &b,
unsigned int i_hue, unsigned char sat, unsigned char val)
/* Вход:
* 0 <= hue < 360 градусов - оттенок. Основные цвета:
* 0 - красный, 60 - желтый,
* 120 - зеленый, 180 - голубой
* 240 - синий, 300 - пурпурный
* hue == 360 - неопределён !!!
* Остальные цвета между ними
* 0 <= sat <= 255 - Saturation - насыщенность
* 0 <= val <= 255 - Value - светлота
*
* Выход:
* 0 <= r,g,b <= 255 - значения красного, зеленого, синего
*/
  {
    int ii;
    double fr,hue ;
    unsigned char c1, c2, c3;
    if (!sat)
      r = g = b = val;
    else
      { /* Хроматический цвет */
        while (i_hue >= 360)
          i_hue -= 360;
        hue = i_hue;
        ii = (int)(hue /= 60.0);
        //ii = (hue >>6);
        fr = hue - ii;
        c1 = (val * (255 - sat)) / 255;
        c2 = (val * (255 - sat * fr)) / 255;
        c3 = (val * (255 - sat * (1.0 - fr))) / 255;
        switch (ii)
          {
            case 0: r = val; g = c3; b = c1; break;
            case 1: r = c2; g = val; b = c1; break;
            case 2: r = c1; g = val; b = c3; break;
            case 3: r = c1; g = c2; b = val; break;
            case 4: r = c3; g = c1; b = val; break;
            case 5: r = val; g = c1; b = c2; break;
          } // switch
      } // if (!sat)
  } // HSV 2 RGB

void HSV2RGB_Binary (unsigned char &r, unsigned char &g, unsigned char &b,
unsigned int i_hue, unsigned char sat, unsigned char val)
  {
    unsigned char c1, c2, c3,ii,fr;
    uint16_t hue;
    if (!sat)
      r = g = b = val;
    else
      { 
        while (i_hue >= 384)
          i_hue -= 384;
        hue = i_hue;
        ii = (hue >>6);//ii = (int)(hue /= 64.0);
        fr = hue - (ii<<6);//fr = hue - ii;
        c1 = (val * (255 - sat)) / 255;
        //c2 = (val * (255 - sat * fr)) / 255;
        //c3 = (val * (255 - sat * (1.0 - fr))) / 255;
        c2 = (val * (255 - sat*fr/63)) / 255;
        c3 = (val * (255 - sat*(63-fr)/127)) / 255;
        switch (ii)
          {
            case 0: r = val; g = c3; b = c1; break;
            case 1: r = c2; g = val; b = c1; break;
            case 2: r = c1; g = val; b = c3; break;
            case 3: r = c1; g = c2; b = val; break;
            case 4: r = c3; g = c1; b = val; break;
            case 5: r = val; g = c1; b = c2; break;
          } // switch
      } // if (!sat)
  } // HSV 2 RGB Binary

void HSV2RGB_Adv (unsigned char &r, unsigned char &g, unsigned char &b,
unsigned int hue, unsigned char sat, unsigned char val)
{
  if (!sat) { // Acromatic color (gray). Hue doesn't mind.
    r=g=b=val;
  } else  { 

    
    //hue = hue%384;
    uint8_t base = ((255 - sat) * val)>>8;

    switch(hue>>8) {
    case 0:
        r = val;
        g = (((val-base)*hue)/256)+base;
        b = base;
    break;

    case 1:
        r = (((val-base)*(256-(hue%256)))/256)+base;
        g = val;
        b = base;
    break;

    case 2:
        r = base;
        g = val;
        b = (((val-base)*(hue%256))/256)+base;
    break;

    case 3:
        r = base;
        g = (((val-base)*(256-(hue%256)))/256)+base;
        b = val;
    break;

    case 4:
        r = (((val-base)*(hue%256))/256)+base;
        g = base;
        b = val;
    break;

    case 5:
        r = val;
        g = base;
        b = (((val-base)*(256-(hue%256)))/256)+base;
    break;
    }
  }   
 } // HSV 2 RGB Binary
 
void HSV2RGB_Adv1 (unsigned char &r, unsigned char &g, unsigned char &b,
uint16_t hue, unsigned char sat, unsigned char val)
{
  if (!sat) { // Acromatic color (gray). Hue doesn't mind.
    r=g=b=val;
  } else  { 

    
    //hue %= 1536;
    uint8_t base = ((255 - sat) * val)>>8;
    uint8_t iiii= hue/256;
    switch(iiii) {
    case 0:
        r = val;
        g = (((val-base)*hue)/256)+base;
        b = base;
    break;

    case 1:
        r = (((val-base)*(256-(hue%256)))/256)+base;
        g = val;
        b = base;
    break;

    case 2:
        r = base;
        g = val;
        b = (((val-base)*(hue%256))/256)+base;
    break;

    case 3:
        r = base;
        g = (((val-base)*(256-(hue%256)))/256)+base;
        b = val;
    break;

    case 4:
        r = (((val-base)*(hue%256))/256)+base;
        g = base;
        b = val;
    break;

    case 5:
        r = val;
        g = base;
        b = (((val-base)*(256-(hue%256)))/256)+base;
    break;
    }
  }   
 } // HSV 2 RGB Binary


void HSV2RGB_Adv2 (unsigned char &r, unsigned char &g, unsigned char &b,
unsigned int hue, unsigned char sat, unsigned char val)
{
  if (sat == 0) { // Acromatic color (gray). Hue doesn't mind.
    r=g=b=val;
  } else  { 

    uint8_t base = ((255 - sat) * val)>>8;

    switch(hue/60) {
    case 0:
        r = val;
        g = (((val-base)*hue)/60)+base;
        b = base;
    break;

    case 1:
        r = (((val-base)*(60-(hue%60)))/60)+base;
        g = val;
        b = base;
    break;

    case 2:
        r = base;
        g = val;
        b = (((val-base)*(hue%60))/60)+base;
    break;

    case 3:
        r = base;
        g = (((val-base)*(60-(hue%60)))/60)+base;
        b = val;
    break;

    case 4:
        r = (((val-base)*(hue%60))/60)+base;
        g = base;
        b = val;
    break;

    case 5:
        r = val;
        g = base;
        b = (((val-base)*(60-(hue%60)))/60)+base;
    break;
    }
  }   
 } // HSV 2 RGB Binary

