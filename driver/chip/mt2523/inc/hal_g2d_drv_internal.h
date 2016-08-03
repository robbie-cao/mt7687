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

#ifndef __G2D_DRV_6260_SERIES_H__
#define __G2D_DRV_6260_SERIES_H__

//#if defined(__DRV_GRAPHICS_G2D_6260_SERIES__)Larkspur

#if 1//defined(__MTK_TARGE__) || defined(G2D_FPGA) L

//#include "reg_base.h"
#define G2D_base                    (0xA0440000)
/*Dream: check with Long Lin. It's can see on \mcu\hal\system\regbase\inc\reg_base_mt6260.h*/

#define REG_G2D_START                                        *(volatile uint16_t *)(G2D_base + 0x0000)
#define REG_G2D_MODE_CTRL                                    *(volatile uint32_t *)(G2D_base + 0x0004)
#define REG_G2D_RESET                                        *(volatile uint16_t *)(G2D_base + 0x0008)
#define REG_G2D_STATUS                                       *(volatile uint16_t *)(G2D_base + 0x000C)
#define REG_G2D_IRQ                                          *(volatile uint32_t *)(G2D_base + 0x0010)
#define REG_G2D_SLOW_DOWN                                    *(volatile uint32_t *)(G2D_base + 0x0014)
//#define REG_G2D_CACHE_CON                                    *(volatile uint32_t *)(G2D_base + 0x0018)
#define REG_G2D_ROI_CON                                      *(volatile uint32_t *)(G2D_base + 0x0040)
#define REG_G2D_W2M_ADDR                                     *(volatile uint32_t *)(G2D_base + 0x0044)
#define REG_G2D_W2M_PITCH                                    *(volatile uint32_t *)(G2D_base + 0x0048)
#define REG_G2D_ROI_OFFSET                                   *(volatile uint32_t *)(G2D_base + 0x004C)
#define REG_G2D_ROI_SIZE                                     *(volatile uint32_t *)(G2D_base + 0x0050)
#define REG_G2D_ROI_BGCLR                                    *(volatile uint32_t *)(G2D_base + 0x0054)

#define REG_G2D_LT_EDGE_FILTER_USER_SPECIFIED_COLOR          *(volatile uint32_t *)(G2D_base + 0x0054)

#define REG_G2D_CLP_MIN                                      *(volatile uint32_t *)(G2D_base + 0x0058)
#define REG_G2D_CLP_MAX                                      *(volatile uint32_t *)(G2D_base + 0x005C)

#define REG_G2D_AVOIDANCE_COLOR                              *(volatile uint32_t *)(G2D_base + 0x0060)
#define REG_G2D_REPLACEMENT_COLOR                            *(volatile uint32_t *)(G2D_base + 0x0064)

#define REG_G2D_W2M_OFFSET                                   *(volatile uint32_t *)(G2D_base + 0x0068)

#define REG_G2D_MW_INIT                                      *(volatile uint32_t *)(G2D_base + 0x0070)
#define REG_G2D_MZ_INIT                                      *(volatile uint32_t *)(G2D_base + 0x0074)
#define REG_G2D_DI_CON                                       *(volatile uint32_t *)(G2D_base + 0x0078)

#define REG_G2D_L0_CON                                       *(volatile uint32_t *)(G2D_base + 0x0080)
#define REG_G2D_L0_ADDR                                      *(volatile uint32_t *)(G2D_base + 0x0084)
#define REG_G2D_L0_PITCH                                     *(volatile uint16_t *)(G2D_base + 0x0088)
#define REG_G2D_L0_OFFSET                                    *(volatile uint32_t *)(G2D_base + 0x008C)
#define REG_G2D_L0_SIZE                                      *(volatile uint32_t *)(G2D_base + 0x0090)
#define REG_G2D_L0_SRC_KEY                                   *(volatile uint32_t *)(G2D_base + 0x0094)
#define REG_G2D_L0_RECTANGLE_FILL_COLOR                      *(volatile uint32_t *)(G2D_base + 0x0094)
#define REG_G2D_L0_FONT_FOREGROUND_COLOR                     *(volatile uint32_t *)(G2D_base + 0x0094)

