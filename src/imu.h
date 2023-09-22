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

void imuInit();
void imuUpdate(ImuData &imuData);

#endif