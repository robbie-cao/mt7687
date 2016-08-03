#include "gdi_datatype.h"
#include "gdi_type_adaptor.h"
#include "gdi_const.h"
#include "gdi_include.h"
#include "gdi_primitive.h"

/****************************************************************************
* MACRO
*****************************************************************************/
#define SWAP(t,a,b) do {t tmp=a;a=b;b=tmp;} while(0)

#undef __inline
#define __inline 

/* check if pixel within clip, if not will return */
#define	GDI_CLIP_POINT_TEST(x, y)\
{\
	if( (x < GDI_LAYER.clipx1) || (x > GDI_LAYER.clipx2) ||\
		(y < GDI_LAYER.clipy1) || (y > GDI_LAYER.clipy2) )\
	{\
		GDI_RETURN_VOID;	\
	}\
}

#define	GDI_CLIP_TEXT_POINT_TEST(x, y)\
{\
	if( (x < GDI_LAYER.act_text_clip.x1) || (x > GDI_LAYER.act_text_clip.x2) ||\
		(y < GDI_LAYER.act_text_clip.y1) || (y > GDI_LAYER.act_text_clip.y2) )\
	{\
		GDI_RETURN_VOID;	\
	}\
}


#define GDI_CLIP_LINE_X_TEST(X1, X2)\
{\
	if(X1 < GDI_LAYER.clipx1)\
		X1 = GDI_LAYER.clipx1;\
	if(X2 > GDI_LAYER.clipx2)\
		X2 = GDI_LAYER.clipx2;\
}

#define GDI_CLIP_LINE_Y_TEST(Y1, Y2)\
{\
	if(Y1 < GDI_LAYER.clipy1)\
		Y1 = GDI_LAYER.clipy1;\
	if(Y2 > GDI_LAYER.clipy2)\
		Y2 = GDI_LAYER.clipy2;\
}

#define GDI_CLIP_LINE_XY_TEST(x1,y1,x2,y2)\
{\
	if(x1 < GDI_LAYER.clipx1)\
		x1 = GDI_LAYER.clipx1;\
	if(x2 > GDI_LAYER.clipx2)\
		x2 = GDI_LAYER.clipx2;\
	if(y1 < GDI_LAYER.clipy1)\
		y1 = GDI_LAYER.clipy1;\
	if(y2 > GDI_LAYER.clipy2)\
		y2 = GDI_LAYER.clipy2;\
}

#define AA_LINE_PRECISION_BITS 16
#define AA_LINE_FLOAT ((S32)((float)(x)*(float)(1 << AA_LINE_PRECISION_BITS)))



/* draw horizontal line */
__inline void GDI_DRAW_H_LINE(S32 x1, S32 x2, S32 y, gdi_color pixel_color)
{
		S32	h_index;
		for(h_index = x1; h_index <= x2; h_index++)
		{
			GDI_SET_BUFFER_PIXEL(h_index, y, pixel_color);
		}
}

__inline void GDI_DRAW_H_LINE_STYLE(S32 x1, S32 x2, S32 y, gdi_color pixel_color, U8 cycle, const U8 *bitvalues)
{
		S32		h_index;
		kal_uint8 bitcount;
		for(h_index = x1, bitcount=0; h_index <= x2; h_index++, bitcount++, bitcount %= cycle)
		{
			if (bitvalues[bitcount])
			{
				GDI_SET_BUFFER_PIXEL(h_index, y, pixel_color);
			}
		}
}

/* draw vertical line */
__inline void GDI_DRAW_V_LINE(S32 y1, S32 y2, S32 x, gdi_color pixel_color)
{
		S32	v_index;

		for(v_index = y1; v_index <= y2; v_index++)
		{
			GDI_SET_BUFFER_PIXEL(x,v_index,pixel_color);
		}
}

__inline void GDI_DRAW_V_LINE_STYLE(S32 y1, S32 y2, S32 x, gdi_color pixel_color, kal_uint8 cycle, const kal_uint8 *bitvalues)
{
		S32		v_index;
		U8		bitcount;
		for(v_index = y1, bitcount=0; v_index <= y2; v_index++, bitcount++, bitcount %= cycle)
		{
			if (bitvalues[bitcount])
			{
				GDI_SET_BUFFER_PIXEL(x, v_index, pixel_color);
			}
		}
}


