#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Arduino_JSON.h>
#include "SPIFFS.h"

// Replace with your network credentials
const char* ssid = "FRITZ!Box Gastzugang";
const char* password = "gastzugang";

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
float gyroXerror = -0.0693;
float gyroYerror = -0.0126;
float gyroZerror = -0.0096;

// Acceleration error
float accXerror = 0.083;
float accYerror = -0.0916;
float accZerror = -1.471;

// Movement detection states
boolean isMoving = false;
boolean wasMoving = false;

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

String getGyroReadings(){
  mpu.getEvent(&a, &g, &temp);

  float gyroX_temp = g.gyro.x;
  if(abs(gyroX_temp) > gyroXerror)  {
    gyroX += gyroX_temp/50.00;
  }
  
  float gyroY_temp = g.gyro.y;
  if(abs(gyroY_temp) > gyroYerror) {
    gyroY += gyroY_temp/70.00;
  }

  float gyroZ_temp = g.gyro.z;
  if(abs(gyroZ_temp) > gyroZerror) {
    gyroZ += gyroZ_temp/90.00;
  }

  readings["gyroX"] = String(gyroX);
  readings["gyroY"] = String(gyroY);
  readings["gyroZ"] = String(gyroZ);

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

boolean detectMovement() {
  float accX = getAccXReading();
  float accY = getAccYReading();
  float accZ = getAccZReading();
  
  boolean accDetected = false;
  if (abs(accX) > 10 || abs(accY) > 10 || abs(accZ) > 10) {
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

void setup() {
  Serial.begin(115200);
  initWiFi();
  initSPIFFS();
  initMPU();

  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.serveStatic("/", SPIFFS, "/");

  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request){
    gyroX=0;
    gyroY=0;
    gyroZ=0;
    request->send(200, "text/plain", "OK");
  });

  server.on("/resetX", HTTP_GET, [](AsyncWebServerRequest *request){
    gyroX=0;
    request->send(200, "text/plain", "OK");
  });

  server.on("/resetY", HTTP_GET, [](AsyncWebServerRequest *request){
    gyroY=0;
    request->send(200, "text/plain", "OK");
  });

  server.on("/resetZ", HTTP_GET, [](AsyncWebServerRequest *request){
    gyroZ=0;
    request->send(200, "text/plain", "OK");
  });

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);

  server.begin();
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
    }
  }
}
