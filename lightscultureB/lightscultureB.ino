#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN        4
#define NUM_LEDS   64
#define BRIGHTNESS 100

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

byte Rarray[8] = { 000, 000, 000, 000, 000, 000, 000, 000 };
byte Garray[8] = { 000, 000, 000, 000, 000, 000, 000, 000 };
byte Barray[8] = { 000, 000, 000, 000, 000, 000, 000, 000 };

void setup() {

  randomSeed(analogRead(0));
  setColorArray();

  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  interpRotate(random(512,2048),random(25));
}

void loop() {
  
  byte  RA,GA,BA,RB,GB,BB;
  float scale;

  static unsigned char  barA     = 0;
  static unsigned int   cycleCnt = 0;
  static byte           state    = 0;
  static float          mscale   = 1.0;
  static unsigned short dd       = random(10);
  static byte           cycs     = random(10,50);
  static byte           cycfade  = random(10,80);
  static byte           cycdur   = random(10,40);
    

  for ( byte j=0; j<cycs; j++ ){
    scale = j/((cycs-1)*1.0);
    for ( byte k=0; k<8; k++ ){
      RA=Rarray[(barA+k)%8];
      GA=Garray[(barA+k)%8];
      BA=Barray[(barA+k)%8];
      RB=Rarray[(barA+k+1)%8];
      GB=Garray[(barA+k+1)%8];
      BB=Barray[(barA+k+1)%8];
        for( byte i=0; i<8; i++ ) {
            strip.setPixelColor(i+k*8, strip.Color( (mscale*(RB*(scale)+RA*(1.0-scale))), (mscale*(GB*(scale)+GA*(1.0-scale))), (mscale*(BB*(scale)+BA*(1.0-scale))) ));      
        }
    }
    strip.show();
    delay(dd);
  }


  if ((7==barA)){
    barA= -1;
  }


  barA     += 1;
  cycleCnt += 1;


  //State machine
  //-----------------------------------
  switch(state){
    
    case 0:
      if (cycleCnt>cycdur){
        state    = 1;
        cycleCnt = 0;
      }
      break;
      
    case 1:
      if (cycleCnt<cycfade){
        mscale = ((1.0*cycfade-1.0)-cycleCnt)/(1.0*cycfade-1.0);
      }else{
        state    = 2;
        cycleCnt = 0;
        dd       = random(10);
        cycs     = random(10,50);
        cycfade  = random(10,80);
        cycdur   = random(10,40);
        setColorArray();

        switch (random()%15){
          case 0:
            rainbowCycle(10);
            break;
          case 1:
            randomizer(random(150,300));
            break;
          case 2:
            colorWipe(strip.Color(random(255),random(255),random(255)),60);
            colorWipe(strip.Color(0,0,0),60);
            break;
          case 3:
            pointFade(random(100,300),random(30,150));
            break;
          case 4:
            bounce(random(100,300),random(30,100));
            break;
          case 5:
            dblBounce(random(100,300),random(30,100));
            break;
          case 6:
            scan(random(100,300),random(30,100));
            break;
          case 7:
            scanTwo(random(100,300),random(30,100));
            break;
          case 8:
            interpCycle(random(768,2400),random(4));
            break;
          case 9:
            interpRotate(random(512,1600),random(25));
            break;
          case 10:
            circles(random(2,5),random(4));
            break;
        }
        
      }
      break;
      
    case 2:
      if (cycleCnt<cycfade){
        mscale = cycleCnt/(1.0*cycfade-1.0);        
      } else {
        state    = 0;
        cycleCnt = 0;
      }
      break;      
  }
  //-----------------------------------


}


// ---------------------------------------------------------------------
//Animation Functions
// ---------------------------------------------------------------------

void makeColorA(byte *RR, byte *GG, byte *BB){
  byte  R,G,B;
  short summ;
  float scale;

  R     = random();
  G     = random();
  B     = random();
  summ  = (R+G+B);
  if (summ>0){
    scale = 255.0/summ;
    R = R*scale;
    G = G*scale;
    B = B*scale;
  } else {
    R=255;
    G=255;
    B=255;
  }
  *RR = R;
  *GG = G;
  *BB = B;
}


void makeColorB(byte *RR, byte *GG, byte *BB){
  byte  R,G,B,mn,mx;
  short summ;
  float scale;

  R     = random();
  G     = random();
  B     = random();
  summ  = (R+G+B);
  if (summ>0){
    mn = min(min(R,G),B);
    mx = max(max(R,G),B);
    scale = 255.0/(mx-mn);
    R = (R-mn)*scale;
    G = (G-mn)*scale;
    B = (B-mn)*scale;
  } else {
    R=255;
    G=255;
    B=255;
  }
  *RR = R;
  *GG = G;
  *BB = B;
}


