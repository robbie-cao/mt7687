#include "stdlib.h"
#include "string.h"

#include "g2d_drv_6260_series.h"
#include "g2d_enum.h"
#include "g2d_common_api.h"
#include "g2d_lt_api.h"
#include "g2d_bitblt_api.h"
#include "g2d_font_api.h"
#include "g2d_rectfill_api.h"

#include "g2d_drv_c_model_6260_series.h"
#include "g2d_test.h"

#include "hal_clock.h"	//power on 

//Dream: Larkspur===============
#include "syslog.h"
#define G2D_LOGI(fmt,...) LOG_I(common,  (fmt), ##__VA_ARGS__)
//==============================


//Dream Let's non cached
#define __noncached_rwdata __attribute__ ((__section__(".noncached_rwdata")))	

/// nHD = 360 x 640
//__align(16) unsigned char layer0_image[LAYER0_SIZE]; L
//__align(16) unsigned char layer1_image[LAYER1_SIZE]; L
//__align(16) unsigned char layer2_image[240*320*4];
//__align(16) unsigned char layer3_image[240*320*4];

//unsigned char  dst_sw_image_240X320[DST_SW_BUFFER_SIZE]; Larkspur only support small display
//unsigned char  dst_hw_image_240X320[DST_HW_BUFFER_SIZE]; Larkspur only support small display

unsigned char __noncached_rwdata layer0_image[LAYER0_SIZE]; 
unsigned char __noncached_rwdata layer1_image[LAYER1_SIZE];


unsigned char  dst_sw_image_120X120[120*120*4];
unsigned char __noncached_rwdata dst_hw_image_120X120[120*120*4];


G2D_REG g2d_sw_register;

uint32_t drv_get_current_time(void)
{
    return (*(volatile uint32_t *)(0xA0180104));
}

uint32_t drv_get_current_time_dream(void)//program by Dream 2015.12.27
{
    return xTaskGetTickCountFromISR();
}


uint32_t drv_get_duration_tick(uint32_t previous_time, uint32_t current_time)
{
    uint32_t result=0;

    if (previous_time > current_time)
    {
        result = 0xFFFFFFFF - previous_time + current_time;
    }
    else
    {
        result = current_time - previous_time;
    }

    return result;
}


void ASSERT(int const expr)
{
    if (0 == (expr))
    {
        G2D_LOGI("ASSERT\n\r");
        while (1)
        {
        }
    }
}


void g2d_compare_sw_hw_result(void)
{
    int i;

    for(i = 0; i < (120*120*4); i++ )
    {
        if(dst_sw_image_120X120[i] != dst_hw_image_120X120[i])
        {
            G2D_LOGI("Error in index = %d   sw=%x != hw=%x\n\r",i, dst_sw_image_120X120[i], dst_hw_image_120X120[i]);
            while(1);
            return;
        }
    }
	G2D_LOGI("g2d_compare_sw_hw_result check ok\n");//Dream add
}



void g2d_set_registers_to_c_model(G2D_REG* reg, unsigned char *dstBuf)
{
    reg->G2D_START = REG_G2D_START;
    reg->G2D_MODE_CON = REG_G2D_MODE_CTRL;
    reg->G2D_IRQ = REG_G2D_IRQ;
    reg->G2D_SLOW_DOWN = REG_G2D_SLOW_DOWN;

    reg->G2D_ROI_CON = REG_G2D_ROI_CON;
    reg->G2D_W2M_ADDR = (uint32_t) dstBuf;;
    reg->G2D_W2M_PITCH = REG_G2D_W2M_PITCH;
    reg->G2D_ROI_OFS = REG_G2D_ROI_OFFSET;
    reg->G2D_ROI_SIZE = REG_G2D_ROI_SIZE;
    reg->G2D_ROI_BGCLR = REG_G2D_ROI_BGCLR;

    reg->G2D_CLP_MIN = REG_G2D_CLP_MIN;
    reg->G2D_CLP_MAX = REG_G2D_CLP_MAX;
    reg->G2D_AVO_CLR = REG_G2D_AVOIDANCE_COLOR;
    reg->G2D_REP_CLR = REG_G2D_REPLACEMENT_COLOR;
    reg->G2D_ROI_MOFS = REG_G2D_W2M_OFFSET;


    reg->G2D_MW_INIT = REG_G2D_MW_INIT;
    reg->G2D_MZ_INIT = REG_G2D_MZ_INIT;
    reg->G2D_DI_CON = REG_G2D_DI_CON;

    reg->G2D_L0_CON = REG_G2D_L0_CON;
    reg->G2D_L0_CLRKEY = REG_G2D_L0_SRC_KEY;
    reg->G2D_L0_ADDR = REG_G2D_L0_ADDR;
    reg->G2D_L0_PITCH = REG_G2D_L0_PITCH;
    reg->G2D_L0_SIZE = REG_G2D_L0_SIZE;
    reg->G2D_L0_OFS = REG_G2D_L0_OFFSET;

    reg->G2D_L1_CON = REG_G2D_L1_CON;
    reg->G2D_L1_CLRKEY = REG_G2D_L1_SRC_KEY;
    reg->G2D_L1_ADDR = REG_G2D_L1_ADDR;
    reg->G2D_L1_PITCH = REG_G2D_L1_PITCH;
    reg->G2D_L1_SIZE = REG_G2D_L1_SIZE;
    reg->G2D_L1_OFS = REG_G2D_L1_OFFSET;

    reg->G2D_L2_CON = REG_G2D_L2_CON;
    reg->G2D_L2_CLRKEY = REG_G2D_L2_SRC_KEY;
    reg->G2D_L2_ADDR = REG_G2D_L2_ADDR;
    reg->G2D_L2_PITCH = REG_G2D_L2_PITCH;
    reg->G2D_L2_SIZE = REG_G2D_L2_SIZE;
    reg->G2D_L2_OFS = REG_G2D_L2_OFFSET;

    reg->G2D_L3_CON = REG_G2D_L3_CON;
    reg->G2D_L3_CLRKEY = REG_G2D_L3_SRC_KEY;
    reg->G2D_L3_ADDR = REG_G2D_L3_ADDR;
    reg->G2D_L3_PITCH = REG_G2D_L3_PITCH;
    reg->G2D_L3_SIZE = REG_G2D_L3_SIZE;
    reg->G2D_L3_OFS = REG_G2D_L3_OFFSET;
}



