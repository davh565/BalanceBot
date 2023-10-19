#include "stepper.h"
// #include "io.h"

/// @brief Initialise stepper motor.
/// @param pinDIR The pin number for the direction output.
/// @param pinPUL The pin number for the pulse output.
/// @param maxFrequency The maximum frequency of the pulse output.
/// @param pulsesPerDeg The number of pulses per degree of rotation.
void Stepper::init(int pinDIR, PulsePin pinPUL, uint32_t maxFrequency, double pulsesPerDeg)
{
    this->pinDIR = pinDIR;
    this->maxFrequency = maxFrequency;
    this->pulsesPerDeg = pulsesPerDeg;
    frequency = maxFrequency;

    dir = FORWARD;
    pinMode(pinDIR, OUTPUT);
    Pulse.init(pinPUL, maxFrequency);
    Pulse.setDirection(dir);
    Pulse.setRunMode(TARGET);
}

/// @brief Set the frequency of the stepper motor. Cap at maxFrequency if input is greater.
/// @param frequency The frequency to set the stepper motor to in Hz.
void Stepper::setFrequency(uint32_t frequency)
{
    if (frequency > maxFrequency)
        frequency = maxFrequency;
    this->frequency = frequency;
    Pulse.setFrequency(frequency);
}

/// @brief  set the direction of the stepper motor
/// @param dir 0 for forward, 1 for reverse
void Stepper::setDirection(direction dir)
{
    this->dir = dir;
    Pulse.setDirection(dir);
    // bool dirState = (dir == FORWARD) ? dirState = HIGH : dirState = LOW;
    digitalWrite(pinDIR, dir);
}

/// @brief Enable or disable the stepper motor.
/// @param isEnabled false to disable, true to enable.
void Stepper::setEnabled(bool isEnabled)
{
    // Serial.println("Stepper::setEnabled()");
    if (isEnabled)
        Pulse.enable();
    else
        Pulse.disable();
}

/// @brief  Run the stepper motor for a number of pulses from current
/// position
/// @param steps The number of pulses to run the motor for.
void Stepper::runPulses(int32_t steps)
{
    Pulse.setRunMode(TARGET);
    if (steps < 0)
        setDirection(REVERSE);
    else
        setDirection(FORWARD);
    long currentStep = getPulseCount();
    setTarget(currentStep + steps);
    enable();
}

/// @brief  Run the stepper motor to a target position.
/// @param target The target position in pulses.
void Stepper::runToTarget(int32_t target)
{
    Pulse.setRunMode(TARGET);
    long currentStep = getPulseCount();
    if (target > currentStep)
    {
        setDirection(FORWARD);
        setTarget(target);
        enable();
    }
    else if (target < currentStep)
    {
        setDirection(REVERSE);
        setTarget(target);
        enable();
    }
}

/// @brief  Move the stepper motor by a number of degrees.
/// @param angle The angle to move the stepper motor by in degrees.
void Stepper::runAngle(double angle)
{
    // Serial.println(angle);
    runPulses(angle * pulsesPerDeg);
}

/// @brief  Run the stepper motor at specified speed indefinitely
void Stepper::run(direction dir, uint32_t frequency)
{
    setDirection(dir);
    setFrequency(frequency);
    Pulse.setRunMode(CONST);
    enable();
}

// /// @brief  Run the stepper motor at current speed indefinitely
// /// @param dir 0 for forward, 1 for reverse.
// void Stepper::run(direction dir)
// {
//     setDirection(dir);
//     Pulse.setRunMode(CONST);
//     enable();
// }

/// @brief  Run the stepper motor at specified speed indefinitely. Direction determined
/// by sign of frequency.
void Stepper::run(int32_t frequency)
{
    if (frequency >= 0)
        setDirection(FORWARD);
    else
        setDirection(REVERSE);
    setFrequency(abs(frequency));
    Pulse.setRunMode(CONST);
    enable();
}

/// @brief Stop the stepper motor by disabling the pulse generator. Does not
/// change the target, so the motor will resume when enable() is called again.
/// use stop() method instead to stop motor at current position indefinitely.
void Stepper::disable() { setEnabled(false); }

/// @brief Enable the stepper motor by enabling the pulse generator.
void Stepper::enable() { setEnabled(true); }

/// @brief Stop the stepper motor. This is achieved by setting the target
/// step count to the current step count. This differs from the disable()
/// method, which stops the stepper motor immediately, but does not effect
/// the target.
void Stepper::stop()
{
    Pulse.setRunMode(TARGET);
    setTarget(getPulseCount());
}
