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

#ifndef _CPLL_H_
#define _CPLL_H_

//#include "drv_features.h"
//#include "reg_base.h"

#define MIXED_base                  (0xA0170000) /*Analog Chip Interface Controller (PLL, CLKSQ, FH, CLKSW and SIMLS) */
#define PLL_base                    MIXED_base   /*Analog Chip Interface Controller (PLL, CLKSQ, FH, CLKSW and SIMLS) */
#define MIXED_SD_base                  (0xA0170000) /*Analog Chip Interface Controller (PLL, CLKSQ, FH, CLKSW and SIMLS) */
#define PLL_SD_base                    MIXED_SD_base   /*Analog Chip Interface Controller (PLL, CLKSQ, FH, CLKSW and SIMLS) */

#if(defined(DRV_ISP_MT6238_HW_SUPPORT))

#define CPLL_BASE      	PLL_base
                                          //0x83000000

#define REG_CPLL_CTRL_REGISTER1				*((volatile unsigned int *) (CPLL_BASE + 0x0008))
#define REG_CPLL_CTRL_REGISTER2 				*((volatile unsigned int *) (CPLL_BASE + 0x000C))

#define REG_CPLL_DIGDIV_MASK							0x0000F000
#define REG_CPLL_RST_BIT								0x00000800
#define REG_CPLL_DIVCTRL_MASK							0x000007E0
#define REG_CPLL_CALI_MASK								0x0000001F

#define REG_CPLL_PDN_BIT								0x00000001

#define SET_CPLL_DIGITAL_DIVIDER(n)				REG_CPLL_CTRL_REGISTER1 &= ~REG_CPLL_DIGDIV_MASK; \
															REG_CPLL_CTRL_REGISTER1 |= ((n)<<12);

#define SET_CPLL_RST				                            REG_CPLL_CTRL_REGISTER1 |= REG_CPLL_RST_BIT;
#define CLEAR_CPLL_RST				                            REG_CPLL_CTRL_REGISTER1 &= ~REG_CPLL_RST_BIT;

#define SET_CPLL_DIVCTRL(n)							  REG_CPLL_CTRL_REGISTER1 &= ~REG_CPLL_DIVCTRL_MASK; \
															REG_CPLL_CTRL_REGISTER1 |= ((n)<<5);
								
#define SET_CPLL_CALI(n)							     REG_CPLL_CTRL_REGISTER1 &= ~REG_CPLL_CALI_MASK; \
															REG_CPLL_CTRL_REGISTER1 |= (n);
															
#define SET_CPLL_PWR_ON				                    REG_CPLL_CTRL_REGISTER2 &= ~REG_CPLL_PDN_BIT;

#define SET_CPLL_PWR_DOWN			                            REG_CPLL_CTRL_REGISTER2 |= REG_CPLL_PDN_BIT;															


#elif(defined(DRV_ISP_MT6268_HW_SUPPORT))

#define CPLL_BASE      	MIXED_base
                                          //0x840C0000

#define REG_CPLL_CTRL_REGISTER1				*((volatile unsigned int *) (CPLL_BASE + 0x0038))
#define REG_CPLL_CTRL_REGISTER2 				*((volatile unsigned int *) (CPLL_BASE + 0x003C))
#define REG_CPLL_CTRL_REGISTER3 				*((volatile unsigned int *) (CPLL_BASE + 0x0048))

/*cpll register1*/
#define REG_CPLL_RST_BIT								0x00000800
#define REG_CPLL_FBDIV_MASK							0x000007E0
#define REG_CPLL_CM_MASK								0x0000001F

/*cpll register2*/
#define REG_CPLL_PDN_BIT								0x00000001

/*cpll register3*/
#define REG_CPLL_DIGDIV_MASK							0x0000000F



#define SET_CPLL_RST				                            REG_CPLL_CTRL_REGISTER1 |= REG_CPLL_RST_BIT;
#define CLEAR_CPLL_RST				                            REG_CPLL_CTRL_REGISTER1 &= ~REG_CPLL_RST_BIT;

#define SET_CPLL_DIVCTRL(n)							  REG_CPLL_CTRL_REGISTER1 &= ~REG_CPLL_FBDIV_MASK; \
															REG_CPLL_CTRL_REGISTER1 |= ((n)<<5);
								
#define SET_CPLL_CALI(n)							     REG_CPLL_CTRL_REGISTER1 &= ~REG_CPLL_CM_MASK; \
															REG_CPLL_CTRL_REGISTER1 |= (n);
															
#define SET_CPLL_PWR_ON				                    REG_CPLL_CTRL_REGISTER2 |= REG_CPLL_PDN_BIT;

