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

#ifndef __BT_A2DP_H__
#define __BT_A2DP_H__

/**
 * @addtogroup BT
 * @{
 * @addtogroup A2DP
 * @{
 * This section describes the A2DP APIs.
 * The Advanced Audio Distribution Profile (A2DP) defines the protocols and procedures that realize distribution of audio content of high-quality in mono or stereo on ACL channels.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b SBC                        | The Low Complexity Subband Coding (SBC) is an audio subband codec specified by the Bluetooth Special Interest Group (SIG) for the Advanced Audio Distribution Profile (A2DP). SBC is a digital audio encoder and decoder used to transfer data to Bluetooth audio output devices like headphones or loudspeakers. |
 * |\b AAC                        | Advanced Audio Coding (AAC) is an audio coding standard for lossy digital audio compression. |
 * |\b SEID                       | Stream End Point Identifier (SEID) is a SEP identifier. |
 * |\b SEP                        | Stream End Point (SEP) is a concept to expose available Transport Services and AV capabilities of the Application in order to negotiate a stream. |
 * |\b SNK                        | Sink. A device is the SNK when it acts as a sink of a digital audio stream delivered from the SRC on the same piconet. |
 * |\b SRC                        | Source. A device is the SRC when it acts as a source of a digital audio stream that is delivered to the SNK of the piconet. |
 * |\b piconet                    | A piconet is a computer network which links a wireless user group of devices using Bluetooth technology protocols. |
 *
 * @section bt_a2dp_api_usage How to use this module
 *
 *  - Step1: Mandatory, implement #bt_a2dp_get_init_params() to set the initial configuration (device role, SEP number, SEP detail, etc).
 *   - Sample code:
 *    @code
 *       bt_status_t bt_a2dp_get_init_params(bt_a2dp_init_params_t *params)
 *       {
 *           params->role = BT_A2DP_SINK;
 *           params->sink_sep_num = 1;
 *           params->sink_sep = &cap;
 *           params->sink_sep->type = BT_A2DP_CODEC_SBC;
 *           memcpy(&params->sink_sep->codec.sbc, &sbc_cap_snk[0], sizeof(bt_a2dp_sbc_codec_t));
 *
 *           return BT_STATUS_SUCCESS;
 *       }
 *    @endcode
 *  - Step2: Mandatory, implement #bt_a2dp_common_callback() to handle the events sent by the A2DP.
 *   - Sample code:
 *    @code
 *       void bt_a2dp_common_callback(bt_event_t event_id, const void *param)
 *       {
 *           switch (event_id)
 *           {
 *               case BT_A2DP_CONNECT_CNF:
 *               {
 *                   bt_a2dp_connect_cnf_t *params = (bt_a2dp_connect_cnf_t *)param;
 *                   // Check connection result and save conn_id if connection is successful.
 *                   break;
 *               }
 *               case BT_A2DP_DISCONNECT_CNF:
 *               {
 *                   bt_a2dp_disconnect_cnf_t *params = (bt_a2dp_disconnect_cnf_t *)param;
 *                   // Check disconnection result and clear the A2DP context.
 *                   break;
 *               }
 *               default:
 *                   break;
 *           }
 *       }
 *    @endcode
 *  - Step3: Optional, initiate a connection to a remote device.
 *   - Sample code:
 *    @code
 *       // A BT_A2DP_CONNECT_CNF event notifies the bt_a2dp_common_callback() once the device is connected.
 *       ret = bt_a2dp_connect(dev_addr, role);
 *    @endcode
 *  - Step4: Mandatory, similar to sink, handles the events in #bt_a2dp_common_callback().
 *   - Sample code:
 *    @code
 *       void bt_a2dp_common_callback(bt_event_t event_id, const void *param)
 *       {
 *           switch (event_id)
 *           {
 *               case BT_A2DP_START_STREAMING_IND:
 *               {
 *                   bt_a2dp_start_streaming_ind_t *params = (bt_a2dp_start_streaming_ind_t *)param;
 *                   // Must accept.
 *                   bt_a2dp_start_streaming_response(params->conn_id, true);
 *                   // Open sink codec (initialize the audio driver).
 *                   handle = bt_a2dp_open(callback, a2dp_codec);
 *                   break;
 *               }
 *               case BT_A2DP_SUSPEND_STREAMING_IND:
 *               {
 *                   bt_a2dp_suspend_streaming_ind_t *params = (bt_a2dp_suspend_streaming_ind_t *)param;
 *                   // Must accept.
 *                   bt_a2dp_suspend_streaming_response(params->conn_id, true);
 *                   break;
 *               }
 *               case BT_A2DP_RECONFIGURE_IND:
 *               {
 *                   bt_a2dp_reconfigure_ind_t *params = (bt_a2dp_reconfigure_ind_t *)param;
 *                   // Must accept.
 *                   bt_a2dp_reconfigure_response(params->conn_id, true);
 *                   break;
 *               }
 *               // Add other A2DP handle event ID.
 *               default:
 *                   break;
 *           }
 *       }
 *    @endcode
 *  - Step5: Mandatory, handle media event and write data to audio driver using #bt_a2dp_common_callback().
 *   - Sample code:
 *    @code
 *       void bt_a2dp_common_callback(bt_event_t event_id, const void *param)
 *       {
 *           switch (event_id)
 *           {
 *               case BT_A2DP_STREAM_RECEIVED_IND:
 *               {
 *                   bt_a2dp_stream_data_received_ind_t *params = (bt_a2dp_stream_data_received_ind_t *)param;
 *                   // Write media data to share the ring buffer, which is a memory shared by audio driver and applications.
 *                   handle->write_data_done(handle, params->payload_size);
 *                   break;
 *               }
 *               default:
 *                   break;
 *           }
 *       }
 *    @endcode
 *  - Step6: Optional, active disconnect from a remote device.
 *   - Sample code:
 *    @code
 *       //  A BT_A2DP_DISCONNECT_CNF event notifies the bt_a2dp_common_callback() once the device is disconnected.
 *       ret = bt_a2dp_disconnect(conn_id);
 *    @endcode
 */