void setColorArray(){
  
  byte  R,G,B;
  short summ;
  float scale;
  byte  lim;
  
  lim = random()%6+2;
  for (byte i=0;i<8;i++){
    makeColorA(&R,&G,&B);
    if (i<lim){
      scale = ((lim-1.0)-i)/(lim-1.0);
    } else{
      scale= 0.0;
    }
    R = R*scale;
    G = G*scale;
    B = B*scale;
    Rarray[i]= R;    
    Garray[i]= G;    
    Barray[i]= B;    
  }
  
}

void circles(short runs, short wait){
  byte xo,yo;
  float x,y;
  float cvt=3.14/180.0;
  short xi,yi;
  byte R,G,B;
  float scale;


  for (short l=0; l<runs; l++){
    xo = random()%8;
    yo = random()%8;
    for (byte k=0; k<6; k++){
      makeColorB(&R,&G,&B);
      for (byte cyc=0; cyc<8;cyc++){
        scale = cyc/7.0;
        for (short ang=0;ang<360;ang++){
          x = cos(ang*cvt)*k*1.5 + xo;
          y = sin(ang*cvt)*k*1.5 + yo;
          xi = (int(x)%8);
          yi = (int(y)%8);
          if (xi<0) xi+=8;
          if (yi<0) yi+=8;
          strip.setPixelColor(xi+yi*8, strip.Color(R*scale,G*scale,B*scale) );        
        }
        strip.show();
        delay(wait);      
      }
    }
    for (byte i=0; i<64; i++){
      strip.setPixelColor(i, strip.Color(0,0,0) );              
    }
    strip.show();
  }
}

void randomizer(short wait){
  for(short j=0;j<30;j++){
    for(byte i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(random(255),random(255),random(255)) );
    }
    strip.show();
    delay(wait);
  }
}


void pointFade(short count, short wait){

  byte  Rdisp[NUM_LEDS];
  byte  Gdisp[NUM_LEDS];
  byte  Bdisp[NUM_LEDS];
  byte  point;
  byte  R,G,B;
  short summ;
  float scale;
  float fade;

  memset(Rdisp,0,NUM_LEDS);
  memset(Gdisp,0,NUM_LEDS);
  memset(Bdisp,0,NUM_LEDS);

  fade = 0.5 + random(46)/100.0;

  for ( short k=0; k<count; k++ ){
    for ( byte i=0; i<NUM_LEDS; i++ ){
      Rdisp[i] = fade*Rdisp[i];
      Gdisp[i] = fade*Gdisp[i];
      Bdisp[i] = fade*Bdisp[i];
    }
    point = random()%NUM_LEDS;
    makeColorB(&R,&G,&B);
    Rdisp[point] = R;
    Gdisp[point] = G;
    Bdisp[point] = B;

    for ( byte i=0; i<NUM_LEDS; i++ ){
      strip.setPixelColor(i, strip.Color(Rdisp[i],Gdisp[i],Bdisp[i]));
    }
    strip.show();
    delay(wait);
    
  }

}


void scan(short count, short wait){

  byte  Rdisp[64];
  byte  Gdisp[64];
  byte  Bdisp[64];
  byte  R,G,B;
  short summ;
  float scale;
  float fade;
  
  signed char x = random()%8;
  byte        xdir = random()%2;

  fade = 0.5 + random(36)/100.0;

  memset(Rdisp,0,64);
  memset(Gdisp,0,64);
  memset(Bdisp,0,64);

  makeColorB(&R,&G,&B);

  for ( short k=0; k<count; k++ ){
    for ( byte i=0; i<64; i++ ){
      Rdisp[i] = fade*Rdisp[i];
      Gdisp[i] = fade*Gdisp[i];
      Bdisp[i] = fade*Bdisp[i];
    }

    for ( byte j=0; j<8; j++ ){
      Rdisp[j*8+x] = R;
      Gdisp[j*8+x] = G;
      Bdisp[j*8+x] = B;      
    }

    if (xdir){
      if (7==x){
        xdir =  0;
        x    -= 1;
      } else {
        x += 1;
      }
    } else {
      if (0==x){
        xdir =  1;
        x    += 1;
      } else {
        x -= 1;
      }      
    }

    for ( byte i=0; i<NUM_LEDS; i++ ){
      strip.setPixelColor(i, strip.Color(Rdisp[i],Gdisp[i],Bdisp[i]));
    }
    strip.show();
    delay(wait);
    
  }

}