#define SET_CPLL_PWR_DOWN			          			REG_CPLL_CTRL_REGISTER2 &= ~REG_CPLL_PDN_BIT;

#define SET_CPLL_DIGITAL_DIVIDER(n)				REG_CPLL_CTRL_REGISTER3 &= ~REG_CPLL_DIGDIV_MASK; \
															REG_CPLL_CTRL_REGISTER3 |= (n);


#elif(defined(DRV_ISP_MT6236_HW_SUPPORT))

#define CPLL_BASE      	PLL_SD_base
                                          //0x801A0000

#define REG_CPLL_CTRL_REGISTER1				*((volatile unsigned int *) (CPLL_BASE + 0x0600))
#define REG_CPLL_CTRL_REGISTER2 				*((volatile unsigned int *) (CPLL_BASE + 0x0604))
#define REG_CPLL_CTRL_REGISTER3 				*((volatile unsigned int *) (CPLL_BASE + 0x0608))

/*cpll register1*/
#define REG_CPLL_CM_MASK								0x000000F0

/*cpll register2*/
#define REG_CPLL_RST_BIT								0x00000001
#define REG_CPLL_PDN_BIT								0x00000002

/*cpll register3*/
#define REG_CPLL_DIGDIV_MASK							0x0000F000
#define REG_CPLL_FBDIV_MASK							0x0000003F

#define SET_CPLL_CALI(n)							     REG_CPLL_CTRL_REGISTER1 &= ~REG_CPLL_CM_MASK; \
															REG_CPLL_CTRL_REGISTER1 |= ((n&0xF) <<4);

#define SET_CPLL_RST				                            REG_CPLL_CTRL_REGISTER2 |= REG_CPLL_RST_BIT;
#define CLEAR_CPLL_RST				                            REG_CPLL_CTRL_REGISTER2 &= ~REG_CPLL_RST_BIT;
#define SET_CPLL_PWR_ON				                    REG_CPLL_CTRL_REGISTER2 |= REG_CPLL_PDN_BIT;
#define SET_CPLL_PWR_DOWN			          			REG_CPLL_CTRL_REGISTER2 &= ~REG_CPLL_PDN_BIT;


#define SET_CPLL_DIVCTRL(n)							  REG_CPLL_CTRL_REGISTER3 &= ~REG_CPLL_FBDIV_MASK; \
															REG_CPLL_CTRL_REGISTER3 |= (n);
																
#define SET_CPLL_DIGITAL_DIVIDER(n)				REG_CPLL_CTRL_REGISTER3 &= ~REG_CPLL_DIGDIV_MASK; \
															REG_CPLL_CTRL_REGISTER3 |= ((n&0xF)<<12);

#elif(defined(DRV_ISP_MT6276_HW_SUPPORT))

#if defined(MT6276)
#define CPLL_BASE      	MIX_ABB_base
                                          //0x61140000
#define REG_PLL_CTRL_REGISTER4 	*((volatile unsigned int *) (CPLL_BASE + 0x0110))
#define REG_CPLL_CON0 				*((volatile unsigned int *) (CPLL_BASE + 0x0200))
/*cpll register1*/
#define REG_FROM_CPLL_208M_MASK		0x00000001 //from cam pll output 208MHz clock
#define REG_CPLL_EN_MASK				0x00000001
#define REG_CPLL_LCON_MASK				0x0000000c
#define REG_CPLL_FX_CTRL_MASK			0x00000070
#define REG_CPLL_FSEL_MASK				0x00003F00


#define SET_CPLL_208M				REG_CPLL_CON0 &= ~REG_CPLL_FSEL_MASK; \
										REG_CPLL_CON0 |= (0x3A00);
#define SET_CPLL_156M				REG_CPLL_CON0 &= ~REG_CPLL_FSEL_MASK; \
										REG_CPLL_CON0 |= (0x2A00);
#define SET_CPLL_124_8M				REG_CPLL_CON0 &= ~REG_CPLL_FSEL_MASK; \
										REG_CPLL_CON0 |= (0x1100);
#define SET_CPLL_104M				REG_CPLL_CON0 &= ~REG_CPLL_FSEL_MASK; \
										REG_CPLL_CON0 |= (0x1A00);												
#define SET_CPLL_PWR_ON				REG_PLL_CTRL_REGISTER4 |= REG_FROM_CPLL_208M_MASK; \
										REG_CPLL_CON0 |= REG_CPLL_EN_MASK; \
										REG_CPLL_CON0 |= (0x4D);
#define SET_CPLL_PWR_DOWN			REG_PLL_CTRL_REGISTER4 &= ~REG_FROM_CPLL_208M_MASK; \
										REG_CPLL_CON0 &= ~REG_CPLL_EN_MASK; 

