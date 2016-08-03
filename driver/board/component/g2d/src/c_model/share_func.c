#include "share_func.h"


void color_set( Pixel* clr, int a, int r, int g, int b)
{
    clr->a = a;
    clr->r = r;
    clr->g = g;
    clr->b = b;
}

void color_shiftright( Pixel* dst, Pixel* src, int shift_amount)
{
    dst->a = src->a >> shift_amount;
    dst->r = src->r >> shift_amount;
    dst->g = src->g >> shift_amount;
    dst->b = src->b >> shift_amount;
}

void color_shiftleft ( Pixel* dst, Pixel* src, int shift_amount)
{
    dst->a = src->a << shift_amount;
    dst->r = src->r << shift_amount;
    dst->g = src->g << shift_amount;
    dst->b = src->b << shift_amount;
}


void color_rgb565_to_rgb888( UINT16 rgb565_data, Pixel* clr)
{
    clr->a = 0x00ff;
    clr->r = (rgb565_data >> 11) & 0x1f;
    clr->g = (rgb565_data >> 5 ) & 0x3f;
    clr->b = (rgb565_data & 0x1f);
    clr->r = (clr->r << 3) | (clr->r >> 2);
    clr->g = (clr->g << 2) | (clr->g >> 4);
    clr->b = (clr->b << 3) | (clr->b >> 2);
}

void color_argb6666_to_argb8888( UINT32 data, Pixel* clr)
{
    clr->a = (data >> 18) & 0x3f;
    clr->r = (data >> 12) & 0x3f;
    clr->g = (data >>  6) & 0x3f;
    clr->b = (data >>  0) & 0x3f;

    clr->a = (clr->a << 2) | (clr->a >> 4);
    clr->r = (clr->r << 2) | (clr->r >> 4);
    clr->g = (clr->g << 2) | (clr->g >> 4);
    clr->b = (clr->b << 2) | (clr->b >> 4);
}

void color_convert_to_pixel( int clr_mode, int ch_swap, UINT32 data32, Pixel* clr)
{
    int data8;
    int data16;
    int tmp;

    data8  = data32 & 0x000000ff;
    data16 = data32 & 0x0000ffff;
    switch( clr_mode ){
        case CLR_8BPP: 
            color_set( clr, 0xff, data8, data8, data8);
            break;
        case CLR_RGB565:
            color_rgb565_to_rgb888( data16, clr);
            break;
        case CLR_RGB888:
            clr->a = 0xff;
            clr->r = (data32 >> 16) & 0xff;
            clr->g = (data32 >>  8) & 0xff;
            clr->b = (data32 >>  0) & 0xff;
            if (ch_swap == 1){
                tmp = clr->r;
                clr->r = clr->b;
                clr->b = tmp;
            }
            break;
        case CLR_ARGB8888:
        case CLR_PARGB8888:
            clr->a = (data32 >> 24) & 0xff;
            clr->r = (data32 >> 16) & 0xff;
            clr->g = (data32 >>  8) & 0xff;
            clr->b = (data32 >>  0) & 0xff;
            if (ch_swap == 1){
                tmp = clr->r;
                clr->r = clr->b;
                clr->b = tmp;
            }
            break;
        case CLR_ARGB565:
        case CLR_PARGB565:
            color_rgb565_to_rgb888( data16, clr);
            clr->a = (data32 >> 16) & 0xff;
            break;
        case CLR_ARGB666:
        case CLR_PARGB666:
            color_argb6666_to_argb8888( data32, clr);
            break;
        case CLR_YUYV422:
            //clr->a = 0;
            //clr->r = 0;
            //clr->g = 0;
            //clr->b = 0;
            yuv422_to_rgb888(data32, clr);
            break;
        default: 
            ///G2D_LOGI( "Illegal color format = %d!!\n", clr_mode);
            ///G2D_LOGI( "   -func: color_convert_to_pixel");
            ///exit(1);
            break;
    }
}

int  color_cmp( Pixel* clr0, Pixel* clr1)
{
    if( clr0->a == clr1->a &&
        clr0->r == clr1->r &&
        clr0->g == clr1->g &&
        clr0->b == clr1->b ){
        return 1;
    }
    else{
        return 0;
    }
    
}



