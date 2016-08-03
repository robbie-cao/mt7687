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

#include "bl_pmu.h"
#include "hal_pmu.h"
#include "hal_i2c_pmic.h"
#include "bl_common.h"

#define BL_PMIC_SLAVE_ADDR (0x6B)

unsigned char bl_wrap_d2d_i2c_write(unsigned char* ptr_send)
{
    unsigned char retry_cnt = 0,result_read;

    *(ptr_send) = *(ptr_send) | 0x40;

    do
    {
        result_read = pmic_i2c_send_polling(PMIC_I2C_MASTER,BL_PMIC_SLAVE_ADDR,ptr_send,3);
        retry_cnt++;
    }while((result_read != 0)&&(retry_cnt <= 60));

    retry_cnt--;
    return(retry_cnt);
}

unsigned char bl_wrap_d2d_i2c_read(unsigned char* ptr_send,unsigned char* ptr_read)
{
    pmic_i2c_send_to_receive_config_t config;
    unsigned char retry_cnt = 0,result_read;

    *(ptr_send) = *(ptr_send) | 0x40;
    config.slave_address      = BL_PMIC_SLAVE_ADDR;
    config.send_data          = ptr_send;
    config.send_length        = 2;
    config.receive_buffer     = ptr_read;
    config.receive_length     = 1;

    do
    {
        result_read = pmic_i2c_send_to_receive_polling(PMIC_I2C_MASTER,&config);
        retry_cnt++;
    }while((result_read != 0)&&(retry_cnt <= 60));

    retry_cnt--;
    return(retry_cnt);
}

bool bl_pmu_set_register_value(uint16_t address,uint16_t mask,uint16_t shift,uint16_t value){

    unsigned char send_buffer[3],receive_buffer[1];

    send_buffer[1] = (address)&0x00FF  ;
    send_buffer[0] = ((address >> 8)&0x00FF) | 0x40;

    bl_wrap_d2d_i2c_read(send_buffer,receive_buffer);

    receive_buffer[0] &= (~(mask<<shift));
    send_buffer[2] = receive_buffer[0] | (value<<shift); //data value

    if(bl_wrap_d2d_i2c_write(send_buffer) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

uint16_t bl_pmu_get_register_value(uint16_t address,uint16_t mask,uint16_t shift){

    unsigned char send_buffer[2],receive_buffer[1];

    send_buffer[1] = (address)&0x00FF;
    send_buffer[0] = ((address >> 8)&0x00FF) | 0x40;    //Addr_H[7:4] = 01xx : read 1B

    bl_wrap_d2d_i2c_read(send_buffer,receive_buffer);
    return((receive_buffer[0]>>shift)&mask);
}

void bl_pmic_vcore_vosel_config(PMIC_PSI psi_mode, PMIC_VCORE_VOSEL vcore_vosel)
{
    // psi_mode : OVR/HP/LP/S1
    int vcbuck_voval[7]={2,4,6,8,10,12,14};
    int vc_ldo_vosel[7]={0,2,4,6, 8,10,12};

    bl_pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR     ,PMU_RG_VRC_CFG_KEY0_MASK   ,PMU_RG_VRC_CFG_KEY0_SHIFT  , 0xFC);
    bl_pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR     ,PMU_RG_VRC_CFG_KEY1_MASK   ,PMU_RG_VRC_CFG_KEY1_SHIFT  , 0xDA);
    bl_pmu_set_register_value(PMU_RG_PSIOVR_ADDR           ,PMU_RG_PSIOVR_MASK         ,PMU_RG_PSIOVR_SHIFT        , 0x1);  // switch to PSIOVR

    switch(psi_mode)
    {
        case PMIC_PSI_OVR:
            bl_pmu_set_register_value(PMU_RG_OVR_VC_BUCK_VOVAL_ADDR    ,PMU_RG_OVR_VC_BUCK_VOVAL_MASK  ,PMU_RG_OVR_VC_BUCK_VOVAL_SHIFT , vcbuck_voval[vcore_vosel]);
            bl_pmu_set_register_value(PMU_RG_OVR_VC_LDO_VOSEL_ADDR     ,PMU_RG_OVR_VC_LDO_VOSEL_MASK   ,PMU_RG_OVR_VC_LDO_VOSEL_SHIFT  , vc_ldo_vosel[vcore_vosel]);
        break;

        case PMIC_PSI_HP:
            bl_pmu_set_register_value(PMU_RG_HP_VC_BUCK_VOVAL_ADDR     ,PMU_RG_HP_VC_BUCK_VOVAL_MASK   ,PMU_RG_HP_VC_BUCK_VOVAL_SHIFT  , vcbuck_voval[vcore_vosel]);
            bl_pmu_set_register_value(PMU_RG_HP_VC_LDO_VOSEL_ADDR      ,PMU_RG_HP_VC_LDO_VOSEL_MASK    ,PMU_RG_HP_VC_LDO_VOSEL_SHIFT   , vc_ldo_vosel[vcore_vosel]);
        break;

        case PMIC_PSI_LP:
            bl_pmu_set_register_value(PMU_RG_LP_VC_BUCK_VOVAL_ADDR     ,PMU_RG_LP_VC_BUCK_VOVAL_MASK   ,PMU_RG_LP_VC_BUCK_VOVAL_SHIFT  , vcbuck_voval[vcore_vosel]);
            bl_pmu_set_register_value(PMU_RG_LP_VC_LDO_VOSEL_ADDR      ,PMU_RG_LP_VC_LDO_VOSEL_MASK    ,PMU_RG_LP_VC_LDO_VOSEL_SHIFT   , vc_ldo_vosel[vcore_vosel]);
        break;

        case PMIC_PSI_S1:
            bl_pmu_set_register_value(PMU_RG_S1_VC_BUCK_VOVAL_ADDR     ,PMU_RG_S1_VC_BUCK_VOVAL_MASK   ,PMU_RG_S1_VC_BUCK_VOVAL_SHIFT  , vcbuck_voval[vcore_vosel]);
            bl_pmu_set_register_value(PMU_RG_S1_VC_LDO_VOSEL_ADDR      ,PMU_RG_S1_VC_LDO_VOSEL_MASK    ,PMU_RG_S1_VC_LDO_VOSEL_SHIFT   , vc_ldo_vosel[vcore_vosel]);
        break;

        default:
            //printf("ERROR: Illegal PSI mode for setting VCORE voltage!\n");
        break;
    }

    bl_pmu_set_register_value(PMU_RG_PSIOVR_ADDR           ,PMU_RG_PSIOVR_MASK         ,PMU_RG_PSIOVR_SHIFT        , 0x0);  // switch to PSI Bus
    bl_pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR     ,PMU_RG_VRC_CFG_KEY0_MASK    ,PMU_RG_VRC_CFG_KEY0_SHIFT  , 0x0);
    bl_pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR     ,PMU_RG_VRC_CFG_KEY1_MASK    ,PMU_RG_VRC_CFG_KEY1_SHIFT  , 0x0);
}

