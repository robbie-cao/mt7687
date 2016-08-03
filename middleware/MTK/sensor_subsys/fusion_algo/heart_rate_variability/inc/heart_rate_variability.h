/**
 * @file    heart_rate_variability.h
 * @brief   ECG/PPG heart rate variability
 * @author  chihming.fu@mediatek.com
 * @version 1.0
 * @date    2016.01.08
 *
 ***************************************************************************
 ****************************************************************************
 */

#ifndef __CMF_ECG_H__
#define __CMF_ECG_H__

#include "stdint.h"
//#include "math_method.h"
#define DEBUG_MODE             (0)  ///< DEBUG_MODE

#if DEBUG_MODE
#include <stdio.h>
#define DEBUG_MEM               1
#else
#define DEBUG_MEM               0
#endif

#define BEAT_DETECTION_IN_ARR    (1)  ///< BEAT_DETECTION_IN_ARR
#define HRV_DETECTION            (1)  ///< HRV_DETECTION
#define MEM_ORDER               (15)  ///< MEM_ORDER
#define EXTEND_BOUNDARY_BEAT     (0)  ///< EXTEND_BOUNDARY_BEAT
#define EXTEND_BEAT_NUM          (3)  ///< EXTEND_BEAT_NUM
#define ECG_FS                 (512)  ///< ECG_FS
#define MEM_SAMPLE_SIZE        (300)  ///< MEM_SAMPLE_SIZE
#define SMALL_SPECTRUM           (1)
#if SMALL_SPECTRUM
#define SPECTRUM_SIZE          (50)  ///< SPECTRUM_SIZE
#else
#define SPECTRUM_SIZE          (500)  ///< SPECTRUM_SIZE
#endif

#define FRAME_SIZE             (512)  ///< FRAME_SIZE
#define BEAT_POS_BUFFER_SIZE   (600)  ///< 120 * 5

#define ECG_SCALE                (1)  ///< ECG_SCALE
#define ECG_BEAT_SCALING     (12583) ///< ECG_BEAT_SCALING
#define ECG_BEAT_SCALING_15  (18874) ///< ECG_BEAT_SCALING
#define MAX_R_AVE_INI        (10000)
#define MAX_INT         (2147483647)  ///< max. value of signed 32-bit integer
#define MIN_INT        (-2147483646)  ///< max. value of signed 32-bit integer
#define RR_SIZE                (300)

/**
 * @addtogroup API
 * @{
 * This document include ECG beat detection APIs.
 * @addtogroup ECG ECG
 * @{
 * This document mainly introduce all details about the ECG beat detection APIs including terms and acronym, how to use this driver and ECG function groups.

 // supported features,software achitechture,how to use this driver,ECG function groups,all enum,structure and functions.
 * @section Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b HRV                        | Heart Rate Variability. For more information, please check <a href="https://en.wikipedia.org/wiki/Heart_rate_variability"> Heart Rate in wiki </a>.|
 *
 * @section Function_Group_Chapter ECG function groups description
 *
 * For easily using,we group the ECG function by its functionality.
 * You can refer to the details of each function by clicking the function name.
 *

 * @section Function_Block_Diagram Control Flow: HRV Estimation
 * Function flow
 * @image html flow.png

 * @section ECG_Usage_Chapter How to use heart_rate_variability.c function
 *  - sample code:
 *    @code
 *#include <stdio.h>
 *#include <stdlib.h>
 *#include "sensor_input_ecg_256.h"
 *#include "heart_rate_variability.h"
 * void example(int state)
 *{
 * int32_t dataLength = DATA_LENGTH;
 * int32_t numBeats;
 * int32_t fs = ECG_FS;
 * int32_t SDNN;
 * int32_t LF;
 * int32_t HF;
 * int32_t LF_HF;
 *
 * HRV_initialization(fs);
 * HRV_preprocessing(sensor_input, dataLength, &numBeats);
 * HRV_estimate(&SDNN, &LF, &HF, &LF_HF);
 *}

 * @endcode

 * - HRV software initialize functions. \n The #HRV_initialization function is a initialization stage.
 *   The functions is:
 *  - #HRV_initialization

 * - HRV software estimate functions. \n The #HRV_estimate function should be called for 300 second data.
 *   The functions is:
 *  - #HRV_estimate

 * - HRV software pre-processing functions. \n The #HRV_preprocessing function should be called for every (duration) 300 second data.
 *   The functions is:
 *  - #HRV_preprocessing
 */

