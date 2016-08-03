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

// For Register AT command handler
// System head file
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"
#include "vsm_driver.h"
#include "syslog.h"
#include "hal_gpio.h"
#include "kiss_fft.h"
//#define GOLDEN_TEST
#ifdef GOLDEN_TEST
#include "at_command_clover.h"
#endif

log_create_module(atci_clover, PRINT_LEVEL_INFO);
#if 1
#define LOGE(fmt,arg...)   LOG_E(atci_clover, "[clover]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atci_clover, "[clover]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atci_clover ,"[clover]"fmt,##arg)
#else
#define LOGE(fmt,arg...)   printf("[atci_clover]"fmt,##arg)
#define LOGW(fmt,arg...)   printf("[atci_clover]"fmt,##arg)
#define LOGI(fmt,arg...)   printf("[atci_clover]"fmt,##arg)
#endif

#define EKG_SAMPLE      (16384)
#define PPG_SAMPLE      (4096)
#define EKG_VPP         (4)       //unit: bits
#define EKG_BITS        (23)      //unit: bits
//#define EKG_ADC_LSB     (1/(1<<21))  //4/2^23
#define EKG_ADC_LSB     (2097152)  //4/2^23
#define EKG_GAIN        (6)

#define PPG_VPP         (3.2f)   //unit: volt
#define PPG_BITS        (16)    //unit: bits
#define PPG_ADC_LSB     (1<<PPG_BITS)//(PPG_VPP/(1<<PPG_BITS))
#define PPG_TIA_GAIN    (1)
#define PPG_PGA_GAIN    (1)

#define EKG_SFREQ       (512)
#define PPG_SFREQ       (124.89f)
#define EKG_BW          (150)
#define PPG_BW          (4)
#define EKG_SUM_START   (0)
#define EKG_SUM_END     (4800) //Nfout=floor(sfout/sfreq*NFFT);
#define PPG_SUM_START   (0)
#define PPG_SUM_END     (131)  //Nfout=floor(sfout/sfreq*NFFT);

#define PPG_SAMPLE_RATE     (125)
#define LED_CUR_TEST_LEN    (5000) // 125(sample rate)*5(sec)*8(led)    (led and ambient)
#define AMBDAC_CUR_TEST_LEN (30000) // 125(sample rate)*5(sec)*8(led) * 6(ambient)
#define TIA_GAIN_100K       (1)
#define TIA_GAIN_500K       (2)
#define LED_CUR_TEST        (1)
#define LED_AMBDAC_CUR_TEST (2)

typedef struct
{
	uint16_t addr;
	uint32_t value;
} signal_data_t;


// test data
static signal_data_t test_data[] = {
    {0x2330,0x1fff007f},
    {0x2334,0xff00ff},
    {0x3368,0x4b6270a},
};

extern void ms_delay(uint32_t ms);

uint32_t read_data[VSM_SRAM_LEN];
uint32_t raw_data[EKG_SAMPLE];
kiss_fft_cpx fft_data_in[EKG_SAMPLE];
static kiss_fft_cfg cfg;
kiss_fft_cpx fft_data_out[EKG_SAMPLE];
float noise[EKG_SAMPLE];
float led_data[AMBDAC_CUR_TEST_LEN];


/*
 * sample code
*/

/*--- Function ---*/
extern void ms_delay(uint32_t ms);
extern int32_t vsm_driver_chip_version_get(void);
atci_status_t atci_cmd_hdlr_clover(atci_parse_cmd_param_t *parse_cmd);

void mean(kiss_fft_cpx *input, double *output, int32_t len)
{
    int i = 0;
    double sum = 0;
    if(!input || !output)
        return;
    for(i = 0; i < len; i++){
        sum += input->r;
        input++;
    }
    LOGI("mean():sum/len %.12f \r\n", sum/len);
    *output =(float) sum/len;
}

void atci_clover_task_dump(void *pvParameters)
{
    int i = 0, data_len = 0, type;
    atci_response_t response = {{0}};
    type = *(int *)pvParameters;
    LOGI("%s():type %d\r\n", __func__, type);
    if (type == 11) {
        data_len = LED_CUR_TEST_LEN;
    } else if (type == 12) {
        data_len = AMBDAC_CUR_TEST_LEN;
    }

    for (i = 0;i < data_len;i ++) {
        //catch led data
        snprintf((char *)response.response_buf, sizeof(response.response_buf), "led_data[%d] %.12f\r\n", i, (double)led_data[i]);
        response.response_len = strlen((char *)response.response_buf);
        atci_send_response(&response);
        ms_delay(15);
    }

    vTaskDelete(NULL);
}