#define REG_G2D_L1_CON                                       *(volatile uint32_t *)(G2D_base + 0x00C0)
#define REG_G2D_L1_ADDR                                      *(volatile uint32_t *)(G2D_base + 0x00C4)
#define REG_G2D_L1_PITCH                                     *(volatile uint16_t *)(G2D_base + 0x00C8)
#define REG_G2D_L1_OFFSET                                    *(volatile uint32_t *)(G2D_base + 0x00CC)
#define REG_G2D_L1_SIZE                                      *(volatile uint32_t *)(G2D_base + 0x00D0)
#define REG_G2D_L1_SRC_KEY                                   *(volatile uint32_t *)(G2D_base + 0x00D4)
#define REG_G2D_L1_RECTANGLE_FILL_COLOR                      *(volatile uint32_t *)(G2D_base + 0x00D4)
#define REG_G2D_L1_FONT_FOREGROUND_COLOR                     *(volatile uint32_t *)(G2D_base + 0x00D4)

#define REG_G2D_L2_CON                                       *(volatile uint32_t *)(G2D_base + 0x0100)
#define REG_G2D_L2_ADDR                                      *(volatile uint32_t *)(G2D_base + 0x0104)
#define REG_G2D_L2_PITCH                                     *(volatile uint16_t *)(G2D_base + 0x0108)
#define REG_G2D_L2_OFFSET                                    *(volatile uint32_t *)(G2D_base + 0x010C)
#define REG_G2D_L2_SIZE                                      *(volatile uint32_t *)(G2D_base + 0x0110)
#define REG_G2D_L2_SRC_KEY                                   *(volatile uint32_t *)(G2D_base + 0x0114)
#define REG_G2D_L2_RECTANGLE_FILL_COLOR                      *(volatile uint32_t *)(G2D_base + 0x0114)
#define REG_G2D_L2_FONT_FOREGROUND_COLOR                     *(volatile uint32_t *)(G2D_base + 0x0114)

#define REG_G2D_L3_CON                                       *(volatile uint32_t *)(G2D_base + 0x0140)
#define REG_G2D_L3_ADDR                                      *(volatile uint32_t *)(G2D_base + 0x0144)
#define REG_G2D_L3_PITCH                                     *(volatile uint16_t *)(G2D_base + 0x0148)
#define REG_G2D_L3_OFFSET                                    *(volatile uint32_t *)(G2D_base + 0x014C)
#define REG_G2D_L3_SIZE                                      *(volatile uint32_t *)(G2D_base + 0x0150)
#define REG_G2D_L3_SRC_KEY                                   *(volatile uint32_t *)(G2D_base + 0x0154)
#define REG_G2D_L3_RECTANGLE_FILL_COLOR                      *(volatile uint32_t *)(G2D_base + 0x0154)
#define REG_G2D_L3_FONT_FOREGROUND_COLOR                     *(volatile uint32_t *)(G2D_base + 0x0154)

#define G2D_LAYER_ADDR_OFFSET                                0x40

#define REG_G2D_LAYER_CON(n)                                 *((volatile uint32_t *) (G2D_base + 0x0080 + (n) * G2D_LAYER_ADDR_OFFSET))
#define REG_G2D_LAYER_ADDR(n)                                *((volatile uint32_t *) (G2D_base + 0x0084 + (n) * G2D_LAYER_ADDR_OFFSET))
#define REG_G2D_LAYER_PITCH(n)                               *((volatile uint16_t *) (G2D_base + 0x0088 + (n) * G2D_LAYER_ADDR_OFFSET))
#define REG_G2D_LAYER_OFFSET(n)                              *((volatile uint32_t *) (G2D_base + 0x008C + (n) * G2D_LAYER_ADDR_OFFSET))
#define REG_G2D_LAYER_SIZE(n)                                *((volatile uint32_t *) (G2D_base + 0x0090 + (n) * G2D_LAYER_ADDR_OFFSET))
#define REG_G2D_LAYER_SRC_KEY(n)                             *((volatile uint32_t *) (G2D_base + 0x0094 + (n) * G2D_LAYER_ADDR_OFFSET))
#define REG_G2D_LAYER_RECTANGLE_FILL_COLOR(n)                *((volatile uint32_t *) (G2D_base + 0x0094 + (n) * G2D_LAYER_ADDR_OFFSET))
#define REG_G2D_LAYER_FONT_FOREGROUND_COLOR(n)               *((volatile uint32_t *) (G2D_base + 0x0094 + (n) * G2D_LAYER_ADDR_OFFSET))

