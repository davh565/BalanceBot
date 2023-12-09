#ifndef BB_IMU
#define BB_IMU

struct Coords
{
    double x;
    double y;
    double z;
};

struct ImuData
{
    Coords orientation;
    Coords angVelocity;
    Coords linearAccel;
    Coords gravity;
};

void MPUInit();
void MPUUpdate(ImuData &imuData);
void BNOInit();
void BNOUpdate(ImuData &imuData);

#endif