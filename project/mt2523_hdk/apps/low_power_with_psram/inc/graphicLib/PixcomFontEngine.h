



#ifndef _PIXCOMFONTENGINE_H
#define _PIXCOMFONTENGINE_H

/* to check the control chars
   add 0x0A,0x0D~0x0F for displaying control characters as space
   add 0xA0 for NBSP in Vietnamese as space */
#define FONT_TEST_CONTROL_CHAR(c)      \
(     (c=='\t'/* 0x09 */)                \
   || (c==0x08)                        \
   || (c==0x0A)                        \
   || (c==0x0D)                        \
   || (c==0x0E)                        \
   || (c==0x0F)                        \
   || (c==0xA0)                        \
   || (c== 0x2028)                    \
   || (c == 0x2029)                   \
)
#define FONT_TEST_DATE_CHARS(c)   ((c) >= 0x87 && (c)<= 0x94)    
#define FONT_TEST_SPACE_CHAR(c) (((c) >= 0x2000 && (c)<= 0x200A) || ((c) == 0xfeff)) /* 0x200B is zero space but we treat it as null character. */
#define FONT_TEST_ZERO_WIDTH_SPACE(c) ((c) == 0x200B)
#define FONT_TEST_ZERO_WIDTH_CHAR(c) (((c)<= 0x1F) || (((c)>= 0x80) && ((c)<= 0x84)) || (((c)>= 0x95) && ((c)<= 0x9f)))
extern void gdi_font_begin(void);

#endif /* _PIXCOMFONTENGINE_H */


