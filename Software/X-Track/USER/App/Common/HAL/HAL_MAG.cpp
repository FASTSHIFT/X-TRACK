#include "HAL.h"
#include <stdlib.h>

extern void DP_MAG_Commit(const void* data, uint32_t size);

void HAL::MAG_Update()
{
    MAG_Info_t mag;
    mag.x = rand() % 1000 - 500;
    mag.y = rand() % 1000 - 500;
    mag.z = rand() % 1000 - 500;

    DP_MAG_Commit(&mag, sizeof(mag));
}