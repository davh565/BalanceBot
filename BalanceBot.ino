#include "src/config.h"
#include "src/imu.h"
#include "src/stepper.h"
#include "src/comms.h"

#include <PID_v1.h>
int speedSP = 0;
struct PidData
{
    double Kp = 300;
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
int printDelay = 250;
bool enableSteppers = true;
bool enablePrint = true;
double maxAngle = 25.0;
double minAngle = maxAngle * -1.0;
int maxFreq = 4000;
MicroStep MS = EIGHTH_STEP;

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
    pinMode(STEP_ENABLE, OUTPUT);

    digitalWrite(STEP_ENABLE, LOW);
    initMicroStep(MS);

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
////////////////////////////////////////////////////////////////////////////////////////

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

        printData(printDelay);
    }
}
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

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

bool inBounds()
{
    return imuData.orientation.z > minAngle && imuData.orientation.z < maxAngle;
}