void g2d_basic_register_read_write_test(void)
{
    uint32_t reg32;

    G2D_LOGI("G2D register read/write test\n\r");

    //0x004
    reg32 = REG_G2D_MODE_CTRL;
    REG_G2D_MODE_CTRL = 1; //0x7F0007; Dream:larkspur only support 1
    if (1 != REG_G2D_MODE_CTRL)
    {
        G2D_LOGI("REG_G2D_MODE_CTRL write error %x\n\r", REG_G2D_MODE_CTRL);
    }
    REG_G2D_MODE_CTRL = 0x0;//it's not support for larkspur
    G2D_LOGI("REG_G2D_MODE_CTRL = %x when you set error value\n\r", REG_G2D_MODE_CTRL);
	
    REG_G2D_MODE_CTRL = reg32;

    //0x008
    reg32 = REG_G2D_RESET;
    REG_G2D_RESET = 0x3;
    if (0x3 != REG_G2D_RESET)
    {
        G2D_LOGI("REG_G2D_RESET write error\n\r");
    }
    REG_G2D_RESET = 0x0;
    if (REG_G2D_RESET != 0x0)
    {
        G2D_LOGI("REG_G2D_RESET write error\n\r");
    }
    REG_G2D_RESET = reg32;

    //0x010
    reg32 = REG_G2D_IRQ;
    REG_G2D_IRQ = 0x01;
    if (0x01 != REG_G2D_IRQ)
    {
        G2D_LOGI("REG_G2D_IRQ write error\n\r");
    }
    REG_G2D_IRQ = 0x0;
    if (REG_G2D_IRQ != 0x0)
    {
        G2D_LOGI("REG_G2D_IRQ write error\n\r");
    }
    REG_G2D_IRQ = reg32;

    //0x014
    reg32 = REG_G2D_SLOW_DOWN;
    REG_G2D_SLOW_DOWN = 0x877000FF;
    if (0x877000FF != REG_G2D_SLOW_DOWN)
    {
        G2D_LOGI("REG_G2D_SLOW_DOWN write error\n\r");
    }
    REG_G2D_SLOW_DOWN = 0x0;
    if (REG_G2D_SLOW_DOWN != 0x0)
    {
        G2D_LOGI("REG_G2D_SLOW_DOWN write error\n\r");
    }
    REG_G2D_SLOW_DOWN = reg32;


    //0x040
    reg32 = REG_G2D_ROI_CON;
    REG_G2D_ROI_CON = 0xF02FFF9F;
    if (0xF02FFF9F != REG_G2D_ROI_CON)
    {
        G2D_LOGI("Reg REG_G2D_ROI_CON write error\n\r");
    }
    REG_G2D_ROI_CON = 0x0;
    if (REG_G2D_ROI_CON != 0x0)
    {
        G2D_LOGI("REG_G2D_ROI_CON write error\n\r");
    }
    REG_G2D_ROI_CON = reg32;

    //0x044
    reg32 = REG_G2D_W2M_ADDR;
    REG_G2D_W2M_ADDR = 0xFFFFFFFF;
    if (0xFFFFFFFF != REG_G2D_W2M_ADDR)
    {
        G2D_LOGI("REG_G2D_W2M_ADDR write error\n\r");
    }
    REG_G2D_W2M_ADDR = 0x0;
    if (REG_G2D_W2M_ADDR != 0x0)
    {
        G2D_LOGI("REG_G2D_W2M_ADDR write error\n\r");
    }
    REG_G2D_W2M_ADDR = reg32;

    //0x048
    reg32 = REG_G2D_W2M_PITCH;
    REG_G2D_W2M_PITCH = 0x03FF;
    if (0x03FF != REG_G2D_W2M_PITCH)
    {
        G2D_LOGI("REG_G2D_W2M_PITCH write error\n\r");
    }
    REG_G2D_W2M_PITCH = 0x0;
    if (REG_G2D_W2M_PITCH != 0x0)
    {
        G2D_LOGI("REG_G2D_W2M_PITCH write error\n\r");
    }
    REG_G2D_W2M_PITCH = reg32;

    //0x04C
    reg32 = REG_G2D_ROI_OFFSET;
    REG_G2D_ROI_OFFSET = 0x0FFF0FFF;
    if (0x0FFF0FFF != REG_G2D_ROI_OFFSET)
    {
        G2D_LOGI("REG_G2D_ROI_OFFSET write error\n\r");
    }
    REG_G2D_ROI_OFFSET = 0x0;
    if (REG_G2D_ROI_OFFSET != 0x0)
    {
        G2D_LOGI("REG_G2D_ROI_OFFSET write error\n\r");
    }
    REG_G2D_ROI_OFFSET = reg32;

    //0x050
    reg32 = REG_G2D_ROI_SIZE;
    REG_G2D_ROI_SIZE = 0x0FFF0FFF;
    if (0x0FFF0FFF != REG_G2D_ROI_SIZE)
    {
        G2D_LOGI("REG_G2D_ROI_SIZE write error\n\r");
    }
    REG_G2D_ROI_SIZE = 0x0;
    if (REG_G2D_ROI_SIZE != 0x0)
    {
        G2D_LOGI("REG_G2D_ROI_SIZE write error\n\r");
    }
    REG_G2D_ROI_SIZE = reg32;

    //0x054
    reg32 = REG_G2D_ROI_BGCLR ;
    REG_G2D_ROI_BGCLR  = 0xFFFFFFFF;
    if (0xFFFFFFFF != REG_G2D_ROI_BGCLR )
    {
        G2D_LOGI("REG_G2D_ROI_BGCLR  write error\n\r");
    }
    REG_G2D_ROI_BGCLR  = 0x0;
    if (REG_G2D_ROI_BGCLR  != 0x0)
    {
        G2D_LOGI("REG_G2D_ROI_BGCLR  write error\n\r");
    }
    REG_G2D_ROI_BGCLR =reg32;

    //0x058
    reg32 = REG_G2D_CLP_MIN;
    REG_G2D_CLP_MIN = 0x0FFF0FFF;
    if (0x0FFF0FFF != REG_G2D_CLP_MIN)
    {
        G2D_LOGI("REG_G2D_CLP_MIN write error\n\r");
    }
    REG_G2D_CLP_MIN = 0x0;
    if (REG_G2D_CLP_MIN != 0x0)
    {
        G2D_LOGI("REG_G2D_CLP_MIN write error\n\r");
    }
    REG_G2D_CLP_MIN=reg32;

    //0x05C
    reg32 = REG_G2D_CLP_MAX;
    REG_G2D_CLP_MAX = 0x0FFF0FFF;
    if (0x0FFF0FFF != REG_G2D_CLP_MAX)
    {
        G2D_LOGI("REG_G2D_CLP_MAX write error\n\r");
    }
    REG_G2D_CLP_MAX = 0x0;
    if (REG_G2D_CLP_MAX != 0x0)
    {
        G2D_LOGI("REG_G2D_CLP_MAX write error\n\r");
    }
    REG_G2D_CLP_MAX = reg32;

    //0x060
    reg32 = REG_G2D_AVOIDANCE_COLOR;
    REG_G2D_AVOIDANCE_COLOR = 0xFFFFFFFF;
    if (0xFFFFFFFF != REG_G2D_AVOIDANCE_COLOR)
    {
        G2D_LOGI("REG_G2D_AVOIDANCE_COLOR write error\n\r");
    }
    REG_G2D_AVOIDANCE_COLOR = 0x0;
    if (REG_G2D_AVOIDANCE_COLOR != 0x0)
    {
        G2D_LOGI("REG_G2D_AVOIDANCE_COLOR write error\n\r");
    }
    REG_G2D_AVOIDANCE_COLOR = reg32;

    //0x064
    reg32 = REG_G2D_REPLACEMENT_COLOR;
    REG_G2D_REPLACEMENT_COLOR = 0xFFFFFFFF;
    if (0xFFFFFFFF != REG_G2D_REPLACEMENT_COLOR)
    {
        G2D_LOGI("REG_G2D_REPLACEMENT_COLOR write error\n\r");
    }
    REG_G2D_REPLACEMENT_COLOR = 0x0;
    if (REG_G2D_REPLACEMENT_COLOR != 0x0)
    {
        G2D_LOGI("REG_G2D_REPLACEMENT_COLOR write error\n\r");
    }
    REG_G2D_REPLACEMENT_COLOR = reg32;

    //0x068
    reg32 = REG_G2D_W2M_OFFSET;
    REG_G2D_W2M_OFFSET = 0x0FFF0FFF;
    if (0x0FFF0FFF != REG_G2D_W2M_OFFSET)
    {
        G2D_LOGI("REG_G2D_W2M_OFFSET write error\n\r");
    }
    REG_G2D_W2M_OFFSET = 0x0;
    if (REG_G2D_W2M_OFFSET != 0x0)
    {
        G2D_LOGI("REG_G2D_W2M_OFFSET write error\n\r");
    }
    REG_G2D_W2M_OFFSET = reg32;

    //0x070
    reg32 = REG_G2D_MW_INIT;
    REG_G2D_MW_INIT = 0xFFFFFFFF;
    if (0xFFFFFFFF != REG_G2D_MW_INIT)
    {
        G2D_LOGI("REG_G2D_MW_INIT write error\n\r");
    }
    REG_G2D_MW_INIT = 0x0;
    if (REG_G2D_MW_INIT != 0x0)
    {
        G2D_LOGI("REG_G2D_MW_INIT write error\n\r");
    }
    REG_G2D_MW_INIT = reg32;

    //0x074
    reg32 = REG_G2D_MZ_INIT;
    REG_G2D_MZ_INIT = 0xFFFFFFFF;
    if (0xFFFFFFFF != REG_G2D_MZ_INIT)
    {
        G2D_LOGI("REG_G2D_MZ_INIT write error\n\r");
    }
    REG_G2D_MZ_INIT = 0x0;
    if (REG_G2D_MZ_INIT != 0x0)
    {
        G2D_LOGI("REG_G2D_MZ_INIT write error\n\r");
    }
    REG_G2D_MZ_INIT = reg32;

    //0x078
    reg32 = REG_G2D_DI_CON;
    REG_G2D_DI_CON = 0x3333;
    if (0x3333 != REG_G2D_DI_CON)
    {
        G2D_LOGI("REG_G2D_DI_CON write error\n\r");
    }
    REG_G2D_DI_CON = 0x0;
    if (REG_G2D_DI_CON != 0x0)
    {
        G2D_LOGI("REG_G2D_DI_CON write error\n\r");
    }
    REG_G2D_DI_CON = reg32;

    //0x080
    reg32 = REG_G2D_L0_CON;
    REG_G2D_L0_CON = 0x70C7FF9F;
    if (0x70C7FF9F != REG_G2D_L0_CON)
    {
        G2D_LOGI("REG_G2D_L0_CON write error\n\r");
    }
    REG_G2D_L0_CON = 0x0;
    if (REG_G2D_L0_CON != 0x0)
    {
        G2D_LOGI("REG_G2D_L0_CON write error\n\r");
    }
    REG_G2D_L0_CON=reg32;

    //0x84
    reg32 = REG_G2D_L0_ADDR;
    REG_G2D_L0_ADDR = 0xFFFFFFFF;
    if (0xFFFFFFFF != REG_G2D_L0_ADDR)
    {
        G2D_LOGI("REG_G2D_L0_ADDR write error\n\r");
    }
    REG_G2D_L0_ADDR = 0x0;
    if (REG_G2D_L0_ADDR != 0x0)
    {
        G2D_LOGI("REG_G2D_L0_ADDR write error\n\r");
    }
    REG_G2D_L0_ADDR=reg32;

    //0x88
    reg32 = REG_G2D_L0_PITCH;
    REG_G2D_L0_PITCH = 0x3FFF;
    if (0x3FFF != REG_G2D_L0_PITCH)
    {
        G2D_LOGI("REG_G2D_L0_PITCH write error\n\r");
    }
    REG_G2D_L0_PITCH = 0x0;
    if (REG_G2D_L0_PITCH != 0x0)
    {
        G2D_LOGI("REG_G2D_L0_PITCH write error\n\r");
    }
    REG_G2D_L0_PITCH=reg32;

    //0x8C
    reg32 = REG_G2D_L0_OFFSET;
    REG_G2D_L0_OFFSET = 0x0FFF0FFF;
    if (0x0FFF0FFF != REG_G2D_L0_OFFSET)
    {
        G2D_LOGI("REG_G2D_L0_OFFSET write error\n\r");
    }
    REG_G2D_L0_OFFSET = 0x0;
    if (REG_G2D_L0_OFFSET != 0x0)
    {
        G2D_LOGI("REG_G2D_L0_OFFSET write error\n\r");
    }
    REG_G2D_L0_OFFSET=reg32;

    //0x090
    reg32 = REG_G2D_L0_SIZE;
    REG_G2D_L0_SIZE = 0x0FFF0FFF;
    if (0x0FFF0FFF != REG_G2D_L0_SIZE)
    {
        G2D_LOGI("REG_G2D_L0_SIZE write error\n\r");
    }
    REG_G2D_L0_SIZE = 0x0;
    if (REG_G2D_L0_SIZE != 0x0)
    {
        G2D_LOGI("REG_G2D_L0_SIZE write error\n\r");
    }
    REG_G2D_L0_SIZE=reg32;

    //0x094
    reg32 = REG_G2D_L0_SRC_KEY;
    REG_G2D_L0_SRC_KEY = 0xFFFFFFFF;
    if (0xFFFFFFFF != REG_G2D_L0_SRC_KEY)
    {
        G2D_LOGI("REG_G2D_L0_SRC_KEY write error\n\r");
    }
    REG_G2D_L0_SRC_KEY = 0x0;
    if (REG_G2D_L0_SRC_KEY != 0x0)
    {
        G2D_LOGI("REG_G2D_L0_SRC_KEY write error\n\r");
    }
    REG_G2D_L0_SRC_KEY = reg32;

    //0x080
    reg32 = REG_G2D_L1_CON;
    REG_G2D_L1_CON = 0x70C7FF9F;
    if (0x70C7FF9F != REG_G2D_L1_CON)
    {
        G2D_LOGI("REG_G2D_L1_CON write error\n\r");
    }
    REG_G2D_L1_CON = 0x0;
    if (REG_G2D_L1_CON != 0x0)
    {
        G2D_LOGI("REG_G2D_L1_CON write error\n\r");
    }
    REG_G2D_L1_CON=reg32;

    //0x84
    reg32 = REG_G2D_L1_ADDR;
    REG_G2D_L1_ADDR = 0xFFFFFFFF;
    if (0xFFFFFFFF != REG_G2D_L1_ADDR)
    {
        G2D_LOGI("REG_G2D_L1_ADDR write error\n\r");
    }
    REG_G2D_L1_ADDR = 0x0;
    if (REG_G2D_L1_ADDR != 0x0)
    {
        G2D_LOGI("REG_G2D_L1_ADDR write error\n\r");
    }
    REG_G2D_L1_ADDR=reg32;

    //0x88
    reg32 = REG_G2D_L1_PITCH;
    REG_G2D_L1_PITCH = 0x3FFF;
    if (0x3FFF != REG_G2D_L1_PITCH)
    {
        G2D_LOGI("REG_G2D_L1_PITCH write error\n\r");
    }
    REG_G2D_L1_PITCH = 0x0;
    if (REG_G2D_L1_PITCH != 0x0)
    {
        G2D_LOGI("REG_G2D_L1_PITCH write error\n\r");
    }
    REG_G2D_L1_PITCH=reg32;

    //0x8C
    reg32 = REG_G2D_L1_OFFSET;
    REG_G2D_L1_OFFSET = 0x0FFF0FFF;
    if (0x0FFF0FFF != REG_G2D_L1_OFFSET)
    {
        G2D_LOGI("REG_G2D_L1_OFFSET write error\n\r");
    }
    REG_G2D_L1_OFFSET = 0x0;
    if (REG_G2D_L1_OFFSET != 0x0)
    {
        G2D_LOGI("REG_G2D_L1_OFFSET write error\n\r");
    }
    REG_G2D_L1_OFFSET=reg32;

    //0x090
    reg32 = REG_G2D_L1_SIZE;
    REG_G2D_L1_SIZE = 0x0FFF0FFF;
    if (0x0FFF0FFF != REG_G2D_L1_SIZE)
    {
        G2D_LOGI("REG_G2D_L1_SIZE write error\n\r");
    }
    REG_G2D_L1_SIZE = 0x0;
    if (REG_G2D_L1_SIZE != 0x0)
    {
        G2D_LOGI("REG_G2D_L1_SIZE write error\n\r");
    }
    REG_G2D_L1_SIZE=reg32;

    //0x094
    reg32 = REG_G2D_L1_SRC_KEY;
    REG_G2D_L1_SRC_KEY = 0xFFFFFFFF;
    if (0xFFFFFFFF != REG_G2D_L1_SRC_KEY)
    {
        G2D_LOGI("REG_G2D_L1_SRC_KEY write error\n\r");
    }
    REG_G2D_L1_SRC_KEY = 0x0;
    if (REG_G2D_L1_SRC_KEY != 0x0)
    {
        G2D_LOGI("REG_G2D_L1_SRC_KEY write error\n\r");
    }
    REG_G2D_L1_SRC_KEY = reg32;

    //0x080
    reg32 = REG_G2D_L2_CON;
    REG_G2D_L2_CON = 0x70C7FF9F;
    if (0x70C7FF9F != REG_G2D_L2_CON)
    {
        G2D_LOGI("REG_G2D_L2_CON write error\n\r");
    }
    REG_G2D_L2_CON = 0x0;
    if (REG_G2D_L2_CON != 0x0)
    {
        G2D_LOGI("REG_G2D_L2_CON write error\n\r");
    }
    REG_G2D_L2_CON=reg32;

    //0x84
    reg32 = REG_G2D_L2_ADDR;
    REG_G2D_L2_ADDR = 0xFFFFFFFF;
    if (0xFFFFFFFF != REG_G2D_L2_ADDR)
    {
        G2D_LOGI("REG_G2D_L2_ADDR write error\n\r");
    }
    REG_G2D_L2_ADDR = 0x0;
    if (REG_G2D_L2_ADDR != 0x0)
    {
        G2D_LOGI("REG_G2D_L2_ADDR write error\n\r");
    }
    REG_G2D_L2_ADDR=reg32;

    //0x88
    reg32 = REG_G2D_L2_PITCH;
    REG_G2D_L2_PITCH = 0x3FFF;
    if (0x3FFF != REG_G2D_L2_PITCH)
    {
        G2D_LOGI("REG_G2D_L2_PITCH write error\n\r");
    }
    REG_G2D_L2_PITCH = 0x0;
    if (REG_G2D_L2_PITCH != 0x0)
    {
        G2D_LOGI("REG_G2D_L2_PITCH write error\n\r");
    }
    REG_G2D_L2_PITCH=reg32;

    //0x8C
    reg32 = REG_G2D_L2_OFFSET;
    REG_G2D_L2_OFFSET = 0x0FFF0FFF;
    if (0x0FFF0FFF != REG_G2D_L2_OFFSET)
    {
        G2D_LOGI("REG_G2D_L2_OFFSET write error\n\r");
    }
    REG_G2D_L2_OFFSET = 0x0;
    if (REG_G2D_L2_OFFSET != 0x0)
    {
        G2D_LOGI("REG_G2D_L2_OFFSET write error\n\r");
    }
    REG_G2D_L2_OFFSET=reg32;

    //0x090
    reg32 = REG_G2D_L2_SIZE;
    REG_G2D_L2_SIZE = 0x0FFF0FFF;
    if (0x0FFF0FFF != REG_G2D_L2_SIZE)
    {
        G2D_LOGI("REG_G2D_L2_SIZE write error\n\r");
    }
    REG_G2D_L2_SIZE = 0x0;
    if (REG_G2D_L2_SIZE != 0x0)
    {
        G2D_LOGI("REG_G2D_L2_SIZE write error\n\r");
    }
    REG_G2D_L2_SIZE=reg32;

    //0x094
    reg32 = REG_G2D_L2_SRC_KEY;
    REG_G2D_L2_SRC_KEY = 0xFFFFFFFF;
    if (0xFFFFFFFF != REG_G2D_L2_SRC_KEY)
    {
        G2D_LOGI("REG_G2D_L2_SRC_KEY write error\n\r");
    }
    REG_G2D_L2_SRC_KEY = 0x0;
    if (REG_G2D_L2_SRC_KEY != 0x0)
    {
        G2D_LOGI("REG_G2D_L2_SRC_KEY write error\n\r");
    }
    REG_G2D_L2_SRC_KEY = reg32;

    //0x080
    reg32 = REG_G2D_L3_CON;
    REG_G2D_L3_CON = 0x70C7FF9F;
    if (0x70C7FF9F != REG_G2D_L3_CON)
    {
        G2D_LOGI("REG_G2D_L3_CON write error\n\r");
    }
    REG_G2D_L3_CON = 0x0;
    if (REG_G2D_L3_CON != 0x0)
    {
        G2D_LOGI("REG_G2D_L3_CON write error\n\r");
    }
    REG_G2D_L3_CON=reg32;

    //0x84
    reg32 = REG_G2D_L3_ADDR;
    REG_G2D_L3_ADDR = 0xFFFFFFFF;
    if (0xFFFFFFFF != REG_G2D_L3_ADDR)
    {
        G2D_LOGI("REG_G2D_L3_ADDR write error\n\r");
    }
    REG_G2D_L3_ADDR = 0x0;
    if (REG_G2D_L3_ADDR != 0x0)
    {
        G2D_LOGI("REG_G2D_L3_ADDR write error\n\r");
    }
    REG_G2D_L3_ADDR=reg32;

    //0x88
    reg32 = REG_G2D_L3_PITCH;
    REG_G2D_L3_PITCH = 0x3FFF;
    if (0x3FFF != REG_G2D_L3_PITCH)
    {
        G2D_LOGI("REG_G2D_L3_PITCH write error\n\r");
    }
    REG_G2D_L3_PITCH = 0x0;
    if (REG_G2D_L3_PITCH != 0x0)
    {
        G2D_LOGI("REG_G2D_L3_PITCH write error\n\r");
    }
    REG_G2D_L3_PITCH=reg32;

    //0x8C
    reg32 = REG_G2D_L3_OFFSET;
    REG_G2D_L3_OFFSET = 0x0FFF0FFF;
    if (0x0FFF0FFF != REG_G2D_L3_OFFSET)
    {
        G2D_LOGI("REG_G2D_L3_OFFSET write error\n\r");
    }
    REG_G2D_L3_OFFSET = 0x0;
    if (REG_G2D_L3_OFFSET != 0x0)
    {
        G2D_LOGI("REG_G2D_L3_OFFSET write error\n\r");
    }
    REG_G2D_L3_OFFSET=reg32;

    //0x090
    reg32 = REG_G2D_L3_SIZE;
    REG_G2D_L3_SIZE = 0x0FFF0FFF;
    if (0x0FFF0FFF != REG_G2D_L3_SIZE)
    {
        G2D_LOGI("REG_G2D_L3_SIZE write error\n\r");
    }
    REG_G2D_L3_SIZE = 0x0;
    if (REG_G2D_L3_SIZE != 0x0)
    {
        G2D_LOGI("REG_G2D_L3_SIZE write error\n\r");
    }
    REG_G2D_L3_SIZE=reg32;

    //0x094
    reg32 = REG_G2D_L3_SRC_KEY;
    REG_G2D_L3_SRC_KEY = 0xFFFFFFFF;
    if (0xFFFFFFFF != REG_G2D_L3_SRC_KEY)
    {
        G2D_LOGI("REG_G2D_L3_SRC_KEY write error\n\r");
    }
    REG_G2D_L3_SRC_KEY = 0x0;
    if (REG_G2D_L3_SRC_KEY != 0x0)
    {
        G2D_LOGI("REG_G2D_L3_SRC_KEY write error\n\r");
    }
    REG_G2D_L3_SRC_KEY = reg32;

    G2D_LOGI("G2D register read/write test ok!!!\n\r");


}



