

.file "startup_bootloader.s"
.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb



/* start address for the initialization values of the .data section.
defined in linker script */
.word  _sidata
/* start address for the .data section. defined in linker script */
.word  _sdata
/* end address for the .data section. defined in linker script */
.word  _edata
/* start address for the .bss section. defined in linker script */
.word  _sbss
/* end address for the .bss section. defined in linker script */
.word  _ebss


.globl _start
_start:
  b Reset_Handler


/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/

.section  .reset_handler
.weak  Reset_Handler
.type  Reset_Handler, %function
Reset_Handler:
  ldr  sp, =_estack    		 /* set stack pointer */
/* Copy the data segment initializers from flash to SRAM */
  mov  r1, #0
  bl  LoopCopyDataInit

/* Initial BSS section and data section(optional) */  
CopyDataInit:
  LDR  r3, =_sidata
  LDR  r3, [r3, r1]
  STR  r3, [r0, r1]
  ADDS  r1, r1, #4

LoopCopyDataInit:
  ldr  r0, =_sdata
  ldr  r3, =_edata
  adds  r2, r0, r1
  cmp  r2, r3
  bcc  CopyDataInit
  ldr  r2, =_sbss
  b  LoopFillZerobss
/* Zero fill the bss segment. */
FillZerobss:
  movs  r3, #0
  str  r3, [r2], #4

LoopFillZerobss:
  ldr  r3, = _ebss
  cmp  r2, r3
  bcc  FillZerobss
  
/* disable interrupt */
//TODO how to check interrupt disabled , check PRIMASK = 1
//TODO reference : http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/CHDBIBGJ.html
  CPSID  I

/* Call the bootloader main function.*/
  bl  bl_main

  
end_bootloader:
  //bl bootloader_error_hanbler
  bl end_bootloader

/*utility function*/
.globl JumpCmd
JumpCmd:
  ORR  r0, #0x01
  BX  r0

/* vector table */
.section  .isr_vector,"a",%progbits
.type  g_pfnVectors, %object
.size  g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
  .word  _estack
  .word  Reset_Handler
