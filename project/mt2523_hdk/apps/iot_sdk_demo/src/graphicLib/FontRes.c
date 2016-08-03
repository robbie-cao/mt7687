#include "FontRes.h"

#if defined(_MSC_VER)
#include "gdi_type_adaptor.h"
#else
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#endif

#ifndef NULL
#define NULL  (void *)0           /*  NULL    :   Null pointer */
#endif

#include "L_English_small.h"
#include "L_gMTKProprietaryFont_small.h"

const sCustFontData * const g_small_font_data_array[] = {
&English_small,
&gMTKProprietaryFont_small
}; 

const sCustFontData * const g_medium_font_data_array[] = {
&English_small,
&gMTKProprietaryFont_small
}; 

const sCustFontData * const g_large_font_data_array[] = {
&English_small,
&gMTKProprietaryFont_small
}; 

const sCustFontData * const g_sublcd_font_data_array[] = {
&English_small,
&gMTKProprietaryFont_small
}; 

const sCustFontData * const g_dialling_font_data_array[] = {
&English_small,
&gMTKProprietaryFont_small
}; 

const sCustFontData * const g_touchscreen_font_data_array[] = {
&English_small,
&gMTKProprietaryFont_small
}; 

const font_group_struct gfontfamily[MAX_FONT_SIZE] = {
{2, g_small_font_data_array},
{2, g_medium_font_data_array},
{2, g_large_font_data_array},
{2, g_sublcd_font_data_array},
{2, g_dialling_font_data_array},
{2, g_touchscreen_font_data_array},
};


