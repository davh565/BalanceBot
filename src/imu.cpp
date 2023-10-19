#include "imu.h"

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include "Arduino.h"

unsigned long previousMillis = 0;

sensors_event_t orientationData, angVelocityData, linearAccelData, magnetometerData, accelerometerData, gravityData;
uint16_t BNO055_SAMPLERATE_MS = 50;

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire);

void imuInit()
{
    while (!Serial)
        delay(10); // wait for serial port to open!

    if (!bno.begin())
    {
        Serial.print("no BNO055 detected");
        while (1)
            ;
    }
}

void imuUpdate(ImuData &imuData)
{
    if (millis() - previousMillis < BNO055_SAMPLERATE_MS)
    {
        return;
    }
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
    delay(BNO055_SAMPLERATE_MS);
}