/**
  * @ recording PPG data while switching led current
  * @ test_type: select tia gain, 1: led current test, 2: led current test on different ambdac
  * @ tia_gain:  select tia gain, 1: set to 100K, 2: set to 500K.
  */
int32_t current_test(int32_t test_type, int32_t tia_gain)
{
    int32_t data_count = 0, i = 0, output_len = 0;
    int32_t led_count = 0;
    uint32_t ambdac_count = 1;
    int32_t err = -1;
    int32_t data_len = 0;

    if (test_type == LED_CUR_TEST) {
        data_len = LED_CUR_TEST_LEN; /* collecting 125 * 5 sec * 8 led current round data */
    } else if (test_type == LED_AMBDAC_CUR_TEST) {
        data_len = AMBDAC_CUR_TEST_LEN; /* collecting 125 * 5 sec * 8 led current round data under 6 ambdac phase */
    }

#ifndef DEVICE_HDK
    /* initialize vsm */
    vsm_driver_init();

    vsm_driver_set_signal(VSM_SIGNAL_PPG1);
#endif
    vsm_driver_disable_signal(VSM_SIGNAL_PPG1);

    /* step 1 set tia_gain
     * 1: set to 100k
     * 2: set to 500k
     */
    if (tia_gain == 1) {
        vsm_driver_set_tia_gain(VSM_SIGNAL_PPG1, VSM_TIA_GAIN_100_K);
        vsm_driver_set_tia_gain(VSM_SIGNAL_PPG2, VSM_TIA_GAIN_100_K);
    } else if (tia_gain == 2) {
        vsm_driver_set_tia_gain(VSM_SIGNAL_PPG1, VSM_TIA_GAIN_500_K);
        vsm_driver_set_tia_gain(VSM_SIGNAL_PPG2, VSM_TIA_GAIN_500_K);
    } else {
        return err;
    }
    err = 0;
    /* step 2: pga gain default: 6*/
    vsm_driver_set_pga_gain(VSM_PGA_GAIN_6);

    /* step 3: ambdac 1/2 set default 0*/
    if (vsm_driver_chip_version_get() == 1) {
        vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_00_MA);
    } else if (vsm_driver_chip_version_get() == 2) {
        vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_01_MA);
        vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_01_MA);
    }

    /* step 4: start from LED1/LED2 = 0/0 */
    vsm_driver_set_led_current(VSM_LED_1, led_count);
    vsm_driver_set_led_current(VSM_LED_2, led_count);

    vsm_driver_enable_signal(VSM_SIGNAL_PPG1);
    err = vsm_driver_read_sram(VSM_SRAM_PPG1, (uint32_t *)read_data, &output_len);

    while (data_count < data_len) {
        err = vsm_driver_read_sram(VSM_SRAM_PPG1, (uint32_t *)read_data, &output_len);
        for (i = 0; i < output_len; i ++) {
            if(read_data[i] > 4194304) {
                read_data[i] -= 8388608;
            }

            if (test_type == LED_CUR_TEST) {
                //keep led current setting and add ambdac current 1
                //if (i % 2 == 0 && read_data[i] > 30240) {
                if (i % 2 == 0 && (int)(read_data[i]) > 29000) {
                    ambdac_count++;
                    LOGI("%s():adjust ambdac data_count %d,ambdac_count %d \r\n", __func__, data_count, ambdac_count);
                    if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_01_MA) {
                        if (vsm_driver_chip_version_get() == 1) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_01_MA);
                        } else if (vsm_driver_chip_version_get() == 2) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_01_MA);
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_01_MA);
                        }
                    } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_02_MA) {
                        if (vsm_driver_chip_version_get() == 1) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_02_MA);
                        } else if (vsm_driver_chip_version_get() == 2) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_02_MA);
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_02_MA);
                        }
                    } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_03_MA) {
                        if (vsm_driver_chip_version_get() == 1) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_03_MA);
                        } else if (vsm_driver_chip_version_get() == 2) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_03_MA);
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_03_MA);

                        }
                    } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_04_MA) {
                        if (vsm_driver_chip_version_get() == 1) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_04_MA);
                        } else if (vsm_driver_chip_version_get() == 2) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_04_MA);
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_04_MA);

                        }
                    } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_05_MA) {
                        if (vsm_driver_chip_version_get() == 1) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_05_MA);
                        } else if (vsm_driver_chip_version_get() == 2) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_05_MA);
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_05_MA);

                        }
                    } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_06_MA) {
                        if (vsm_driver_chip_version_get() == 1) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_06_MA);
                        } else if (vsm_driver_chip_version_get() == 2) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_06_MA);
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_06_MA);
                        }
                    } 

                    if (vsm_driver_chip_version_get() == 1) {
                        if (VSM_AMBDAC_CURR_00_MA + ambdac_count > VSM_AMBDAC_CURR_06_MA) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_06_MA);
                        }
                    } else if (vsm_driver_chip_version_get() == 2) {
                        if (VSM_AMBDAC_CURR_01_MA + ambdac_count > VSM_AMBDAC_CURR_06_MA) {
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_06_MA);
                            vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_06_MA);
                        } 
                    }
                }

                /* collectiing led data*/
                if (i % 2 == 0) {
                    led_data[data_count] = (float)(((float)(int)read_data[i]) * 1000 * PPG_VPP/ (PPG_TIA_GAIN * PPG_PGA_GAIN) / PPG_ADC_LSB);
                    /* collectiing engough data then break */
                    if (data_count++ == LED_CUR_TEST_LEN) {
                        break;
                    }
                    /* adjust current while end of run*/
                    if (data_count != 0 && (data_count % (PPG_SAMPLE_RATE * 5)) == 0 ) {
                        /* 125(sample_rate) * 5 sec * 2 (led/ambient) */
                        /* switch led current setting once collecting 5 sec data */
                        led_count += 0x20;
                        LOGI("%s():adjust led data_count %d, led_count 0x%x \r\n", __func__, data_count, led_count);
                        vsm_driver_set_led_current(VSM_LED_1,led_count);
                        vsm_driver_set_led_current(VSM_LED_2,led_count);
                        break;
                    }
                }
            } else if (test_type == LED_AMBDAC_CUR_TEST) {
                /* collectiing led data*/
                if (i % 2 == 0) {
                    led_data[data_count] = (float)(((float)(int)read_data[i]) * 1000 * PPG_VPP/ (PPG_TIA_GAIN * PPG_PGA_GAIN) / PPG_ADC_LSB);
                    /* collectiing engough data then break */
                    if (data_count++ == LED_AMBDAC_CUR_TEST) {
                        break;
                    }
                    /* adjust current while end of run*/
                    if (data_count != 0 && (data_count % (PPG_SAMPLE_RATE * 5)) == 0 ) {
                        /* 125(sample_rate) * 5 sec * 2 (led/ambient) */
                        /* switch led current setting once collecting 5 sec data */
                        led_count += 0x20;
                        if (led_count <= 0xe0) {
                            LOGI("%s():adjust led data_count %d, led_count 0x%x \r\n", __func__, data_count, led_count);
                            vsm_driver_set_led_current(VSM_LED_1,led_count);
                            vsm_driver_set_led_current(VSM_LED_2,led_count);
                        } else {
                            /* every led current phase collecting done, change to next ambdac */
                            LOGI("data_count %ld, ambdac level %lu \r\n", data_count, ambdac_count);
                            led_count = 0;
                            ambdac_count++;
                            vsm_driver_set_led_current(VSM_LED_1,led_count);
                            vsm_driver_set_led_current(VSM_LED_2,led_count);
                            if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_01_MA) {
                                if (vsm_driver_chip_version_get() == 1) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_01_MA);
                                } else if (vsm_driver_chip_version_get() == 2) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_01_MA);
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_01_MA);
                                }
                            } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_02_MA) {
                                if (vsm_driver_chip_version_get() == 1) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_02_MA);
                                } else if (vsm_driver_chip_version_get() == 2) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_02_MA);
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_02_MA);
                                }
                            } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_03_MA) {
                                if (vsm_driver_chip_version_get() == 1) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_03_MA);
                                } else if (vsm_driver_chip_version_get() == 2) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_03_MA);
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_03_MA);

                                }
                            } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_04_MA) {
                                if (vsm_driver_chip_version_get() == 1) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_04_MA);
                                } else if (vsm_driver_chip_version_get() == 2) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_04_MA);
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_04_MA);

                                }
                            } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_05_MA) {
                                if (vsm_driver_chip_version_get() == 1) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_05_MA);
                                } else if (vsm_driver_chip_version_get() == 2) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_05_MA);
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_05_MA);

                                }
                            } else if ((VSM_AMBDAC_CURR_00_MA + ambdac_count) == VSM_AMBDAC_CURR_06_MA) {
                                if (vsm_driver_chip_version_get() == 1) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_06_MA);
                                } else if (vsm_driver_chip_version_get() == 2) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_06_MA);
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_06_MA);
                                }
                            } 

                            if (vsm_driver_chip_version_get() == 1) {
                                if (VSM_AMBDAC_CURR_00_MA + ambdac_count > VSM_AMBDAC_CURR_06_MA) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_06_MA);
                                }
                            } else if (vsm_driver_chip_version_get() == 2) {
                                if (VSM_AMBDAC_CURR_01_MA + ambdac_count > VSM_AMBDAC_CURR_06_MA) {
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_1, VSM_AMBDAC_CURR_06_MA);
                                    vsm_driver_set_ambdac_current( VSM_AMBDAC_2, VSM_AMBDAC_CURR_06_MA);
                                } 
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
#if 0
    for (i = 0;i < LED_CUR_TEST_LEN;i ++) {
        if (i % 2 == 0) {
            LOGI("odd_data[%d] %d\r\n", i, raw_data[i]);
        } else {
            LOGI("even_data[%d] %d\r\n", i, raw_data[i]);
        }
    }
