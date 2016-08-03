#ifndef BITBLT_H
#define BITBLT_H

#include <stdio.h>
#include <stdlib.h>

#include "g2d_drv_c_model.h"
//#include "define.h"
#include "share_func.h"

void bitblt_sad(G2D_REG* reg);

void alpbld_twophase( int src_clr_mode, UINT8 const_alp, Pixel* src, Pixel* dst);
void alpha_blending( int src_clr_mode, UINT8 const_alp, UINT8 alpbld_type, Pixel* src, Pixel* dst);
void sad( int clr_mode, Pixel* src,Pixel* dst, UINT32* sad_acc, int* sad_en);

#endif