#define REG_G2D_SZ_INIT                                      *(volatile uint32_t *)(G2D_base + 0x0094)
#define REG_G2D_SDXDX                                        *(volatile uint32_t *)(G2D_base + 0x0104)
#define REG_G2D_SDXDY                                        *(volatile uint32_t *)(G2D_base + 0x0110)
#define REG_G2D_SDYDX                                        *(volatile uint32_t *)(G2D_base + 0x010C)
#define REG_G2D_SDYDY                                        *(volatile uint32_t *)(G2D_base + 0x0114)
#define REG_G2D_SDZDX                                        *(volatile uint32_t *)(G2D_base + 0x0144)
#define REG_G2D_SDZDY                                        *(volatile uint32_t *)(G2D_base + 0x0150)
#define REG_G2D_SX_INIT                                      *(volatile uint32_t *)(G2D_base + 0x014C)
#define REG_G2D_SY_INIT                                      *(volatile uint32_t *)(G2D_base + 0x0154)
#define REG_G2D_START_DST_X                                  *(volatile uint32_t *)(G2D_base + 0x0108)
#define REG_G2D_START_DST_Y                                  *(volatile uint32_t *)(G2D_base + 0x0148)


#else

#define REG_G2D_START                                        g2d_hw_registers[0]
#define REG_G2D_MODE_CTRL                                    g2d_hw_registers[1]
#define REG_G2D_RESET                                        g2d_hw_registers[2]
#define REG_G2D_STATUS                                       g2d_hw_registers[3]
#define REG_G2D_IRQ                                          g2d_hw_registers[4]
#define REG_G2D_SLOW_DOWN                                    g2d_hw_registers[5]
//#define REG_G2D_CACHE_CON                                    g2d_hw_registers[6]
#define REG_G2D_ROI_CON                                      g2d_hw_registers[7]
#define REG_G2D_W2M_ADDR                                     g2d_hw_registers[8]
#define REG_G2D_W2M_PITCH                                    g2d_hw_registers[9]
#define REG_G2D_ROI_OFFSET                                   g2d_hw_registers[10]
#define REG_G2D_ROI_SIZE                                     g2d_hw_registers[11]
#define REG_G2D_ROI_BGCLR                                    g2d_hw_registers[12]

#define REG_G2D_LT_EDGE_FILTER_USER_SPECIFIED_COLOR          g2d_hw_registers[12]

#define REG_G2D_CLP_MIN                                      g2d_hw_registers[13]
#define REG_G2D_CLP_MAX                                      g2d_hw_registers[14]

#define REG_G2D_AVOIDANCE_COLOR                              g2d_hw_registers[15]
#define REG_G2D_REPLACEMENT_COLOR                            g2d_hw_registers[16]

#define REG_G2D_W2M_OFFSET                                   g2d_hw_registers[17]
#define REG_G2D_MW_INIT                                      g2d_hw_registers[18]
#define REG_G2D_MZ_INIT                                      g2d_hw_registers[19]
#define REG_G2D_DI_CON                                       g2d_hw_registers[20]

#define REG_G2D_L0_CON                                       g2d_hw_registers[21]
#define REG_G2D_L0_ADDR                                      g2d_hw_registers[22]
#define REG_G2D_L0_PITCH                                     g2d_hw_registers[23]
#define REG_G2D_L0_OFFSET                                    g2d_hw_registers[24]
#define REG_G2D_L0_SIZE                                      g2d_hw_registers[25]
#define REG_G2D_L0_SRC_KEY                                   g2d_hw_registers[26]
#define REG_G2D_L0_RECTANGLE_FILL_COLOR                      g2d_hw_registers[26]
#define REG_G2D_L0_FONT_FOREGROUND_COLOR                     g2d_hw_registers[26]

