#include "src/config.h"
#include "src/imu.h"
#include "src/stepper.h"

#include <PID_v1.h>

#define POT_PIN A0
#define DIR_PIN_LEFT 4
#define PULSE_PIN_LEFT 5
#define PULSE_PIN_RIGHT 6
#define DIR_PIN_RIGHT 7
#define STEP_ENABLE 8
#define MS1 9
#define MS2 10
#define MS3 11
#define TXS 12
#define RXS 13

int speedSP = 0;
struct PidData
{
    double Kp = 50;
    double Ki = 0.0;
    double Kd = 0.00;
    double setpoint = 0.0;
    double input = 0;
    double output = 0;
} angleControl;

ImuData imuData;
Stepper stepperLeft;
Stepper stepperRight;

PID anglePID(&angleControl.input,
             &angleControl.output,
             &angleControl.setpoint,
             angleControl.Kp,
             angleControl.Ki,
             angleControl.Kd,
             REVERSE);

String buffer;
long prevMillis = 0;
long curMillis = 0;
int printDelay = 5;
bool enableSteppers = true;
bool enablePrint = true;
double minAngle = -25.0;
double maxAngle = 25.0;
int maxFreq = 500;
// int scaleAnalog(double value)
// {
//     return map(value, -60, 60, -maxFreq, maxFreq);
// }

// int scaleAnalog(int value)
// {
//     return map(value, 0, 1023, -1000, 1000);
// }

void printData()
{
    curMillis = millis();
    if (curMillis - prevMillis > printDelay)
    {
        Serial.print("Output:");
        Serial.print(angleControl.output);
        Serial.print(", y:");
        Serial.print(imuData.orientation.y);
        Serial.print(", z:");
        Serial.print(imuData.orientation.z);
        Serial.print(", Setpoint:");
        Serial.print(angleControl.setpoint);
        Serial.print(", Gravity:");
        Serial.print((imuData.gravity.z - 9.81) * 10);
        Serial.print("\n");
        // printSensorData();
        prevMillis = curMillis;
    }
}

bool inBounds()
{
    return imuData.orientation.z > minAngle && imuData.orientation.z < maxAngle;
}

////////////////////////////////////////////////////////////////////////////////////////
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

void parseCommand(String &input)
{
    input.trim();
    double val;
    if (input == "RUN#")
    {
        //        Serial.println("Run");
        enableSteppers = true;
    }
    else if (input == "STOP#")
    {
        //        Serial.println("Stop");
        enableSteppers = false;
    }
    else if (input == "PRINT#")
    {
        //        Serial.println("Print");
        enablePrint = true;
    }
    else if (input == "NOPRINT#")
    {
        //        Serial.println("No Print");
        enablePrint = false;
    }
    else if (input.startsWith("SP:"))
    {
        val = extractNum(input);
        //        Serial.print("Setpoint: ");
        //        Serial.println(val);
        angleControl.setpoint = val;
    }
    else if (input.startsWith("KP:"))
    {
        val = extractNum(input);
        //        Serial.print("Kp: ");
        //        Serial.println(val);
        angleControl.Kp = val;
    }
    else if (input.startsWith("KI:"))
    {
        val = extractNum(input);
        //        Serial.print("Ki: ");
        //        Serial.println(val);
        angleControl.Ki = val;
    }
    else if (input.startsWith("KD:"))
    {
        val = extractNum(input);
        //        Serial.print("Kd: ");
        //        Serial.println(val);
        angleControl.Kd = val;
    }
    else if (input.startsWith("LM:"))
    {
        val = extractNum(input);
        //        Serial.print("Limits: ");
        //        Serial.println(val);
        minAngle = -val;
        maxAngle = val;
    }
    else if (input.startsWith("FQ:"))
    {
        val = extractNum(input);
        //        Serial.print("MaxSpeed: ");
        //        Serial.println(val);
        maxFreq = val;
        // anglePID.SetOutputLimits(-maxFreq, maxFreq);
    }
}
////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
    pinMode(STEP_ENABLE, OUTPUT);
    pinMode(MS1, OUTPUT);
    pinMode(MS2, OUTPUT);
    pinMode(MS3, OUTPUT);

    digitalWrite(STEP_ENABLE, LOW);
    digitalWrite(MS1, HIGH);
    digitalWrite(MS2, LOW);
    digitalWrite(MS3, LOW);

    Serial.begin(115200);
    imuInit();
    // Serial.begin(115200);

    stepperLeft.init(DIR_PIN_LEFT, PULSE_PIN_LEFT, maxFreq, 0.555555556);
    stepperRight.init(DIR_PIN_RIGHT, PULSE_PIN_RIGHT, maxFreq, 0.555555556);

    anglePID.SetMode(AUTOMATIC);
    anglePID.SetOutputLimits(-maxFreq, maxFreq);
    anglePID.SetSampleTime(10);

    delay(1000);
}

void loop()
{
    if (readStringUntilChar(buffer, '#'))
    {
        parseCommand(buffer);
        buffer = "";
    }

    imuUpdate(imuData);
    angleControl.input = imuData.orientation.z;
    anglePID.SetTunings(angleControl.Kp, angleControl.Ki, angleControl.Kd);
    anglePID.SetOutputLimits(-maxFreq, maxFreq);
    anglePID.Compute();

    if (enableSteppers && inBounds())
    // if (0)
    {
        //        anglePID.SetMode(1); //enable PID
        stepperLeft.run(angleControl.output);
        stepperRight.run(angleControl.output);
    }
    else
    {
        //        anglePID.SetMode(0); //enable PID
        stepperLeft.stop();
        stepperRight.stop();
    }

    if (enablePrint)
    {

        printData();
    }
}
