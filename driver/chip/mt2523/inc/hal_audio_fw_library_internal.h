#ifndef __HAL_AUDIO_FW_LIBRARY_INTERNAL_H__
#define __HAL_AUDIO_FW_LIBRARY_INTERNAL_H__

/*******************************************************************************\
| Include Files                                                                 |
\*******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal_audio_fw_sherif.h"

/*******************************************************************************\
| Value Defines                                                                 |
\*******************************************************************************/

#define PATCH_DSP_PAGE              2
#define PATCH_DSP_ADDR              0x3C00
#define PATCH_DSP_LEN               0x400

#define DSP_D2M_IO4_BIT             (1 << 4)
#define DSP_D2M_IO5_BIT             (1 << 5)
#define DSP_D2M_IO6_BIT             (1 << 6)
#define DSP_D2M_EXCEPTION_BIT       (1 << 7)

#define DSP_RESET_BEGIN_VALUE       0x0006
#define DSP_RESET_END_VALUE         0x0007
#define DSP_SLOW_IDLE_DIVIDER_VALUE 11
#define DSP_DEEP_CLK_DIVIDER_VALUE  0xC81
#define DSP_RUNNING_VALUE           0x6666

#define IDX_MCU_HW_PATCH            1
#define IDX_DSP_HW_PATCH            2
#define PATCH_MCU_NUM               2
#define PATCH_TOTAL_NUM             24

#endif  /* __HAL_AUDIO_FW_LIBRARY_INTERNAL_H__ */
