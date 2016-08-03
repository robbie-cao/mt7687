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

#ifndef DEMOS_VSM_SIGNAL_REG_H_
#define DEMOS_VSM_SIGNAL_REG_H_

#include "vsm_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	uint16_t addr;
	uint32_t value;
} signal_data_t;

// voltage signal
signal_data_t VSM_SIGNAL_IDLE_array[]={
    {0x2308,0xC0CCCC00}, //IO DRV0  
    {0x230C,0x00000000}, //IO DRV1  
    {0x2324,0x00000155}, //GPIO5~9 OFF 
    {0x3344,0x7CC0007C}, //TOP CK CON Mode  
    {0x3348,0x800000FF}, //TOP CK CON1 Mode  
    {0x3300,0xA9C71555}, //Enable Bandgap Rotation  
    {0x3300,0xA8C71555}, //Enable Bandgap Rotation  
    {0x3308,0x0101D043}, //IA & EKGFE PWD
    {0x3310,0x002F5555}, //EKGADC PWD
    {0x3314,0x0000A800}, //EKGADC CLK PWD
    {0x3318,0x70244212}, //PPGFE PWD
    {0x331C,0x0048C429}, //PPGFE PWD
    {0x3320,0x000D5555}, //PPGADC PWD
    {0x3324,0x0000A050}, //PPGADC PWD
    {0x3328,0x00002900}, //LED DRV PWD
#ifndef USE_EXTERNAL_BOOST
    {0x3334,0x00000040}, //LEDSUP Boost (Disable)
    {0x3334,0x00480000}, //LEDSUP Boost (Disable)
    {0x3334,0x00480100}, //LEDSUP Boost (Enable)
#else
    {0x3334,0x00000000}, //LEDSUP Boost (Disable)
#endif
    {0x3360,0x00000000}, //Enable digital part  Mode[0:6]=[000:018:124:1A5:164:13C:1FF]
};

// voltage signal
signal_data_t VSM_SIGNAL_EKG_array[]={
    {0x2308,0xC0CCCC00}, //IO DRV0  
    {0x230C,0x00000000}, //IO DRV1  
    {0x2324,0x00000155}, //GPIO5~9 OFF 
    //{0x3344,0x7CC00048}, //TOP CK CON Mode  
    {0x3344,0x7CC00068}, //TOP CK CON Mode  
    //{0x3348,0x80000000}, //TOP CK CON1 Mode  
    {0x3348,0x800001E7}, //TOP CK CON1 Mode  
    {0x3334,0x00000000},   //LEDSUP Boost (Disable)
//  {0x3364,0x0000000A}, //EKG Mode selection (1"010") Fs = 256Hz  
    {0x3364,0x0000000B},   //EKG Mode selection (1"011") Fs = 512Hz
//  {0x3364,0x00000009},   //EKG Mode selection (1"011") Fs = 128Hz  
    {0x3300,0xA8C71555},   //Enable Bandgap Rotation  
//  {0x3308,0x0001D462}, //IA gain = 12
//  {0x3308,0x0001D452}, //IA gain = 8
//  {0x3308,0x0001D442},   //IA gain = 6 , 2E mode
    {0x3308,0x0001D042},   //IA gain = 6 , RLD mode
//  {0x3308,0x0001D432}, //IA gain = 4 
//  {0x3308,0x0001D422}, //IA gain = 3
//  {0x3308,0x0001D412}, //IA gain = 2
//  {0x3308,0x0001D402}, //IA gain = 1 
//  {0x3308,0x0001D402}, //IA gain = 1 
//  {0x3310,0x002B5554}, //EKG Mode selection (002B5554) Fs = 256Hz
    {0x3310,0x002F5554},   //EKG Mode selection (002F5554) Fs = 512Hz 
//  {0x3310,0x00275554},   //EKG Mode selection (002F5554) Fs = 128Hz
    {0x3314,0x0000A802},   //EKGADC CON1 
//  {0x334C,0x00000006},   //Enable EKG SRAM Interrupt_Mode-0 EKG 
    {0x334C,0x00000002},   //Enable EKG SRAM threshold Interrupt_Mode-0 EKG 
    {0x33CC,0x00000040},   //Write irq_th EKG 
//  {0x33DC,0x000000FA}, //Write irq_th PPG1
//  {0x33EC,0x000000FA}, //Write irq_th PPG2
//  {0x33FC,0x00000100}, //Write irq_th BISI
    {0x33C0,0x60000000},   //Write address & 0xC0[29] & 0xC0[30] 
//  {0x33D0,0x60000000},   
//  {0x33E0,0x60000000},   
//  {0x33F0,0x60000000},     
    {0x3360,0x00000018}    //Enable Mode[0:5]=[018:124:187:164:33C:3FF]                        
};