void scanTwo(short count, short wait){

  byte  Rdisp[64];
  byte  Gdisp[64];
  byte  Bdisp[64];
  byte  R,G,B;
  short summ;
  float scale;
  float fade;
  
  signed char x = random()%8;
  byte        xdir = random()%2;

  fade = 0.5 + random(36)/100.0;

  memset(Rdisp,0,64);
  memset(Gdisp,0,64);
  memset(Bdisp,0,64);

  makeColorB(&R,&G,&B);

  for ( short k=0; k<count; k++ ){
    for ( byte i=0; i<64; i++ ){
      Rdisp[i] = fade*Rdisp[i];
      Gdisp[i] = fade*Gdisp[i];
      Bdisp[i] = fade*Bdisp[i];
    }

    for ( byte j=0; j<8; j++ ){
      Rdisp[j*8+x] = R;
      Gdisp[j*8+x] = G;
      Bdisp[j*8+x] = B;      
    }

    for ( byte j=0; j<8; j++ ){
      Rdisp[x*8+j] = B;
      Gdisp[x*8+j] = G;
      Bdisp[x*8+j] = R;      
    }

    if (xdir){
      if (7==x){
        xdir =  0;
        x    -= 1;
      } else {
        x += 1;
      }
    } else {
      if (0==x){
        xdir =  1;
        x    += 1;
      } else {
        x -= 1;
      }      
    }

    for ( byte i=0; i<NUM_LEDS; i++ ){
      strip.setPixelColor(i, strip.Color(Rdisp[i],Gdisp[i],Bdisp[i]));
    }
    strip.show();
    delay(wait);
    
  }

}


void interpCycle(short count, short wait){
  byte  R[4],G[4],B[4];
  byte  Rp,Gp,Bp;
  short summ;
  float scale;
  float x,y,dx,dy;  

  for (byte k=0; k<4; k++){
    makeColorB(&R[k],&G[k],&B[k]);
  }

  for (short cyc=0; cyc<count; cyc+=4){
    for ( byte j=0; j<8; j++){
      y  = j/8.0;
      dy = 1.0-y;
      for ( byte i=0; i<8; i++){
        x  = i/8.0;
        dx = 1.0-x;
        Rp = R[0]*dx*dy + R[1]*x*dy + R[2]*dx*y + R[3]*x*y;
        Gp = G[0]*dx*dy + G[1]*x*dy + G[2]*dx*y + G[3]*x*y;
        Bp = B[0]*dx*dy + B[1]*x*dy + B[2]*dx*y + B[3]*x*y;
        Rp = (Rp+cyc)%256;
        Gp = (Gp+cyc)%256;
        Bp = (Bp+cyc)%256;
        strip.setPixelColor(j*8+i, strip.Color(Rp,Gp,Bp));
      }
    }
    strip.show();
    delay(wait);
  }

}



void interpRotate(short count, short wait){
  byte  R[4],G[4],B[4];
  byte  Ro[4],Go[4],Bo[4];
  byte  Rp,Gp,Bp;
  short summ;
  float scale;
  float track = 0.0;
  float x,y,dx,dy;
  byte  idxA, idxB; 
  byte  iMap[4] = {0,1,3,2}; 

  for (byte k=0; k<4; k++){
    makeColorB(&R[k],&G[k],&B[k]);
  }

  R[0] = 0;
  G[0] = 0;
  B[0] = 0;

  if (random()%10 < 6){

    R[1] = 255;
    G[1] = 0;
    B[1] = 0;
    
    R[2] = 0;
    G[2] = 255;
    B[2] = 0;
  
    R[3] = 0;
    G[3] = 0;
    B[3] = 255;

  }

  for (byte k=0; k<4; k++){
    Ro[k]  = R[k];
    Go[k]  = G[k];
    Bo[k]  = B[k];
  }

  for (short cyc=0; cyc<count; cyc+=4){
    for ( byte j=0; j<8; j++){
      y  = j/8.0;
      dy = 1.0-y;
      for ( byte i=0; i<8; i++){
        x  = i/8.0;
        dx = 1.0-x;
        Rp = R[0]*dx*dy + R[1]*x*dy + R[2]*dx*y + R[3]*x*y;
        Gp = G[0]*dx*dy + G[1]*x*dy + G[2]*dx*y + G[3]*x*y;
        Bp = B[0]*dx*dy + B[1]*x*dy + B[2]*dx*y + B[3]*x*y;
        strip.setPixelColor(j*8+i, strip.Color(Rp,Gp,Bp));
      }
    }
    strip.show();
    delay(wait);

    track += 1.0/32.0;
    scale = track - int(track);

    for (byte k=0; k<4; k++){
      idxA = int(track+k)%4;
      idxB = (idxA+1)%4;
      R[iMap[k]] = (1.0-scale)*Ro[idxA] + scale*Ro[idxB];
      G[iMap[k]] = (1.0-scale)*Go[idxA] + scale*Go[idxB];
      B[iMap[k]] = (1.0-scale)*Bo[idxA] + scale*Bo[idxB];
    }
    
  }

}