/**************************************************************

	FUNCTION NAME		: gdi_draw_point()

  	PURPOSE				: draw a point

	INPUT PARAMETERS	: S32			x
							  S32			y
							  gdi_color pixel_color

	OUTPUT PARAMETERS	: nil

	RETURNS				: void

	REMARKS				: nil

***************************************************************/
__inline void gdi_draw_point(S32 x, S32 y, gdi_color pixel_color)
{
		GDI_CLIP_POINT_TEST(x,y);
		GDI_SET_BUFFER_PIXEL(x,y,pixel_color);
}


void gdi_draw_line_ext(S32 x1, S32 y1, S32 x2, S32 y2, gdi_color line_color, BOOL is_dotted)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    BOOL steep;                                             /* y-scan or x-scan */
    S32 slope, dx, dy, d, incrE, incrNE, x, y;              /* slope is the direction that y goes */
    S32 rate, tmp;                                          /* used in cliping */
    S32 clipx1, clipx2, clipy1, clipy2;                     /* the clip area */
    BOOL draw_dot = TRUE;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    {
        clipx1 = x1;    /* clipx1,y1,x2,y2 is only used as tmp variables here */
        clipy1 = y1;
        clipx2 = x2;
        clipy2 = y2;
        if (x1 > x2)
        {
            clipx1 = x2;
            clipx2 = x1;
        }
        if (y1 > y2)
        {
            clipy1 = y2;
            clipy2 = y1;
        }
        /* return if the line is not in the clip region */
        GDI_CLIP_RECT_TEST(clipx1, clipy1, clipx2, clipy2, GDI_RETURN_VOID);
    }
    
    if ((!is_dotted) && (y1 == y2) && (y1 >= GDI_LAYER.clipy1) && (y1 <= GDI_LAYER.clipy2)) // Horizontal Line /
	{
		if(x1 > x2)     // exchange x /
		{
		    tmp = x1;
            x1 = x2;
            x2 = tmp;
		}
        if((x1 <= GDI_LAYER.clipx2) && (x2 >= GDI_LAYER.clipx1))
        {
            GDI_CLIP_LINE_X_TEST(x1,x2);
            GDI_DRAW_H_LINE(x1, x2, y1, line_color);
    	}
	}
	else if ((!is_dotted) && (x1 == x2) && (x1 >= GDI_LAYER.clipx1) && (x1 <= GDI_LAYER.clipx2)) // Vertical Line /
    {
		if(y1 > y2)     // exchange y /
		{
			tmp = y1;
            y1 = y2;
            y2 = tmp;
		}
        if((y1 <= GDI_LAYER.clipy2) && (y2 >= GDI_LAYER.clipy1))
        {
            GDI_CLIP_LINE_Y_TEST(y1,y2);
            GDI_DRAW_V_LINE(y1, y2, x1, line_color);
	    }
	}
    else
    {
        /* clip */
        gdi_layer_get_clip(&clipx1, &clipy1, &clipx2, &clipy2);

        if (x1 > x2)    
        {
            /* exchange two points: (x1,y1) and (x2,y2)*/
            tmp = x1;
            x1 = x2;
            x2 = tmp;

            tmp = y1;
            y1 = y2;
            y2 = tmp;
        }
        dx = x2 - x1;
        dy = y2 - y1;
        
        if (dx == 0)
        {
            rate = 0;
        }
        else
        {
            rate = (dy << 20) / dx;
        }
        
        if (y1 <= y2) 
    	{
    		if (y1 < clipy1) 
    		{
    			int orgy = y1;
    			y1 = clipy1;
    			if (y1 > y2)
    				GDI_RETURN_VOID;
    			x1 = x1 - ((orgy - y1) * dx) / dy;
    		}

    		if (y2 > clipy2) 
    		{
    			int orgy = y2;
    			y2 = clipy2;
    			if (y1 > y2)
    				GDI_RETURN_VOID;
    			x2 = x2 - ((orgy - y2) * dx) / dy;
    		}
    	} 
    	else 
    	{ /* y1 > y2 */
    		if (y2 < clipy1) 
    		{
    			int orgy = y2;
    			y2 = clipy1;
    			if (y2 > y1)
    				GDI_RETURN_VOID;
    			x2 = x2 - ((orgy - y2) * dx) / dy;
    		}

    		if (y1 > clipy2) 
    		{
    			int orgy = y1;
    			y1 = clipy2;
    			if (y2 > y1)
    				GDI_RETURN_VOID;
    			x1 = x1 - ((orgy - y1) * dx) / dy;
    		}
    	}

    	if (x1 < clipx1) 
    	{
    		int orgx = x1;
    		x1 = clipx1;
    		if (x1 > x2)
    			GDI_RETURN_VOID;
    		y1 = y1 - (((orgx - x1) * rate) >> 20);
    	}

    	if (x2 > clipx2) 
    	{
    		int orgx = x2;
    		x2 = clipx2;
    		if (x1 > x2)
    			GDI_RETURN_VOID;
    		y2 = y2 - (((orgx - x2) * rate) >> 20);
    	}
        
        /* if the slope>1, need to exchange x-axis and y-axis */
        steep = (abs(y2 - y1) > abs(x2 - x1));  
        if (steep)
        {
            /* exchange x1 and y1, x2 and y2 */
            tmp = x1;
            x1 = y1;
            y1 = tmp;

            tmp = x2;
            x2= y2;
            y2 = tmp;
        }
        
        if (x1 > x2)    
        {
            /* exchange two points: (x1,y1) and (x2,y2) */
            tmp = x1;
            x1 = x2;
            x2 = tmp;

            tmp = y1;
            y1 = y2;
            y2 = tmp;
        }
        
        dx = x2 - x1;
        dy = y2 - y1;
        
        if (dy < 0)
        {
            slope = -1;
            dy = -dy;
        }
        else
        {
            slope = 1;
        }

        /* variables for mid-point draw line algorithm */
        d = (2 * dy) - dx;
        incrE = 2 * dy;
        incrNE = 2 * (dy - dx);

        for (x = x1, y = y1; x <= x2; x++)
        {
            S32 outx, outy;
            if (steep)
            {
                outx = y;
                outy = x;
            }
            else
            {
                outx = x;
                outy = y;
            }

            if (is_dotted)
            {
                if (draw_dot == FALSE)
                {
                    draw_dot = TRUE;
                }
                else
                {
                    GDI_CLIP_POINT_TEST(outx,outy);
                    gdi_act_put_pixel(outx, outy, line_color);
                    draw_dot = FALSE;
                }
            }
            else
            {
                GDI_CLIP_POINT_TEST(outx,outy);
                gdi_act_put_pixel(outx, outy, line_color);
            }
 
            if (d < 0)
            {
                d += incrE;
            }
            else
            {
                d += incrNE;
                y += slope;
            }
        }
    }
 
}

