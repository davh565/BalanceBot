#include "src/config.h"
#include "src/imu.h"
#include "src/stepper.h"
#include "src/comms.h"

#include <PID_v1.h>

PidData angCtrl, posCtrl;
ImuData BNOData;
Stepper stepperLeft;
Stepper stepperRight;
PID anglePID(&angCtrl.in, &angCtrl.out, &angCtrl.sp, angCtrl.Kp, angCtrl.Ki, angCtrl.Kd, REVERSE);
PID posPID(&posCtrl.in, &posCtrl.out, &posCtrl.sp, posCtrl.Kp, posCtrl.Ki, posCtrl.Kd, DIRECT);
double stepAngle;
double stepPos;
double tiltOffset = 0.0;

// PARAMS_IN//
#define ANG_KP 300.0
#define ANG_KI 1000.0
#define ANG_KD 0.15

#define POS_KP 0.1
#define POS_KI 0.0
#define POS_KD 0.0
// int speedSP = 0;
double maxAngle = 25.0;
int maxFreq = 3000;
double maxTilt = 2.0;
MicroStep MS = EIGHTH_STEP;
bool enableSteppers = false;
bool enablePrint = true;

kvp<bool> flagsIn[] =
    {
        {"EnStp", &enableSteppers},
        {"EnPrn", &enablePrint}};

kvp<double> paramsIn[] =
    {
        {"AngSP", &tiltOffset},
        {"AngKp", &angCtrl.Kp},
        {"AngKi", &angCtrl.Ki},
        {"AngKd", &angCtrl.Kd},
        {"PosSP", &posCtrl.sp},
        {"PosKp", &posCtrl.Kp},
        {"PosKi", &posCtrl.Ki},
        {"PosKd", &posCtrl.Kd},
};

kvp<double> paramsOut[] =
    {
        {"ImuAngZ", &BNOData.orientation.z},
        {"AngOut", &angCtrl.out},
        {"PosOut", &posCtrl.out},
        {"StepAng", &stepAngle},
        {"StepPos", &stepPos},
};

int8_t printArraySize;
String buffer;
int printDelay = 250;

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

    angCtrl.Kp = ANG_KP;
    angCtrl.Ki = ANG_KI;
    angCtrl.Kd = ANG_KD;
    anglePID.SetMode(AUTOMATIC);
    anglePID.SetOutputLimits(-maxFreq, maxFreq);
    anglePID.SetSampleTime(10);

    posCtrl.Kp = POS_KP;
    posCtrl.Ki = POS_KI;
    posCtrl.Kd = POS_KD;
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
        // parseCommand(buffer);
        parseParamsIn(buffer, paramsIn, sizeOf(paramsIn));
        buffer = "";
    }

    BNOUpdate(BNOData);
    stepAngle = (stepperLeft.getAngle() + stepperRight.getAngle()) / 2;
    stepPos = (stepperLeft.getPosition() + stepperRight.getPosition()) / 2;

    // MPUUpdate(MPUData);
    angCtrl.in = BNOData.orientation.z;
    angCtrl.sp = tiltOffset + posCtrl.out;
    anglePID.SetTunings(angCtrl.Kp, angCtrl.Ki, angCtrl.Kd);
    anglePID.SetOutputLimits(-maxFreq, maxFreq);
    anglePID.Compute();

    posCtrl.in = stepperLeft.getPosition();
    posPID.SetTunings(posCtrl.Kp, posCtrl.Ki, posCtrl.Kd);
    posPID.SetOutputLimits(-maxTilt, maxTilt);
    posPID.Compute();

    if (enableSteppers && inBounds())
    // if (0)
    {
        //        anglePID.SetMode(1); //enable PID
        stepperLeft.run(angCtrl.out);
        stepperRight.run(angCtrl.out);
    }
    else
    {
        //        anglePID.SetMode(0); //enable PID
        stepperLeft.stop();
        stepperRight.stop();
    }

    if (enablePrint)
    {

        printParamsOut(paramsIn, sizeOf(paramsIn), printDelay, millis());
    }
}
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

bool inBounds()
{
    return BNOData.orientation.z > -maxAngle && BNOData.orientation.z < maxAngle;
}
