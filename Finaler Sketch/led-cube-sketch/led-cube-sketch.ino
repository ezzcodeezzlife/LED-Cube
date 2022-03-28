#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Arduino_JSON.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include "SPIFFS.h"

// Replace with your network credentials
const char* ssid = "LEDCube";
const char* password = "ledcube!";

// PINS
const int PIN1 = 18;
const int PIN2 = 19;
const int PIN3 = 23;
const int PIN4 = 33;
const int PIN5 = 5;
const int PIN6 = 26;
const int NUMPIXELS = 25;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create an Event Source on /events
AsyncEventSource events("/events");

// Json Variable to Hold Sensor Readings
JSONVar readings;

// Timer variables
unsigned long lastTime = 0;  
unsigned long lastTimeAcc = 0;
unsigned long movingStartTime = 0;
unsigned long movingStopTime = 0;
unsigned long gyroDelay = 10;
unsigned long accelerometerDelay = 200;
unsigned long movingStartDelay = 1000;
unsigned long movingStopDelay = 3000;

// MPU6050 object
Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;

/// Readings
float gyroX, gyroY, gyroZ;
float accX, accY, accZ;

// Gyroscope sensor deviation
float gyroXerror = -0.070218;
float gyroYerror = -0.011311;
float gyroZerror = -0.011546;

// Accelerometer deviation
float accXtarget = 0.0;
float accYtarget = 0.0;
float accZtarget = 9.81;
float accXerror = 0.273466;
float accYerror = 0.023643;
float accZerror = -1.416153;

// Calibration temp sums
float gyroXsum = 0;
float gyroYsum = 0;
float gyroZsum = 0;
float accXsum = 0;
float accYsum = 0;
float accZsum = 0;

// Movement detection states
boolean isMoving = false;
boolean wasMoving = false;

// LED panel control
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
RGB t = { 0,   255,   0 }; //turkis
RGB b = { 0,     0, 255 }; //blue
RGB y = { 255, 255,   0 }; //yellow
RGB m = { 255,   0, 255 }; //magenta
RGB c = { 0,   255, 150 }; //cyan
RGB o = { 0,     0,   0 }; //off

RGB n1[5][5] = {
  {o, o, o, o, o},
  {o, o, o, o, o},
  {o, o, t, o, o},
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

RGB empty[5][5] = {
  {o, o, o, o, o},
  {o, o, o, o, o},
  {o, o, o, o, o},
  {o, o, o, o, o},
  {o, o, o, o, o},
};

// Init MPU6050
void initMPU(){
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
}

void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  Serial.println(WiFi.localIP());
}

float getGyroXReading() {
  mpu.getEvent(&a, &g, &temp);
  
  float gyroX_temp = g.gyro.x;
  if(abs(gyroX_temp) > gyroXerror)  {
    gyroX += gyroX_temp/50.00;
  }
  
  return gyroX - gyroXerror;
}

float getGyroYReading() {
  mpu.getEvent(&a, &g, &temp);

  float gyroY_temp = g.gyro.y;
  if(abs(gyroY_temp) > gyroYerror) {
    gyroY += gyroY_temp/70.00;
  }
  
  return gyroY - gyroYerror;
}

float getGyroZReading() {
  mpu.getEvent(&a, &g, &temp);

  float gyroZ_temp = g.gyro.z;
  if(abs(gyroZ_temp) > gyroZerror) {
    gyroZ += gyroZ_temp/90.00;
  }
  
  return gyroZ - gyroZerror;
}

String getGyroReadings(){
  readings["gyroX"] = String(getGyroXReading());
  readings["gyroY"] = String(getGyroYReading());
  readings["gyroZ"] = String(getGyroZReading());

  String jsonString = JSON.stringify(readings);
  return jsonString;
}

float getAccXReading() {
  mpu.getEvent(&a, &g, &temp);
  return a.acceleration.x - accXerror;
}

float getAccYReading() {
  mpu.getEvent(&a, &g, &temp);
  return a.acceleration.y - accYerror;
}

float getAccZReading() {
  mpu.getEvent(&a, &g, &temp);
  return a.acceleration.z - accZerror;
}

String getAccReadingsJson() {
  readings["accX"] = String(getAccXReading());
  readings["accY"] = String(getAccYReading());
  readings["accZ"] = String(getAccZReading());
  String accString = JSON.stringify(readings);
  return accString;
}