int32_t myround(float x);

/**
 * @brief This function should be called at initialization stage.
 * @param SfreqDSP is the sampling rate of input signal in Hz
 * @sa  #version_info
 * @par Example
 * Sample code please refer to @ref PPG_Usage_Chapter
*/
void HRV_initialization(int32_t SfreqDSP);

/**
 * @brief This function should be called for a 5x60 secs EKG signal.
 * @param[in] *dataIn is a input data buffer.
 * @param[in] dataECGLen is a input data length.
 * @param[out] beatLen is number of beat detected.
 * @param[out] beatPos is the output beat position.
 * @sa  #version_info
 * @par       Example
 * Sample code please refer to @ref ECG_Usage_Chapter
*/
int32_t HRV_preprocessing(int32_t *dataIn, int32_t dataECGLen, int32_t dataIdx, int32_t* beatLen, int32_t* beatPos);


/**
 * @brief This function will calculate the heart rate variability parameters
 * @param[in] *dataIn is a input data buffer.
 * @param[in] dataECGLen is a input data length.
 * @param[out] SDNN is standard deviation of ECG normal to normal beat interval.
 * @param[out] LF is the low frequency component of autonomic nervous system.
 * @param[out] HF is the high frequency component of autonomic nervous system.
 * @param[out] LF_HF is LF/HF which is the balance of autonomic nervous system .
 * @sa  #version_info
 * @par Example
 * Sample code please refer to @ref ECG_Usage_Chapter
*/
void HRV_estimate(int32_t* SDNN,int32_t* LF,int32_t* HF,int32_t* LF_HF);


/**
 * @brief This function remove noise from input signal.
 * @param[in] *dataIn is a input data buffer.
 * @param[out] *dataOut is a output data buffer.
 * @param[out] dataLength is the length of data buffer.
 * @sa  #version_info
 * @par Example
 * Sample code please refer to @ref ECG_Usage_Chapter
*/
void cmf_ECG_remove_noise(int32_t *dataIn, int32_t *dataOut, int32_t dataLength);

/**
 * @brief This function get beat interval of input signal.
 * @param[in] *dataIn is a input data buffer.
 * @param[in] FrameSize is a the data length of input signal.
 * @param[in] shiftI is the shift value compare with orignal signal.
 * @param[out] beatPos is output of beat position.
 * @sa  #version_info
 * @par Example
 * Sample code please refer to @ref ECG_Usage_Chapter
*/
void cmf_ECG_detect_beats(int32_t *dataIn, int32_t FrameSize,  int32_t shiftI, int32_t *beatPos);

/**
 * @brief This function get beat interval of input signal.
 * @param[in] *rr is a input rr interval.
 * @param[in] len is the data length of input signal.
 * @sa  #version_info
 * @par Example
 * Sample code please refer to @ref ECG_Usage_Chapter
*/
void cmf_ECG_get_MEM_coef(int32_t* rr, int32_t len);

#if DEBUG_MODE
void cmf_ECG_get_MEM_coef_int64(int32_t* rr, int32_t len);
void cmf_output_hrv(int32_t SDNN,int32_t LF,int32_t HF,int32_t LF_HF);
void cmf_output_beat_pos(int32_t numBeats, int32_t* beatPos);
void cmf_output_waveform(int32_t len);
void down_sample_ecg(int*data, int len);
#endif

#endif

