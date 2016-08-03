#include "gdi_const.h"
#include "gdi_datatype.h"
#include "gdi_include.h"

const U32 gdi_tilt_table[61] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 2,
    2, 2, 2, 2, 3, 3, 3, 3, 4, 4,
    4, 4, 5, 5, 5, 5, 5, 6, 6, 6,
    6, 7, 7, 7, 7, 8, 8, 8, 8, 8,
    9, 9, 9, 9, 10, 10, 10, 10, 11, 11,
    11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13
};
#define MAX_TILT  13

static S32 text_clip_x1;    /* Left edge of text clip rectangle */
static S32 text_clip_y1;    /* Top edge of text clip rectangle  */
static S32 text_clip_x2;    /* Right edge of text clip rectangle   */
static S32 text_clip_y2;    /* Bottom edge of text clip rectangle  */

#define GDI_CHECK_CHAR_CLIP(x,y,char_width,char_height,NULL_ACTION)                 \
   {                                                                                \
      if((x -1 > text_clip_x2   )    || (x+char_width  +MAX_TILT +1 < text_clip_x1) \
      ||(y -1 > text_clip_y2) || (y+char_height +1 < text_clip_y1))                 \
         NULL_ACTION;                                                               \
   }                                                                                \


static gd_put_pixel_func put_pixel;

#define put_pixel_with_clip(x,y,c)                    \
   {                                                  \
      if((x >= text_clip_x1) && (x <= text_clip_x2)   \
      &&(y >= text_clip_y1) && (y <= text_clip_y2))   \
         put_pixel(x,y,c);                            \
   }


#define __GDI_SHOW_CHAR__  gdi_show_char_no_clip
#define __GDI_SET_PIXEL__  put_pixel
#include "gdi_font_show_char.h"
#undef __GDI_SHOW_CHAR__
#undef __GDI_SET_PIXEL__

#define __GDI_SHOW_CHAR__  gdi_show_char_with_clip
#define __GDI_SET_PIXEL__  put_pixel_with_clip
#include "gdi_font_show_char.h"
#undef __GDI_SHOW_CHAR__
#undef __GDI_SET_PIXEL__


/*****************************************************************************
 * FUNCTION
 *  gdi_show_char
 * DESCRIPTION
 *  Low-level font drawing
 * PARAMETERS
 *  x                   [IN]        X
 *  y                   [IN]        Y
 *  color               [IN]        Font color
 *  font_data           [IN]        Raw font data
 *  font_data_size      [IN]        Font data size in bytes
 *  char_width          [IN]        Character width
 *  char_height         [IN]        Character height
 *  font_attr           [IN]        Font attributes
 * RETURNS
 *  void
 *****************************************************************************/
GDI_RESULT gdi_show_char(
            S32 x,
            S32 y,
            gdi_color color,
            U8 *font_data,
            U32 font_data_size,
            U16 char_width,
            U16 char_height,
            U8 font_attr,
            MMI_BOOL is_gray_bitmap)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_result ret = GDI_FAILED;


    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* Skip unnecessary action when the character is out of clip */
    GDI_CHECK_CHAR_CLIP(x, y, char_width, char_height, return GDI_SUCCEED);


    /* software font engine */
    if ((x - 1 < text_clip_x1) || (x + char_width + MAX_TILT + 1 > text_clip_x2) ||
             (y - 1 < text_clip_y1) || (y + char_height + 1 > text_clip_y2))
    {
        gdi_show_char_with_clip(x, y, color, font_data, font_data_size, char_width, char_height, font_attr);
    }
    else
    {
        gdi_show_char_no_clip(x, y, color, font_data, font_data_size, char_width, char_height, font_attr);
    }
    ret = GDI_SUCCEED;  //fix warning 
    return ret;
}


void gdi_font_begin()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    put_pixel = gd_put_pixel[GDI_LAYER.cf];

    text_clip_x1 = GDI_LAYER.clipx1;
    text_clip_y1 = GDI_LAYER.clipy1;
    text_clip_x2 = GDI_LAYER.clipx2;
    text_clip_y2 = GDI_LAYER.clipy2;

}

