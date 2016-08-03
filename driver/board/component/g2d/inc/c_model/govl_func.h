#ifndef GOVL_FUNC_H
#define GOVL_FUNC_H

#include <stdio.h>
#include <stdlib.h>

#include "g2d_drv_c_model.h"
//#include "define.h"
#include "bitblt.h"
#include "share_func.h"


void govl_top(G2D_REG* reg);

void govl_process_roi(layer_config*  pLayer_conf, roi_config* pROI_conf);

void process_pixel(layer_config*  pLayer_conf, 
                   roi_config*    pROI_conf, 
                   int            roi_x, 
                   int            roi_y,
                   Pixel*         color_out,
                   int*           skip
                   );

int  calc_layer_pos(layer_config* pLayer_conf, 
                    int  roi_x, 
                    int  roi_y,
                    int* layer_x,
                    int* layer_y
                    );

//SAD related
void sad_top(G2D_REG* reg);
void process_sad(layer_config*  pLayer_conf, roi_config* pROI_conf, g2d_config* pG2D_conf);

#endif

