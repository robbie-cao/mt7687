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

#ifndef __HAL_PLATFORM_H__
#define __HAL_PLATFORM_H__


#include "hal_define.h"
#include "hal_feature_config.h"
#include "mt2523.h"
#include "hal_weak.h"

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
* module sub features define
* All the subfeatures described below are mandatory for the drivers. No change is recommended.
*****************************************************************************/
#ifdef HAL_ADC_MODULE_ENABLED
#define HAL_ADC_FEATURE_SET_INPUT_AND_DISABLE_PULL
#endif

#ifdef HAL_I2C_MASTER_MODULE_ENABLED
#define HAL_I2C_MASTER_FEATURE_SEND_TO_RECEIVE  /*Enable I2C master send to receive feature. */
#define HAL_I2C_MASTER_FEATURE_EXTENDED_DMA    /* Enable I2C master extend DMA feature.*/

#endif

#ifdef HAL_SPI_MASTER_MODULE_ENABLED
#define HAL_SPI_MASTER_FEATURE_ADVANCED_CONFIG       /*Enable SPI master advanced configuration feature. For more details, please refer to hal_spi_master.h.*/
#define HAL_SPI_MASTER_FEATURE_DEASSERT_CONFIG       /*Enable SPI master deassert configuration feature to deassert the chip select signal after each byte data transfer is complete. For more details, please refer to hal_spi_master.h.*/
#define HAL_SPI_MASTER_FEATURE_MACRO_CONFIG          /*Enable SPI master pad_macro configuration feature to set pad_macro for related SPI pins. For more details, please refer to hal_spi_master.h.*/
#define HAL_SPI_MASTER_FEATURE_CHIP_SELECT_TIMING    /*Enable SPI master chip select timing configuration feature to set timing value for chip select signal. For more details, please refer to hal_spi_master.h.*/
#define HAL_SPI_MASTER_FEATURE_DMA_MODE              /*Enable SPI master DMA mode feature to do data transfer. For more details, please refer to hal_spi_master.h.*/
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
#define HAL_GPIO_FEATURE_INVERSE           /*The input data of pin can be set to the reverse state. For more details, please refer to hal_gpio.h.*/
#define HAL_GPIO_FEATURE_PUPD                  /*The pull state of the pin can be configured with different resister through different combination of GPIO_PUPD_x,GPIO_RESEN0_x and GPIO_RESEN1_x. For more details, please refer to hal_gpio.h.*/
#define HAL_GPIO_FEATURE_CLOCKOUT          /*The pin can be configured to output clock. For more details, please refer to hal_gpio.h.*/
#define HAL_GPIO_FEATURE_HIGH_Z           /*The the pin can be configured to high impedance state to prevent possible electric leakage. For more details, please refer to hal_gpio.h.*/
#endif

#ifdef HAL_EINT_MODULE_ENABLED
#define HAL_EINT_FEATURE_MASK                /* */
#define HAL_EINT_FEATURE_SW_TRIGGER_EINT     /* Support software triggered EINT interrupt. */
#endif

#ifdef HAL_GPC_MODULE_ENABLED
#define HAL_GPC_FEATURE_ADVANCED_CONFIG     /*Supports the interrupt mode.*/
#endif

#ifdef HAL_GPT_MODULE_ENABLED
#define HAL_GPT_FEATURE_US_TIMER               /*Supports the us timer.*/
#define HAL_GPT_SW_GPT_FEATURE                 /*Supports software GPT timer.*/
#define HAL_GPT_PORT_ALLOCATE                  /*This feature introducts port allocation*/
#endif

#ifdef HAL_PWM_MODULE_ENABLED
#define HAL_PWM_FEATURE_ADVANCED_CONFIG        /**/
#endif

#ifdef HAL_RTC_MODULE_ENABLED
#define HAL_RTC_FEATURE_TIME_CALLBACK           /* Supports time change notification callback. */
#endif

#ifdef HAL_SPI_SLAVE_MODULE_ENABLED
#define HAL_SPI_SLAVE_FEATURE_SW_CONTROL        /*Supports SD card detection. If this feature is used, the corresponding EINT pin should be cofingured in the EPT tool.*/
#endif