void getcolor( UINT8* data_ptr, 
               int clr_mode, 
               int ch_swap, 
               int sizeX, 
               int x_i, 
               int y_i, 
               Pixel* clr)
{
    UINT8*  ptr8;
    UINT8   data8;
    UINT16  data16_l, data16_h, data16;
    UINT32  data32;
    int     BytePerPixel = 0;
    UINT32  Addr_offset;
    UINT32  yuvdata;
    int     tmp;
    int     y_0, u, y_1, v;
    
    switch(clr_mode){
        case CLR_8BPP:      BytePerPixel = 1; break;
        case CLR_RGB565:    BytePerPixel = 2; break;
        case CLR_YUYV422:   BytePerPixel = 2; break;
        case CLR_ARGB8888:  BytePerPixel = 4; break;
        case CLR_PARGB8888: BytePerPixel = 4; break;

        case CLR_RGB888:    BytePerPixel = 3; break;
        case CLR_ARGB565:   BytePerPixel = 3; break;
        case CLR_PARGB565:  BytePerPixel = 3; break;
        case CLR_ARGB666 :  BytePerPixel = 3; break;
        case CLR_PARGB666:  BytePerPixel = 3; break;
    }

    Addr_offset = (y_i * sizeX) + (x_i * BytePerPixel);
    ptr8 = (UINT8*)(data_ptr + Addr_offset);

    switch( clr_mode )
    {
    case CLR_8BPP:
        data8 = *(ptr8);
        clr->a = 0xff;
        clr->r = clr->g = clr->b = data8;
        break;
    case CLR_RGB888:
        clr->a = 0xff;
        clr->r = *(ptr8 + 2);
        clr->g = *(ptr8 + 1);
        clr->b = *(ptr8 + 0);
        data32 = 0xff000000 | (clr->r << 16) | (clr->g << 8) | clr->b;
        if(ch_swap == 1){
            tmp = clr->r;
            clr->r = clr->b;
            clr->b = tmp;
        }
        break;
    case CLR_RGB565:
        data16_l = *(ptr8 + 0);
        data16_h = *(ptr8 + 1);
        data16 = (data16_l & 0xff) | ((data16_h & 0xff) << 8);
        color_rgb565_to_rgb888( data16, clr);
        break;
    case CLR_ARGB8888:
    case CLR_PARGB8888:
        clr->a = *(ptr8 + 3);
        clr->r = *(ptr8 + 2);
        clr->g = *(ptr8 + 1);
        clr->b = *(ptr8 + 0);
        data32 = (clr->a << 24) | (clr->r << 16) | (clr->g << 8) | clr->b;
        if(ch_swap == 1){
            tmp = clr->r;
            clr->r = clr->b;
            clr->b = tmp;
        }
        break;
    case CLR_ARGB565 :
    case CLR_PARGB565:
        data16_l = *(ptr8 + 0);
        data16_h = *(ptr8 + 1);
        data16 = (data16_l & 0xff) | ((data16_h & 0xff) << 8);
        color_rgb565_to_rgb888( data16, clr);
        clr->a   = *(ptr8 + 2);
        break;
    case CLR_ARGB666 :
    case CLR_PARGB666:
        data32 = *(ptr8 + 2);
        data32 <<= 8;
        data32 |= *(ptr8 + 1);
        data32 <<= 8;
        data32 |= *(ptr8 + 0);
        color_argb6666_to_argb8888(data32, clr);
        break;
    case CLR_YUYV422:
        if(x_i & 1){   //odd pixel
            ptr8 = (UINT8*)(data_ptr + Addr_offset - 2);
        }
        if( ch_swap == 1){
            y_0 = *(ptr8 + 0);
            u   = *(ptr8 + 1);
            y_1 = *(ptr8 + 2);
            v   = *(ptr8 + 3);
        }
        else{
            u   = *(ptr8 + 0);
            y_0 = *(ptr8 + 1);
            v   = *(ptr8 + 2);
            y_1 = *(ptr8 + 3);
        }
        if(x_i & 1){
            yuvdata = y_1 | (u << 8) | (v << 16);
        }
        else{
            yuvdata = y_0 | (u << 8) | (v << 16);
        }
#ifdef DEBUG
        G2D_LOGI("  -yuvdata = 0x%6.6x\n", yuvdata);
#endif
        yuv422_to_rgb888(yuvdata, clr);
        break;
    default: 
        ///G2D_LOGI("Error: Illegal color format!!\n");
        ///exit(1);
        break;
    }
}


