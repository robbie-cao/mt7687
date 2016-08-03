#include "gdi_datatype.h"
#include "gdi_const.h"
#include "gdi_include.h"
#include "graphic_log.h"

#ifdef GDI_COLORFORMAT8_PALETTE
#if defined GDI_SUPPORT_PALETTES
gdi_color gdi_layer_palette[GDI_SUPPORT_PALETTES][256];
#endif
#endif

static BOOL gdi_is_layer_enable = FALSE;
static gdi_handle gdi_layer_stack[GDI_LAYER_ACT_LAYER_STACK_COUNT];
static S32 gdi_layer_stack_index;

gdi_layer_struct GDI_LAYERS[GDI_LAYER_TOTAL_LAYER_COUNT];
gdi_layer_struct *gdi_act_layer;
gd_color_from_rgb_func gdi_act_color_from_rgb;
gd_color_to_rgb_func gdi_act_color_to_rgb;
gd_put_pixel_func gdi_act_put_pixel;
gd_get_pixel_func gdi_act_get_pixel;
gd_replace_src_key_func gdi_act_replace_src_key;

gdi_lcd_layer_struct gdi_layer_info[GDI_LAYER_TOTAL_LAYER_COUNT];   /* this struct is used for output lcd */
gdi_lcd_layer_struct *gdi_act_layer_info;
static int gdi_palette_used;
static BOOL gdi_is_non_block_blt;
static S32 gdi_draw_section_level;
static BOOL gdi_is_blting;


void gdi_layer_set_active(gdi_handle handle)
{
    kal_uint32 lr = 0;

    gdi_act_layer = (gdi_layer_struct*) handle;
    gdi_act_layer_info = &(gdi_layer_info[gdi_act_layer->id]);


    gdi_act_color_from_rgb = gd_color_from_rgb[gdi_act_layer->cf];
    gdi_act_color_to_rgb = gd_color_to_rgb[gdi_act_layer->cf];

    gdi_act_put_pixel = gd_put_pixel[gdi_act_layer->cf];
    gdi_act_get_pixel = gd_get_pixel[gdi_act_layer->cf];
    gdi_act_replace_src_key = gd_replace_src_key[gdi_act_layer->cf];

}


/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_clip
 * DESCRIPTION
 *  Set clip region.
 *  
 *  Will set clip region, use layer's own coordinate.
 * PARAMETERS
 *  x1                  [IN]        
 *  y1                  [IN]        
 *  x2                  [IN]        
 *  y2                  [IN]        
 *  gdi_handle(?)       [IN]        Handle
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_layer_set_clip(S32 x1, S32 y1, S32 x2, S32 y2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 x_max_limit;
    S32 y_max_limit;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if (x2 < x1)
    {
        x2 = x1;
    }
    if (y2 < y1)
    {
        y2 = y1;
    }

    x_max_limit = gdi_act_layer->width - 1;
    y_max_limit = gdi_act_layer->height - 1;


    GDI_LAYER_SET_CLIP(gdi_act_layer, x1, y1, x2, y2);

}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_reset_clip
 * DESCRIPTION
 *  Reset clip region to layer size
 *  
 *  Will reset clip region to layer's width/height.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_layer_reset_clip(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_LAYER_SET_CLIP(gdi_act_layer, 0, 0, gdi_act_layer->width - 1, gdi_act_layer->height - 1);
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_clip
 * DESCRIPTION
 *  Get current clip region of a layer.
 * PARAMETERS
 *  x1                  [IN]        
 *  y1                  [IN]        
 *  x2                  [IN]        
 *  y2                  [IN]        
 *  gdi_handle(?)       [IN]        Handle
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_layer_get_clip(S32 *x1, S32 *y1, S32 *x2, S32 *y2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *x1 = gdi_act_layer->clipx1;
    *y1 = gdi_act_layer->clipy1;
    *x2 = gdi_act_layer->clipx2;
    *y2 = gdi_act_layer->clipy2;
}