#include <stdint.h>
#include <stdbool.h>
#include "bt_address.h"
#include "bt_status.h"
#include "bt_events.h"

/**
 * @defgroup bt_a2dp_enum Enum
 * @{
 */

/**
 *  @brief This enum defines the A2DP codec type.
 */
typedef enum {
    BT_A2DP_CODEC_SBC = 0,      /**< SBC codec. */
    BT_A2DP_CODEC_AAC = 1       /**< AAC codec. */
} bt_a2dp_codec_type_t;

/**
 *  @brief This enum defines the A2DP role type.
 */
typedef enum {
    BT_A2DP_SOURCE,             /**< SRC role. */
    BT_A2DP_SINK,               /**< SNK role. */
    BT_A2DP_SOURCE_AND_SINK,    /**< Both roles for a single device (series case). */
    BT_A2DP_INVALID_ROLE = -1
} bt_a2dp_role_t;

/**
 *  @brief This enum defines A2DP state type.
 */
typedef enum {
    BT_A2DP_IDLE,               /**< Idle state. */
    BT_A2DP_OPENED,             /**< Open state. */
    BT_A2DP_STREAMING,          /**< Streaming state. */
    BT_A2DP_INVALID_STATE = -1
} bt_a2dp_state_t;

/**
 * @}
 */

/**
 * @defgroup bt_a2dp_struct Struct
 * @{
 */

/**
 *  @brief This struct defines the SBC codec details.
 */
typedef struct {
    uint8_t min_bit_pool;       /**< The minimum bit pool. */
    uint8_t max_bit_pool;       /**< The maximum bit pool. */
    uint8_t block_length;       /**< b0: 16, b1: 12, b2: 8, b3: 4. */
    uint8_t subband_num;        /**< b0: 8, b1: 4. */
    uint8_t alloc_method;       /**< b0: loudness, b1: SNR. */
    uint8_t sample_rate;        /**< b0: 48000, b1: 44100, b2: 32000, b3: 16000. */
    uint8_t channel_mode;       /**< b0: joint stereo, b1: stereo, b2: dual channel, b3: mono. */
} bt_a2dp_sbc_codec_t;

/**
 *  @brief This struct defines the AAC codec details.
 */
typedef struct {
    bool vbr;                   /**< Indicates if VBR is supported or not. */
    uint8_t object_type;        /**< b4: M4-scalable, b5: M4-LTP, b6: M4-LC, b7: M2-LC. */
    uint8_t channels;           /**< b0: 2, b1: 1. */
    uint16_t sample_rate;       /**< b0~b11: 96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000. */
    uint32_t bit_rate;          /**< Constant/peak bits per second in 23 bit UiMsbf, 0:unknown. */
} bt_a2dp_aac_codec_t;

/**
 *  @brief This union defines the A2DP codec.
 */
typedef union {
    bt_a2dp_sbc_codec_t sbc;    /**< SBC codec. */
    bt_a2dp_aac_codec_t aac;    /**< AAC codec. */
} bt_a2dp_codec_t;

