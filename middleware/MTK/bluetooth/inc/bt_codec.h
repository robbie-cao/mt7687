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

#ifndef __BT_CODEC_H__
#define __BT_CODEC_H__

/**
 * @addtogroup BT
 * @{
 * @addtogroup Common
 * @{
 * @addtogroup Common_4 Bluetooth codec
 * @{
 * This document introduces details on Bluetooth codec APIs including terms and acronyms,
 * supported features, details on how to use the codec, function groups, enums, structures and functions.
 *
 * The Bluetooth codec is used for A2DP and HFP.
 *
 * @section Terms_Chapter Terms and acronyms
 *
 * |Terms               |Details                                                                                                                                                 |
 * |--------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------|
 * |\b A2DP             | The Advanced Audio Distribution Profile (A2DP) defines the protocols and procedures that implement distribution of audio content of high-quality in mono or stereo on ACL channels. |
 * |\b HFP              | The Hands-free Profile (HFP) defines the device acting as the audio gateway's remote audio input and output mechanism. |
 * |\b SBC              | The Low Complexity Subband Coding (SBC) is an audio subband codec specified by the Bluetooth Special Interest Group (SIG) for the Advanced Audio Distribution Profile (A2DP). SBC is a digital audio encoder and decoder used to transfer data to Bluetooth audio output devices like headphones or loudspeakers. |
 *
 * @section bt_codec_api_usage How to use this module
 *
 * - The A2DP sink codec
 *   - Open and close the A2DP sink codec using #bt_codec_a2dp_open() and #bt_codec_a2dp_close().
 *    - Sample code:
 *     @code
 *       bt_media_handle_t  handle;
 *
 *       void bt_audio_callback(bt_media_handle_t *handle, bt_event_t event)
 *       {
 *           if (event == BT_MEDIA_ERROR ) {
 *               // Error handling, such as inform the Bluetooth module or stop the A2DP codec.
 *           }
 *       }
 *
 *       void bt_audio_sink_sbc_open(void)
 *       {
 *           bt_codec_a2dp_audio_t sbc_sink_config;
 *           bt_a2dp_sbc_codec_t   sbc_codec_parameter;
 *           uint8_t* ring_buf =  NULL;
 *
 *           // Configure the SBC codec capability parameters.
 *           sbc_codec_parameter.min_bit_pool =;
 *           sbc_codec_parameter.max_bit_pool =;
 *           sbc_codec_parameter.block_length =;
 *           sbc_codec_parameter.subband_num  =;
 *           sbc_codec_parameter.alloc_method =;
 *           sbc_codec_parameter.sample_rate  =;
 *           sbc_codec_parameter.channel_mode =;
 *
 *           sbc_sink_config.role                = BT_A2DP_SINK;
 *           sbc_sink_config.codec_cap.type      = BT_A2DP_CODEC_SBC;
 *           sbc_sink_config.codec_cap.codec.sbc = sbc_codec_parameter;
 *
 *           // Open the A2DP codec.
 *           handle = bt_codec_a2dp_open(bt_audio_callback, &sbc_sink_config);
 *           if (handle == NULL) {
 *               // Error handling, failed to open the codec.
 *           }
 *
 *           // Set a shared buffer.
 *           ring_buf = (uint8_t *)malloc(BT_RING_BUFFER_SIZE);
 *           handle->set_buffer(handle, (uint8_t*)ring_buf, BT_RING_BUFFER_SIZE);
 *
 *           // Start the A2DP codec.
 *           handle->play(handle);
 *       }
 *
 *       void bt_audio_sink_sbc_close(void)
 *       {
 *           handle->stop(handle);
 *           bt_codec_a2dp_close(handle);
 *       }
 *
 *       void bt_a2dp_sink_fill_data(void)
 *       {
 *           uint32_t ring_buffer_free_space = 0, available_size = 0, data_length = 0;
 *           uint8_t *data_buffer = 0;
 *
 *           // Data length and data management at Bluetooth driver itself.
 *
 *           ring_buffer_free_space = handle->get_free_space(handle);
 *           // Get the current length of buffer to write the data.
 *           handle->get_write_buffer(handle, &data_buffer, &available_size);
 *
 *           // Fill the buffer #data_buffer with proper amount of data depending on #ring_buffer_free_space, #available_size and #data_length.
 *           handle->write_data_done(handle, data_length);
 *       }
 *     @endcode
 * - The HFP codec
 *   - Open and close the HFP codec using #bt_codec_hfp_open() and #bt_codec_hfp_close().
 *    - Sample code:
 *     @code
 *       bt_media_handle_t handle;
 *
 *       void bt_audio_codec_hfp_callback(bt_media_handle_t *handle, bt_event_t event)
 *       {
 *           if (event == BT_MEDIA_ERROR ) {
 *               // Error handling, such as inform the Bluetooth module or stop the HFP codec.
 *           }
 *       }
 *
 *       void bt_audio_codec_hfp_start(void)
 *       {
 *           bt_codec_hfp_audio_t hfp_config;
 *           bt_status_t status;
 *           hfp_config.type = BT_HF_CODEC_CVSD;
 *           // Open the HFP codec.
 *           handle = bt_codec_hfp_open(bt_audio_codec_hfp_callback, &hfp_config);
 *           if (handle == NULL) {
 *               // Error handling, failed to open the codec.
 *           }
 *           // Start the HFP codec.
 *           status = handle->play(handle);
 *           if (status != BT_STATUS_MEDIA_OK) {
 *               // Error handling, failed to play the media.
 *           }
 *       }
 *
 *       void bt_audio_codec_hfp_stop(void)
 *       {
 *           bt_status_t status;
 *           status = handle->stop(handle);
 *           if (status != BT_STATUS_MEDIA_OK) {
 *               // Error handling, failed to stop the codec.
 *           }
 *           status = bt_codec_hfp_close(handle);
 *           if (status != BT_STATUS_MEDIA_OK) {
 *               // Error handling, failed to close the codec.
 *           }
 *       }
 *     @endcode
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "bt_a2dp.h"
#include "bt_hfp.h"
#include "bt_status.h"
#include "bt_events.h"

/** @defgroup bt_codec_enum Enum
  * @{
  */