void get_font_idx( UINT8*  data_ptr, 
                  int     pitch,
                  int     idx,
                  int     x_i,
                  int     y_i,
                  UINT32* idx_data
                  )
{
    UINT32 idx_bit_addr, idx_byte_addr, idx_bit_ofs;
    UINT8  idx_rdata;

    idx_bit_addr = x_i + y_i * pitch;
    switch( idx ){
    case 0: 
        idx_byte_addr = idx_bit_addr >> 3; 
        idx_bit_ofs   = idx_bit_addr & 7; 
        idx_rdata = *(data_ptr + idx_byte_addr);
        idx_rdata = (idx_rdata >> idx_bit_ofs) & 0x1;
        *idx_data = (idx_rdata == 1) ? 0xff : 0;        
        break;
    case 1: 
        idx_byte_addr = idx_bit_addr >> 2; 
        idx_bit_ofs   = idx_bit_addr & 3; 
        idx_rdata = *(data_ptr + idx_byte_addr);
        idx_rdata = (idx_rdata >> (idx_bit_ofs << 1)) & 0x3;
        *idx_data = (idx_rdata << 6) | (idx_rdata << 4) | (idx_rdata << 2) | idx_rdata;
        break;
    case 2: 
        idx_byte_addr = idx_bit_addr >> 1; 
        idx_bit_ofs = idx_bit_addr & 1; 
        idx_rdata = *(data_ptr + idx_byte_addr);
        idx_rdata = (idx_rdata >> (idx_bit_ofs << 2)) & 0xf;
        *idx_data = (idx_rdata << 4) | idx_rdata;
        break;
    case 3: 
        idx_byte_addr = idx_bit_addr >> 0; 
        idx_bit_ofs = 0; 
        idx_rdata = *(data_ptr + idx_byte_addr);
        idx_rdata = idx_rdata & 0xff;
        *idx_data = idx_rdata;
        break;
    }
}

void yuv422_to_rgb888(UINT32 yuvdata, Pixel* clr)
{
    int y, u, v, r, g, b;
    y = yuvdata & 0xff;
    u = (yuvdata >> 8) & 0xff;
    v = (yuvdata >>16) & 0xff;

    r = (32*y + 45*(v-128)) / 32;
    g = (32*y - 11*(u-128) - 23*(v-128)) / 32;
    b = (32*y + 57*(u-128)) / 32;

    if      (r > 255) r = 255;
    else if (r < 0)   r = 0;

    if      (g > 255) g = 255;
    else if (g < 0)   g = 0;

    if      (b > 255) b = 255;
    else if (b < 0)   b = 0;
    
    clr->a = 0xff;
    clr->r = r;
    clr->g = g;
    clr->b = b;
}