signal_data_t VSM_SIGNAL_EEG_array[]={
//  {0x2308,0xC0CCCC00}, //IO DRV0                                    
//  {0x230C,0x00000000}, //IO DRV1                                    
//  {0x2324,0x000001A2}, //GPIO5~9 OFF                                
//  {0x3344,0x7CE06060}, //TOP CK CON Mode                            
//  {0x3348,0x800000E7}, //TOP CK CON1 Mode                           
    {0x3334,0x00000000},   //LEDSUP Boost (Disable)                     
//  {0x3364,0x0000000A}, //EKG Mode selection (1"010") Fs = 256Hz     
    {0x3364,0x0000000B},   //EKG Mode selection (1"011") Fs = 512Hz     
    {0x3300,0xA9C71555},   //Enable Bandgap Rotation                    
//  {0x3308,0x0001D462}, //IA gain = 12                             
//  {0x3308,0x0001D452}, //IA gain = 8                              
    {0x3308,0x0001D442},   //IA gain = 6                                
//  {0x3308,0x0001D432}, //IA gain = 4                              
//  {0x3308,0x0001D422}, //IA gain = 3                              
//  {0x3308,0x0001D412}, //IA gain = 2                              
//  {0x3308,0x0001D402}, //IA gain = 1                              
//  {0x3310,0x002B5554}, //EKG Mode selection (002B5554) Fs = 256Hz 
    {0x3310,0x002F5554},   //EKG Mode selection (002F5554) Fs = 512Hz  
    {0x3314,0x0000A802},   //EKGADC CON1                               
    {0x334C,0x00000006},   //Enable EKG SRAM Interrupt_Mode-0 EKG      
    {0x33CC,0x00000000},   //Write irq_th EKG                              
//  {0x33DC,0x000000FA}, //Write irq_th PPG1                               
//  {0x33EC,0x000000FA}, //Write irq_th PPG2                               
//  {0x33FC,0x00000100}, //Write irq_th BISI                               
    {0x33C0,0x60000000},   //Write address & 0xC0[29] & 0xC0[30]       
//  {0x33D0,0x60000000},                                              
//  {0x33E0,0x60000000},                                              
//  {0x33F0,0x60000000},                                                
    {0x3360,0x00000018}     //Enable Mode[0:5]=[018:124:187:164:33C:3FF]

};

