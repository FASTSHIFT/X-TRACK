/*
 * MIT License
 * Copyright (c) 2017 - 2022 _VIFEXTech
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "HardwareSerial.h"

typedef struct
{
    usart_data_bit_num_type data_bit;
    usart_parity_selection_type parity_selection;
    usart_stop_bit_num_type stop_bit;
} SERIAL_ConfigGrp_t;

static const SERIAL_ConfigGrp_t SERIAL_ConfigGrp[] =
{
    {USART_DATA_7BITS, USART_PARITY_NONE, USART_STOP_1_BIT},   // SERIAL_7N1
    {USART_DATA_7BITS, USART_PARITY_NONE, USART_STOP_2_BIT},   // SERIAL_7N2
    {USART_DATA_7BITS, USART_PARITY_EVEN, USART_STOP_1_BIT},   // SERIAL_7E1
    {USART_DATA_7BITS, USART_PARITY_EVEN, USART_STOP_2_BIT},   // SERIAL_7E2
    {USART_DATA_7BITS, USART_PARITY_ODD,  USART_STOP_1_BIT},   // SERIAL_7O1
    {USART_DATA_7BITS, USART_PARITY_ODD,  USART_STOP_2_BIT},   // SERIAL_7O2
    {USART_DATA_7BITS, USART_PARITY_NONE, USART_STOP_0_5_BIT}, // SERIAL_7N0_5
    {USART_DATA_7BITS, USART_PARITY_NONE, USART_STOP_1_5_BIT}, // SERIAL_7N1_5
    {USART_DATA_7BITS, USART_PARITY_EVEN, USART_STOP_0_5_BIT}, // SERIAL_7E0_5
    {USART_DATA_7BITS, USART_PARITY_EVEN, USART_STOP_1_5_BIT}, // SERIAL_7E1_5
    {USART_DATA_7BITS, USART_PARITY_ODD,  USART_STOP_0_5_BIT}, // SERIAL_7O0_5
    {USART_DATA_7BITS, USART_PARITY_ODD,  USART_STOP_1_5_BIT}, // SERIAL_7O1_5

    {USART_DATA_8BITS, USART_PARITY_NONE, USART_STOP_1_BIT},   // SERIAL_8N1
    {USART_DATA_8BITS, USART_PARITY_NONE, USART_STOP_2_BIT},   // SERIAL_8N2
    {USART_DATA_8BITS, USART_PARITY_EVEN, USART_STOP_1_BIT},   // SERIAL_8E1
    {USART_DATA_8BITS, USART_PARITY_EVEN, USART_STOP_2_BIT},   // SERIAL_8E2
    {USART_DATA_8BITS, USART_PARITY_ODD,  USART_STOP_1_BIT},   // SERIAL_8O1
    {USART_DATA_8BITS, USART_PARITY_ODD,  USART_STOP_2_BIT},   // SERIAL_8O2
    {USART_DATA_8BITS, USART_PARITY_NONE, USART_STOP_0_5_BIT}, // SERIAL_8N0_5
    {USART_DATA_8BITS, USART_PARITY_NONE, USART_STOP_1_5_BIT}, // SERIAL_8N1_5
    {USART_DATA_8BITS, USART_PARITY_EVEN, USART_STOP_0_5_BIT}, // SERIAL_8E0_5
    {USART_DATA_8BITS, USART_PARITY_EVEN, USART_STOP_1_5_BIT}, // SERIAL_8E1_5
    {USART_DATA_8BITS, USART_PARITY_ODD,  USART_STOP_0_5_BIT}, // SERIAL_8O0_5
    {USART_DATA_8BITS, USART_PARITY_ODD,  USART_STOP_1_5_BIT}, // SERIAL_8O1_5

    {USART_DATA_9BITS, USART_PARITY_NONE, USART_STOP_1_BIT},   // SERIAL_9N1
    {USART_DATA_9BITS, USART_PARITY_NONE, USART_STOP_2_BIT},   // SERIAL_9N2
    {USART_DATA_9BITS, USART_PARITY_EVEN, USART_STOP_1_BIT},   // SERIAL_9E1
    {USART_DATA_9BITS, USART_PARITY_EVEN, USART_STOP_2_BIT},   // SERIAL_9E2
    {USART_DATA_9BITS, USART_PARITY_ODD,  USART_STOP_1_BIT},   // SERIAL_9O1
    {USART_DATA_9BITS, USART_PARITY_ODD,  USART_STOP_2_BIT},   // SERIAL_9O2
    {USART_DATA_9BITS, USART_PARITY_NONE, USART_STOP_0_5_BIT}, // SERIAL_9N0_5
    {USART_DATA_9BITS, USART_PARITY_NONE, USART_STOP_1_5_BIT}, // SERIAL_9N1_5
    {USART_DATA_9BITS, USART_PARITY_EVEN, USART_STOP_0_5_BIT}, // SERIAL_9E0_5
    {USART_DATA_9BITS, USART_PARITY_EVEN, USART_STOP_1_5_BIT}, // SERIAL_9E1_5
    {USART_DATA_9BITS, USART_PARITY_ODD,  USART_STOP_0_5_BIT}, // SERIAL_9O0_5
    {USART_DATA_9BITS, USART_PARITY_ODD,  USART_STOP_1_5_BIT}, // SERIAL_9O1_5
};

/**
  * @brief  串口对象构造函数
  * @param  串口外设地址
  * @retval 无
  */
