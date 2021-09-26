#ifndef __MCU_TYPE
#define __MCU_TYPE

#ifdef __cplusplus
extern "C" {
#endif

#include "mcu_cfg.h"
#include "stm32/stm32f4xx_conf.h"
#include "stm32f4xx.h"

// typedef GPIO_TypeDef        GPIO_Type;
// typedef GPIO_InitTypeDef    GPIO_InitType;
// typedef GPIOSpeed_TypeDef   GPIOMaxSpeed_Type;
// #define GPIO_MaxSpeed_50MHz GPIO_Speed_50MHz

// typedef RCC_ClocksTypeDef   RCC_ClocksType;
// #define RCC_APB2PERIPH_SPI1 RCC_APB2Periph_SPI1
// #define RCC_APB1PERIPH_SPI2 RCC_APB1Periph_SPI2
// #define RCC_APB1PERIPH_SPI3 RCC_APB1Periph_SPI3

// typedef TIM_TypeDef         TMR_Type;
// #define TMR_Cmd             TIM_Cmd

// typedef DMA_InitTypeDef     DMA_InitType;
// #define DMA_Reset           DMA_DeInit

// typedef I2C_TypeDef         I2C_Type;

// typedef NVIC_InitTypeDef    NVIC_InitType;

// typedef EXTI_InitTypeDef    EXTI_InitType;
// typedef EXTITrigger_TypeDef EXTITrigger_Type;

// typedef SPI_TypeDef         SPI_Type;
// typedef SPI_InitTypeDef     SPI_InitType;
// #define SPI_Enable          SPI_Cmd
// #define SPI_I2S_Reset       SPI_I2S_DeInit
// #define SPI_FRAMESIZE_8BIT  SPI_DataSize_8b
// #define SPI_FRAMESIZE_16BIT SPI_DataSize_16b
// #define SPI_CPOL_LOW        SPI_CPOL_Low
// #define SPI_CPOL_HIGH       SPI_CPOL_High
// #define SPI_CPHA_1EDGE      SPI_CPHA_1Edge
// #define SPI_CPHA_2EDGE      SPI_CPHA_2Edge
// #define SPI_MODE_MASTER     SPI_Mode_Master
// #define SPI_MODE_SLAVE      SPI_Mode_Slave
// #define SPI_NSSSEL_SOFT     SPI_NSS_Soft
// #define SPI_NSSSEL_HARD     SPI_NSS_Hard
// #define SPI_MCLKP_2         SPI_BaudRatePrescaler_2
// #define SPI_MCLKP_4         SPI_BaudRatePrescaler_4
// #define SPI_MCLKP_8         SPI_BaudRatePrescaler_8
// #define SPI_MCLKP_16        SPI_BaudRatePrescaler_16
// #define SPI_MCLKP_32        SPI_BaudRatePrescaler_32
// #define SPI_MCLKP_64        SPI_BaudRatePrescaler_64
// #define SPI_MCLKP_128       SPI_BaudRatePrescaler_128
// #define SPI_MCLKP_256       SPI_BaudRatePrescaler_256
// #define SPI_FIRSTBIT_LSB    SPI_FirstBit_LSB
// #define SPI_FIRSTBIT_MSB    SPI_FirstBit_MSB
// #define SPI_I2S_FLAG_RNE    SPI_I2S_FLAG_RXNE
// #define SPI_I2S_FLAG_TE     SPI_I2S_FLAG_TXE
// #define SPI_I2S_FLAG_BUSY   SPI_I2S_FLAG_BSY
// #define SPI_STS_BSY         SPI_FLAG_BSY
// #define SPI_STS_TE          SPI_FLAG_TXE

// typedef USART_TypeDef       USART_Type;
// typedef USART_InitTypeDef   USART_InitType;
// #define USART_INT_RDNE  USART_IT_RXNE
// #define USART_FLAG_TDE  USART_FLAG_TXE
// #define USART_INTConfig USART_ITConfig

// typedef ADC_TypeDef         ADC_Type;
// typedef ADC_InitTypeDef     ADC_InitType;
// #define ADC_Reset           ADC_DeInit
// #define ADC_FLAG_EC         ADC_FLAG_EOC
// #define ADC_INT_EC              ADC_IT_EOC
// #define ADC_GetINTStatus        ADC_GetITStatus
// #define ADC_ClearINTPendingBit  ADC_ClearITPendingBit
// #define ADC_ExternalTrig_None   ADC_ExternalTrigConvEdge_None
// #define RCC_APB2CLK_Div8        RCC_HCLK_Div8
// #define RCC_APB2PERIPH_ADC1     RCC_APB2Periph_ADC1

#define __wfi __WFI

#define TRACE(x)
#ifndef  ARM_MATH_CM4
#define  ARM_MATH_CM4
#endif
#include <arm_math.h>

#define __STM32__
#define __STM32F4__
#define F_CPU                   SystemCoreClock
#define CYCLES_PER_MICROSECOND (F_CPU / 1000000U)

#define __KEILDUINO__ 220

#ifdef __cplusplus
}
#endif

#endif
