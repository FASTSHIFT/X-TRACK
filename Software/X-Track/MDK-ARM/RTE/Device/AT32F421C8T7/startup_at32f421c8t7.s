;**************************************************************************
;* File   : startup_at32f421c8t7.s
;* Version: V1.2.9
;* Date   : 2021-01-15
;* Brief  : at32f4xx startup file for keil
;**************************************************************************
;

; Amount of memory (in bytes) allocated for Stack
; Tailor this value to your application needs
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp

; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000200

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp                        ; Top of Stack
                DCD     Reset_Handler                       ; Reset Handler
                DCD     NMI_Handler                         ; NMI Handler
                DCD     HardFault_Handler                   ; Hard Fault Handler
                DCD     MemManage_Handler                   ; MPU Fault Handler
                DCD     BusFault_Handler                    ; Bus Fault Handler
                DCD     UsageFault_Handler                  ; Usage Fault Handler
                DCD     0                                   ; Reserved
                DCD     0                                   ; Reserved
                DCD     0                                   ; Reserved
                DCD     0                                   ; Reserved
                DCD     SVC_Handler                         ; SVCall Handler
                DCD     DebugMon_Handler                    ; Debug Monitor Handler
                DCD     0                                   ; Reserved
                DCD     PendSV_Handler                      ; PendSV Handler
                DCD     SysTick_Handler                     ; SysTick Handler

                ; External Interrupts
                DCD     WWDG_IRQHandler                     ; Window Watchdog
                DCD     PVD_IRQHandler                      ; PVD through EXTI Line detect
                DCD     ERTC_IRQHandler                     ; ERTC
                DCD     FLASH_IRQHandler                    ; Flash
                DCD     RCC_IRQHandler                      ; RCC
                DCD     EXTI1_0_IRQHandler                  ; EXTI Line [1:0]
                DCD     EXTI3_2_IRQHandler                  ; EXTI Line [3:2]
                DCD     EXTI15_4_IRQHandler                 ; EXTI Line [15:4]
                DCD     0                                   ; Reserved
                DCD     DMA1_Channel1_IRQHandler            ; DMA1 Channel 1
                DCD     DMA1_Channel3_2_IRQHandler          ; DMA1 Channel [3:2]
                DCD     DMA1_Channel7_4_IRQHandler          ; DMA1 Channel [7:4]
                DCD     ADC_COMP_IRQHandler                 ; ADC & COMP
                DCD     TMR1_BRK_UP_TRG_COM_IRQHandler      ; TMR1 Break Update Trigger Commutation
                DCD     TMR1_CC_IRQHandler                  ; TMR1 Capture Compare
                DCD     0                                   ; Reserved
                DCD     TMR3_GLOBAL_IRQHandler              ; TMR3
                DCD     TMR6_GLOBAL_IRQHandler              ; TMR6
                DCD     0                                   ; Reserved
                DCD     TMR14_GLOBAL_IRQHandler             ; TMR14
                DCD     TMR15_GLOBAL_IRQHandler             ; TMR15
                DCD     TMR16_GLOBAL_IRQHandler             ; TMR16
                DCD     TMR17_GLOBAL_IRQHandler             ; TMR17
                DCD     I2C1_EV_IRQHandler                  ; I2C1 Event
                DCD     I2C2_EV_IRQHandler                  ; I2C2 Event
                DCD     SPI1_IRQHandler                     ; SPI1
                DCD     SPI2_IRQHandler                     ; SPI2
                DCD     USART1_IRQHandler                   ; USART1
                DCD     USART2_IRQHandler                   ; USART2
                DCD     0                                   ; Reserved
                DCD     0                                   ; Reserved
                DCD     0                                   ; Reserved
                DCD     I2C1_ER_IRQHandler                  ; I2C1 Error
                DCD     0                                   ; Reserved
                DCD     I2C2_ER_IRQHandler                  ; I2C2 Error
__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