#endif
    return err;
}
/**
  * @ measure signal noise
  * @ signal: signal type.
  * @ signal_noise: pointer for the signal noise reuslt.
  * @ is_odd: measure ppg/led if true, else measue ppg/ambient.
  * @ is_amplitude: measure amplitude error if true, else measue signal noise.
  */
int32_t signal_noise_compute(vsm_signal_t signal, double *signal_noise, bool is_odd, bool is_amplitude)
{
    uint32_t i = 0, data_count = 0;
    int sum = 0, output_len;
    int sample_count = 0, sum_start = 0, sum_end = 0, sram_type = 0;;
    double mean_data = 0, signal_result = 0;

    if (!signal_noise) {
        return -1;
    }

    if(signal == VSM_SIGNAL_EKG || signal == VSM_SIGNAL_PPG1) {

        if (signal == VSM_SIGNAL_EKG) {

            sum_start = EKG_SUM_START;
            sum_end = EKG_SUM_END;
            #ifndef GOLDEN_TEST
            sample_count = EKG_SAMPLE;
            sram_type = VSM_SRAM_EKG;
            #else
            sample_count = sizeof(data_ekg_input)/sizeof(data_ekg_input[0]);
            #endif
        } else if (signal == VSM_SIGNAL_PPG1) {
            sum_start = PPG_SUM_START;
            sum_end = PPG_SUM_END;
            #ifndef GOLDEN_TEST
            sram_type = VSM_SRAM_PPG1;
            sample_count = PPG_SAMPLE;
            #else
            sample_count = sizeof(data_ppg_even_input)/sizeof(data_ppg_even_input[0]);
            #endif
        }

        LOGI("prepare init \r\n");

#ifndef GOLDEN_TEST
        /* initialize vsm */
        vsm_driver_init();
        LOGI("prepare set signal %d \r\n", signal);
        /* enable ekg mode and get ppg data */
        vsm_driver_set_signal(signal);
#endif
        memset(fft_data_in, 0, sizeof(fft_data_in));
        memset(fft_data_out, 0, sizeof(fft_data_out));
        memset(noise, 0, sizeof(noise));
        //LOGI("prepare get data and convert to mV, %f\r\n", 1.2f * 1.2f);
#ifndef GOLDEN_TEST
        /*1. collect ekg 16384 point sram data to fft_data[EKG_SAMPLE] */
        while(data_count < sample_count) {
            vsm_driver_read_sram((vsm_sram_type_t)sram_type, (uint32_t *)read_data, (int32_t *)&output_len);
            for (i = 0; i < output_len; i ++) {
                if (data_count == sample_count) {
                    break;
                }

                //filter data by data_ppg1_buf(find(data_ppg1_buf >2^22)) = data_ppg1_buf(find(data_ppg1_buf >2^22)) - 2^23; % 2's complement
                if(read_data[i] > 4194304) {
                    read_data[i] -= 8388608;
                }
                // get sram data and convert to mV
                if (signal == VSM_SIGNAL_EKG) {
                    if ((data_count == 0 && i > 50) || (data_count > 0)) {
                        fft_data_in[data_count].i = 0.0;
                        raw_data[data_count] = read_data[i];
                        fft_data_in[data_count].r = (double)(((double)(int)read_data[i]) * 1000 / EKG_GAIN / EKG_ADC_LSB);
                        data_count++;
                    }
                } else if (signal == VSM_SIGNAL_PPG1 && is_odd && (i%2 == 0)) {
                    /*input ppg1 led1 raw data*/
                    if ((data_count == 0 && i > 10) || (data_count > 0)) {
                        fft_data_in[data_count].i = 0.0;
                        raw_data[data_count] = read_data[i];
                        fft_data_in[data_count].r = (float)(((float)(int)read_data[i]) * 1000 * PPG_VPP/ (PPG_TIA_GAIN * PPG_PGA_GAIN) / PPG_ADC_LSB);
                        data_count++;
                    }
                    #if 0
                    if(i < 10) {
                        printf("read_data[%d] %d\r\n", i, read_data[i]);
                    }
                    #endif
                } else if (signal == VSM_SIGNAL_PPG1 && !is_odd && (i%2 == 1)) {
                    /*input ppg1 ambient1 raw data*/
                    if ((data_count == 0 && i > 10) || (data_count > 0)) {
                        fft_data_in[data_count].i = 0.0;
                        raw_data[data_count] = read_data[i];
                        fft_data_in[data_count].r = (float)(((float)(int)read_data[i]) * 1000 * PPG_VPP/ (PPG_TIA_GAIN * PPG_PGA_GAIN) / PPG_ADC_LSB);
                        data_count++;
                    }
                    #if 0
                    if(i < 10) {
                        printf("read_data[%d] %d\r\n", i, read_data[i]);
                    }
                    #endif
                }
                #if 0
                if(i < 10) {
                    LOGI("read_data[%d] %lu\r\n", i, read_data[i]);
                }
                #endif

            }
            sum += output_len;

            // wait data if not collecting enough data
            if (data_count < sample_count) {
                ms_delay(100);
            }
        }

        /* amplitude computation, mV data minus reference data*/
        if (is_amplitude) {

        }
#else
        for (i = 0; i < sample_count; i ++) {
            /*1. get sram data and convert to mV */
            if (signal == VSM_SIGNAL_EKG) {
                if(data_ekg_input[i] > 4194304) {
                    data_ekg_input[i] -= 8388608;
                }
                fft_data_in[i].i = 0.0;
                fft_data_in[i].r = (double)(((double)data_ekg_input[i]) * 1000 / EKG_GAIN / EKG_ADC_LSB);
            } else if (signal == VSM_SIGNAL_PPG1 && is_odd) {
                /*input ppg1 led1 raw data*/
                if(data_ppg_odd_input[i] > 4194304) {
                    data_ppg_odd_input[i] -= 8388608;
                }
                fft_data_in[i].i = 0.0;
                fft_data_in[i].r = (float)(((float)data_ppg_odd_input[i]) * 1000 * PPG_VPP/ (PPG_TIA_GAIN * PPG_PGA_GAIN) / PPG_ADC_LSB);
            }else if (signal == VSM_SIGNAL_PPG1 && !is_odd) {
                /*input ppg1 ambient1 raw data*/
                if(data_ppg_even_input[i] > 4194304) {
                    data_ppg_even_input[i] -= 8388608;
                }
                fft_data_in[i].i = 0.0;
                fft_data_in[i].r = (float)(((float)data_ppg_even_input[i]) * 1000 * PPG_VPP/ (PPG_TIA_GAIN * PPG_PGA_GAIN) / PPG_ADC_LSB);
            }
            #if 0
            if (i < 10) {
                LOGI("%.12f\r\n", fft_data_in[i].r);
            }
            #endif
        }
#endif

        /*2. convert mV to noise*/
        /*2-1. sub mean*/
        mean(fft_data_in, &mean_data, sample_count);
        LOGI("prepare convert to mV, mean_data %f,%e\r\n", mean_data, mean_data);
        for( i = 0; i < sample_count; i ++) {
            #if 0
            LOGI("fft_data_in[%d].r %.12f \r\n", i, fft_data_in[i].r);
            #endif
            fft_data_in[i].r = fft_data_in[i].r - (mean_data);
            //fft_data_in[i].r = (fft_data_in[i].r - (mean_data)) * 1000;
        }

        if ((cfg = kiss_fft_alloc(sample_count, 0/*is_inverse_fft*/, NULL, NULL)) != NULL)
        {
            int i;
            //compute fft
            kiss_fft(cfg, fft_data_in, fft_data_out);
            KISS_FFT_FREE(cfg);

            #if 0
            //print part of fft results
            for (i = 0; i < 10; i++)
              LOGI(" in[%d] = %e , %e    "
                     "out[%d] = %e , %e\r\n",
                     i, fft_data_in[i].r, fft_data_in[i].i,
                     i, fft_data_out[i].r, fft_data_out[i].i);
            #endif

            //Fsn_data = fft(sn_data' .* h_hamming, NFFT) / record_size;(optional)
            //FPsn_data = Fsn_data .* conj(Fsn_data) ; %magnitude , real square + i square
            for (i = 0; i < sample_count; i ++) {
                fft_data_out[i].r /= sample_count;
                fft_data_out[i].i /= sample_count;
                noise[i] = fft_data_out[i].r * fft_data_out[i].r + fft_data_out[i].i * fft_data_out[i].i;
                #if 0
                if (i < 10) {
                    LOGI("fft_data_out[%d] = %e , %e, square out: noise[%d]:%e\r\n",
                        i, (double)fft_data_out[i].r, (double)fft_data_out[i].i, i, (double)noise[i]);
                }
                #endif
            }
            //Nfin=floor(sfin/sfreq*NFFT);
            //Nfout=floor(sfout/sfreq*NFFT);
            //nn=sum(FPsn_data(Nfin:Nfout))^0.5*2*1000;
            //compute noise result
            *signal_noise = 0;
            for (i = sum_start; i < sum_end; i ++) {
                *signal_noise += (double)noise[i];
                signal_result += (double)noise[i];
            }
            LOGI("sum noise: signal_result:%.12f\r\n", signal_result);
            *signal_noise = (float)sqrt(*signal_noise)*2*1000;
            signal_result = (float)sqrt(signal_result)*2*1000;
            //signal_result = (float)sqrt(signal_result < 0?signal_result*(-1):signal_result)*2*1000;

            LOGI("noise_result %.12f , signal_result %.12f\r\n", *signal_noise, signal_result);
        } else {
            return -1;
        }
    }else {
        return -1;
    }
    return 0;
}