#define REG_G2D_L1_CON                                       g2d_hw_registers[27]
#define REG_G2D_L1_ADDR                                      g2d_hw_registers[28]
#define REG_G2D_L1_PITCH                                     g2d_hw_registers[29]
#define REG_G2D_L1_OFFSET                                    g2d_hw_registers[30]
#define REG_G2D_L1_SIZE                                      g2d_hw_registers[31]
#define REG_G2D_L1_SRC_KEY                                   g2d_hw_registers[32]
#define REG_G2D_L1_RECTANGLE_FILL_COLOR                      g2d_hw_registers[32]
#define REG_G2D_L1_FONT_FOREGROUND_COLOR                     g2d_hw_registers[32]

#define REG_G2D_L2_CON                                       g2d_hw_registers[33]
#define REG_G2D_L2_ADDR                                      g2d_hw_registers[34]
#define REG_G2D_L2_PITCH                                     g2d_hw_registers[35]
#define REG_G2D_L2_OFFSET                                    g2d_hw_registers[36]
#define REG_G2D_L2_SIZE                                      g2d_hw_registers[37]
#define REG_G2D_L2_SRC_KEY                                   g2d_hw_registers[38]
#define REG_G2D_L2_RECTANGLE_FILL_COLOR                      g2d_hw_registers[38]
#define REG_G2D_L2_FONT_FOREGROUND_COLOR                     g2d_hw_registers[38]

#define REG_G2D_L3_CON                                       g2d_hw_registers[39]
#define REG_G2D_L3_ADDR                                      g2d_hw_registers[40]
#define REG_G2D_L3_PITCH                                     g2d_hw_registers[41]
#define REG_G2D_L3_OFFSET                                    g2d_hw_registers[42]
#define REG_G2D_L3_SIZE                                      g2d_hw_registers[43]
#define REG_G2D_L3_SRC_KEY                                   g2d_hw_registers[44]
#define REG_G2D_L3_RECTANGLE_FILL_COLOR                      g2d_hw_registers[44]
#define REG_G2D_L3_FONT_FOREGROUND_COLOR                     g2d_hw_registers[44]

#define G2D_LAYER_ADDR_OFFSET                                0x6

#define REG_G2D_LAYER_CON(n)                                 g2d_hw_registers[21  + (n) * G2D_LAYER_ADDR_OFFSET]
#define REG_G2D_LAYER_ADDR(n)                                g2d_hw_registers[22  + (n) * G2D_LAYER_ADDR_OFFSET]
#define REG_G2D_LAYER_PITCH(n)                               g2d_hw_registers[23  + (n) * G2D_LAYER_ADDR_OFFSET]
#define REG_G2D_LAYER_OFFSET(n)                              g2d_hw_registers[24  + (n) * G2D_LAYER_ADDR_OFFSET]
#define REG_G2D_LAYER_SIZE(n)                                g2d_hw_registers[25  + (n) * G2D_LAYER_ADDR_OFFSET]
#define REG_G2D_LAYER_SRC_KEY(n)                             g2d_hw_registers[26  + (n) * G2D_LAYER_ADDR_OFFSET]
#define REG_G2D_LAYER_RECTANGLE_FILL_COLOR(n)                g2d_hw_registers[26  + (n) * G2D_LAYER_ADDR_OFFSET]
#define REG_G2D_LAYER_FONT_FOREGROUND_COLOR(n)               g2d_hw_registers[26  + (n) * G2D_LAYER_ADDR_OFFSET]

#define REG_G2D_SZ_INIT                                      g2d_hw_registers[26]
#define REG_G2D_SDXDX                                        g2d_hw_registers[34]
#define REG_G2D_SDXDY                                        g2d_hw_registers[37]
#define REG_G2D_SDYDX                                        g2d_hw_registers[36]
#define REG_G2D_SDYDY                                        g2d_hw_registers[38]
#define REG_G2D_SDZDX                                        g2d_hw_registers[40]
#define REG_G2D_SDZDY                                        g2d_hw_registers[43]
#define REG_G2D_SX_INIT                                      g2d_hw_registers[42]
#define REG_G2D_SY_INIT                                      g2d_hw_registers[44]
#define REG_G2D_START_DST_X                                  g2d_hw_registers[35]
#define REG_G2D_START_DST_Y                                  g2d_hw_registers[41]

#endif

/// bit mapping of graphic 2D engine IRQ register
#define G2D_START_BIT                                           0x00000001

