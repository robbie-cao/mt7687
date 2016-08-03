/*
 *  Simple MPI demonstration program
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf     printf
#endif

#if defined(MBEDTLS_BIGNUM_C)
#include "mbedtls/bignum.h"

#include <stdio.h>
#endif


#if !defined(MBEDTLS_BIGNUM_C)
/**
* @brief       This function tests MPI.
* @return     0, if OK.\n
*                 Error code, if errors occurred.\n
*/
int mpi_demo_main( void )
{
    mbedtls_printf("MBEDTLS_BIGNUM_C and/or MBEDTLS_FS_IO not defined.\n");
    return ( 0 );
}
#else
#define BUF_LEN 512

/**
* @brief       This function tests MPI.
* @return     0, if OK.\n
*                 Error code, if errors occurred.\n
*/
int mpi_demo_main( void )
{
    int ret;
    mbedtls_mpi E, P, Q, N, H, D, X, Y, Z;
    char buffer[BUF_LEN];
    size_t olen;

    mbedtls_printf("%s\n", __func__);
    mbedtls_mpi_init( &E );
    mbedtls_mpi_init( &P );
    mbedtls_mpi_init( &Q );
    mbedtls_mpi_init( &N );
    mbedtls_mpi_init( &H );
    mbedtls_mpi_init( &D );
    mbedtls_mpi_init( &X );
    mbedtls_mpi_init( &Y );
    mbedtls_mpi_init( &Z );

    MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &P, 10, "2789" ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &Q, 10, "3203" ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &E, 10,  "257" ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &N, &P, &Q ) );

    mbedtls_printf( "\n  Public key:\n\n" );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_string( &N, 16, buffer, BUF_LEN, &olen ) );
    mbedtls_printf("N = %s\n", buffer);
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_string( &E, 16, buffer, BUF_LEN, &olen ) );
    mbedtls_printf("E = %s\n", buffer);

    mbedtls_printf( "\n  Private key:\n\n" );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_string( &P, 16, buffer, BUF_LEN, &olen ) );
    mbedtls_printf("P = %s\n", buffer);

    MBEDTLS_MPI_CHK( mbedtls_mpi_write_string( &Q, 16, buffer, BUF_LEN, &olen ) );
    mbedtls_printf("Q = %s\n", buffer);

#if defined(MBEDTLS_GENPRIME)
    MBEDTLS_MPI_CHK( mbedtls_mpi_sub_int( &P, &P, 1 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_sub_int( &Q, &Q, 1 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &H, &P, &Q ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_inv_mod( &D, &E, &H ) );

    mbedtls_mpi_write_string( &D, 16, buffer, BUF_LEN, &olen );
    mbedtls_printf("D = E^-1 mod (P-1)*(Q-1) = %s\n", buffer);
#else
    mbedtls_printf("\nTest skipped (MBEDTLS_GENPRIME not defined).\n\n");
#endif
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &X, 10, "55555" ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &Y, &X, &E, &N, NULL ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_exp_mod( &Z, &Y, &D, &N, NULL ) );

    mbedtls_printf( "\n  RSA operation:\n\n" );
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_string( &X, 16, buffer, BUF_LEN, &olen ) );
    mbedtls_printf("X (plaintext)  = %s\n", buffer);
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_string( &Y, 16, buffer, BUF_LEN, &olen ) );
    mbedtls_printf("Y (ciphertext) = X^E mod N = %s\n", buffer);
    MBEDTLS_MPI_CHK( mbedtls_mpi_write_string( &Z, 16, buffer, BUF_LEN, &olen ) );
    mbedtls_printf("Z (decrypted)  = Y^D mod N = %s\n\n", buffer);

cleanup:
    mbedtls_mpi_free( &E );
    mbedtls_mpi_free( &P );
    mbedtls_mpi_free( &Q );
    mbedtls_mpi_free( &N );
    mbedtls_mpi_free( &H );
    mbedtls_mpi_free( &D );
    mbedtls_mpi_free( &X );
    mbedtls_mpi_free( &Y );
    mbedtls_mpi_free( &Z );

    if ( ret != 0 ) {
        mbedtls_printf( "\nAn error occurred.\n" );
        ret = 1;
    }

    return ( ret );
}
#endif /* MBEDTLS_BIGNUM_C */
