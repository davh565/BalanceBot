#include "imu.h"
#include <MPU6500_WE.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include "Arduino.h"

unsigned long currentMillis = 0;
unsigned long previousMillisBNO = 0;
unsigned long previousMillisMPU = 0;

#define MPU6500_ADDR 0x68
MPU6500_WE myMPU6500 = MPU6500_WE(MPU6500_ADDR);
uint16_t MPU6500_SAMPLERATE_MS = 50;
// xyzFloat gValue;
// xyzFloat angles;
// MPU9250_orientation orientation;

sensors_event_t orientationData, angVelocityData, linearAccelData, magnetometerData, accelerometerData, gravityData;
uint16_t BNO055_SAMPLERATE_MS = 50;

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire);

void MPUInit()
{
    if (TWCR == 0) // do this check so that Wire only gets initialized once
    {
        Wire.begin();
    }
    if (!myMPU6500.init())
    {
        Serial.println("MPU6500 does not respond");
    }
    else
    {
        Serial.println("MPU6500 is connected");
    }
    Serial.println("Position you MPU6500 flat and don't move it - calibrating...");
    delay(1000);
    // myMPU6500.autoOffsets();
    Serial.println("Done!");

    myMPU6500.enableGyrDLPF();

    myMPU6500.setGyrDLPF(MPU6500_DLPF_6);
    myMPU6500.setSampleRateDivider(5);
    myMPU6500.setGyrRange(MPU6500_GYRO_RANGE_250);
    myMPU6500.setAccRange(MPU6500_ACC_RANGE_2G);
    myMPU6500.enableAccDLPF(true);
    myMPU6500.setAccDLPF(MPU6500_DLPF_6);
    delay(200);
}

void MPUUpdate(ImuData &imuData)
{
    currentMillis = millis();
    if (currentMillis - previousMillisMPU >= BNO055_SAMPLERATE_MS)
    {
        xyzFloat gValue = myMPU6500.getGValues();
        xyzFloat angles = myMPU6500.getAngles();
        // orientation = myMPU6500.getOrientation();
        imuData.orientation.x = angles.x;
        imuData.orientation.y = angles.y;
        imuData.orientation.z = angles.z;
        imuData.gravity.x = gValue.x;
        imuData.gravity.y = gValue.y;
        imuData.gravity.z = gValue.z;

        previousMillisMPU = currentMillis;
    }
}

void BNOInit()
{
    while (!Serial)
    {
        delay(10); // wait for serial port to open!
    }

    if (!bno.begin())
    {
        Serial.print("no BNO055 detected");
        while (1)
            ;
    }
}

void BNOUpdate(ImuData &imuData)
{
    currentMillis = millis();
    if (currentMillis - previousMillisBNO >= BNO055_SAMPLERATE_MS)
    {

        bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
        bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
        bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
        bno.getEvent(&gravityData, Adafruit_BNO055::VECTOR_GRAVITY);

        imuData.orientation.x = orientationData.orientation.x;
        imuData.orientation.y = orientationData.orientation.y;
        imuData.orientation.z = orientationData.orientation.z;
        imuData.angVelocity.x = angVelocityData.gyro.x;
        imuData.angVelocity.y = angVelocityData.gyro.y;
        imuData.angVelocity.z = angVelocityData.gyro.z;
        imuData.linearAccel.x = linearAccelData.acceleration.x;
        imuData.linearAccel.y = linearAccelData.acceleration.y;
        imuData.linearAccel.z = linearAccelData.acceleration.z;
        imuData.gravity.x = gravityData.acceleration.x;
        imuData.gravity.y = gravityData.acceleration.y;
        imuData.gravity.z = gravityData.acceleration.z;
        previousMillisBNO = currentMillis;
    }
}
