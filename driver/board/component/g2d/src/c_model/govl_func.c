#include "govl_func.h"

void govl_top(G2D_REG* reg)
{
    layer_config pLayer_conf[4];
    roi_config   pROI_conf;

    Reg_to_ROICONF(reg, &pROI_conf);
    Reg_to_LAYER(reg->G2D_L0_CON, 
                 reg->G2D_L0_CLRKEY,
                 reg->G2D_L0_ADDR,
                 reg->G2D_L0_PITCH,
                 reg->G2D_L0_SIZE,
                 reg->G2D_L0_OFS,
                 //reg->G2D_L0_MOFS,
                 &pLayer_conf[0]);
    Reg_to_LAYER(reg->G2D_L1_CON, 
        reg->G2D_L1_CLRKEY,
        reg->G2D_L1_ADDR,
        reg->G2D_L1_PITCH,
        reg->G2D_L1_SIZE,
        reg->G2D_L1_OFS,
        //reg->G2D_L1_MOFS,
        &pLayer_conf[1]);
    Reg_to_LAYER(reg->G2D_L2_CON, 
        reg->G2D_L2_CLRKEY,
        reg->G2D_L2_ADDR,
        reg->G2D_L2_PITCH,
        reg->G2D_L2_SIZE,
        reg->G2D_L2_OFS,
        //reg->G2D_L2_MOFS,
        &pLayer_conf[2]);
    Reg_to_LAYER(reg->G2D_L3_CON, 
        reg->G2D_L3_CLRKEY,
        reg->G2D_L3_ADDR,
        reg->G2D_L3_PITCH,
        reg->G2D_L3_SIZE,
        reg->G2D_L3_OFS,
        //reg->G2D_L3_MOFS,
        &pLayer_conf[3]);

    govl_process_roi(pLayer_conf, &pROI_conf);

}

