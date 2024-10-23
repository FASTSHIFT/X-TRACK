#include "DataProc.h"
#include "../HAL/HAL.h"

static DataCenter center("CENTER");

DataCenter* DataProc::Center()
{
    return &center;
}

void DataProc_Init()
{
#define DP_DEF(NODE_NAME, BUFFER_SIZE)\
    Account* act##NODE_NAME = new Account(#NODE_NAME, &center, BUFFER_SIZE);
#  include "DP_LIST.inc"
#undef DP_DEF

#define DP_DEF(NODE_NAME, BUFFER_SIZE)\
do{\
    DATA_PROC_INIT_DEF(NODE_NAME);\
    _DP_##NODE_NAME##_Init(act##NODE_NAME);\
}while(0)
#  include "DP_LIST.inc"
#undef DP_DEF

}