/*****************************************************************************
 * FUNCTION
 *  gdi_layer_push_clip
 * DESCRIPTION
 *  Push current clip region into stack.
 *  
 *  JF's opnion is the merge the clip region
 *  to get final clip region - need more study
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_layer_push_clip(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_rect_struct *rect;
    kal_uint32 lr = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ASSERT(gdi_act_layer->clips_top < GDI_LAYER_CLIP_STACK_COUNT);


    rect = &gdi_act_layer->clips[gdi_act_layer->clips_top];

    rect->x1 = gdi_act_layer->clipx1;
    rect->y1 = gdi_act_layer->clipy1;
    rect->x2 = gdi_act_layer->clipx2;
    rect->y2 = gdi_act_layer->clipy2;

    gdi_act_layer->clips_top++;
}

void gdi_push_and_set_clip(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    gdi_layer_push_and_set_clip(x1, y1, x2, y2);
}

void gdi_pop_clip()
{
    gdi_layer_pop_clip();
}

void gdi_get_clip(int32_t *x1, int32_t *y1, int32_t *x2, int32_t *y2)
{
    
    gdi_layer_get_clip(x1, y1, x2, y2);
}


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
void gdi_layer_push_and_set_clip(S32 x1, S32 y1, S32 x2, S32 y2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    gdi_layer_push_clip();
    gdi_layer_set_clip(x1, y1, x2, y2);
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_pop_clip
 * DESCRIPTION
 *  Pop current clip region into stack.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_layer_pop_clip(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_rect_struct *rect;
    kal_uint32 lr = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    GDI_ASSERT(gdi_act_layer->clips_top > 0);
    rect = &gdi_act_layer->clips[--(gdi_act_layer->clips_top)];


    gdi_act_layer->clipx1 = rect->x1;
    gdi_act_layer->clipy1 = rect->y1;
    gdi_act_layer->clipx2 = rect->x2;
    gdi_act_layer->clipy2 = rect->y2;

}



/*****************************************************************************
 * FUNCTION
 *  gdi_layer_create_internal
 * DESCRIPTION
 *  Init a layer.
 *  
 *  Init layer as most basic layer, which does not
 *  support src_key, opacity, etc.
 *  This function will retrun FALSE if it cant allocate buffer.
 * PARAMETERS
 *  handle              [IN]        
 *  offset_x            [IN]        
 *  offset_y            [IN]        
 *  width               [IN]        
 *  height              [IN]        
 *  cf                  [IN]        
 *  is_double_layer     [IN]        
 *  buf_ptr             [?]         
 *  buf_ptr1            [?]         
 *  buf_size            [IN]        
 *  lcd_handle(?)       [IN]        
 * RETURNS
 *  void
 * BOOL
 *  
 *****************************************************************************/
static void gdi_layer_create_internal(
                gdi_handle handle,
                S32 offset_x,
                S32 offset_y,
                S32 width,
                S32 height,
                gdi_color_format cf,
                BOOL is_double_layer,
                U8 *buf_ptr,
                U8 *buf_ptr1,
                U32 buf_size
                )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_layer_struct *layer = (gdi_layer_struct*) handle;
    U8 id = layer->id;
    gdi_lcd_layer_struct *layer_info = &gdi_layer_info[id];

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    memset(layer, 0, sizeof(gdi_layer_struct));
    memset(layer_info, 0, sizeof(gdi_lcd_layer_struct));
    layer->id = id;

    GDI_LAYER_SET_FLAG(layer, GDI_LAYER_FLAG_USED);

    /* 
     * Layer framebuffer can only be obtained in the following five ways:
     * 
     * 1. App-based ASM: applib_mem_ap_alloc_framebuffer()
     * 2. Screen-based ASM: applib_mem_screen_alloc_framebuffer()
     * 3. ADM for framebuffer: MMI_FRM_ADM_ALLOC_FRAMEBUFFER()
     * 4. Media buffer
     * 5. ZI data in "NONCACHEDZI" section
     */
    layer->buf_ptr = buf_ptr;
    GDI_ASSERT(layer->buf_ptr);                            /* buffer available */
    GDI_ASSERT(((kal_uint32)layer->buf_ptr & 0x03) == 0);              /* buffer 4-byte alignment */
    
    layer->cf = cf;

    if ((GDI_COLOR_FORMAT_32 == layer->cf) ||
        (GDI_COLOR_FORMAT_32_PARGB== layer->cf) ||
        (GDI_COLOR_FORMAT_PARGB6666 == layer->cf))
    {
        layer_info->opacity_enable = KAL_TRUE;
        layer_info->opacity_value = 0xFF;
    }


    layer->width = (U16) width;
    layer->height = (U16) height;
    layer->bits_per_pixel = 16;
    layer->layer_size = (U32) buf_size;

    layer->offset_x = offset_x;
    layer->offset_y = offset_y;


    /* set clipping */
    GDI_LAYER_RESET_CLIPS(layer);
    GDI_LAYER_SET_CLIP(layer, 0, 0, width - 1, height - 1);

    /* set blt region */
    GDI_LAYER_SET_BLT_RECT(layer, 0, 0, width, height);

    // default layer background is WHITE
    //W05.39 Because Hardware only could be set to 24 bit color not labyer color format
    layer->background = gd_color_from_rgb_16(255, 255, 255, 255);

    /* init lcd_layer */
    layer_info->dither_enable = KAL_FALSE;
    layer_info->frame_buffer_address = (U32) layer->buf_ptr;

    layer_info->x_offset = offset_x;
    layer_info->y_offset = offset_y;
}


