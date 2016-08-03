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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "sys_init.h"
#include "wifi_api.h"
#include "syslog.h"
#include "mbedtls/aes.h"
#include "mbedtls/md.h"
#include "sta_network.h"

/**
* @brief       This function tests AES encryption and decryption.
* @return     0, if OK.\n
*                 Error code, if errors occurred.\n
*/
int aes_main( void );
/**
* @brief       This function tests MD5.
* @return     0, if OK.\n
*                 Error code, if errors occurred.\n
*/
int md5_main( void );
/**
* @brief       This function tests MPI.
* @return     0, if OK.\n
*                 Error code, if errors occurred.\n
*/
int mpi_demo_main( void );
/**
* @brief          This function tests private key decryption.
* @param[in]   enc_buf: Encrypted data.
* @param[in]   enc_len: The size of enc_buf.
* @param[in]   key_buf: Private key.
* @param[in]   key_len: The size of key_buf.
* @return        0, if OK.\n
*                    Error code, if errors occurred.\n
*/
int pk_decrypt_main(char *enc_buf, size_t enc_len, char *key_buf, size_t key_len);
/**
* @brief           This function tests public key encryption.
* @param[in]    input: Message to encrypt.
* @param[out]  output: Encrypted output.
* @param[out]  olen: Encrypted output length.
* @param[in]    output_buf_len: Size of the output buffer.
* @param[in]    key_buf: Public key.
* @param[in]    key_len: The size of key_buf.
* @return         0, if OK.\n
*                     Error code, if errors occurred.\n
*/
int pk_encrypt_main( char *input, char *output, size_t *olen, size_t output_buf_len, char *key_buf, size_t key_len );
/**
* @brief          This function tests RSA decryption.
* @param[in]   input: Encrypted data.
* @return        0, if OK.\n
*                    Error code, if errors occurred.\n
*/
int rsa_decrypt_main( char *input );
/**
* @brief           This function tests RSA encryption.
* @param[in]    input: Message to encrypt.
* @param[out]  output: Encrypted output.
* @return         0, if OK.\n
*                     Error code, if errors occurred.\n
*/
int rsa_encrypt_main( char *input, char *output );
/**
* @brief           This function calls mbedtls' selftest functions to test MD5, SHA256, SHA512, AES, DES, GCM, and so on.
* @param[in]    argc: Argument counter.
* @param[in]    argv: Argument vector.
* @return         0, if OK.\n
*                     Error code, if errors occurred.\n
*/
int tls_selftest_main( int argc, char *argv[] );
/**
* @brief           This function is a demonstration of ssl client.
* @return         0, if OK.\n
*                     Error code, if errors occurred.\n
*/
int ssl_client_main(void);


char g_plaintext[512] = "===Hello! This is plaintext!===";

char pkey_test_pub_rsa[] =
    "-----BEGIN PUBLIC KEY-----\r\n"
    "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCvzifpyCVxJXDC6nNg84+Uu4ed\r\n"
    "m2izOV2WD30lyuz0/VY5AS6ZEiqtUSxImkrJkqdda46VRyhSll5Ep+tHsdRc6ADl\r\n"
    "NQtOfz4EZAHrze4jkXnJ6duzb0S5ZSyDCTpVOZCZsYMYE0o2O4JglNss0zeT6DaR\r\n"
    "c13AKjrGPrywT6yfSwIDAQAB\r\n"
    "-----END PUBLIC KEY-----\r\n";