HardwareSerial::HardwareSerial(usart_type* usart)
    : _USARTx(usart)
    , _callbackFunction(NULL)
    , _rxBufferHead(0)
    , _rxBufferTail(0)
{
    memset(_rxBuffer, 0, sizeof(_rxBuffer));
}

/**
  * @brief  串口中断入口
  * @param  无
  * @retval 无
  */
void HardwareSerial::IRQHandler()
{
    if(usart_flag_get(_USARTx, USART_RDBF_FLAG) != RESET)
    {
        uint8_t c = usart_data_receive(_USARTx);
        uint16_t i = (uint16_t)(_rxBufferHead + 1) % SERIAL_RX_BUFFER_SIZE;
        if (i != _rxBufferTail)
        {
            _rxBuffer[_rxBufferHead] = c;
            _rxBufferHead = i;
        }

        if(_callbackFunction)
        {
            _callbackFunction(this);
        }
        usart_flag_clear(_USARTx, USART_RDBF_FLAG);
    }
}

/**
  * @brief  串口初始化
  * @param  BaudRate: 波特率
  * @param  Config: 配置参数
  * @param  PreemptionPriority: 抢占优先级
  * @param  SubPriority: 从优先级
  * @retval 无
  */
void HardwareSerial::begin(
    uint32_t baudRate,
    SERIAL_Config_t config,
    uint8_t preemptionPriority,
    uint8_t subPriority
)
{
    gpio_type *GPIOx;
    gpio_init_type gpio_init_struct;
    uint16_t Tx_Pin, Rx_Pin;
    IRQn_Type USARTx_IRQn;

    if(_USARTx == USART1)
    {
        GPIOx = GPIOA;
        Tx_Pin = GPIO_Pin_9;
        Rx_Pin = GPIO_Pin_10;
        USARTx_IRQn = USART1_IRQn;

        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
        crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE);
    }
    else if(_USARTx == USART2)
    {
        GPIOx = GPIOA;
        Tx_Pin = GPIO_Pin_2;
        Rx_Pin = GPIO_Pin_3;
        USARTx_IRQn = USART2_IRQn;

        crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
        crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, TRUE);
    }
    else if(_USARTx == USART3)
    {
        GPIOx = GPIOB;
        Tx_Pin = GPIO_Pin_10;
        Rx_Pin = GPIO_Pin_11;
        USARTx_IRQn = USART3_IRQn;

        crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
        crm_periph_clock_enable(CRM_USART3_PERIPH_CLOCK, TRUE);
    }
    else
    {
        return;
    }

    gpio_default_para_init(&gpio_init_struct);
    gpio_init_struct.gpio_pins =  Tx_Pin | Rx_Pin;
    gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
    gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
    gpio_init(GPIOx, &gpio_init_struct);

    gpio_pin_mux_config(GPIOx, GPIO_GetPinSource(Tx_Pin), GPIO_MUX_7);
    gpio_pin_mux_config(GPIOx, GPIO_GetPinSource(Rx_Pin), GPIO_MUX_7);

    usart_init(_USARTx, baudRate, SERIAL_ConfigGrp[config].data_bit, SERIAL_ConfigGrp[config].stop_bit);
    usart_parity_selection_config(_USARTx, SERIAL_ConfigGrp[config].parity_selection);
    usart_transmitter_enable(_USARTx, TRUE);
    usart_receiver_enable(_USARTx, TRUE);

    nvic_irq_enable(USARTx_IRQn, preemptionPriority, subPriority);
    usart_interrupt_enable(_USARTx, USART_RDBF_INT, TRUE);

    usart_enable(_USARTx, TRUE);
}