/*****************************************************************************
 * FUNCTION
 *  gdi_layer_clear
 * DESCRIPTION
 *  draw currently active layer's color
 * PARAMETERS
 *  bg_color        [IN]        
 * RETURNS
 *  GDI_RESULT
 *****************************************************************************/
GDI_RESULT gdi_layer_clear(gdi_color bg_color)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    
    if (GDI_LAYER.bits_per_pixel == 16)
    {
        gdi_memset16((U8*) gdi_act_layer->buf_ptr, (U16) bg_color, gdi_act_layer->layer_size);
    }
    else
    {
        gdi_layer_push_clip();
        gdi_layer_reset_clip();
        gdi_draw_solid_rect(0, 0, gdi_act_layer->width - 1, gdi_act_layer->height - 1, bg_color);
        gdi_layer_pop_clip();
    }

    return GDI_SUCCEED;
}


/*****************************************************************************
 * FUNCTION
 *  gdi_layer_init
 * DESCRIPTION
 *  Init the Frame Buffer Manager
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
GDI_RESULT gdi_layer_init(kal_uint8 *buf, kal_int32 size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    gdi_color_format cf;
    int i;

    // initialize layer array
    memset(&GDI_LAYERS, 0, sizeof(GDI_LAYERS));
    memset(&gdi_layer_info, 0, sizeof(gdi_layer_info));
    for (i = 0; i < GDI_LAYER_TOTAL_LAYER_COUNT; i++)
    {
        GDI_LAYERS[i].id = (U8)i;
    }

    /* init layer stack */
    gdi_layer_stack_index = 0;

    /* non_block blt */
    gdi_is_non_block_blt = FALSE;
    gdi_draw_section_level = 0;
    gdi_is_blting = FALSE;


    /* init mian lcd base layer */
    cf = GDI_COLOR_FORMAT_16;
    gdi_layer_create_internal(
        &GDI_LAYERS[0],
        0,
        0,
        GDI_LCD_WIDTH,
        GDI_LCD_HEIGHT,
        cf,
        FALSE,
        buf,
        NULL,
        size
        );
    GDI_LAYER_SET_FLAG(((gdi_handle)(&GDI_LAYERS[0])), GDI_LAYER_FLAG_USE_OUTSIDE_MEMORY);
    gdi_layer_set_active(((gdi_handle)(&GDI_LAYERS[0])));
    gdi_layer_clear(GDI_LAYERS[0].background);
   

    /* misc, some flags */
    gdi_is_layer_enable = FALSE;

    /* no palette is allocated */
    gdi_palette_used = 0;

    return GDI_SUCCEED;
}

void gdi_init(uint8_t *frame_buffer, int32_t buffer_size)
{
    gdi_layer_init(frame_buffer, buffer_size);
}
#ifdef _MSC_VER
#else

void graphic_lib_blt(void);
#endif
void gdi_flush_screen()
{
#ifdef _MSC_VER
	UpdateScreen();
#else
    GRAPHICLOG("gdi_flush_screen");
    graphic_lib_blt();
#endif
}