void writecolor( UINT8* data_ptr, int clr_mode, int ch_swap, int sizeX, int x_i, int y_i, Pixel* clr)
{
    UINT8*  ptr8;
    UINT16  data16;
    UINT32  data32;
    int     BytePerPixel = 0;
    UINT32  Addr_offset;
    
    switch(clr_mode){
        case CLR_8BPP:      BytePerPixel = 1; break;
        case CLR_RGB565:    BytePerPixel = 2; break;
        case CLR_ARGB8888:  BytePerPixel = 4; break;
        case CLR_PARGB8888: BytePerPixel = 4; break;

        case CLR_RGB888:    BytePerPixel = 3; break;
        case CLR_ARGB565:   BytePerPixel = 3; break;
        case CLR_PARGB565:  BytePerPixel = 3; break;
        case CLR_ARGB666:   BytePerPixel = 3; break;
        case CLR_PARGB666:  BytePerPixel = 3; break;
    }

    Addr_offset = (y_i * sizeX) + (x_i * BytePerPixel);
    ptr8 = (UINT8*)(data_ptr + Addr_offset);
#ifdef DEBUG
    G2D_LOGI("  wr_addr = 0x%08x\n", Addr_offset);
#endif
    switch( clr_mode )
    {
    case CLR_8BPP:
        *(ptr8) = clr->b;
        break;
    case CLR_RGB888:
        if(ch_swap == 1){
            *(ptr8 + 2) = clr->b;
            *(ptr8 + 1) = clr->g;
            *(ptr8 + 0) = clr->r;
        }
        else{
            *(ptr8 + 2) = clr->r;
            *(ptr8 + 1) = clr->g;
            *(ptr8 + 0) = clr->b;
        }
        break;
    case CLR_RGB565:
        data16 = ((clr->r & 0xF8)<<8)|((clr->g&0xFC)<<3)|((clr->b&0xF8)>>3);
        *(ptr8 + 0) = data16 & 0xff;
        *(ptr8 + 1) = (data16 >> 8) & 0xff;
        break;
    case CLR_ARGB8888 :
    case CLR_PARGB8888:
        if(ch_swap == 1)
            data32=(clr->a<<24)|(clr->b<<16)|(clr->g<<8)|clr->r;
        else
            data32=(clr->a<<24)|(clr->r<<16)|(clr->g<<8)|clr->b;

        *(ptr8 + 0) = data32 & 0xff;
        *(ptr8 + 1) = (data32>> 8) & 0xff;
        *(ptr8 + 2) = (data32>>16) & 0xff;
        *(ptr8 + 3) = (data32>>24) & 0xff;
        break;
    case CLR_ARGB565:
    case CLR_PARGB565:
        data16 = ((clr->r & 0xF8)<<8)|((clr->g&0xFC)<<3)|((clr->b&0xF8)>>3);
        *(ptr8 + 0) = data16 & 0xff;
        *(ptr8 + 1) = (data16 >> 8) & 0xff;
        *(ptr8 + 2) = clr->a;
        break;
    case CLR_ARGB666:
    case CLR_PARGB666:
        data32 = ((clr->a & 0xfc)<<16) |
                 ((clr->r & 0xfc)<<10) |
                 ((clr->g & 0xfc)<< 4) |
                 ((clr->b & 0xfc)>> 2) ;
        *(ptr8 + 0) = data32 & 0xff;
        *(ptr8 + 1) = (data32>> 8) & 0xff;
        *(ptr8 + 2) = (data32>>16) & 0xff;
        break;
    default:
        ///G2D_LOGI( "Error: Illegal color format = %d!!\n", clr_mode);
        ///G2D_LOGI( "  -func: write_color\n");
        ///exit(1);
        break;
    }


}


void color_copy( Pixel* dst, Pixel* src)
{
    dst->a = src->a;
    dst->r = src->r;
    dst->g = src->g;
    dst->b = src->b;
}



int  srckey_hit_hw(int src_key_en, int clr_mode, Pixel* clr, Pixel* src_key)
{
    if( src_key_en == 0)
        return 0;

    switch( clr_mode )
    {
        case CLR_8BPP: 
            if(clr->b == src_key->b)
                return 1;
            else
                return 0;
        case CLR_RGB565:
            if( ((clr->r & 0xf8) == (src_key->r & 0xf8) ) &&
                ((clr->g & 0xfc) == (src_key->g & 0xfc) ) &&
                ((clr->b & 0xf8) == (src_key->b & 0xf8) ) )
                return 1;
            else 
                return 0;
        case CLR_YUYV422:
            return 0;
        case CLR_RGB888:
            if( (clr->b == src_key->b) && 
                (clr->g == src_key->g) && 
                (clr->r == src_key->r) ) 
                return 1;
            else
                return 0;
        case CLR_ARGB8888:
        case CLR_PARGB8888:
            if( (clr->b == src_key->b) && 
                (clr->g == src_key->g) && 
                (clr->r == src_key->r) && 
                (clr->a == src_key->a) ) 
                return 1;
            else
                return 0;
        case CLR_ARGB565:
        case CLR_PARGB565:
            if( ((clr->a & 0xff) == (src_key->a & 0xff) ) &&
                ((clr->r & 0xf8) == (src_key->r & 0xf8) ) &&
                ((clr->g & 0xfc) == (src_key->g & 0xfc) ) &&
                ((clr->b & 0xf8) == (src_key->b & 0xf8) ) )
                return 1;
            else 
                return 0;
        case CLR_ARGB666:
        case CLR_PARGB666:
            if( ((clr->a & 0xfc) == (src_key->a & 0xfc) ) &&
                ((clr->r & 0xfc) == (src_key->r & 0xfc) ) &&
                ((clr->g & 0xfc) == (src_key->g & 0xfc) ) &&
                ((clr->b & 0xfc) == (src_key->b & 0xfc) ) )
                return 1;
            else 
                return 0;
        //default:
            //G2D_LOGI("Error: Illegal color mode in src key compare!\n");
            ///exit(1);
    }
	return 0;	//Dream: default return;
    //if( (src_key_en==1) && (clr->b==src_key->b) && 
    //  ( (clr_mode==CLR_8BPP) || 
    //    ( ((clr->a==src_key->a) || (clr_mode!=CLR_ARGB8888)) &&
    //      (clr->r==src_key->r) && (clr->g==src_key->g)))){
    //          return 1;
    //}
    //else
    //    return 0;
}

