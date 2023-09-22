#include "config.h"
#include "motors.h"

#include <L298NX2.h>

L298NX2 motorsL298N(EN_A, IN1_A, IN2_A, EN_B, IN1_B, IN2_B);

void Motors::init(double speed = 100.0, double driftCorrection = 1.0)
{
    this->driftCorrection = driftCorrection;
    setSpeed(speed);
    stop();
}

void Motors::setSpeed(double speed)
{
    motorSpeedA = abs(speed);
    motorSpeedB = abs(speed) * driftCorrection;

    if (speed > 0)
    {
        isReverse = false;
    }
    else
    {
        isReverse = true;
    }
    if (motorSpeedA > 255)
    {
        motorSpeedA = 255;
    }
    if (motorSpeedB > 255)
    {
        motorSpeedB = 255;
    }

    motorsL298N.setSpeedA(motorSpeedA);
    motorsL298N.setSpeedB(motorSpeedB);
}

void Motors::run()
{
    if (isReverse)
    {
        motorsL298N.backward();
    }
    else
    {

        motorsL298N.forward();
    }
}

void Motors::stop()
{
    motorsL298N.stop();
}

void Motors::setDriftCorrection(double driftCorrection)
{
    this->driftCorrection = driftCorrection;
}