void govl_process_roi(layer_config*  pLayer_conf, roi_config* pROI_conf)
{
    int   i;
    int   pixel_num, skip;
    int   cur_roi_x, cur_roi_y;
    int   local_x, local_y;
    int   abs_x, abs_y, wr_x, wr_y;
    Pixel   ovl_pixel, avo_color, rep_color;
    MWCData mwc;
    int     noise;
    
    mwc.m_w = pROI_conf->mwc_mw;
    mwc.m_z = pROI_conf->mwc_mz;
    mwc.valid_bits = 0;

    color_convert_to_pixel(pROI_conf->clrfmt, pROI_conf->ch_swap, pROI_conf->avo_clr, &avo_color);
    color_convert_to_pixel(pROI_conf->clrfmt, pROI_conf->ch_swap, pROI_conf->rep_clr, &rep_color);

    pixel_num = pROI_conf->width * pROI_conf->height;
    gen_first_MWC(&mwc);
    local_x = 0;
    local_y = 0;
    for(i=0; i < pixel_num; i++){
        cur_roi_x = local_x + pROI_conf->ofs_x;
        cur_roi_y = local_y + pROI_conf->ofs_y;
        process_pixel( pLayer_conf, pROI_conf, cur_roi_x, cur_roi_y, &ovl_pixel, &skip);

        abs_x = local_x + pROI_conf->ofs_x;
        abs_y = local_y + pROI_conf->ofs_y;
        wr_x  = local_x + pROI_conf->mofs_x;
        wr_y  = local_y + pROI_conf->mofs_y;
        if((wr_x < 0) || (wr_y < 0) || (wr_x > 2047) || (wr_y > 2047)){
                skip |= 1;
        }
        skip |= outside_draw_region( 
            abs_x, 
            abs_y, 
            pROI_conf->clip_en,
            pROI_conf->clip_min_x,
            pROI_conf->clip_min_y,
            pROI_conf->clip_max_x,
            pROI_conf->clip_max_y
            );

        if(skip == 0){
            // Stage 1: Dithering
            if (pROI_conf->dither_mode != 0){
                if (pROI_conf->dither_mode == 1){
                    noise = getMWC_noise(&mwc);
                } else {
                    noise = getFIX_noise(wr_x, wr_y);
                }
                dither_one_ch ( &(ovl_pixel.r), noise, pROI_conf->di_rbit);
                dither_one_ch ( &(ovl_pixel.g), noise, pROI_conf->di_gbit);
                dither_one_ch ( &(ovl_pixel.b), noise, pROI_conf->di_bbit);
            }

            // Stage 2: Output alpha replacement
            if(pROI_conf->out_alpha_en){
                if( (pROI_conf->clrfmt == CLR_PARGB666) ||
                    (pROI_conf->clrfmt == CLR_ARGB666) )
                {
                    ovl_pixel.a = ((pROI_conf->out_alpha & 0x3f) << 2);       
                }
                else{
                    ovl_pixel.a = pROI_conf->out_alpha;
                }
            }

            // Stage 3: Output color replacement
            if (dst_avo_match(pROI_conf->clr_rep_en, pROI_conf->clrfmt, &ovl_pixel, &avo_color) == 1){
                color_copy( &ovl_pixel, &rep_color);
            }

            writecolor((UINT8*)pROI_conf->w2m_addr,
                pROI_conf->clrfmt,
                pROI_conf->ch_swap,
                pROI_conf->w2m_pitch,
                wr_x,
                wr_y,
                &ovl_pixel);
#ifdef DEBUG
            G2D_LOGI("wr(%d, %d) => (%02x, %02x, %02x, %02x)\n", wr_x, wr_y,
                ovl_pixel.a,
                ovl_pixel.r,
                ovl_pixel.g,
                ovl_pixel.b);
#endif
        }

        // Calculate next coordinate
        if (pROI_conf->tile_en == 1){
            if ((local_x == (pROI_conf->width - 1)) ||
                ((pROI_conf->tile_size == 0) && ((local_x & 0x3) == 0x3)) || 
                ((pROI_conf->tile_size == 1) && ((local_x & 0x7) == 0x7))
            ){
                if (local_y == (pROI_conf->height - 1)){
                    local_y = 0;
                    local_x++;
                }
                else{
                    if (pROI_conf->tile_size == 1)
                        local_x &= 0xfffffff8;
                    else
                        local_x &= 0xfffffffc;
                    local_y++;
                }
            }
            else{
                local_x++;
            }
        }
        else{
            if (local_x == (pROI_conf->width - 1) ){
                local_x = 0;
                local_y++;
            }
            else{
                local_x++;
            }
        }
    }
}

