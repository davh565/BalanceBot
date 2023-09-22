#include "src/config.h"
#include "src/imu.h"
#include "src/motors.h"

ImuData imuData;
void setup(void)
{
    Serial.begin(115200);

    imuInit();

    delay(1000);
}

void loop(void)
{
    imuUpdate(imuData);
    printSensorData();
}

void printSensorData()
{
    Serial.print("(");
    Serial.print(imuData.orientation.x);
    Serial.print(",");
    Serial.print(imuData.orientation.y);
    Serial.print(",");
    Serial.print(imuData.orientation.z);
    Serial.print(")");
    Serial.print("\n");
}