signal_data_t VSM_SIGNAL_EMG_array[]={

//  {0x2308,0xC0CCCC00}, //IO DRV0                                    
//  {0x230C,0x00000000}, //IO DRV1                                    
//  {0x2324,0x000001A2}, //GPIO5~9 OFF                                
//  {0x3344,0x7CE06060}, //TOP CK CON Mode                            
//  {0x3348,0x800000E7}, //TOP CK CON1 Mode                           
    {0x3334,0x00000000},   //LEDSUP Boost (Disable)                     
//  {0x3364,0x0000000A}, //EKG Mode selection (1"010") Fs = 256Hz     
    {0x3364,0x0000000B},   //EKG Mode selection (1"011") Fs = 512Hz     
    {0x3300,0xA9C71555},   //Enable Bandgap Rotation                    
//  {0x3308,0x0001D462}, //IA gain = 12                             
//  {0x3308,0x0001D452}, //IA gain = 8                              
    {0x3308,0x0001D442},   //IA gain = 6                                
//  {0x3308,0x0001D432}, //IA gain = 4                              
//  {0x3308,0x0001D422}, //IA gain = 3                              
//  {0x3308,0x0001D412}, //IA gain = 2                              
//  {0x3308,0x0001D402}, //IA gain = 1                              
//  {0x3310,0x002B5554}, //EKG Mode selection (002B5554) Fs = 256Hz 
    {0x3310,0x002F5554},   //EKG Mode selection (002F5554) Fs = 512Hz  
    {0x3314,0x0000A802},   //EKGADC CON1                               
    {0x334C,0x00000006},   //Enable EKG SRAM Interrupt_Mode-0 EKG      
    {0x33CC,0x00000000},   //Write irq_th EKG                             
//  {0x33DC,0x000000FA}, //Write irq_th PPG1                             
//  {0x33EC,0x000000FA}, //Write irq_th PPG2                             
//  {0x33FC,0x00000100}, //Write irq_th BISI                             
    {0x33C0,0x60000000},   //Write address & 0xC0[29] & 0xC0[30]       
//  {0x33D0,0x60000000},                                              
//  {0x33E0,0x60000000},                                              
//  {0x33F0,0x60000000},                                                
    {0x3360,0x00000018}    //Enable Mode[0:5]=[018:124:187:164:33C:3FF]

};

signal_data_t VSM_SIGNAL_GSR_array[]={

//  {0x2308,0xC0CCCC00}, //IO DRV0                                    
//  {0x230C,0x00000000}, //IO DRV1                                    
//  {0x2324,0x000001A2}, //GPIO5~9 OFF                                
//  {0x3344,0x7CE06060}, //TOP CK CON Mode                            
//  {0x3348,0x800000E7}, //TOP CK CON1 Mode                           
    {0x3334,0x00000000},   //LEDSUP Boost (Disable)                     
//  {0x3364,0x0000000A}, //EKG Mode selection (1"010") Fs = 256Hz     
    {0x3364,0x0000000B},   //EKG Mode selection (1"011") Fs = 512Hz     
    {0x3300,0xA9C71555},   //Enable Bandgap Rotation                    
//  {0x3308,0x0001D462}, //IA gain = 12                             
//  {0x3308,0x0001D452}, //IA gain = 8                              
//  {0x3308,0x0001D442}, //IA gain = 6                                
//  {0x3308,0x0001D432}, //IA gain = 4                              
//  {0x3308,0x0001D422}, //IA gain = 3                              
//  {0x3308,0x0001D412}, //IA gain = 2                              
    {0x3308,0x0001D402},   //IA gain = 1                              
//  {0x3310,0x002B5554}, //EKG Mode selection (002B5554) Fs = 256Hz 
    {0x3310,0x002F5554},   //EKG Mode selection (002F5554) Fs = 512Hz  
    {0x3314,0x0000A802},   //EKGADC CON1                               
    {0x334C,0x00000006},   //Enable EKG SRAM Interrupt_Mode-0 EKG      
    {0x33CC,0x00000000},   //Write irq_th EKG                              
//  {0x33DC,0x000000FA}, //Write irq_th PPG1                              
//  {0x33EC,0x000000FA}, //Write irq_th PPG2                              
//  {0x33FC,0x00000100}, //Write irq_th BISI                              
    {0x33C0,0x60000000},   //Write address & 0xC0[29] & 0xC0[30]       
//  {0x33D0,0x60000000},                                              
//  {0x33E0,0x60000000},                                              
//  {0x33F0,0x60000000},                                                
    {0x3360,0x00000018}    //Enable Mode[0:5]=[018:124:187:164:33C:3FF]

};