void process_pixel(layer_config*  pLayer_conf, 
                   roi_config*    pROI_conf, 
                   int            roi_x, 
                   int            roi_y,
                   Pixel*         color_out,
                   int*           skip
                   )
{
    int i, layer_en_cnt;
    int layer_x[4];
    int layer_y[4];
    char layer_en[4];
    char pos_hit[4];
    char hit_layer[4];
    char l1_not_cover, l2_not_cover, l3_not_cover;
    char color_skey_hit;
    UINT32 idx_data;
    int  font_skip;
    int  pixel_is_modified;

    Pixel src_color;
    Pixel dst_color;
    Pixel key_color;
    Pixel bg_color;
//    Pixel avo_color;
//    Pixel rep_color;

    //Find hitten layer and layer local coordinate
    layer_en_cnt = 0;
    for(i=0; i<4; i++){
        layer_en[i] = (pROI_conf->layer_en >> (3-i) ) & 1;
        layer_en_cnt += layer_en[i];
        pos_hit[i] = calc_layer_pos( &pLayer_conf[i],roi_x, roi_y, &layer_x[i], &layer_y[i]) & layer_en[i];
    }

    l1_not_cover = (pos_hit[1] == 0) || (pLayer_conf[1].src_key_en == 1) || (pLayer_conf[1].alpha_en == 1) || (pLayer_conf[1].font_en == 1);
    l2_not_cover = (pos_hit[2] == 0) || (pLayer_conf[2].src_key_en == 1) || (pLayer_conf[2].alpha_en == 1) || (pLayer_conf[2].font_en == 1);
    l3_not_cover = (pos_hit[3] == 0) || (pLayer_conf[3].src_key_en == 1) || (pLayer_conf[3].alpha_en == 1) || (pLayer_conf[3].font_en == 1);
    
    hit_layer[0] = pos_hit[0] & l1_not_cover & l2_not_cover & l3_not_cover;
    hit_layer[1] = pos_hit[1] & l2_not_cover & l3_not_cover;
    hit_layer[2] = pos_hit[2] & l3_not_cover;
    hit_layer[3] = pos_hit[3];
    
    color_convert_to_pixel(CLR_PARGB8888, 0, pROI_conf->bgclr, &bg_color);
    //color_convert_to_pixel(pROI_conf->clrfmt, pROI_conf->ch_swap, pROI_conf->avo_clr, &avo_color);
    //color_convert_to_pixel(pROI_conf->clrfmt, pROI_conf->ch_swap, pROI_conf->rep_clr, &rep_color);
    color_copy(&dst_color, &bg_color);

    *skip = 0;
    pixel_is_modified = 0;
    for(i=0; i<4; i++){
        font_skip = 0;
        if(hit_layer[i] == 1){
            color_convert_to_pixel( pLayer_conf[i].clrfmt, 
                pLayer_conf[i].ch_swap,
                pLayer_conf[i].src_key, 
                &key_color);
            
            if(pLayer_conf[i].rect_en == 1){
                color_copy(&src_color, &key_color);
            }
            else if(pLayer_conf[i].font_en == 1){
                get_font_idx( (UINT8*)pLayer_conf[i].addr, 
                    pLayer_conf[i].pitch,
                    pLayer_conf[i].idx,
                    layer_x[i], //+ pLayer_conf[i].mem_ofs_x,
                    layer_y[i], //+ pLayer_conf[i].mem_ofs_y,
                    &idx_data
                    );
                if( pLayer_conf[i].alpha_en == 1){
                    alpha_blending( pLayer_conf[i].clrfmt, 
                        idx_data, 2, 
                        &key_color, 
                        &src_color);
                }
                else{
                    if (idx_data == 0){
                        font_skip = 1;
                    }
                    color_copy(&src_color, &key_color);
                }
            }
            else{
                getcolor( (UINT8*)(pLayer_conf[i].addr),
                      pLayer_conf[i].clrfmt,
                      pLayer_conf[i].ch_swap, 
                      pLayer_conf[i].pitch, 
                      layer_x[i], //+ pLayer_conf[i].mem_ofs_x, 
                      layer_y[i], //+ pLayer_conf[i].mem_ofs_y, 
                      &src_color);
            }
#ifdef DEBUG
            G2D_LOGI("L[%d](%d, %d) src_color = (%02x, %02x ,%02x, %02x)\n", i, layer_x[i], layer_y[i], 
                src_color.a, 
                src_color.r,
                src_color.g,
                src_color.b);
#endif

            color_skey_hit = srckey_hit_hw(pLayer_conf[i].src_key_en, 
                                           pLayer_conf[i].clrfmt,
                                           &src_color,
                                           &key_color);
                              
#ifdef DEBUG
            if(color_skey_hit == 1){
                G2D_LOGI("Source key hit!!\n");
            }
#endif
            if(pLayer_conf[i].src_key_en == 1){
                if( color_skey_hit == 0){
                    if(pLayer_conf[i].alpha_en){
                        alpbld_twophase( pLayer_conf[i].clrfmt, pLayer_conf[i].alpha, &src_color, &dst_color);
                    }   
                    else{
                        color_copy(&dst_color, &src_color);
                    }
                    pixel_is_modified |= 1;
                }
            }
            else{
                if(pLayer_conf[i].alpha_en){
                    alpbld_twophase( pLayer_conf[i].clrfmt, pLayer_conf[i].alpha, &src_color, &dst_color);
                    pixel_is_modified |= 1;
                }   
                else{
                    if (font_skip == 0){
                        color_copy(&dst_color, &src_color);
                        pixel_is_modified |= 1;
                    }
                }
            }
        }
    }

    if ( (pROI_conf->dis_bg == 1) && 
         (pixel_is_modified == 0)
       )
    {
        *skip = 1;
    }
    color_copy( color_out, &dst_color);

/*
    if(pROI_conf->out_alpha_en){
        if( (pROI_conf->clrfmt == CLR_PARGB666) ||
            (pROI_conf->clrfmt == CLR_ARGB666) )
        {
            dst_color.a = ((pROI_conf->out_alpha & 0x3f) << 2);       
        }
        else{
            dst_color.a = pROI_conf->out_alpha;
        }
    }

    if (dst_avo_match(pROI_conf->clr_rep_en, pROI_conf->clrfmt, &dst_color, &avo_color) == 1){
        color_copy( color_out, &rep_color);
    }
    else{
        color_copy( color_out, &dst_color);
    }
*/
}   
    
