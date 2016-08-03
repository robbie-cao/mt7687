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

#ifndef __BT_SYSTEM_H__
#define __BT_SYSTEM_H__

#include "bt_platform.h"
#include "bt_type.h"

BT_EXTERN_C_BEGIN

/**
 * @addtogroup Bluetooth
 * @{
 * @addtogroup Common
 * @{
 * This section introduces the Bluetooth system APIs including details on how to use Bluetooth function groups, enums, structures and functions related to memory configuration and power management.
 * @addtogroup SYS_MEM Memory management
 * @{
 * This section defines the macros and API prototypes related to memory configuration. Apply memory configuration after the system boots up.
 *
 * @section memory_conifguration_usage How to use this module
 *
 * - Call the function #bt_memory_init_packet() to assign buffers for the Bluetooth stack. There are two buffers.
 *   One is the #BT_MEMORY_TX_BUFFER buffer to send commands and application data to the controller with a recommended size from 256 to 512 bytes.
 *   The other is the #BT_MEMORY_RX_BUFFER buffer to receive events and application data from the controller with a recommended size from 512 to 1024 bytes.
 * - Call the function #bt_memory_init_control_block() to assign buffers with fixed block size for the Bluetooth stack. There are two buffers.
 *   One is the #BT_MEMORY_CONTROL_BLOCK_TIMER buffer for the software timer usage in the Bluetooth stack.
 *   The other is the #BT_MEMORY_CONTROL_BLOCK_LE_CONNECTION buffer to store connection information in the stack.
 *
 *  - Sample code:
 *      @code
 *          #define BT_CONNECTION_MAX   5
 *          #define BT_TIMER_NUM 10
 *          #define BT_TX_BUF_SIZE 512
 *          #define BT_RX_BUF_SIZE 1024
 *          #define BT_TIMER_BUF_SIZE (BT_TIMER_NUM * BT_CONTROL_BLOCK_SIZE_OF_TIMER) // For more details, please refer to BT_CONTROL_BLOCK_SIZE_OF_TIMER.
 *          #define BT_CONNECTION_BUF_SIZE (BT_CONNECTION_MAX* BT_CONTROL_BLOCK_SIZE_OF_LE_CONNECTION) // For more details, please refer to BT_CONTROL_BLOCK_SIZE_OF_LE_CONNECTION.
 *          static char timer_cb_buf[BT_TIMER_BUF_SIZE];
 *          static char connection_cb_buf[BT_CONNECTION_BUF_SIZE];
 *          static char tx_buf[BT_TX_BUF_SIZE];
 *          static char rx_buf[BT_RX_BUF_SIZE];
 *          bt_bd_addr_t local_public_addr = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};//Initialize the public_addr according to your requirement based on application.
 *
 *          // Call memory initialization and task creation in the bt_init() only once.
 *          static void bt_init(void)
 *          {
 *               bt_memory_init_packet(BT_MEMORY_TX_BUFFER, tx_buf, BT_TX_BUF_SIZE);
 *               bt_memory_init_packet(BT_MEMORY_RX_BUFFER, rx_buf, BT_RX_BUF_SIZE);
 *               bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_TIMER, timer_cb_buf, BT_TIMER_BUF_SIZE);
 *               bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_LE_CONNECTION, connection_cb_buf,
 *                                 BT_CONNECTION_BUF_SIZE);
 *
 *               //Create the Bluetooth task. The Bluetooth will power on automatically after the task is created.
 *               if (pdPASS != xTaskCreate(bt_task, "bt_task", 2048, (void *)local_public_addr, 5, NULL)) {
 *                   printf("cannot create bt_task.");
 *               }
 *          }
 *      @endcode
 *
 */

/* -------------- Buffer size of a single control block --------------------- */
#define BT_CONTROL_BLOCK_SIZE_OF_TIMER 20         /**< A control block size for one BT_MEMORY_CONTROL_BLOCK_TIMER. */
#define BT_CONTROL_BLOCK_SIZE_OF_LE_CONNECTION 96    /**< A control block size for one BT_MEMORY_CONTROL_BLOCK_LE_CONNECTION. */

/**
 * @brief     Bluetooth memory control block types.
 */
 typedef enum {
    BT_MEMORY_CONTROL_BLOCK_TIMER = 0,         /**< Memory control block for timer.*/
    BT_MEMORY_CONTROL_BLOCK_LE_CONNECTION,     /**< Memory control block for connection.*/
    BT_MEMORY_CONTROL_BLOCK_NUM                /**< Total number of memory control block types.*/
} bt_memory_control_block_t;

/**
 * @defgroup Bluetooth memory buffer types.
 * @{
 */
typedef enum {
    BT_MEMORY_TX_BUFFER = 0,     /**< TX packet buffer, a buffer type for the Memory Management module.*/
    BT_MEMORY_RX_BUFFER          /**< RX packet buffer, a buffer type for the Memory Management module.*/
} bt_memory_packet_t;

