#include "Arduino.h"
#include "Display/Display.h"
#include "HAL/HAL.h"

void setup()
{
    HAL::HAL_Init();
    Display_Init();
}

void loop()
{
    HAL::HAL_Update();
    lv_task_handler();
    __wfi();
}

/**
  * @brief  Main Function
  * @param  None
  * @retval None
  */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    GPIO_JTAG_Disable();
    Delay_Init();
    ADCx_Init(ADC1);
    setup();
    for(;;)loop();
}