void BL_PMIC_VR_CONTROL(PMIC_VR vr_name, PMIC_VR_CTL enable)
{
    int poll_stb_b=1;

    if(enable)  // polling STB status
        poll_stb_b = 0;

    // Fill protection key
    bl_pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR     ,PMU_RG_VRC_CFG_KEY0_MASK    ,PMU_RG_VRC_CFG_KEY0_SHIFT  , 0xFC);
    bl_pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR     ,PMU_RG_VRC_CFG_KEY1_MASK    ,PMU_RG_VRC_CFG_KEY1_SHIFT  , 0xDA);

    switch(vr_name)
    {
        case PMIC_VUSB:
            bl_pmu_set_register_value(PMU_RG_VUSB_EN_ADDR  ,PMU_RG_VUSB_EN_MASK    ,PMU_RG_VUSB_EN_SHIFT   , enable);
            while(poll_stb_b==0)
            {
                poll_stb_b = bl_pmu_get_register_value(PMU_RGS_VUSB_STBSTS_ADDR    ,PMU_RGS_VUSB_STBSTS_MASK   ,PMU_RGS_VUSB_STBSTS_SHIFT);
            }
        break;

        default:
            //printf("ERROR: No matching VR!\n");
        break;
    }

    // Clear protection key
    bl_pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR     ,PMU_RG_VRC_CFG_KEY0_MASK    ,PMU_RG_VRC_CFG_KEY0_SHIFT  , 0x0);
    bl_pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR     ,PMU_RG_VRC_CFG_KEY1_MASK    ,PMU_RG_VRC_CFG_KEY1_SHIFT  , 0x0);
}