/**************************************************************

	FUNCTION NAME		: gdi_draw_line()

  	PURPOSE				: draw a line

	INPUT PARAMETERS	: S32			x1
							  S32			y1
							  S32			x2
							  S32			y2
							  gdi_color line_color

	OUTPUT PARAMETERS	: nil

	RETURNS				: void

	REMARKS				: nil

***************************************************************/
void gdi_draw_line(S32 x1, S32 y1, S32 x2, S32 y2, gdi_color line_color)
{
	gdi_draw_line_ext(x1,y1,x2,y2, line_color,FALSE);
}

#if 0
static __inline void GDI_DRAW_RECT(S32 x1, S32 y1, S32 x2, S32 y2, gdi_color pixel_color)
{
	S32	r_index;
	for(r_index = y1; r_index <=y2; r_index++)
	{
		GDI_DRAW_H_LINE(x1, x2, r_index, pixel_color);
	}
}
#endif

static __inline void GDI_DRAW_RECT16(S32 x1, S32 y1, S32 x2, S32 y2, gdi_color pixel_color)
{
	U8* dest = GDI_LAYER.buf_ptr + ((x1+y1*GDI_LAYER.width)<<1);

	y2 -=y1; 
	y2++;

	x2-=x1;
	x2++;
	x2<<=1;
	x1 = GDI_LAYER.width<<1;
	
	while(y2--)
	{
		gdi_memset16(dest,(U16)pixel_color,x2);
		dest +=x1;
	}
}


