#include "HAL.h"
#include "lvgl/lvgl.h"

/**
  * @brief  背光亮度渐变，受lv_anim控制
  * @param  obj:无用
  * @param  brightness:亮度值
  * @retval None
  */
static void Backlight_AnimCallback(void * obj, int32_t brightness)
{
    HAL::Backlight_SetValue(brightness);
}

/**
  * @brief  背光初始化
  * @param  无
  * @retval 无
  */
void HAL::Backlight_Init()
{
    /*PWM初始化，1000级，20KHz频率*/
    PWM_Init(CONFIG_SCREEN_BLK_PIN, 1000, 20000);
    Backlight_SetValue(0);
}

/**
  * @brief  背光设置，渐变效果
  * @param  target:目标亮度(0~1000 -> 0~100%)
  * @retval 无
  */
void HAL::Backlight_SetGradual(uint16_t target, uint16_t time)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)Backlight_AnimCallback);
    lv_anim_set_values(&a, Backlight_GetValue(), target);
    lv_anim_set_time(&a, time);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);

    lv_anim_start(&a);
}

/**
  * @brief  获取背光亮度
  * @param  无
  * @retval 当前亮度(0~1000 -> 0~100%)
  */
uint16_t HAL::Backlight_GetValue()
{
    return Timer_GetCompare(
               PIN_MAP[CONFIG_SCREEN_BLK_PIN].TIMx,
               PIN_MAP[CONFIG_SCREEN_BLK_PIN].TimerChannel
           );
}

/**
  * @brief  设置背光亮度
  * @param  val: 亮度(0~1000 -> 0~100%)
  * @retval 无
  */
void HAL::Backlight_SetValue(int16_t val)
{
    CM_VALUE_LIMIT(val, 0, 1000);
    analogWrite(CONFIG_SCREEN_BLK_PIN, val);
}

/**
  * @brief  背光强制点亮
  * @param  en: 背光使能
  * @retval 无
  */
void HAL::Backlight_ForceLit(bool en)
{
    pinMode(CONFIG_SCREEN_BLK_PIN, OUTPUT);
    digitalWrite(CONFIG_SCREEN_BLK_PIN, en);
}
