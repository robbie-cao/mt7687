//#if defined (__DRV_GRAPHICS_G2D_6260_SERIES__)

///#include "g2d_engine.h"
#include "g2d_drv_c_model.h"
#include "assert.h"
#ifndef ASSERT
   #define ASSERT(x) { if (!(x))  while (1); }
#endif

//Dream: header file define
void Reg_to_G2DCONF( G2D_REG* reg, g2d_config* g2d_conf);
void sad_top(G2D_REG* reg);
void govl_top(G2D_REG* reg);


void g2d_c_model(G2D_REG* reg)
{
    g2d_config g2d_conf;
    
    Reg_to_G2DCONF(reg, &g2d_conf);
    switch(g2d_conf.eng_mode)
    {
       case BITBLT: 
         if(g2d_conf.sad_en  == 1)
         {
            sad_top(reg);
         }
         else
         {
            govl_top(reg);
         }
       break;
       case LINEAR_TRANSFORM:
          ///g2d_linear_transform_down( reg );
          ///g2d_linear_transform_up( reg );
       break;
       default: 
       	 ASSERT(0);
    }
}

//#endif
