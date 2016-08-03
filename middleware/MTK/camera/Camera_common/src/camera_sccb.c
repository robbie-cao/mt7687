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

#include "camera_sccb.h"
#include "hal_i2c_master.h"
#include "stdio.h"
#include "hal_gpio.h"
#include "syslog.h"

typedef struct
{
  kal_uint16 SccbSpeed;
  kal_uint8 SccbAddrLen;
  kal_uint8 SccbDataLen;
  kal_uint8 SccbWriteId;
  hal_i2c_port_t I2C_port;
} CAMERA_SCCB_INFO_STRUCT, *P_CAMERA_SCCB_INFO_STRUCT;

//P_CAMERA_SCCB_INFO_STRUCT Info = NULL;
CAMERA_SCCB_INFO_STRUCT Info = {0,};

/*************************************************************************
* FUNCTION
*    CamWriteCmosSensor
*
* DESCRIPTION
*    This function wirte data to CMOS sensor through I2C
*
* PARAMETERS
*    Addr: address of register
*    Para: value of register
*
* RETURNS
*    None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void CamWriteCmosSensor(kal_uint8 Addr, kal_uint8 Para)
{
    kal_uint8 txbuffer[2];
    hal_i2c_status_t ret;

    txbuffer[0] = Addr;
    txbuffer[1] = Para;

    ret = hal_i2c_master_send_polling(Info.I2C_port, Info.SccbWriteId, txbuffer, sizeof(txbuffer));
    if ( HAL_I2C_STATUS_OK != ret) {
        LOG_E(hal, "[CmosSensor] I2C Trasmit error\r\n");
    }
}

/*************************************************************************
* FUNCTION
*    CamReadCmosSensor
*
* DESCRIPTION
*    This function read data from CMOS sensor through I2C.
*
* PARAMETERS
*    Addr: address of register
*
* RETURNS
*    data read through I2C
*
* GLOBALS AFFECTED
*
*************************************************************************/
kal_uint16 CamReadCmosSensor(kal_uint16 Addr)
{
    hal_i2c_status_t ret;

    uint8_t WriteBuff[2] = {0};
    uint8_t ReadBuff[2] = {0};
    kal_uint16 GetByte = 0;

    if (2 == Info.SccbAddrLen) {
      WriteBuff[0] = Addr >> 8;
      WriteBuff[1] = Addr;
    } else {
      WriteBuff[0] = Addr;
    }

    hal_i2c_send_to_receive_config_t config;

    config.slave_address = Info.SccbWriteId;
    config.send_data = &WriteBuff;
    config.send_length = 1;
    config.receive_buffer = &ReadBuff;
    config.receive_length = sizeof(ReadBuff);

    ret = hal_i2c_master_send_to_receive_polling(Info.I2C_port, &config);
    if ( HAL_I2C_STATUS_OK != ret) {
        LOG_E(hal, "[CamI2C] I2C send and receive error:%d\r\n", ret);
    }

    GetByte = (2 == Info.SccbDataLen ? (ReadBuff[0] << 8)|ReadBuff[1] : ReadBuff[0]);

    LOG_I(hal, "%s:%d GetByte=%x\r\n", __FUNCTION__, __LINE__, GetByte);

    return GetByte;
}

/*************************************************************************
* FUNCTION
*    LensWriteCmosSensor
*
* DESCRIPTION
*    This function wirte data to Lens through I2C
*
* PARAMETERS
*    Addr: address of register
*    Para: value of register
*
* RETURNS
*    None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void LensWriteCmosSensor(kal_uint16 Addr, kal_uint16 Para)
{
//  CameraWriteCmosSensor(CAMERA_SCCB_LENS, Addr, Para);
}

/*************************************************************************
* FUNCTION
*    LensReadCmosSensor
*
* DESCRIPTION
*    This function read data from lens through I2C.
*
* PARAMETERS
*    Addr: address of register
*
* RETURNS
*    data read through I2C
*
* GLOBALS AFFECTED
*
*************************************************************************/
kal_uint16 LensReadCmosSensor(kal_uint16 Addr)
{
//  return CameraReadCmosSensor(CAMERA_SCCB_LENS, Addr);
    return 0;
}

