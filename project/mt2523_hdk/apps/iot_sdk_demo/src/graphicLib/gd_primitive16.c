#include "gdi_type_adaptor.h"
#include "gdi_datatype.h"
#include "gdi_const.h"
#include "gdi_include.h"


/*****************************************************************************
 * FUNCTION
 *  gd_color_from_rgb_16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  A       [IN]        
 *  R       [IN]        
 *  G       [IN]        
 *  B       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
gdi_color gd_color_from_rgb_16(U32 A, U32 R, U32 G, U32 B)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return (gdi_color) ((((B) & 0xf8) >> 3) | (((G) & 0xfc) << 3) | (((R) & 0xf8) << 8));
}


/*****************************************************************************
 * FUNCTION
 *  gd_color_to_rgb_16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  A       [?]         
 *  R       [?]         
 *  G       [?]         
 *  B       [?]         
 *  c       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_color_to_rgb_16(U32 *A, U32 *R, U32 *G, U32 *B, gdi_color c)
{
    int t;
    *A = 0xFF;

    t = ((U32) (((c) & 0xf800) >> 8));
    *R = t | (t>>5);
    
    t = ((U32) (((c) & 0x07e0) >> 3));
    *G = t | (t>>6);

    t = ((U32) (((c) & 0x001f) << 3));
    *B = t | (t>>5);
}


/*****************************************************************************
 * FUNCTION
 *  gd_get_pixel_16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  x       [IN]        
 *  y       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
gdi_color gd_get_pixel_16(int x, int y)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return *(U16*) (gdi_act_layer->buf_ptr + ((y * gdi_act_layer->width + x) << 1));
}


/*****************************************************************************
 * FUNCTION
 *  gd_put_pixel_16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  x       [IN]        
 *  y       [IN]        
 *  c       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_put_pixel_16(int x, int y, gdi_color c)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *(U16*) (gdi_act_layer->buf_ptr + ((y * gdi_act_layer->width + x) << 1)) = (U16) c;
}


/*****************************************************************************
 * FUNCTION
 *  gd_put_pixel_to_layer_16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  x       [IN]        
 *  y       [IN]        
 *  c       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_put_pixel_to_layer_16(gdi_handle layer_handle, int x, int y, gdi_color c)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_layer_struct *layer = (gdi_layer_struct*)layer_handle;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *(U16*) (layer->buf_ptr + ((y * layer->width + x) << 1)) = (U16) c;
}


/*****************************************************************************
 * FUNCTION
 *  gd_get_buf_pixel_16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  buf_ptr         [?]         
 *  buf_width       [IN]        
 *  x               [IN]        
 *  y               [IN]        
 * RETURNS
 *  
 *****************************************************************************/
gdi_color gd_get_buf_pixel_16(U8 *buf_ptr, U32 buf_width, int x, int y)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return *(U16*) (buf_ptr + ((y * buf_width + x) << 1));
}


/*****************************************************************************
 * FUNCTION
 *  gd_put_buf_pixel_16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  buf_ptr         [?]         
 *  buf_width       [IN]        
 *  x               [IN]        
 *  y               [IN]        
 *  c               [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_put_buf_pixel_16(U8 *buf_ptr, U32 buf_width, int x, int y, gdi_color c)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *(U16*) (buf_ptr + ((y * buf_width + x) << 1)) = (U16) c;
}


/*****************************************************************************
 * FUNCTION
 *  gd_replace_src_key_16
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dest_ptr            [?]         
 *  dest_pitch          [IN]        
 *  dest_offset_x       [IN]        
 *  dest_offset_y       [IN]        
 *  clipx1              [IN]        
 *  clipy1              [IN]        
 *  clipx2              [IN]        
 *  clipy2              [IN]        
 *  src_key             [IN]        
 *  width               [IN]        
 *  height              [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_replace_src_key_16(
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
        S32 height)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    U8 *dest_line_start_ptr;

    S32 jump_size;
    S32 start_x, start_y, end;
    S32 x_draw_count, y_draw_count;

    /* if not totally out of clip region */
    if (!((dest_offset_x > clipx2) ||
          (dest_offset_y > clipy2) || (dest_offset_x + width < clipx1) || (dest_offset_y + height < clipy1)))
    {
        /* find x_draw_count and y_draw_count */
        start_x = (dest_offset_x < clipx1) ? clipx1 : dest_offset_x;
        end = (dest_offset_x + (S32) width - 1 > clipx2) ? clipx2 : dest_offset_x + (S32) width - 1;
        x_draw_count = end - start_x + 1;

        start_y = (dest_offset_y < clipy1) ? clipy1 : dest_offset_y;
        end = (dest_offset_y + (S32) height - 1 > clipy2) ? clipy2 : dest_offset_y + (S32) height - 1;
        y_draw_count = end - start_y + 1;

        /* adjust dest ptr to the start of drawing point */
        /* find start ptr for actual drawing */
        jump_size = ((start_y * dest_pitch + start_x) * 16) >> 3;
        dest_line_start_ptr = dest_ptr + jump_size;
        
        {
            U16 *dest;
            S32 x, y;

            for (y = 0; y < y_draw_count; y++)
            {
                dest = (U16*) (dest_line_start_ptr + ((dest_pitch * y * 16) >> 3));

                for (x = 0; x < x_draw_count; x++)
                {
                    U16 c = *dest;

                    if (c == src_key)
                    {
                        c ^= 1;
                        *dest = c;
                    }
                    dest++;
                }
            }
        }
    }
}

