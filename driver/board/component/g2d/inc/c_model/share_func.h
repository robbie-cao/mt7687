#ifndef SHARE_FUNC_H
#define SHARE_FUNC_H

#include <stdio.h>
#include <stdlib.h>
//#include "define.h"
#include "g2d_drv_c_model.h"

void color_set( Pixel* clr, int a, int r, int g, int b);
void color_shiftright( Pixel* dst, Pixel* src, int shift_amount);
void color_shiftleft ( Pixel* dst, Pixel* src, int shift_amount);
void color_rgb565_to_rgb888( UINT16 rgb565_data, Pixel* clr);
void color_argb6666_to_argb8888( UINT32 data, Pixel* clr);

int  color_cmp( Pixel* clr0, Pixel* clr1);
int  srckey_hit_hw(int src_key_en, int clr_mode, Pixel* clr, Pixel* src_key);
int  dst_avo_match(int clr_rep_en, int clr_mode, Pixel* dst, Pixel* dst_avo_clr);
int  outside_draw_region( 
                         int   dst_cur_x, 
                         int   dst_cur_y, 
                         UINT8 clip_en,
                         int   clp_min_x,
                         int   clp_min_y,
                         int   clp_max_x,
                         int   clp_max_y);


void color_copy( Pixel* dst, Pixel* src);
void getcolor( UINT8*  data_ptr, 
               int clr_mode, 
               int ch_swap, 
               int sizeX, 
               int x_i, 
               int y_i,
               Pixel* clr);

void get_font_idx( UINT8*  data_ptr, 
                   int     pitch,
                   int     idx,
                   int     x_i,
                   int     y_i,
                   UINT32* idx_data
             );

/*
void getcolor_withidx( UINT8*  data_ptr, 
               UINT8*  idx_ptr, 
               int     clr_mode, 
               int     rb_swap, 
               int     byte_inv,
               int     bidx_en, 
               int     bidx, 
               int     sizeX, 
               int     x_i, 
               int     y_i, 
               Pixel*  clr);
*/

void writecolor( UINT8* data_ptr, int clr_mode, int ch_swap, int sizeX, int x_i, int y_i, Pixel* clr);
void color_convert_to_pixel( int clr_mode, int ch_swap, UINT32 data32, Pixel* clr);
void pixel_convert_to_565(int dst_clr_mode, Pixel* clr);

void Reg_to_G2DCONF( G2D_REG* reg, g2d_config* g2d_conf);
void Reg_to_ROICONF(G2D_REG* reg, roi_config* pROI);
void Reg_to_LAYER( UINT32 con,
                  UINT32 clr_key,
                  UINT32 addr,
                  UINT32 pitch,
                  UINT32 size,
                  UINT32 ofs,
                  //UINT32 mem_ofs,
                  layer_config* pLayer);
void swap_rb_channel(Pixel* clr);
void yuv422_to_rgb888(UINT32 yuvdata, Pixel* clr);

void Reg_to_Detail( G2D_REG* reg, G2D_REG_DETAIL *detail);
int  getMWC_noise(MWCData* mwc);
int  gen_first_MWC(MWCData* mwc);
void dither_one_ch (int* data, int noise, int di_bit);
int  getFIX_noise(int x, int y);

#endif