void g2d_basic_reset_test(void)
{
    g2d_handle_t *g2dHandle;
    uint32_t *dstBuf;
    g2d_color_format_t dstColorFormat, srcColorFormat;
    uint32_t dstRectW, dstRectH, srcRecW, srcRectH;
	int index;
    G2D_LOGI("g2d_basic_reset_test()\n\r");

    {
        G2D_LOGI("g2d warm reset\n\r");
        /// HW
        memset(dst_hw_image_120X120, 168, 120*120*4);	//168 is seldom number

        dstBuf = (uint32_t *)&dst_hw_image_120X120[0];
        srcColorFormat = G2D_COLOR_FORMAT_ARGB8888;
        dstColorFormat = G2D_COLOR_FORMAT_ARGB8888;
        dstRectW = 100;
        dstRectH = 100;

        /// G2D_STATUS_BUSY means someone is using G2D
        if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
        {
        	G2D_LOGI("g2d_basic_reset_test busy\n\r");
            return;
        }
        hal_g2d_set_callback_function(g2dHandle, NULL);
        hal_set_dst_rgb_buffer_info(g2dHandle, (uint8_t *)dstBuf, 120*120*4, dstRectW, dstRectH, dstColorFormat);
        hal_g2d_set_color_replacement(g2dHandle, false, 0, 255, 0, 0, 0, 0, 0, 255);
        hal_g2d_set_destination_clip_window(g2dHandle, false, 0, 0, dstRectW, dstRectH);
        hal_g2d_set_source_key(g2dHandle, false, 0, 0, 0, 0);
        g2dRectFillSetDstWindow(g2dHandle, 0, 0, dstRectW, dstRectH);
        g2dRectFillSetColor(g2dHandle, 0x12345678);
        g2dRectFillStart(g2dHandle);

        hal_g2d_release_handle(g2dHandle);


		//=====================
        WARM_RESET_G2D_ENGINE;

        /// G2D_STATUS_BUSY means someone is using G2D
        if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
        {
	        G2D_LOGI("g2d_basic_reset_test busy2\n\r");
            return;
        }
        hal_g2d_set_callback_function(g2dHandle,NULL);
        hal_set_dst_rgb_buffer_info(g2dHandle, (uint8_t *)dstBuf, 120*120*4, dstRectW, dstRectH, dstColorFormat);
        hal_g2d_set_color_replacement(g2dHandle, false, 0, 255, 0, 0, 0, 0, 0, 255);
        hal_g2d_set_destination_clip_window(g2dHandle, false, 0, 0, dstRectW, dstRectH);
        hal_g2d_set_source_key(g2dHandle, false, 0, 0, 0, 0);
        g2dRectFillSetDstWindow(g2dHandle, 0, 0, dstRectW, dstRectH);
        g2dRectFillSetColor(g2dHandle, 0x12345678);
        g2dRectFillStart(g2dHandle);
        while(hal_g2d_get_status(g2dHandle)) {};
        hal_g2d_release_handle(g2dHandle);
        {
            /// C-model
            memset(dst_sw_image_120X120, 168, 120*120*4);
			
            dstBuf = (uint32_t *)&dst_sw_image_120X120[0];

            g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)dstBuf);

            g2d_c_model(&g2d_sw_register);
        }

        {						
            g2d_compare_sw_hw_result();
        }

    }

    {
        G2D_LOGI("g2d hard reset\n\r");
        /// HW
        memset(dst_hw_image_120X120, 0xCD, 120*120*4);

        dstBuf = (uint32_t *)&dst_hw_image_120X120[0];
        srcColorFormat = G2D_COLOR_FORMAT_ARGB8888;
        dstColorFormat = G2D_COLOR_FORMAT_ARGB8888;
        dstRectW = 100;
        dstRectH = 100;

        /// G2D_STATUS_BUSY means someone is using G2D
        if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
            return;

        hal_g2d_set_callback_function(g2dHandle,NULL);
        hal_set_dst_rgb_buffer_info(g2dHandle, (uint8_t *)dstBuf, 120*120*4, dstRectW, dstRectH, dstColorFormat);
        hal_g2d_set_color_replacement(g2dHandle, false, 0, 255, 0, 0, 0, 0, 0, 255);
        hal_g2d_set_destination_clip_window(g2dHandle, false, 0, 0, dstRectW, dstRectH);
        hal_g2d_set_source_key(g2dHandle, false, 0, 0, 0, 0);
        g2dRectFillSetDstWindow(g2dHandle, 0, 0, dstRectW, dstRectH);
        g2dRectFillSetColor(g2dHandle, 0x12345678);
        g2dRectFillStart(g2dHandle);
        hal_g2d_release_handle(g2dHandle);

        HARD_RESET_G2D_ENGINE;

        /// G2D_STATUS_BUSY means someone is using G2D
        if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
            return;

        hal_g2d_set_callback_function(g2dHandle,NULL);
        hal_set_dst_rgb_buffer_info(g2dHandle, (uint8_t *)dstBuf, 120*120*4, dstRectW, dstRectH, dstColorFormat);
        hal_g2d_set_color_replacement(g2dHandle, false, 0, 255, 0, 0, 0, 0, 0, 255);
        hal_g2d_set_destination_clip_window(g2dHandle, false, 0, 0, dstRectW, dstRectH);
        hal_g2d_set_source_key(g2dHandle, false, 0, 0, 0, 0);
        g2dRectFillSetDstWindow(g2dHandle, 0, 0, dstRectW, dstRectH);
        g2dRectFillSetColor(g2dHandle, 0x12345678);
        g2dRectFillStart(g2dHandle);
        while(hal_g2d_get_status(g2dHandle)) {};
        hal_g2d_release_handle(g2dHandle);

        {
            /// C-model
            memset(dst_sw_image_120X120, 0xCD, 120*120*4);
            dstBuf = (uint32_t *)&dst_sw_image_120X120[0];

            g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)dstBuf);

            g2d_c_model(&g2d_sw_register);
        }

        {
            g2d_compare_sw_hw_result();
        }

    }
}



