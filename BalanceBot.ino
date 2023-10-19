#include "src/config.h"
#include "src/imu.h"
#include "src/stepper.h"

#include <PID_v1.h>

#define POT_PIN A0
#define PULSE_PIN_LEFT 5
#define PULSE_PIN_RIGHT 6
#define DIR_PIN_LEFT 4
#define DIR_PIN_RIGHT 7

int speedSP = 0;
struct PidData
{
    double Kp = 5;
    double Ki = 0;
    double Kd = 0.1;
    double setpoint = 0;
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

int scaleAnalog(int value)
{
    return map(value, 0, 1023, -1000, 1000);
}

void printSensorData()
{
    Serial.print("(");
    Serial.print(imuData.orientation.x);
    Serial.print(",");
    Serial.print(imuData.orientation.y);
    Serial.print(",");
    Serial.print(imuData.orientation.z);
    Serial.print(")");
    Serial.print("\n");
}
void setup()
{
    Serial.begin(115200);

    // imuInit();

    stepperLeft.init(DIR_PIN_LEFT, PULSE_PIN_LEFT, 1000, 0.555555556);
    stepperRight.init(DIR_PIN_RIGHT, PULSE_PIN_RIGHT, 1000, 0.555555556);

    // anglePID.SetMode(AUTOMATIC);
    // anglePID.SetOutputLimits(-255, 255);
    // anglePID.SetSampleTime(1);

    delay(1000);
}

void loop()
{
    speedSP = scaleAnalog(analogRead(POT_PIN));
    stepperLeft.run(speedSP);
    stepperRight.run(speedSP);

    // imuUpdate(imuData);
    // angleControl.input = imuData.orientation.y;
    // anglePID.Compute();
    // // Serial.println(angleControl.output);
    // Serial.println(angleControl.output);
    // stepperLeft.run(angleControl.output);
    // stepperRight.run(angleControl.output);
    // printSensorData();
}