#define SET_CPLL_CLK(n)				REG_CPLL_CON0 &= ~REG_CPLL_FSEL_MASK; \
										REG_CPLL_CON0 |= (n<<8);
#define SET_CPLL_RST					//not use in MT6276 
#define CLEAR_CPLL_RST					//not use in MT6276 	   
#define SET_CPLL_DIVCTRL(n)				//not use in MT6276 		
#define SET_CPLL_DIGITAL_DIVIDER(n)	//not use in MT6276 	

#elif defined(MT6256)||defined(MT6255)               //MT6256/55 use UPLL
#define UPLL_BASE                  0x801A0000
#define REG_UPLL_CTRL_REGISTER3    *((volatile unsigned int *) (UPLL_BASE + 0x020C))
#define MM_CLK_GATING_CLR_REGISTER *((volatile unsigned int *) (MMCONFG_base + 0x008))
#define MM_CLK_GATING_SET_REGISTER *((volatile unsigned int *) (MMCONFG_base + 0x008))
#define MM_CAM_CLK_GATING_MASK     0x00000020
#define REG_UPLL_RST_MASK          0x00000001
#define REG_UPLL_EN_MASK				   0x00000002
#define REG_UPLL_FSEL_MASK			   0x000000c0
/*
  UPLL_CON3 bit 7:6    0x801A020c
  0: 96, 1: 124, 2: 156, 3: 208
*/
#define UPLL_96M_CLK           0
#define UPLL_124M_CLK           1
#define UPLL_156M_CLK           2
#define UPLL_208M_CLK           3

#define SET_CAM_GATING          MM_CLK_GATING_SET_REGISTER |= MM_CAM_CLK_GATING_MASK;
#define CLR_CAM_GATING          MM_CLK_GATING_CLR_REGISTER |= MM_CAM_CLK_GATING_MASK;

#define SET_CPLL_208M				REG_UPLL_CTRL_REGISTER3 &= ~REG_UPLL_FSEL_MASK; \
                                				REG_UPLL_CTRL_REGISTER3 |= UPLL_208M_CLK<<6;
#define SET_CPLL_156M				REG_UPLL_CTRL_REGISTER3 &= ~REG_UPLL_FSEL_MASK; \
                                				REG_UPLL_CTRL_REGISTER3 |= UPLL_156M_CLK<<6;
#define SET_CPLL_124_8M				REG_UPLL_CTRL_REGISTER3 &= ~REG_UPLL_FSEL_MASK; \
									REG_UPLL_CTRL_REGISTER3 |= UPLL_124M_CLK<<6;
#define SET_CPLL_96M				REG_UPLL_CTRL_REGISTER3 &= ~REG_UPLL_FSEL_MASK; \
									REG_UPLL_CTRL_REGISTER3 |= UPLL_96M_CLK<<6;
										
#define SET_CPLL_PWR_ON         REG_UPLL_CTRL_REGISTER3 |= REG_UPLL_EN_MASK; \
                                CLR_CAM_GATING;
#define SET_CPLL_PWR_DOWN       SET_CAM_GATING;                   //Don't Diable UPLL which also used by other module(TDMA etc.)

#define SET_CPLL_RST					  REG_UPLL_CTRL_REGISTER3 |= REG_UPLL_RST_MASK;
#define CLEAR_CPLL_RST					REG_UPLL_CTRL_REGISTER3 &= ~REG_UPLL_RST_MASK;
#define SET_CPLL_DIVCTRL(n)				//not use in MT6276 		
#define SET_CPLL_DIGITAL_DIVIDER(n)	//not use in MT6276 	

#endif


#else // other chip

#define CPLL_BASE      	
                              //0x840C0000

#define REG_CPLL_CTRL_REGISTER1		
#define REG_CPLL_CTRL_REGISTER2 		
#define REG_CPLL_CTRL_REGISTER3 		
/*cpll register1*/
#define REG_CPLL_RST_BIT		
#define REG_CPLL_FBDIV_MASK	
#define REG_CPLL_CM_MASK		
/*cpll register2*/
#define REG_CPLL_PDN_BIT	
/*cpll register3*/
#define REG_CPLL_DIGDIV_MASK  
#define SET_CPLL_RST 
#define CLEAR_CPLL_RST		
#define SET_CPLL_DIVCTRL(n)							 
#define SET_CPLL_CALI(n)			
#define SET_CPLL_PWR_ON		
#define SET_CPLL_PWR_DOWN		
#define SET_CPLL_DIGITAL_DIVIDER(n)	


#endif //MT6238,MT6268,MT6236

#endif