void g2d_basic_slow_down_test(void)
{
    g2d_handle_t *g2dHandle;
    uint32_t *dstBuf;
    g2d_color_format_t dstColorFormat, srcColorFormat;
    uint32_t dstRectW, dstRectH, srcRectW, srcRectH;
    uint32_t readType, writeType, slowCount;

    G2D_LOGI("g2d_basic_slow_down_test()\n\r");

    for(readType = 0; readType < 3; readType++)
    {
        if(2 == readType)
        {
            continue;
        }
        for(writeType = 0; writeType < 5; writeType++)
        {
            if(1 == writeType)
            {
                continue;
            }
            for(slowCount = 0; slowCount < 16; slowCount++)
            {
                G2D_LOGI("readType = %d, writeType= %d, enable slow down, slowCount = %d\n\r",readType, writeType, slowCount);
                /// HW
                memset(dst_hw_image_120X120, 0xCD, 120*120*4);

                dstBuf = (uint32_t *)&dst_hw_image_120X120[0];
                srcColorFormat = G2D_COLOR_FORMAT_ARGB8888;
                dstColorFormat = G2D_COLOR_FORMAT_ARGB8888;
                dstRectW = 100;
                dstRectH = 100;

                /// G2D_STATUS_BUSY means someone is using G2D
                if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
                    return;

                //slow down
                g2dSetReadBurstType(g2dHandle, readType);
                g2dSetWriteBurstType(g2dHandle, writeType);
                g2dSetSlowDownCount(g2dHandle, true, slowCount);

                hal_g2d_set_callback_function(g2dHandle, NULL);
                hal_set_dst_rgb_buffer_info(g2dHandle, (uint8_t *)dstBuf, 120*120*4, dstRectW, dstRectH, dstColorFormat);
                hal_g2d_set_color_replacement(g2dHandle, false, 0, 255, 0, 0, 0, 0, 0, 255);
                hal_g2d_set_destination_clip_window(g2dHandle, false, 0, 0, dstRectW, dstRectH);
                hal_g2d_set_source_key(g2dHandle, false, 0, 0, 0, 0);
                g2dRectFillSetDstWindow(g2dHandle, 0, 0, dstRectW, dstRectH);
                g2dRectFillSetColor(g2dHandle, 0x12345678);
                g2dRectFillStart(g2dHandle);
                while(hal_g2d_get_status(g2dHandle)) {};
                hal_g2d_release_handle(g2dHandle);

                {
                    /// C-model
                    memset(dst_sw_image_120X120, 0xCD, 120*120*4);
                    dstBuf = (uint32_t *)&dst_sw_image_120X120[0];

                    g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)dstBuf);

                    g2d_c_model(&g2d_sw_register);
                }

                {
                    g2d_compare_sw_hw_result();
                }
            }
        }
    }

    {
        {
            {
                readType = 0;
                writeType = 0;
                slowCount = 0xFF;
                G2D_LOGI("readType = %d, writeType= %d, enable slow down, slowCount = %d\n\r",readType, writeType, slowCount);
                /// HW
                memset(dst_hw_image_120X120, 0xCD, 120*120*4);

                dstBuf = (uint32_t *)&dst_hw_image_120X120[0];
                srcColorFormat = G2D_COLOR_FORMAT_ARGB8888;
                dstColorFormat = G2D_COLOR_FORMAT_ARGB8888;
                dstRectW = 10;
                dstRectH = 10;

                /// G2D_STATUS_BUSY means someone is using G2D
                if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
                    return;

                //slow down
                g2dSetReadBurstType(g2dHandle, readType);
                g2dSetWriteBurstType(g2dHandle, writeType);
                g2dSetSlowDownCount(g2dHandle, true, slowCount);

                hal_g2d_set_callback_function(g2dHandle, NULL);
                hal_set_dst_rgb_buffer_info(g2dHandle, (uint8_t *)dstBuf, 120*120*4, dstRectW, dstRectH, dstColorFormat);
                hal_g2d_set_color_replacement(g2dHandle, false, 0, 255, 0, 0, 0, 0, 0, 255);
                hal_g2d_set_destination_clip_window(g2dHandle, false, 0, 0, dstRectW, dstRectH);
                hal_g2d_set_source_key(g2dHandle, false, 0, 0, 0, 0);
                g2dRectFillSetDstWindow(g2dHandle, 0, 0, dstRectW, dstRectH);
                g2dRectFillSetColor(g2dHandle, 0x12345678);
                g2dRectFillStart(g2dHandle);
                hal_g2d_release_handle(g2dHandle);
                while(hal_g2d_get_status(g2dHandle)) {};
                hal_g2d_release_handle(g2dHandle);

                {
                    /// C-model
                    memset(dst_sw_image_120X120, 0xCD, 120*120*4);
                    dstBuf = (uint32_t *)&dst_sw_image_120X120[0];

                    g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)dstBuf);

                    g2d_c_model(&g2d_sw_register);
                }

                {
                    g2d_compare_sw_hw_result();
                }
            }
        }
    }

    for(readType = 0; readType < 3; readType++)
    {
        for(writeType = 0; writeType < 5; writeType++)
        {
            {
                G2D_LOGI("readType = %d, writeType= %d, disable slow dow\n\r",readType, writeType);
                /// HW
                memset(dst_hw_image_120X120, 0xCD, 120*120*4);

                dstBuf = (uint32_t *)&dst_hw_image_120X120[0];
                srcColorFormat = G2D_COLOR_FORMAT_ARGB8888;
                dstColorFormat = G2D_COLOR_FORMAT_ARGB8888;
                dstRectW = 100;
                dstRectH = 100;

                /// G2D_STATUS_BUSY means someone is using G2D
                if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
                    return;

                //slow down
                g2dSetReadBurstType(g2dHandle, readType);
                g2dSetWriteBurstType(g2dHandle, writeType);
                g2dSetSlowDownCount(g2dHandle, false, slowCount);

                hal_g2d_set_callback_function(g2dHandle, NULL);
                hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)dstBuf, 120*120*4, dstRectW, dstRectH, dstColorFormat);
                hal_g2d_set_color_replacement(g2dHandle, false, 0, 255, 0, 0, 0, 0, 0, 255);
                hal_g2d_set_destination_clip_window(g2dHandle, false, 0, 0, dstRectW, dstRectH);
                hal_g2d_set_source_key(g2dHandle, false, 0, 0, 0, 0);
                g2dRectFillSetDstWindow(g2dHandle, 0, 0, dstRectW, dstRectH);
                g2dRectFillSetColor(g2dHandle, 0x12345678);
                g2dRectFillStart(g2dHandle);
                hal_g2d_release_handle(g2dHandle);
                while(hal_g2d_get_status(g2dHandle)) {};
                hal_g2d_release_handle(g2dHandle);

                {
                    /// C-model
                    memset(dst_sw_image_120X120, 0xCD, 120*120*4);
                    dstBuf = (uint32_t *)&dst_sw_image_120X120[0];

                    g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)dstBuf);

                    g2d_c_model(&g2d_sw_register);
                }

                {
                    g2d_compare_sw_hw_result();
                }
            }
        }
    }

    REG_G2D_SLOW_DOWN = 0x00400000;
}