/**************************************************************

	FUNCTION NAME		: gdi_draw_rect()

  	PURPOSE				: draw a rect (border only)

	INPUT PARAMETERS	: S32			x1
							  S32			y1
							  S32			x2
							  S32			y2
							  gdi_color frame_color

	OUTPUT PARAMETERS	: nil

	RETURNS				: void

	REMARKS				: nil

***************************************************************/
void gdi_draw_rect(S32 x1, S32 y1, S32 x2, S32 y2, gdi_color frame_color)
{
		S32 nx1,ny1,nx2,ny2;

		nx1 = x1;
		ny1 = y1;
		nx2 = x2;
		ny2 = y2;

		GDI_CLIP_RECT_TEST(nx1,ny1,nx2,ny2,GDI_RETURN_VOID);

		if(GDI_LAYER.clipy1<= y1 && y1 <= GDI_LAYER.clipy2)
			GDI_DRAW_H_LINE(nx1, nx2, y1, frame_color);	/* up horizontal */

		if(GDI_LAYER.clipy1<= y2 && y2 <= GDI_LAYER.clipy2)
			GDI_DRAW_H_LINE(nx1, nx2, y2, frame_color);	/* down horizontal */

		if(GDI_LAYER.clipx1<= x1 && x1 <= GDI_LAYER.clipx2)
			GDI_DRAW_V_LINE(ny1, ny2, x1, frame_color);	/* left vertical */

		if(GDI_LAYER.clipx1<= x2 && x2 <= GDI_LAYER.clipx2)
			GDI_DRAW_V_LINE(ny1, ny2, x2, frame_color);	/* right vertical */
		
}



/**************************************************************

	FUNCTION NAME		: gdi_draw_solid_rect()

  	PURPOSE				: draw a solid rect

	INPUT PARAMETERS	: S32			x1
							  S32			y1
							  S32			x2
							  S32			y2
							  gdi_color rect_color

	OUTPUT PARAMETERS	: nil

	RETURNS				: void

	REMARKS				: nil

***************************************************************/
void gdi_draw_solid_rect(S32 x1, S32 y1, S32 x2, S32 y2, gdi_color rect_color)
{
    do
    {
        GDI_CLIP_RECT_TEST(x1,y1,x2,y2,break);
    
        if(GDI_LAYER.bits_per_pixel == 16)
        {
            if (GDI_LAYER.cf == GDI_COLOR_FORMAT_UYVY422)
            {
                rect_color = 0x0080; /* Only support clear to black */
            }
        
            GDI_DRAW_RECT16(x1, y1, x2, y2, rect_color);
        }
    }while(0);
}


