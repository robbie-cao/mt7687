//#if defined (__DRV_GRAPHICS_G2D_6260_SERIES__)

#ifndef __G2D_DRV_C_MODEL_6260_SERIES_H__
#define __G2D_DRV_C_MODEL_6260_SERIES_H__


//----- Swicth definition -----//
//#define DEBUG
//#define G2D_DRAW_SAD_ON_DST             //should not define for real c-model, this is for testing SAD


//----- Constant definition -----//
#define CLR_8BPP        0
#define CLR_RGB565      1
#define CLR_YUYV422     2
#define CLR_RGB888      3
#define CLR_ARGB8888    8
#define CLR_ARGB565     9
#define CLR_ARGB666    10
#define CLR_PARGB8888  12
#define CLR_PARGB565   13
#define CLR_PARGB666   14

#define BITBLT           1
#define LINEAR_TRANSFORM 2
#define RECTANGLE_FILL   4

//----- type define -----//
typedef unsigned char  UINT8;
typedef unsigned short UINT16;
typedef unsigned int   UINT32;
typedef int            INT32;
//#ifndef WIN32
//    typedef long __int64;
//#endif


typedef struct _G2D_REG {
    UINT32 G2D_START;
    UINT32 G2D_MODE_CON;
    UINT32 G2D_SAD_SUM;
    UINT32 G2D_IRQ;
    UINT32 G2D_SLOW_DOWN;

    UINT32 G2D_ROI_CON;    
    UINT32 G2D_W2M_ADDR;
    UINT32 G2D_W2M_PITCH;
    UINT32 G2D_ROI_OFS;
    UINT32 G2D_ROI_SIZE;
    UINT32 G2D_ROI_BGCLR;     //BGCLR
    UINT32 G2D_CLP_MIN;
    UINT32 G2D_CLP_MAX;
    UINT32 G2D_AVO_CLR;
    UINT32 G2D_REP_CLR;
    UINT32 G2D_ROI_MOFS;
    UINT32 G2D_MW_INIT;
    UINT32 G2D_MZ_INIT;
    UINT32 G2D_DI_CON;

    UINT32 G2D_L0_CON;
    UINT32 G2D_L0_CLRKEY;     //FGCLR, buf_sta_addr_0
    UINT32 G2D_L0_ADDR;
    UINT32 G2D_L0_PITCH;
    UINT32 G2D_L0_SIZE;
    UINT32 G2D_L0_OFS;
    //UINT32 G2D_L0_MOFS;       //start_dst_x, start_dst_y

    UINT32 G2D_L1_CON;
    UINT32 G2D_L1_CLRKEY;
    UINT32 G2D_L1_ADDR;
    UINT32 G2D_L1_PITCH;
    UINT32 G2D_L1_SIZE;
    UINT32 G2D_L1_OFS;
    //UINT32 G2D_L1_MOFS;

    UINT32 G2D_L2_CON;
    UINT32 G2D_L2_CLRKEY;
    UINT32 G2D_L2_ADDR;       //TILT_0300
    UINT32 G2D_L2_PITCH; 
    UINT32 G2D_L2_SIZE;       //TILT_0740
    UINT32 G2D_L2_OFS;        //TILT_0B08
    //UINT32 G2D_L2_MOFS;       //TILT_0F0C

    UINT32 G2D_L3_CON;
    UINT32 G2D_L3_CLRKEY;
    UINT32 G2D_L3_ADDR;       //TILT_1310
    UINT32 G2D_L3_PITCH;       
    UINT32 G2D_L3_SIZE;       //TILT_1714
    UINT32 G2D_L3_OFS;        //TILT_1B18
    //UINT32 G2D_L3_MOFS;       //TILT_1F1C

} G2D_REG;


typedef struct tag_roi_config {
    UINT32   w2m_addr;
    UINT32   w2m_pitch;
    int      ofs_x;
    int      ofs_y;
    UINT32   width;
    UINT32   height;
    UINT32   bgclr;
    UINT32   out_alpha;

    UINT32   clip_en;
    int      clip_min_x;
    int      clip_min_y;
    int      clip_max_x;
    int      clip_max_y;

    UINT32   clrfmt;
    UINT32   ch_swap;
    UINT32   force_ts;
    UINT32   tile_en;
    UINT32   tile_size;
    UINT32   out_alpha_en;
    UINT32   layer_en;
    UINT32   dis_bg;
    //UINT32   sw_base;
    UINT32   clr_rep_en;
    UINT32   avo_clr;
    UINT32   rep_clr;
    int      mofs_x;
    int      mofs_y;

    UINT32   dither_mode;
    UINT32   mwc_mw;
    UINT32   mwc_mz;
    UINT32   di_rbit;
    UINT32   di_gbit;
    UINT32   di_bbit;

}roi_config;

