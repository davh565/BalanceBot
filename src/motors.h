#ifndef BB_MOTORS
#define BB_MOTORS

class Motors
{
private:
    bool isRunning;
    bool isReverse;
    double driftCorrection;
    double motorSpeedA;
    double motorSpeedB;

public:
    void init(double speed, double driftCorrection);
    void setSpeed(double speed);
    // void turn(double direction);
    void stop();
    void run();

    void setDriftCorrection(double driftCorrection);
};

#endif