int  dst_avo_match(int clr_rep_en, int clr_mode, Pixel* dst, Pixel* dst_avo_clr)
{
    if( clr_rep_en == 0)
        return 0;
    switch( clr_mode){
        case CLR_8BPP:      //compare blue channel only
            if ( dst->b == dst_avo_clr->b)
                return 1;
            break;
        case CLR_RGB565:
            if(  ((dst->r & 0xf8) == (dst_avo_clr->r & 0xf8) ) &&
                 ((dst->g & 0xfc) == (dst_avo_clr->g & 0xfc) ) &&
                 ((dst->b & 0xf8) == (dst_avo_clr->b & 0xf8) ) )
                return 1;
            break;
        case CLR_RGB888:
            if(  ((dst->r & 0xff) == (dst_avo_clr->r & 0xff) ) &&
                 ((dst->g & 0xff) == (dst_avo_clr->g & 0xff) ) &&
                 ((dst->b & 0xff) == (dst_avo_clr->b & 0xff) ) )
                 return 1;
            break;
        case CLR_ARGB8888:
        case CLR_PARGB8888:
            if(  ((dst->a & 0xff) == (dst_avo_clr->a & 0xff) ) &&
                 ((dst->r & 0xff) == (dst_avo_clr->r & 0xff) ) &&
                 ((dst->g & 0xff) == (dst_avo_clr->g & 0xff) ) &&
                 ((dst->b & 0xff) == (dst_avo_clr->b & 0xff) ) )
                 return 1;
            break;
        case CLR_ARGB565:
        case CLR_PARGB565:
            if( ((dst->a & 0xff) == (dst_avo_clr->a & 0xff) ) &&
                ((dst->r & 0xf8) == (dst_avo_clr->r & 0xf8) ) &&
                ((dst->g & 0xfc) == (dst_avo_clr->g & 0xfc) ) &&
                ((dst->b & 0xf8) == (dst_avo_clr->b & 0xf8) ) )
                return 1;
            break;
        case CLR_ARGB666:
        case CLR_PARGB666:
            if( ((dst->a & 0xfc) == (dst_avo_clr->a & 0xfc) ) &&
                ((dst->r & 0xfc) == (dst_avo_clr->r & 0xfc) ) &&
                ((dst->g & 0xfc) == (dst_avo_clr->g & 0xfc) ) &&
                ((dst->b & 0xfc) == (dst_avo_clr->b & 0xfc) ) )
                return 1;
            break;
        default:
            ///G2D_LOGI("Error: Illegal color format for dst clr!\n");
            ///exit(1);
            break;
    }
    return 0;
}


int  outside_draw_region( int   dst_cur_x, 
                         int   dst_cur_y, 
                         UINT8 clip_en,
                         int   clp_min_x,
                         int   clp_min_y,
                         int   clp_max_x,
                         int   clp_max_y)
{
//    if( dst_cur_x < 0 || dst_cur_y < 0)
//        return 1;
    if( clip_en == 0)
        return 0;
    if( (dst_cur_x <= clp_max_x) && (dst_cur_y <= clp_max_y)  &&
        (dst_cur_x >= clp_min_x) && (dst_cur_y >= clp_min_y))
        return 0;
    else
        return 1;
}



void Reg_to_G2DCONF( G2D_REG* reg, g2d_config* g2d_conf)
{
    g2d_conf->skeyedge_en = (((reg->G2D_MODE_CON >> 21) & 1) == 1) ? 0 : 1;
    g2d_conf->skey_fl    = (reg->G2D_MODE_CON >> 20) & 1;
    g2d_conf->smpl       = (reg->G2D_MODE_CON >> 16) & 0xf;

    g2d_conf->sad_dofs   = (reg->G2D_MODE_CON >> 14) & 3;
    g2d_conf->sad_sofs   = (reg->G2D_MODE_CON >> 12) & 3;
    g2d_conf->sad_mod    = (reg->G2D_MODE_CON >> 9) & 3;
    g2d_conf->sad_en     = (reg->G2D_MODE_CON >> 8) & 1;

    g2d_conf->eng_mode   = reg->G2D_MODE_CON & 0x7;
}