// Current signal
signal_data_t VSM_SIGNAL_PPG1_array[]={
    {0x2308, 0xC0CCCC00},  //IO DRV0
    {0x230C, 0x00000000},  //IO DRV1
    {0x2324, 0x00000155},  //GPIO5~9 off
    //{0x3344, 0x7CC00048},  //TOP CK CON Mode
    {0x3344, 0x7CC00058},  //TOP CK CON Mode
    //{0x3348, 0x80000000},  //TOP CK CON1 Mode
    {0x3348, 0x800000dB},  //TOP CK CON1 Mode
    {0x232C, 0x000020CB},   //PRP=125
    {0x2330, 0x004100C1},   //LED1
    {0x2334, 0x004100C1},   //LED2
    {0x2338, 0x008000C0},   //SAMPLED1
    {0x233C, 0x02020242},   //SAMPLED2
    {0x2340, 0x01410181},   //SAMPAMB1
    {0x2344, 0x02C30303},   //SAMPAMB2
    {0x2348, 0x00C20182},   //ConvLED1
    {0x234C, 0x02440304},   //ConvLED2
    {0x2350, 0x01830243},   //ConvAMB1
    {0x2354, 0x030503C5},   //ConvAMB2
    {0x2358, 0x00C100C2},   //ADC RST1
    {0x235C, 0x01820183},   //ADC RST2
    {0x2360, 0x02430244},   //ADC RST3
    {0x2364, 0x03040305},   //ADC RST4
    {0x2368, 0x03F72010},   //ADC Enable Dynamic PWD
//  {0x2368, 0x00000000}, //ADC Disable Dynamic PWD
    {0x236C, 0x03F72042},   //FE Enable Dynamic PWD
//  {0x236C, 0x00000000}, //FE Disable Dynamic PWD
    {0x2370, 0x00C00343},   //PPG out EN Enable Dynamic PWD
//  {0x2370, 0x20C320C3}, //PPG out EN Disable Dynamic PWD
                        
    {0x3368, 0x04B6270A},   //NUMAVG=1
//  {0x3368, 0x08B6270A}, //NUMAVG=2
//  {0x3370,0x03682AA9},  //Set SI
//  {0x3378,0x000E488F},  //Set BI
//  {0x337C,0x00007AF9},  //Set BI
//  {0x3380,0x00020000},  //Set LPF Coefficients
//  {0x3384,0x02B103A8},  //Set LPF Coefficients
//  {0x3388,0x038501E4},  //Set LPF Coefficients
//  {0x338C,0xFEE3FB36},  //Set LPF Coefficients
//  {0x3390,0xF804F695},  //Set LPF Coefficients
//  {0x3394,0xF7E8FC68},  //Set LPF Coefficients
//  {0x3398,0x03C60D0D},  //Set LPF Coefficients
//  {0x339C,0x16E41FD3},  //Set LPF Coefficients
//  {0x33A0,0x26902A2D},  //Set LPF Coefficients
//    {0x334C,0x00000030},    //Enable ALL SRAM Interrupt_PPG1
    {0x334C,0x00000010},    //Enable threshold SRAM Interrupt_PPG1
    {0x3300,0xA8C71555},      //BG Reference Setting 
//  {0x3318,0x303E4212},  //small PD (TIA=100KOhm,PGA=6,Cp=+50pF)
    {0x3318,0x303E5292},    //Large PD (TIA=100KOhm,PGA=6,Cp=+200pF)
//  {0x3320,0x000F5554},  //PPG ADC Chopper ON    
    {0x3320,0x000D5554},    //PPG ADC Chopper OFF
    {0x3324,0x0000A010},    //PPGADC_CON1
    {0x3328,0x00002CFE},    //LED Full range Current [7:5]=[010] =>7.5mA*3 =22.5mA
    {0x332C,0x00002020},    //LED Current =22.5mA/256x(DAC code)= 0.087890625mA x 32 = 2.8125mA
#ifndef USE_EXTERNAL_BOOST
    {0x3334,0x00480000},    //LEDSUP Boost (Bias Enable)
    {0x3334,0x00480100},    //LEDSUP Boost (Boost Enable)
#else
    {0x3334,0x00000000}, //LEDSUP Boost (Disable)
#endif
//  {0x3334,0x00000000},  //LEDSUP Boost (Disable) 
//  {0x33CC,0x00000000},  //Write irq_th EKG
    //{0x33DC,0x000000FA},    //Write irq_th PPG1
    {0x33DC,0x00000020},    //Write irq_th PPG1
//  {0x33EC,0x000000FA},  //Write irq_th PPG2
//  {0x33FC,0x00000100},  //Write irq_th BISI
//  {0x33C0,0x60000000},  //Write address & 0xC0[29] & 0xC0[30]
    {0x33D0,0x60000000},  
//  {0x33E0,0x60000000},
//  {0x33F0,0x60000000},
    {0x3360,0x00000124}   //Enable Mode[0:5]=[018:124:187:164:33C:3FF]

};