/**
  * @brief  关闭串口
  * @param  无
  * @retval 无
  */
void HardwareSerial::end(void)
{
    usart_interrupt_enable(_USARTx, USART_RDBF_INT, FALSE);
    usart_enable(_USARTx, FALSE);
}

/**
  * @brief  串口中断回调
  * @param  Function: 回调函数
  * @retval 无
  */
void HardwareSerial::attachInterrupt(CallbackFunction_t func)
{
    _callbackFunction = func;
}

/**
  * @brief  获取可从串行端口读取的字节数
  * @param  无
  * @retval 可读取的字节数
  */
int HardwareSerial::available(void)
{
    return ((unsigned int)(SERIAL_RX_BUFFER_SIZE + _rxBufferHead - _rxBufferTail)) % SERIAL_RX_BUFFER_SIZE;
}

/**
  * @brief  读取传入的串行数据(字符)
  * @param  无
  * @retval 可用的传入串行数据的第一个字节 (如果没有可用的数据, 则为-1)
  */
int HardwareSerial::read(void)
{
    // if the head isn't ahead of the tail, we don't have any characters
    if (_rxBufferHead == _rxBufferTail)
    {
        return -1;
    }
    else
    {
        uint8_t c = _rxBuffer[_rxBufferTail];
        _rxBufferTail = (uint16_t)(_rxBufferTail + 1) % SERIAL_RX_BUFFER_SIZE;
        return c;
    }
}

/**
  * @brief  返回传入串行数据的下一个字节(字符), 而不将其从内部串行缓冲区中删除
  * @param  无
  * @retval 可用的传入串行数据的第一个字节 (如果没有可用的数据, 则为-1)
  */
int HardwareSerial::peek(void)
{
    if (_rxBufferHead == _rxBufferTail)
    {
        return -1;
    }
    else
    {
        return _rxBuffer[_rxBufferTail];
    }
}

/**
  * @brief  清空串口缓存
  * @param  无
  * @retval 无
  */
void HardwareSerial::flush(void)
{
    _rxBufferHead = _rxBufferTail;
}

/**
  * @brief  串口写入一个字节
  * @param  写入的字节
  * @retval 字节
  */
size_t HardwareSerial::write(uint8_t n)
{
    while(usart_flag_get(_USARTx, USART_TDBE_FLAG) == RESET) {};
    usart_data_transmit(_USARTx, n);
    return 1;
}

#if SERIAL_1_ENABLE
HardwareSerial Serial(SERIAL_1_USART);

extern "C" SERIAL_1_IRQ_HANDLER_DEF()
{
    Serial.IRQHandler();
}
#endif

#if SERIAL_2_ENABLE
HardwareSerial Serial2(SERIAL_2_USART);

extern "C" SERIAL_2_IRQ_HANDLER_DEF()
{
    Serial2.IRQHandler();
}
#endif

#if SERIAL_3_ENABLE
HardwareSerial Serial3(SERIAL_3_USART);

extern "C" SERIAL_3_IRQ_HANDLER_DEF()
{
    Serial3.IRQHandler();
}
#endif