/// G2D_ENG_MODE,2D engine function mode
#define G2D_MODE_CON_ENG_MODE_G2D_LT_BIT                        0x0002
#define G2D_MODE_CON_ENG_MODE_G2D_BITBLT_BIT                    0x0001
#define G2D_MODE_CON_ENABLE_SAD_BIT                             0x0100

/// bit mapping of graphic 2D engine IRQ register
#define G2D_IRQ_ENABLE_BIT                                      0x00000001
#define G2D_IRQ_STATUS_BIT                                      0x00010000

/// bit mapping of 2D engine common control register
#define G2D_RESET_WARM_RESET_BIT                                0x0001
#define G2D_RESET_HARD_RESET_BIT                                0x0002

/// bit mapping of graphic 2D engine status register
#define HAL_G2D_STATUS_BUSY_BIT                                     0x00000001


#define G2D_READ_BURST_TYPE_MASK                                0x03000000
#define G2D_WRITE_BURST_TYPE_MASK                               0x00700000


#define G2D_SLOW_DOWN_COUNT_MASK                                0x0000FFFF
#define G2D_SLOW_DOWN_ENABLE_BIT                                0x80000000

#define G2D_CACHE_CON_MAX_OUTSTANDING_MASK                      0x00F0
#define G2D_CACHE_CON_LT_CACHE_SIZE_MASK                        0x1


#define G2D_ROI_CON_REPLACEMENT_ENABLE_BIT                      0x00200000       /// CLR_REP_EN
#define G2D_ROI_CON_CLIP_ENABLE_BIT                             0x10000
#define G2D_ROI_CON_DISABLE_BG_BIT                              0x80000
#define G2D_ROI_CON_ENABLE_LAYER0_BIT                           0x80000000
#define G2D_ROI_CON_ENABLE_LAYER1_BIT                           0x40000000
#define G2D_ROI_CON_ENABLE_LAYER2_BIT                           0x20000000
#define G2D_ROI_CON_ENABLE_LAYER3_BIT                           0x10000000
#define G2D_ROI_CON_ENABLE_LAYER_MASK                           0xF0000000
#define G2D_ROI_CON_CONSTANT_ALPHA_MASK                         0xFF00
#define G2D_ROI_CON_ALPHA_ENABLE_BIT                            0x80
#define G2D_ROI_CON_ENABLE_FORCE_TS_BIT                         0x20000
#define G2D_ROI_CON_TILE_SIZE_8x8_BIT                           0x00000
#define G2D_ROI_CON_TILE_SIZE_16x8_BIT                          0x40000



/// DST_CLR_MODE,destination color mode
#define G2D_ROI_CON_W2M_COLOR_GRAY                              0x00
#define G2D_ROI_CON_W2M_COLOR_RGB565                            0x01
#define G2D_ROI_CON_W2M_COLOR_ARGB8888                          0x08 //0x04
#define G2D_ROI_CON_W2M_COLOR_ARGB8565                          0x09
#define G2D_ROI_CON_W2M_COLOR_ARGB6666                          0x0A
#define G2D_ROI_CON_W2M_COLOR_RGB888                            0x03
#define G2D_ROI_CON_W2M_COLOR_BGR888                            0x13 //0x0B
#define G2D_ROI_CON_W2M_COLOR_PARGB8888                         0x0C //0x05
#define G2D_ROI_CON_W2M_COLOR_PARGB8565                         0x0D
#define G2D_ROI_CON_W2M_COLOR_PARGB6666                         0x0E
#define G2D_ROI_CON_W2M_COLOR_MASK                              0x1F //0x0F

#define G2D_LX_CON_ENABLE_SRC_KEY_BIT                           0x00800000
#define G2D_LX_CON_ENABLE_RECT_BIT                              0x00400000
#define G2D_LX_CON_ENABLE_FONT_BIT                              0x40000000
#define G2D_LX_CON_AA_FONT_BIT_MASK                             0x30000000