#define BT_MEMORY_TX_BUFFERFER_AVAILABLE_IND                     (BT_MODULE_MM | 0x00010000)      /**< TX buffer available indication with #bt_memory_tx_buffer_available_ind_t. */
/**
 * @}
 */

/**
* @defgroup hbif_memory_struct Struct
* @{
*/

/**
*  @brief TX buffer available indication.
*/
typedef struct {
    uint32_t      size; /**< The size of available TX buffer. */
} bt_memory_tx_buffer_available_ind_t;

/**
* @}
*/
/**
 * @brief     This function initializes a buffer for the Bluetooth stack.
 * @param[in] type is #BT_MEMORY_TX_BUFFER or #BT_MEMORY_RX_BUFFER.
 * @param[in] buf  is a pointer to the buffer.
 * @param[in] size is the size of the buffer, in bytes.
 * @return    void.
 */
void  bt_memory_init_packet(bt_memory_packet_t type, char *buf, uint32_t size);
/**
 * @brief     This function initializes a buffer for the Bluetooth stack.
 * @param[in] type is #BT_MEMORY_CONTROL_BLOCK_TIMER or #BT_MEMORY_CONTROL_BLOCK_LE_CONNECTION.
 * @param[in] buf  is a pointer to the buffer.
 * @param[in] size is the size of the buffer, in bytes.
 * @return    void.
 */
void  bt_memory_init_control_block(bt_memory_control_block_t type, char *buf, uint32_t size);

/**
 * @}
 */

/**
 * @addtogroup SYS_POWER Power management
 * @{
 * This section defines the confirmation types and APIs related to switching the power on and off.
 *
 * @section bt_power_management_usage How to use this module
 *
 *   The application layer can power on the Bluetooth when it is powered off and stack memory is assigned.
 *  - Sample code:
 *      @code
 *          // Power on the Bluetooth from the application.
 *          void user_application_power_on_function()
 *          {
 *              //Initialize the public_addr and the random_addr according to your requirement based on application.
 *              bt_bd_addr_t public_addr = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
 *              bt_bd_addr_t random_addr = {0xc0, 0x22, 0x33, 0x44, 0x55, 0xc6};
 *
 *              bt_power_on(&public_addr, &random_addr);
 *          }
 *
 *          // The application receives the power on confirmation event in the function #bt_app_event_callback() after calling #bt_power_on().
 *          bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buf)
 *          {
 *              switch(msg)
 *              {
 *                  case BT_POWER_ON_CNF:
 *                      // Add your flow after the Bluetooth is powered on.
 *                      break;
 *                  case BT_GAP_LE_CONNECT_IND:
 *                      // Connection is established.
 *                      break;
 *                  default:
 *                      break;
 *              }
 *              return status;
 *          }
 *      @endcode
 *
 */

/**
 * @defgroup hbif_power_define Define
 * @{
 */

#define BT_POWER_ON_CNF                     (BT_MODULE_SYSTEM | 0x0001)    /**< Power-on confirmation, with #bt_power_on_cnf_t. */
#define BT_POWER_OFF_CNF                    (BT_MODULE_SYSTEM | 0x0002)    /**< Power-off confirmation, with NULL payload. */

/**
 * @}
 */

/**
 * @defgroup hbif_power_struct Struct
 * @{
 */

/**
 *  @brief Power on confirmation.
 */
typedef struct {
    const bt_bd_addr_t      local_public_addr; /**< The local public address of the device. */
} bt_power_on_cnf_t;

/**
 * @}
 */

/**
 * @brief     Power on the Bluetooth. The application layer receives #BT_POWER_ON_CNF with #bt_power_on_cnf_t after the Bluetooth is powered on.
 * @param[in] public_addr   is a pointer to a given public address(6 bytes).
 *                          If public_addr is NULL, the controller will generate a public address and provide it to the user using #BT_POWER_ON_CNF with #bt_power_on_cnf_t.
 *                          If public_addr is not NULL, the address in the #BT_POWER_ON_CNF is same as public_addr. 
 *                          The given public address should not be [00-00-00-00-00-00].
 * @param[in] random_addr   is a pointer to a given random address(6 bytes).
 * @return    #BT_STATUS_SUCCESS, if the operation completed successfully. Otherwise, the failure status is returned.
 */
bt_status_t bt_power_on(bt_bd_addr_ptr_t public_addr, bt_bd_addr_ptr_t random_addr);

/**
 * @brief     Power off the Bluetooth. The SDK will not send disconnect requests to the existing connections during the power off operation.
 *            The application layer will receive #BT_POWER_OFF_CNF after the Bluetooth is powered off.
 * @return    #BT_STATUS_SUCCESS, if the operation completed successfully. Otherwise, the failure status is returned.
 */
bt_status_t bt_power_off(void);

/**
 * @brief   This function is a static callback for the application to listen to the event. Provide a user-defined callback.
 * @param[in] msg     is the callback message type.
 * @param[in] status  is the status of the callback message.
 * @param[in] buf     is the payload of the callback message.
 * @return            The status of this operation returned from the callback.
 */
bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buf);

/** @}
 *  @}
 *  @}
 */

BT_EXTERN_C_END

#endif

