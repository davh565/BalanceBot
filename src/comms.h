#ifndef BB_COMMS
#define BB_COMMS
#include "Arduino.h"

void printData(int printDelay);
bool readStringUntilChar(String &buffer, char terminator);
double extractNum(String buffer);
#endif