g2d_handle_t *g2dGlobalHandle;
volatile uint8_t g2d_interrupt_flag = 1;

bool g2d_callback(void* state)
{
    REG_G2D_IRQ &= ~G2D_IRQ_ENABLE_BIT;
    hal_g2d_release_handle(g2dGlobalHandle);
    g2d_interrupt_flag = 0;
    return true;
}


void g2d_basic_interrupt_test(void)
{
    uint32_t *dstBuf;
    g2d_color_format_t dstColorFormat, srcColorFormat;
    uint32_t dstRectW, dstRectH, srcRectW, srcRectH;
    G2D_LOGI("g2d_basic_interrupt_test()\n\r");
    {
        memset(dst_hw_image_120X120, 0xCD, 120*120*4);

        dstBuf = (uint32_t *)&dst_hw_image_120X120[0];
        srcColorFormat = G2D_COLOR_FORMAT_ARGB8888;
        dstColorFormat = G2D_COLOR_FORMAT_ARGB8888;
        dstRectW = 100;
        dstRectH = 100;
        /// G2D_STATUS_BUSY means someone is using G2D
        if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dGlobalHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
        {
        	G2D_LOGI("g2d_basic_interrupt_test busy!\n");
        	return;
        }
        hal_g2d_set_callback_function(g2dGlobalHandle, &g2d_callback);
		G2D_LOGI("Set Callback Function %x  IRQ=%x\n\r", g2d_callback, REG_G2D_IRQ);
        hal_set_destination_rgb_buffer_information(g2dGlobalHandle, (uint8_t *)dstBuf, 120*120*4, dstRectW, dstRectH, dstColorFormat);		
        hal_g2d_set_color_replacement(g2dGlobalHandle, false, 0, 255, 0, 0, 0, 0, 0, 255);
        hal_g2d_set_destination_clip_window(g2dGlobalHandle, false, 0, 0, dstRectW, dstRectH);				
        hal_g2d_set_source_key(g2dGlobalHandle, false, 0, 0, 0, 0);
        g2dRectFillSetDstWindow(g2dGlobalHandle, 0, 0, dstRectW, dstRectH);
        g2dRectFillSetColor(g2dGlobalHandle, 0x12345678);
        g2dRectFillStart(g2dGlobalHandle);

        //while(g2d_interrupt_flag) 		{}	
		G2D_LOGI("g2d_interrupt_flag exit\n");
		
        //while(hal_g2d_get_status(g2dGlobalHandle)) {};	//Dream add
        //hal_g2d_release_handle(g2dGlobalHandle);		//Dream 	add because prmask will be disable on boot
		
        {
            /// C-model
            memset(dst_sw_image_120X120, 0xCD, 120*120*4);
            dstBuf = (uint32_t *)&dst_sw_image_120X120[0];
            g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)dstBuf);
            g2d_c_model(&g2d_sw_register);
        }

        {
            g2d_compare_sw_hw_result();
        }

    }
}


