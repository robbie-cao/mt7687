#include "FreeRTOS.h"
#include "task.h"

void vTraceTaskSwitchIn(void* current_tcb)
{
	printf("switch to %s\n", pcTaskGetTaskName(current_tcb));
}

