#if defined(_MSC_VER)
#else
#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include "stdio.h"
#include "stdlib.h"
#include "graphic_log.h"

#endif
#include "graphic_interface.h"
#include "main_screen.h"
#include "memory_attribute.h"

#define CONFIG_INCLUDE_HEADER
#include "screen_config.h"
#undef CONFIG_INCLUDE_HEADER
#define CONFIG_INCLUD_BODY

#define DEMO_ITEM_NAME_MAX_LEN 50
#define PERVIOUS_PAGE_STRING_NAME "previous page"
#define NEXT_PAGE_STRING_NAME "next page"
#define DEMO_TITLE_STRING_NAME "Demo option:"
typedef struct list_item_struct {
    show_screen_proc_f show_screen_f;
    event_handle_func event_handle_f;
    uint8_t name[DEMO_ITEM_NAME_MAX_LEN];
} list_item_struct_t;


const list_item_struct_t demo_item[] =
{
#include "screen_config.h"
};

struct {
    int32_t start_item;
    int32_t curr_item_num;
    int32_t one_screen_item_num;
    int32_t total_item_num;
    int32_t top_gap;
    int32_t left_gap;
    int32_t right_gap;
    int32_t bottom_gap;
    int32_t line_gap;
    int32_t item_height;
    int32_t item_width;
    int32_t LCD_WIDTH;
    int32_t LCD_HEIGHT;
    int32_t has_previous_page;
    int32_t has_next_page;
    gdi_color color;
} main_screen_cntx;

static event_handle_func curr_event_handler;

#define ScrnWidth (240)
#define ScrnHeight (240)

ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN unsigned char frame_buffer[ScrnWidth*ScrnHeight*2];


static int32_t main_screen_get_index(int32_t x, int32_t y);
static void main_screen_draw(void);
static void main_screen_scroll_to_prevoius_page(void);
static void main_screen_scroll_to_next_page(void);

void main_screen_event_handle(message_id_enum event_id, int32_t param1, void* param2)
{
}

void main_screen_pen_event_handler(touch_event_struct_t* touch_event, void* user_data)
{
    static int32_t item_down_index = -1;
	int32_t temp_index;
    
    temp_index = main_screen_get_index(touch_event->position.x, touch_event->position.y);
	if (touch_event->type == TOUCH_EVENT_DOWN) {
		item_down_index = temp_index;
		return;
	} else if (touch_event->type == TOUCH_EVENT_UP) {
		if (item_down_index == -1) {
			return;
		}
	}

	if (item_down_index != temp_index) {
		item_down_index = -1;
		return;
	}

	item_down_index = -1;

    switch (temp_index) {
        case -1:
            return;
        case -2:
            main_screen_scroll_to_prevoius_page();
            break;
        case -3:
            main_screen_scroll_to_next_page();
            break;
        default:
            curr_event_handler = demo_item[temp_index].event_handle_f;
            if (demo_item[temp_index].show_screen_f) {
                demo_item[temp_index].show_screen_f();
            }
            return;
    }
    main_screen_draw();
}

void common_event_handler(message_id_enum event_id, int32_t param1, void* param2)
{
    if (curr_event_handler) {
        curr_event_handler(event_id, param1, param2);
    }
}

uint8_t* convert_string_to_wstring(uint8_t* string)
{
	static uint8_t wstring[50];
	int32_t index = 0;
	if (!string) {
		return NULL;
	}
	while (*string) {
		wstring[index] = *string;
		wstring[index + 1] = 0;
		string++;
		index+=2;
	}
	return wstring;
}
static void main_screen_cntx_init()
{
    static int32_t is_init;
    if (is_init)
        return;
    is_init = 1;
    
    main_screen_cntx.LCD_WIDTH = 240;
    main_screen_cntx.LCD_HEIGHT = 240;
    main_screen_cntx.top_gap = 50;
    main_screen_cntx.left_gap = 40;
    main_screen_cntx.right_gap = 3;
    main_screen_cntx.bottom_gap = 3;
    main_screen_cntx.line_gap = 10;
    main_screen_cntx.total_item_num = sizeof(demo_item)/sizeof(list_item_struct_t);
    main_screen_cntx.start_item = 0;
    main_screen_cntx.color = 0xFFFFFFFF;
    
    font_engine_get_string_width_height(
                                    convert_string_to_wstring((uint8_t*) demo_item[0].name), 
                                    strlen((char*) demo_item[0].name),
                                    &main_screen_cntx.item_width,
                                    &main_screen_cntx.item_height);
    if ((main_screen_cntx.LCD_WIDTH - main_screen_cntx.left_gap - main_screen_cntx.right_gap) < main_screen_cntx.item_width) {
        main_screen_cntx.item_width = main_screen_cntx.LCD_WIDTH - main_screen_cntx.left_gap - main_screen_cntx.right_gap;
    }
    main_screen_cntx.item_height += main_screen_cntx.line_gap;
    main_screen_cntx.one_screen_item_num = (main_screen_cntx.LCD_HEIGHT - main_screen_cntx.top_gap - main_screen_cntx.bottom_gap)/main_screen_cntx.item_height;
    if (main_screen_cntx.one_screen_item_num < main_screen_cntx.total_item_num) {
        main_screen_cntx.curr_item_num = main_screen_cntx.one_screen_item_num - 1;
        main_screen_cntx.has_next_page = 1;
    } else {
        main_screen_cntx.curr_item_num = main_screen_cntx.total_item_num;
        main_screen_cntx.has_next_page = 0;
    }
    main_screen_cntx.has_previous_page = 0;
	gdi_init(frame_buffer, sizeof(frame_buffer));
}

