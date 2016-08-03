#ifndef __GDI_INCLUDE_H__
#define __GDI_INCLUDE_H__


#define GDI_LAYER_ACT_LAYER_STACK_COUNT 1
#define GDI_LAYER_TOTAL_LAYER_COUNT 1


/****************************************************************************
* Struct / Typedef                                                                 
*****************************************************************************/
// TODO: merge this struct to gdi_layer_struct*, remove duplicate variable between this two structures.
typedef struct
{
   kal_uint8   rotate_value;           /* (keep)rotate select for specified layer */
   kal_uint8   opacity_value;          /* (keep)opacity value for specified layer */
   kal_bool    opacity_enable;         /* (keep)enable/disable opacity for specified layer */
   kal_bool    source_key_enable;      /* (keep)enable/disable source key for specified layer */
   kal_uint32  source_key;             /* (keep)source key color in RGB565 format for specified layer */
   kal_int32   x_offset;               /* (keep)x axis offset from main window for specified layer, should only refrence by lcd */
   kal_int32   y_offset;               /* (keep)y axis offset from main widnow for specified layer, should only refrence by lcd */
   kal_uint32  frame_buffer_address;   /* frame buffer start address of specified layer */
   kal_bool    dither_enable;          /* (keep)enable/disable source key for specified layer */

   kal_bool    color_palette_enable;   /* enable/disable color palette for specified layer */
   kal_uint8   color_palette_select;   /* selection of color palette table */
   kal_uint8   source_color_format;    /* color format of the specified layer */   
} gdi_lcd_layer_struct;


#define GDI_LAYER_CLIP_STACK_COUNT 10


typedef struct gdi_layer_struct
{
    /* Field position is arranged to optimize the usage of cache
       for govl_config_layer */
    U8 id;
    U8 flag;
    U8 clips_top;
    U16 width, height;
    
    U8 cf;          /* real color format ( it will support LCD color format) */
    U8 vcf;         /* virtual color format (convert to standard color format) */
    U8 bits_per_pixel;
    U8 rotate_value;
    
    S32 offset_x, offset_y;
    U16 blt_x, blt_y, blt_width, blt_height; /* blt rect, the coordinate is offset from layer frame buffer */
    
    gdi_color background;   /* only valide when this layer is the "lowest" layer. */
    U8 *buf_ptr;
    U8 *buf_ptr1;

    S16 clipx1, clipy1, clipx2, clipy2;
    gdi_rect_struct clips[GDI_LAYER_CLIP_STACK_COUNT];
#if defined(__GDI_BLT_WITH_DOUBLE_BUF_SUPPORT__)
    U8* bltdb_buf_ptr1;     /*inactive buf when calling blt*/
    U8* bltdb_buf_ptr2;     /*hw update buf*/
#endif
    U32 hw_update_element;   /* if it's hw layer, remember its blt element after hw udpate stop. For blt or flatten hw layer after hw update stop. */
    S32 layer_size; /* bytes */
    gd_get_pixel_func get_pixel;
    gd_put_pixel_func put_pixel;
    
    void (*lazy_free_callback)(gdi_handle handle);
#ifdef GDI_DEBUG_SUPPORT
    /* The layer creator for debug */
    void *creator_lr;
#endif
} gdi_layer_struct;

extern gdi_layer_struct GDI_LAYERS[GDI_LAYER_TOTAL_LAYER_COUNT];
extern gdi_layer_struct *gdi_act_layer;
extern gd_color_from_rgb_func gdi_act_color_from_rgb;
extern gd_color_to_rgb_func gdi_act_color_to_rgb;
extern gd_put_pixel_func gdi_act_put_pixel;
extern gd_get_pixel_func gdi_act_get_pixel;

#define GDI_LAYER (*gdi_act_layer)


#define  GDI_SET_BUFFER_PIXEL(X,Y,VALUE)        gdi_act_put_pixel(X,Y,VALUE)
#define  GDI_GET_BUFFER_PIXEL(X,Y,VALUE)     VALUE=gdi_act_get_pixel(X,Y)


#define GDI_LAYER_FLAG_USED               (1<<0)
#define GDI_LAYER_FLAG_DOUBLE_LAYER       (1<<1)
#define GDI_LAYER_FLAG_USE_OUTSIDE_MEMORY (1<<2)
#define GDI_LAYER_FLAG_FROZEN             (1<<3)
#define GDI_LAYER_FLAG_LAZY_FREE          (1<<4)
#define GDI_LAYER_FLAG_HW_UPDATE          (1<<5)

#define GDI_LAYER_CLEAR_FLAG(HANDLE,FLAG)    ((gdi_layer_struct*)HANDLE)->flag  &= ~(U32)(FLAG)
#define GDI_LAYER_SET_FLAG(HANDLE,FLAG)      ((gdi_layer_struct*)HANDLE)->flag  |= (FLAG)
#define GDI_LAYER_GET_FLAG(HANDLE,FLAG)      (((gdi_layer_struct*)HANDLE)->flag  & (FLAG))

