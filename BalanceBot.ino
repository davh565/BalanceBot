#include "src/config.h"
#include "src/imu.h"
#include "src/stepper.h"
#include "src/comms.h"

#include <PID_v1.h>

#define ANG_KP 300.0
#define ANG_KI 1000.0
#define ANG_KD 0.15

#define POS_KP 0.1
#define POS_KI 0.0
#define POS_KD 0.0
// int speedSP = 0;
struct PidData
{
    double Kp = 1;
    double Ki = 0.0;
    double Kd = 0.0;
    double setpoint = 0.0;
    double input = 0;
    double output = 0;
} angleControl, posControl;

long prevMillis = 0;
long curMillis = 0;
ImuData BNOData;
// ImuData MPUData;
Stepper stepperLeft;
Stepper stepperRight;

PID anglePID(&angleControl.input,
             &angleControl.output,
             &angleControl.setpoint,
             angleControl.Kp,
             angleControl.Ki,
             angleControl.Kd,
             REVERSE);

PID posPID(&posControl.input,
           &posControl.output,
           &posControl.setpoint,
           posControl.Kp,
           posControl.Ki,
           posControl.Kd,
           REVERSE);

String buffer;
int printDelay = 250;
bool enableSteppers = false;
bool enablePrint = true;
double maxAngle = 25.0;
int maxFreq = 4000;
double maxTilt = 5.0;
MicroStep MS = EIGHTH_STEP;

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
    pinMode(STEP_ENABLE, OUTPUT);

    digitalWrite(STEP_ENABLE, LOW);

    Serial.begin(115200);
    BNOInit();
    // MPUInit();
    // Serial.begin(115200);

    stepperLeft.init(DIR_PIN_LEFT, PULSE_PIN_LEFT, MS, maxFreq, 200, 100);
    stepperRight.init(DIR_PIN_RIGHT, PULSE_PIN_RIGHT, MS, maxFreq, 200, 100);

    angleControl.Kp = ANG_KP;
    angleControl.Ki = ANG_KI;
    angleControl.Kd = ANG_KD;
    anglePID.SetMode(AUTOMATIC);
    anglePID.SetOutputLimits(-maxFreq, maxFreq);
    anglePID.SetSampleTime(10);

    posControl.Kp = POS_KP;
    posControl.Ki = POS_KI;
    posControl.Kd = POS_KD;
    posPID.SetMode(AUTOMATIC);
    posPID.SetOutputLimits(-maxTilt, maxTilt);
    posPID.SetSampleTime(50);

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
    // MPUUpdate(MPUData);
    angleControl.input = BNOData.orientation.z;
    angleControl.setpoint = posControl.output;
    anglePID.SetTunings(angleControl.Kp, angleControl.Ki, angleControl.Kd);
    anglePID.SetOutputLimits(-maxFreq, maxFreq);
    anglePID.Compute();

    posControl.input = stepperLeft.getPosition();
    posPID.SetTunings(posControl.Kp, posControl.Ki, posControl.Kd);
    posPID.SetOutputLimits(-maxTilt, maxTilt);
    posPID.Compute();

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
        Serial.print(", Setpoint:");
        Serial.print(angleControl.setpoint);
        Serial.print(", Ang:");
        Serial.print(stepperLeft.getAngle());
        Serial.print(", Pos:");
        Serial.print(stepperLeft.getPosition());
        Serial.print("\n");
        // printSensorData();
        prevMillis = curMillis;
    }
}