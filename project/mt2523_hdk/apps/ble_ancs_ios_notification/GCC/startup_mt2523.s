/**
  ******************************************************************************
  * @file      startup_mt2523.s
  * @author    Hugh Chen, MediaTek MCD/IOT team
  * @version   V1.0
  * @date      2015-07-15
  * @brief     MT2523 startup for GCC toolchain. 
  *
  ******************************************************************************
  */

.file "startup_mt2523.s"
.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.global  g_pfnVectors
.global  Default_Handler

/**
 * @brief  reset_handler is the entry point that processor starts to boot
 * @param  None
 * @retval : None
*/

  .section  .reset_handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function
Reset_Handler:

/* set stack pointer */
  ldr  sp, =_stack_end

/* interrupt disable */
  cpsid i

/* cache disable */
  movs  r0, #0
  movs  r1, #3
  ldr  r2, =0xA0880000
  str  r0, [r2], #4
  str  r1, [r2, #0]

/* watch dog disable */
  ldr  r0, =0xA2050000
  ldr  r1, =0x2200
  str  r1, [r0, #0]

/* make virtual space available */
  ldr  r0, =0x10000023
  movs  r1, #0
  ldr  r2, =0xA0881000
  str  r0, [r2], #4
  str  r1, [r2, #0]

/* stack space zero init */
  movs  r0, #0
  ldr  r1, =_stack_start
  ldr  r2, =_stack_end
FillZero:
  str  r0, [r1], #4
  cmp  r1, r2
  blo  FillZero

/* ram_text section init */
  ldr  r1, =_ram_code_load
  ldr  r2, =_ram_code_start
  ldr  r3, =_ram_code_end
  bl  Data_Init

/* noncached_data section init */
  ldr  r1, =_ram_noncached_rw_load
  ldr  r2, =_ram_noncached_rw_start
  ldr  r3, =_ram_noncached_rw_end
  bl  Data_Init

  ldr  r2, =_ram_noncached_zi_start
  ldr  r3, =_ram_noncached_zi_end
  bl  Bss_Init

/* cached_data section */
  ldr  r1, =_data_load
  ldr  r2, =_data_start
  ldr  r3, =_data_end
  bl  Data_Init

  ldr  r2, =_bss_start
  ldr  r3, =_bss_end
  bl  Bss_Init

/* tcm section init */
  ldr  r1, =_tcm_text_load
  ldr  r2, =_tcm_text_start
  ldr  r3, =_tcm_text_end
  bl  Data_Init

  ldr  r2, =_tcm_zi_start
  ldr  r3, =_tcm_zi_end
  bl  Bss_Init

/* Call the clock system intitialization function.*/
  bl  SystemInit

/* Call the application's entry point.*/
  bl  main
  bx  lr
.size  Reset_Handler, .-Reset_Handler

/**
 * @brief  This is data init sub-function
 * @param  None
 * @retval None
*/
  .section  .text.Data_Init,"ax",%progbits
Data_Init:
CopyDataLoop:
  cmp     r2, r3
  ittt    lo
  ldrlo   r0, [r1], #4
  strlo   r0, [r2], #4
  blo     CopyDataLoop
  bx  lr
  .size  Data_Init, .-Data_Init

/**
 * @brief  This is bss init sub-function
 * @param  None
 * @retval None
*/
  .section  .text.Bss_Init,"ax",%progbits
Bss_Init:
ZeroBssLoop:
  cmp     r2, r3
  ittt    lo
  movlo   r0, #0
  strlo   r0, [r2], #4
  blo     ZeroBssLoop
  bx  lr
  .size  Bss_Init, .-Bss_Init

/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 * @param  None
 * @retval None
*/
  .section  .text.Default_Handler,"ax",%progbits
Default_Handler:
Infinite_Loop:
  b  Infinite_Loop
  .size  Default_Handler, .-Default_Handler

/**
 * @brief  This is the code for system exception handling(svcCall,systick,pendSV), must suspend 
           flash before fetch code from flash
 * @param  None
 * @retval None
*/

  .section  .ram_code,"ax",%progbits
  .type  SvcCall_Main, %function
  .global  SvcCall_Main
SvcCall_Main:
  cpsid i
  push {lr}
  blx Flash_ReturnReady
  pop  {lr}
  cpsie i 
  b SVC_Handler
  .size  SvcCall_Main, .-SvcCall_Main

  .type  PendSV_Main, %function
  .global  PendSV_Main
PendSV_Main:
  cpsid i
  push {lr}
  blx Flash_ReturnReady
  pop  {lr}
  cpsie i 
  b PendSV_Handler
  .size  PendSV_Main, .-PendSV_Main  

  .type  SysTick_Main, %function
  .global  SysTick_Main
SysTick_Main:
  cpsid i
  push {lr}
  blx Flash_ReturnReady
  cpsie i 
  blx SysTick_Handler
  pop {lr}
  bx lr
  .size  SysTick_Main, .-SysTick_Main    
    
/******************************************************************************
*
* The minimal vector table for a Cortex M4. Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
*******************************************************************************/
  .section  .isr_vector,"a",%progbits
  .type  g_pfnVectors, %object
  .size  g_pfnVectors, .-g_pfnVectors


g_pfnVectors:
  .word  _stack_end
  .word  Reset_Handler
  .word  NMI_Handler
  .word  HardFault_Handler
  .word  MemManage_Handler
  .word  BusFault_Handler
  .word  UsageFault_Handler
  .word  0
  .word  0
  .word  0
  .word  0
  .word  SvcCall_Main
  .word  DebugMon_Handler
  .word  0
  .word  PendSV_Main
  .word  SysTick_Main

/* External Interrupts */
  .word     isrC_main     /*16:  OS timer    */
  .word     isrC_main     /*17:  Reserved    */
  .word     isrC_main     /*18:  Modem TOPSM */
  .word     isrC_main     /*19:  CM4 TOPSM   */
  .word     isrC_main     /*20:  Reserved    */
  .word     isrC_main     /*21:  Reserved    */
  .word     isrC_main     /*22:  Reserved    */
  .word     isrC_main     /*23:  Reserved    */  
  .word     isrC_main     /*24:  Accessory Detector  */
  .word     isrC_main     /*25:  RTC                 */
  .word     isrC_main     /*26:  Keypad              */
  .word     isrC_main     /*27:  General Purpose Timer */
  .word     isrC_main     /*28:  External Interrupt    */
  .word     isrC_main     /*29:  LCD           */
  .word     isrC_main     /*30:  LCD AAL       */
  .word     isrC_main     /*31:  DSI           */
  .word     isrC_main     /*32:  RESIZE        */
  .word     isrC_main     /*33:  Graphics 2D   */
  .word     isrC_main     /*34:  Camera        */
  .word     isrC_main     /*35:  ROT DMA       */
  .word     isrC_main     /*36:  SCAM          */
  .word     isrC_main     /*37:  DMA           */
  .word     isrC_main     /*38:  DMA always on */
  .word     isrC_main     /*39:  I2C dual      */
  .word     isrC_main     /*40:  I2C0          */
  .word     isrC_main     /*41:  I2C1          */
  .word     isrC_main     /*42:  I2C2          */
  .word     isrC_main     /*43:  Reserved      */  
  .word     isrC_main     /*44:  General Purpose Counter  */
  .word     isrC_main     /*45:  UART0         */
  .word     isrC_main     /*46:  UART1         */
  .word     isrC_main     /*47:  UART2         */
  .word     isrC_main     /*48:  UART3         */
  .word     isrC_main     /*49:  USB2.0        */
  .word     isrC_main     /*50:  MSDC0         */
  .word     isrC_main     /*51:  MSDC1         */
  .word     isrC_main     /*52:  Reserved      */  
  .word     isrC_main     /*53:  Reserved      */    
  .word     isrC_main     /*54:  Serial FLASH  */
  .word     isrC_main     /*55:  DSP22CPU      */
  .word     isrC_main     /*56:  Sensor DMA    */
  .word     isrC_main     /*57:  RGU           */
  .word     isrC_main     /*58:  SPI Slave     */
  .word     isrC_main     /*59:  SPI Master0   */
  .word     isrC_main     /*60:  SPI Master1   */
  .word     isrC_main     /*61:  SPI Master2   */
  .word     isrC_main     /*62:  SPI Master3   */
  .word     isrC_main     /*63:  TRNG          */
  .word     isrC_main     /*64:  Bluetooth TIMCON */
  .word     isrC_main     /*65:  Bluetooth IF    */
  .word     isrC_main     /*66:  Reserved        */  
  .word     isrC_main     /*67:  Reserved        */      
  .word     isrC_main     /*68:  LISR0             */
  .word     isrC_main     /*69:  LISR1             */
  .word     isrC_main     /*70:  LISR2             */
  .word     isrC_main     /*71:  LISR3             */
  .word     isrC_main     /*72:  PSI Rise Edge   */
  .word     isrC_main     /*73:  PSI Fall Edge    */
  .word     isrC_main     /*74:  Reserved         */  
  .word     isrC_main     /*75:  Reserved         */  
  .word     isrC_main     /*76:  PSI Fall Edge    */
  .word     isrC_main     /*77:  Reserved         */  
  .word     isrC_main     /*78:  Reserved         */ 
  .word     isrC_main     /*79:  Reserved         */ 

/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/

  .weak      NMI_Handler
  .thumb_set NMI_Handler,Default_Handler
  
  .weak      HardFault_Handler
  .thumb_set HardFault_Handler,Default_Handler
  
  .weak      MemManage_Handler
  .thumb_set MemManage_Handler,Default_Handler
  
  .weak      BusFault_Handler
  .thumb_set BusFault_Handler,Default_Handler
  
  .weak      UsageFault_Handler
  .thumb_set UsageFault_Handler,Default_Handler
  
  .weak      SVC_Handler
  .thumb_set SVC_Handler,Default_Handler
  
  .weak      DebugMon_Handler
  .thumb_set DebugMon_Handler,Default_Handler
  
  .weak      PendSV_Handler
  .thumb_set PendSV_Handler,Default_Handler
  
  .weak      SysTick_Handler
  .thumb_set SysTick_Handler,Default_Handler
  
 

/************************ (C) COPYRIGHT MEDIATEK *****END OF FILE****/