/*****************************************************************************
 * FUNCTION
 *  gd_null_pointer_function
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void gd_null_pointer_function(void)
{
    //GDI_DEBUG_ASSERT(0);
}


#define gd_color_from_rgb_main      GD_FUNC_16(gd_color_from_rgb)
#define gd_color_to_rgb_main        GD_FUNC_16(gd_color_to_rgb)
#define gd_get_pixel_main           GD_FUNC_16(gd_get_pixel)
#define gd_put_pixel_main           GD_FUNC_16(gd_put_pixel)


#define gd_color_from_rgb_sub       gd_null_pointer_function
#define gd_color_to_rgb_sub         gd_null_pointer_function
#define gd_get_pixel_sub            gd_null_pointer_function
#define gd_put_pixel_sub            gd_null_pointer_function
#define gd_put_pixel_to_layer_sub   gd_null_pointer_function
#define gd_get_buf_pixel_sub        gd_null_pointer_function
#define gd_put_buf_pixel_sub        gd_null_pointer_function

///////////////////////////////////////////////////////////////////////////////////
// define the default main func
///////////////////////////////////////////////////////////////////////////////////
#define GD_FUNC_MAIN(NAME)       GD_FUNC_16(NAME)

///////////////////////////////////////////////////////////////////////////////////
// define the default sub func
///////////////////////////////////////////////////////////////////////////////////
#define GD_FUNC_SUB(NAME)       (NAME##_func) gd_null_pointer_function


///////////////////////////////////////////////////////////////////////////////////
// define the default color format function
///////////////////////////////////////////////////////////////////////////////////
#define GD_FUNC_1(NAME)         (NAME##_func) gd_null_pointer_function
#define GD_FUNC_8(NAME)         (NAME##_func) gd_null_pointer_function //NAME##_8
#define GD_FUNC_16(NAME)        (NAME##_func) NAME##_16
#define GD_FUNC_24(NAME)        (NAME##_func) gd_null_pointer_function //NAME##_24
#define GD_FUNC_32(NAME)        (NAME##_func) gd_null_pointer_function //NAME##_32
#define GD_FUNC_32PARGB(NAME)   (NAME##_func) gd_null_pointer_function //NAME##_32PARGB
#define GD_FUNC_UYVY422(NAME)   (NAME##_func) gd_null_pointer_function
#define GD_FUNC_A8(NAME)        (NAME##_func) gd_null_pointer_function //NAME##_8

#define GD_FUNC_PARGB6666(NAME) (NAME##_func) gd_null_pointer_function

/* Venus requires 32-bit layer support even without HW support. */
#if !defined(GDI_USING_LAYER_COLOR_FORMAT) && !defined(__VENUS_UI_ENGINE__)
    #if GDI_MAINLCD_BIT_PER_PIXEL != 8 && GDI_SUBLCD_BIT_PER_PIXEL != 8
    #undef  GD_FUNC_8
    #define GD_FUNC_8(NAME)         (NAME##_func) gd_null_pointer_function
    #endif
    #if GDI_MAINLCD_BIT_PER_PIXEL != 16 && GDI_SUBLCD_BIT_PER_PIXEL != 16
    #undef  GD_FUNC_16
    #define GD_FUNC_16(NAME)        (NAME##_func) gd_null_pointer_function
    #endif
    #if GDI_MAINLCD_BIT_PER_PIXEL != 24 && (!defined(GDI_SUBLCD_BIT_PER_PIXEL) || (GDI_SUBLCD_BIT_PER_PIXEL != 24))
    #undef  GD_FUNC_24
    #define GD_FUNC_24(NAME)        (NAME##_func) gd_null_pointer_function
    #endif
    #if GDI_MAINLCD_BIT_PER_PIXEL != 32 && (!defined(GDI_SUBLCD_BIT_PER_PIXEL) || (GDI_SUBLCD_BIT_PER_PIXEL != 32))
    #undef  GD_FUNC_32
    #define GD_FUNC_32(NAME)        (NAME##_func) gd_null_pointer_function
    #endif

    #undef  GD_FUNC_32PARGB
    #define GD_FUNC_32PARGB(NAME)   (NAME##_func) gd_null_pointer_function
    
#endif /* !defined(GDI_USING_LAYER_COLOR_FORMAT) && !defined(__VENUS_UI_ENGINE__) */

/* Convert color from ARGB8888 */
const gd_color_from_rgb_func gd_color_from_rgb[] = 
{
    GD_FUNC_1       (gd_color_from_rgb),            /* 1 */
    GD_FUNC_8       (gd_color_from_rgb),            /* 8 */
    GD_FUNC_16      (gd_color_from_rgb),            /* 16 */
    GD_FUNC_24      (gd_color_from_rgb),            /* 24 */
    GD_FUNC_32      (gd_color_from_rgb),            /* 32 */
    GD_FUNC_32PARGB (gd_color_from_rgb),            /* 32PARGB */
    (gd_color_from_rgb_func) gd_null_pointer_function,                      /* UYVY422 work around for gdi_layer_clear */
    GD_FUNC_A8      (gd_color_from_rgb),
    (gd_color_from_rgb_func)gd_color_from_rgb_main, /* main */
    (gd_color_from_rgb_func)gd_color_from_rgb_sub   /* sub */
};

/* Convert color to ARGB8888 */
const gd_color_to_rgb_func gd_color_to_rgb[] = 
{
    GD_FUNC_1       (gd_color_to_rgb),          /* 1 */
    GD_FUNC_8       (gd_color_to_rgb),          /* 8 */
    GD_FUNC_16      (gd_color_to_rgb),          /* 16 */
    GD_FUNC_24      (gd_color_to_rgb),          /* 24 */
    GD_FUNC_32      (gd_color_to_rgb),          /* 32 */
    GD_FUNC_32PARGB (gd_color_to_rgb),          /* 32PARGB */
    GD_FUNC_UYVY422 (gd_color_to_rgb),          /* UYVY422 */
    GD_FUNC_A8      (gd_color_to_rgb),
    (gd_color_to_rgb_func)gd_color_to_rgb_main, /* main */
    (gd_color_to_rgb_func)gd_color_to_rgb_sub   /* sub */
};