int calc_layer_pos(layer_config* pLayer_conf, 
                    int  roi_x, 
                    int  roi_y,
                    int* layer_x,
                    int* layer_y
                    )
{
    int  swp_xy, dx, dy;
    int  vec_x, vec_y;
    
    dy     = (pLayer_conf->rotate >> 2) & 1;
    dx     = (pLayer_conf->rotate >> 1) & 1;
    swp_xy = pLayer_conf->rotate & 1;
    
    dx = (dx << 1) - 1; dx *= -1;
    dy = (dy << 1) - 1; dy *= -1;
    
    vec_x = roi_x - pLayer_conf->ofs_x;
    vec_y = roi_y - pLayer_conf->ofs_y;

    *layer_x = swp_xy ? vec_y : vec_x;
    *layer_y = swp_xy ? vec_x : vec_y;
    *layer_x *= dx;
    *layer_y *= dy;
    
    if( (*layer_x >= 0) && (*layer_x < pLayer_conf->width) &&
        (*layer_y >= 0) && (*layer_y < pLayer_conf->height)
        )
        return 1;
    else
        return 0;
}

void sad_top(G2D_REG* reg)
{
    layer_config pLayer_conf[2];
    roi_config   pROI_conf;
    g2d_config   pG2D_conf;

    Reg_to_ROICONF(reg, &pROI_conf);
    Reg_to_LAYER(reg->G2D_L0_CON, 
        reg->G2D_L0_CLRKEY,
        reg->G2D_L0_ADDR,
        reg->G2D_L0_PITCH,
        reg->G2D_L0_SIZE,
        reg->G2D_L0_OFS,
        //reg->G2D_L0_MOFS,
        &pLayer_conf[0]);
    Reg_to_LAYER(reg->G2D_L1_CON, 
        reg->G2D_L1_CLRKEY,
        reg->G2D_L1_ADDR,
        reg->G2D_L1_PITCH,
        reg->G2D_L1_SIZE,
        reg->G2D_L1_OFS,
        //reg->G2D_L1_MOFS,
        &pLayer_conf[1]);
    Reg_to_G2DCONF(reg, &pG2D_conf);

    process_sad( pLayer_conf, &pROI_conf, &pG2D_conf);
    reg->G2D_SAD_SUM = pG2D_conf.sad_acc;
}