#define MCU_TOPSM_base 0xA0180000
#define REG_RM_PWR_CON2 (*((volatile unsigned int*) RM_PWR_CON2_REG))
#define RM_PWR_CON2_REG (MCU_TOPSM_base + 0x0808) // MMSYS Power control

#if 0
uint32_t time1, time2;
uint32_t time[5];
uint32_t time_count;
#endif
void g2d_test_main(void)
{
    G2D_LOGI("======= g2d_test_main Start ========+\n\r");

    hal_clock_enable(HAL_CLOCK_CG_G2D) ;//power on Larkspur

    hal_g2d_init();
    srand(2523);

	g2d_flatten_test();
	if(0)
    {
		G2D_LOGI("time=%d\n\r", drv_get_current_time_dream());
        g2d_basic_register_read_write_test();		//DVT test 1 pass
        
        g2d_basic_reset_test();						//DVT test 2 pass
     
        g2d_basic_slow_down_test();					//DVT test 3 pass (10min+)
        
        g2d_basic_interrupt_test();					//DVT test 4 pass
   
        g2d_overlay_tile_scan_test();						//DVT test 5 pass	       
        g2d_overlay_layer_color_format_test();			//DVT test 6 pass
        g2d_overlay_layer_source_color_key_test();		//DVT test 7 pass


        g2d_overlay_layer_rotation_test();				//DVT test 8 pass~ long time test!!
        g2d_overlay_layer_alpha_blending_test();			//DVT test 9 pass 5min
        g2d_overlay_layer_coordinate_test();			//DVT 10 pass
        g2d_overlay_layer_size_test();				//DVT 11 pass
        g2d_overlay_layer_pitch_test();				//DVT 12 pass  
        g2d_overlay_layer_address_test();			//DVT 13 pass 3min

        g2d_overlay_layer_rectangle_fill_test();		//DVT 14 pass fast		
        g2d_overlay_layer_normal_font_test();			//DVT 15 pass		
        g2d_overlay_layer_aa_font_test();				//DVT 16 pass

        g2d_overlay_roi_clipping_window_test();		//DVT 17 pass
        g2d_overlay_roi_memory_out_color_format_test();	//DVT 18 pass
        g2d_overlay_roi_color_replacement_test();		//DVT 19 pass
        g2d_overlay_roi_backgroud_color_test();			//DVT 20 pass
        g2d_overlay_roi_output_constant_alpha_test();	//DVT 21 pass
	

        g2d_overlay_roi_memory_coordinate_test();		//DVT 22 pass
        g2d_overlay_roi_dithering_test();				//DVT 23 pass
        g2d_overlay_all_scenario_combination_test();	//DVT 24 pass 5hour test

#if 0
        //g2d_overlay_performance_test();
        //g2d_overlay_layer_alpha_blending_performance_test();

        //Measure the g2d overlay performance from pm requirement
        //g2d_overlay_performance_test_PM_Require_2Buffers();   //<==don't test it. 2013/1/4


        //Enable 2 layer, alpha disable src disable one of layer rotate will produce performace degrade (mt6250 hw bug)
        //Enable 2 layer, layer1 alpha enable layer0 alpha disable, layer0 and layer1 src key enable


        g2d_overlay_performance_test_PM_Require();	//DVT 25 pass
#endif
    }


    {
#if 0
        time1 = drv_get_current_time();
        for (i=0 ; i<1000 ; i++);
        time2 = drv_get_current_time();
        time[time_count++] = drv_get_duration_tick(time1, time2);
        time1 = drv_get_current_time();
        for (i=0 ; i<10000 ; i++);
        time2 = drv_get_current_time();
        time[time_count++] = drv_get_duration_tick(time1, time2);
#endif


#if 0

        //g2d_lt_WHQA_00009054_test();

        g2d_lt_performance_test();



        t32dclInit();
        g2d_lt_basic_test();
        g2d_lt_tile_scan_test();
        g2d_lt_color_format_test();
        g2d_lt_source_color_key_test();
        g2d_lt_alpha_blending_test();
        g2d_lt_roi_coordinate_test();
        g2d_lt_size_test();

        g2d_lt_pitch_test();
        g2d_lt_address_test();
        g2d_lt_sample_mode_test();
        g2d_lt_affine_rotate_test();
        g2d_lt_radom_matrix_test();
#endif

    }


    G2D_LOGI("=======g2d_test_main End ========\n\r");


}