; Reset handler
Reset_Handler   PROC
                EXPORT  Reset_Handler                       [WEAK]
                IMPORT  __main
                IMPORT  SystemInit
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler                         [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler                   [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler                   [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler                    [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler                  [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler                         [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler                    [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler                      [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler                     [WEAK]
                B       .
                ENDP

Default_Handler PROC
                EXPORT  WWDG_IRQHandler                     [WEAK]
                EXPORT  PVD_IRQHandler                      [WEAK]
                EXPORT  ERTC_IRQHandler                     [WEAK]
                EXPORT  FLASH_IRQHandler                    [WEAK]
                EXPORT  RCC_IRQHandler                      [WEAK]
                EXPORT  EXTI1_0_IRQHandler                  [WEAK]
                EXPORT  EXTI3_2_IRQHandler                  [WEAK]
                EXPORT  EXTI15_4_IRQHandler                 [WEAK]
                EXPORT  DMA1_Channel1_IRQHandler            [WEAK]
                EXPORT  DMA1_Channel3_2_IRQHandler          [WEAK]
                EXPORT  DMA1_Channel7_4_IRQHandler          [WEAK]
                EXPORT  ADC_COMP_IRQHandler                 [WEAK]
                EXPORT  TMR1_BRK_UP_TRG_COM_IRQHandler      [WEAK]
                EXPORT  TMR1_CC_IRQHandler                  [WEAK]
                EXPORT  TMR3_GLOBAL_IRQHandler              [WEAK]
                EXPORT  TMR6_GLOBAL_IRQHandler              [WEAK]
                EXPORT  TMR14_GLOBAL_IRQHandler             [WEAK]
                EXPORT  TMR15_GLOBAL_IRQHandler             [WEAK]
                EXPORT  TMR16_GLOBAL_IRQHandler             [WEAK]
                EXPORT  TMR17_GLOBAL_IRQHandler             [WEAK]
                EXPORT  I2C1_EV_IRQHandler                  [WEAK]
                EXPORT  I2C2_EV_IRQHandler                  [WEAK]
                EXPORT  SPI1_IRQHandler                     [WEAK]
                EXPORT  SPI2_IRQHandler                     [WEAK]
                EXPORT  USART1_IRQHandler                   [WEAK]
                EXPORT  USART2_IRQHandler                   [WEAK]
                EXPORT  I2C1_ER_IRQHandler                  [WEAK]
                EXPORT  I2C2_ER_IRQHandler                  [WEAK]
WWDG_IRQHandler
PVD_IRQHandler
ERTC_IRQHandler
FLASH_IRQHandler
RCC_IRQHandler
EXTI1_0_IRQHandler
EXTI3_2_IRQHandler
EXTI15_4_IRQHandler
DMA1_Channel1_IRQHandler
DMA1_Channel3_2_IRQHandler
DMA1_Channel7_4_IRQHandler
ADC_COMP_IRQHandler
TMR1_BRK_UP_TRG_COM_IRQHandler
TMR1_CC_IRQHandler
TMR3_GLOBAL_IRQHandler
TMR6_GLOBAL_IRQHandler
TMR14_GLOBAL_IRQHandler
TMR15_GLOBAL_IRQHandler
TMR16_GLOBAL_IRQHandler
TMR17_GLOBAL_IRQHandler
I2C1_EV_IRQHandler
I2C2_EV_IRQHandler
SPI1_IRQHandler
SPI2_IRQHandler
USART1_IRQHandler
USART2_IRQHandler
I2C1_ER_IRQHandler
I2C2_ER_IRQHandler
                B       .

                ENDP

                ALIGN

;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                 IF      :DEF:__MICROLIB

                 EXPORT  __initial_sp
                 EXPORT  __heap_base
                 EXPORT  __heap_limit

                 ELSE

                 IMPORT  __use_two_region_memory
                 EXPORT  __user_initial_stackheap

__user_initial_stackheap

                 LDR     R0, =  Heap_Mem
                 LDR     R1, =(Stack_Mem + Stack_Size)
                 LDR     R2, = (Heap_Mem +  Heap_Size)
                 LDR     R3, = Stack_Mem
                 BX      LR

                 ALIGN

                 ENDIF

                 END