#define GDI_LAYER_SET_CLIP(HANDLE,X1,Y1,X2,Y2)  \
   do                                           \
   {                                            \
      ((gdi_layer_struct*)HANDLE)->clipx1=X1;   \
      ((gdi_layer_struct*)HANDLE)->clipy1=Y1;   \
      ((gdi_layer_struct*)HANDLE)->clipx2=X2;   \
      ((gdi_layer_struct*)HANDLE)->clipy2=Y2;   \
   }while(0)

#define GDI_LAYER_RESET_CLIPS(HANDLE)  ((gdi_layer_struct*)HANDLE)->clips_top = 0

#define GDI_LAYER_SET_BLT_RECT(HANDLE,X,Y,W,H)  \
   do                                           \
   {                                            \
      ((gdi_layer_struct*)HANDLE)->blt_x=X;   \
      ((gdi_layer_struct*)HANDLE)->blt_y=Y;   \
      ((gdi_layer_struct*)HANDLE)->blt_width=W;   \
      ((gdi_layer_struct*)HANDLE)->blt_height=H;   \
   }while(0)


#define GDI_ASSERT(x) NULL

#define GDI_LCD_WIDTH         240
#define GDI_LCD_HEIGHT        240

extern const unsigned short gd_sqrt[1025];
extern const unsigned short gd_sin_table[91];
extern const unsigned short gd_tan_table[91];

extern const gd_color_from_rgb_func gd_color_from_rgb[];
extern const gd_color_to_rgb_func gd_color_to_rgb[];
extern const gd_get_pixel_func gd_get_pixel[];
extern const gd_put_pixel_func gd_put_pixel[];
extern const gd_put_pixel_to_layer_func gd_put_pixel_to_layer[];
extern const gd_get_buf_pixel_func gd_get_buf_pixel[];
extern const gd_put_buf_pixel_func gd_put_buf_pixel[];
extern const gd_replace_src_key_func gd_replace_src_key[];
extern const gd_bitblt_func gd_resize_bitblt[];
extern const gd_bitblt_func gd_bitblt[];
extern const gd_memset_func gd_memset[];
extern const gd_fill_dot_rect_func gd_fill_dot_rect[];
extern const gd_image_bits_draw_func gd_image_bits_draw[];

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_clip
 * DESCRIPTION
 *  Get current clip region of a layer.
 * PARAMETERS
 *  x1                : [OUT]   x of left-top cornor of clip region
 *  y1                : [OUT]   y of left-top cornor of clip region
 *  x2                : [OUT]   x of right-down cornor of clip region
 *  y2                : [OUT]   y of right-down cornor of clip region
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_layer_get_clip(S32 *x1, S32 *y1, S32 *x2, S32 *y2);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_clip
 * DESCRIPTION
 *  Set clip region.
 *  
 *  Will set clip region, use layer's own coordinate.
 * PARAMETERS
 *  x1                : [IN]   x of left-top cornor of clip region
 *  y1                : [IN]   y of left-top cornor of clip region
 *  x2                : [IN]   x of right-down cornor of clip region
 *  y2                : [IN]   y of right-down cornor of clip region
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_layer_set_clip(S32 x1, S32 y1, S32 x2, S32 y2);
extern GDI_RESULT gdi_layer_test_set_clip(gdi_rect_struct *rect);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_reset_clip
 * DESCRIPTION
 *  Reset clip region to layer size
 *  Will reset clip region to layer's width/height.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_layer_reset_clip(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_push_clip
 * DESCRIPTION
 *  Push current clip region into clip stack.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_layer_push_clip(void);

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_push_and_set_clip
 * DESCRIPTION
  *  Push current clip region into stack and set clip region, use layer's 
  *  own coordinate.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_layer_push_and_set_clip(S32 x1, S32 y1, S32 x2, S32 y2);

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_pop_clip
 * DESCRIPTION
 *  Pop current clip region from stack.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_layer_pop_clip(void);

extern gdi_color gd_color_from_rgb_8(U32 A, U32 R, U32 G, U32 B);
extern gdi_color gd_color_from_rgb_16(U32 A, U32 R, U32 G, U32 B);
extern gdi_color gd_color_from_rgb_24(U32 A, U32 R, U32 G, U32 B);
extern gdi_color gd_color_from_rgb_32(U32 A, U32 R, U32 G, U32 B);
extern gdi_color gd_color_from_rgb_32PARGB(U32 A, U32 R, U32 G, U32 B);
extern gdi_color gd_color_from_rgb_PARGB6666(U32 A, U32 R, U32 G, U32 B);

extern void gd_color_to_rgb_8(U32 *R, U32 *A, U32 *G, U32 *B, gdi_color c);
extern void gd_color_to_rgb_16(U32 *R, U32 *A, U32 *G, U32 *B, gdi_color c);
extern void gd_color_to_rgb_24(U32 *R, U32 *A, U32 *G, U32 *B, gdi_color c);
extern void gd_color_to_rgb_32(U32 *R, U32 *A, U32 *G, U32 *B, gdi_color c);
extern void gd_color_to_rgb_32PARGB(U32 *R, U32 *A, U32 *G, U32 *B, gdi_color c);
extern void gd_color_to_rgb_PARGB6666(U32 *R, U32 *A, U32 *G, U32 *B, gdi_color c);