const gd_get_pixel_func gd_get_pixel[] = 
{
    GD_FUNC_1       (gd_get_pixel),         /* 1 */
    GD_FUNC_8       (gd_get_pixel),         /* 8 */
    GD_FUNC_16      (gd_get_pixel),         /* 16 */
    GD_FUNC_24      (gd_get_pixel),         /* 24 */
    GD_FUNC_32      (gd_get_pixel),         /* 32 */
    GD_FUNC_32PARGB (gd_get_pixel),         /* 32PARGB */
    GD_FUNC_UYVY422 (gd_get_pixel),         /* UYVY422 */
    GD_FUNC_A8      (gd_get_pixel),
    (gd_get_pixel_func)gd_get_pixel_main,   /* main */
    (gd_get_pixel_func)gd_get_pixel_sub     /* sub */
};

const gd_put_pixel_func gd_put_pixel[] = 
{
    GD_FUNC_1       (gd_put_pixel),         /* 1 */
    GD_FUNC_8       (gd_put_pixel),         /* 8 */
    GD_FUNC_16      (gd_put_pixel),         /* 16 */
    GD_FUNC_24      (gd_put_pixel),         /* 24 */
    GD_FUNC_32      (gd_put_pixel),         /* 32 */
    GD_FUNC_32PARGB (gd_put_pixel),         /* 32PARGB */
    GD_FUNC_UYVY422 (gd_put_pixel),         /* UYVY422 */
    GD_FUNC_A8      (gd_put_pixel),
    (gd_put_pixel_func)gd_put_pixel_main,   /* main */
    (gd_put_pixel_func)gd_put_pixel_sub     /* sub */
};


const gd_replace_src_key_func gd_replace_src_key[] = 
{
    GD_FUNC_1       (gd_replace_src_key),   /* 1 */
    GD_FUNC_8       (gd_replace_src_key),   /* 8 */
    GD_FUNC_16      (gd_replace_src_key),   /* 16 */
    GD_FUNC_24      (gd_replace_src_key),   /* 24 */
    GD_FUNC_32      (gd_replace_src_key),   /* 32 */
    GD_FUNC_32PARGB (gd_replace_src_key),   /* 32PARGB */
    GD_FUNC_UYVY422 (gd_replace_src_key),   /* UYVY422 */
    (gd_replace_src_key_func) gd_null_pointer_function, /* A8 */
    GD_FUNC_MAIN    (gd_replace_src_key),   /* main */
    GD_FUNC_SUB     (gd_replace_src_key)    /* sub */
};

#if (0)
//#define __RVCT__
#ifndef __RVCT__

__inline int GDI_BYTE16SET(int sizeReg, int add, int a)
{
    __asm
	{
	        MOV r0, a;
	        MOV r1, r0;
	        MOV r2, r0;
	        MOV r3, r0;
	        MOV r4, add;
	        MOV r5, sizeReg;
	    CPYLOOP:
	        STMIA r4!, {r0, r1, r2, r3};
	        CMP r5, r4;
	        BGT CPYLOOP;
	        MOV add, r4;
	};

	return add;
}

#else


__asm void GDI_BYTE32SET(kal_uint32 *addr_end, kal_uint32 *addr, kal_uint32 val)
{
    ARM
    STMDB sp!, {r4-r9}

    MOV r9, r0 ; addr_end
    MOV r8, r1 ; addr

    ; prepare data registers
    MOV r0, r2
    MOV r1, r0
    MOV r2, r0
    MOV r3, r0
    MOV r4, r0
    MOV r5, r0
    MOV r6, r0
    MOV r7, r0
    
GDI_BYTE32SET_CPYLOOP
    STMIA r8!, {r0-r7}
    CMP r8, r9
    BLT GDI_BYTE32SET_CPYLOOP ; addr < addr_end

    LDMIA sp!, {r4-r9}
    
    BX lr
}


