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

 #include "bitblt.h"



void bitblt_sad(G2D_REG* reg)
{
    int     i, j;
    int     dst_width, dst_height;
    int     src_width;
    int     src_cur_x, src_cur_y, dst_cur_x, dst_cur_y;
    int     src_clr_mode, dst_clr_mode;
    UINT32  sad_acc;
    int     sad_en[4];
    UINT8*  src_buf_ptr8;
    UINT8*  dst_buf_ptr8;
    int     sad_w_mod;
    int     src_pitch, dst_pitch;
    Pixel   src, dst;

    sad_w_mod    = (reg->G2D_MODE_CON >> 22) & 0x3;
    src_buf_ptr8 = (UINT8*)reg->G2D_L1_ADDR;
    dst_buf_ptr8 = (UINT8*)reg->G2D_L0_ADDR;
    dst_width    = (reg->G2D_L0_SIZE >> 16) & 0xfff;
    dst_height   = reg->G2D_L0_SIZE & 0xfff;
    src_pitch    = reg->G2D_L1_PITCH;
    dst_pitch    = reg->G2D_L0_PITCH;
    src_clr_mode = reg->G2D_L1_CON & 0xf;
    dst_clr_mode = reg->G2D_L0_CON & 0xf;
    src_width    = (reg->G2D_L1_SIZE >> 16) & 0xfff;

    sad_acc = 0;                                    //added by chia-hua, 2009.03.18
    for(i = 0 ; i < dst_height; i++){
        for(j = 0 ; j < dst_width; j++){
            dst_cur_x = j;
            dst_cur_y = i;
            src_cur_x = j;
            src_cur_y = i;
            
            getcolor( src_buf_ptr8, 
                      src_clr_mode,
                      0, 
                      src_pitch,
                      src_cur_x,
                      src_cur_y,
                      &src);
            
            getcolor( dst_buf_ptr8, 
                dst_clr_mode,
                0, 
                dst_pitch,
                dst_cur_x,
                dst_cur_y,
                &dst);

            if(src_cur_x == (src_width - 1)){
                switch( sad_w_mod )
                {
                case 0: sad_en[3] = 1; sad_en[2] = 1; sad_en[1] = 1; sad_en[0] = 1; break;
                case 1: sad_en[3] = 0; sad_en[2] = 0; sad_en[1] = 0; sad_en[0] = 1; break;
                case 2: sad_en[3] = 0; sad_en[2] = 0; sad_en[1] = 1; sad_en[0] = 1; break;
                case 3: sad_en[3] = 0; sad_en[2] = 1; sad_en[1] = 1; sad_en[0] = 1; break;
                default: 
                	///G2D_LOGI("Error: Illegal sad_w_mod setting!! \n"); exit(1); 
                break;
                }
            }
            else{
                sad_en[3] = 1; sad_en[2] = 1; sad_en[1] = 1; sad_en[0] = 1;
            }

            sad( src_clr_mode, &src, &dst, &sad_acc, sad_en);       
        }
    }
    reg->G2D_SAD_SUM = sad_acc;  //added by chia-hau, 2009.03.18

#ifdef G2D_DRAW_SAD_ON_DST
        dst_buf_ptr8 = (UINT8*)(reg->G2D_W2M_ADDR);
        dst.a = (reg->G2D_SAD_SUM >> 24) & 0xff;
        dst.r = (reg->G2D_SAD_SUM >> 16) & 0xff;
        dst.g = (reg->G2D_SAD_SUM >>  8) & 0xff;
        dst.b = (reg->G2D_SAD_SUM >>  0) & 0xff;
        writecolor( dst_buf_ptr8, CLR_ARGB8888, 0, reg->G2D_W2M_PITCH, 0, 0, &dst);
        
        dst.a = dst.r = dst.g = dst.b = 0;
        writecolor( dst_buf_ptr8, CLR_ARGB8888, 0, reg->G2D_W2M_PITCH, 1, 0, &dst);
        G2D_LOGI("SAD result = %d\n", sad_acc);
#endif

}