#define G2D_LX_CON_COLOR_GRAY                                   0x00
#define G2D_LX_CON_COLOR_RGB565                                 0x01
#define G2D_LX_CON_COLOR_UYVY                                   0x02
#define G2D_LX_CON_COLOR_RGB888                                 0x03
#define G2D_LX_CON_COLOR_ARGB8888                               0x08 //0x04
#define G2D_LX_CON_COLOR_ARGB8565                               0x09
#define G2D_LX_CON_COLOR_ARGB6666                               0x0A
#define G2D_LX_CON_COLOR_PARGB8888                              0x0C //0x05
#define G2D_LX_CON_COLOR_PARGB8565                              0x0D
#define G2D_LX_CON_COLOR_PARGB6666                              0x0E
#define G2D_LX_CON_COLOR_BGR888                                 0x13 //0x0B
#define G2D_LX_CON_COLOR_MASK                                   0x1F //0x0F

#define G2D_LX_CON_CONSTANT_ALPHA_MASK                          0xFF00
#define G2D_LX_CON_ALPHA_ENABLE_BIT                             0x80
#define G2D_LX_CON_CCW_ROTATE_MASK                              0x70000
#define G2D_LX_CON_CCW_ROTATE_000                               0x00000
#define G2D_LX_CON_CCW_ROTATE_MIRROR_090                        0x10000
#define G2D_LX_CON_CCW_ROTATE_MIRROR_000                        0x20000
#define G2D_LX_CON_CCW_ROTATE_090                               0x30000
#define G2D_LX_CON_CCW_ROTATE_MIRROR_180                        0x40000
#define G2D_LX_CON_CCW_ROTATE_270                               0x50000
#define G2D_LX_CON_CCW_ROTATE_180                               0x60000
#define G2D_LX_CON_CCW_ROTATE_MIRROR_270                        0x70000

#define CLEAR_G2D_INT_STATUS                                                                                    \
  do {                                                                                                          \
    REG_G2D_IRQ &= ~G2D_IRQ_STATUS_BIT;                                                                         \
  } while(0)


#define WARM_RESET_G2D_ENGINE                                                                                   \
  do {                                                                                                          \
    REG_G2D_RESET = G2D_RESET_WARM_RESET_BIT ;                                                                  \
    while(REG_G2D_STATUS & HAL_G2D_STATUS_BUSY_BIT){}                                                               \
    REG_G2D_RESET = G2D_RESET_HARD_RESET_BIT ;                                                                  \
    REG_G2D_RESET = 0;                                                                                          \
  } while(0)


#define HARD_RESET_G2D_ENGINE                                                                                   \
  do {                                                                                                          \
    REG_G2D_RESET = 0;                                                                                          \
    REG_G2D_RESET = G2D_RESET_HARD_RESET_BIT ;                                                                  \
    REG_G2D_RESET = 0;                                                                                          \
  } while(0)

/// W2M
#define SET_G2D_W2M_ADDR(addr)                                                    \
  do {                                                                            \
    REG_G2D_W2M_ADDR = addr;                                                      \
  } while(0)

#define SET_G2D_W2M_PITCH(pitch)                                                  \
  do {                                                                            \
    REG_G2D_W2M_PITCH = pitch;                                                    \
  } while(0)

#define SET_G2D_W2M_OFFSET(x, y)                                                          \
  do {                                                                                    \
    REG_G2D_W2M_OFFSET = (((int16_t)(x) & 0xFFFF)<< 16) | ((int16_t)(y) & 0xFFFF);    \
  } while(0)

#define SET_G2D_W2M_COLOR_FORMAT(format)                                          \
  do {                                                                            \
    REG_G2D_ROI_CON &= ~G2D_ROI_CON_W2M_COLOR_MASK;                               \
    REG_G2D_ROI_CON |= (format & G2D_ROI_CON_W2M_COLOR_MASK);                     \
  } while(0)

/// MODE_CON
#define ENABLE_G2D_SAD                                                            \
  do {                                                                            \
    REG_G2D_MODE_CTRL |= (1 << (31 - layer));                                     \
  } while(0)

#define SET_G2D_MODE_CON_SAD_DOFS(n)                                              \
  do {                                                                            \
    REG_G2D_MODE_CTRL |= (n & 0x3) << 14;                                         \
  } while(0)

#define SET_G2D_MODE_CON_SAD_SOFS(n)                                              \
  do {                                                                            \
    REG_G2D_MODE_CTRL |= (n & 0x3) << 12;                                         \
  } while(0)