/** @brief Bluetooth codec state */
typedef enum {
    BT_CODEC_STATE_IDLE,      /**< The Bluetooth codec is inactive. */
    BT_CODEC_STATE_READY,     /**< The Bluetooth codec is ready to play the media. */
    BT_CODEC_STATE_PLAY,      /**< The Bluetooth codec is in a playing state. */
    BT_CODEC_STATE_ERROR,     /**< The Bluetooth codec reported an error. */
    BT_CODEC_STATE_STOP       /**< The Bluetooth codec has stopped. */
} bt_codec_state_t;

/**
  * @}
  */

/** @defgroup bt_codec_struct Struct
  * @{
  */

/** @brief share buffer structure */
typedef struct {
    uint8_t         *buffer_base;         /**< Pointer to the ring buffer. */
    int32_t         buffer_size;          /**< Size of the ring buffer. */
    int32_t         write;                /**< Index of the ring buffer to write the data. */
    int32_t         read;                 /**< Index of the ring buffer to read the data. */
    void            *param;               /**< Extended parameter or information. */
} bt_audio_buffer_t;

/** @brief A2DP audio handle structure type*/
typedef struct bt_media_handle_s bt_media_handle_t;

/** @brief A2DP audio handle structure */
struct bt_media_handle_s {
    bt_codec_state_t      state;          /**< The Bluetooth codec state. */
    bt_audio_buffer_t     buffer_info;    /**< The shared buffer information for the Bluetooth codec. */
    bool                  underflow;      /**< The Bluetooth codec data underflow occurred. */
    bool                  waiting;        /**< The Bluetooth codec is waiting to fill data. */
    uint16_t              audio_id;       /**< The audio ID of the Bluetooth codec. */

    void           (*handler)           (bt_media_handle_t *handle, bt_event_t event_id);                  /**< The Bluetooth codec handler. */
    void           (*set_buffer)        (bt_media_handle_t *handle, uint8_t  *buffer, uint32_t  length);   /**< Set the shared buffer for bitstream. */
    void           (*get_write_buffer)  (bt_media_handle_t *handle, uint8_t **buffer, uint32_t *length);   /**< Get the available length to write into a shared buffer and a pointer to the shared buffer. */
    void           (*get_read_buffer)   (bt_media_handle_t *handle, uint8_t **buffer, uint32_t *length);   /**< Get the available length to read from a shared buffer and a pointer to the shared buffer. */
    void           (*write_data_done)   (bt_media_handle_t *handle, uint32_t length);                      /**< Update the write pointer to the shared buffer. */
    void           (*finish_write_data) (bt_media_handle_t *handle);                                       /**< Indicate last data transfer. */
    void           (*reset_share_buffer)(bt_media_handle_t *handle);                                       /**< Reset the shared buffer's information. */
    void           (*read_data_done)    (bt_media_handle_t *handle, uint32_t length);                      /**< Update the read pointer to the shared buffer. */
    int32_t        (*get_free_space)    (bt_media_handle_t *handle);                                       /**< Get the free space length available in the shared buffer. */
    int32_t        (*get_data_count)    (bt_media_handle_t *handle);                                       /**< Get available data amount of the shared buffer. */

