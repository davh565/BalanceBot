#ifndef BB_COMMS
#define BB_COMMS
#include "Arduino.h"
#include "config.h"

void printParamsOut(kvp<double> params[], int8_t arraySize, int printDelay, unsigned long curMillis);
void parseParamsIn(String &input, kvp<double> params[], int8_t arraySize);
void parseFlagsIn(String &input, kvp<bool> params[], int8_t arraySize);
bool readStringUntilChar(String &buffer, char terminator);
double extractNum(String buffer);
#endif