void Reg_to_ROICONF(G2D_REG* reg, roi_config* pROI)
{
    int linear_mode;
    int layer_num;
    int layer_rot[4];
    int tile_2ly1rot;

    linear_mode = ((reg->G2D_MODE_CON & 7) == LINEAR_TRANSFORM) ? 1 : 0;
    layer_num   = ((reg->G2D_ROI_CON >> 28) & 0x1) + 
                  ((reg->G2D_ROI_CON >> 29) & 0x1) +
                  ((reg->G2D_ROI_CON >> 30) & 0x1) + 
                  ((reg->G2D_ROI_CON >> 31) & 0x1) ;
    layer_rot[0] = ((reg->G2D_ROI_CON >> 31) & 0x1) & ((reg->G2D_L0_CON >> 16) & 0x1);
    layer_rot[1] = ((reg->G2D_ROI_CON >> 30) & 0x1) & ((reg->G2D_L1_CON >> 16) & 0x1);
    layer_rot[2] = ((reg->G2D_ROI_CON >> 29) & 0x1) & ((reg->G2D_L2_CON >> 16) & 0x1);
    layer_rot[3] = ((reg->G2D_ROI_CON >> 28) & 0x1) & ((reg->G2D_L3_CON >> 16) & 0x1);


    pROI->layer_en       = (reg->G2D_ROI_CON >> 28) & 0xf;
    pROI->clr_rep_en     = (reg->G2D_ROI_CON >> 21) & 1;
    pROI->dis_bg         = (reg->G2D_ROI_CON >> 19) & 1;
    pROI->force_ts       = (reg->G2D_ROI_CON >> 17) & 1;
    tile_2ly1rot = (layer_num == 2) && (layer_rot[0] ^ layer_rot[1] ^ layer_rot[2] ^ layer_rot[3]);
    if (pROI->force_ts == 1){
        pROI->tile_size = (reg->G2D_ROI_CON >> 18) & 1;
    }
    else{
        if ( (linear_mode == 0) && ((layer_num==1) || tile_2ly1rot))
            pROI->tile_size = 1;
        else 
            pROI->tile_size = 0;
    }

    if (linear_mode | layer_rot[0] | layer_rot[1] | layer_rot[2] | layer_rot[3] )
        pROI->tile_en = 1;
    else
        pROI->tile_en = 0;

    //pROI->tile_size      = (reg->G2D_ROI_CON >> 18) & 1;
    //pROI->tile_en        = (reg->G2D_ROI_CON >> 17) & 1;
    pROI->clip_en        = (reg->G2D_ROI_CON >> 16) & 1;
    pROI->out_alpha      = (reg->G2D_ROI_CON >>  8) & 0xff;
    pROI->out_alpha_en   = (reg->G2D_ROI_CON >>  7) & 1;
    pROI->ch_swap        = (reg->G2D_ROI_CON >>  4) & 1; 
    pROI->clrfmt         = reg->G2D_ROI_CON & 0xf;

    pROI->w2m_pitch      = reg->G2D_W2M_PITCH & 0x3fff;
    pROI->w2m_addr       = reg->G2D_W2M_ADDR;
    pROI->bgclr          = reg->G2D_ROI_BGCLR;
    pROI->width          = (reg->G2D_ROI_SIZE >> 16) & 0xfff;
    pROI->height         = reg->G2D_ROI_SIZE & 0xfff;
    pROI->ofs_x          = (reg->G2D_ROI_OFS >> 16) & 0xfff;
    pROI->ofs_y          = reg->G2D_ROI_OFS & 0xfff;
    pROI->ofs_x <<= 20; pROI->ofs_x >>= 20;
    pROI->ofs_y <<= 20; pROI->ofs_y >>= 20;

    pROI->clip_min_x     = (reg->G2D_CLP_MIN >> 16) & 0xfff;
    pROI->clip_min_y     = reg->G2D_CLP_MIN & 0xfff;
    pROI->clip_max_x     = (reg->G2D_CLP_MAX >> 16) & 0xfff;
    pROI->clip_max_y     = reg->G2D_CLP_MAX & 0xfff;
    pROI->clip_min_x <<= 20; pROI->clip_min_x >>= 20; 
    pROI->clip_min_y <<= 20; pROI->clip_min_y >>= 20; 
    pROI->clip_max_x <<= 20; pROI->clip_max_x >>= 20; 
    pROI->clip_max_y <<= 20; pROI->clip_max_y >>= 20; 

    pROI->mofs_x         = (reg->G2D_ROI_MOFS >> 16) & 0xfff;
    pROI->mofs_y         = reg->G2D_ROI_MOFS & 0xfff;
    pROI->mofs_x <<= 20; pROI->mofs_x >>= 20;
    pROI->mofs_y <<= 20; pROI->mofs_y >>= 20;
    
    pROI->avo_clr = reg->G2D_AVO_CLR;
    pROI->rep_clr = reg->G2D_REP_CLR;
    pROI->mwc_mw  = reg->G2D_MW_INIT;
    pROI->mwc_mz  = reg->G2D_MZ_INIT;

    pROI->dither_mode = reg->G2D_DI_CON & 0x3;
    pROI->di_bbit     = (reg->G2D_DI_CON >> 4) & 0x3;
    pROI->di_gbit     = (reg->G2D_DI_CON >> 8) & 0x3;
    pROI->di_rbit     = (reg->G2D_DI_CON >>12) & 0x3;
}