signal_data_t VSM_SIGNAL_PPG2_array[]={
                                                                                               
    {0x2308,0xC0CCCC00},  //IO DRV0                                       
    {0x230C,0x00000000},  //IO DRV1                                       
    {0x2324,0x00000155},  //GPIO5~9 off
    //{0x3344, 0x7CC00048},  //TOP CK CON Mode
    {0x3344, 0x7CC00058},  //TOP CK CON Mode
    //{0x3348, 0x80000000},  //TOP CK CON1 Mode
    {0x3348, 0x8000009B},  //TOP CK CON1 Mode
    {0x232C, 0x000020CB},   //PRP=125                                           
    {0x2330, 0x004100C1},   //LED1                                          
    {0x2334, 0x01C30243},   //LED2                                          
    {0x2338, 0x008000C0},   //SAMPLED1                                      
    {0x233C, 0x02020242},   //SAMPLED2                                      
    {0x2340, 0x01410181},   //SAMPAMB1                                      
    {0x2344, 0x02C30303},   //SAMPAMB2                                      
    {0x2348, 0x00C20182},   //ConvLED1                                      
    {0x234C, 0x02440304},   //ConvLED2                                      
    {0x2350, 0x01830243},   //ConvAMB1                                      
    {0x2354, 0x030503C5},   //ConvAMB2                                      
    {0x2358, 0x00C100C2},   //ADC RST1                                      
    {0x235C, 0x01820183},   //ADC RST2                                      
    {0x2360, 0x02430244},   //ADC RST3                                      
    {0x2364, 0x03040305},   //ADC RST4                                      
    {0x2368, 0x03F72010},   //ADC Enable Dynamic PWD                        
//  {0x2368, 0x00000000}, //ADC Disable Dynamic PWD                       
    {0x236C, 0x03F72042},   //FE Enable Dynamic PWD                         
//  {0x236C, 0x00000000}, //FE Disable Dynamic PWD                        
    {0x2370, 0x00C00343},   //PPG out EN Enable Dynamic PWD                 
//  {0x2370, 0x20C320C3}, //PPG out EN Disable Dynamic PWD                  
                                                                          
    {0x3368, 0x04B6270A},   //NUMAVG=1                                      
//  {0x3368, 0x08B6270A}, //NUMAVG=2                                        
//  {0x3370,0x03682AA9},  //Set SI                                        
//  {0x3378,0x000E488F},  //Set BI                                        
//  {0x337C,0x00007AF9},  //Set BI                                        
//  {0x3380,0x00020000},  //Set LPF Coefficients                          
//  {0x3384,0x02B103A8},  //Set LPF Coefficients                          
//  {0x3388,0x038501E4},  //Set LPF Coefficients                          
//  {0x338C,0xFEE3FB36},  //Set LPF Coefficients                          
//  {0x3390,0xF804F695},  //Set LPF Coefficients                          
//  {0x3394,0xF7E8FC68},  //Set LPF Coefficients                          
//  {0x3398,0x03C60D0D},  //Set LPF Coefficients                          
//  {0x339C,0x16E41FD3},  //Set LPF Coefficients                          
//  {0x33A0,0x26902A2D},  //Set LPF Coefficients                          
//    {0x334C,0x00000180},  //Enable ALL SRAM Interrupt_PPG2        
    {0x334C,0x00000080},  //Enable threshold SRAM Interrupt_PPG2        
    {0x3300,0xA8C71555},  //BG Reference Setting                          
    {0x3318,0x303E5292},  //small PD (TIA=100KOhm,PGA=6,Cp=+50pF)         
//  {0x3318,0x303EC612},  //Large PD (TIA=100KOhm,PGA=6,Cp=+200pF)        
//  {0x3320,0x000F5554},  //PPG ADC Chopper ON                            
    {0x3320,0x000D5554},  //PPG ADC Chopper OFF                           
    {0x3324,0x0000A010},  //PPGADC_CON1                                   
    {0x3328,0x00002CFE},  //LED Full range Current [7:5]=[010] =>7.5mA    
    {0x332C,0x00002020},  //LED Current =22.5mA/256x(DAC code)= 0.0878
#ifndef USE_EXTERNAL_BOOST
    {0x3334,0x00480000},    //LEDSUP Boost (Bias Enable)
    {0x3334,0x00480100},    //LEDSUP Boost (Boost Enable)
#else
    {0x3334,0x00000000}, //LEDSUP Boost (Disable)
#endif
//  {0x3334,0x00000000},  //LEDSUP Boost (Disable)                        
//  {0x33CC,0x00000000},  //Write irq_th EKG                                 
    {0x33DC,0x00000020},  //Write irq_th PPG1                                               
    {0x33EC,0x00000020},  //Write irq_th PPG2                                             
//  {0x33FC,0x00000100},  //Write irq_th BISI                                               
//  {0x33C0,0x60000000},  //Write address & 0xC0[29] & 0xC0[30]           
    {0x33D0,0x60000000},                                                      
    {0x33E0,0x60000000},                                                
//  {0x33F0,0x60000000},                                                  
    {0x3360,0x00000144}   //Enable Mode[0:5]=[018:124:187:164:33C:3FF]    
    

};