void bounce(short count, short wait){

  byte  Rdisp[64];
  byte  Gdisp[64];
  byte  Bdisp[64];
  byte  R,G,B;
  short summ;
  float scale;
  float fade;
  
  signed char x = random()%8;
  signed char y = random()%8;
  byte        xdir = random()%2;
  byte        ydir = random()%2;

  fade = 0.5 + random(46)/100.0;

  memset(Rdisp,0,64);
  memset(Gdisp,0,64);
  memset(Bdisp,0,64);

  makeColorB(&R,&G,&B);

  for ( short k=0; k<count; k++ ){
    for ( byte i=0; i<64; i++ ){
      Rdisp[i] = fade*Rdisp[i];
      Gdisp[i] = fade*Gdisp[i];
      Bdisp[i] = fade*Bdisp[i];
    }
        
    Rdisp[y*8+x] = R;
    Gdisp[y*8+x] = G;
    Bdisp[y*8+x] = B;

    if (xdir){
      if (7==x){
        xdir =  0;
        x    -= 1;
      } else {
        x += 1;
      }
    } else {
      if (0==x){
        xdir =  1;
        x    += 1;
      } else {
        x -= 1;
      }      
    }

    if (ydir){
      if (7==y){
        ydir =  0;
        y    -= 1;
      } else {
        y += 1;
      }
    } else {
      if (0==y){
        ydir =  1;
        y    += 1;
      } else {
        y -= 1;
      }      
    }

    for ( byte i=0; i<NUM_LEDS; i++ ){
      strip.setPixelColor(i, strip.Color(Rdisp[i],Gdisp[i],Bdisp[i]));
    }
    strip.show();
    delay(wait);
    
  }

}


void dblBounce(short count, short wait){

  byte  Rdisp[64];
  byte  Gdisp[64];
  byte  Bdisp[64];
  byte  R,G,B;
  short summ;
  float scale;
  float fade;
  
  signed char x = random()%8;
  signed char y = random()%8;
  byte        xdir = random()%2;
  byte        ydir = random()%2;

  signed char xa = random()%8;
  signed char ya = random()%8;
  byte        xadir = random()%2;
  byte        yadir = random()%2;

  fade = 0.5 + random(46)/100.0;

  memset(Rdisp,0,64);
  memset(Gdisp,0,64);
  memset(Bdisp,0,64);

  makeColorB(&R,&G,&B);

  for ( short k=0; k<count; k++ ){
    for ( byte i=0; i<64; i++ ){
      Rdisp[i] = fade*Rdisp[i];
      Gdisp[i] = fade*Gdisp[i];
      Bdisp[i] = fade*Bdisp[i];
    }
        
    Rdisp[y*8+x] = R;
    Gdisp[y*8+x] = G;
    Bdisp[y*8+x] = B;

    Rdisp[ya*8+xa] = B;
    Gdisp[ya*8+xa] = G;
    Bdisp[ya*8+xa] = R;

    if (xdir){
      if (7==x){
        xdir =  0;
        x    -= 1;
      } else {
        x += 1;
      }
    } else {
      if (0==x){
        xdir =  1;
        x    += 1;
      } else {
        x -= 1;
      }      
    }

    if (ydir){
      if (7==y){
        ydir =  0;
        y    -= 1;
      } else {
        y += 1;
      }
    } else {
      if (0==y){
        ydir =  1;
        y    += 1;
      } else {
        y -= 1;
      }      
    }

    if (xadir){
      if (7==xa){
        xadir =  0;
        xa    -= 1;
      } else {
        xa += 1;
      }
    } else {
      if (0==xa){
        xadir =  1;
        xa    += 1;
      } else {
        xa -= 1;
      }      
    }

    if (yadir){
      if (7==ya){
        yadir =  0;
        ya    -= 1;
      } else {
        ya += 1;
      }
    } else {
      if (0==ya){
        yadir =  1;
        ya    += 1;
      } else {
        ya -= 1;
      }      
    }


    for ( byte i=0; i<NUM_LEDS; i++ ){
      strip.setPixelColor(i, strip.Color(Rdisp[i],Gdisp[i],Bdisp[i]));
    }
    strip.show();
    delay(wait);
    
  }

}


// ---------------------------------------------------------------------
// Code included after this section is credited to Adarfruit industries.
// ---------------------------------------------------------------------

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256 * 5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3,0);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3,0);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0,0);
}

uint8_t red(uint32_t c) {
  return (c >> 16);
}
uint8_t green(uint32_t c) {
  return (c >> 8);
}
uint8_t blue(uint32_t c) {
  return (c);
}