void alpha_blending( int src_clr_mode, UINT8 const_alp, UINT8 alpbld_type, Pixel* src, Pixel* dst)
{
    UINT32 c;
    UINT8  alp;

#ifdef DEBUG
    G2D_LOGI("  -alpbld type = %d\n", alpbld_type);
    G2D_LOGI("  -alpbld src     = (%2.2x, %2.2x, %2.2x, %2.2x)\n", src->a, src->r, src->g, src->b);
    if( alpbld_type != 2)
        G2D_LOGI("  -alpbld dst     = (%2.2x, %2.2x, %2.2x, %2.2x)\n", dst->a, dst->r, dst->g, dst->b);
#endif
    //alpbld_type = 0, 1, 2, 6

    //if( (src_clr_mode==CLR_ARGB8888) && (alpbld_type!=2) )        //modified by chia-hua, 2009.07.20
    if( (alpbld_type==0 || alpbld_type==1) && 
        (src_clr_mode==CLR_ARGB8888 || src_clr_mode==CLR_PARGB8888 ||
         src_clr_mode==CLR_ARGB565  || src_clr_mode==CLR_PARGB565  ||
         src_clr_mode==CLR_ARGB666  || src_clr_mode==CLR_PARGB666  
        )
      )
        alp = src->a;
    else
        alp = const_alp;
    
    switch(alpbld_type)
    {
    case 0: 
        c = alp*src->r + (255-alp)*dst->r + 0x80;
        dst->r = ( c + (c >> 8) ) >> 8;
        dst->r = (dst->r > 0xff) ? 0xff : dst->r;
        c = alp*src->g + (255-alp)*dst->g + 0x80;
        dst->g = ( c + (c >> 8) ) >> 8;
        dst->g = (dst->g > 0xff) ? 0xff : dst->g;
        c = alp*src->b + (255-alp)*dst->b + 0x80;
        dst->b = ( c + (c >> 8) ) >> 8;
        dst->b = (dst->b > 0xff) ? 0xff : dst->b;

        //if(src_clr_mode==CLR_ARGB8888){       //modified by chia-hua, 2009.07.20
        if(src_clr_mode==CLR_ARGB8888 || src_clr_mode==CLR_PARGB8888 ||
           src_clr_mode==CLR_ARGB565  || src_clr_mode==CLR_PARGB565  ||
           src_clr_mode==CLR_ARGB666  || src_clr_mode==CLR_PARGB666  
          )
        {
            c = (255-alp)*dst->a + 0x80;
            dst->a = src->a + ((c + (c>>8)) >>8);
        }
        else{
            c = alp*src->a + (255-alp)*dst->a + 0x80;
            dst->a = (c + (c>>8) ) >> 8;
        }
        dst->a = (dst->a > 0xff) ? 0xff : dst->a;
        break;

    case 1:
        c = (255-alp)*dst->r + 0x80;
        dst->r = src->r + (( c + (c >> 8) ) >> 8);
        dst->r = (dst->r > 0xff) ? 0xff : dst->r;
        c = (255-alp)*dst->g + 0x80;
        dst->g = src->g + (( c + (c >> 8) ) >> 8);
        dst->g = (dst->g > 0xff) ? 0xff : dst->g;
        c = (255-alp)*dst->b + 0x80;
        dst->b = src->b + (( c + (c >> 8) ) >> 8);
        dst->b = (dst->b > 0xff) ? 0xff : dst->b;
        c = (255-alp)*dst->a + 0x80;
        dst->a = src->a + (( c + (c >> 8) ) >> 8);
        dst->a = (dst->a > 0xff) ? 0xff : dst->a;
        break;
    case 2:
        if(src_clr_mode==CLR_PARGB8888 || src_clr_mode==CLR_PARGB565 || src_clr_mode==CLR_PARGB666)
        {
            c = alp*src->r + 0x80;
            dst->r = ( c + (c >> 8) ) >> 8;
            dst->r = (dst->r > 0xff) ? 0xff : dst->r;
            c = alp*src->g + 0x80;
            dst->g = ( c + (c >> 8) ) >> 8;
            dst->g = (dst->g > 0xff) ? 0xff : dst->g;
            c = alp*src->b + 0x80;
            dst->b = ( c + (c >> 8) ) >> 8;
            dst->b = (dst->b > 0xff) ? 0xff : dst->b;
        }
        else if( src_clr_mode==CLR_ARGB8888 || src_clr_mode==CLR_ARGB565 || src_clr_mode==CLR_ARGB666)
        {
            dst->r = src->r;
            dst->g = src->g;
            dst->b = src->b;
        }
        else{
            ///G2D_LOGI("Error:Illegla color mode!!\n");
            ///exit(1);
        }
        c = alp*src->a + 0x80;
        dst->a = ( c + (c >> 8) ) >> 8;
        dst->a = (dst->a > 0xff) ? 0xff : dst->a;
        break;
    /*
    case 6:     //font
        c = alp*src->r + 0x80;
        dst->r = ( c + (c >> 8) ) >> 8;
        dst->r = (dst->r > 0xff) ? 0xff : dst->r;
        c = alp*src->g + 0x80;
        dst->g = ( c + (c >> 8) ) >> 8;
        dst->g = (dst->g > 0xff) ? 0xff : dst->g;
        c = alp*src->b + 0x80;
        dst->b = ( c + (c >> 8) ) >> 8;
        dst->b = (dst->b > 0xff) ? 0xff : dst->b;
        c = alp*src->a + 0x80;
        dst->a = ( c + (c >> 8) ) >> 8;
        dst->a = (dst->a > 0xff) ? 0xff : dst->a;
        break;
    */
    default: 
        ///G2D_LOGI("Error: Illegal alpbld type selection!\n");
        ///exit(1);
        break;
    }

    /*
    if(alpbld_type == 0){
        c = alp*src->r + (255-alp)*dst->r + 0x80;
        dst->r = ( c + (c >> 8) ) >> 8;
        dst->r = (dst->r > 0xff) ? 0xff : dst->r;
        c = alp*src->g + (255-alp)*dst->g + 0x80;
        dst->g = ( c + (c >> 8) ) >> 8;
        dst->g = (dst->g > 0xff) ? 0xff : dst->g;
        c = alp*src->b + (255-alp)*dst->b + 0x80;
        dst->b = ( c + (c >> 8) ) >> 8;
        dst->b = (dst->b > 0xff) ? 0xff : dst->b;

        //if(src_clr_mode==CLR_ARGB8888){       //modified by chia-hua, 2009.07.20
        if(src_clr_mode==CLR_ARGB8888 || src_clr_mode==CLR_PARGB8888){
            c = (255-alp)*dst->a + 0x80;
            dst->a = src->a + ((c + (c>>8)) >>8);
        }
        else{
            c = alp*src->a + (255-alp)*dst->a + 0x80;
            dst->a = (c + (c>>8) ) >> 8;
        }
        dst->a = (dst->a > 0xff) ? 0xff : dst->a;
    }
    else if(alpbld_type == 1) {
        c = (255-alp)*dst->r + 0x80;
        dst->r = src->r + (( c + (c >> 8) ) >> 8);
        dst->r = (dst->r > 0xff) ? 0xff : dst->r;
        c = (255-alp)*dst->g + 0x80;
        dst->g = src->g + (( c + (c >> 8) ) >> 8);
        dst->g = (dst->g > 0xff) ? 0xff : dst->g;
        c = (255-alp)*dst->b + 0x80;
        dst->b = src->b + (( c + (c >> 8) ) >> 8);
        dst->b = (dst->b > 0xff) ? 0xff : dst->b;
        c = (255-alp)*dst->a + 0x80;
        dst->a = src->a + (( c + (c >> 8) ) >> 8);
        dst->a = (dst->a > 0xff) ? 0xff : dst->a;
    }
    else {
        c = alp*src->r + 0x80;
        dst->r = ( c + (c >> 8) ) >> 8;
        dst->r = (dst->r > 0xff) ? 0xff : dst->r;
        c = alp*src->g + 0x80;
        dst->g = ( c + (c >> 8) ) >> 8;
        dst->g = (dst->g > 0xff) ? 0xff : dst->g;
        c = alp*src->b + 0x80;
        dst->b = ( c + (c >> 8) ) >> 8;
        dst->b = (dst->b > 0xff) ? 0xff : dst->b;
        c = alp*src->a + 0x80;
        dst->a = ( c + (c >> 8) ) >> 8;
        dst->a = (dst->a > 0xff) ? 0xff : dst->a;
    }
    */
#ifdef DEBUG
    G2D_LOGI("  -alpbld result = (%2.2x, %2.2x, %2.2x, %2.2x)\n", dst->a, dst->r, dst->g, dst->b);
#endif
}


