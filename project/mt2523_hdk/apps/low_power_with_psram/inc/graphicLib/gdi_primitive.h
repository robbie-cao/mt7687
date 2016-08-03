#include <string.h>
#include <stdio.h>
#include "gdi_datatype.h"
#include "gdi_const.h"

#define GDI_CLIP_RECT_TEST(X1, Y1, X2, Y2,NULL_ACTION)    \
{                                                         \
   if(X1 > X2) { NULL_ACTION; }                           \
   else if(Y1 > Y2)  { NULL_ACTION; }                     \
   else if(X2 < GDI_LAYER.clipx1) { NULL_ACTION; }        \
   else if(Y2 < GDI_LAYER.clipy1) { NULL_ACTION; }        \
   else if(X1 > GDI_LAYER.clipx2) { NULL_ACTION; }        \
   else if(Y1 > GDI_LAYER.clipy2) { NULL_ACTION; }        \
   else                                                   \
   {                                                      \
      if(X1 < GDI_LAYER.clipx1) { X1 = GDI_LAYER.clipx1; }\
      if(Y1 < GDI_LAYER.clipy1) { Y1 = GDI_LAYER.clipy1; }\
      if(X2 > GDI_LAYER.clipx2) { X2 = GDI_LAYER.clipx2; }\
      if(Y2 > GDI_LAYER.clipy2) { Y2 = GDI_LAYER.clipy2; }\
   }                                                      \
}

/****************************************************************************
* function
*****************************************************************************/
extern void gdi_memset16(U8 *address, U16 pixel, int size);
extern void gdi_memset24(U8* address,U32 pixel,int size);
extern void gdi_memset32(U8* address,U32 pixel,int size);
void gdi_draw_solid_rect(S32 x1, S32 y1, S32 x2, S32 y2, gdi_color rect_color);

#define GDI_RETURN_VOID return