/**
 *  @brief This struct defines the A2DP codec capability.
 */
typedef struct {
    bt_a2dp_codec_type_t type;  /**< Codec type. */
    bt_a2dp_codec_t codec;      /**< Codec information. */
} bt_a2dp_codec_capability_t;

/**
 *  @brief This struct defines the result of BT_A2DP_CONNECT_IND event.
 */
typedef struct {
    uint16_t conn_id;           /**< Connection ID. */
    bt_a2dp_role_t role;        /**< SEP A2DP role. */
    bt_address_t dev_addr;      /**< Remote device Bluetooth address. */
} bt_a2dp_connect_ind_t;

/**
 *  @brief This struct defines BT_A2DP_CONNECT_CNF result.
 */
typedef struct {
    bt_status_t status;         /**< Connection status. */
    uint16_t conn_id;           /**< Connection ID. */
    bt_address_t dev_addr;      /**< Remote device Bluetooth address. */
} bt_a2dp_connect_cnf_t;

/**
 *  @brief This struct defines BT_A2DP_DISCONNECT_IND result.
 */
typedef struct {
    uint16_t conn_id;           /**< Connection ID. */
} bt_a2dp_disconnect_ind_t;

/**
 *  @brief This struct defines BT_A2DP_DISCONNECT_CNF result.
 */
typedef struct {
    bt_status_t status;         /**< Disconnect status. */
    uint16_t conn_id;           /**< Connection ID. */
} bt_a2dp_disconnect_cnf_t;

/**
 *  @brief This struct defines BT_A2DP_START_STREAMING_IND result.
 */
typedef struct {
    uint16_t conn_id;                           /**< Connection ID. */
    bt_a2dp_codec_capability_t codec_cap;       /**< Streaming channel codec capability. */
} bt_a2dp_start_streaming_ind_t;

/**
 *  @brief This struct defines BT_A2DP_START_STREAMING_CNF result.
 */
typedef struct {
    bt_status_t status;                         /**< Start streaming status. */
    uint16_t conn_id;                           /**< Connection ID. */
    bt_a2dp_codec_capability_t codec_cap;       /**< Streaming channel codec capability. */
} bt_a2dp_start_streaming_cnf_t;

/**
 *  @brief This struct defines BT_A2DP_SUSPEND_STREAMING_IND result.
 */
typedef struct {
    uint16_t conn_id;                           /**< Connection ID. */
} bt_a2dp_suspend_streaming_ind_t;

/**
 *  @brief This struct defines BT_A2DP_SUSPEND_STREAMING_CNF result.
 */
typedef struct {
    bt_status_t status;                         /**< Suspend streaming status. */
    uint16_t conn_id;                           /**< Connection ID. */
} bt_a2dp_suspend_streaming_cnf_t;

/**
 *  @brief This struct defines BT_A2DP_RECONFIGURE_IND result.
 */
typedef struct {
    uint16_t conn_id;                           /**< Connection ID. */
    bt_a2dp_codec_capability_t codec_cap;       /**< Streaming channel codec capability. */
} bt_a2dp_reconfigure_ind_t;

/**
 *  @brief This struct defines BT_A2DP_RECONFIGURE_CNF result.
 */
typedef struct {
    bt_status_t status;                         /**< Reconfigure streaming status. */
    uint16_t conn_id;                           /**< Connection ID. */
    bt_a2dp_codec_capability_t codec_cap;       /**< Streaming channel codec capability. */
} bt_a2dp_reconfigure_cnf_t;

/**
 *  @brief This struct defines BT_A2DP_STREAM_RECEIVED_IND result.
 */
typedef struct {
    uint16_t conn_id;                           /**< Connection ID. */
    uint16_t payload_size;                      /**< Media data size. */
    void *data;                                 /**< Media data pointer. */
} bt_a2dp_stream_data_received_ind_t;

/**
 *  @brief This struct defines initialization parameters.
 */
typedef struct {
    bt_a2dp_role_t role;                        /**< Support role(SNK, SRC, or both). */
    uint8_t src_sep_num;                        /**< Source SEP number. */
    uint8_t sink_sep_num;                       /**< Sink SEP number. */
    bt_a2dp_codec_capability_t *sink_sep;       /**< Sink SEP details. */
    bt_a2dp_codec_capability_t *src_sep;        /**< Source SEP details. */
} bt_a2dp_init_params_t;