void rop_decode( Pixel* P, Pixel* S, Pixel* D, Pixel* result, UINT8 rop_code)
{
    int i, comb, bit;
    int data;
    //Alpha channel
    data = 0;
    for(i=0; i<8; i++){
        comb = (((P->a >> i) & 1) << 2) |
               (((S->a >> i) & 1) << 1) |
               ((D->a >> i) & 1);
        bit = rop_code >> comb;
        bit &= 1;
        data |= (bit << i);
    }
    result->a = data;

    //Red channel
    data = 0;
    for(i=0; i<8; i++){
        comb = (((P->r >> i) & 1) << 2) |
               (((S->r >> i) & 1) << 1) |
               ((D->r >> i) & 1);
        bit = rop_code >> comb;
        bit &= 1;
        data |= (bit << i);
    }
    result->r = data;

    //Green channel
    data = 0;
    for(i=0; i<8; i++){
        comb = (((P->g >> i) & 1) << 2) |
               (((S->g >> i) & 1) << 1) |
               ((D->g >> i) & 1);
        bit = rop_code >> comb;
        bit &= 1;
        data |= (bit << i);
    }
    result->g = data;

    //Blue channel
    data = 0;
    for(i=0; i<8; i++){
        comb = (((P->b >> i) & 1) << 2) |
               (((S->b >> i) & 1) << 1) |
               ((D->b >> i) & 1);
        bit = rop_code >> comb;
        bit &= 1;
        data |= (bit << i);
    }
    result->b = data;

}