typedef struct tag_layer_config {
    int      clrfmt;
    int      rect_en;
    int      src_key_en;
    int      ch_swap;
    int      rotate;
    int      alpha_en;
    int      font_en;
    int      idx;

    UINT32   addr;
    UINT32   pitch;
    UINT32   width;
    UINT32   height;
    UINT32   alpha;
    UINT32   src_key;
    int      ofs_x;       //12bit signed int
    int      ofs_y;       //12bit signed int
    //int      mem_ofs_x;   //11bit unsigned int
    //int      mem_ofs_y;   //11bit unsigned int
}layer_config;

typedef struct tag_g2d_config{
    //UINT32   persp_en;
    UINT32   skey_fl;
    UINT32   skeyedge_en;
    UINT32   smpl;
    UINT32   sad_dofs;
    UINT32   sad_sofs;
    UINT32   sad_mod;
    UINT32   sad_en;
    UINT32   eng_mode;

    UINT32   sad_acc;
}g2d_config;


typedef struct _G2D_REG_DETAIL{
    //G2D_FMODE_CON
    int     src_clr_mode;
    int     dst_clr_mode;
    int     g2d_eng_mode;
    int     src_rb_swap;
    int     dst_rb_swap;

    //G2D_SMODE_CON
    int     fita;
    int     fnbg;
    int     ltile_en;
    int     clr_rep_en;
    int     ltile_size;
    int     bb_mode;
    int     alpha_0;
    int     bidx_en;
    int     bidx;
    int     smpl;
    int     font_idx;
    int     brot;

    //G2D_COM_CON
    int     tidle_rst;
    int     skey_fl;
    int     dst_trnsp_en;
    int     alp_mode_0;
    //int     persp_en;
    int     clp_en;
    int     skeyedge_en;
    int     srckey_en;
    int     rst;

    //G2D_SRC
    UINT32  src_base;
    int     src_pitch;
    int     src_x;
    int     src_y;
    int     src_w;
    int     src_h;
    UINT32  src_key;
    UINT32  dst_avo_clr;
    UINT32  dst_rep_clr;
    
    //G2D_DST
    UINT32  dst_base;
    int     dst_pitch;
    int     dst_x0;
    int     dst_y0;
    int     dst_w;
    int     dst_h;

    UINT32  fgclr;
    UINT32  bgclr;
    int     clip_min_x;
    int     clip_min_y;
    int     clip_max_x;
    int     clip_max_y;
    
    UINT32  buf_sta_addr_0;
    UINT32  tilt_0300;
    UINT32  tilt_0704;
    UINT32  tilt_0B08;
    UINT32  tilt_0F0C;
    UINT32  tilt_1310;
    UINT32  tilt_1714;
    UINT32  tilt_1B18;
    UINT32  tilt_1F1C;

    int     start_dst_x;
    int     start_dst_y;

    int     out_alpha;
    int     dither_mode;
    int     di_rbit;
    int     di_gbit;
    int     di_bbit;
}G2D_REG_DETAIL;


typedef struct _Pixel{
    int a;
    int r;
    int g;
    int b;
}Pixel;

typedef struct tagBITMAPFILEHEADER {
    UINT32   bfSize;
    UINT16   bfReserved1;
    UINT16   bfReserved2;
    UINT32   bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    UINT32   biSize;
    UINT32   biWidth;
    UINT32   biHeight;
    UINT16   biPlanes;
    UINT16   biBitCount;
    UINT32   biCompression;
    UINT32   biSizeImage;
    UINT32   biXPelsPerMeter;
    UINT32   biYPelsPerMeter;
    UINT32   biClrUsed;
    UINT32   biClrImportant;
}BITMAPINFOHEADER;

typedef struct _ImgInfo{
    char*   l0_hex_filename;
    char*   l1_hex_filename;
    char*   l2_hex_filename;
    char*   l3_hex_filename;
    char*   dump_hex_filename;
    char*   dump_bmp_filename;

    UINT32  l0_init_addr;
    UINT32  l1_init_addr;
    UINT32  l2_init_addr;
    UINT32  l3_init_addr;

    UINT32  canvas_addr;
    UINT32  canvas_width;
    UINT32  canvas_height;
}ImgInfo;

/*
typedef struct _ImgInfo{
    char*   src_hex_filename;
    char*   dst_hex_filename;
    char*   idx_hex_filename;
    char*   dump_hex_filename;
    char*   dump_bmp_filename;

    int     src_sizeX;
    int     src_sizeY;
    int     dst_sizeX;
    int     dst_sizeY;
}ImgInfo;
*/

typedef struct _MWCData {
    UINT32  m_z;
    UINT32  m_w;
    
    UINT32  rnd;
    UINT32  valid_bits;
    UINT32  noise;
}MWCData;

#endif
//#endif