__asm int GDI_BYTE16SET_ONCE(kal_uint32 *addr, kal_uint32 val)
{
    ARM
    STMDB sp!, {r4}

    ; prepare data registers
    MOV r2, r1
    MOV r3, r1
    MOV r4, r1

    STMIA r0!, {r1-r4}
    LDMIA sp!, {r4}
    
    BX lr
}


#endif

void gdi_memset16(U8* address,U16 pixel,int size)
{
    if(size < 20)
    {
        register int add = (int)address;
        int sizeR = size;
        int postcount;
        if(add & 0x3)
        {
            *((unsigned short*)add) = pixel;
            add += 2;
            sizeR -= 2;
        }
        postcount = sizeR & 0x3;
        sizeR -= postcount;
        if(sizeR > 0)
        {
            register unsigned int a;
            register int sizeReg = sizeR;
            register int endAdd = add + sizeReg;
            a = pixel + (pixel << 16);
            do
            {
                ((unsigned int*)add)[0] = a;
                add += 4;
            }while(add < endAdd);
        }
        if(postcount != 0)
        {
            *((unsigned short*)add) = pixel;
        }
    }
    else
#ifdef __RVCT__ /* No non-RVCT version of GDI_BYTE32SET(), GDI_BYTE16SET_ONCE() */
    {
        register kal_uint8 *addr = (kal_uint8*)address;
        kal_int32 sizeR = size;
        kal_int32 postcount;
        kal_uint32 d_pixel = ((pixel << 16) | pixel);

        if ( (((kal_uint32)addr) & 0x2) != 0 )
        {
            /* Assume addr is 2-byte aligned */
            *((kal_uint16*)addr) = pixel;
            addr += 2;
            sizeR -= 2;
        }

        /* Now addr is 4-byte aligned */

        if (sizeR >= 32)
        {
            register kal_uint8 *addr_end;

            postcount = (sizeR & 0x01f);
            sizeR -= postcount;
            addr_end = addr + sizeR;
            GDI_BYTE32SET((kal_uint32*)addr_end, (kal_uint32*)addr, d_pixel);
            
            addr = addr_end;
            sizeR = postcount;
        }
        
        if (sizeR >= 16)
        {
            /* Now sizeR < 32 */
            GDI_BYTE16SET_ONCE((kal_uint32*)addr, d_pixel);
            
            addr += 16;
            sizeR -= 16;
        }

        postcount = sizeR & 0x2;
        sizeR = ((sizeR >> 2) << 2);
        if (sizeR != 0)
        {
            kal_uint8* addr_end = addr + sizeR;
            do
            {
                *((kal_uint32*)addr) = d_pixel;
                addr += 4;
            } while (addr != addr_end);
        }

        if (postcount != 0)
        {
            *((kal_uint16*)addr) = pixel;
        }
    }
#else
	{
		register int add = (int)address;
		int sizeR = size;
		int postcount;
		register unsigned int a;
		if(add & 0x3)
		{
			*((unsigned short*)add) = pixel;
			add += 2;
			sizeR -= 2;
		}
		postcount = (((unsigned int)sizeR) &0x00f);
		sizeR -= postcount;
		{
			register int sizeReg = sizeR + (int)add;
			a = pixel + (pixel << 16);
			add = GDI_BYTE16SET(sizeReg, add, a);
		}
		sizeR = ((postcount >> 2) << 2);
		if(sizeR > 0)
		{
			register int sizeReg = sizeR;
			register int endAdd = add + sizeReg;
			do
			{
				((unsigned int*)add)[0] = a;
				add += 4;
			}while(add < endAdd);
		}
		if((postcount & 0x3) != 0)
		{
			*((unsigned short*)add) = pixel;
		}
	}
#endif
}
#else
void gdi_memset16(U8* address,U16 pixel,int size)
{
	if (size & 0x03){
		*((uint16_t*)(address + size - 2)) = pixel;
		size = (size >> 2) << 2;
	}
	while (size) {
		((uint32_t *)address)[0] = (pixel << 16) | (pixel);
		address += 4;
		size -= 4;
	}
}
#endif

