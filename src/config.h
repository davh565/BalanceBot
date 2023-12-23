#ifndef BB_CONFIG
#define BB_CONFIG

// #define EN_A 6
// #define EN_B 7
// #define IN1_A 8
// #define IN2_A 9
// #define IN1_B 10
// #define IN2_B 11

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

struct PidData
{
    double Kp = 1;
    double Ki = 0.0;
    double Kd = 0.0;
    double sp = 0.0;
    double in = 0;
    double out = 0;
};

// Key-Value Pair
template <typename T>
struct kvp
{
    String key;
    T *valuePtr;
};

// Determine Array Size on Compile
template <size_t N, typename T>
constexpr size_t sizeOf(T (&)[N]) { return N; }

#endif