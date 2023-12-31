#ifndef BB_PULSE
#define BB_PULSE

// #define TARGET 0
// #define CONSTANT 1

enum direction
{
    FORWARD,
    REVERSE
};
enum pulseMode
{
    TARGET,
    CONST
};
/// @brief PWM pins for Timers 1,3,4,5. Note timer 1 is used by the Servo library
/// If using servo library, do not use Timer 1 pwm pins (OC1A)
enum PulsePin : uint8_t
{
    oc3a = 5,
    oc4a = 6,
    oc1a = 11,
    oc5a = 46
};
/// @brief Pulse Generator class, Should not have more instances than available
/// hardware timers, but this is not currently enforced.
class PulseGenerator
{
public:
    bool getEnabled();
    long getCount();
    void disable();
    void enable();
    void init(PulsePin pulsePin, uint32_t frequency);
    void resetCount();
    void setCount(long count);
    void setDirection(bool dir);
    void setFrequency(uint32_t frequency);
    void setRunMode(bool mode);
    void setTarget(long target);

private:
    uint8_t pulsePin;
};

#endif // BB_PULSE