signal_data_t VSM_SIGNAL_BISI_array[]={

    {0x2308,0xC0CCCC00},  //IO DRV0
    {0x230C,0x00000000},  //IO DRV1
    {0x2324,0x00000155},  //GPIO5~9 off
    {0x3344,0x7CC00058},  //TOP CK CON Mode
    //{0x3344, 0x7CC00048},  //TOP CK CON Mode
//  {0x3348,0x8000005A},  //TOP CK CON1 Mode
    //{0x3348, 0x80000000},  //TOP CK CON1 Mode
    {0x3348, 0x8000005A},  //TOP CK CON1 Mode
    {0x232C, 0x000020CB},   //PRP=125                                                                       
    {0x2330, 0x004100C1},   //LED1                                                                      
    {0x2334, 0x004100C1},   //LED2                                                                      
    {0x2338, 0x008000C0},   //SAMPLED1                                                                  
    {0x233C, 0x02020242},   //SAMPLED2                                                                  
    {0x2340, 0x01410181},   //SAMPAMB1                                                                  
    {0x2344, 0x02C30303},   //SAMPAMB2                                                                  
    {0x2348, 0x00C20182},   //ConvLED1                                                                  
    {0x234C, 0x02440304},   //ConvLED2                                                                  
    {0x2350, 0x01830243},   //ConvAMB1                                                                  
    {0x2354, 0x030503C5},   //ConvAMB2                                                                  
    {0x2358, 0x00C100C2},   //ADC RST1                                                                  
    {0x235C, 0x01820183},   //ADC RST2                                                                  
    {0x2360, 0x02430244},   //ADC RST3                                                                  
    {0x2364, 0x03040305},   //ADC RST4                                                                  
    {0x2368, 0x03F72010},   //ADC Enable Dynamic PWD                                                    
//  {0x2368, 0x00000000}, //ADC Disable Dynamic PWD                                                   
    {0x236C, 0x03F72042},   //FE Enable Dynamic PWD                                                     
//  {0x236C, 0x00000000}, //FE Disable Dynamic PWD                                                    
    {0x2370, 0x00C00343},   //PPG out EN Enable Dynamic PWD                                             
//  {0x2370, 0x20C320C3}, //PPG out EN Disable Dynamic PWD                                                                                                      
    {0x3368, 0x04B6270A},   //NUMAVG=1                                                                  
//  {0x3368, 0x08B6270A}, //NUMAVG=2
    {0x3370,0x05099BA8},  //Set SI                                                                    
    {0x3378,0x000E488F},  //Set BI                                                                    
    {0x337C,0x000078FA},  //Set BI                                                                    
    {0x3380,0x0001E000},  //Set LPF Coefficients                                                      
    {0x3384,0xFEEDFF2C},  //Set LPF Coefficients                                                      
    {0x3388,0xFFBA00A4},  //Set LPF Coefficients                                                      
    {0x338C,0x01F103A0},  //Set LPF Coefficients                                                      
    {0x3390,0x05A807F5},  //Set LPF Coefficients                                                      
    {0x3394,0x0A6D0CEE},  //Set LPF Coefficients                                                      
    {0x3398,0x0F531177},  //Set LPF Coefficients                                                      
    {0x339C,0x13361472},  //Set LPF Coefficients                                                      
    {0x33A0,0x15160000},  //Set LPF Coefficients
//    {0x334C,0x00000C00},    //Enable ALL SRAM Interrupt_PPG1+BISI
    {0x334C,0x00000400},    //Enable threshold SRAM Interrupt_PPG1+BISI
    {0x3300,0xA8C71555},      //BG Reference Setting                                                      
//  {0x3318,0x303E4212},  //small PD (TIA=100KOhm,PGA=6,Cp=+50pF)                                     
    {0x3318,0x303E5292},    //Large PD (TIA=100KOhm,PGA=6,Cp=+200pF)                                    
//  {0x3320,0x000F5554},  //PPG ADC Chopper ON                                                        
    {0x3320,0x000D5554},    //PPG ADC Chopper OFF                                                       
    {0x3324,0x0000A010},  //PPGADC_CON1                                                               
    {0x3328,0x00002CFE},  //LED Full range Current [7:5]=[010] =>7.5mA*3 =22.5mA                      
    {0x332C,0x00002020},  //LED Current =22.5mA/256x(DAC code)= 0.087890625mA x 32 = 2.8125mA         
#ifndef USE_EXTERNAL_BOOST
    {0x3334,0x00480000},    //LEDSUP Boost (Bias Enable)
    {0x3334,0x00480100},    //LEDSUP Boost (Boost Enable)
#else
    {0x3334,0x00000000}, //LEDSUP Boost (Disable)
#endif
//  {0x3334,0x00000000},  //LEDSUP Boost (Disable)                                                    
//  {0x33CC,0x00000000},  //Write irq_th EKG                                                          
    {0x33DC,0x00000020},  //Write irq_th PPG1                                                         
//  {0x33EC,0x000000FA},  //Write irq_th PPG2                                                         
    {0x33FC,0x00000020},  //Write irq_th BISI                                                         
//  {0x33C0,0x60000000},  //Write address & 0xC0[29] & 0xC0[30]                                       
    {0x33D0,0x60000000},                                                                                
//  {0x33E0,0x60000000},                                                                              
    {0x33F0,0x60000000},                                                                              
    {0x3360,0x000001A5}   //Enable Mode[0:6]=[000:018:124:1A5:164:13C:1FF]                                
                                                                                                      
};

