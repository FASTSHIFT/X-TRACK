#include "HAL.h"
#include <stdlib.h>
#include "Common/DataProc/DataProc.h"

void HAL::MAG_Update()
{
    MAG_Info_t mag;
    mag.x = rand() % 1000 - 500;
    mag.y = rand() % 1000 - 500;
    mag.z = rand() % 1000 - 500;

    DataProc::MAG_Commit(&mag);
}