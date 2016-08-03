#if defined(MTK_MINICLI_ENABLE)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

#include "os_cli.h"


/****************************************************************************
 *
 * Constants.
 *
 ****************************************************************************/


#define SZ_OF_TASK_LIST_BUF     (256)


/****************************************************************************
 *
 * Types.
 *
 ****************************************************************************/


/****************************************************************************
 *
 * Static variables.
 *
 ****************************************************************************/


#if defined(MTK_OS_CPU_UTILIZATION_ENABLE)
static uint32_t _cpu_meausre_time;
#endif


/****************************************************************************
 *
 * Local functions.
 *
 ****************************************************************************/


#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS == 1 ) )
static uint8_t _os_cli_show_task_info(uint8_t len, char *param[])
{
    printf("Show task info:\n");
    printf("parameter meaning:\n");
    printf("1: pcTaskName\n");
    printf("2: cStatus\n");
    printf("3: uxCurrentPriority\n");
    printf("4: usStackHighWaterMark\n");
    printf("5: xTaskNumber\n\n");

    char *task_list_buf;
    int32_t buf_size = uxTaskGetNumberOfTasks() * (configMAX_TASK_NAME_LEN + 18);

    if ((task_list_buf = pvPortMalloc(buf_size)) == NULL) {
        printf("memory malloced failed.\n");
        return 1;
    }

    vTaskList(task_list_buf);
    strlen(task_list_buf);
    printf("%s\n", task_list_buf);
    printf("buf len: %u\n", strlen(task_list_buf));
    vPortFree(task_list_buf);

    return 0;
}
#endif


#if defined(MTK_OS_CPU_UTILIZATION_ENABLE)
static void _cpu_utilization_task(void *arg)
{
    char *task_list_buf;
    int32_t buf_size = uxTaskGetNumberOfTasks() * (configMAX_TASK_NAME_LEN + 20);
    if ((task_list_buf = pvPortMalloc(buf_size)) == NULL) {
        printf("memory malloced failed.\n");
        return;
    }

    vConfigureTimerForRunTimeStats();
    vTaskClearTaskRunTimeCounter();
    vTaskDelay(_cpu_meausre_time);
    vTaskGetRunTimeStats(task_list_buf);
    printf("%s\n", task_list_buf);

    vPortFree(task_list_buf);
    vTaskDelete(xTaskGetCurrentTaskHandle());
    while (1);
}
#endif


#if defined(MTK_OS_CPU_UTILIZATION_ENABLE)
static uint8_t _os_cli_cpu_utilization(uint8_t len, char *param[])
{
    BaseType_t ret;

    if (len == 1) {
        TaskHandle_t xHandle = NULL;
        _cpu_meausre_time = atoi(param[0]);
        ret = xTaskCreate(_cpu_utilization_task, "CPU", 256, NULL, 4, &xHandle);
        if (ret != pdPASS) {
            configASSERT(0);
        }
    } else {
        printf("os 2 <duration>\n");
    }

    return 0;
}
#endif


static uint8_t _os_cli_mem(uint8_t len, char *param[])
{
    printf("heap:\n");
    printf("\ttotal: %u\n", configTOTAL_HEAP_SIZE);
    printf("\tfree:  %u\n", xPortGetFreeHeapSize());
    printf("\tlow:   %u\n", xPortGetMinimumEverFreeHeapSize());
    return 0;
}


/****************************************************************************
 *
 * API variable.
 *
 ****************************************************************************/


/****************************************************************************
 *
 * API functions.
 *
 ****************************************************************************/

cmd_t os_cli[] = {
#if defined(MTK_OS_CPU_UTILIZATION_ENABLE)
    { "cpu",   "show cpu utilization", _os_cli_cpu_utilization },
#endif
#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS == 1 ) )
    { "task",  "show FreeRtos task",   _os_cli_show_task_info  },
#endif
    { "mem",   "show heap status",     _os_cli_mem             },
    { NULL }
};

#endif /* #if defined(MTK_MINICLI_ENABLE) */