#ifdef HAL_SLEEP_MANAGER_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SLEEP_MANAGER
 * @{
 * @addtogroup hal_sleep_manager_enum Enum
 * @{
 */
/*****************************************************************************
 * Enum
 *****************************************************************************/
/** @brief Sleep modes */
typedef enum {
    HAL_SLEEP_MODE_NONE = 0,        /**< No sleep. */
    HAL_SLEEP_MODE_IDLE,            /**< Idle state. */
    HAL_SLEEP_MODE_SLEEP,           /**< Sleep state. */
    HAL_SLEEP_MODE_DEEP_SLEEP,      /**< Deep sleep state. */
    HAL_SLEEP_MODE_NUMBER           /**< To support range detection. */
} hal_sleep_mode_t;
/** @brief sleep_manager wake up source */
typedef enum {
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_GPT = 0,                     /**< General purpose timer. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_EINT,                        /**< External interrupt. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_RESERVED,                    /**< Reserved. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_KP,                          /**< Keypad. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_MSDC1,                       /**< SD/eMMC'IP.  */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_MDIF,                        /**< Modem interface. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_RESERVED_2,                  /**< Reserved. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_MSDC2,                       /**< SD/eMMC's second IP. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_SPI_SLAVE,                   /**< SPI protocol slave. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_RGU,                         /**< Reset Generation Unit. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_MM_DBI,                      /**< Multimedia display bus interface. */
    HAL_SLEEP_MANAGER_WAKEUP_SOURCE_MM_DSI,                      /**< Multimedia display serial interface. */
} hal_sleep_manager_wakeup_source_t;
/**
 * @}
 * @}
 * @}
 */
#endif

#ifdef HAL_UART_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup UART
 * @{
 * @addtogroup hal_uart_enum Enum
 * @{
 */
/*****************************************************************************
* uart
*****************************************************************************/
/** @brief UART port index
 * There are total of four UART ports. And only UART0 and UART1 support hardware flow control.
 * | UART port | Hardware Flow Control |
 * |-----------|-----------------------|
 * |  UART0    |           V           |
 * |  UART1    |           V           |
 * |  UART2    |           X           |
 * |  UART3    |           X           |
 */
typedef enum {
    HAL_UART_0 = 0,                            /**< UART port 0. */
    HAL_UART_1 = 1,                            /**< UART port 1. */
    HAL_UART_2 = 2,                            /**< UART port 2. */
    HAL_UART_3 = 3,                            /**< UART port 3. */
    HAL_UART_MAX                               /**< The total number of UART ports (invalid UART port number). */
} hal_uart_port_t;

/**
  * @}
  */

/**
 * @}
 * @}
 */
#endif


#ifdef HAL_I2C_MASTER_MODULE_ENABLED
#ifdef HAL_I2C_MASTER_FEATURE_EXTENDED_DMA
/**
 * @addtogroup HAL
 * @{
 * @addtogroup I2C_MASTER
 * @{
 * @section HAL_I2C_Transaction_Pattern_Chapter Transaction Length And Waveform Pattern of APIs
 *  The Transaction waveform pattern is the waveform pattern sent by the I2C master using SCL and SDA.
 *  Different APIs have different waveform patterns, as shown below.
 * - \b Transaction \b length \b supported \b by \b the \b APIs \n
 *  The total transaction length is determined by 4 parameters: send_packet_length(Ns), send_bytes_in_one_packet(Ms), receive_packet_length(Nr), receive_bytes_in_one_packet(Mr).
 *  And also, these 4 parameters will affect the waveform pattern. The ralationship between waveform pattern and these 4 parameters will be introduced in the next section.
 *  - Total transaction length = Ns * Ms + Nr * Mr.
 *   - Ns is the packet length to be sent by the I2C master.
 *   - Ms is the total number of bytes in a sent packet.
 *   - Nr is the packet length to be received by the I2C master.
 *   - Mr is the total number of bytes in a received packet.
 *  - NA means the related parameter should be ignored.
 *  - 1~8 specifies the parameter range is from 1 to 8. 1~15 specifies the parameter range is from 1 to 15. 1~255 specifies the parameter range from 1 to 255.
 *  - 1 means the parameter value can only be 1.
 *  - Note: Only those functions with the suffix "_ex" have these 4 parameters. Other functions only have the "size" parameter and the driver splits the "size" into these 4 parameters.
 *    #hal_i2c_master_send_polling() for example, the "size" will be divided like this: send_packet_length = 1, send_bytes_in_one_packet = size.
 *          As a result, the total size should be: send_packet_length * send_bytes_in_one_packet = 1 * size = size. The range of "size" should be from 1 to 8.
 * |API                                         |send_packet_length(Ns) | send_bytes_in_one_packet(Ms) | receive_packet_length(Nr) | receive_bytes_in_one_packet(Mr) |
 * |--------------------------------------------|-----------------------|------------------------------|---------------------------|---------------------------------|
 * |hal_i2c_master_send_polling                 |          1            |            1~8               |            NA             |                NA               |
 * |hal_i2c_master_receive_polling              |          NA           |            NA                |            1              |                1~8              |                              
 * |hal_i2c_master_send_to_receive_polling      |          1            |            1~8               |            1              |                1~8              |                              
 * |hal_i2c_master_send_dma                     |          1            |            1~15              |            NA             |                NA               |                              
 * |hal_i2c_master_receive_dma                  |          NA           |            NA                |            1~155          |                1                |                              
 * |hal_i2c_master_send_to_receive_dma          |          1            |            1~15              |            1~254          |                1                |
 * |hal_i2c_master_send_dma_ex                  |          1~255        |            1~15              |            NA             |                NA               |
 * |hal_i2c_master_receive_dma_ex               |          NA           |            NA                |            1~255          |                1~15             |
 * |hal_i2c_master_send_to_receive_dma_ex       |          1            |            1~15              |            1~254          |                1~15             |
 *
 * - \b Waveform \b pattern \b supported \b by \b the \b APIs \n
 *  The 4 parameters(send_packet_length(Ns), send_bytes_in_one_packet(Ms), receive_packet_length(Nr), receive_bytes_in_one_packet(Mr) will also affect the waveform pattern.
 *  The relationship between waveform pattern and these 4 parameters is shown below.
 *  - Ns is the send_packet_length.
 *  - Ms is the send_bytes_in_one_packet.
 *  - Nr is the receive_packet_length.
 *  - Mr is the receive_bytes_in_one_packet.
 * |API                                          |Waveform Pattern                                          |
 * |---------------------------------------------|----------------------------------------------------------|
 * | hal_i2c_master_send_polling                 |  @image html hal_i2c_send_poling_waveform.png            |
 * | hal_i2c_master_receive_polling              |  @image html hal_i2c_receive_poling_waveform.png         |
 * | hal_i2c_master_send_to_receive_polling      |  @image html hal_i2c_send_to_receive_poling_waveform.png |
 * | hal_i2c_master_send_dma                     |  @image html hal_i2c_send_dma_waveform.png            |
 * | hal_i2c_master_receive_dma                  |  @image html hal_i2c_receive_dma_waveform.png         |
 * | hal_i2c_master_send_to_receive_dma          |  @image html hal_i2c_send_to_receive_dma_waveform.png |
 * | hal_i2c_master_send_dma_ex                  |  @image html hal_i2c_send_dma_ex_waveform.png            |
 * | hal_i2c_master_receive_dma_ex               |  @image html hal_i2c_receive_dma_ex_waveform.png         |
 * | hal_i2c_master_send_to_receive_dma_ex       |  @image html hal_i2c_send_to_receive_dma_ex_waveform.png |
 *
 *
 *
 *
 */
#endif

/** @defgroup hal_i2c_master_define Define
 * @{
  */

/** @brief  The maximum polling mode transaction size.
  */
#define HAL_I2C_MAXIMUM_POLLING_TRANSACTION_SIZE  8

/** @brief  The maximum DMA mode transaction size.
  */
#define HAL_I2C_MAXIMUM_DMA_TRANSACTION_SIZE   15

/**
  * @}
  */

/** @addtogroup hal_i2c_master_enum Enum
  * @{
  */

/*****************************************************************************
* I2C master
*****************************************************************************/
/** @brief This enum defines the I2C port.
 *  The platform supports 3 I2C masters. Two of them support polling mode and DMA mode
 *  while the other one only supports polling mode. For more information about the polling mode,
 *  DMA mode, queue mode, please refer to @ref HAL_I2C_Features_Chapter. The details
 *  are shown below:
 *  - I2C masters supported feature table \n
 *    V : means support;  X : means not support.
 * |I2C Master   | Polling mode | DMA mode | Extended DMA mode |
 * |-------------|--------------|----------|-------------------|
 * |I2C0         |      V       |    V     |         V         |
 * |I2C1         |      V       |    V     |         V         |
 * |I2C2         |      V       |    X     |         X         |
 *
 *
*/
typedef enum {
    HAL_I2C_MASTER_0 = 0,                /**< I2C master 0. */
    HAL_I2C_MASTER_1 = 1,                /**< I2C master 1. */
    HAL_I2C_MASTER_2 = 2,                /**< I2C master 2. */
    HAL_I2C_MASTER_MAX                   /**< The total number of I2C masters (invalid I2C Master number). */
} hal_i2c_port_t;

/**
  * @}
  */

/**
 * @}
 * @}
 */
#endif


#ifdef HAL_GPIO_MODULE_ENABLED
/**
* @addtogroup HAL
* @{
* @addtogroup GPIO
* @{
*
* @addtogroup hal_gpio_enum Enum
* @{
*/

/*****************************************************************************
* gpio
*****************************************************************************/
/** @brief This enum defines the GPIO port.
 *  The platform supports a total of 49 GPIO pins with various functionality.
 *
*/

typedef enum {
    HAL_GPIO_0 = 0,                            /**< GPIO pin0 */
    HAL_GPIO_1 = 1,                            /**< GPIO pin1 */
    HAL_GPIO_2 = 2,                            /**< GPIO pin2 */
    HAL_GPIO_3 = 3,                            /**< GPIO pin3 */
    HAL_GPIO_4 = 4,                            /**< GPIO pin4 */
    HAL_GPIO_5 = 5,                            /**< GPIO pin5 */
    HAL_GPIO_6 = 6,                            /**< GPIO pin6 */
    HAL_GPIO_7 = 7,                            /**< GPIO pin7 */
    HAL_GPIO_8 = 8,                            /**< GPIO pin8 */
    HAL_GPIO_9 = 9,                            /**< GPIO pin9 */
    HAL_GPIO_10 = 10,                          /**< GPIO pin10 */
    HAL_GPIO_11 = 11,                          /**< GPIO pin11 */
    HAL_GPIO_12 = 12,                          /**< GPIO pin12 */
    HAL_GPIO_13 = 13,                          /**< GPIO pin13 */
    HAL_GPIO_14 = 14,                          /**< GPIO pin14 */
    HAL_GPIO_15 = 15,                          /**< GPIO pin15 */
    HAL_GPIO_16 = 16,                          /**< GPIO pin16 */
    HAL_GPIO_17 = 17,                          /**< GPIO pin17 */
    HAL_GPIO_18 = 18,                          /**< GPIO pin18 */
    HAL_GPIO_19 = 19,                          /**< GPIO pin19 */
    HAL_GPIO_20 = 20,                          /**< GPIO pin20 */
    HAL_GPIO_21 = 21,                          /**< GPIO pin21 */
    HAL_GPIO_22 = 22,                          /**< GPIO pin22 */
    HAL_GPIO_23 = 23,                          /**< GPIO pin23 */
    HAL_GPIO_24 = 24,                          /**< GPIO pin24 */
    HAL_GPIO_25 = 25,                          /**< GPIO pin25 */
    HAL_GPIO_26 = 26,                          /**< GPIO pin26 */
    HAL_GPIO_27 = 27,                          /**< GPIO pin27 */
    HAL_GPIO_28 = 28,                          /**< GPIO pin28 */
    HAL_GPIO_29 = 29,                          /**< GPIO pin29 */
    HAL_GPIO_30 = 30,                          /**< GPIO pin30 */
    HAL_GPIO_31 = 31,                          /**< GPIO pin31 */
    HAL_GPIO_32 = 32,                          /**< GPIO pin32 */
    HAL_GPIO_33 = 33,                          /**< GPIO pin33 */
    HAL_GPIO_34 = 34,                          /**< GPIO pin34 */
    HAL_GPIO_35 = 35,                          /**< GPIO pin35 */
    HAL_GPIO_36 = 36,                          /**< GPIO pin36 */
    HAL_GPIO_37 = 37,                          /**< GPIO pin37 */
    HAL_GPIO_38 = 38,                          /**< GPIO pin38 */
    HAL_GPIO_39 = 39,                          /**< GPIO pin39 */
    HAL_GPIO_40 = 40,                          /**< GPIO pin40 */
    HAL_GPIO_41 = 41,                          /**< GPIO pin41 */
    HAL_GPIO_42 = 42,                          /**< GPIO pin42 */
    HAL_GPIO_43 = 43,                          /**< GPIO pin43 */
    HAL_GPIO_44 = 44,                          /**< GPIO pin44 */
    HAL_GPIO_45 = 45,                          /**< GPIO pin45 */
    HAL_GPIO_46 = 46,                          /**< GPIO pin46 */
    HAL_GPIO_47 = 47,                          /**< GPIO pin47 */
    HAL_GPIO_48 = 48,                          /**< GPIO pin48 */
    HAL_GPIO_MAX                               /**< The total number of GPIO pins (invalid GPIO pin number). */
} hal_gpio_pin_t;

/**
  * @}
  */

/**
 * @}
 * @}
 */
#endif

#ifdef HAL_ADC_MODULE_ENABLED
/**
* @addtogroup HAL
* @{
* @addtogroup ADC
* @{
*
* @addtogroup hal_adc_enum Enum
* @{
*/

/*****************************************************************************
* adc
*****************************************************************************/
/** @brief adc channel */
typedef enum {
    HAL_ADC_CHANNEL_7 = 7,                        /**< ADC channel 7. */
    HAL_ADC_CHANNEL_8 = 8,                        /**< ADC channel 8. */
    HAL_ADC_CHANNEL_11 = 11,                      /**< ADC channel 11. */
    HAL_ADC_CHANNEL_12 = 12,                      /**< ADC channel 12. */
    HAL_ADC_CHANNEL_13 = 13,                      /**< ADC channel 13. */
    HAL_ADC_CHANNEL_14 = 14,                      /**< ADC channel 14. */
    HAL_ADC_CHANNEL_15 = 15,                      /**< ADC channel 15. */
    HAL_ADC_CHANNEL_MAX                           /**< The total number of ADC channels (invalid ADC channel).*/
} hal_adc_channel_t;

/**
  * @}
  */


/**
 * @}
 * @}
 */
#endif



#ifdef HAL_I2S_MODULE_ENABLED
/**
* @addtogroup HAL
* @{
* @addtogroup I2S
* @{
*
* @addtogroup hal_i2s_enum Enum
* @{
*/

/*****************************************************************************
* i2s
*****************************************************************************/
/** @brief This enum defines initial type of the I2S.
 */

typedef enum {
    HAL_I2S_TYPE_EXTERNAL_MODE          = 0,        /**< External mode. */
    HAL_I2S_TYPE_EXTERNAL_TDM_MODE      = 1,        /**< External TDM mode(invalid). */
    HAL_I2S_TYPE_INTERNAL_MODE          = 2,        /**< Internal mode(invalid). */
    HAL_I2S_TYPE_INTERNAL_LOOPBACK_MODE = 3         /**< Internal loopback mode. */
} hal_i2s_initial_type_t;


/** @brief I2S event */
typedef enum {
    HAL_I2S_EVENT_ERROR               = -1, /**<  An error occurred during the function call. */
    HAL_I2S_EVENT_NONE                =  0, /**<  No error occurred during the function call. */
    HAL_I2S_EVENT_OVERFLOW            =  1, /**<  RX data overflow. */
    HAL_I2S_EVENT_UNDERFLOW           =  2, /**<  TX data underflow. */
    HAL_I2S_EVENT_DATA_REQUEST        =  3, /**<  Request for user-defined data. */
    HAL_I2S_EVENT_DATA_NOTIFICATION   =  4  /**<  Notify user the RX data is ready. */
} hal_i2s_event_t;


/** @brief I2S sampling rates */
typedef enum {
    HAL_I2S_SAMPLE_RATE_8K        = 0,  /**<  8000Hz  */
    HAL_I2S_SAMPLE_RATE_11_025K   = 1,  /**<  11025Hz */
    HAL_I2S_SAMPLE_RATE_12K       = 2,  /**<  12000Hz */
    HAL_I2S_SAMPLE_RATE_16K       = 3,  /**<  16000Hz */
    HAL_I2S_SAMPLE_RATE_22_05K    = 4,  /**<  22050Hz */
    HAL_I2S_SAMPLE_RATE_24K       = 5,  /**<  24000Hz */
    HAL_I2S_SAMPLE_RATE_32K       = 6,  /**<  32000Hz */
    HAL_I2S_SAMPLE_RATE_44_1K     = 7,  /**<  44100Hz */
    HAL_I2S_SAMPLE_RATE_48K       = 8,  /**<  48000Hz */
} hal_i2s_sample_rate_t;

/**
  * @}
  */

/**
 * @}
 * @}
 */
#endif


#ifdef HAL_SPI_MASTER_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SPI_MASTER
 * @{
 * @defgroup hal_spi_master_define Define
 * @{
 */

/** @brief  The maximum polling mode transaction size.
 */
#define HAL_SPI_MAXIMUM_POLLING_TRANSACTION_SIZE  32

/**
 * @}
 */

/**
 * @addtogroup hal_spi_master_enum Enum
 * @{
 */

/*****************************************************************************
* SPI master
*****************************************************************************/
/** @brief This enum defines the SPI master port.
 *  The chip supports total of 4 SPI master ports, each of them supports polling mode
 *  and DMA mode. About polling mode, and DMA mode, please refer to @ref
 *  HAL_SPI_MASTER_Features_Chapter.
 */
typedef enum {
    HAL_SPI_MASTER_0 = 0,                              /**< SPI master port 0. */
    HAL_SPI_MASTER_1 = 1,                              /**< SPI master port 1. */
    HAL_SPI_MASTER_2 = 2,                              /**< SPI master port 2. */
    HAL_SPI_MASTER_3 = 3,                              /**< SPI master port 3. */
    HAL_SPI_MASTER_MAX                                 /**< The total number of SPI master ports (invalid SPI master port). */
} hal_spi_master_port_t;


/** @brief This enum defines the options to connect the SPI slave device to the SPI master's CS pins. */
typedef enum {
    HAL_SPI_MASTER_SLAVE_0 = 0,                       /**< The SPI slave device is connected to the SPI master's CS0 pin. */
    HAL_SPI_MASTER_SLAVE_1 = 1,                       /**< The SPI slave device is connected to the SPI master's CS1 pin. */
    HAL_SPI_MASTER_SLAVE_MAX                          /**< The total number of SPI master CS pins (invalid SPI master CS pin). */
} hal_spi_master_slave_port_t;

/**
 * @}
 */

/**
 * @}
 * @}
 */
#endif

#ifdef HAL_SPI_SLAVE_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SPI_SLAVE
 * @{
 * @addtogroup hal_spi_slave_enum Enum
 * @{
 */

/*****************************************************************************
* SPI slave
*****************************************************************************/
/** @brief This enum defines the SPI slave port. This chip supports only one
 *  SPI slave port.
 */
typedef enum {
    HAL_SPI_SLAVE_0 = 0,                             /**< SPI slave port 0. */
    HAL_SPI_SLAVE_MAX                                /**< The total number of SPI slave ports (invalid SPI slave port number). */
} hal_spi_slave_port_t;

/**
 * @}
 */

/**
 * @}
 * @}
 */
#endif


#ifdef HAL_RTC_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup RTC
 * @{
 */

/*****************************************************************************
* rtc
*****************************************************************************/
/* NULL */

/**
 * @}
 * @}
 */
#endif


#ifdef HAL_EINT_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup EINT
 * @{
 * @addtogroup hal_eint_enum Enum
 * @{
 */

/*****************************************************************************
* eint
*****************************************************************************/
/** @brief EINT pins. */
typedef enum {
    HAL_EINT_NUMBER_0 = 0,
    HAL_EINT_NUMBER_1 = 1,
    HAL_EINT_NUMBER_2 = 2,
    HAL_EINT_NUMBER_3 = 3,
    HAL_EINT_NUMBER_4 = 4,
    HAL_EINT_NUMBER_5 = 5,
    HAL_EINT_NUMBER_6 = 6,
    HAL_EINT_NUMBER_7 = 7,
    HAL_EINT_NUMBER_8 = 8,
    HAL_EINT_NUMBER_9 = 9,
    HAL_EINT_NUMBER_10 = 10,
    HAL_EINT_NUMBER_11 = 11,
    HAL_EINT_NUMBER_12 = 12,
    HAL_EINT_NUMBER_13 = 13,
    HAL_EINT_NUMBER_14 = 14,
    HAL_EINT_NUMBER_15 = 15,
    HAL_EINT_NUMBER_16 = 16,
    HAL_EINT_NUMBER_17 = 17,
    HAL_EINT_NUMBER_18 = 18,
    HAL_EINT_NUMBER_19 = 19,
    HAL_EINT_KEYPAD    = 20,      /**< EINT number 20:  Keypad. */
    HAL_EINT_UART_0_RX = 21,      /**< EINT number 21:  UART0 RX. */
    HAL_EINT_UART_1_RX = 22,      /**< EINT number 22:  UART1 RX. */
    HAL_EINT_UART_2_RX = 23,      /**< EINT number 23:  UART2 RX. */
    HAL_EINT_UART_3_RX = 24,      /**< EINT number 24:  BTSYS. */
    HAL_EINT_BTSYS     = 25,      /**< EINT number 25:  BTSYS BTIF. */
    HAL_EINT_BTSYS_BTIF = 26,     /**< EINT number 26:  UART3 RX. */
    HAL_EINT_USB       = 27,      /**< EINT number 27:  USB. */
    HAL_EINT_ACCDET    = 28,      /**< EINT number 28:  ACCDET. */
    HAL_EINT_RTC       = 29,      /**< EINT number 29:  RTC. */
    HAL_EINT_PMIC      = 30,      /**< EINT number 30:  PMIC. */
    HAL_EINT_GPCOUNTER = 31,      /**< EINT number 31:  GPCOUNTER. */
    HAL_EINT_NUMBER_MAX           /**< The total number of EINT channels (invalid EINT channel). */
} hal_eint_number_t;

/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif

#ifdef HAL_GPT_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPT
 * @{
 * @addtogroup hal_gpt_enum Enum
 * @{
 */

/*****************************************************************************
* GPT
*****************************************************************************/
/** @brief GPT port */
typedef enum {
    HAL_GPT_0 = 0,                          /**< GPT port 0. */
    HAL_GPT_1 = 1,                          /**< GPT port 1. */
    HAL_GPT_2 = 2,                          /**< GPT port 2. */
    HAL_GPT_3 = 3,                          /**< GPT port 3. */
    HAL_GPT_4 = 4,                          /**< GPT port 4. */
    HAL_GPT_5 = 5,                          /**< GPT port 5. */
    HAL_GPT_MAX_PORT                        /**< The total number of GPT ports (invalid GPT port). */
} hal_gpt_port_t;

/** @brief GPT clock source */
typedef enum {
    HAL_GPT_CLOCK_SOURCE_32K = 0,            /**< Set the GPT clock source to 32kHz. */
    HAL_GPT_CLOCK_SOURCE_1M  = 1             /**< Set the GPT clock source to 1MHz. */
} hal_gpt_clock_source_t;


/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif

#ifdef HAL_FLASH_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup FLASH
 * @{
 */

/*****************************************************************************
* Flash
*****************************************************************************/
/* NULL */

/**
 * @}
 * @}
 */
#endif

#ifdef HAL_GDMA_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GDMA
 * @{
 * @addtogroup hal_gdma_enum Enum
 * @{
 */
/*****************************************************************************
* GDMA
*****************************************************************************/
/** @brief gdma channel */
typedef enum {
    HAL_GDMA_CHANNEL_0 = 0,                        /**< GDMA channel 0. */
    HAL_GDMA_CHANNEL_MAX                           /**< The total number of GDMA channels (invalid GDMA channel). */
} hal_gdma_channel_t;

/**
  * @}
  */


/**
 * @}
 * @}
 */
#endif

#ifdef HAL_PWM_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup PWM
 * @{
 * @addtogroup hal_pwm_enum Enum
 * @{
 */
/*****************************************************************************
* PWM
*****************************************************************************/
/** @brief The PWM channels */
typedef enum {
    HAL_PWM_0 = 0,                            /**< PWM channel 0. */
    HAL_PWM_1 = 1,                            /**< PWM channel 1. */
    HAL_PWM_2 = 2,                            /**< PWM channel 2. */
    HAL_PWM_3 = 3,                            /**< PWM channel 3. */
    HAL_PWM_4 = 4,                            /**< PWM channel 4. */
    HAL_PWM_5 = 5,                            /**< PWM channel 5. */
    HAL_PWM_MAX_CHANNEL                     /**< The total number of PWM channels (invalid PWM channel).*/
} hal_pwm_channel_t;


/** @brief PWM clock source options */
typedef enum {
    HAL_PWM_CLOCK_13MHZ = 0,                /**< PWM clock source 13MHz. */
    HAL_PWM_CLOCK_32KHZ = 1,                /**< PWM clock srouce 32kHz. */
} hal_pwm_source_clock_t ;

/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif

#ifdef HAL_WDT_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup WDT
 * @{
 */

/*****************************************************************************
* WDT
*****************************************************************************/
/* NULL */

/**
 * @}
 * @}
 */
#endif

#ifdef HAL_CACHE_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup CACHE
 * @{
 */

/*****************************************************************************
* Cache
*****************************************************************************/
/* NULL */

/**
 * @}
 * @}
 */
#endif

#ifdef HAL_GPC_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup GPC
 * @{
 * @addtogroup hal_gpc_enum Enum
 * @{
 */
/** @brief GPC port */
typedef enum {
    HAL_GPC_0 = 0,                          /**< GPC port 0. */
    HAL_GPC_MAX_PORT                        /**< The total number of GPC ports (invalid GPC port). */
} hal_gpc_port_t;


/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif


#ifdef HAL_SD_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SD
 * @{
 * @addtogroup hal_sd_enum Enum
 * @{
 */
/*****************************************************************************
* SD
*****************************************************************************/
/** @brief  This enum defines the SD/eMMC port. */
typedef enum {
    HAL_SD_PORT_0 = 0,                                             /**<  SD/eMMC port 0. */
    HAL_SD_PORT_1 = 1                                              /**<  SD/eMMC port 1. */
} hal_sd_port_t;

/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif


#ifdef HAL_SDIO_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup SDIO
 * @{
 * @addtogroup hal_sdio_enum Enum
 * @{
 */
/*****************************************************************************
* SDIO
*****************************************************************************/
/** @brief  This enum defines the SDIO port.  */
typedef enum {
    HAL_SDIO_PORT_0 = 0,                                             /**< SDIO port 0. */
    HAL_SDIO_PORT_1 = 1                                              /**< SDIO port 1. */
} hal_sdio_port_t;

/**
  * @}
  */
/**
 * @}
 * @}
 */
#endif

#ifdef HAL_CLOCK_MODULE_ENABLED
/**
 * @addtogroup HAL
 * @{
 * @addtogroup CLOCK
 * @{
 */
#include "hal_pmu_internal.h"

/*****************************************************************************
* Clock
*****************************************************************************/
/** @addtogroup clock
 * @{
 */
/** @brief Use clock_id in CLOCK HAL API. */
/*************************************************************************
 * Define clock gating registers and bit structure.
 * Note: Mandatory, modify clk_cg_mask in hal_clock.c source file, if hal_clock_cg_id has changed.
 *************************************************************************/
typedef enum {
    /* NR_PDN_COND0 = 7 */
    HAL_CLOCK_CG_BT_26M          =  9,        /* bit 9,   PDN_COND0_FROM */
    HAL_CLOCK_CG_CAMINF48M       = 10,        /* bit 10,  */
    HAL_CLOCK_CG_BT_BUSCK        = 13,        /* bit 13,  */
    HAL_CLOCK_CG_BT_N9           = 15,        /* bit 15,  */
    HAL_CLOCK_CG_BUS             = 20,        /* bit 20,  */
    HAL_CLOCK_CG_CM              = 21,        /* bit 21,  */
    HAL_CLOCK_CG_BSI             = 23,        /* bit 23,  PDN_COND0_TO */

    /* NR_PDN_COND1 = 17 */
    HAL_CLOCK_CG_SEJ             =  0 + 32,   /* bit 0,   PDN_COND1_FROM */
    HAL_CLOCK_CG_DMA             =  1 + 32,   /* bit 1,   */
    HAL_CLOCK_CG_USB48M          =  2 + 32,   /* bit 2,   */
    HAL_CLOCK_CG_MSDC0           =  3 + 32,   /* bit 3,   */
    HAL_CLOCK_CG_I2C_D2D         =  6 + 32,   /* bit 6,   */
    HAL_CLOCK_CG_I2C2            =  7 + 32,   /* bit 7,   */
    HAL_CLOCK_CG_CM4_OSTIMER     = 10 + 32,   /* bit 10,  */
    HAL_CLOCK_CG_DMA_AO          = 12 + 32,   /* bit 12,  */
    HAL_CLOCK_CG_UART0           = 13 + 32,   /* bit 13,  */
    HAL_CLOCK_CG_UART1           = 14 + 32,   /* bit 14,  */
    HAL_CLOCK_CG_UART2           = 15 + 32,   /* bit 15,  */
    HAL_CLOCK_CG_UART3           = 16 + 32,   /* bit 16,  */
    HAL_CLOCK_CG_SPISLV          = 19 + 32,   /* bit 19,  */
    HAL_CLOCK_CG_SPI0            = 20 + 32,   /* bit 20,  */
    HAL_CLOCK_CG_SPI1            = 21 + 32,   /* bit 21,  */
    HAL_CLOCK_CG_SPI2            = 22 + 32,   /* bit 22,  */
    HAL_CLOCK_CG_SPI3            = 23 + 32,   /* bit 23,  PDN_COND1_TO */

    /* NR_PDN_COND2 = 18 */
    HAL_CLOCK_CG_PWM0            =  0 + 64,   /* bit 0,   PDN_COND2_FROM */
    HAL_CLOCK_CG_BTIF            =  1 + 64,   /* bit 1,   */
    HAL_CLOCK_CG_GPTIMER         =  2 + 64,   /* bit 2,   */
    HAL_CLOCK_CG_GPCOUNTER       =  3 + 64,   /* bit 3,   */
    HAL_CLOCK_CG_PWM1            =  4 + 64,   /* bit 4,   */
    HAL_CLOCK_CG_EFUSE           =  6 + 64,   /* bit 6,   */
    HAL_CLOCK_CG_LPM             =  7 + 64,   /* bit 7,   */
    HAL_CLOCK_CG_CM_SYSROM       =  8 + 64,   /* bit 8,   */
    HAL_CLOCK_CG_SFC             =  9 + 64,   /* bit 9,   */
    HAL_CLOCK_CG_MSDC1           = 10 + 64,   /* bit 10,  */
    HAL_CLOCK_CG_USB_DMA         = 12 + 64,   /* bit 12,  */
    HAL_CLOCK_CG_USB_BUS         = 13 + 64,   /* bit 13,  */
    HAL_CLOCK_CG_DISP_PWM        = 14 + 64,   /* bit 14,  */
    HAL_CLOCK_CG_TRNG            = 16 + 64,   /* bit 16,  */
    HAL_CLOCK_CG_PWM2            = 19 + 64,   /* bit 19,  */
    HAL_CLOCK_CG_PWM3            = 20 + 64,   /* bit 20,  */
    HAL_CLOCK_CG_PWM4            = 21 + 64,   /* bit 21,  */
    HAL_CLOCK_CG_PWM5            = 22 + 64,   /* bit 22,  PDN_COND2_TO */

    /* NR_CM_PDN_COND0 = 10 */
    HAL_CLOCK_CG_LCD             =  0 + 96,   /* bit 0,   CM_PDN_COND0_FROM */
    HAL_CLOCK_CG_RESEIZER        =  1 + 96,   /* bit 1,   */
    HAL_CLOCK_CG_ROTDMA          =  2 + 96,   /* bit 2,   */
    HAL_CLOCK_CG_CAM_BCLK        =  3 + 96,   /* bit 3,   */
    HAL_CLOCK_CG_PAD2CAM         =  4 + 96,   /* bit 4,   */
    HAL_CLOCK_CG_G2D             =  5 + 96,   /* bit 5,   */
    HAL_CLOCK_CG_MM_COLOR        =  6 + 96,   /* bit 6,   */
    HAL_CLOCK_CG_AAL             =  7 + 96,   /* bit 7,   */
    HAL_CLOCK_CG_DSI0            =  8 + 96,   /* bit 8,   */
    HAL_CLOCK_CG_LCD_APB         =  9 + 96,   /* bit 9,   CM_PDN_COND0_TO */

    /* NR_ACFG_CLK_RG = 5 */
    HAL_CLOCK_CG_AUXADC          =  2 + 128,  /* bit 2,   ACFG_CLK_RG_FROM */
    HAL_CLOCK_CG_GPDAC           =  6 + 128,  /* bit 6,   */
    HAL_CLOCK_CG_SENSOR_DMA      =  8 + 128,  /* bit 8,   */
    HAL_CLOCK_CG_I2C1            =  9 + 128,  /* bit 9,   */
    HAL_CLOCK_CG_I2C0            = 10 + 128,  /* bit 10,  ACFG_CLK_RG_TO */
} hal_clock_cg_id;

/**
  * @}
  */

/**
 * @}
 * @}
 */
#endif

#ifdef __cplusplus
}
#endif

#endif /* __HAL_PLATFORM_H__ */