void Reg_to_LAYER( UINT32 con,
                   UINT32 clr_key,
                   UINT32 addr,
                   UINT32 pitch,
                   UINT32 size,
                   UINT32 ofs,
                   //UINT32 mem_ofs,
                   layer_config* pLayer)
{
    pLayer->font_en      = (con >> 30) & 1;
    pLayer->idx          = (con >> 28) & 3;
    pLayer->src_key_en   = (con >> 23) & 1;
    pLayer->rect_en      = (con >> 22) & 1;
    pLayer->rotate       = (con >> 16) & 7;
    pLayer->alpha        = (con >>  8) & 0xff;
    pLayer->alpha_en     = (con >>  7) & 1;
    pLayer->ch_swap      = (con >>  4) & 1;
    pLayer->clrfmt       = con & 0xf;

    pLayer->addr         = addr;
    pLayer->pitch        = pitch & 0x3fff;
    pLayer->width        = (size >> 16) & 0xfff;
    pLayer->height       = size & 0xfff;
    pLayer->src_key      = clr_key;
    pLayer->ofs_x        = (ofs >> 16) & 0xfff;
    pLayer->ofs_y        = ofs & 0xfff;
    pLayer->ofs_x <<= 20; pLayer->ofs_x >>=20;
    pLayer->ofs_y <<= 20; pLayer->ofs_y >>=20;
    //pLayer->mem_ofs_x    = (mem_ofs >> 16) & 0x7ff;
    //pLayer->mem_ofs_y    = mem_ofs & 0x7ff;

}

void pixel_convert_to_565(int dst_clr_mode, Pixel* clr)
{
    if( (dst_clr_mode == CLR_RGB565)   || 
        (dst_clr_mode == CLR_ARGB565)  || 
        (dst_clr_mode == CLR_PARGB565)   )
    {
        clr->a &= 0xff;
        clr->r &= 0xf8;
        clr->g &= 0xfc;
        clr->b &= 0xf8;
    }
}

void swap_rb_channel(Pixel* clr)
{
    int tmp;
    tmp = clr->r;
    clr->r = clr->b;
    clr->b = tmp;
}

