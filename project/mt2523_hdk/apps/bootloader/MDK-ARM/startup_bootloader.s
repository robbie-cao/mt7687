; ******************************************************************************
; * @file      startup_mt2523.s
; * @author    Hugh Chen, MediaTek MCD/IOT team
; * @version   V1.0
; * @date      2015-07-15
; * @brief     MT2523 startup for ARM RVDS toolchain.
; *
; ******************************************************************************

; fake Stack, to make compilation pass on KEIL MDK
; real main stack allocates at end of TCM
Stack_Size      EQU     0x8

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp

; fake Heap, to make compilation pass on KEIL MDK
; real heap allocates at ZI space
Heap_Size       EQU     0x8

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB

; Vector Table Mapped to head of RAM
                AREA    |.isr_vector|, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size
                IMPORT  |Image$$STACK$$ZI$$Limit|

__Vectors       DCD     |Image$$STACK$$ZI$$Limit|  ; Top of Stack
                DCD     Reset_Handler              ; Reset Handler


__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors

                AREA    |.reset_handler|, CODE, READONLY


; Reset handler
Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                EXPORT  JumpCmd
                IMPORT  bl_main
                IMPORT  |Image$$STACK$$ZI$$Base|
                IMPORT  |Image$$STACK$$ZI$$Limit|

                ;set stack pointer
                LDR     SP, =|Image$$STACK$$ZI$$Limit|

                ;interrupt disable
                CPSID   I
                CPSID   F

                ;cache disable
                LDR     R0, =0xA0880000
                MOVS    R1, #0
                STR     R1, [R0, #0]

                ;stack space zero init
                MOVS    R0, #0
                LDR     R1, =|Image$$STACK$$ZI$$Base|
                LDR     R2, =|Image$$STACK$$ZI$$Limit|
FillZero
                STR     R0, [R1], #4
                CMP     R1, R2
                BCC     FillZero
                ;disable interrupt
                CPSID   I
                ;Call the application's entry point
                LDR     R0, =bl_main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

;                B       .

;                ENDP

                ALIGN

      AREA |INTERNCODE|, CODE, READONLY
; TODO need to check JumpCmd can work on FPGA
JumpCmd			    
                ORR     r0, #0x01
                BX      r0

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

;************************ (C) COPYRIGHT MEDIATEK *****END OF FILE*****
