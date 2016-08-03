#ifndef DRV_FEATURES_6261_H 
#define DRV_FEATURES_6261_H 



/****************************MT6261  Driver features*******************************/



//#if  defined(MT6261) || defined(MT2501) || defined(MT2502) || defined(MT2523)


#define    DRV_RTC_6261_SERIES                      

#define    DRV_F32K_6261_SERIES                     

#define    DRV_SIM_6250_SERIES                      
                                            
#define    DRV_LCD_MT6260_SERIES                    

#define    DRV_GPIO_6261_SERIES                     

#define    DRV_GPT_6261_SERIES                      

#define    DRV_I2C_6261_SERIES                      

#define    DRV_PWM_6261_SERIES                      

#define    DRV_WDT_6261_SERIES                      
                                           
#define    DRV_MSDC_6261_SERIES    

#define    DRV_MISC_6261_SERIES                  
                                           
#define    __DRV_GRAPHICS_JPEG_6261_SERIES__        

#define    DRV_ACCDET_6261_SERIES                   

#define    DRV_BMT_6261_SERIES                      

#define    DRV_ISP_6235_SERIES

#define    DRV_ISP_MT6250_HW_SUPPORT 

#define    DRV_IDP_6252_SERIES                      

#define    DRV_MM_POWER_CTRL_MT6250_SERIES                                                   

#define    DRV_USB_6261_SERIES                      

#define    DRV_ADC_6261_SERIES                      

#define   DRV_CHR_DET_6261_SERIES

#define    DRV_KBD_6261_SERIES                      

#define    DRV_TP_6261_SERIES                       

#define    DRV_GCU_6225_SERIES                      

#define    DRV_SPI_6261_SERIES                      

#define    DRV_SPI_SLAVE_6261_SERIES                

#define    DRV_UART_6261_SERIES                     

#define    __DRV_GRAPHICS_RGB_RESIZER_6250_SERIES__ 

#define    __DRV_GRAPHICS_RGB_ROTATOR_6250_SERIES__ 

#define    __DRV_GRAPHICS_YUV_ROTATOR_6252_SERIES__ 

#if defined(G2D_SUPPORT)

  #define    __DRV_GRAPHICS_G2D_6260_SERIES__         

#endif

#define	   __DRV_COLOR_MT6260_SERIES__

//#endif   /*#if  defined(MT6261)*/ 

#endif /*#ifndef DRV_FEATURES_6261_H*/ 


 