#define SET_G2D_MODE_CON_SAD_MOD(n)                                               \
  do {                                                                            \
    REG_G2D_MODE_CTRL |= (n & 0x3) << 9;                                          \
  } while(0)

/// ROI
#define DISABLE_G2D_ROI_ALL_LAYER                                                 \
  do {                                                                            \
    REG_G2D_ROI_CON &= ~G2D_ROI_CON_ENABLE_LAYER_MASK;                            \
  } while(0)


#define ENABLE_G2D_ROI_LAYER(layer)                                               \
  do {                                                                            \
    REG_G2D_ROI_CON |= (1 << (31 - layer));                                       \
  } while(0)

#define DISABLE_G2D_ROI_CON_BG                                                    \
  do {                                                                            \
    REG_G2D_ROI_CON |= G2D_ROI_CON_DISABLE_BG_BIT;                                \
  } while(0)

#define SET_G2D_ROI_CON_BG_COLOR(color)                                           \
  do {                                                                            \
    REG_G2D_ROI_BGCLR = color;                                                    \
  } while(0)

#define ENABLE_G2D_ROI_CON_ALPHA                                                  \
  do {                                                                            \
    REG_G2D_ROI_CON |= G2D_ROI_CON_ALPHA_ENABLE_BIT;                              \
  } while(0)

#define SET_G2D_ROI_CON_ALPHA(alpha)                                              \
  do {                                                                            \
    REG_G2D_ROI_CON &= ~G2D_ROI_CON_CONSTANT_ALPHA_MASK;                          \
    REG_G2D_ROI_CON |= ((alpha << 8) & G2D_ROI_CON_CONSTANT_ALPHA_MASK);          \
  } while(0)


#define SET_G2D_ROI_OFFSET(x, y)                                                  \
  do {                                                                            \
    REG_G2D_ROI_OFFSET = (((x) << 16) | (0xFFFF & (y)));                          \
  } while(0)

#define SET_G2D_ROI_SIZE(w, h)                                                    \
  do {                                                                            \
    REG_G2D_ROI_SIZE = (((w) << 16) | (h));                                       \
  } while(0)



#define SET_G2D_MW_INIT(mw)                                                       \
  do {                                                                            \
    REG_G2D_MW_INIT = mw;                                                         \
  } while(0)



#define SET_G2D_MZ_INIT(mz)                                                       \
  do {                                                                            \
    REG_G2D_MZ_INIT = mz;                                                         \
  } while(0)



#define SET_G2D_DI_CON(r, g, b, mode)                                             \
  do {                                                                            \
    REG_G2D_DI_CON = (((r) << 12) | ((g) << 8) | ((b) << 4) | mode);              \
  } while(0)

/// CACHE
#define SET_REG_G2D_CACHE_CON_LT_CACHE_SIZE(n)                                    \
  do {                                                                            \
    REG_G2D_CACHE_CON &= ~G2D_CACHE_CON_LT_CACHE_SIZE_MASK;                       \
    REG_G2D_CACHE_CON |= n;                                                       \
  } while(0)

#define SET_REG_G2D_CACHE_CON_MAX_OUTSTANDING_NUM(n)                              \
  do {                                                                            \
    REG_G2D_CACHE_CON &= ~G2D_CACHE_CON_MAX_OUTSTANDING_MASK;                     \
    REG_G2D_CACHE_CON |= (((n) << 4) & G2D_CACHE_CON_MAX_OUTSTANDING_MASK);       \
  } while(0)


/// Layer

#define CLR_G2D_LAYER_CON(n)                                                      \
  do {                                                                            \
    REG_G2D_LAYER_CON(n) = 0;                                                     \
  } while(0)

#define SET_G2D_LAYER_CON_COLOR_FORMAT(n, format)                                 \
  do {                                                                            \
    REG_G2D_LAYER_CON(n) &= ~G2D_LX_CON_COLOR_MASK;                               \
    REG_G2D_LAYER_CON(n) |= (format & G2D_LX_CON_COLOR_MASK);                     \
  } while(0)