void detectMovement() {
  float accX = getAccXReading();
  float accY = getAccYReading();
  float accZ = getAccZReading();
  
  boolean accDetected = false;
  if (abs(accX) > 15 || abs(accY) > 15 || abs(accZ) > 15) {
    accDetected = true;
  }
  
  if (accDetected) {
    if ((millis() - movingStartTime) > movingStartDelay) {
      movingStartTime = millis();
      movingStopTime = millis();
      if (!isMoving) {
        isMoving = true;
        Serial.printf("Started movement. ");
        Serial.printf("Acceleration: x=%f y=%f z=%f\n", accX, accY, accZ);  
      }
    }
  } else {
    if ((millis() - movingStopTime) > movingStopDelay) {
      movingStartTime = millis();
      movingStopTime = millis();
      if (isMoving) {
        isMoving = false;
        Serial.printf("Stopped movement. ");
        Serial.printf("Acceleration: x=%f y=%f z=%f\n", accX, accY, accZ); 
      }
    }
  }
}

void calibrateGyroscope(int precision) {
  Serial.printf("Doing %d gyro measurements\n", precision);
  
  for (int i = 0; i<precision; i++) {
    float gyroXreading = getGyroXReading();
    float gyroYreading = getGyroYReading();
    float gyroZreading = getGyroZReading();
    gyroXsum += gyroXreading;
    gyroYsum += gyroYreading;
    gyroZsum += gyroZreading;
  }

  gyroXerror = gyroXsum/precision;
  gyroYerror = gyroYsum/precision;
  gyroZerror = gyroZsum/precision;
  Serial.printf("Gyroscope average deviation: x=%f y=%f z=%f\n", gyroXerror, gyroYerror, gyroZerror);
}

void calibrateAccelerometer(int precision) {
  Serial.printf("Doing %d accelerometer measurements\n", precision);
  
  for (int i = 0; i<precision; i++) {
    float accXreading = getAccXReading();
    float accYreading = getAccYReading();
    float accZreading = getAccZReading();
    accXsum += accXreading;
    accYsum += accYreading;
    accZsum += accZreading;    
  }

  accXerror = accXsum/precision - accXtarget;
  accYerror = accYsum/precision - accYtarget;
  accZerror = accZsum/precision - accZtarget;
  Serial.printf("Accelerometer average deviation: x=%f y=%f z=%f\n", accXerror , accYerror, accZerror); 
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
  Serial.begin(115200);
  initWiFi();
  initSPIFFS();
  initMPU();

  calibrateGyroscope(500);
  calibrateAccelerometer(500);

  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.serveStatic("/", SPIFFS, "/");

  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request) {
    gyroX=0;
    gyroY=0;
    gyroZ=0;
    request->send(200, "text/plain", "OK");
  });

  server.on("/resetX", HTTP_GET, [](AsyncWebServerRequest *request) {
    gyroX=0;
    request->send(200, "text/plain", "OK");
  });

  server.on("/resetY", HTTP_GET, [](AsyncWebServerRequest *request) {
    gyroY=0;
    request->send(200, "text/plain", "OK");
  });

  server.on("/resetZ", HTTP_GET, [](AsyncWebServerRequest *request) {
    gyroZ=0;
    request->send(200, "text/plain", "OK");
  });

  server.on("/calibrateGyro", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "OK");
    calibrateGyroscope(500);
  });

  server.on("/calibrateAcc", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "OK");
    calibrateAccelerometer(500);
  });

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client) {
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);

  server.begin();

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
}

void loop() {
  
  if ((millis() - lastTime) > gyroDelay) {
    // Send Events to the Web Server with the Sensor Readings
    events.send(getGyroReadings().c_str(),"gyro_readings", millis());
    lastTime = millis();
  }
  if ((millis() - lastTimeAcc) > accelerometerDelay) {
    // Send Events to the Web Server with the Sensor Readings
    events.send(getAccReadingsJson().c_str(),"accelerometer_readings", millis());
    lastTimeAcc = millis();

    detectMovement();
    if (wasMoving != isMoving) {
      wasMoving = isMoving;
      events.send(String(isMoving).c_str(),"accelerometer_movement", millis());

      if (isMoving) {
        render(empty);
      } else {
        // reset gyro
        gyroX=0;
        gyroY=0;
        gyroZ=0;
        
        // roll the dice
        int randomNumber = random(6) + 1;
        Serial.printf("Random number: %d \n", randomNumber);
        events.send(String(randomNumber).c_str(),"dice_result", millis());

        // show numbers
        switch(randomNumber) {
          case 1:
            render(n1);
            break;
          case 2:
            render(n2);
            break;
          case 3:
            render(n3);
            break;
          case 4:
            render(n4);
            break;
          case 5:
            render(n5);
            break;
          case 6:
            render(n6);
            break;
        }
      }
    }
  }
}