/**
 * @}
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief               This function is employed to connect the specified remote device. #BT_A2DP_CONNECT_CNF event will be sent to the upper layer with connection request result.
 * @param[in] dev_addr  is the Bluetooth address of the remote device.
 * @param[in] role      is the device role.
 * @return              If the operation was successful, the returned value is #BT_STATUS_A2DP_OK, otherwise the returned value is less than zero.
 * @sa                  #bt_a2dp_disconnect()
 */
bt_status_t bt_a2dp_connect(const bt_address_t *dev_addr, bt_a2dp_role_t role);

/**
 * @brief               This function is employed to respond the specified remote device incoming connection. #BT_A2DP_CONNECT_CNF event will be sent to the upper layer with connection request result.
 * @param[in] conn_id   is the identifier of the remote device.
 * @param[in] accept    is selection: accept or reject remote device connection.
 * @return              If the operation was successful, the returned value is #BT_STATUS_A2DP_OK, otherwise the returned value is less than zero.
 * @sa                  #bt_a2dp_disconnect()
 */
bt_status_t bt_a2dp_connect_response(uint16_t conn_id, bool accept);

/**
 * @brief               This function is employed to disconnect the specified remote device. #BT_A2DP_DISCONNECT_CNF event will be sent to the upper layer with disconnection request result.
 * @param[in] conn_id   is the identifier of the remote device.
 * @return              If the operation was successful, the returned value is #BT_STATUS_A2DP_OK, otherwise the returned value is less than zero.
 * @sa                  #bt_a2dp_connect(), #bt_a2dp_connect_response()
 */
bt_status_t bt_a2dp_disconnect(uint16_t conn_id);

/**
 * @brief               This function is employed to respond to the incoming streaming from the specified remote device.
 * @param[in] conn_id   is the identifier of the remote device.
 * @param[in] accept    is selection: accept or reject remote device streaming.
 * @return              If the operation was successful, the returned value is #BT_STATUS_A2DP_OK, otherwise the returned value is less than zero.
 * @sa                  #bt_a2dp_suspend_streaming_response()
 */
bt_status_t bt_a2dp_start_streaming_response(uint16_t conn_id, bool accept);

/**
 * @brief               This function is employed to respond to suspended streaming from the specified remote device .
 * @param[in] conn_id   is the identifier of the remote device.
 * @param[in] accept    is selection: accept or reject remote suspended streaming.
 * @return              If the operation was successful, the returned value is #BT_STATUS_A2DP_OK, otherwise the returned value is less than zero.
 * @sa                  #bt_a2dp_start_streaming_response()
 */
bt_status_t bt_a2dp_suspend_streaming_response(uint16_t conn_id, bool accept);

/**
 * @brief               This function is employed to respond to the reconfiguration of the specified remote device.
 * @param[in] conn_id   is the identifier of the remote device.
 * @param[in] accept    is selection: accept or reject remote reconfiguration.
 * @return              If the operation was successful, the returned value is #BT_STATUS_A2DP_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_a2dp_reconfigure_response(uint16_t conn_id, bool accept);

/**
 * @brief               This function is employed to get local role for the specified remote device.
 * @param[in] conn_id   is the identifier of the remote device.
 * @return              Please refer to #bt_a2dp_role_t.
 */
bt_a2dp_role_t bt_a2dp_get_role(uint16_t conn_id);

/**
 * @brief               This function is employed to get local state for the specified remote device.
 * @param[in] conn_id   is the identifier of the remote device.
 * @return              Please refer to #bt_a2dp_state_t.
 */
bt_a2dp_state_t bt_a2dp_get_state(uint16_t conn_id);

/**
 * @brief               This function is employed to get codec capability for the specified remote device.
 * @param[in] conn_id   is the identifier of the remote device.
 * @param[out] codec_cap is detail configuration.
 * @return              If the operation was successful, the returned value is #BT_STATUS_A2DP_OK, otherwise the returned value is less than zero.
 */
bt_status_t bt_a2dp_get_codec_config(uint16_t conn_id, bt_a2dp_codec_capability_t *codec_cap);

/**
 * @brief               This function is the common callback with indication or response result to the upper layer.
 * @param[in] event_id  is the related A2DP event ID.
 * @param[in] param     is the result.
 * @return              None
 */
void bt_a2dp_common_callback(bt_event_t event_id, const void *param);

/**
 * @brief               This function is A2DP initialization API. Implementation is required.
 * @param[out] param    is the initialization parameter. The value is provided once it's called by A2DP.
 * @return              If the returned value is #BT_STATUS_A2DP_OK, A2DP will be enabled or disabled.
 */
bt_status_t bt_a2dp_get_init_params(bt_a2dp_init_params_t *param);

#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
 */

#endif /*__BT_A2DP_H__*/