void process_sad(layer_config*  pLayer_conf, roi_config* pROI_conf, g2d_config* pG2D_conf)
{
    int   i, pixel_num;
    int   cur_roi_x, cur_roi_y;
    int   local_x, local_y;
    int   layer_x[2];
    int   layer_y[2];
    char  pos_hit[2];
    char  hit_layer[2];
    char  l1_not_cover;
    Pixel dst_color, src_color;
    int   sad_ch_en[4];
    UINT32 sad_acc;


    pixel_num = pROI_conf->width * pROI_conf->height;
    local_x = 0;
    local_y = 0;

    if (pROI_conf->layer_en != 0xc){
        ///G2D_LOGI("Error: Illegal setting for SAD, layer_en should be 0xc!\n");
        ///exit(1);
    }
    sad_acc = 0;
    for(i=0; i < pixel_num; i++){
        cur_roi_x = local_x + pROI_conf->ofs_x;
        cur_roi_y = local_y + pROI_conf->ofs_y;
        
        //process_pixel( pLayer_conf, pROI_conf, cur_roi_x, cur_roi_y, &ovl_pixel, &skip);
        pos_hit[0] = calc_layer_pos(&pLayer_conf[0],cur_roi_x, cur_roi_y, &layer_x[0], &layer_y[0]);
        pos_hit[1] = calc_layer_pos(&pLayer_conf[1],cur_roi_x, cur_roi_y, &layer_x[1], &layer_y[1]);

        l1_not_cover = (pos_hit[1] == 0) || (pLayer_conf[1].src_key_en == 1) || (pLayer_conf[1].alpha_en == 1) || (pLayer_conf[1].font_en == 1);
        hit_layer[0] = pos_hit[0] & l1_not_cover;
        hit_layer[1] = pos_hit[1];

        if((hit_layer[0] == 0) || (hit_layer[1] == 0)){
            ///G2D_LOGI("Error: SAD window range setting error, both of src and dst should be covered!\n");
            ///exit(1);
        }

        getcolor( (UINT8*)(pLayer_conf[0].addr + pG2D_conf->sad_dofs),
            pLayer_conf[0].clrfmt,
            pLayer_conf[0].ch_swap, 
            pLayer_conf[0].pitch, 
            layer_x[0],
            layer_y[0],
            &dst_color);
        getcolor( (UINT8*)(pLayer_conf[1].addr + pG2D_conf->sad_sofs),
            pLayer_conf[1].clrfmt,
            pLayer_conf[1].ch_swap, 
            pLayer_conf[1].pitch, 
            layer_x[1],
            layer_y[1],
            &src_color);

#ifdef DEBUG
        G2D_LOGI("L[%d](%d, %d) src_color = (%02x, %02x ,%02x, %02x)\n", 0, layer_x[0], layer_y[0], 
            dst_color.a, 
            dst_color.r,
            dst_color.g,
            dst_color.b);
        G2D_LOGI("L[%d](%d, %d) src_color = (%02x, %02x ,%02x, %02x)\n", 1, layer_x[1], layer_y[1], 
            src_color.a, 
            src_color.r,
            src_color.g,
            src_color.b);
#endif

        if(local_x == (pROI_conf->width - 1)){
            switch( pG2D_conf->sad_mod ) {
                case 0: sad_ch_en[3] = 1; sad_ch_en[2] = 1; sad_ch_en[1] = 1; sad_ch_en[0] = 1; break;
                case 1: sad_ch_en[3] = 0; sad_ch_en[2] = 0; sad_ch_en[1] = 0; sad_ch_en[0] = 1; break;
                case 2: sad_ch_en[3] = 0; sad_ch_en[2] = 0; sad_ch_en[1] = 1; sad_ch_en[0] = 1; break;
                case 3: sad_ch_en[3] = 0; sad_ch_en[2] = 1; sad_ch_en[1] = 1; sad_ch_en[0] = 1; break;
                default: 
                	///G2D_LOGI("Error: Illegal sad_mod setting!! \n"); exit(1); 
                break;
            }
        }
        else{
            sad_ch_en[3] = 1; sad_ch_en[2] = 1; sad_ch_en[1] = 1; sad_ch_en[0] = 1;
        }
        sad( CLR_PARGB8888, &src_color, &dst_color, &sad_acc, sad_ch_en);       

        if (local_x == (pROI_conf->width - 1) ){
            local_x = 0;
            local_y++;
        }
        else{
            local_x++;
        }
    }

    ///G2D_LOGI("SW SAD result = %d\n", sad_acc);
    
    pG2D_conf->sad_acc = sad_acc;
}