static void main_screen_scroll_to_next_page()
{
    int32_t left_item;
    if (main_screen_cntx.has_next_page) {
        left_item = main_screen_cntx.total_item_num - (main_screen_cntx.start_item + main_screen_cntx.curr_item_num);
        if (left_item > 0) {
            main_screen_cntx.has_previous_page = 1;
            if (left_item <= main_screen_cntx.one_screen_item_num - main_screen_cntx.has_previous_page) {
                main_screen_cntx.has_next_page = 0;
            } else {
                main_screen_cntx.has_next_page = 1;
            }
            main_screen_cntx.start_item += main_screen_cntx.curr_item_num;
            main_screen_cntx.curr_item_num = main_screen_cntx.one_screen_item_num 
                                            - main_screen_cntx.has_previous_page - main_screen_cntx.has_next_page;
        } else {
            //error case, logic wrong, should not be hit
        }
    } else {
        return;
    }
}

static void main_screen_scroll_to_prevoius_page()
{
    if (main_screen_cntx.has_previous_page) {
        if (main_screen_cntx.start_item == main_screen_cntx.one_screen_item_num -1) {
            main_screen_cntx.has_previous_page = 0;
        } else {
            main_screen_cntx.has_previous_page = 1;
        }
        
        main_screen_cntx.has_next_page = 1;
		main_screen_cntx.curr_item_num =  main_screen_cntx.one_screen_item_num 
                                            - main_screen_cntx.has_previous_page - main_screen_cntx.has_next_page;
        main_screen_cntx.start_item -= main_screen_cntx.curr_item_num;
    } else {
    }
}

char* my_itoa(int num,char* str,int radix)
{
	char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	unsigned unum;
	int i=0,j,k;
	char temp;

	if(radix==10 && num<0) {
		unum=(unsigned)-num;
		str[i++]='-';
	} else {
		unum=(unsigned)num;
	}

	do {
		str[i++] = index[unum%(unsigned)radix];
		unum /= radix;
	} while(unum);

	str[i]='\0';
	if(str[0]=='-') {
		k=1;
	} else {
		k=0;
	}

	for(j=k;j<=(i-1)/2;j++)
	{
		temp=str[j];
		str[j]=str[i-1+k-j];
		str[i-1+k-j]=temp;
	}
	return str;
}


static void main_screen_draw()
{
    int32_t index = main_screen_cntx.start_item;
    int32_t num = main_screen_cntx.curr_item_num;
    int32_t x,y;

    x = main_screen_cntx.left_gap;
    y = main_screen_cntx.top_gap;
    gdi_draw_solid_rect(0,0,239,239,0);
    font_engine_show_string(10, 
                            10, 
			    convert_string_to_wstring((uint8_t*) DEMO_TITLE_STRING_NAME), 
                            strlen(DEMO_TITLE_STRING_NAME), 
                            main_screen_cntx.color);


    if (main_screen_cntx.has_previous_page) {
        
        font_engine_show_string(x, 
                                y, 
                                convert_string_to_wstring((uint8_t*) PERVIOUS_PAGE_STRING_NAME), 
                                strlen(PERVIOUS_PAGE_STRING_NAME), 
                                main_screen_cntx.color);
                                
        y += main_screen_cntx.item_height;
    }
    while (num) {
        uint8_t pre_index[10];
		int32_t str_len;
		my_itoa((int) index, (char*) pre_index,10);
		str_len = strlen((char*) pre_index);
		pre_index[str_len] = '.';
		pre_index[str_len + 1] = 0;

        font_engine_show_string(x - 30, 
                                y, 
                                convert_string_to_wstring((uint8_t*) pre_index), 
                                strlen((char*) pre_index), 
                                main_screen_cntx.color);

		font_engine_show_string(x, 
                                y, 
                                convert_string_to_wstring((uint8_t*) demo_item[index].name), 
                                strlen((char*) demo_item[index].name), 
                                main_screen_cntx.color);
                                
        y += main_screen_cntx.item_height;
        index++;
        num--;
    }
    
    if (main_screen_cntx.has_next_page) {
        
        font_engine_show_string(x, 
                                y, 
                                convert_string_to_wstring((uint8_t*) NEXT_PAGE_STRING_NAME), 
                                strlen(NEXT_PAGE_STRING_NAME), 
                                main_screen_cntx.color);
                                
        y += main_screen_cntx.item_height;        
    }
    
    gdi_flush_screen();
}

// -1 means not hit, -2 means prevoius page, -3 means next page
static int32_t main_screen_get_index(int32_t x, int32_t y)
{
    int32_t ui_index = -1;
    if (x > main_screen_cntx.left_gap && x < main_screen_cntx.LCD_WIDTH - main_screen_cntx.right_gap) {
        if (y > main_screen_cntx.top_gap + 1) {
            ui_index = (y - main_screen_cntx.top_gap)/main_screen_cntx.item_height;
        }
    }

    if (ui_index >= main_screen_cntx.curr_item_num + main_screen_cntx.has_previous_page + main_screen_cntx.has_next_page) {
        return -1;
    } 

    if (ui_index == 0 && main_screen_cntx.has_previous_page) {
        return -2;
    }

    if (ui_index == main_screen_cntx.one_screen_item_num - 1 && main_screen_cntx.has_next_page) {
        return -3;
    }

    ui_index -= main_screen_cntx.has_previous_page;

    return main_screen_cntx.start_item + ui_index;
}

void show_main_screen()
{
    curr_event_handler = main_screen_event_handle;
    demo_ui_register_touch_event_callback(main_screen_pen_event_handler, NULL);
    main_screen_cntx_init();
#ifndef _MSC_VER
    GRAPHICLOG("show_main_screen");
#endif
    main_screen_draw();
}

