#ifndef _FONTRES_H_
#define _FONTRES_H_

#if defined(_MSC_VER)
#include "gdi_type_adaptor.h"
#else
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#endif

#define  MAX_FONT_TYPES      6

#define  FONTATTRIB_NORMAL                   0x00000001

typedef struct Range
{
    uint16_t nMin;
    uint16_t nMax;
} RangeData;

typedef struct RangeInfo
{
    uint16_t nNoOfRanges;
    const RangeData *pRangeData;

} RangeDetails;

typedef struct _CustFontData
{
    uint8_t nHeight;
    uint8_t nWidth;
    uint8_t nAscent;
    uint8_t nDescent;
    uint8_t nEquiDistant;
    uint8_t nCharBytes;
    uint16_t nMaxChars;
    uint8_t *pDWidthArray;
    uint8_t *pWidthArray;
    uint32_t *pOffsetArray;
    uint8_t *pDataArray;
    uint16_t *pRange;
    const RangeDetails *pRangeDetails;
    uint32_t language_flag;
} sCustFontData;

typedef struct _FontFamily
{
    uint16_t nTotalFonts;
    sCustFontData *fontData[MAX_FONT_TYPES];
} sFontFamily;

typedef struct 
{
    uint16_t nTotalFonts;
    const sCustFontData *const *fontData;
} font_group_struct;

#define MAX_FONT_SIZE MAX_FONT_TYPES

#endif /* _FONTRES_H_ */ 


