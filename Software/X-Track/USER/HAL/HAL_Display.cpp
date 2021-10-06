#include "HAL.h"
#include "Adafruit_ST7789/Adafruit_ST7789.h"
#include "Adafruit_GFX_Library/Fonts/FreeMono12pt7b.h"

#define DISP_USE_FPS_TEST    0
#define DISP_USE_DMA         1
#define DISP_DMA_CHANNEL     DMA1_Channel3
#define DISP_DMA_MAX_SIZE    65535

typedef Adafruit_ST7789 SCREEN_CLASS;

static SCREEN_CLASS screen(
    CONFIG_SCREEN_CS_PIN,
    CONFIG_SCREEN_DC_PIN,
    CONFIG_SCREEN_RST_PIN
);

static uint8_t* Disp_DMA_TragetPoint = nullptr;
static uint8_t* Disp_DMA_CurrentPoint = nullptr;
static HAL::Display_CallbackFunc_t Disp_Callback = nullptr;

#if (DISP_USE_FPS_TEST == 1)
static float Display_GetFPS(SCREEN_CLASS* scr, uint32_t loopNum)
{
    uint32_t startTime = millis();
    for(uint32_t f = 0; f < loopNum; f++)
    {
        scr->fillScreen(SCREEN_CLASS::Red);
        scr->fillScreen(SCREEN_CLASS::Green);
        scr->fillScreen(SCREEN_CLASS::Blue);
    }
    uint32_t costTime = millis() - startTime;
    float fps = loopNum * 3 / (costTime / 1000.0f);

    scr->fillScreen(SCREEN_CLASS::Blue);
    scr->setTextSize(1);
    scr->setTextColor(SCREEN_CLASS::White);
    scr->setCursor(0, scr->height() / 2);

    scr->print("Frame:");
    scr->println(loopNum * 3);

    scr->print("Time:");
    scr->print(costTime);
    scr->println("ms");

    scr->print("FPS:");
    scr->println(fps);

    return fps;
}
#endif

static void Display_SPI_DMA_Send(const void* buf, uint32_t size)
{
    if(size > DISP_DMA_MAX_SIZE)
    {
        if(Disp_DMA_TragetPoint == NULL)
        {
            Disp_DMA_TragetPoint = (uint8_t*)buf + size;
        }
        Disp_DMA_CurrentPoint = (uint8_t*)buf + DISP_DMA_MAX_SIZE;
        size = DISP_DMA_MAX_SIZE;
    }
    else
    {
        Disp_DMA_CurrentPoint = NULL;
        Disp_DMA_TragetPoint = NULL;
    }

    DMA_ChannelEnable(DISP_DMA_CHANNEL, DISABLE);
    DISP_DMA_CHANNEL->CMBA = (uint32_t)buf;
    DISP_DMA_CHANNEL->TCNT = size;
    DMA_ChannelEnable(DISP_DMA_CHANNEL, ENABLE);
}

extern "C" void DMA1_Channel3_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_INT_TC3) != RESET)
    {
        DMA_ClearITPendingBit(DMA1_INT_TC3);
        if(Disp_DMA_CurrentPoint < Disp_DMA_TragetPoint)
        {
            Display_SPI_DMA_Send(Disp_DMA_CurrentPoint, Disp_DMA_TragetPoint - Disp_DMA_CurrentPoint);
        }
        else
        {
            digitalWrite_HIGH(CONFIG_SCREEN_CS_PIN);
            if(Disp_Callback)
            {
                Disp_Callback();
            }
        }
    }
}

