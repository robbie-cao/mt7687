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

#ifndef __HAL_AUDIO_DYNAMIC_DOWNLOAD_HEADER_H__
#define __HAL_AUDIO_DYNAMIC_DOWNLOAD_HEADER_H__

#ifdef __cplusplus
extern "C" {
#endif


/** @brief This structure is for the codec that without using LZMA compression
 */
typedef struct {
    int dsp_start_address;    /**< The dsp address that the codec will be downloaded to.*/
    unsigned char *data_array_buffer;    /**< The data buffer of the codec*/
    unsigned int data_array_length;    /**< The data buffer length of the codec*/
} dsp_dynamic_download_uncompressed_data_t;

/** @brief This structure is for the codec that using LZMA compression
 */
typedef struct {
    int dsp_start_address;    /**< The dsp address that the codec will be downloaded to.*/
    unsigned char *compression_property;    /**< The LZMA compression property*/
    unsigned int property_size;    /**< LZMA_PROPS_SIZE = 5*/
    unsigned char *compressed_data_array_buffer;    /**< The data buffer of the compressed codec*/
    unsigned int compressed_data_array_length;    /**< The data buffer length of the compressed codec*/
    unsigned int uncompressed_data_array_length;    /**< The data length of the uncompressed codec*/
} dsp_dynamic_download_compressed_data_t;

/** @brief This structure is the header for dynamic download
 */
typedef struct {
    int dsp_start_address;    /**< The dsp address that the data will be donwloaded to.*/
    unsigned char *data_array_buffer;    /**< The data buffer that will be donwloaded to dsp */
    unsigned int data_array_length;    /**< The data buffer length of the data buffer length*/
} dsp_dynamic_download_download_info_t;



#ifdef __cplusplus
}
#endif

#endif //__HAL_AUDIO_DYNAMIC_DOWNLOAD_HEADER_H__
