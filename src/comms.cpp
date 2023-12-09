#include "comms.h"

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
