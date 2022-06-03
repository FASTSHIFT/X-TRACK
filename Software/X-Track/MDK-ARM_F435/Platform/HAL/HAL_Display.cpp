#include "HAL/HAL.h"
#include "Adafruit_ST7789/Adafruit_ST7789.h"
#include "Adafruit_GFX_Library/Fonts/FreeMono12pt7b.h"

#define DISP_USE_FPS_TEST    0
#define DISP_USE_DMA         1
#define DISP_DMA_CHANNEL     DMA1_CHANNEL3
#define DISP_DMA_MAX_SIZE    65535

typedef Adafruit_ST7789 SCREEN_CLASS;

static SCREEN_CLASS screen(
    CONFIG_SCREEN_CS_PIN,
    CONFIG_SCREEN_DC_PIN,
    CONFIG_SCREEN_RST_PIN,
    &CONFIG_SCREEN_SPI,
    CONFIG_SCREEN_HOR_RES,
    CONFIG_SCREEN_VER_RES
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
        scr->fillScreen(SCREEN_CLASS::COLOR_RED);
        scr->fillScreen(SCREEN_CLASS::COLOR_GREEN);
        scr->fillScreen(SCREEN_CLASS::COLOR_BLUE);
    }
    uint32_t costTime = millis() - startTime;
    float fps = loopNum * 3 / (costTime / 1000.0f);

    scr->fillScreen(SCREEN_CLASS::COLOR_BLUE);
    scr->setTextSize(1);
    scr->setTextColor(SCREEN_CLASS::COLOR_WHITE);
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

    dma_channel_enable(DISP_DMA_CHANNEL, FALSE);
    DISP_DMA_CHANNEL->maddr = (uint32_t)buf;
    DISP_DMA_CHANNEL->dtcnt_bit.cnt = size;
    dma_channel_enable(DISP_DMA_CHANNEL, TRUE);
}

extern "C" void DMA1_Channel3_IRQHandler(void)
{
    if(dma_flag_get(DMA1_FDT3_FLAG) != RESET)
    {
        dma_flag_clear(DMA1_FDT3_FLAG);
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
    crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);

    dma_reset(DISP_DMA_CHANNEL);

    dma_init_type dma_init_struct;
    dma_default_para_init(&dma_init_struct);

    dma_init_struct.buffer_size = DISP_DMA_MAX_SIZE;
    dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_base_addr = (uint32_t)NULL;
    dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
    dma_init_struct.memory_inc_enable = TRUE;
    dma_init_struct.peripheral_base_addr = (uint32_t)&SPI1->dt;
    dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
    dma_init_struct.peripheral_inc_enable = FALSE;
    dma_init_struct.priority = DMA_PRIORITY_MEDIUM;
    dma_init_struct.loop_mode_enable = FALSE;

    dma_init(DISP_DMA_CHANNEL, &dma_init_struct);
    
    dmamux_enable(DMA1, TRUE);
    dmamux_init(DMA1MUX_CHANNEL3, DMAMUX_DMAREQ_ID_SPI1_TX);

    spi_i2s_dma_transmitter_enable(SPI1, TRUE);

    NVIC_EnableIRQ(DMA1_Channel3_IRQn);

    dma_interrupt_enable(DISP_DMA_CHANNEL, DMA_FDT_INT, TRUE);
}

void HAL::Display_Init()
{
    Serial.print("Display: init...");
    screen.begin();
    screen.setRotation(0);
    screen.fillScreen(SCREEN_CLASS::COLOR_BLACK);

    screen.setTextWrap(true);
    screen.setTextSize(1);
    screen.setCursor(0, 0);
    screen.setFont();
    screen.setTextColor(SCREEN_CLASS::COLOR_WHITE, SCREEN_CLASS::COLOR_BLUE);

    Display_SPI_DMA_Init();

#if (DISP_USE_FPS_TEST == 1)
    HAL::Backlight_ForceLit(true);
    Display_GetFPS(&screen, 100);
    while(1);
#endif
    
    HAL::Backlight_SetGradual(1000, 1000);
    Serial.println("success");
}

void HAL::Display_DumpCrashInfo(const char* info)
{
#   define TEXT_HEIGHT_1   8
#   define TEXT_WIDTH_1    6

    screen.fillScreen(SCREEN_CLASS::COLOR_BLUE);
    screen.setTextColor(SCREEN_CLASS::COLOR_WHITE);
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

void HAL::Display_SendPixels(const uint16_t* pixels, uint32_t len)
{
    digitalWrite_LOW(CONFIG_SCREEN_CS_PIN);
    digitalWrite_HIGH(CONFIG_SCREEN_DC_PIN);

    Display_SPI_DMA_Send(pixels, len * sizeof(uint16_t));
}

void HAL::Display_SetSendFinishCallback(Display_CallbackFunc_t func)
{
    Disp_Callback = func;
}