/*************************************************************************
* FUNCTION
*    EepromWriteCmosSensor
*
* DESCRIPTION
*    This function wirte data to eeprom through I2C
*
* PARAMETERS
*    Addr: address of register
*    Para: value of register
*
* RETURNS
*    None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void EepromWriteCmosSensor(kal_uint16 Addr, kal_uint16 Para)
{
//  CameraWriteCmosSensor(CAMERA_SCCB_EEPROM, Addr, Para);
}

/*************************************************************************
* FUNCTION
*    EepromReadCmosSensor
*
* DESCRIPTION
*    This function read data from eeprom through I2C.
*
* PARAMETERS
*    Addr: address of register
*
* RETURNS
*    data read through I2C
*
* GLOBALS AFFECTED
*
*************************************************************************/
kal_uint16 EepromReadCmosSensor(kal_uint16 Addr)
{
//  return CameraReadCmosSensor(CAMERA_SCCB_EEPROM, Addr);
    return 0;
}

/*************************************************************************
* FUNCTION
*    CameraSccbOpen
*
* DESCRIPTION
*    This function open and config camera sccb by owner,
*    MUST BE called after CameraSccbInit function
*
* PARAMETERS
*    Owner
*    SccbWriteId
*    SccbAddrWidth: CAMERA_SCCB_8BIT or CAMERA_SCCB_16BIT
*    SccbDataWidth: CAMERA_SCCB_8BIT or CAMERA_SCCB_16BIT
*    HWSccbSpeed: fast speed mode: 100~400(Kbps), high speed mode: >400(Kbps)
*
* RETURNS
*    None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void CameraSccbOpen(CAMERA_SCCB_OWNER Owner,
                           kal_uint8 SccbWriteId,
                           CAMERA_SCCB_WIDTH SccbAddrWidth, CAMERA_SCCB_WIDTH SccbDataWidth,
                           kal_uint16 HWSccbSpeed)
{
    static hal_i2c_config_t i2c_init_parameter;
    hal_i2c_status_t ret;

    switch(HWSccbSpeed)
    {
        case 50:
            i2c_init_parameter.frequency = HAL_I2C_FREQUENCY_50K;
            break;
        case 100:
            i2c_init_parameter.frequency = HAL_I2C_FREQUENCY_100K;
            break;
        case 200:
            i2c_init_parameter.frequency = HAL_I2C_FREQUENCY_200K;
            break;
        case 400:
            i2c_init_parameter.frequency = HAL_I2C_FREQUENCY_400K;
            break;
        default:
            i2c_init_parameter.frequency = HAL_I2C_FREQUENCY_100K;
            break;
    }

    Info.I2C_port = HAL_I2C_MASTER_0;

    ret = hal_i2c_master_init(Info.I2C_port, &i2c_init_parameter);
    if ( HAL_I2C_STATUS_OK != ret ) {
        LOG_E(hal, "[CamI2C] I2C init error:%d\r\n", ret);
    }

    Info.SccbSpeed = HWSccbSpeed;
    Info.SccbWriteId = SccbWriteId;
    Info.SccbAddrLen = (CAMERA_SCCB_8BIT == SccbAddrWidth) ? 1 : 2;
    Info.SccbDataLen = (CAMERA_SCCB_8BIT == SccbDataWidth) ? 1 : 2;
}

/*************************************************************************
* FUNCTION
*    CameraSccbClose
*
* DESCRIPTION
*    This function close camera sccb by owner
*
* PARAMETERS
*    Owner
*
* RETURNS
*    None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void CameraSccbClose(CAMERA_SCCB_OWNER Owner)
{
    hal_i2c_status_t ret;

    ret = hal_i2c_master_deinit(Info.I2C_port);
    if(ret != HAL_I2C_STATUS_OK) {
        LOG_E(hal, "[CamI2C] I2C De-init error:%d\r\n", ret);
    }
}

