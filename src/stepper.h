#ifndef BB_STP
#define BB_STP

#include <Arduino.h>
#include "pulse.h"
#include "config.h"

enum MicroStep
{
    FULL_STEP,
    HALF_STEP,
    QUARTER_STEP,
    EIGHTH_STEP,
    SIXTEENTH_STEP
};

/// @brief Stepper motor class.
class Stepper
{
private:
    bool isEnabled;
    double stepsPerRev;
    double wheelDiameter;
    int microStepMultiplier;
    MicroStep MS;
    direction dir;
    int pinDIR;
    uint32_t maxFrequency;
    uint32_t frequency;
    void resetPulseCount() { Pulse.resetCount(); }
    void setCount(long count) { Pulse.setCount(count); }

    void setDirection(direction dir);
    void setTarget(int32_t target) { Pulse.setTarget(target); }
    void setEnabled(bool isEnabled);
    void initMicroStep(MicroStep MS);

public:
    PulseGenerator Pulse;
    int getDirection() { return dir; }
    uint32_t getFrequency() { return frequency; }
    uint32_t getMaxFrequency() { return maxFrequency; }
    bool getEnabled() { return Pulse.getEnabled(); }
    long getPulseCount() { return Pulse.getCount(); }
    double getPosition();
    double getAngle();
    void setFrequency(uint32_t frequency);
    void runPulses(int32_t steps);
    void runAngle(double angle);
    void runToTarget(int32_t target);
    void run(direction dir, uint32_t frequency);
    // void run(direction dir);
    void run(int32_t frequency);
    void disable();
    void enable();
    // void runPulses() { runPulses(0); }
    void stop();
    void init(int pinDIR,
              PulsePin pinPUL,
              MicroStep MS,
              uint32_t maxFrequency,
              double stepsPerRev,
              double wheelDiameter);
};

#endif // BB_STP