    bt_status_t  (*play)   (bt_media_handle_t *handle);                       /**< The Bluetooth codec play function. */
    bt_status_t  (*process)(bt_media_handle_t *handle, bt_event_t event_id);  /**< The Bluetooth codec process function. */
    bt_status_t  (*stop)   (bt_media_handle_t *handle);                       /**< The Bluetooth codec stop function. */
};

/** @brief The A2DP audio codec structure */
typedef struct {
    bt_a2dp_codec_capability_t codec_cap;  /**< The capabilities of Bluetooth codec */
    bt_a2dp_role_t             role;       /**< The Bluetooth codec roles */
} bt_codec_a2dp_audio_t;

/** @brief The HFP codec structure */
typedef struct {
    bt_hf_codec_type_t type;   /**< 1: CVSD, 2: mSBC */
} bt_codec_hfp_audio_t;

/**
  * @}
  */

/** @defgroup bt_codec_typedef Typedef
  * @{
  */

/** @brief  This defines the A2DP codec callback function prototype.
 *          The user should register a callback function while opening the A2DP codec. Please refer to #bt_codec_a2dp_open().
 *  @param[in] handle is the media handle of the A2DP codec.
 *  @param[in] event_id is the value defined in #bt_event_t. This parameter is given by the driver to notify the user about the data flow behavior.
 */
typedef void (*bt_codec_a2dp_callback_t)(bt_media_handle_t *handle, bt_event_t event_id);

/** @brief  This defines the HFP codec callback function prototype.
 *          The user should register a callback function while opening a HFP codec. Please refer to #bt_codec_hfp_open().
 *  @param[in] handle is the media handle of the HFP codec.
 *  @param[in] event_id is the value defined in #bt_event_t. This parameter is given by the driver to tell the user of data flow processing behavior.
 */
typedef void (*bt_codec_hfp_callback_t)(bt_media_handle_t *handle, bt_event_t event_id);

/**
  * @}
  */


/**
 * @brief     This function opens the A2DP audio codec.
 * @param[in] callback is a pointer to the callback function for the A2DP data control.
 * @param[in] param is the settings of A2DP codec.
 * @return    Media handle of the A2DP. Please refer to #bt_media_handle_t.
 * @sa        #bt_codec_a2dp_close()
 */
bt_media_handle_t *bt_codec_a2dp_open(bt_codec_a2dp_callback_t callback, const bt_codec_a2dp_audio_t *param);

/**
 * @brief     This function closes the A2DP audio codec.
 * @param[in] handle is the A2DP codec handle.
 * @return    If the operation completed successfully, the return value is #BT_STATUS_MEDIA_OK, otherwise the return value is #BT_STATUS_MEDIA_ERROR.
 * @sa        #bt_codec_a2dp_open()
 */
bt_status_t bt_codec_a2dp_close(bt_media_handle_t *handle);

/**
 * @brief     This function opens the HFP audio codec.
 * @param[in] callback is a pointer to the callback function for HFP error handling.
 * @param[in] param is the HFP codec settings.
 * @return    Media handle of the HFP.
 * @sa        #bt_codec_hfp_close()
 */
bt_media_handle_t *bt_codec_hfp_open(bt_codec_hfp_callback_t callback, const bt_codec_hfp_audio_t *param);

/**
 * @brief     This function closes the HFP codec.
 * @param[in] handle is the HFP codec handle.
 * @return    If the operation completed successfully, the return value is #BT_STATUS_MEDIA_OK, otherwise the return value is #BT_STATUS_MEDIA_ERROR.
 * @sa        #bt_codec_hfp_open()
 */
bt_status_t bt_codec_hfp_close(bt_media_handle_t *handle);

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
* @}
*/

#endif /* __BT_CODEC_H__ */

