#include "Arduino.h"
#include "Display/Display.h"
#include "HAL/HAL.h"

void setup()
{
    Serial.begin(115200);
    
    Display_Init();
    HAL::HAL_Init();
}

void loop()
{
    lv_task_handler();
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
