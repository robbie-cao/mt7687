/*****************************************************************************
 * FUNCTION
 *  __GDI_SHOW_CHAR__
 * DESCRIPTION
 *  
 * PARAMETERS
 *  x                   [IN]        
 *  y                   [IN]        
 *  color               [IN]        
 *  font_data           [?]         
 *  font_data_size      [IN]        
 *  char_width          [IN]        
 *  char_height         [IN]        
 *  font_attr           [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
static void __GDI_SHOW_CHAR__(
                S32 x,
                S32 y,
                gdi_color color,
                U8 *font_data,
                U32 font_data_size,
                U16 char_width,
                U16 char_height,
                U8 font_attr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    S32 X1, Y1;
    S32 X2 = 0, Y2;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    X1 = x;
    Y1 = y;
    X2 = 0;

    /* draw underline */    
    while (font_data_size--)
    {
        U8 pattern = *font_data++;

        if (!pattern)
        {
            U32 nTemp;

            X2 += 8;
            nTemp = X2 / char_width;
            if (nTemp)
            {
                Y1 += nTemp;
                char_height -= (U16) nTemp;
                if (!char_height)
                {
                    return;
                }
            }
            X2 %= char_width;
            X1 = x + X2;
        }
        else
        {
#define FONT_DRAW_PATTERN(SET_PIXEL,XFUNC,YFUNC)                              \
            {                                                                          \
               for(Y2 = 0; Y2 < 8; Y2++)                                               \
               {                                                                       \
                  if(pattern&1)                                                        \
                     SET_PIXEL(XFUNC, YFUNC, color);                                   \
                  ++X2;                                                                \
                  if(X2 == (S32)( char_width ) )                                       \
                  {                                                                    \
                     X1=x;                                                             \
                     char_height--;                                                    \
                     ++Y1;                                                             \
                                                                                       \
                     if(char_height == 0)                                              \
                        return ;                                                       \
                     X2=0;                                                             \
                  }                                                                    \
                  else                                                                 \
                  {                                                                    \
                     ++X1;                                                             \
                  }                                                                    \
                  pattern >>=1;                                                        \
               }                                                                       \
            }
                FONT_DRAW_PATTERN(__GDI_SET_PIXEL__, X1, Y1)}
                }
                }



