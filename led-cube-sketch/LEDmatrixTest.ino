#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

const int PIN = 4;
const int NUMPIXELS = 25;

//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(5, 5, 4,
  NEO_MATRIX_BOTTOM  + NEO_MATRIX_RIGHT +  //NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

struct RGB {
  byte r;
  byte g;
  byte b;
};

// Define some colors we'll use frequently
RGB w = { 255, 255, 255 };
RGB r = { 255,   0,   0 };
RGB g = { 0,   255,   0 };
RGB b = { 0,     0, 255 };
RGB o = { 0,     0,   0 };

RGB number1[] = {
  o, o, w, o, o,
  o, w, w, o, o,
  o, o, w, o, o,
  o, o, w, o, o,
  o, w, w, w, o
};

RGB number2[] = {
  o, w, w, w, o,
  o, o, o, w, o,
  o, o, w, o, o,
  o, w, o, o, o,
  o, w, w, w, o
};

RGB number3[] = {
  o, w, w, w, o,
  o, o, o, w, o,
  o, o, w, w, o,
  o, o, o, w, o,
  o, w, w, w, o
};

RGB number4[] = {
  o, w, o, w, o,
  o, w, o, w, o,
  o, w, w, w, o,
  o, o, o, w, o,
  o, o, o, w, o
};

RGB number5[] = {
  o, w, w, w, o,
  o, w, o, o, o,
  o, w, w, o, o,
  o, o, o, w, o,
  o, w, w, w, o
};

RGB number6[] = {
  o, w, w, w, o,
  o, w, o, o, o,
  o, w, w, w, o,
  o, w, o, w, o,
  o, w, w, w, o
};

RGB test[] = {

  r, r, o, g, g,
  r, o, o, o, g,
  o, o, o, o, o,
  b, o, o, o, w,
  b, b, o, w, w
  
};


void setup() {
  //pixels.begin();
  matrix.begin();
  matrix.setBrightness(30);
  Serial.begin(9600);
}

void loop() {
  int counter = 0;
  int counter2 = 0;
  for(int i = 0; i <6; i++){
    for(int x = 0; x < 5; x++){
      for(int y = 0; y < 5; y++){
        //pixels.setPixelColor(i, pixels.Color(number1[i][0],number1[i][1],number1[i][2]));
        switch (counter2){
          case 0:
            matrix.drawPixel(x, y, matrix.Color(number1[counter].r, number1[counter].g, number1[counter].b));
          case 1:
            matrix.drawPixel(x, y, matrix.Color(number2[counter].r, number2[counter].g, number2[counter].b));
          case 2:
            matrix.drawPixel(x, y, matrix.Color(number3[counter].r, number3[counter].g, number3[counter].b));
          case 3:
            matrix.drawPixel(x, y, matrix.Color(number4[counter].r, number4[counter].g, number4[counter].b));
          case 4:
            matrix.drawPixel(x, y, matrix.Color(number5[counter].r, number5[counter].g, number5[counter].b));
          case 5:
            matrix.drawPixel(x, y, matrix.Color(number6[counter].r, number6[counter].g, number6[counter].b));
        }
        matrix.show();
        counter++;
      }
    }
    counter2++;
    delay(1000);
  }
  //pixels.show();
}
