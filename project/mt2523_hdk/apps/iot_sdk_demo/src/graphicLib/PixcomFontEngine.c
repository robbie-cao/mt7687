
#if defined(_MSC_VER)
#define DEBUG_VC
#endif

#ifdef DEBUG_VC
#include "gdi_type_adaptor.h"
#else
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#endif

#include "PixcomFontEngine.h"
#include "FontRes.h"
#include "gdi_type_adaptor.h"
#include "gdi_datatype.h"
#include "gdi_const.h"
#include "gdi_include.h"

uint8_t gnCurrentFontAttrib = FONTATTRIB_NORMAL;

uint32_t gnCurrentFont;
const sCustFontData *gpCurrentFont = NULL;

extern font_group_struct gfontfamily[MAX_FONT_SIZE];

extern int gdi_show_char(
            int32_t x,
            int32_t y,
            gdi_color color,
            uint8_t *font_data,
            uint32_t font_data_size,
            uint16_t char_width,
            uint16_t char_height,
            uint8_t font_attr,
            bool is_gray_bitmap);

static bool mmi_fe_test_display_as_space(uint32_t ch)
{
    if (FONT_TEST_CONTROL_CHAR(ch) || FONT_TEST_SPACE_CHAR(ch))
    {
        return true;
    }
    return false;
}

static bool mmi_fe_test_display_as_invalid(uint32_t ch)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((FONT_TEST_DATE_CHARS(ch)) || (ch == 0x0086))
    {
        return true;
    }
    return false;
}

static int32_t SearchIndexinList(uint8_t nListIndex, uint16_t nFontCount, uint32_t nCh, uint32_t *pnBase)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t nFirst = 0;
    int32_t nLast = gfontfamily[gnCurrentFont].fontData[nFontCount]->pRangeDetails->nNoOfRanges - 1;
    const RangeData *pRangeData = gfontfamily[gnCurrentFont].fontData[nFontCount]->pRangeDetails->pRangeData;
    uint16_t *pRange = gfontfamily[gnCurrentFont].fontData[nFontCount]->pRange;
    int16_t nMid;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while (nLast >= nFirst)
    {
        nMid = (nFirst + nLast) / 2;
        if ((nCh >= pRangeData[nMid].nMin) && (nCh <= pRangeData[nMid].nMax))
        {
            *pnBase = pRangeData[nMid].nMin;
            return pRange[nMid];

        }
        if (nCh > pRangeData[nMid].nMin)
        {
            nFirst = nMid + 1;
        }
        else
        {
            nLast = nMid - 1;
        }

    }

    return -1;
}

static void GetLangIndex(uint8_t *pCount, int32_t *pnFontIndex, int32_t *pnIndex, uint32_t nCh, uint32_t *pnBase)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint16_t nFontCount = 0;
    const font_group_struct *font_group = &(gfontfamily[gnCurrentFont]);
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *pCount = 0;

    nFontCount = 0;

    while (nFontCount < font_group->nTotalFonts)
    {

        if (nCh >= font_group->fontData[nFontCount]->pRangeDetails->pRangeData[0].nMin &&
            nCh <=
            font_group->fontData[nFontCount]->pRangeDetails->pRangeData[font_group->fontData[nFontCount]->
                                                                        pRangeDetails->nNoOfRanges - 1].nMax)
        {
            /* pnIndex is index of range block, pnBase is index of character which is start of range block */
            *pnIndex = SearchIndexinList(*pCount, nFontCount, nCh, pnBase);
            if (*pnIndex == -1)
            {
                ++nFontCount;
            }
            else
            {
                gpCurrentFont = font_group->fontData[nFontCount];
                *pnFontIndex = nFontCount;
                return;
            }

        }
        else
        {
            ++nFontCount;
        }
    }   /* while(nFontCount < pFontFamily->nTotalFonts) */

    return;
}

