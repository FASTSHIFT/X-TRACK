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

#define SERIAL_GET_WORDLENGTH(SERIAL_x)    ((uint16_t)(SERIAL_x&0xF000))
#define SERIAL_GET_PARITY(SERIAL_x)        ((uint16_t)(SERIAL_x&0x0F00))
#define SERIAL_GET_STOPBITS(SERIAL_x)      ((uint16_t)((SERIAL_x&0x00F0) << 8))

/**
  * @brief  串口对象构造函数
  * @param  串口外设地址
  * @retval 无
  */
HardwareSerial::HardwareSerial(USART_Type* usart)
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
    if(USART_GetITStatus(_USARTx, USART_INT_RDNE) != RESET)
    {
        uint8_t c = USART_ReceiveData(_USARTx);
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

        USART_ClearITPendingBit(_USARTx, USART_INT_RDNE);
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
    GPIO_InitType GPIO_InitStructure;
    USART_InitType USART_InitStructure;
    NVIC_InitType NVIC_InitStructure;
    uint16_t Tx_Pin, Rx_Pin;
    uint16_t ItChannel;
    GPIO_TypeDef *GPIOx;

    if(_USARTx == USART1)
    {
        Tx_Pin = GPIO_Pin_9;
        Rx_Pin = GPIO_Pin_10;
        GPIOx = GPIOA;
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_USART1, ENABLE);
        ItChannel = USART1_IRQn;
    }
    else if(_USARTx == USART2)
    {
        Tx_Pin = GPIO_Pin_2;
        Rx_Pin = GPIO_Pin_3;
        GPIOx = GPIOA;
        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOA, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART2, ENABLE);
        ItChannel = USART2_IRQn;
    }
    else if(_USARTx == USART3)
    {
        Tx_Pin = GPIO_Pin_10;
        Rx_Pin = GPIO_Pin_11;
        GPIOx = GPIOB;

        RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIOB, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1PERIPH_USART3, ENABLE);
        ItChannel = USART3_IRQn;
    }
    else
    {
        return;
    }

    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pins =  Tx_Pin | Rx_Pin;
    GPIO_InitStructure.GPIO_MaxSpeed = GPIO_MaxSpeed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOx, &GPIO_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = ItChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = preemptionPriority ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = subPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate = baudRate;
    USART_InitStructure.USART_WordLength = SERIAL_GET_WORDLENGTH(config);
    USART_InitStructure.USART_StopBits = SERIAL_GET_STOPBITS(config);
    USART_InitStructure.USART_Parity = SERIAL_GET_PARITY(config);
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(_USARTx, &USART_InitStructure);

    USART_INTConfig(_USARTx, USART_INT_RDNE, ENABLE);
    USART_Cmd(_USARTx, ENABLE);
}

/**
  * @brief  关闭串口
  * @param  无
  * @retval 无
  */
void HardwareSerial::end(void)
{
    USART_Cmd(_USARTx, DISABLE);
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
    while(!USART_GetFlagStatus(_USARTx, USART_FLAG_TDE)) {};
    USART_SendData(_USARTx, n);
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