// Current signal
signal_data_t VSM_SIGNAL_PPG1_512HZ_array[]={

//  {0x2308,0xC0CCCC00},  //IO DRV0
//  {0x230C,0x00000000},  //IO DRV1
//  {0x2324,0x000001A2},  //GPIO5~9 off
//  {0x3344,0x7CD81858},  //TOP CK CON Mode
//  {0x3348,0x8000009B},  //TOP CK CON1 Mode
    {0x232C, 0x000007FF},   //PRP=125
    {0x2330, 0x004100C1},   //LED1
    {0x2334, 0x004100C1},   //LED2
    {0x2338, 0x008000C0},   //SAMPLED1
    {0x233C, 0x02020242},   //SAMPLED2
    {0x2340, 0x01410181},   //SAMPAMB1
    {0x2344, 0x02C30303},   //SAMPAMB2
    {0x2348, 0x00C20182},   //ConvLED1
    {0x234C, 0x02440304},   //ConvLED2
    {0x2350, 0x01830243},   //ConvAMB1
    {0x2354, 0x030503C5},   //ConvAMB2
    {0x2358, 0x00C100C2},   //ADC RST1
    {0x235C, 0x01820183},   //ADC RST2
    {0x2360, 0x02430244},   //ADC RST3
    {0x2364, 0x03040305},   //ADC RST4
    {0x2368, 0x03F70744},   //ADC Enable Dynamic PWD
//  {0x2368, 0x00000000}, //ADC Disable Dynamic PWD
    {0x236C, 0x03F70776},   //FE Enable Dynamic PWD
//  {0x236C, 0x00000000}, //FE Disable Dynamic PWD
    {0x2370, 0x00C00343},   //PPG out EN Enable Dynamic PWD
//  {0x2370, 0x20C320C3}, //PPG out EN Disable Dynamic PWD
                        
    {0x3368, 0x04B6270A},   //NUMAVG=1
//  {0x3368, 0x08B6270A}, //NUMAVG=2
//  {0x3370,0x03682AA9},  //Set SI
//  {0x3378,0x000E488F},  //Set BI
//  {0x337C,0x00007AF9},  //Set BI
//  {0x3380,0x00020000},  //Set LPF Coefficients
//  {0x3384,0x02B103A8},  //Set LPF Coefficients
//  {0x3388,0x038501E4},  //Set LPF Coefficients
//  {0x338C,0xFEE3FB36},  //Set LPF Coefficients
//  {0x3390,0xF804F695},  //Set LPF Coefficients
//  {0x3394,0xF7E8FC68},  //Set LPF Coefficients
//  {0x3398,0x03C60D0D},  //Set LPF Coefficients
//  {0x339C,0x16E41FD3},  //Set LPF Coefficients
//  {0x33A0,0x26902A2D},  //Set LPF Coefficients
    {0x334C,0x00000030},    //Enable ALL SRAM Interrupt_PPG1
    {0x3364,0x0000000B},    //Enable ALL SRAM Interrupt_PPG1
    {0x3300,0xA8C71555},      //BG Reference Setting 
    {0x3308,0x0001D042},   
    {0x3310,0x002F5554},   
    {0x3314,0x0000A802},   
//  {0x3318,0x303E4212},  //small PD (TIA=100KOhm,PGA=6,Cp=+50pF)
//    {0x3318,0x303E5292},  //Large PD (TIA=100KOhm,PGA=6,Cp=70pF)
    {0x3318,0x303E39D2},  //Large PD (TIA=100KOhm,PGA=6,Cp=70pF)
//  {0x3320,0x000F5554},  //PPG ADC Chopper ON    
    {0x3320,0x000D5554},    //PPG ADC Chopper OFF
    {0x3324,0x0000A010},    //PPGADC_CON1
    {0x3328,0x00002CFE},    //LED Full range Current [7:5]=[010] =>7.5mA*3 =22.5mA
    {0x332C,0x00002020},    //LED Current =22.5mA/256x(DAC code)= 0.087890625mA x 32 = 2.8125mA
#ifndef USE_EXTERNAL_BOOST
    {0x3334,0x00480000},    //LEDSUP Boost (Bias Enable)
    {0x3334,0x00480100},    //LEDSUP Boost (Boost Enable)
#else
    {0x3334,0x00000000}, //LEDSUP Boost (Disable)
#endif
//  {0x3334,0x00000000},    //LEDSUP Boost (Disable) 
//  {0x33CC,0x00000000},  //Write irq_th EKG
    {0x33DC,0x000000FA},    //Write irq_th PPG1
//  {0x33EC,0x000000FA},  //Write irq_th PPG2
//  {0x33FC,0x00000100},  //Write irq_th BISI
//  {0x33C0,0x60000000},  //Write address & 0xC0[29] & 0xC0[30]
    {0x33D0,0x60000000},  
//  {0x33E0,0x60000000},
//  {0x33F0,0x60000000},
    {0x3360,0x00000124}   //Enable Mode[0:5]=[018:124:187:164:33C:3FF]
};

#ifdef __cplusplus
}
#endif

#endif /* DEMOS_VSM_SIGNAL_REG_H_ */