// AT command handler
atci_status_t atci_cmd_hdlr_clover(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}};
    char *param = NULL;
    int  param1_val = -1, param2_val = -1;


    LOGI("atci_cmd_hdlr_clover \r\n");
    response.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+EKEYURC=?
            strcpy((char *)response.response_buf, "+ECLOVER=(0,1)\r\nOK\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
        case ATCI_CMD_MODE_EXECUTION: // rec: AT+EKEYURC=<op>  the handler need to parse the parameters
            LOGI("AT Executing...\r\n");
            #if 0
            param = strtok(parse_cmd->string_ptr, ",\n\r");
            param = strtok(parse_cmd->string_ptr, "AT+ECLOVER=");
            param1_val = atoi(param);
            #endif
            param = strtok(parse_cmd->string_ptr, "=");
            param1_val = atoi(strtok(NULL, ","));
            param2_val = atoi(strtok(NULL, ","));

            if (param != NULL && (param1_val == 0 || param1_val == 1 || param1_val == 2)) {
                bus_data_t data;
                uint32_t reg_data;
                int err = 0;
                int32_t len, i = 0;
                //set mode
                data.data_buf = (uint8_t *)&reg_data;
            	data.length = sizeof(reg_data);
                /* valid parameter, update the data and return "OK"
                                param1_val = 0 :  I2C operation.
                                param1_val = 1 :  turn on/off LED.
                */

                if ( param1_val == 0) {
                    /* initialize vsm */
                    vsm_driver_init();
                    /*get specific register data to verify I2C operation*/
                    len = sizeof(test_data)/sizeof(test_data[0]);
                    for (i = 0; i < len; i ++) {
                        data.addr = (test_data[i].addr & 0xFF00) >> 8;
		                data.reg = (test_data[i].addr & 0xFF);
		                    LOGI("data.addr 0x%x, data.reg 0x%x\r\n", data.addr, data.reg);
                        vsm_driver_read_register(&data);
                        if (*(uint32_t *)(data.data_buf) != test_data[i].value) {
                            LOGI("clover data not consistent...\r\n");
    						response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
    						response.response_len   = 0;
                            err = 1;
                            break;
                        }
                    }
                    if (err == 0) {
                        LOGI("all clover data are consistent...\r\n");
						response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
						response.response_len   = 0;
                    }
                }
                else if (param1_val == 1) {
                    /* initialize vsm */
                    vsm_driver_init();
                    /* enable ppg mode and also enable led */
                    vsm_driver_set_signal(VSM_SIGNAL_PPG1);
                    ms_delay(1000);
                    #if 0
                    /* turn off ppg1 led */
                    err = vsm_driver_set_led_current(VSM_SIGNAL_PPG1, 0x00);
                    if (err == VSM_STATUS_OK) {
                        LOGI("clover turn off ppg1 led successfully... \r\n");
                        /* wait for while */

                        /* turn off ppg2 led */
                        err = vsm_driver_set_led_current(VSM_SIGNAL_PPG2, 0x00);
                        if (err == VSM_STATUS_OK) {
                            LOGI("clover turn off ppg2 led successfully... \r\n");
                            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
    						response.response_len   = 0;
                        } else {
                            LOGI("clover turn off ppg2 led failed... \r\n");
                            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
	    					response.response_len   = 0;
                        }
                    } else {
                        LOGI("clover turn off ppg1 led failed... \r\n");
                        response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
						response.response_len   = 0;
                    }
                    #else
                    hal_gpio_set_output(HAL_GPIO_11, HAL_GPIO_DATA_LOW);
                    LOGI("clover turn off ppg2 led successfully... \r\n");
                    response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
    				response.response_len   = 0;
                    #endif
                } else if (param1_val == 2) {
                    int32_t output_len;

                    /* initialize vsm */
                    vsm_driver_init();
                    /* enable ppg mode and get ppg data */
                    vsm_driver_set_signal(VSM_SIGNAL_PPG1);
                    ms_delay(1000);
                    /* read back ppg data */
                    memset(read_data, 0, sizeof(read_data));
                    vsm_driver_read_sram(VSM_SRAM_PPG1, (uint32_t *)read_data, &output_len);
                    if (output_len > 1) {
                        snprintf((char *)response.response_buf, sizeof(response.response_buf), "+ECLOVER: 0x%lx\r\n", read_data[output_len/2]);
                        response.response_len = strlen((char *)response.response_buf);
                        //response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                        atci_send_response(&response);
                    }
                    #if 0
                    if (ret == VSM_STATUS_OK) {
                        /* enable ekg mode and get ekg data */
                        vsm_driver_set_signal(VSM_SIGNAL_EKG);
                        ms_delay(1000);
                        /* read back ppg data */
                        memset(read_data, 0, sizeof(read_data));
                        ret = vsm_driver_read_sram(VSM_SIGNAL_EKG, (uint32_t *)read_data, &output_len);

                        if (ret == VSM_STATUS_OK) {
                            LOGI("clover read sram successfully... \r\n");
                            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
    						response.response_len   = 0;
                        } else {
                            LOGI("clover read sram fail... \r\n");
                            strcpy(response.response_buf, "ERROR\r\n");
                            response.response_len = strlen(response.response_buf);
                        }
                    } else {
                        LOGI("clover read sram fail... \r\n");
                        strcpy(response.response_buf, "ERROR\r\n");
                        response.response_len = strlen(response.response_buf);
                    }
                    #else
                    hal_gpio_set_output(HAL_GPIO_11, HAL_GPIO_DATA_LOW);
                    LOGI("clover read sram successfully... \r\n");
                    response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
    				response.response_len   = 0;
                    #endif
                }
            }else if (param1_val == 3) {
                /* initialize vsm */
                vsm_driver_init();
                /* enable ppg mode and get ppg data */
                vsm_driver_set_signal(VSM_SIGNAL_PPG1);
                LOGI("powern on and turn on ppg successfully \r\n");
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                response.response_len   = 0;
            } else if (param1_val == 4) {
                /* deinitialize vsm */
                vsm_driver_deinit();
                LOGI("disable clover successfully \r\n");
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                response.response_len   = 0;
            } else if (param1_val == 5) { /*ekg noise test*/
                double signal_nosie;
                signal_noise_compute(VSM_SIGNAL_EKG, &signal_nosie, false, false);
                snprintf((char *)response.response_buf, sizeof(response.response_buf), "+ECLOVER:0,%.12f\r\n", signal_nosie);
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                //response.response_len   = 0;
                response.response_len = strlen((char *)response.response_buf);
            } else if (param1_val == 6) { /*ppg led1 noise test*/
                double signal_nosie;
                signal_noise_compute(VSM_SIGNAL_PPG1, &signal_nosie, true, false);
                snprintf((char *)response.response_buf, sizeof(response.response_buf), "+ECLOVER:1,%.12f\r\n", signal_nosie);
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                //response.response_len   = 0;
                response.response_len = strlen((char *)response.response_buf);
            } else if (param1_val == 7) { /*ppg ambient1 noise test*/
                double signal_nosie;
                signal_noise_compute(VSM_SIGNAL_PPG1, &signal_nosie, false, false);
                snprintf((char *)response.response_buf, sizeof(response.response_buf), "+ECLOVER:2,%.12f\r\n", signal_nosie);
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                //response.response_len   = 0;
                response.response_len = strlen((char *)response.response_buf);
            } else if (param1_val == 8) { /*ekg amplitude noise test*/
                double signal_nosie;
                signal_noise_compute(VSM_SIGNAL_EKG, &signal_nosie, false, true);
                snprintf((char *)response.response_buf, sizeof(response.response_buf), "+ECLOVER:2,%.12f\r\n", signal_nosie);
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                //response.response_len   = 0;
                response.response_len = strlen((char *)response.response_buf);
            } else if (param1_val == 9) { /*ppg1 led amplitude noise test*/
                double signal_nosie;
                signal_noise_compute(VSM_SIGNAL_PPG1, &signal_nosie, true, true);
                snprintf((char *)response.response_buf, sizeof(response.response_buf), "+ECLOVER:2,%.12f\r\n", signal_nosie);
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                //response.response_len   = 0;
                response.response_len = strlen((char *)response.response_buf);
            } else if (param1_val == 10) { /*ppg1 led ambient amplitude noise test*/
                double signal_nosie;
                signal_noise_compute(VSM_SIGNAL_PPG1, &signal_nosie, false, true);
                snprintf((char *)response.response_buf, sizeof(response.response_buf), "+ECLOVER:2,%.12f\r\n", signal_nosie);
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                //response.response_len   = 0;
                response.response_len = strlen((char *)response.response_buf);
            } else if (param1_val == 11) {
                if (param2_val == 1 || param2_val == 2) {
                    if (param2_val == 1) {
                        /*led current test, tia_gain 100K*/
                        current_test(LED_CUR_TEST, TIA_GAIN_100K);
                    } else if (param2_val == 2) {
                        /*led current test, tia_gain 500K*/
                        current_test(LED_CUR_TEST, TIA_GAIN_500K);
                    }
                    xTaskCreate( atci_clover_task_dump, "current_test_dump_task", 4096, &param1_val, 3, NULL);
                    response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                    response.response_len = 0;
                } else {
                    /*invalide parameter, return "ERROR"*/
                    strcpy((char *)response.response_buf, "ERROR\r\n");
                    response.response_len = strlen((char *)response.response_buf);
                }
            } else if (param1_val == 12) {
                if (param2_val == 1 || param2_val == 2) {
                    if (param2_val == 1) {
                        /*ambdac current test, tia_gain 100K*/
                        current_test(LED_AMBDAC_CUR_TEST, TIA_GAIN_100K);
                    } else if (param2_val == 2) {
                        /*ambdac current test, tia_gain 500K*/
                        current_test(LED_AMBDAC_CUR_TEST, TIA_GAIN_100K);
                    }
                    xTaskCreate( atci_clover_task_dump, "current_test_dump_task", 4096, &param1_val, 3, NULL);
                    response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                    response.response_len = 0;
                } else {
                    /*invalide parameter, return "ERROR"*/
                    strcpy((char *)response.response_buf, "ERROR\r\n");
                    response.response_len = strlen((char *)response.response_buf);
                }
            }
            else {
                /*invalide parameter, return "ERROR"*/
                strcpy((char *)response.response_buf, "ERROR\r\n");
                response.response_len = strlen((char *)response.response_buf);
            }

            atci_send_response(&response);
            param = NULL;
            break;

        default :
            /* others are invalid command format */
            strcpy((char *)response.response_buf, "ERROR\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}


