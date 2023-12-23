#include "comms.h"

void printParamsOut(kvp<double> params[], int8_t arraySize, int printDelay, unsigned long curMillis)
{
    static unsigned long prevMillis;
    if (curMillis - prevMillis > printDelay)
    {
        for (int i = 0; i < arraySize; ++i)
        {
            if (i > 0)
            {
                Serial.print(", ");
            }
            Serial.print(params[i].key);
            Serial.print(":");
            Serial.print(*params[i].valuePtr);
        }
        Serial.println();
        prevMillis = curMillis;
    }
}

void parseParamsIn(String &input, kvp<double> params[], int8_t arraySize)
{
    input.trim();
    double val = extractNum(input);
    for (int i = 0; i < arraySize; ++i)
    {
        if (input.startsWith(params[i].key))
            *params[i].valuePtr = val;
    }
}

void parseFlagsIn(String &input, kvp<bool> params[], int8_t arraySize)
{
    input.trim();
    double val = extractNum(input);
    for (int i = 0; i < arraySize; ++i)
    {
        if (input.startsWith(params[i].key))
        {
            if (input.endsWith("ON"))
                *params[i].valuePtr = true;
            else if (input.endsWith("ON"))
                *params[i].valuePtr = false;
            else if (input.endsWith("TGL"))
                *params[i].valuePtr = !*params[i].valuePtr;
        }
        *params[i].valuePtr = val;
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