void Reg_to_Detail( G2D_REG* reg, G2D_REG_DETAIL *detail)
{
    g2d_config   g2d_conf;   
    layer_config pLayer_conf[4];
    roi_config   pROI_conf;

    Reg_to_G2DCONF(reg, &g2d_conf);
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

    detail->src_clr_mode = pLayer_conf[1].clrfmt;
    detail->dst_clr_mode = pLayer_conf[0].clrfmt;
    detail->g2d_eng_mode = g2d_conf.eng_mode;
    detail->src_rb_swap  = pLayer_conf[1].ch_swap;
    detail->dst_rb_swap  = pLayer_conf[0].ch_swap;

    detail->ltile_en     = pROI_conf.tile_en;
    detail->clr_rep_en   = pROI_conf.clr_rep_en;
    detail->ltile_size   = pROI_conf.tile_size;
    detail->alpha_0      = pLayer_conf[1].alpha;
    detail->smpl         = g2d_conf.smpl;

    detail->skey_fl      = g2d_conf.skey_fl;
    detail->dst_trnsp_en = pROI_conf.out_alpha_en ? 0 : 1;
    detail->alp_mode_0   = pLayer_conf[1].alpha_en;
    detail->skeyedge_en  = g2d_conf.skeyedge_en;
    detail->clp_en       = pROI_conf.clip_en;
    detail->srckey_en    = pLayer_conf[1].src_key_en;

    detail->src_base     = pLayer_conf[1].addr;
    detail->src_pitch    = pLayer_conf[1].pitch;
    detail->src_x        = 0;
    detail->src_y        = 0;
    detail->src_w        = pLayer_conf[1].width;
    detail->src_h        = pLayer_conf[1].height;
    detail->src_key      = pLayer_conf[1].src_key;

    detail->dst_avo_clr  = reg->G2D_AVO_CLR;
    detail->dst_rep_clr  = reg->G2D_REP_CLR;
    detail->fgclr        = pROI_conf.bgclr;

    detail->dst_base     = pROI_conf.w2m_addr;
    detail->dst_pitch    = pROI_conf.w2m_pitch;
    detail->dst_x0       = pROI_conf.ofs_x;
    detail->dst_y0       = pROI_conf.ofs_y;
    detail->dst_w        = pROI_conf.width;
    detail->dst_h        = pROI_conf.height;

    detail->clip_min_x   = pROI_conf.clip_min_x;
    detail->clip_min_y   = pROI_conf.clip_min_y;
    detail->clip_max_x   = pROI_conf.clip_max_x;
    detail->clip_max_y   = pROI_conf.clip_max_y;

    detail->buf_sta_addr_0 = reg->G2D_L0_CLRKEY;
    detail->tilt_0300      = reg->G2D_L2_ADDR;
    detail->tilt_0704      = reg->G2D_L2_SIZE;
    detail->tilt_0B08      = reg->G2D_L2_OFS;
    detail->tilt_0F0C      = reg->G2D_L2_CLRKEY;
    detail->tilt_1310      = reg->G2D_L3_ADDR;
    detail->tilt_1714      = reg->G2D_L3_SIZE;
    detail->tilt_1B18      = reg->G2D_L3_OFS;
    detail->tilt_1F1C      = reg->G2D_L3_CLRKEY;
    
    detail->start_dst_x    = reg->G2D_L2_PITCH & 0x1fff;
    detail->start_dst_y    = reg->G2D_L3_PITCH & 0x1fff;
    detail->start_dst_x <<= 19; detail->start_dst_x >>= 19;               //sign extension
    detail->start_dst_y <<= 19; detail->start_dst_y >>= 19;               //sign extension

    detail->out_alpha      = pROI_conf.out_alpha;
    detail->dither_mode    = pROI_conf.dither_mode;
    detail->di_rbit        = pROI_conf.di_rbit;
    detail->di_gbit        = pROI_conf.di_gbit;
    detail->di_bbit        = pROI_conf.di_bbit;
}

int  getMWC_noise(MWCData* mwc)
{
    if(mwc->valid_bits == 0){
        mwc->m_z = 36969 * (mwc->m_z & 65535) + (mwc->m_z >> 16);
        mwc->m_w = 18000 * (mwc->m_w & 65535) + (mwc->m_w >> 16);
        mwc->rnd = ((mwc->m_z << 16) + mwc->m_w);
        mwc->valid_bits = 32;
    }

    mwc->noise = mwc->rnd & 0xf;
    mwc->rnd >>= 4;
    mwc->valid_bits -= 4;
    return (int)mwc->noise;
}

int  gen_first_MWC(MWCData* mwc)
{
    mwc->rnd = ((mwc->m_z << 16) + mwc->m_w);
    mwc->valid_bits = 32;
    mwc->noise = mwc->rnd & 0xf;
    return (int)mwc->noise;
}

void dither_one_ch (int* data, int noise, int di_bit)
{
    int n;
    n = noise & 7;
    n = n >> (3-di_bit);

    *data = *data + n;
    if (*data > 255)
        *data = 255;
}

int  getFIX_noise(int x, int y) 
{
    int n;
    int _x, _y;
    _x = x & 1; _y = y & 1;
    if (_x == 0 && _y == 0) {
        n = 0;
    } 
    else if (_x == 1 && _y == 0) {
        n = 2;
    }
    else if (_x == 0 && _y == 1){
        n = 4;
    }
    else {
        n = 6;
    }
    return n;
}