#define SET_G2D_LAYER_CON_ALPHA(n, alpha)                                         \
  do {                                                                            \
    REG_G2D_LAYER_CON(n) &= ~G2D_LX_CON_CONSTANT_ALPHA_MASK;                      \
    REG_G2D_LAYER_CON(n) |= ((alpha << 8) & G2D_LX_CON_CONSTANT_ALPHA_MASK);      \
  } while(0)

#define ENABLE_G2D_LAYER_CON_ALPHA(n)                                             \
  do {                                                                            \
    REG_G2D_LAYER_CON(n) |= G2D_LX_CON_ALPHA_ENABLE_BIT;                          \
  } while(0)

#define DISABLE_G2D_LAYER_CON_ALPHA(n)                                            \
  do {                                                                            \
    REG_G2D_LAYER_CON(n) &= ~G2D_LX_CON_ALPHA_ENABLE_BIT;                         \
  } while(0)

#define ENABLE_G2D_LAYER_CON_RECT_FILL(n)                                         \
  do {                                                                            \
    REG_G2D_LAYER_CON(n) |= G2D_LX_CON_ENABLE_RECT_BIT;                           \
  } while(0)

#define DISABLE_G2D_LAYER_CON_RECT_FILL(n)                                        \
  do {                                                                            \
    REG_G2D_LAYER_CON(n) &= ~G2D_LX_CON_ENABLE_RECT_BIT;                          \
  } while(0)

#define ENABLE_G2D_LAYER_CON_FONT(n)                                              \
  do {                                                                            \
    REG_G2D_LAYER_CON(n) |= G2D_LX_CON_ENABLE_FONT_BIT;                           \
  } while(0)

#define SET_G2D_LAYER_CON_AA_FONT_BIT(n, bit)                                     \
  do {                                                                            \
    REG_G2D_LAYER_CON(n) &= ~G2D_LX_CON_AA_FONT_BIT_MASK;                         \
    REG_G2D_LAYER_CON(n) |= (bit);                                                \
  } while(0)

#define ENABLE_G2D_LAYER_CON_SRC_KEY(n)                                           \
  do {                                                                            \
    REG_G2D_LAYER_CON(n) |= G2D_LX_CON_ENABLE_SRC_KEY_BIT;                        \
  } while(0)

#define DISABLE_G2D_LAYER_CON_SRC_KEY(n)                                          \
  do {                                                                            \
    REG_G2D_LAYER_CON(n) &= ~G2D_LX_CON_ENABLE_SRC_KEY_BIT;                       \
  } while(0)

#define SET_G2D_LAYER_CON_ROTATE(n, rot)                                          \
  do {                                                                            \
    REG_G2D_LAYER_CON(n) &= ~G2D_LX_CON_CCW_ROTATE_MASK;                          \
    REG_G2D_LAYER_CON(n) |= (rot);                                                \
  } while(0)

#define SET_G2D_LAYER_ADDR(n, addr)                                               \
  do {                                                                            \
    REG_G2D_LAYER_ADDR(n) = addr;                                                 \
  } while(0)

#define SET_G2D_LAYER_PITCH(n, pitch)                                             \
  do {                                                                            \
    REG_G2D_LAYER_PITCH(n) = pitch;                                               \
  } while(0)

#define SET_G2D_LAYER_OFFSET(n, x, y)                                             \
  do {                                                                            \
    REG_G2D_LAYER_OFFSET(n) = (((x) << 16) | (0xFFFF & (y)));                     \
  } while(0)

#define SET_G2D_LAYER_SIZE(n, w, h)                                               \
  do {                                                                            \
    REG_G2D_LAYER_SIZE(n) = (((w) << 16) | (h));                                  \
  } while(0)

#define SET_G2D_LAYER_SRC_KEY(n, color)                                           \
  do {                                                                            \
    REG_G2D_LAYER_SRC_KEY(n) = color;                                             \
  } while(0)

#define SET_G2D_LAYER_RECTANGLE_FILL_COLOR(n, color)                              \
  do {                                                                            \
    REG_G2D_LAYER_RECTANGLE_FILL_COLOR(n) = color;                                \
  } while(0)

#define SET_G2D_LAYER_FONT_FOREGROUND_COLOR(n, color)                             \
  do {                                                                            \
    REG_G2D_LAYER_FONT_FOREGROUND_COLOR(n) = color;                               \
  } while(0)



//#endif

#endif


