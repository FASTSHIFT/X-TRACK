#include "Arduino.h"
#include "Display/Display.h"
#include "HAL/HAL.h"
#include "App/App.h"
#include "lv_examples/lv_demo.h"

static void setup()
{
    HAL::HAL_Init();
    Display_Init();
    
    App_Init();
    //lv_demo_benchmark();
    //lv_demo_widgets();
    //lv_demo_keypad_encoder();
}

static void loop()
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
    //ADCx_Init(ADC1);
    setup();
    for(;;)loop();
}
