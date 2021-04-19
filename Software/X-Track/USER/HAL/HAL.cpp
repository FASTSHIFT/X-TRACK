#include "HAL.h"

static void HAL_InterrputUpdate()
{
    HAL::Power_Update();
    HAL::Encoder_Update();
    //HAL::IMU_Update();
    //HAL::MAG_Update();
    HAL::Audio_Update();
}

void HAL::HAL_Init()
{
    Serial.begin(115200);
    Power_Init();
    Backlight_Init();
    Encoder_Init();
    Clock_Init();
    Buzz_init();
    GPS_Init();
    I2C_Scan(true);
    IMU_Init();
    MAG_Init();
    Audio_Init();
    SD_Init();
    
    ADC_DMA_Init();
   
    Audio_PlayMusic("StartUp");
    
    Timer_SetInterrupt(TIM4, 10 * 1000, HAL_InterrputUpdate);
    TIM_Cmd(TIM4, ENABLE);
}

void HAL::HAL_Update()
{
    __IntervalExecute(SD_Update(), 500);
    GPS_Update();
}
