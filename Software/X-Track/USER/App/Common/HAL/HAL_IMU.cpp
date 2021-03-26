#include "HAL.h"

int16_t HAL::IMU_GetSteps()
{
    static int16_t steps;
    steps++;
    if (steps > 9999)
    {
        steps = 0;
    }
    return steps;
}