void sad( int clr_mode, Pixel* src,Pixel* dst, UINT32* sad_acc, int* sad_en)
{
    int diff = 0 ;
    switch( clr_mode ){
        case CLR_8BPP:
            diff = abs( src->b - dst->b) * sad_en[0];
            break;
        case CLR_RGB565:
        case CLR_RGB888:
            ///G2D_LOGI("Error: Not support this color format yet\n");
            ///exit(1);
            break;
        case CLR_ARGB8888:
        case CLR_PARGB8888:
            diff  = abs( src->b - dst->b) * sad_en[0];
            diff += abs( src->g - dst->g) * sad_en[1];
            diff += abs( src->r - dst->r) * sad_en[2];
            diff += abs( src->a - dst->a) * sad_en[3];
            break;
        default:
            ///G2D_LOGI("Error: Illegal color format!\n");
            ///exit(1);
            break;
    }
#ifdef DEBUG
    G2D_LOGI("    abs_diff_sum = %d\n", diff);
#endif
    *sad_acc += diff;
}


void alpbld_twophase( int src_clr_mode, UINT8 const_alp, Pixel* src, Pixel* dst)
{
    UINT8 cur_alpbld_type;
    Pixel src_tmp;
    switch( src_clr_mode )
    {
    case CLR_PARGB8888: 
    case CLR_PARGB565: 
    case CLR_PARGB666: 
            if( const_alp != 255){
                cur_alpbld_type = 2;
                alpha_blending( src_clr_mode, const_alp, cur_alpbld_type, src, &src_tmp);
            }
            else{
                color_copy( &src_tmp, src);
            }
            cur_alpbld_type = 1;
            alpha_blending( src_clr_mode, const_alp, cur_alpbld_type, &src_tmp, dst);
            break;
    case CLR_ARGB8888: 
    case CLR_ARGB565: 
    case CLR_ARGB666: 
            if( const_alp != 255){
                cur_alpbld_type = 2;
                alpha_blending( src_clr_mode, const_alp, cur_alpbld_type, src, &src_tmp);
            }
            else{
                color_copy( &src_tmp, src);
            }
            cur_alpbld_type = 0;
            alpha_blending( src_clr_mode, const_alp, cur_alpbld_type, &src_tmp, dst);
            break;
    default: 
        cur_alpbld_type = 0;
        alpha_blending( src_clr_mode, const_alp, cur_alpbld_type, src, dst);
        break;
    }
}
