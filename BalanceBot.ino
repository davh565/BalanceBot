#include "src/config.h"
#include "src/imu.h"
#include "src/stepper.h"
#include "src/comms.h"

#include <PID_v1.h>
int speedSP = 0;
struct PidData
{
    double Kp = 400;
    double Ki = 500.0;
    double Kd = 0.1;
    double setpoint = -2.0;
    double input = 0;
    double output = 0;
} angleControl;

long prevMillis = 0;
long curMillis = 0;
ImuData BNOData;
ImuData MPUData;
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
bool enableSteppers = false;
bool enablePrint = true;
double maxAngle = 25.0;
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
    BNOInit();
    MPUInit();
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

    BNOUpdate(BNOData);
    MPUUpdate(MPUData);
    angleControl.input = BNOData.orientation.z;
    // angleControl.input = -MPUData.orientation.y;
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
    else if (input.startsWith("SP:"))
    {
        val = extractNum(input);
        angleControl.setpoint = val;
    }
    else if (input.startsWith("KP:"))
    {
        val = extractNum(input);
        angleControl.Kp = val;
    }
    else if (input.startsWith("KI:"))
    {
        val = extractNum(input);
        angleControl.Ki = val;
    }
    else if (input.startsWith("KD:"))
    {
        val = extractNum(input);
        angleControl.Kd = val;
    }
    else if (input.startsWith("LM:"))
    {
        val = extractNum(input);
        maxAngle = val;
    }
    else if (input.startsWith("FQ:"))
    {
        val = extractNum(input);
        maxFreq = val;
    }
}

bool inBounds()
{
    return BNOData.orientation.z > -maxAngle && BNOData.orientation.z < maxAngle;
}

void printData(int printDelay)
{
    curMillis = millis();
    if (curMillis - prevMillis > printDelay)
    {
        Serial.print("Output:");
        Serial.print(angleControl.output);
        // Serial.print(", y:");
        // Serial.print(BNOData.orientation.y);
        Serial.print(", Bz:");
        Serial.print(BNOData.orientation.z);
        // Serial.print(", Bg:");
        // Serial.print(BNOData.gravity.z);
        // Serial.print(", Mx:");
        // Serial.print(MPUData.orientation.x);
        Serial.print(", My:");
        Serial.print(-MPUData.orientation.y);
        // Serial.print(", Mz:");
        // Serial.print(MPUData.orientation.z);
        // Serial.print(", Mg:");
        // Serial.print(MPUData.gravity.z);
        Serial.print(", Setpoint:");
        Serial.print(angleControl.setpoint);
        Serial.print("\n");
        // printSensorData();
        prevMillis = curMillis;
    }
}