#ifdef __ICCARM__
static uint32_t mmi_fe_get_font_data_internal(
#else
__inline static uint32_t mmi_fe_get_font_data_internal(
#endif
                        uint16_t Ch,
                        uint8_t type,
                        uint8_t **ppCharData,
                        int32_t *pnWidth,
                        int32_t *advance_x,
                        uint16_t *pnHeight,
                        uint16_t *glyph_width,
                        uint16_t *glyph_height,
                        int32_t *ascent,
                        int32_t *descent)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint8_t count;
    int32_t NumChar = 0;
    int32_t index = -1;
    int32_t nFontIndex = -1;
    uint32_t nIndexInList;
    uint32_t nBase;
    const sCustFontData *TempFont = gpCurrentFont;

    GetLangIndex(&count, &nFontIndex, &index, Ch, &nBase);

    if (index > -1)
    {
        TempFont = gpCurrentFont;
    }
    else
    {
        Ch = 65515; /* 0x4E01;//0x000F; */
		GetLangIndex(&count, &nFontIndex, &index, Ch, &nBase);
        if (index == -1)
        {
            *advance_x = 0;
            *pnWidth = 0;
            *glyph_height = 0;
            *glyph_width = 0;
            *ascent = 0;
            *descent = 0;
            return NumChar;
        }
    }

	*pnWidth = TempFont->nWidth;
    *advance_x = TempFont->nWidth;
    *pnHeight = TempFont->nHeight;
    *ascent = TempFont->nAscent;
    *descent = TempFont->nDescent;

    nIndexInList = index + (Ch - nBase);

	if (TempFont->nEquiDistant){
        NumChar = TempFont->nCharBytes;
        *ppCharData = (TempFont->pDataArray + (nIndexInList * NumChar));
    }
	else{
        *pnWidth = TempFont->pWidthArray[nIndexInList];
        *advance_x = TempFont->pWidthArray[nIndexInList];
        NumChar = (TempFont->pOffsetArray[nIndexInList + 1] - TempFont->pOffsetArray[nIndexInList]);
        *ppCharData = (TempFont->pDataArray + TempFont->pOffsetArray[nIndexInList]);

	}

	*glyph_height = *pnHeight;
	*glyph_width = *pnWidth;

    return NumChar;
}

#ifdef __ICCARM__
static void mmi_fe_show_font_data(
#else
__inline static void mmi_fe_show_font_data(
#endif
                        uint16_t ch,
                        int32_t x,
                        int32_t y,
						gdi_color color,
                        uint8_t *glyph_ptr,
                        uint32_t glyph_size,
                        uint32_t glyph_attr,
                        uint16_t glyph_width,
                        uint16_t glyph_height,
                        uint32_t Bordered,
                        uint8_t font_attr
                        )
{
        gdi_show_char(x, y, color, glyph_ptr, glyph_size, glyph_width, glyph_height, font_attr, false);
}

void font_engine_get_string_width_height(uint8_t *string, int32_t len, int32_t *width_p, int32_t *height_p)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    uint8_t *CharData;
    uint32_t Counter = 0;
    int32_t nWidth;
    uint16_t unicode;
    uint16_t glyph_width;
    uint16_t glyph_height;
    int32_t ascent;
    int32_t descent;
    int32_t advance_x;
    uint16_t nHeight;

    gnCurrentFont = 0;
        
	*width_p = 0;
	*height_p = 0;
	
    while (len != 0)
    {
        if ((string[0] == '\0') && (string[1] == '\0'))
        {
            break;
        }
        len--;

        unicode = string[0];
        unicode |= (string[1] << 8);

        if (mmi_fe_test_display_as_space(unicode))
        {
            unicode = 0x20;
        }
        if (mmi_fe_test_display_as_invalid(unicode))
        {
            unicode = 0xFFEB;
        }

        mmi_fe_get_font_data_internal(
                    unicode,
                    0,//MMI_FE_GET_DATA,
                    &CharData,
                    &nWidth,
                    &advance_x,
                    &nHeight,
                    &glyph_width,
                    &glyph_height,
                    &ascent,
                    &descent);

        Counter++;
       
        *width_p += nWidth;
		if (nHeight > *height_p){
			*height_p = nHeight;
		}
			
        string += 2;
    }

}

void font_engine_show_string(int32_t x, int32_t y, uint8_t *string, int32_t len, gdi_color font_color)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    uint8_t *CharData;

    uint32_t NumChar;
    uint32_t Counter = 0;
    int32_t CurrentX;
    int32_t nWidth;
    uint16_t unicode;
    uint16_t glyph_width;
    uint16_t glyph_height;
    int32_t ascent;
    int32_t descent;
    int32_t advance_x;
    uint16_t nHeight;

	gdi_font_begin();

    gnCurrentFont = 0;
        
    CurrentX = x;
    while (len != 0)
    {
        if ((string[0] == '\0') && (string[1] == '\0'))
        {
            break;
        }
        len--;

        unicode = string[0];
        unicode |= (string[1] << 8);

        if (mmi_fe_test_display_as_space(unicode))
        {
            unicode = 0x20;
        }
        if (mmi_fe_test_display_as_invalid(unicode))
        {
            unicode = 0xFFEB;
        }

        NumChar = mmi_fe_get_font_data_internal(
                    unicode,
                    0,//MMI_FE_GET_DATA,
                    &CharData,
                    &nWidth,
                    &advance_x,
                    &nHeight,
                    &glyph_width,
                    &glyph_height,
                    &ascent,
                    &descent);

        Counter++;
        mmi_fe_show_font_data(
            unicode,
            CurrentX,
			y,
			font_color,
            CharData,
            NumChar,
            0,
            glyph_width,
            glyph_height,
            0,
            gnCurrentFontAttrib
			);

        CurrentX += nWidth;

        string += 2;
    }
}


