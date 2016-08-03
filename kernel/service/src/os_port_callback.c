#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "os_port_callback.h"

#ifdef MTK_OS_CPU_UTILIZATION_ENABLE
#include "hal_gpt.h"  
#endif

#ifdef MTK_OS_CPU_UTILIZATION_ENABLE
extern uint32_t get_current_count(void );
uint32_t runtime_counter_base;

void vConfigureTimerForRunTimeStats( void )
{
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K,&runtime_counter_base);
}

uint32_t ulGetRunTimeCounterValue( void )
{
    uint32_t cur_count;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K,&cur_count);

	return (cur_count - runtime_counter_base);
}
#endif /* MTK_OS_CPU_UTILIZATION_ENABLE */

#if( configCHECK_FOR_STACK_OVERFLOW > 0 )
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    printf("stack overflow: %x %s\r\n",(unsigned int)xTask,(portCHAR *)pcTaskName);
    configASSERT(0);
}
#endif 