extern void gd_put_pixel_8(int x, int y, gdi_color c);
extern void gd_put_pixel_16(int x, int y, gdi_color c);
extern void gd_put_pixel_24(int x, int y, gdi_color c);
extern void gd_put_pixel_32(int x, int y, gdi_color c);
extern void gd_put_pixel_32PARGB(int x, int y, gdi_color c);
extern void gd_put_pixel_PARGB6666(int x, int y, gdi_color c);

extern void gd_put_pixel_to_layer_8(gdi_handle layer_handle, int x, int y, gdi_color c);
extern void gd_put_pixel_to_layer_16(gdi_handle layer_handle, int x, int y, gdi_color c);
extern void gd_put_pixel_to_layer_24(gdi_handle layer_handle, int x, int y, gdi_color c);
extern void gd_put_pixel_to_layer_32(gdi_handle layer_handle, int x, int y, gdi_color c);
extern void gd_put_pixel_to_layer_32PARGB(gdi_handle layer_handle, int x, int y, gdi_color c);
extern void gd_put_pixel_to_layer_PARGB6666(gdi_handle layer_handle, int x, int y, gdi_color c);

extern gdi_color gd_get_pixel_8(int x, int y);
extern gdi_color gd_get_pixel_16(int x, int y);
extern gdi_color gd_get_pixel_24(int x, int y);
extern gdi_color gd_get_pixel_32(int x, int y);
extern gdi_color gd_get_pixel_32PARGB(int x, int y);
extern gdi_color gd_get_pixel_PARGB6666(int x, int y);

extern void gd_put_buf_pixel_8(U8 *buf_ptr, U32 buf_width, int x, int y, gdi_color c);
extern void gd_put_buf_pixel_16(U8 *buf_ptr, U32 buf_width, int x, int y, gdi_color c);
extern void gd_put_buf_pixel_24(U8 *buf_ptr, U32 buf_width, int x, int y, gdi_color c);
extern void gd_put_buf_pixel_32(U8 *buf_ptr, U32 buf_width, int x, int y, gdi_color c);
extern void gd_put_buf_pixel_32PARGB(U8 *buf_ptr, U32 buf_width, int x, int y, gdi_color c);
extern void gd_put_buf_pixel_PARGB6666(U8 *buf_ptr, U32 buf_width, int x, int y, gdi_color c);

extern gdi_color gd_get_buf_pixel_8(U8 *buf_ptr, U32 buf_width, int x, int y);
extern gdi_color gd_get_buf_pixel_16(U8 *buf_ptr, U32 buf_width, int x, int y);
extern gdi_color gd_get_buf_pixel_24(U8 *buf_ptr, U32 buf_width, int x, int y);
extern gdi_color gd_get_buf_pixel_32(U8 *buf_ptr, U32 buf_width, int x, int y);
extern gdi_color gd_get_buf_pixel_32PARGB(U8 *buf_ptr, U32 buf_width, int x, int y);
extern gdi_color gd_get_buf_pixel_PARGB6666(U8 *buf_ptr, U32 buf_width, int x, int y);

extern void gd_replace_src_key_8(
                U8 *dest_ptr,
                S32 dest_pitch,
                S32 dest_offset_x,
                S32 dest_offset_y,
                S32 clipx1,
                S32 clipy1,
                S32 clipx2,
                S32 clipy2,
                gdi_color src_key,
                S32 width,
                S32 height);
extern void gd_replace_src_key_16(
                U8 *dest_ptr,
                S32 dest_pitch,
                S32 dest_offset_x,
                S32 dest_offset_y,
                S32 clipx1,
                S32 clipy1,
                S32 clipx2,
                S32 clipy2,
                gdi_color src_key,
                S32 width,
                S32 height);
extern void gd_replace_src_key_24(
                U8 *dest_ptr,
                S32 dest_pitch,
                S32 dest_offset_x,
                S32 dest_offset_y,
                S32 clipx1,
                S32 clipy1,
                S32 clipx2,
                S32 clipy2,
                gdi_color src_key,
                S32 width,
                S32 height);
extern void gd_replace_src_key_32(
                U8 *dest_ptr,
                S32 dest_pitch,
                S32 dest_offset_x,
                S32 dest_offset_y,
                S32 clipx1,
                S32 clipy1,
                S32 clipx2,
                S32 clipy2,
                gdi_color src_key,
                S32 width,
                S32 height);
extern void gd_replace_src_key_32PARGB(
                U8 *dest_ptr,
                S32 dest_pitch,
                S32 dest_offset_x,
                S32 dest_offset_y,
                S32 clipx1,
                S32 clipy1,
                S32 clipx2,
                S32 clipy2,
                gdi_color src_key,
                S32 width,
                S32 height);
extern void gd_replace_src_key_PARGB6666(
                U8 *dest_ptr,
                S32 dest_pitch,
                S32 dest_offset_x,
                S32 dest_offset_y,
                S32 clipx1,
                S32 clipy1,
                S32 clipx2,
                S32 clipy2,
                gdi_color src_key,
                S32 width,
                S32 height);

#endif
