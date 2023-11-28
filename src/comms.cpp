#include "comms.h"

long prevMillis = 0;
long curMillis = 0;
void printData(int printDelay)
{
    curMillis = millis();
    if (curMillis - prevMillis > printDelay)
    {
        // Serial.print("Output:");
        // Serial.print(angleControl.output);
        // Serial.print(", y:");
        // Serial.print(imuData.orientation.y);
        // Serial.print(", z:");
        // Serial.print(imuData.orientation.z);
        // Serial.print(", Setpoint:");
        // Serial.print(angleControl.setpoint);
        // Serial.print(", Gravity:");
        // Serial.print((imuData.gravity.z - 9.81) * 10);
        // Serial.print("\n");
        // printSensorData();
        prevMillis = curMillis;
    }
}

bool readStringUntilChar(String &buffer, char terminator)
{
    while (Serial.available())
    {
        // Serial.println(buffer);
        char c = Serial.read();
        buffer += c;
        if (c == terminator)
        {
            // Serial.println(buffer);
            return true;
        }
    }
    return false;
}

double extractNum(String buffer) { return buffer.substring(3, buffer.length() - 1).toFloat(); }
