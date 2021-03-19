#include "Display/Display.h"
#include "cm_backtrace/cm_backtrace.h"
#include "Adafruit_GFX_Library/Fonts/FreeMono12pt7b.h"

static SCREEN_CLASS* screen = NULL;

#define TEXT_HEIGHT_1   8
#define TEXT_WIDTH_1    6

static void Delay(uint32_t ms)
{
    volatile uint32_t i = F_CPU / 1000 * ms / 5;
    while(i--);
}

static void Reboot()
{
    while(digitalRead(ENCODER_PUSH_PIN) == HIGH)
    {
        Delay(1000);
    }
    NVIC_SystemReset();
}

static void DisplayError_SetReports(const char* report)
{
    screen->fillScreen(SCREEN_CLASS::Blue);
    screen->setTextColor(SCREEN_CLASS::White);
    screen->setFont(&FreeMono12pt7b);
    screen->setTextSize(2);
    screen->setCursor(0, 34);
    screen->print(":(");

    screen->setFont();

    screen->setTextSize(1);
    screen->setCursor(0, screen->height() / 2 - TEXT_HEIGHT_1 - 5);
    screen->println(report);
    screen->print("Press KEY to reboot..");

    screen->setCursor(0, screen->height() - TEXT_HEIGHT_1 * 6);
    screen->println("Error code:");
    screen->printf("MMFAR = 0x%08X\r\n", SCB->MMFAR);
    screen->printf("BFAR  = 0x%08X\r\n", SCB->BFAR);
    screen->printf("CFSR  = 0x%08X\r\n", SCB->CFSR);
    screen->printf("HFSR  = 0x%08X\r\n", SCB->HFSR);
    screen->printf("DFSR  = 0x%08X\r\n", SCB->DFSR);
}

void DisplayFault_Init(SCREEN_CLASS* scr)
{
    screen = scr;
    cm_backtrace_init(SYS_FIRMWARE_NAME, SYS_HARDWARE_VERSION, SYS_SOFTWARE_VERSION" "__DATE__);
    
    screen->setTextWrap(true);
    screen->setTextSize(1);
    screen->setCursor(0, 0);
    screen->setFont();
    screen->setTextColor(SCREEN_CLASS::White, SCREEN_CLASS::Blue);
}

void cmb_printf(const char *__restrict __format, ...)
{
    char printf_buff[256];

    va_list args;
    va_start(args, __format);
    int ret_status = vsnprintf(printf_buff, sizeof(printf_buff), __format, args);
    va_end(args);
    
    Serial.print(printf_buff);
}

/* xFault handlers */
extern "C"
{
    /*
    void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName)
    {
        char strBuf[configMAX_TASK_NAME_LEN + 1];
        snprintf(strBuf, sizeof(strBuf), "stack overflow\n < %s >", pcTaskName);
        DisplayError_SetReports(strBuf);
        Reboot();
    }
    
    void vApplicationMallocFailedHook()
    {
        DisplayError_SetReports("malloc failed");
        Reboot();
    }
    */
    
    void vApplicationHardFaultHook()
    {
        DisplayError_SetReports("FXXK HardFault!");
        Reboot();
    }
    
    __asm void HardFault_Handler()
    {
        extern vApplicationHardFaultHook
        extern cm_backtrace_fault
            
        mov r0, lr
        mov r1, sp
        bl cm_backtrace_fault
        bl vApplicationHardFaultHook
Fault_Loop
        b Fault_Loop
    }
}
