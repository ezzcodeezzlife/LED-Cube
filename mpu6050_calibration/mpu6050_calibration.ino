#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Create a sensor object
Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;

float gyroX, gyroY, gyroZ;
float accX, accY, accZ;

int ATTEMPTS = 200;

// Gyroscope deviation
float gyroXerror = -0.0693;
float gyroYerror = -0.0126;
float gyroZerror = -0.0096;

// Acceleration deviation
float accXtarget = 0.0;
float accYtarget = 0.0;
float accZtarget = 9.81;
float accXerror = 0.083;
float accYerror = -0.0916;
float accZerror = -1.471;

// Sums
float gyroXsum = 0;
float gyroYsum = 0;
float gyroZsum = 0;
float accXsum = 0;
float accYsum = 0;
float accZsum = 0;

// Counter
int gyroReadCounter = 0;
int accReadCounter = 0;

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



void setup() {
  Serial.begin(115200);
  initMPU();
}

void loop() {
  float gyroXreading = getGyroXReading();
  float gyroYreading = getGyroYReading();
  float gyroZreading = getGyroZReading();

  if (gyroReadCounter <= ATTEMPTS) {
    if (gyroReadCounter == 0) {
      Serial.printf("Doing %d gyro measurements\n", ATTEMPTS);
    }
    gyroXsum += gyroXreading;
    gyroYsum += gyroYreading;
    gyroZsum += gyroZreading;

    if (gyroReadCounter == ATTEMPTS) {
      float gyroXaverage = gyroXsum/ATTEMPTS;
      float gyroYaverage = gyroYsum/ATTEMPTS;
      float gyroZaverage = gyroZsum/ATTEMPTS;
      Serial.printf("Gyro average: x=%f y=%f z=%f\n", gyroXaverage, gyroYaverage, gyroZaverage); 
      Serial.printf("Gyro deviation: x=%f y=%f z=%f\n", gyroXaverage - 0, gyroYaverage - 0, gyroZaverage - 0); 
    }
    gyroReadCounter++;
  } 
  
  float accXreading = getAccXReading();
  float accYreading = getAccYReading();
  float accZreading = getAccZReading();

  if (accReadCounter <= ATTEMPTS) {
    if (accReadCounter == 0) {
      Serial.printf("Doing %d acc measurements\n", ATTEMPTS);
    }
    accXsum += accXreading;
    accYsum += accYreading;
    accZsum += accZreading;

    if (accReadCounter == ATTEMPTS) {
      float accXaverage = accXsum/ATTEMPTS;
      float accYaverage = accYsum/ATTEMPTS;
      float accZaverage = accZsum/ATTEMPTS;
      Serial.printf("Acceleration average: x=%f y=%f z=%f\n", accXaverage, accYaverage, accZaverage); 
      Serial.printf("Acceleration deviation: x=%f y=%f z=%f\n", accXaverage - accXtarget, accYaverage - accYtarget, accZaverage - accZtarget); 
    }
    accReadCounter++;
  }
}
