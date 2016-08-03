/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef _IMAGE_SENSOR_NVRAM_H
#define _IMAGE_SENSOR_NVRAM_H

#include "kal_release.h"

typedef struct
{
    kal_uint32	addr;
    kal_uint32	para;
} sensor_reg_struct;

#if ( !defined(__CAMERA_NVRAM_REDUCTION__))

#define FACTORY_START_ADDR (0)
#define ENGINEER_START_ADDR (39)

typedef enum group_enum
{
    PRE_GAIN=0,
    CMMCLK_CURRENT,
    FRAME_RATE_LIMITATION,
    REGISTER_EDITOR,
    GROUP_TOTAL_NUMS
} FACTORY_GROUP_ENUM;

typedef enum register_index
{
    SENSOR_BASEGAIN=FACTORY_START_ADDR,
    PRE_GAIN_R_INDEX,
    PRE_GAIN_Gr_INDEX,
    PRE_GAIN_Gb_INDEX,
    PRE_GAIN_B_INDEX,
    FACTORY_END_ADDR
} FACTORY_REGISTER_INDEX;

typedef enum engineer_index
{
    CMMCLK_CURRENT_INDEX=ENGINEER_START_ADDR,
    ENGINEER_END_ADDR
} FACTORY_ENGINEER_INDEX;

#else

#define FACTORY_START_ADDR (0)
#define ENGINEER_START_ADDR (30)

typedef enum group_enum
{
    PRE_GAIN=0,
    CMMCLK_CURRENT,
    FRAME_RATE_LIMITATION,
    REGISTER_EDITOR,
    GROUP_TOTAL_NUMS
} FACTORY_GROUP_ENUM;

typedef enum register_index
{
    AWB_GAIN_R_INDEX=ENGINEER_START_ADDR,
    AWB_GAIN_B_INDEX,
    SENSOR_DBLC_INDEX,
    GAMMA_ENABLE_INDEX,
    CMMCLK_CURRENT_INDEX,
    ENGINEER_END_ADDR
} FACTORY_REGISTER_INDEX;

typedef enum cct_index
{
    SENSOR_BASEGAIN=FACTORY_START_ADDR,
    PRE_GAIN_R_INDEX,
    PRE_GAIN_Gr_INDEX,
    PRE_GAIN_Gb_INDEX,
    PRE_GAIN_B_INDEX,
    FACTORY_END_ADDR
} CCT_REGISTER_INDEX;

