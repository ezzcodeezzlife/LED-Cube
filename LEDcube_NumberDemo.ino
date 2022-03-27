#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

const int PIN1 = 18;
const int PIN2 = 19;
const int PIN3 = 23;
const int PIN4 = 33;
const int PIN5 = 5;
const int PIN6 = 26;
const int NUMPIXELS = 25;

unsigned long previousMillis = 0; 
const long interval = 2000; 

//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

Adafruit_NeoMatrix matrix1 = Adafruit_NeoMatrix(5, 5, PIN1,
  NEO_MATRIX_BOTTOM  + NEO_MATRIX_RIGHT +  //NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

Adafruit_NeoMatrix matrix2 = Adafruit_NeoMatrix(5, 5, PIN2,
  NEO_MATRIX_BOTTOM  + NEO_MATRIX_RIGHT +  //NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

Adafruit_NeoMatrix matrix3 = Adafruit_NeoMatrix(5, 5, PIN3,
  NEO_MATRIX_BOTTOM  + NEO_MATRIX_RIGHT +  //NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

Adafruit_NeoMatrix matrix4 = Adafruit_NeoMatrix(5, 5, PIN4,
  NEO_MATRIX_BOTTOM  + NEO_MATRIX_RIGHT +  //NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

Adafruit_NeoMatrix matrix5 = Adafruit_NeoMatrix(5, 5, PIN5,
  NEO_MATRIX_BOTTOM  + NEO_MATRIX_RIGHT +  //NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

Adafruit_NeoMatrix matrix6 = Adafruit_NeoMatrix(5, 5, PIN6,
  NEO_MATRIX_BOTTOM  + NEO_MATRIX_RIGHT +  //NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);


struct RGB {
  byte r;
  byte g;
  byte b;
};

RGB w = { 255, 255, 255 }; //white
RGB r = { 255,   0,   0 }; //red
RGB g = { 0,   255,   0 }; //green
RGB b = { 0,     0, 255 }; //blue
RGB y = { 255, 255,   0 }; //yellow
RGB m = { 255,   0, 255 }; //magenta
RGB c = { 0,   255, 150 }; //cyan
RGB o = { 0,     0,   0 }; //off

RGB n1[5][5] = {
  {o, o, o, o, o},
  {o, o, o, o, o},
  {o, o, g, o, o},
  {o, o, o, o, o},
  {o, o, o, o, o},
};

RGB n2[5][5] = {
  {o, o, o, o, o},
  {o, y, o, o, o},
  {o, o, o, o, o},
  {o, o, o, y, o},
  {o, o, o, o, o},
};

RGB n3[5][5] = {
  {c, o, o, o, o},
  {o, o, o, o, o},
  {o, o, c, o, o},
  {o, o, o, o, o},
  {o, o, o, o, c},
};

RGB n4[5][5] = {
  {o, o, o, o, o},
  {o, b, o, b, o},
  {o, o, o, o, o},
  {o, b, o, b, o},
  {o, o, o, o, o},
};

RGB n5[5][5] = {
  {m, o, o, o, m},
  {o, o, o, o, o},
  {o, o, m, o, o},
  {o, o, o, o, o},
  {m, o, o, o, m},
};

RGB n6[5][5] = {
  {r, o, o, o, r},
  {o, o, o, o, o},
  {r, o, o, o, r},
  {o, o, o, o, o},
  {r, o, o, o, r},
};


void wait(int milliseconds){
  unsigned long currentMillis = millis();
  unsigned long waitMillis = currentMillis + milliseconds;
  boolean waitComplete = false;
  while(!waitComplete){
    if (millis() >= waitMillis) {
      waitComplete = true;
    }
  }
}

void render(RGB frameArray[5][5]){
  for(int x = 0; x < 5; x++){
    for(int y = 0; y < 5; y++){
      uint32_t color = matrix1.Color(frameArray[x][y].r, frameArray[x][y].g, frameArray[x][y].b);
      matrix1.drawPixel(x, y, color);
      matrix2.drawPixel(x, y, color);
      matrix3.drawPixel(x, y, color);
      matrix4.drawPixel(x, y, color);
      matrix5.drawPixel(x, y, color);
      matrix6.drawPixel(x, y, color);
    }
  }
  matrix1.show();
  matrix2.show();
  matrix3.show();
  matrix4.show();
  matrix5.show();
  matrix6.show();
}


void setup() {
  matrix1.begin();
  matrix1.setBrightness(10);
  matrix2.begin();
  matrix2.setBrightness(10);
  matrix3.begin();
  matrix3.setBrightness(10);
  matrix4.begin();
  matrix4.setBrightness(10);
  matrix5.begin();
  matrix5.setBrightness(10);
  matrix6.begin();
  matrix6.setBrightness(10);
  Serial.begin(9600);
}

void loop() {
  render(n1);
  wait(500);
  render(n2);
  wait(500);
  render(n3);
  wait(500);
  render(n4);
  wait(500);
  render(n5);
  wait(500);
  render(n6);
  wait(500);
 
}