char pkey_test_key_rsa[] =
    "-----BEGIN RSA PRIVATE KEY-----\r\n"
    "MIICXAIBAAKBgQCvzifpyCVxJXDC6nNg84+Uu4edm2izOV2WD30lyuz0/VY5AS6Z\r\n"
    "EiqtUSxImkrJkqdda46VRyhSll5Ep+tHsdRc6ADlNQtOfz4EZAHrze4jkXnJ6duz\r\n"
    "b0S5ZSyDCTpVOZCZsYMYE0o2O4JglNss0zeT6DaRc13AKjrGPrywT6yfSwIDAQAB\r\n"
    "AoGAESTigYrSE+mZyHhCjibSTqfG/tij6i5i8PpLsv7KAs4dtWtnFuhNnx82WVIq\r\n"
    "juOtTI/rlKUeyob9ZGaXrCMsWUWSutCKus9myRjpGxEqWHPF0Ge6KiFKMdyTYvNk\r\n"
    "ymC3sFKDImcF619wj80e13GS4iHGrkFA5mZO1F9cxtI+RUECQQDiwzlfGlJyY8be\r\n"
    "3y1L5nKeLpXF4riz7RQQKQR5WOH6Y2HkkKrUlZeMIe668L330OWBfSsOTkGEXRUp\r\n"
    "Y+OwT1mrAkEAxnj6p9rRvtbGhl6Mlj3xWzBwXRHwsxPnjex7IEEIovRL3FmyA+4V\r\n"
    "t9TMeKxc+aurh+X7Pyk5itvcqC/5fBxw4QJASwui3AeBC5xbv3yKqBjPC+yM4p2C\r\n"
    "1QD759E7StGQj+X+Cr+Z2ZrcOaMtN67en7oBilYbPrPFWQHZNAZ73uiT2wJBAKU8\r\n"
    "X2KT+P+rDAkeemkzFNfYkhPKNdzBe4xbD38g5bHVNbs0KdK/yvELh1gIGDf8xogT\r\n"
    "3oMNLU0AEssrdcfwXcECQDsjIvLARh4qfzAcDtzkYseKNxl5C1fev0TXqYHseplC\r\n"
    "ezfOKLU5FqbA5mow/QaAAzLCw4vIv3D4HBIC+ZRRU7E=\r\n"
    "-----END RSA PRIVATE KEY-----\r\n";

/**
* @brief           This function runs the task of mbedtls example.
* @return         0, if OK.\n
*                     Error code, if errors occurred.\n
*/
int mbedtls_example()
{
    char rsa_output[512];
    size_t olen = 0;

    printf("%s is running\n", __func__);

    printf("AES demonstration starts...\n");
    aes_main();
    printf("AES demonstration ends...\n");

    printf("MD5 demonstration starts...\n");
    md5_main();
    printf("MD5 demonstration ends...\n");

    printf("MPI demonstration starts...\n");
    mpi_demo_main();
    printf("MPI demonstration ends...\n");

    printf("RSA encryption & decryption demonstration starts...\n");
    rsa_encrypt_main(g_plaintext, rsa_output);
    rsa_decrypt_main(rsa_output);
    printf("RSA encryption & decryption demonstration ends...\n");

    printf("Public key-based encryption & decryption demonstration starts...\n");
    pk_encrypt_main(g_plaintext, rsa_output, &olen, 512, pkey_test_pub_rsa, sizeof(pkey_test_pub_rsa));
    pk_decrypt_main(rsa_output, olen, pkey_test_key_rsa, sizeof(pkey_test_key_rsa));
    printf("Public key-based encryption & decryption demonstration ends...\n");

    printf("Selftest demonstration starts...\n");
    tls_selftest_main(0 , NULL);
    printf("Selftest demonstration ends...\n");

    printf("SSL client demonstration starts...\n");
    ssl_client_main();
    printf("SSL client demonstration ends...\n");

    return 0;
}


/**
  * @brief     Create a task for mbedtls example
  * @param[in] void *args:Not used
  * @retval    None
  */
void user_entry(void *args)
{
    sta_network_ready();

    mbedtls_example();

    while (1) {
    }
}


int main(void)
{
    system_init();

    sta_network_init();

    xTaskCreate(user_entry, "user entry", 2048, NULL, 1, NULL);

    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for ( ;; );
}