#define CAMERA_NVRAM_REDUCTION_DEFAULT_VALUE  \
{\
    /* ARRAY: SENSOR.reg[35] */\
    {\
        /* STRUCT: SENSOR.reg[0] */\
        {\
        	/* SENSOR.reg[0].addr */ 0xFFFFFFFF, /* SENSOR.reg[0].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[1] */\
        {\
        	/* SENSOR.reg[1].addr */ 0xFFFFFFFF, /* SENSOR.reg[1].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[2] */\
        {\
        	/* SENSOR.reg[2].addr */ 0xFFFFFFFF, /* SENSOR.reg[2].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[3] */\
        {\
        	/* SENSOR.reg[3].addr */ 0xFFFFFFFF, /* SENSOR.reg[3].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[4] */\
        {\
        	/* SENSOR.reg[4].addr */ 0xFFFFFFFF, /* SENSOR.reg[4].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[5] */\
        {\
        	/* SENSOR.reg[5].addr */ 0xFFFFFFFF, /* SENSOR.reg[5].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[6] */\
        {\
        	/* SENSOR.reg[6].addr */ 0xFFFFFFFF, /* SENSOR.reg[6].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[7] */\
        {\
        	/* SENSOR.reg[7].addr */ 0xFFFFFFFF, /* SENSOR.reg[7].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[8] */\
        {\
        	/* SENSOR.reg[8].addr */ 0xFFFFFFFF, /* SENSOR.reg[8].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[9] */\
        {\
        	/* SENSOR.reg[9].addr */ 0xFFFFFFFF, /* SENSOR.reg[9].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[10] */\
        {\
        	/* SENSOR.reg[10].addr */ 0xFFFFFFFF, /* SENSOR.reg[10].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[11] */\
        {\
        	/* SENSOR.reg[11].addr */ 0xFFFFFFFF, /* SENSOR.reg[11].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[12] */\
        {\
        	/* SENSOR.reg[12].addr */ 0xFFFFFFFF, /* SENSOR.reg[12].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[13] */\
        {\
        	/* SENSOR.reg[13].addr */ 0xFFFFFFFF, /* SENSOR.reg[13].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[14] */\
        {\
        	/* SENSOR.reg[14].addr */ 0xFFFFFFFF, /* SENSOR.reg[14].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[15] */\
        {\
        	/* SENSOR.reg[15].addr */ 0xFFFFFFFF, /* SENSOR.reg[15].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[16] */\
        {\
        	/* SENSOR.reg[16].addr */ 0xFFFFFFFF, /* SENSOR.reg[16].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[17] */\
        {\
        	/* SENSOR.reg[17].addr */ 0xFFFFFFFF, /* SENSOR.reg[17].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[18] */\
        {\
        	/* SENSOR.reg[18].addr */ 0xFFFFFFFF, /* SENSOR.reg[18].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[19] */\
        {\
        	/* SENSOR.reg[19].addr */ 0xFFFFFFFF, /* SENSOR.reg[19].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[20] */\
        {\
        	/* SENSOR.reg[20].addr */ 0xFFFFFFFF, /* SENSOR.reg[20].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[21] */\
        {\
        	/* SENSOR.reg[21].addr */ 0xFFFFFFFF, /* SENSOR.reg[21].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[22] */\
        {\
        	/* SENSOR.reg[22].addr */ 0xFFFFFFFF, /* SENSOR.reg[22].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[23] */\
        {\
        	/* SENSOR.reg[23].addr */ 0xFFFFFFFF, /* SENSOR.reg[23].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[24] */\
        {\
        	/* SENSOR.reg[24].addr */ 0xFFFFFFFF, /* SENSOR.reg[24].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[25] */\
        {\
        	/* SENSOR.reg[25].addr */ 0xFFFFFFFF, /* SENSOR.reg[25].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[26] */\
        {\
        	/* SENSOR.reg[26].addr */ 0xFFFFFFFF, /* SENSOR.reg[26].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[27] */\
        {\
        	/* SENSOR.reg[27].addr */ 0xFFFFFFFF, /* SENSOR.reg[27].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[28] */\
        {\
        	/* SENSOR.reg[28].addr */ 0xFFFFFFFF, /* SENSOR.reg[28].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[29] */\
        {\
        	/* SENSOR.reg[29].addr */ 0xFFFFFFFF, /* SENSOR.reg[29].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[30] */\
        {\
        	/* SENSOR.reg[30].addr */ 0xFFFFFFFF, /* SENSOR.reg[30].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[31] */\
        {\
        	/* SENSOR.reg[31].addr */ 0xFFFFFFFF, /* SENSOR.reg[31].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[32] */\
        {\
        	/* SENSOR.reg[32].addr */ 0xFFFFFFFF, /* SENSOR.reg[32].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[33] */\
        {\
        	/* SENSOR.reg[33].addr */ 0xFFFFFFFF, /* SENSOR.reg[33].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.reg[34] */\
        {\
        	/* SENSOR.reg[39].addr */ 0xFFFFFFFF, /* SENSOR.reg[39].para */ ISP_DRIVING_8MA\
        }\
    },\
    {\
        /* STRUCT: SENSOR.cct[0] */\
        {\
        	/* SENSOR.cct[0].addr */ 0xFFFFFFFF, /* SENSOR.reg[34].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.cct[1] */\
        {\
        	/* SENSOR.cct[1].addr */ 0xFFFFFFFF, /* SENSOR.reg[35].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.cct[2] */\
        {\
        	/* SENSOR.cct[2].addr */ 0xFFFFFFFF, /* SENSOR.reg[36].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.cct[3] */\
        {\
        	/* SENSOR.cct[3].addr */ 0xFFFFFFFF, /* SENSOR.reg[37].para */ 0xFFFFFFFF\
        },\
        /* STRUCT: SENSOR.cct[4] */\
        {\
        	/* SENSOR.cct[4].addr */ 0xFFFFFFFF, /* SENSOR.reg[38].para */ 0xFFFFFFFF\
        }\
    }\
}
#endif


#endif /* _IMAGE_SENSOR_NVRAM_H */