static void Display_SPI_DMA_Init()
{
    RCC_AHBPeriphClockCmd(RCC_AHBPERIPH_DMA1, ENABLE);

    DMA_Reset(DISP_DMA_CHANNEL);

    DMA_InitType DMA_InitStructure;
    DMA_DefaultInitParaConfig(&DMA_InitStructure);

    DMA_InitStructure.DMA_Mode = DMA_MODE_NORMAL;  //��������������ģʽ
    DMA_InitStructure.DMA_MTOM = DMA_MEMTOMEM_DISABLE;  //DMAͨ�� û������Ϊ�ڴ浽�ڴ洫��
    DMA_InitStructure.DMA_Direction = DMA_DIR_PERIPHERALDST;  //���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
    DMA_InitStructure.DMA_Priority = DMA_PRIORITY_MEDIUM; //DMAͨ�� �����ȼ�

    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0;  //DMA�ڴ����ַ
    DMA_InitStructure.DMA_MemoryInc = DMA_MEMORYINC_ENABLE;  //�ڴ��ַ�Ĵ�������
    DMA_InitStructure.DMA_MemoryDataWidth = DMA_MEMORYDATAWIDTH_BYTE; //���ݿ��
    DMA_InitStructure.DMA_BufferSize = DISP_DMA_MAX_SIZE;  //DMAͨ����DMA����Ĵ�С

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(SPI1->DT));  //DMA����SPI����ַ
    DMA_InitStructure.DMA_PeripheralInc = DMA_PERIPHERALINC_DISABLE;  //�����ַ�Ĵ�������
    DMA_InitStructure.DMA_PeripheralDataWidth = DMA_PERIPHERALDATAWIDTH_BYTE;  //���ݿ��

    DMA_Init(DISP_DMA_CHANNEL, &DMA_InitStructure);

    SPI_I2S_DMAEnable(SPI1, SPI_I2S_DMA_TX, ENABLE);

    NVIC_EnableIRQ(DMA1_Channel3_IRQn);

    DMA_INTConfig(DISP_DMA_CHANNEL, DMA_INT_TC, ENABLE);
}

void HAL::Display_Init()
{
    Serial.print("Display: init...");
    screen.begin();
    screen.setRotation(0);
    screen.fillScreen(screen.Black);

    screen.setTextWrap(true);
    screen.setTextSize(1);
    screen.setCursor(0, 0);
    screen.setFont();
    screen.setTextColor(SCREEN_CLASS::White, SCREEN_CLASS::Blue);
    
    Display_SPI_DMA_Init();

#if (DISP_USE_FPS_TEST == 1)
    Display_GetFPS(&screen, 50);
    while(1);
#endif
    HAL::Backlight_SetGradual(1000, 1000);
    Serial.println("success");
}

void HAL::Display_DumpCrashInfo(const char* info)
{
#   define TEXT_HEIGHT_1   8
#   define TEXT_WIDTH_1    6
    HAL::Backlight_ForceLit(true);

    screen.fillScreen(SCREEN_CLASS::Blue);
    screen.setTextColor(SCREEN_CLASS::White);
    screen.setFont(&FreeMono12pt7b);
    screen.setTextSize(2);
    screen.setCursor(0, 34);
    screen.print(":(");

    screen.setFont();

    screen.setTextSize(1);
    screen.setCursor(0, screen.height() / 2 - TEXT_HEIGHT_1 - 5);
    screen.println(info);
    screen.print("Press KEY to reboot..");

    screen.setCursor(0, screen.height() - TEXT_HEIGHT_1 * 6);
    screen.println("Error code:");
    screen.printf("MMFAR = 0x%08X\r\n", SCB->MMFAR);
    screen.printf("BFAR  = 0x%08X\r\n", SCB->BFAR);
    screen.printf("CFSR  = 0x%08X\r\n", SCB->CFSR);
    screen.printf("HFSR  = 0x%08X\r\n", SCB->HFSR);
    screen.printf("DFSR  = 0x%08X\r\n", SCB->DFSR);
}

void HAL::Display_SetAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
    screen.setAddrWindow(x0, y0, x1, y1);
}

void HAL::Display_SendPixels(uint16_t* pixels, uint32_t len)
{
    digitalWrite_LOW(CONFIG_SCREEN_CS_PIN);
    digitalWrite_HIGH(CONFIG_SCREEN_DC_PIN);

    Display_SPI_DMA_Send(pixels, len * sizeof(uint16_t));
}

void HAL::Display_SetSendFinishCallback(Display_CallbackFunc_t func)
{
    Disp_Callback = func;
}
