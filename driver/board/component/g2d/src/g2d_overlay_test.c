#include "stdlib.h"
#include "string.h"
#include "g2d_enum.h"
#include "g2d_common_api.h"
#include "g2d_lt_api.h"
#include "g2d_bitblt_api.h"
#include "g2d_font_api.h"
#include "g2d_rectfill_api.h"
#include "g2d_overlay_api.h"

#include "g2d_drv_c_model_6260_series.h"
#include "g2d_test.h"

extern G2D_REG g2d_sw_register;
extern unsigned char rgb565_120X120[];

extern unsigned char layer0_image[];
extern unsigned char layer1_image[];

extern unsigned char dst_hw_image_120X120[];
extern unsigned char dst_sw_image_120X120[];


//Dream: Larkspur===============
#include "syslog.h"
#define G2D_LOGI(fmt,...) LOG_I(common,  (fmt), ##__VA_ARGS__)
//==============================

static void _g2d_compare_sw_hw_result100x100(void);


void g2d_overlay_tile_scan_test(void)
{
   g2d_handle_t *g2dHandle;

   uint8_t *layer0Addr;
   g2d_color_format_t layer0ColorFormat;
   uint32_t layer0Width, layer0Height;
   int32_t layer0RectX, layer0RectY;
   uint32_t layer0RectW, layer0RectH;
   bool layer0SrcKeyEnable;
   uint32_t layer0SrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layer0Rotate;
   bool layer0AlphaEnable;
   uint32_t layer0AlphaValue;

   uint8_t *layer1Addr;
   g2d_color_format_t layer1ColorFormat;
   uint32_t layer1Width, layer1Height;
   int32_t layer1RectX, layer1RectY;
   uint32_t layer1RectW, layer1RectH;
   bool layer1SrcKeyEnable;
   uint32_t layer1SrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layer1Rotate;
   bool layer1AlphaEnable;
   uint32_t layer1AlphaValue;

   uint32_t bgColor;
   bool outputAlphaEnable;
   uint32_t outputAlphaValue;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;

   uint8_t *dstBuf;
   g2d_color_format_t dstColorFormat;
   int32_t dstRectX, dstRectY;
   uint32_t dstWidth, dstHeight;

   G2D_LOGI("g2d_overlay_tile_scan_test()\n\r");

   {
      layer0Addr = (uint8_t *)&layer0_image[0];
      layer0ColorFormat = G2D_COLOR_FORMAT_PARGB6666;
      layer0Width = 100;
      layer0Height = 110;
      layer0RectX = 0;
      layer0RectY = 0;
      layer0RectW = 100;
      layer0RectH = 110;
      layer0SrcKeyEnable = KAL_FALSE;
      layer0SrcKeyValue = 0xFFFFFFFF;
      layer0Rotate = G2D_CW_ROTATE_ANGLE_270;
      layer0AlphaEnable = KAL_TRUE;
      layer0AlphaValue = 0x56;
      memset(layer0_image, 0x56, LAYER0_SIZE);

      layer1Addr = (uint8_t *)&layer1_image[0];
      layer1ColorFormat = G2D_COLOR_FORMAT_PARGB8565;
      layer1Width = 110;
      layer1Height = 100;
      layer1RectX = 0;
      layer1RectY = 0;
      layer1RectW = 110;
      layer1RectH = 100;
      layer1SrcKeyEnable = KAL_FALSE;
      layer1SrcKeyValue = 0xFFFFFFFF;
      layer1Rotate = G2D_CW_ROTATE_ANGLE_090;
      layer1AlphaEnable = KAL_TRUE;
      layer1AlphaValue = 0x34;
      memset(layer1_image, 0x34, LAYER1_SIZE);

      bgColor = 0x46;
      outputAlphaEnable = KAL_FALSE;
      outputAlphaValue = 0xFF;
      roiRectX = 0;
      roiRectY = 0;
      roiRectW = 110;
      roiRectH = 110;

      dstBuf = (uint8_t *)&dst_hw_image_120X120[0];
      dstColorFormat = G2D_COLOR_FORMAT_PARGB8888;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = 110;
      dstHeight = 110;

      {
         G2D_LOGI("FORCE_TS = KALSE\n\r");
         dstBuf = (uint8_t *)&dst_hw_image_120X120[0];
         memset(dst_hw_image_120X120, 0xCD, 120*120*4);

         /// G2D_STATUS_BUSY means someone is using G2D
         if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
         {
         	G2D_LOGI("G2D_STATUS_BUSY\n\r");
            return;
         }
         hal_g2d_set_callback_function(g2dHandle, NULL);
         g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
         hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)dstBuf, DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
         hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
         hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

         g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
         g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
         g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

         g2dOverlaySetBgColor(g2dHandle, bgColor);
         g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
         g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

         /// Layer 0
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
         g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
         g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, layer0Width, layer0Height, layer0ColorFormat);
         g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);
         g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
         g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
         g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);

         /// Layer 1
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
         g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
         g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, layer1Addr, LAYER1_SIZE, layer1Width, layer1Height, layer1ColorFormat);
         g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, layer1RectX, layer1RectY, layer1RectW, layer1RectH);
         g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layer1SrcKeyEnable, layer1SrcKeyValue);
         g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layer1Rotate);
         g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layer1AlphaEnable, layer1AlphaValue);
         g2dOverlayStart(g2dHandle);
         while(hal_g2d_get_status(g2dHandle)) {};

         hal_g2d_release_handle(g2dHandle);
         {
            /// C-model
            memset(dst_sw_image_120X120, 0xCD, 120*120*4);
            dstBuf = (uint8_t *)&dst_sw_image_120X120[0];
            g2d_set_registers_to_c_model(&g2d_sw_register, dstBuf);

            g2d_c_model(&g2d_sw_register);
         }
         {
            g2d_compare_sw_hw_result();
         }
      }

      {
         G2D_LOGI("FORCE_TS = TRUE, G2D_TILE_SIZE_TYPE_8x8\n\r");

         memset(dst_hw_image_120X120, 0xCD, 120*120*4);
         dstBuf = (uint8_t *)&dst_hw_image_120X120[0];
         /// G2D_STATUS_BUSY means someone is using G2D
         if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
            return;

         hal_g2d_set_callback_function(g2dHandle, NULL);
         g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
         hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)dstBuf, DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
         hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
         hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

         g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
         g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
         g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

         g2dOverlaySetBgColor(g2dHandle, bgColor);
         g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
         g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

         /// Layer 0
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
         g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
         g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, layer0Width, layer0Height, layer0ColorFormat);
         g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);
         g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
         g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
         g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);

         /// Layer 1
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
         g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
         g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, layer1Addr, LAYER1_SIZE, layer1Width, layer1Height, layer1ColorFormat);
         g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, layer1RectX, layer1RectY, layer1RectW, layer1RectH);
         g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layer1SrcKeyEnable, layer1SrcKeyValue);
         g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layer1Rotate);
         g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layer1AlphaEnable, layer1AlphaValue);

         g2dOverlayStart(g2dHandle);
         while(hal_g2d_get_status(g2dHandle)) {};
         hal_g2d_release_handle(g2dHandle);
         {
            /// C-model
            memset(dst_sw_image_120X120, 0xCD, 120*120*4);
            dstBuf = (uint8_t *)&dst_sw_image_120X120[0];

            g2d_set_registers_to_c_model(&g2d_sw_register, dstBuf);

            g2d_c_model(&g2d_sw_register);
         }

         {
            g2d_compare_sw_hw_result();
         }
      }

      {
         G2D_LOGI("FORCE_TS = TRUE, G2D_TILE_SIZE_TYPE_16x8\n\r");

         memset(dst_hw_image_120X120, 0xCD, 120*120*4);
         dstBuf = (uint8_t *)&dst_hw_image_120X120[0];
         /// G2D_STATUS_BUSY means someone is using G2D
         if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
            return;

         hal_g2d_set_callback_function(g2dHandle, NULL);
         g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
         hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)dstBuf, DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
         hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
         hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

         g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_16x8);
         g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
         g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

         g2dOverlaySetBgColor(g2dHandle, bgColor);
         g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
         g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

         /// Layer 0
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
         g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
         g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, layer0Width, layer0Height, layer0ColorFormat);
         g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);
         g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
         g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
         g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);

         /// Layer 1
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
         g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
         g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, layer1Addr, LAYER1_SIZE, layer1Width, layer1Height, layer1ColorFormat);
         g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, layer1RectX, layer1RectY, layer1RectW, layer1RectH);
         g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layer1SrcKeyEnable, layer1SrcKeyValue);
         g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layer1Rotate);
         g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layer1AlphaEnable, layer1AlphaValue);

         g2dOverlayStart(g2dHandle);
         while(hal_g2d_get_status(g2dHandle)) {};
         hal_g2d_release_handle(g2dHandle);
         {
            /// C-model
            memset(dst_sw_image_120X120, 0xCD, 120*120*4);
            dstBuf = (uint8_t *)&dst_sw_image_120X120[0];

            g2d_set_registers_to_c_model(&g2d_sw_register, dstBuf);

            g2d_c_model(&g2d_sw_register);
         }

         {
            g2d_compare_sw_hw_result();
         }
      }
   }
}



void g2d_overlay_layer_color_format_test(void)
{
   /*****************************************
    * To test all kind of layer color format
    * 1. Memory output fixes as ARGB8888
    * 2. Overlay layer 0, enable background color
    * 3. Test layer color format
    *    a. RGB565
    *    b. BGR888
    *    c. RGB888
    *    d. YUYV422
    *    e. ARGB8888
    *    f. PARGB8888
    *****************************************/

   g2d_handle_t *g2dHandle;

   uint8_t *layer0Addr;
   uint32_t layer0ColorFormat;
   uint32_t layer0Width, layer0Height;
   int32_t layer0RectX, layer0RectY;
   uint32_t layer0RectW, layer0RectH;
   bool layer0SrcKeyEnable;
   uint32_t layer0SrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layer0Rotate;
   bool layer0AlphaEnable;
   uint32_t layer0AlphaValue;

   uint32_t bgColor;
   bool outputAlphaEnable;
   uint32_t outputAlphaValue;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;

   uint8_t *dstBuf;
   g2d_color_format_t dstColorFormat;
   int32_t dstRectX, dstRectY;
   uint32_t dstWidth, dstHeight;

   {
      layer0Addr = (uint8_t *)&layer0_image[0];
      //layer0ColorFormat = G2D_COLOR_FORMAT_RGB565;
      layer0Width = 111;
      layer0Height = 112;
      layer0RectX = 0;
      layer0RectY = 0;
      layer0RectW = 100;
      layer0RectH = 99;
      layer0SrcKeyEnable = KAL_FALSE;
      layer0SrcKeyValue = 0xFFFFFFFF;
      layer0Rotate = G2D_CW_ROTATE_ANGLE_000;
      layer0AlphaEnable = KAL_TRUE;
      layer0AlphaValue = 0x56;
      memset(layer0_image, 0x56, LAYER0_SIZE);

      bgColor = 0x46;
      outputAlphaEnable = KAL_FALSE;
      outputAlphaValue = 0xFF;
      roiRectX = 0;
      roiRectY = 0;
      roiRectW = 66;
      roiRectH = 77;

      dstBuf = (uint8_t *)&dst_hw_image_120X120[0];
      dstColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = 66;
      dstHeight = 77;

      G2D_LOGI("g2d_overlay_layer_color_format_test()\n\r");
      {
         dstBuf = (uint8_t *)&dst_hw_image_120X120[0];
         memset(dst_hw_image_120X120, 0xCD, 120*120*4);

         /// G2D_STATUS_BUSY means someone is using G2D
         if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
         {
            G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
            return;
         }

         hal_g2d_set_callback_function(g2dHandle, NULL);
         g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
         hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)dstBuf, DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
         hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
         hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

         g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
         g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
         g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

         g2dOverlaySetBgColor(g2dHandle, bgColor);
         g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
         g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

         /// Layer 0
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
         g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
         //g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, layer0Width, layer0Height, layer0ColorFormat);
         g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);
         g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
         g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
         g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);


         for(layer0ColorFormat = 1; layer0ColorFormat <= 10; layer0ColorFormat++)
         {
            G2D_LOGI("\tColor Format: %d\n\r", layer0ColorFormat);
            if(G2D_COLOR_FORMAT_UYVY422 == layer0ColorFormat)
            {
               uint8_t *bufferAddr[3];
               uint32_t bufferSize[3];

               bufferAddr[0] = layer0Addr;
               bufferAddr[1] = NULL;
               bufferAddr[2] = NULL;
               bufferSize[0] = LAYER0_SIZE;
               bufferSize[1] = 0;
               bufferSize[2] = 0;

               g2dOverlaySetLayerYUVBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, bufferAddr, bufferSize, layer0Width, layer0Height, (g2d_color_format_t)layer0ColorFormat);
            }
            else
               g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, layer0Width, layer0Height, (g2d_color_format_t)layer0ColorFormat);

            g2dOverlayStart(g2dHandle);
            while(hal_g2d_get_status(g2dHandle)) {};
            {
               /// C-model
               memset(dst_sw_image_120X120, 0xCD, 120*120*4);
               dstBuf = (uint8_t *)&dst_sw_image_120X120[0];

               g2d_set_registers_to_c_model(&g2d_sw_register, dstBuf);

               g2d_c_model(&g2d_sw_register);
            }

            {
               g2d_compare_sw_hw_result();
            }
         }

         hal_g2d_release_handle(g2dHandle);
      }
   }
}



void g2d_overlay_layer_source_color_key_test(void)
{
   /*****************************************
    * To test source key in all kind of layer color formats
    * 1. Enable source key
    * 2. Test layer color format
    *    a. RGB565
    *    b. BGR888
    *    c. RGB888
    *    d. ARGB8888
    *    e. PARGB8888
    *****************************************/

   g2d_handle_t *g2dHandle;

   uint8_t *layer0Addr;
   uint32_t layer0ColorFormat;
   uint32_t layer0Width, layer0Height;
   int32_t layer0RectX, layer0RectY;
   uint32_t layer0RectW, layer0RectH;
   //bool layer0SrcKeyEnable;
   uint32_t layer0SrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layer0Rotate;
   bool layer0AlphaEnable;
   uint32_t layer0AlphaValue;

   uint32_t bgColor;
   bool outputAlphaEnable;
   uint32_t outputAlphaValue;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;

   uint8_t *dstBuf;
   g2d_color_format_t dstColorFormat;
   int32_t dstRectX, dstRectY;
   uint32_t dstWidth, dstHeight;

   {
      layer0Addr = (uint8_t *)&layer0_image[0];
      //layer0ColorFormat = G2D_COLOR_FORMAT_RGB565;
      layer0Width = 120;
      layer0Height = 120;
      layer0RectX = 0;
      layer0RectY = 0;
      layer0RectW = 120;
      layer0RectH = 160;
      //layer0SrcKeyEnable = KAL_TRUE;
      layer0SrcKeyValue = 0xFFFFFFFF;
      layer0Rotate = G2D_CW_ROTATE_ANGLE_000;
      layer0AlphaEnable = KAL_FALSE;
      layer0AlphaValue = 0x56;
      //memset(layer0_image, 0x56, 120*120*4);

      bgColor = 0x46;
      outputAlphaEnable = KAL_FALSE;
      outputAlphaValue = 0xFF;
      roiRectX = 60;
      roiRectY = 80;
      roiRectW = 61;//121;
      roiRectH = 80;//160;

      dstBuf = (uint8_t *)&dst_hw_image_120X120[0];
      dstColorFormat = G2D_COLOR_FORMAT_RGB888;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = 61;//121;
      dstHeight = 80;//160;

      G2D_LOGI("g2d_overlay_layer_source_color_key_test()\n\r");
      {
         dstBuf = (uint8_t *)&dst_hw_image_120X120[0];
         memset(dst_hw_image_120X120, 0xCD, 120*120*4);
         memset(layer0_image, 0xFF, 120*120*4);

         /// G2D_STATUS_BUSY means someone is using G2D
         if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
         {
            G2D_LOGI("\tError: What are you doing!! I'm busy now. g2d_overlay_layer_source_color_key_test\n\r");
            return;
         }

         hal_g2d_set_callback_function(g2dHandle, NULL);
         g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
         hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)dstBuf, DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
         hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
         hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

         g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
         g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
         g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

         g2dOverlaySetBgColor(g2dHandle, bgColor);
         g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
         g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

         /// Layer 0
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
         g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
         //g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, layer0Width, layer0Height, layer0ColorFormat);
         g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);
         //g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
         g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
         g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);


         for(layer0ColorFormat = 1; layer0ColorFormat <= 10; layer0ColorFormat++)
         {
            G2D_LOGI("\tColor Format: %d\n\r", layer0ColorFormat);
            if(G2D_COLOR_FORMAT_UYVY422 == layer0ColorFormat)
            {
               continue;
            }
            else
               g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, layer0Width, layer0Height, (g2d_color_format_t)layer0ColorFormat);


            G2D_LOGI("\t  Enabled\r\n");
            g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE, layer0SrcKeyValue);
            g2dOverlayStart(g2dHandle);
            while(hal_g2d_get_status(g2dHandle)) {};
            {
               /// C-model
               memset(dst_sw_image_120X120, 0xCD, 120*120*4);
               dstBuf = (uint8_t *)&dst_sw_image_120X120[0];

               g2d_set_registers_to_c_model(&g2d_sw_register, dstBuf);

               g2d_c_model(&g2d_sw_register);
            }

            {
               g2d_compare_sw_hw_result();
            }

            G2D_LOGI("\t  Disabled\r\n");
            g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, KAL_FALSE, layer0SrcKeyValue);
            g2dOverlayStart(g2dHandle);
            while(hal_g2d_get_status(g2dHandle)) {};
            {
               /// C-model
               memset(dst_sw_image_120X120, 0xCD, 120*120*4);
               dstBuf = (uint8_t *)&dst_sw_image_120X120[0];

               g2d_set_registers_to_c_model(&g2d_sw_register, dstBuf);

               g2d_c_model(&g2d_sw_register);
            }

            {
               g2d_compare_sw_hw_result();
            }
         }

         hal_g2d_release_handle(g2dHandle);
      }
   }
}



void g2d_overlay_layer_rotation_test(void)
{
   /*****************************************
    * To test layer rotation in all directions
    * 1. Layer color format is RGB565
    * 2. Test layer rotation
    *    a. 0, 90, 180, 270
    *    b. rotation with mirror
    * 3. Layer combinations
    *    a. layer 0
    *    b. layer 0 + 1
    *    c. layer 0 + 1 + 2
    *    d. layer 0 + 1 + 2 + 3
    *****************************************/

   g2d_handle_t *g2dHandle;

   uint8_t *layerAddr = (uint8_t *)&rgb565_120X120[0];
   g2d_color_format_t layerColorFormat = G2D_COLOR_FORMAT_RGB565;
   uint32_t layerWidth = 80;
   uint32_t layerHeight = 90;
   uint32_t layerRectW = 51;
   uint32_t layerRectH = 61;
   bool layerSrcKeyEnable = KAL_TRUE;
   uint32_t layerSrcKeyValue = 0xFFFFFFFF;
   bool layerAlphaEnable = KAL_TRUE;
   uint32_t layerAlphaValue = 0x33;

   uint32_t bgColor = 0x0;
   bool outputAlphaEnable = KAL_FALSE;
   uint32_t outputAlphaValue = 0xFF;
   int32_t roiRectX = 0;
   int32_t roiRectY = 0;
   uint32_t roiRectW = 80;
   uint32_t roiRectH = 90;

   g2d_color_format_t dstColorFormat = G2D_COLOR_FORMAT_RGB888;
   int32_t dstRectX = 0;
   int32_t dstRectY = 0;
   uint32_t dstWidth = 80;
   uint32_t dstHeight = 90;

   int32_t i0, i1, i2, i3;
   bool i1On = KAL_FALSE, i2On = KAL_FALSE, i3On = KAL_FALSE;

   {
      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      // layer 0
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layerAddr, 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 23, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      // layer 1
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, layerAddr, 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 0, 21, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layerAlphaEnable, layerAlphaValue);

      // layer 2
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER2, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER2, layerAddr, 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER2, 11, 15, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER2, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER2, layerAlphaEnable, layerAlphaValue);

      // layer 3
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER3, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER3, layerAddr, 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER3, -3, -2, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER3, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER3, layerAlphaEnable, layerAlphaValue);

      G2D_LOGI("g2d_overlay_layer_rotation_test()\n\r");
      {
         //i3On = KAL_TRUE;
         for(i3 = 0; i3< 8; i3++)
         {
            if(i3On)
            {
               g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER3, KAL_TRUE);
               g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER3, (G2D_CW_ROTATE_ANGLE_ENUM)i3);
            }
            else
            {
               i3On = KAL_TRUE;
               i3 = -1;
            }

            //i2On = KAL_TRUE; 
            for(i2 = 0; i2< 8; i2++)
            {
               if(i2On)
               {
                  g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER2, KAL_TRUE);
                  g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER2, (G2D_CW_ROTATE_ANGLE_ENUM)i2);
               }
               else
               {
                  i2On = KAL_TRUE;
                  i2 = -1;
               }
               
               //i1On = KAL_TRUE;
               for(i1 = 0; i1 < 8; i1++)
               {
                  if(i1On)
                  {
                     g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
                     g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, (G2D_CW_ROTATE_ANGLE_ENUM)i1);
                  }
                  else
                  {
                     i1On = KAL_TRUE;
                     i1 = -1;
                  }

                  for(i0 = 0; i0 < 8; i0++)
                  {
                     g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
                     g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, (G2D_CW_ROTATE_ANGLE_ENUM)i0);

                     G2D_LOGI("Layer 0: %d, Layer 1: %d, Layer 2: %d, Layer 3: %d \n\r", i0, i1On?i1:9, i2On?i2:9, i3On?i3:9);

                     memset(dst_hw_image_120X120, 0xCD, 120*120*4);

                     g2dOverlayStart(g2dHandle);
                     while(hal_g2d_get_status(g2dHandle)) {};
                     {
                        /// C-model
                        memset(dst_sw_image_120X120, 0xCD, 120*120*4);

                        g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)&dst_sw_image_120X120[0]);
                        g2d_c_model(&g2d_sw_register);
                     }
                     {
                        _g2d_compare_sw_hw_result100x100();
                     }
                  }
               }
            }
         }
      }

      hal_g2d_release_handle(g2dHandle);
   }
}


void g2d_overlay_layer_alpha_blending_test(void)
{
   /*****************************************
    * To test the correctness of alpha blending
    * 1. Fix Layer 0: ARGB8888 without alpha blending
    * 2. Layer 1
    *  I. Alpha value
    *    a. 0x00
    *    b. 0xFF
    *    c. random
    *  II. Color format
    *    a. RGB565
    *    b. BGR888
    *    c. RGB888
    *    d. ARGB8888
    *    e. PARGB8888
    *    f. YUV422
    *****************************************/

   g2d_handle_t *g2dHandle;

   uint8_t *layerAddr;

   uint32_t layerWidth, layer_height;
   bool layerSrcKeyEnable;
   uint32_t layerSrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layerRotate;
   uint32_t layerAlphaValue;

   uint32_t bgColor;
   bool outputAlphaEnable;
   uint32_t outputAlphaValue;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;

   g2d_color_format_t dstColorFormat;
   int32_t dstRectX, dstRectY;
   uint32_t dstWidth, dstHeight;

   int32_t colorLoop, alphaLoop;
   int32_t i0, i1, i2, i3;

   G2D_LOGI("g2d_overlay_layer_alpha_blending_test()\n\r");

   {
      layerAddr = (uint8_t *)(uint8_t *)&layer0_image[0];
      //layer0ColorFormat = G2D_COLOR_FORMAT_RGB565;
      layerWidth = 120;
      layer_height = 120;
      layerSrcKeyEnable = KAL_FALSE;
      layerSrcKeyValue = 0xFFFFFFFF;
      layerRotate = G2D_CW_ROTATE_ANGLE_000;
      //layerAlphaValue = 0x56;

      bgColor = 0x66;
      outputAlphaEnable = KAL_FALSE;
      outputAlphaValue = 0xFF;
      roiRectX = 0;
      roiRectY = 0;
      roiRectW = 100;
      roiRectH = 100;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = 100;
      dstHeight = 100;

      {
        uint32_t i;
        uint32_t *ptr = (uint32_t *)&layer0_image[0];

        for(i = 0; i < 120 * 120; i++)
        {
           *ptr = rand() & 0xFFFFFFFF;
           ptr++;
        }
      }

      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      /// Layer 0
      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layerAddr, LAYER0_SIZE, layerWidth, layer_height, G2D_COLOR_FORMAT_ARGB8888);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, 100, 100);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, KAL_FALSE, 0);

      /// Layer 1
      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 0, 0, 100, 100);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      for(colorLoop = 1; colorLoop <= 10; colorLoop++)
      {
         G2D_LOGI("Color Format: %d\n\r", colorLoop);
         if(G2D_COLOR_FORMAT_UYVY422 == colorLoop)
         {
            uint8_t *bufferAddr[3];
            uint32_t bufferSize[3];

            bufferAddr[0] = layerAddr;
            bufferAddr[1] = NULL;
            bufferAddr[2] = NULL;
            bufferSize[0] = LAYER0_SIZE;
            bufferSize[1] = 0;
            bufferSize[2] = 0;

            g2dOverlaySetLayerYUVBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, bufferAddr, bufferSize, layerWidth, layer_height, (g2d_color_format_t)colorLoop);
         }
         else
         {
            g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, layerAddr, LAYER0_SIZE, layerWidth, layer_height, (g2d_color_format_t)colorLoop);
         }

         for(alphaLoop = 0; alphaLoop < 5; alphaLoop++)
         {
            if(alphaLoop > 1)
            {
               layerAlphaValue = rand();
            }
            else if(alphaLoop == 1)
            {
               layerAlphaValue = 0xFF;
            }
            else
            {
               layerAlphaValue = 0x00;
            }

            G2D_LOGI("  Alpha Value: %x\n\r", layerAlphaValue);
            g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE, layerAlphaValue);

            memset(dst_hw_image_120X120, 0xCD, 120*120*4);
            g2dOverlayStart(g2dHandle);
            while(hal_g2d_get_status(g2dHandle)) {};
            {
               /// C-model
               memset(dst_sw_image_120X120, 0xCD, 120*120*4);
               g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)&dst_sw_image_120X120[0]);
               g2d_c_model(&g2d_sw_register);
            }

            g2d_compare_sw_hw_result();
         }
      }

      G2D_LOGI("Multiple Layer Alpha Blending Test\n\r");

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER2, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER2, layerAddr, LAYER0_SIZE, layerWidth, layer_height, G2D_COLOR_FORMAT_ARGB6666);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER2, 0, 0, 100, 100);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER2, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER2, layerRotate);

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER3, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER3, layerAddr, LAYER0_SIZE, layerWidth, layer_height, G2D_COLOR_FORMAT_PARGB8565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER3, 0, 0, 100, 100);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER3, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER3, layerRotate);

      G2D_LOGI("==-1: not enabled, 0: 0, 1: random, 2: 0xFF==\n\r");
      for(i3 = -1; i3 < 3; i3++)
      {
         if(i3 < 0)
         {
            g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER3, KAL_FALSE);
         }
         else
         {
            g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER3, KAL_TRUE);
            g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER3, KAL_TRUE, rand());
         }

         for(i2 = -1; i2 < 3; i2++)
         {
            if(i2 < 0)
            {
               g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER2, KAL_FALSE);
            }
            else
            {
               g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER2, KAL_TRUE);
               g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER2, KAL_TRUE, rand());
            }

            for(i1 = -1; i1 < 3; i1++)
            {
               if(i1 < 0)
               {
                  g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_FALSE);
               }
               else
               {
                  g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
                  g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE, rand());
               }

               for(i0 = -1; i0 < 3; i0++)
               {
                  if(i0 < 0)
                  {
                     g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_FALSE);
                  }
                  else
                  {
                     g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
                     g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE, rand());
                  }

                  G2D_LOGI("Layer 0: %d, Layer 1: %d, Layer 2: %d, Layer 3: %d\n\r", i0, i1, i2, i3);

                  memset(dst_hw_image_120X120, 0xCD, 120*120*4);
                  g2dOverlayStart(g2dHandle);
                  while(hal_g2d_get_status(g2dHandle)) {};
                  {
                     /// C-model
                     memset(dst_sw_image_120X120, 0xCD, 120*120*4);
                     g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)&dst_sw_image_120X120[0]);
                     g2d_c_model(&g2d_sw_register);
                  }

                  g2d_compare_sw_hw_result();
               }
            }
         }
      }

      hal_g2d_release_handle(g2dHandle);
   }
}



void g2d_overlay_layer_coordinate_test(void)
{
   /*****************************************
    * To test the OFFSET correctness.
    * 1. Layer 0
    *    a. x from -2048 to 2047
    *    b. y from -2048 to 2047
    *    c. should cover the boundary case
    *****************************************/

   g2d_handle_t *g2dHandle;

   uint8_t *layer0Addr;
   g2d_color_format_t layer0ColorFormat;
   uint32_t layer0Width, layer0Height;
   int32_t layer0RectX, layer0RectY;
   uint32_t layer0RectW, layer0RectH;
   bool layer0SrcKeyEnable;
   uint32_t layer0SrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layer0Rotate;
   bool layer0AlphaEnable;
   uint32_t layer0AlphaValue;

   uint32_t bgColor;
   bool outputAlphaEnable;
   uint32_t outputAlphaValue;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;

   g2d_color_format_t dstColorFormat;
   int32_t dstRectX, dstRectY;
   uint32_t dstWidth, dstHeight;

   int32_t loopCount;

   G2D_LOGI("g2d_overlay_layer_coordinate_test()\n\r");

   {
      layer0Addr = (uint8_t *)&layer0_image[0];
      layer0ColorFormat = G2D_COLOR_FORMAT_RGB888;
      layer0Width = 1;
      layer0Height = 1;
      //layer0RectX = 0;
      //layer0RectY = 0;
      layer0RectW = 1;
      layer0RectH = 1;
      layer0SrcKeyEnable = KAL_FALSE;
      layer0SrcKeyValue = 0xFFFFFFFF;
      layer0Rotate = G2D_CW_ROTATE_ANGLE_000;
      layer0AlphaEnable = KAL_FALSE;
      layer0AlphaValue = 0xFF;
      memset(layer0_image, 0x56, 120*120*4);

      bgColor = 0xF6;
      outputAlphaEnable = KAL_FALSE;
      outputAlphaValue = 0xFF;
      roiRectX = -5;
      roiRectY = -10;
      roiRectW = 54;
      roiRectH = 87;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = 48;
      dstHeight = 48;

      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, layer0Width, layer0Height, layer0ColorFormat);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);

      for(loopCount = 295; loopCount < 500; loopCount++)
      {
         if(loopCount >= 250)
         {
            layer0RectX = (rand() % 4096) - 2048;
            layer0RectY = (rand() % 4096) - 2048;

            layer0RectW = 5;
            layer0RectH = 2048;

            g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, 5, 2048, layer0ColorFormat);
            g2dOverlaySetRoiWindow(g2dHandle, -1024, -2, 2048, 4);
            hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, 2048, 4, dstColorFormat);
         }
         else if(loopCount >= 9)
         {
            layer0RectX = (rand() % 4096) - 2048;
            layer0RectY = (rand() % 4096) - 2048;

            layer0RectW = 2048;
            layer0RectH = 5;

            g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, 2048, 5, layer0ColorFormat);
            g2dOverlaySetRoiWindow(g2dHandle, -2, -1024, 4, 2048);
            hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, 4, 2048, dstColorFormat);
         }
         else if(loopCount == 8)
         {
            layer0RectX = 2047;
            layer0RectY = 2047;

            g2dOverlaySetRoiWindow(g2dHandle, 2000, 2000, 48, 48);
         }
         else if(loopCount == 7)
         {
            layer0RectX = 2047;
            layer0RectY = 0;

            g2dOverlaySetRoiWindow(g2dHandle, 2000, -24, 48, 48);
         }
         else if(loopCount == 6)
         {
            layer0RectX = 2047;
            layer0RectY = -2048;

            g2dOverlaySetRoiWindow(g2dHandle, 2000, -2048, 48, 48);
         }
         else if(loopCount == 5)
         {
            layer0RectX = 0;
            layer0RectY = 2047;

            g2dOverlaySetRoiWindow(g2dHandle, -24, 2000, 48, 48);
         }
         else if(loopCount == 4)
         {
            layer0RectX = 0;
            layer0RectY = 0;

            g2dOverlaySetRoiWindow(g2dHandle, -24, -24, 48, 48);
         }
         else if(loopCount == 3)
         {
            layer0RectX = 0;
            layer0RectY = -2048;

            g2dOverlaySetRoiWindow(g2dHandle, -24, -2048, 48, 48);
         }
         else if(loopCount == 2)
         {
            layer0RectX = -2048;
            layer0RectY = 2047;

            g2dOverlaySetRoiWindow(g2dHandle, -2048, 2000, 48, 48);
         }
         else if(loopCount == 1)
         {
            layer0RectX = -2048;
            layer0RectY = 0;

            g2dOverlaySetRoiWindow(g2dHandle, -2048, -24, 48, 48);
         }
         else
         {
            layer0RectX = -2048;
            layer0RectY = -2048;

            g2dOverlaySetRoiWindow(g2dHandle, -2048, -2048, 48, 48);
         }

         G2D_LOGI("  X OFFSET: %d\tY OFFSET: %d\n\r", layer0RectX, layer0RectY);

         memset(dst_hw_image_120X120, 0xCD, 120*120*4);


         g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);

         /// Layer 0

         g2dOverlayStart(g2dHandle);
         while(hal_g2d_get_status(g2dHandle)) {};
         {
            /// C-model
            memset(dst_sw_image_120X120, 0xCD, 120*120*4);

            g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)&dst_sw_image_120X120[0]);

            g2d_c_model(&g2d_sw_register);
         }

         {
            g2d_compare_sw_hw_result();
         }
      }

      hal_g2d_release_handle(g2dHandle);
   }
}



void g2d_overlay_layer_size_test(void)
{
   /*****************************************
    * To test the layer size.
    * 1. Layer 0
    *    a. width from 1 to 2048
    *    b. height from 1 to 2048
    *    c. should cover the boundary case
    *****************************************/

   g2d_handle_t *g2dHandle;

   uint8_t *layer0Addr;
   g2d_color_format_t layer0ColorFormat;
   uint32_t layer0Width, layer0Height;
   int32_t layer0RectX, layer0RectY;
   uint32_t layer0RectW, layer0RectH;
   bool layer0SrcKeyEnable;
   uint32_t layer0SrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layer0Rotate;
   bool layer0AlphaEnable;
   uint32_t layer0AlphaValue;

   uint32_t bgColor;
   bool outputAlphaEnable;
   uint32_t outputAlphaValue;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;

   uint8_t *dstBuf;
   g2d_color_format_t dstColorFormat;
   int32_t dstRectX, dstRectY;
   uint32_t dstWidth, dstHeight;

   int32_t loopCount;

   G2D_LOGI("g2d_overlay_layer_size_test()\n\r");

   {
      layer0Addr = (uint8_t *)&rgb565_120X120[0];
      layer0ColorFormat = G2D_COLOR_FORMAT_RGB565;
      layer0Width = 120;
      layer0Height = 120;
      layer0RectX = 0;
      layer0RectY = 0;
      //layer0RectW = 1;
      //layer0RectH = 1;
      layer0SrcKeyEnable = KAL_FALSE;
      layer0SrcKeyValue = 0xFFFFFFFF;
      layer0Rotate = G2D_CW_ROTATE_ANGLE_000;
      layer0AlphaEnable = KAL_FALSE;
      layer0AlphaValue = 0xFF;
      memset(layer0_image, 0x56, 120*120*4);

      bgColor = 0xF6;
      outputAlphaEnable = KAL_FALSE;
      outputAlphaValue = 0xFF;
      roiRectX = -5;
      roiRectY = -10;
      roiRectW = 55;
      roiRectH = 60;

      dstBuf = (uint8_t *)&dst_hw_image_120X120[0];
      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = 60;
      dstHeight = 60;

      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, layer0Width, layer0Height, layer0ColorFormat);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);

      for(loopCount = 0; loopCount < 106; loopCount++)
      {
         if(loopCount >= 57)
         {
            int32_t tempW, tempH;

            layer0Width = 2048;
            layer0Height = 5;
            g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, layer0Width, layer0Height, layer0ColorFormat);

            layer0RectW = (rand() % 2048) + 1;
            layer0RectH = 5;

            tempW = layer0RectW - 24;
            if(tempW + 48 > 2048)
               tempW= 2048 - 48;

            tempH = 0;

            g2dOverlaySetRoiWindow(g2dHandle, tempW, tempH, 48, 48);
         }
         else if(loopCount >= 7)
         {
            int32_t tempW, tempH;

            layer0Width = 5;
            layer0Height = 2048;
            g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, layer0Width, layer0Height, layer0ColorFormat);

            layer0RectW = 5;
            layer0RectH = (rand() % 2048) + 1;

            tempW = 0;

            tempH = layer0RectH - 24;
            if(tempH + 48 > 2048)
               tempH= 2048 - 48;

            g2dOverlaySetRoiWindow(g2dHandle, tempW, tempH, 48, 48);
         }
         /*else if(loopCount == 7) //this case data abort due to we don't have so much memory
         {
            layer0Width = 2048;
            layer0Height = 2048;
            g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, layer0Width, layer0Height, layer0ColorFormat);

            layer0RectW = 2048;
            layer0RectH = 2048;

            g2dOverlaySetRoiWindow(g2dHandle, 2000, 2000, 48, 48);
         }*/
         else if(loopCount == 6)
         {
            layer0Width = 1;
            layer0Height = 2048;
            g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, layer0Width, layer0Height, layer0ColorFormat);

            layer0RectW = 2048;
            layer0RectH = 1;

            g2dOverlaySetRoiWindow(g2dHandle, 2000, -24, 48, 48);
         }
         else if(loopCount == 5)
         {
            layer0Width = 1;
            layer0Height = 2048;
            g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, layer0Width, layer0Height, layer0ColorFormat);

            layer0RectW = 1;
            layer0RectH = 2048;

            g2dOverlaySetRoiWindow(g2dHandle, -24, 2000, 48, 48);
         }
         else if(loopCount == 4)
         {
            layer0Width = 1;
            layer0Height = 2048;
            layer0RectW = 1;
            layer0RectH = 1;

            g2dOverlaySetRoiWindow(g2dHandle, -24, -24, 48, 48);
            g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, layer0Width, layer0Height, layer0ColorFormat);
         }
         else if(loopCount == 3)
         {
            layer0RectW = 2048;
            layer0RectH = 2048;
         }
         else if(loopCount == 2)
         {
            layer0RectW = 2048;
            layer0RectH = 1;
         }
         else if(loopCount == 1)
         {
            layer0RectW = 1;
            layer0RectH = 2048;
         }
         else
         {
            layer0RectW = 1;
            layer0RectH = 1;
         }

         G2D_LOGI("  Width: %d\tHeight: %d\n\r", layer0RectW, layer0RectH);

         dstBuf = (uint8_t *)&dst_hw_image_120X120[0];
         memset(dst_hw_image_120X120, 0xCD, 120*120*4);


         g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);

         g2dOverlayStart(g2dHandle);
         while(hal_g2d_get_status(g2dHandle)) {};
         {
            /// C-model
            memset(dst_sw_image_120X120, 0xCD, 120*120*4);
            dstBuf = (uint8_t *)&dst_sw_image_120X120[0];

            g2d_set_registers_to_c_model(&g2d_sw_register, dstBuf);

            g2d_c_model(&g2d_sw_register);
         }

         {
            g2d_compare_sw_hw_result();
         }
      }

      hal_g2d_release_handle(g2dHandle);
   }
}




void g2d_overlay_layer_pitch_test(void)
{
   /*****************************************
    * To test the layer pitch.
    * 1. Layer 0
    *    a. pitch from 1*2 to 2048*4
    *    b. RGB565, BRG888, RGB888, YUYV422, ARGB8888, PARGB8888
    *    c. should cover the boundary case
    *****************************************/

   g2d_handle_t *g2dHandle;

   uint8_t *layer0Addr;
   uint32_t layer0ColorFormat;
   uint32_t layer0Width, layer0Height;
   int32_t layer0RectX, layer0RectY;
   uint32_t layer0RectW, layer0RectH;
   bool layer0SrcKeyEnable;
   uint32_t layer0SrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layer0Rotate;
   bool layer0AlphaEnable;
   uint32_t layer0AlphaValue;

   uint32_t bgColor;
   bool outputAlphaEnable;
   uint32_t outputAlphaValue;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;

   g2d_color_format_t dstColorFormat;
   int32_t dstRectX, dstRectY;
   uint32_t dstWidth, dstHeight;

   int32_t loopCount;

   G2D_LOGI("g2d_overlay_layer_pitch_test()\n\r");

   {
      layer0Addr = (uint8_t *)&layer0_image[0];
      //layer0ColorFormat = G2D_COLOR_FORMAT_RGB565;
      //layer0Width = 240;
      layer0Height = 120;
      layer0RectX = 0;
      layer0RectY = 0;
      layer0RectW = 48;
      layer0RectH = 10;
      layer0SrcKeyEnable = KAL_FALSE;
      layer0SrcKeyValue = 0xFFFFFFFF;
      layer0Rotate = G2D_CW_ROTATE_ANGLE_000;
      layer0AlphaEnable = KAL_FALSE;
      layer0AlphaValue = 0xFF;
      memset(layer0_image, 0x56, 120*120*4);

      bgColor = 0xF6;
      outputAlphaEnable = KAL_FALSE;
      outputAlphaValue = 0xFF;
      roiRectX = 0;
      roiRectY = 0;
      roiRectW = 48;
      roiRectH = 10;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = 48;
      dstHeight = 15;

      {
        uint32_t i;
        uint32_t *ptr = (uint32_t *)&layer0_image[0];

        for(i = 0; i < 120 * 120; i++)
        {
           *ptr = rand() & 0xFFFFFFFF;
           ptr++;
        }
      }

      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      //g2dOverlaySetLayerBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, layer0Width, layer0Height, layer0ColorFormat);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);

      for(layer0ColorFormat = 1; layer0ColorFormat <= 10; layer0ColorFormat++)
      {
         G2D_LOGI("Color Format: %d\n\r", layer0ColorFormat);

         for(loopCount = 0; loopCount < 102; loopCount++)
         {
            if(loopCount >= 2)
            {
               layer0Width = (rand() % 2048) + 1;
            }
            else if(loopCount == 1)
            {
               layer0Width = 2048;
            }
            else
            {
               layer0Width = 1;
            }

            G2D_LOGI("  Pitch in pixel: %d\n\r", layer0Width);

            memset(dst_hw_image_120X120, 0xCD, 120*120*4);

            if(G2D_COLOR_FORMAT_UYVY422 == layer0ColorFormat)
            {
               uint8_t *bufferAddr[3];
               uint32_t bufferSize[3];

               bufferAddr[0] = layer0Addr;
               bufferAddr[1] = NULL;
               bufferAddr[2] = NULL;
               bufferSize[0] = LAYER0_SIZE;
               bufferSize[1] = 0;
               bufferSize[2] = 0;

               g2dOverlaySetLayerYUVBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, bufferAddr, bufferSize, layer0Width, layer0Height, (g2d_color_format_t)layer0ColorFormat);
            }
            else
               g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, layer0Width, layer0Height, (g2d_color_format_t)layer0ColorFormat);


            g2dOverlayStart(g2dHandle);
            while(hal_g2d_get_status(g2dHandle)) {};
            {
               /// C-model
               memset(dst_sw_image_120X120, 0xCD, 120*120*4);

               g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)&dst_sw_image_120X120[0]);

               g2d_c_model(&g2d_sw_register);
            }

            {
               g2d_compare_sw_hw_result();
            }
         }
      }

      hal_g2d_release_handle(g2dHandle);
   }
}



void g2d_overlay_layer_address_test(void)
{
   /*****************************************
    * To test the layer address and W2M address.
    * 1. Layer 0
    *    a. RGB565, BGR888, RGB888, YUYV422, ARGB8888, PARGB8888
    *    b. Address shoulb be pixel-aligned
    *****************************************/

   g2d_handle_t *g2dHandle;

   uint8_t *layer0Addr;
   uint32_t layer0ColorFormat;
   uint32_t layer0Width, layer0Height;
   int32_t layer0RectX, layer0RectY;
   uint32_t layer0RectW, layer0RectH;
   bool layer0SrcKeyEnable;
   uint32_t layer0SrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layer0Rotate;
   bool layer0AlphaEnable;
   uint32_t layer0AlphaValue;

   uint32_t bgColor;
   bool outputAlphaEnable;
   uint32_t outputAlphaValue;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;

   g2d_color_format_t dstColorFormat;
   int32_t dstRectX, dstRectY;
   uint32_t dstWidth, dstHeight;

   int32_t loopCount;

   G2D_LOGI("g2d_overlay_layer_address_test()\n\r");

   {
#if 1

      //layer0Addr = (uint8_t *)&rgb565_120X120[0];
      //layer0ColorFormat = G2D_COLOR_FORMAT_UYVY;
      layer0Width = 120;
      layer0Height = 120;
      layer0RectX = -100;
      layer0RectY = -100;
      layer0RectW = 100;
      layer0RectH = 75;
      layer0SrcKeyEnable = KAL_FALSE;
      layer0SrcKeyValue = 0xFFFFFFFF;
      layer0Rotate = G2D_CW_ROTATE_ANGLE_000;
      layer0AlphaEnable = KAL_FALSE;
      layer0AlphaValue = 0xFF;
      memset(layer0_image, 0x56, 120*120*4);

      bgColor = 0xF6;
      outputAlphaEnable = KAL_FALSE;
      outputAlphaValue = 0xFF;
      roiRectX = 0;
      roiRectY = 0;
      roiRectW = 100;
      roiRectH = 50;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = 100;
      dstHeight = 50;

      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      //g2dOverlaySetLayerBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, layer0Width, layer0Height, layer0ColorFormat);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);

      // layer address
      G2D_LOGI("Layer Address Test\n\r");
      for(layer0ColorFormat = 1; layer0ColorFormat <= 10; layer0ColorFormat++)
      {
         uint32_t bpp;

         if((G2D_COLOR_FORMAT_RGB565 == layer0ColorFormat) || (G2D_COLOR_FORMAT_UYVY422 == layer0ColorFormat)) // RGB565 or UYVY
         {
            bpp = 2;
         }
         else if((G2D_COLOR_FORMAT_BGR888 == layer0ColorFormat) || (G2D_COLOR_FORMAT_RGB888 ==layer0ColorFormat) ||
                 (G2D_COLOR_FORMAT_ARGB8565 == layer0ColorFormat) || (G2D_COLOR_FORMAT_ARGB6666 == layer0ColorFormat) ||
                 (G2D_COLOR_FORMAT_PARGB8565 == layer0ColorFormat) || (G2D_COLOR_FORMAT_PARGB6666 == layer0ColorFormat)) // RGB888 or BGR888
         {
            bpp = 3;
         }
         else // ARGB8888 or PARGB8888
         {
            bpp = 4;
         }

         G2D_LOGI("Color Format: %d\n\r", layer0ColorFormat);

         for(loopCount = 0; loopCount < 10; loopCount++)
         {
            layer0Addr = (uint8_t *)((uint32_t)(rgb565_120X120) + bpp * loopCount);

            G2D_LOGI("  Address: %x\n\r", layer0Addr);

            memset(dst_hw_image_120X120, 0xCD, 120*120*4);


            if(G2D_COLOR_FORMAT_UYVY422 == layer0ColorFormat)
            {
               uint8_t *bufferAddr[3];
               uint32_t bufferSize[3];

               bufferAddr[0] = layer0Addr;
               bufferAddr[1] = NULL;
               bufferAddr[2] = NULL;
               bufferSize[0] = LAYER0_SIZE;
               bufferSize[1] = 0;
               bufferSize[2] = 0;

               g2dOverlaySetLayerYUVBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, bufferAddr, bufferSize, layer0Width, layer0Height, (g2d_color_format_t)layer0ColorFormat);
            }
            else
               g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, layer0Width, layer0Height, (g2d_color_format_t)layer0ColorFormat);


            g2dOverlayStart(g2dHandle);
            while(hal_g2d_get_status(g2dHandle)) {};
            {
               /// C-model
               memset(dst_sw_image_120X120, 0xCD, 120*120*4);

               g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)&dst_sw_image_120X120[0]);

               g2d_c_model(&g2d_sw_register);
            }

            {
               g2d_compare_sw_hw_result();
            }
         }
      }

      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&rgb565_120X120[0], 120 * 120 * 2, layer0Width, layer0Height, G2D_COLOR_FORMAT_RGB565);

      // W2M address
      G2D_LOGI("W2M Address Test\n\r");
      for(layer0ColorFormat = 1; layer0ColorFormat <= 10; layer0ColorFormat++)
      {
         uint32_t bpp;

         if((G2D_COLOR_FORMAT_RGB565 == layer0ColorFormat) || (G2D_COLOR_FORMAT_UYVY422 == layer0ColorFormat)) // RGB565 or UYVY
         {
            bpp = 2;
         }
         else if((G2D_COLOR_FORMAT_BGR888 == layer0ColorFormat) || (G2D_COLOR_FORMAT_RGB888 ==layer0ColorFormat) ||
                 (G2D_COLOR_FORMAT_ARGB8565 == layer0ColorFormat) || (G2D_COLOR_FORMAT_ARGB6666 == layer0ColorFormat) ||
                 (G2D_COLOR_FORMAT_PARGB8565 == layer0ColorFormat) || (G2D_COLOR_FORMAT_PARGB6666 == layer0ColorFormat)) // RGB888 or BGR888
         {
            bpp = 3;
         }
         else // ARGB8888 or PARGB8888
         {
            bpp = 4;
         }

         G2D_LOGI("Color Format: %d\n\r", layer0ColorFormat);

         for(loopCount = 0; loopCount < 10; loopCount++)
         {
            layer0Addr = (uint8_t *)((uint32_t)(dst_hw_image_120X120) + bpp * loopCount);

            G2D_LOGI("  Address: %x\n\r", layer0Addr);

            memset(dst_hw_image_120X120, 0xCD, 120*120*4);

            if(G2D_COLOR_FORMAT_UYVY422 == layer0ColorFormat)
            {
               hal_set_destination_rgb_buffer_information(g2dHandle, layer0Addr, DST_HW_BUFFER_SIZE, dstWidth, dstHeight, G2D_COLOR_FORMAT_RGB565);
            }
            else
               hal_set_destination_rgb_buffer_information(g2dHandle, layer0Addr, DST_HW_BUFFER_SIZE, dstWidth, dstHeight, (g2d_color_format_t)layer0ColorFormat);


            g2dOverlayStart(g2dHandle);
            while(hal_g2d_get_status(g2dHandle)) {};
            {
               /// C-model
               memset(dst_sw_image_120X120, 0xCD, 120*120*4);

               g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)((uint32_t)(dst_sw_image_120X120) + bpp * loopCount));

               g2d_c_model(&g2d_sw_register);
            }

            {
               g2d_compare_sw_hw_result();
            }
         }
      }
#endif
      //Add W2M Address Test by Michael 20120319
      layer0Width = 120;
      layer0Height = 120;
      layer0RectX = -100;
      layer0RectY = -100;
      layer0RectW = 100;
      layer0RectH = 75;
      layer0SrcKeyEnable = KAL_FALSE;
      layer0SrcKeyValue = 0xFFFFFFFF;
      layer0Rotate = G2D_CW_ROTATE_ANGLE_000;
      layer0AlphaEnable = KAL_FALSE;
      layer0AlphaValue = 0xFF;
      memset(layer0_image, 0x56, 120*120*4);


      bgColor = 0xF6;
      outputAlphaEnable = KAL_FALSE;
      outputAlphaValue = 0xFF;
      roiRectX = 0;
      roiRectY = 0;
      roiRectW = 1;
      roiRectH = 1;

      dstColorFormat = G2D_COLOR_FORMAT_RGB888;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = 100;
      dstHeight = 50;

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      //g2dOverlaySetLayerBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, layer0Width, layer0Height, layer0ColorFormat);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);


      layer0ColorFormat = G2D_COLOR_FORMAT_RGB888;
      //g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, layer0Width, layer0Height, (g2d_color_format_t)layer0ColorFormat);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], LAYER0_SIZE, layer0Width, layer0Height, (g2d_color_format_t)layer0ColorFormat);
      G2D_LOGI("W2M Address Test New Added By Michael\n\r");

      for(loopCount = 0; loopCount < 128; loopCount++)
      {
           layer0Addr = (uint8_t *)((uint32_t)(dst_hw_image_120X120) + loopCount);

           G2D_LOGI("  Address: %x\n\r", layer0Addr);

           memset(dst_hw_image_120X120, 0xCD, 120*120*4);

           memset(layer0_image, (0x56+loopCount), 120*120*4);

           hal_set_destination_rgb_buffer_information(g2dHandle, layer0Addr, DST_HW_BUFFER_SIZE, dstWidth, dstHeight, G2D_COLOR_FORMAT_RGB888);
 
           g2dOverlayStart(g2dHandle);
           while(hal_g2d_get_status(g2dHandle)) {};
           {
              /// C-model
              memset(dst_sw_image_120X120, 0xCD, 120*120*4);

              //g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)((uint32_t)(dst_sw_image_120X120) * loopCount));
              g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)((uint32_t)(dst_sw_image_120X120) + loopCount));

              g2d_c_model(&g2d_sw_register);
           }

           {
              g2d_compare_sw_hw_result();
           }
      }

      hal_g2d_release_handle(g2dHandle);
   }
}


void g2d_overlay_layer_rectangle_fill_test(void)
{
   /*****************************************
    * To test the layer rectangle fill.
    * 1. Layer 0
    *    a. Enable alpha blending
    *    b. ARGB8888, PARGB8888
    *****************************************/

   g2d_handle_t *g2dHandle;

   uint32_t layer0ColorFormat;
   uint32_t layer0Width, layer0Height;
   int32_t layer0RectX, layer0RectY;
   uint32_t layer0RectW, layer0RectH;
   bool layer0SrcKeyEnable;
   uint32_t layer0SrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layer0Rotate;
   bool layer0AlphaEnable;
   uint32_t layer0AlphaValue;

   uint32_t bgColor;
   bool outputAlphaEnable;
   uint32_t outputAlphaValue;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;

   uint8_t *dstBuf;
   g2d_color_format_t dstColorFormat;
   int32_t dstRectX, dstRectY;
   uint32_t dstWidth, dstHeight;

   {
      ///layer0Addr = (uint8_t *)&layer0_image[0];
      //layer0ColorFormat = G2D_COLOR_FORMAT_PARGB8888;
      layer0Width = 100;
      layer0Height = 110;
      layer0RectX = 0;
      layer0RectY = 0;
      layer0RectW = 100;
      layer0RectH = 110;
      layer0SrcKeyEnable = KAL_FALSE;
      layer0SrcKeyValue = 0xFFFFFFFF;
      layer0Rotate = G2D_CW_ROTATE_ANGLE_000;
      layer0AlphaEnable = KAL_TRUE;
      layer0AlphaValue = 0x56;
      memset(layer0_image, 0x56, 100*110*4);

      bgColor = 0x46;
      outputAlphaEnable = KAL_FALSE;
      outputAlphaValue = 0xFF;
      roiRectX = 10;
      roiRectY = 10;
      roiRectW = 110;
      roiRectH = 110;

      dstBuf = (uint8_t *)&dst_hw_image_120X120[0];
      dstColorFormat = G2D_COLOR_FORMAT_PARGB8565;
      dstRectX = 10;
      dstRectY = 10;
      dstWidth = 110;
      dstHeight = 110;

       /// G2D_STATUS_BUSY means someone is using G2D
       if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
       {
          G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
          return;
       }

       G2D_LOGI("g2d_overlay_layer_rectangle_fill_test()\n\r");
       {
         memset(dst_hw_image_120X120, 0xCD, 120*120*4);

         hal_g2d_set_callback_function(g2dHandle, NULL);
         g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
         hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)dstBuf, DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
         hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
         hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

         g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
         g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
         g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

         g2dOverlaySetBgColor(g2dHandle, bgColor);
         g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
         g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

         /// Layer 0
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
         g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_RECT);
         //g2dOverlaySetLayerBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, NULL, layer0Width, layer0Height, layer0ColorFormat);
         g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);
         g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
         g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
         g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);
         g2dOverlaySetLayerRectFillColor(g2dHandle, G2D_OVERLAY_LAYER0, 0x12345678);

         for(layer0ColorFormat = 1; layer0ColorFormat <= 10; layer0ColorFormat++)
         {
            if(G2D_COLOR_FORMAT_UYVY422 == layer0ColorFormat)
               continue;
            G2D_LOGI("Color Format: %d\n\r", layer0ColorFormat);

            g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, NULL, 0, layer0Width, layer0Height, (g2d_color_format_t)layer0ColorFormat);

            g2dOverlayStart(g2dHandle);
            while(hal_g2d_get_status(g2dHandle)) {};
            {
               /// C-model
               memset(dst_sw_image_120X120, 0xCD, 120*120*4);
               dstBuf = (uint8_t *)&dst_sw_image_120X120[0];

               g2d_set_registers_to_c_model(&g2d_sw_register, dstBuf);

               g2d_c_model(&g2d_sw_register);
            }

            {
               g2d_compare_sw_hw_result();
            }
         }
      }
      hal_g2d_release_handle(g2dHandle);
   }
}



void g2d_overlay_layer_normal_font_test(void)
{
   /*****************************************
    * To test the normal font.
    * 1. Layer 0
    *    a. ARGB8888
    *    b. Enable Font
    *    c. Test 1 bit index color
    *****************************************/

   g2d_handle_t *g2dHandle;

   uint8_t *layer0Addr;
   int32_t layer0RectX, layer0RectY;
   uint32_t layer0RectW, layer0RectH;
   bool layer0SrcKeyEnable;
   uint32_t layer0SrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layer0Rotate;
   bool layer0AlphaEnable;
   uint32_t layer0AlphaValue;

   uint32_t bgColor;
   bool outputAlphaEnable;
   uint32_t outputAlphaValue;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;

   uint8_t *dstBuf;
   g2d_color_format_t dstColorFormat;
   int32_t dstRectX, dstRectY;
   uint32_t dstWidth, dstHeight;

   G2D_LOGI("g2d_overlay_normal_font_test()\n\r");

   {
      layer0Addr = (uint8_t *)&layer0_image[0];
      //layer0Width = 100;
      //layer0Height = 110;
      layer0RectX = 0;
      layer0RectY = 0;
      layer0RectW = 100;
      layer0RectH = 110;
      layer0SrcKeyEnable = KAL_FALSE;
      layer0SrcKeyValue = 0xFFFFFFFF;
      layer0Rotate = G2D_CW_ROTATE_ANGLE_000;
      layer0AlphaEnable = KAL_FALSE;
      layer0AlphaValue = 0x56;
      memset(layer0_image, 0xAA, 100*110*4);

      bgColor = 0x46;
      outputAlphaEnable = KAL_FALSE;
      outputAlphaValue = 0xFF;
      roiRectX = 10;
      roiRectY = 10;
      roiRectW = 110;
      roiRectH = 110;

      dstBuf = (uint8_t *)&dst_hw_image_120X120[0];
      dstColorFormat = G2D_COLOR_FORMAT_PARGB8888;
      dstRectX = 10;
      dstRectY = 10;
      dstWidth = 110;
      dstHeight = 110;

      {
        uint32_t i;
        uint32_t *ptr = (uint32_t *)&layer0_image[0];

        for(i = 0; i < 120 * 120; i++)
        {
           *ptr = rand() & 0xFFFFFFFF;
           ptr++;
        }
      }

      {
         memset(dst_hw_image_120X120, 0xCD, 120*120*4);

         /// G2D_STATUS_BUSY means someone is using G2D
         if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
            return;

         hal_g2d_set_callback_function(g2dHandle, NULL);
         g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
         hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)dstBuf, DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
         hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
         hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

         g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
         g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
         g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

         g2dOverlaySetBgColor(g2dHandle, bgColor);
         g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
         g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

         /// Layer 0
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
         g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_NORMAL_FONT);
         g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);
         g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
         g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
         g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);

         {

            g2dOverlaySetLayerBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr , LAYER0_SIZE);

            g2dOverlayStart(g2dHandle);
            while(hal_g2d_get_status(g2dHandle)) {};
            {
               /// C-model
               memset(dst_sw_image_120X120, 0xCD, 120*120*4);
               dstBuf = (uint8_t *)&dst_sw_image_120X120[0];

               g2d_set_registers_to_c_model(&g2d_sw_register, dstBuf);

               g2d_c_model(&g2d_sw_register);
            }

            {
               g2d_compare_sw_hw_result();
            }
         }

         hal_g2d_release_handle(g2dHandle);
      }
   }
}



void g2d_overlay_layer_aa_font_test(void)
{
   g2d_handle_t *g2dHandle;

   uint8_t *layer0Addr;
   int32_t layer0RectX, layer0RectY;
   uint32_t layer0RectW, layer0RectH;
   bool layer0SrcKeyEnable;
   uint32_t layer0SrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layer0Rotate;
   bool layer0AlphaEnable;
   uint32_t layer0AlphaValue;

   uint32_t bgColor;
   bool outputAlphaEnable;
   uint32_t outputAlphaValue;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;

   uint8_t *dstBuf;
   g2d_color_format_t dstColorFormat;
   int32_t dstRectX, dstRectY;
   uint32_t dstWidth, dstHeight;

   G2D_LOGI("g2d_overlay_aa_font_test()\n\r");

   {
      layer0Addr = (uint8_t *)&layer0_image[0];
      //layer0ColorFormat = G2D_COLOR_FORMAT_PARGB8888;
      //layer0Width = 100;
      //layer0Height = 110;
      layer0RectX = 0;
      layer0RectY = 0;
      layer0RectW = 100;
      layer0RectH = 110;
      layer0SrcKeyEnable = KAL_FALSE;
      layer0SrcKeyValue = 0xFFFF0000;
      layer0Rotate = G2D_CW_ROTATE_ANGLE_000;
      layer0AlphaEnable = KAL_TRUE;
      layer0AlphaValue = 0x56;
      //layer0_font_weight_value = G2D_FONT_INDEX_COLOR_FORMAT_4_BIT;
      memset(layer0_image, 0xAA, 100*110*4);

      bgColor = 0x46;
      outputAlphaEnable = KAL_FALSE;
      outputAlphaValue = 0xFF;
      roiRectX = 10;
      roiRectY = 10;
      roiRectW = 110;
      roiRectH = 110;

      dstBuf = (uint8_t *)&dst_hw_image_120X120[0];
      dstColorFormat = G2D_COLOR_FORMAT_PARGB8888;
      dstRectX = 10;
      dstRectY = 10;
      dstWidth = 110;
      dstHeight = 110;

      {
        uint32_t i;
        uint32_t *ptr = (uint32_t *)&layer0_image[0];

        for(i = 0; i < 120 * 120; i++)
        {
           *ptr = rand() & 0xFFFFFFFF;
           ptr++;
        }
      }
      {
         memset(dst_hw_image_120X120, 0xCD, 120*120*4);

         /// G2D_STATUS_BUSY means someone is using G2D
         if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
            return;

         hal_g2d_set_callback_function(g2dHandle, NULL);
         g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
         hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)dstBuf, DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
         hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
         hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

         g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
         g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
         g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

         g2dOverlaySetBgColor(g2dHandle, bgColor);
         g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
         g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

         /// Layer 0
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
         g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_AA_FONT);
         //g2dOverlaySetLayerBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, NULL, layer0Width, layer0Height, layer0ColorFormat);
         g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);
         g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
         g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
         g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);
         //g2dOverlaySetLayerFontBit(g2dHandle, G2D_OVERLAY_LAYER0, layer0_font_weight_value);

         {

            g2dOverlaySetLayerBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr , LAYER0_SIZE);

            G2D_LOGI("  Weight: 2\n\r");
            {
               g2dOverlaySetLayerFontBit(g2dHandle, G2D_OVERLAY_LAYER0, G2D_FONT_BIT_2_BIT);

               g2dOverlayStart(g2dHandle);
               while(hal_g2d_get_status(g2dHandle)) {};
               {
                  /// C-model
                  memset(dst_sw_image_120X120, 0xCD, 120*120*4);
                  dstBuf = (uint8_t *)&dst_sw_image_120X120[0];

                  g2d_set_registers_to_c_model(&g2d_sw_register, dstBuf);

                  g2d_c_model(&g2d_sw_register);
               }

               {
                  g2d_compare_sw_hw_result();
               }
            }

            G2D_LOGI("  Weight: 4\n\r");
            {
               g2dOverlaySetLayerFontBit(g2dHandle, G2D_OVERLAY_LAYER0, G2D_FONT_BIT_4_BIT);

               g2dOverlayStart(g2dHandle);
               while(hal_g2d_get_status(g2dHandle)) {};
               {
                  /// C-model
                  memset(dst_sw_image_120X120, 0xCD, 120*120*4);
                  dstBuf = (uint8_t *)&dst_sw_image_120X120[0];

                  g2d_set_registers_to_c_model(&g2d_sw_register, dstBuf);

                  g2d_c_model(&g2d_sw_register);
               }

               {
                  g2d_compare_sw_hw_result();
               }
            }
            G2D_LOGI("  Weight: 8\n\r");
            {
               g2dOverlaySetLayerFontBit(g2dHandle, G2D_OVERLAY_LAYER0, G2D_FONT_BIT_8_BIT);

               g2dOverlayStart(g2dHandle);
               while(hal_g2d_get_status(g2dHandle)) {};
               {
                  /// C-model
                  memset(dst_sw_image_120X120, 0xCD, 120*120*4);
                  dstBuf = (uint8_t *)&dst_sw_image_120X120[0];

                  g2d_set_registers_to_c_model(&g2d_sw_register, dstBuf);

                  g2d_c_model(&g2d_sw_register);
               }

               {
                  g2d_compare_sw_hw_result();
               }
            }
         }

         hal_g2d_release_handle(g2dHandle);
      }
   }
}



void g2d_overlay_roi_clipping_window_test(void)
{
   /*****************************************
    * To test the ROI clipping window.
    * 1. Layer 0: RGB565
    * 2. ROI
    *    a. Enable and disable clipping window
    *    b. Min X from -2048 to 2047, Y from -2048 to 2047
    *    c. Max X from -2048 to 2047, Y from -2048 to 2047
    *    d. Should test the boundary
    *****************************************/

   g2d_handle_t *g2dHandle;

   uint8_t *layer0Addr;
   g2d_color_format_t layer0ColorFormat;
   uint32_t layer0Width, layer0Height;
   int32_t layer0RectX, layer0RectY;
   uint32_t layer0RectW, layer0RectH;
   bool layer0SrcKeyEnable;
   uint32_t layer0SrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layer0Rotate;
   bool layer0AlphaEnable;
   uint32_t layer0AlphaValue;

   uint32_t bgColor;
   bool outputAlphaEnable;
   uint32_t outputAlphaValue;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;

   g2d_color_format_t dstColorFormat;
   int32_t dstRectX, dstRectY;
   uint32_t dstWidth, dstHeight;

   int32_t clipX, clipY;
   int32_t clipW, clipH;

   int32_t loopCount;

   G2D_LOGI("g2d_overlay_roi_clipping_window_test()\n\r");

   {
      layer0Addr = (uint8_t *)&layer0_image[0];
      layer0ColorFormat = G2D_COLOR_FORMAT_RGB565;
      layer0Width = 1;
      layer0Height = 1;
      //layer0RectX = 0;
      //layer0RectY = 0;
      layer0RectW = 1;
      layer0RectH = 1;
      layer0SrcKeyEnable = KAL_FALSE;
      layer0SrcKeyValue = 0xFFFFFFFF;
      layer0Rotate = G2D_CW_ROTATE_ANGLE_000;
      layer0AlphaEnable = KAL_FALSE;
      layer0AlphaValue = 0xFF;
      ///memset(layer0_image, 0x56, 120*120*4);

      bgColor = 0xF6;
      outputAlphaEnable = KAL_FALSE;
      outputAlphaValue = 0xFF;
      roiRectX = -5;
      roiRectY = -10;
      roiRectW = 54;
      roiRectH = 87;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = 48;
      dstHeight = 48;

      {
        uint32_t i;
        uint32_t *ptr = (uint32_t *)&layer0_image[0];

        for(i = 0; i < 120 * 120; i++)
        {
           *ptr = rand() & 0xFFFFFFFF;
           ptr++;
        }
      }

      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      //hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, layer0Width, layer0Height, (g2d_color_format_t)layer0ColorFormat);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);

      for(loopCount = 0; loopCount < 110; loopCount++)
      {
      	G2D_LOGI("loopCount =%d\n\r", loopCount);
         if(loopCount >= 60)
         {
            layer0RectX = (rand() % 4096) - 2048;
            layer0RectY = (rand() % 4096) - 2048;

            layer0RectW = 5;
            layer0RectH = 2048;

            clipX = layer0RectX - 10;
            if(clipX > 2048 - 25)
               clipX = 2048 - 25;
            else if(clipX < -2048)
               clipX = -2048;

            clipY = layer0RectY - 10;
            if(clipY > 2048 - 25)
               clipY = 2048 - 25;
            else if(clipY < -2048)
               clipY = -2048;

            clipW = 25;
            clipH = 25;

            g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, 5, 2048, (g2d_color_format_t)layer0ColorFormat);
            g2dOverlaySetRoiWindow(g2dHandle, -1024, -2, 2048, 4);
            hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, 2048, 4, dstColorFormat);
         }
         else if(loopCount >= 10)
         {
            layer0RectX = (rand() % 4096) - 2048;
            layer0RectY = (rand() % 4096) - 2048;

            layer0RectW = 2048;
            layer0RectH = 5;

            clipX = layer0RectX - 10;
            if(clipX > 2048 - 25)
               clipX = 2048 - 25;
            else if(clipX < -2048)
               clipX = -2048;

            clipY = layer0RectY - 10;
            if(clipY > 2048 - 25)
               clipY = 2048 - 25;
            else if(clipY < -2048)
               clipY = -2048;

            clipW = 25;
            clipH = 25;

            g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, 2048, 5, (g2d_color_format_t)layer0ColorFormat);
            g2dOverlaySetRoiWindow(g2dHandle, -2, -1024, 4, 2048);
            hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, 4, 2048, dstColorFormat);
         }
         else if(loopCount >= 9)
         {
            layer0RectX = -5;
            layer0RectY = -1000;

            layer0RectW = 2048;
            layer0RectH = 5;

            clipX = layer0RectX - 10;
            if(clipX > 2048 - 25)
               clipX = 2048 - 25;
            else if(clipX < -2048)
               clipX = -2048;

            clipY = layer0RectY - 10;
            if(clipY > 2048 - 25)
               clipY = 2048 - 25;
            else if(clipY < -2048)
               clipY = -2048;

            clipW = 25;
            clipH = 25;

            g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, LAYER0_SIZE, 2048, 5, (g2d_color_format_t)layer0ColorFormat);
            g2dOverlaySetRoiWindow(g2dHandle, -2, -1024, 4, 2048);
            hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, 4, 2048, dstColorFormat);
         }
         else if(loopCount == 8)
         {
            layer0RectX = 2047;
            layer0RectY = 2047;

            clipX = 2024;
            clipY = 2024;
            clipW = 24;
            clipH = 24;

            g2dOverlaySetRoiWindow(g2dHandle, 2000, 2000, 48, 48);
         }
         else if(loopCount == 7)
         {
            layer0RectX = 2047;
            layer0RectY = 0;

            clipX = 2024;
            clipY = -12;
            clipW = 24;
            clipH = 24;

            g2dOverlaySetRoiWindow(g2dHandle, 2000, -24, 48, 48);
         }
         else if(loopCount == 6)
         {
            layer0RectX = 2047;
            layer0RectY = -2048;

            clipX = 2024;
            clipY = -2048;
            clipW = 24;
            clipH = 24;

            g2dOverlaySetRoiWindow(g2dHandle, 2000, -2048, 48, 48);
         }
         else if(loopCount == 5)
         {
            layer0RectX = 0;
            layer0RectY = 2047;

            clipX = -12;
            clipY = 2024;
            clipW = 24;
            clipH = 24;

            g2dOverlaySetRoiWindow(g2dHandle, -24, 2000, 48, 48);
         }
         else if(loopCount == 4)
         {
            layer0RectX = 0;
            layer0RectY = 0;

            clipX = -12;
            clipY = -12;
            clipW = 24;
            clipH = 24;

            g2dOverlaySetRoiWindow(g2dHandle, -24, -24, 48, 48);
         }
         else if(loopCount == 3)
         {
            layer0RectX = 0;
            layer0RectY = -2048;

            clipX = -12;
            clipY = -2048;
            clipW = 24;
            clipH = 24;

            g2dOverlaySetRoiWindow(g2dHandle, -24, -2048, 48, 48);
         }
         else if(loopCount == 2)
         {
            layer0RectX = -2048;
            layer0RectY = 2047;

            clipX = -2048;
            clipY = 2024;
            clipW = 24;
            clipH = 24;

            g2dOverlaySetRoiWindow(g2dHandle, -2048, 2000, 48, 48);
         }
         else if(loopCount == 1)
         {
            layer0RectX = -2048;
            layer0RectY = 0;

            clipX = -2048;
            clipY = -12;
            clipW = 24;
            clipH = 24;

            g2dOverlaySetRoiWindow(g2dHandle, -2048, -24, 48, 48);
         }
         else
         {
            layer0RectX = -2048;
            layer0RectY = -2048;

            clipX = -1024;//-2048; Dream: Range issue
            clipY = -1024;//-2048; Dream: Range issue
            clipW = 24;
            clipH = 24;

            g2dOverlaySetRoiWindow(g2dHandle, -2048, -2048, 48, 48);
         }

         G2D_LOGI("  X: %d, Y: %d, W: %d, H: %d\n\r", clipX, clipY, clipW, clipH);

         memset(dst_hw_image_120X120, 0xCD, 120*120*4);

         g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);

         G2D_LOGI("    Enabled\n\r");
         {
            hal_g2d_set_destination_clip_window(g2dHandle, KAL_TRUE, clipX, clipY, clipW, clipH);
            g2dOverlayStart(g2dHandle);
            while(hal_g2d_get_status(g2dHandle)) {};
            {
               /// C-model
               memset(dst_sw_image_120X120, 0xCD, 120*120*4);

               g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)&dst_sw_image_120X120[0]);

               g2d_c_model(&g2d_sw_register);
            }

            {
               g2d_compare_sw_hw_result();
            }
         }
         G2D_LOGI("    Disabled\n\r");
         {
            hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, clipX, clipY, clipW, clipH);
            g2dOverlayStart(g2dHandle);
            while(hal_g2d_get_status(g2dHandle)) {};
            {
               /// C-model
               memset(dst_sw_image_120X120, 0xCD, 120*120*4);

               g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)&dst_sw_image_120X120[0]);

               g2d_c_model(&g2d_sw_register);
            }

            {
               g2d_compare_sw_hw_result();
            }
         }
      }

      hal_g2d_release_handle(g2dHandle);
   }
}



void g2d_overlay_roi_memory_out_color_format_test(void)
{
   /*****************************************
    * To test the ROI memory out color format.
    * 1. Layer 0: ARGB8888, enable background color
    * 2. ROI
    *    a. RGB565, BGR888, RGB888, ARGB8888, PARGB8888
    *****************************************/

   g2d_handle_t *g2dHandle;

   uint8_t *layer0Addr;
   g2d_color_format_t layer0ColorFormat;
   uint32_t layer0Width, layer0Height;
   int32_t layer0RectX, layer0RectY;
   uint32_t layer0RectW, layer0RectH;
   bool layer0SrcKeyEnable;
   uint32_t layer0SrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layer0Rotate;
   bool layer0AlphaEnable;
   uint32_t layer0AlphaValue;

   uint32_t bgColor;
   bool outputAlphaEnable;
   uint32_t outputAlphaValue;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;

   uint32_t dstColorFormat;
   int32_t dstRectX, dstRectY;
   uint32_t dstWidth, dstHeight;

   G2D_LOGI("g2d_overlay_roi_memory_out_color_format_test()\n\r");
   {
      layer0Addr = (uint8_t *)&rgb565_120X120[0];
      layer0ColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layer0Width = 60;
      layer0Height = 120;
      layer0RectX = 0;
      layer0RectY = 0;
      layer0RectW = 60;
      layer0RectH = 120;
      layer0SrcKeyEnable = KAL_TRUE;
      layer0SrcKeyValue = 0xFFFFFFFF;
      layer0Rotate = G2D_CW_ROTATE_ANGLE_000;
      layer0AlphaEnable = KAL_FALSE;
      layer0AlphaValue = 0xFF;
      ///memset(layer0_image, 0x56, 320*240*4);

      bgColor = 0xFF0;
      outputAlphaEnable = KAL_FALSE;
      outputAlphaValue = 0xFF;
      roiRectX = 0;
      roiRectY = 0;
      roiRectW = 60;
      roiRectH = 120;

      //dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = 60;
      dstHeight = 120;

      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      //hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)dstBuf, DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_TRUE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, 120 * 120 * 2, layer0Width, layer0Height, layer0ColorFormat);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);

      for(dstColorFormat = 1; dstColorFormat <= 10; dstColorFormat++)
      {
         if(G2D_COLOR_FORMAT_UYVY422 == dstColorFormat)
         {
            continue;
         }

         G2D_LOGI("Color Format: %d\n\r", dstColorFormat);

         hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)dst_hw_image_120X120, DST_HW_BUFFER_SIZE, dstWidth, dstHeight, (g2d_color_format_t)dstColorFormat);

         memset(dst_hw_image_120X120, 0xCD, 120*120*4);
         g2dOverlayStart(g2dHandle);
         while(hal_g2d_get_status(g2dHandle)) {};
         {
            /// C-model
            memset(dst_sw_image_120X120, 0xCD, 120*120*4);

            g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)&dst_sw_image_120X120[0]);

            g2d_c_model(&g2d_sw_register);
         }

         {
            g2d_compare_sw_hw_result();
         }
      }

      hal_g2d_release_handle(g2dHandle);
   }
}



void g2d_overlay_roi_color_replacement_test(void)
{
   /*****************************************
    * To test the color replacement mechanism.
    * 1. Layer 0
    *    a. Enable and disable color replacement
    *    b. RGB565, BRG888, RGB888, ARGB8888, PARGB8888
    *****************************************/

   g2d_handle_t *g2dHandle;

   uint8_t *layer0Addr;
   uint32_t layer0ColorFormat;
   uint32_t layer0Width, layer0Height;
   int32_t layer0RectX, layer0RectY;
   uint32_t layer0RectW, layer0RectH;
   bool layer0SrcKeyEnable;
   uint32_t layer0SrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layer0Rotate;
   bool layer0AlphaEnable;
   uint32_t layer0AlphaValue;

   uint32_t bgColor;
   bool outputAlphaEnable;
   uint32_t outputAlphaValue;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;

   g2d_color_format_t dstColorFormat;
   int32_t dstRectX, dstRectY;
   uint32_t dstWidth, dstHeight;

   bool colorReplacementEnable;
   uint8_t a0, r0, g0, b0, a1, r1, g1, b1;

   G2D_LOGI("g2d_overlay_roi_color_replacement_test()\n\r");

   {
      layer0Addr = (uint8_t *)&rgb565_120X120[0];
      //layer0ColorFormat = G2D_COLOR_FORMAT_RGB565;
      layer0Width = 60;
      layer0Height = 120;
      layer0RectX = 0;
      layer0RectY = 0;
      layer0RectW = 60;
      layer0RectH = 120;
      layer0SrcKeyEnable = KAL_TRUE;
      layer0SrcKeyValue = 0xFFFFFFFF;
      layer0Rotate = G2D_CW_ROTATE_ANGLE_000;
      layer0AlphaEnable = KAL_FALSE;
      layer0AlphaValue = 0xFF;
      memset(layer0_image, 0x56, 120*120*4);

      bgColor = 0xF6;
      outputAlphaEnable = KAL_FALSE;
      outputAlphaValue = 0xFF;
      roiRectX = 0;
      roiRectY = 0;
      roiRectW = 60;
      roiRectH = 120;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = 60;
      dstHeight = 120;

      //colorReplacementEnable = KAL_TRUE;
      a0 = 0x00;
      r0 = 0x00;
      g0 = 0x00;
      b0 = 0xF6;

      a1 = 0x00;
      r1 = 0xFF;
      g1 = 0xFF;
      b1 = 0x00;

      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)dst_hw_image_120X120, DST_HW_BUFFER_SIZE, dstWidth, dstHeight, (g2d_color_format_t)dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      //hal_g2d_set_color_replacement(g2dHandle, colorReplacementEnable, a0, r0, g0, b0, a1, r1, g1, b1);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      //g2dOverlaySetLayerBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, layer0Width, layer0Height, layer0ColorFormat);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);

      for(layer0ColorFormat = 1; layer0ColorFormat <= 10; layer0ColorFormat++)
      {
         if(G2D_COLOR_FORMAT_UYVY422 == layer0ColorFormat)
         {
            continue;
         }

         G2D_LOGI("Color Format: %d\n\r", layer0ColorFormat);
         g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, 120 * 120 * 2, layer0Width, layer0Height, (g2d_color_format_t)layer0ColorFormat);

         G2D_LOGI("  Enabled\n\r");
         {
            colorReplacementEnable = KAL_TRUE;
            hal_g2d_set_color_replacement(g2dHandle, colorReplacementEnable, a0, r0, g0, b0, a1, r1, g1, b1);

            memset(dst_hw_image_120X120, 0xCD, 120*120*4);

            g2dOverlayStart(g2dHandle);
            while(hal_g2d_get_status(g2dHandle)) {};
            {
               /// C-model
               memset(dst_sw_image_120X120, 0xCD, 120*120*4);

               g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)&dst_sw_image_120X120[0]);

               g2d_c_model(&g2d_sw_register);
            }
            {
               g2d_compare_sw_hw_result();
            }
         }

         G2D_LOGI("  Disabled\n\r");
         {
            colorReplacementEnable = KAL_FALSE;
            hal_g2d_set_color_replacement(g2dHandle, colorReplacementEnable, a0, r0, g0, b0, a1, r1, g1, b1);

            memset(dst_hw_image_120X120, 0xCD, 120*120*4);

            g2dOverlayStart(g2dHandle);
            while(hal_g2d_get_status(g2dHandle)) {};
            {
               /// C-model
               memset(dst_sw_image_120X120, 0xCD, 120*120*4);

               g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)&dst_sw_image_120X120[0]);

               g2d_c_model(&g2d_sw_register);
            }
            {
               g2d_compare_sw_hw_result();
            }
         }
      }

      hal_g2d_release_handle(g2dHandle);
   }
}




void g2d_overlay_roi_backgroud_color_test(void)
{
   /*****************************************
    * To test the background color.
    * 1. Layer 0 enable source key
    *    a. Enable and disable bg color
    *    b. RGB565, BRG888, RGB888, ARGB8888, PARGB8888
    * 2. Layer 0
    *    a. Enable font 1 bit
    *    b. Enable and disable bg color
    *****************************************/

   g2d_handle_t *g2dHandle;

   uint8_t *layer0Addr;
   uint32_t layer0ColorFormat;
   uint32_t layer0Width, layer0Height;
   int32_t layer0RectX, layer0RectY;
   uint32_t layer0RectW, layer0RectH;
   bool layer0SrcKeyEnable;
   uint32_t layer0SrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layer0Rotate;
   bool layer0AlphaEnable;
   uint32_t layer0AlphaValue;

   bool outputAlphaEnable;
   uint32_t outputAlphaValue;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;

   g2d_color_format_t dstColorFormat;
   int32_t dstRectX, dstRectY;
   uint32_t dstWidth, dstHeight;

   G2D_LOGI("g2d_overlay_roi_backgroud_color_test()\n\r");

   {
      layer0Addr = (uint8_t *)&rgb565_120X120[0];
      //layer0ColorFormat = G2D_COLOR_FORMAT_RGB565;
      layer0Width = 60;
      layer0Height = 120;
      layer0RectX = 0;
      layer0RectY = 0;
      layer0RectW = 60;
      layer0RectH = 120;
      layer0SrcKeyEnable = KAL_TRUE;
      layer0SrcKeyValue = 0xFFFFFFFF;
      layer0Rotate = G2D_CW_ROTATE_ANGLE_000;
      layer0AlphaEnable = KAL_FALSE;
      layer0AlphaValue = 0xFF;
      memset(layer0_image, 0x56, 120*120*4);

      outputAlphaEnable = KAL_FALSE;
      outputAlphaValue = 0xFF;
      roiRectX = 0;
      roiRectY = 0;
      roiRectW = 50;
      roiRectH = 120;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = 60;
      dstHeight = 120;

      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)dst_hw_image_120X120, DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      //g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      //g2dOverlaySetLayerBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, layer0Width, layer0Height, layer0ColorFormat);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);

      for(layer0ColorFormat = 1; layer0ColorFormat <= 10; layer0ColorFormat++)
      {
         if(G2D_COLOR_FORMAT_UYVY422 == layer0ColorFormat)
         {
            continue;
         }

         G2D_LOGI("Color Format: %d\n\r", layer0ColorFormat);
         g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, 120 * 120 * 2, layer0Width, layer0Height, (g2d_color_format_t)layer0ColorFormat);

         G2D_LOGI("  Enabled\n\r");
         {
            g2dOverlayDisableBg(g2dHandle, KAL_FALSE);

            memset(dst_hw_image_120X120, 0xCD, 120*120*4);

            g2dOverlayStart(g2dHandle);
            while(hal_g2d_get_status(g2dHandle)) {};
            {
               /// C-model
               memset(dst_sw_image_120X120, 0xCD, 120*120*4);

               g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)&dst_sw_image_120X120[0]);

               g2d_c_model(&g2d_sw_register);
            }
            {
               g2d_compare_sw_hw_result();
            }
         }

         G2D_LOGI("  Disabled\n\r");
         {
            g2dOverlayDisableBg(g2dHandle, KAL_TRUE);

            memset(dst_hw_image_120X120, 0xCD, 120*120*4);

            g2dOverlayStart(g2dHandle);
            while(hal_g2d_get_status(g2dHandle)) {};
            {
               /// C-model
               memset(dst_sw_image_120X120, 0xCD, 120*120*4);

               g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)&dst_sw_image_120X120[0]);

               g2d_c_model(&g2d_sw_register);
            }
            {
               g2d_compare_sw_hw_result();
            }
         }
      }


      G2D_LOGI("Font Test\n\r");
      {
         g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, 120 * 120 * 2, layer0Width, layer0Height, G2D_COLOR_FORMAT_PARGB8888);
         g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_NORMAL_FONT);
         g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, KAL_FALSE, layer0SrcKeyValue);

         G2D_LOGI("  Enabled\n\r");
         {
            g2dOverlayDisableBg(g2dHandle, KAL_FALSE);

            memset(dst_hw_image_120X120, 0xCD, 120*120*4);

            g2dOverlayStart(g2dHandle);
            while(hal_g2d_get_status(g2dHandle)) {};
            {
               /// C-model
               memset(dst_sw_image_120X120, 0xCD, 120*120*4);

               g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)&dst_sw_image_120X120[0]);

               g2d_c_model(&g2d_sw_register);
            }
            {
               g2d_compare_sw_hw_result();
            }
         }

         G2D_LOGI("  Disabled\n\r");
         {
            g2dOverlayDisableBg(g2dHandle, KAL_TRUE);

            memset(dst_hw_image_120X120, 0xCD, 120*120*4);

            g2dOverlayStart(g2dHandle);
            while(hal_g2d_get_status(g2dHandle)) {};
            {
               /// C-model
               memset(dst_sw_image_120X120, 0xCD, 120*120*4);

               g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)&dst_sw_image_120X120[0]);

               g2d_c_model(&g2d_sw_register);
            }
            {
               g2d_compare_sw_hw_result();
            }
         }
      }

      hal_g2d_release_handle(g2dHandle);
   }
}



void g2d_overlay_roi_output_constant_alpha_test(void)
{
   /*****************************************
    * To test the correctness of output constant alpha.
    * 1. Layer 0 enable constant alpha output
    *    a. Different alpha value
    *****************************************/

   g2d_handle_t *g2dHandle;

   uint8_t *layer0Addr;
   g2d_color_format_t layer0ColorFormat;
   uint32_t layer0Width, layer0Height;
   int32_t layer0RectX, layer0RectY;
   uint32_t layer0RectW, layer0RectH;
   bool layer0SrcKeyEnable;
   uint32_t layer0SrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layer0Rotate;
   bool layer0AlphaEnable;
   uint32_t layer0AlphaValue;

   uint32_t bgColor;
   bool outputAlphaEnable;
   uint32_t outputAlphaValue;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;

   g2d_color_format_t dstColorFormat;
   int32_t dstRectX, dstRectY;
   uint32_t dstWidth, dstHeight;

   int32_t loopCount;

   G2D_LOGI("g2d_overlay_roi_output_constant_alpha_test()\n\r");

   {
      layer0Addr = (uint8_t *)&rgb565_120X120[0];
      layer0ColorFormat = G2D_COLOR_FORMAT_RGB565;
      layer0Width = 120;
      layer0Height = 120;
      layer0RectX = 0;
      layer0RectY = 0;
      layer0RectW = 120;
      layer0RectH = 160;
      layer0SrcKeyEnable = KAL_FALSE;
      layer0SrcKeyValue = 0xFFFFFFFF;
      layer0Rotate = G2D_CW_ROTATE_ANGLE_000;
      layer0AlphaEnable = KAL_FALSE;
      layer0AlphaValue = 0xFF;
      memset(layer0_image, 0x56, 120*120*4);

      bgColor = 0xF6;
      outputAlphaEnable = KAL_TRUE;
      //outputAlphaValue = 0x88;
      roiRectX = 0;
      roiRectY = 0;
      roiRectW = 50;
      roiRectH = 120;

      dstColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = 60;
      dstHeight = 120;

      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)dst_hw_image_120X120, DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      //g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, 120 * 120 * 2, layer0Width, layer0Height, layer0ColorFormat);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);

      for(loopCount = 0; loopCount < 100; loopCount++)
      {
         outputAlphaValue = (rand() % 256);

         G2D_LOGI("  Alpha: %d\n\r", outputAlphaValue);
         {
            g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);

            memset(dst_hw_image_120X120, 0xCD, 120*120*4);

            g2dOverlayStart(g2dHandle);
            while(hal_g2d_get_status(g2dHandle)) {};
            {
               /// C-model
               memset(dst_sw_image_120X120, 0xCD, 120*120*4);

               g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)&dst_sw_image_120X120[0]);

               g2d_c_model(&g2d_sw_register);
            }
            {
               g2d_compare_sw_hw_result();
            }
         }
      }

      hal_g2d_release_handle(g2dHandle);
   }
}



void g2d_overlay_roi_memory_coordinate_test(void)
{
   /*****************************************
    * To test the memory out offset.
    * 1. Layer 0
    * 2. Destination
    *    a. x offset from -2048 to 2047
    *    b. y offset from -2048 to 2047
    *    c. Should test the boundary
    *****************************************/

   g2d_handle_t *g2dHandle;

   uint8_t *layer0Addr;
   g2d_color_format_t layer0ColorFormat;
   uint32_t layer0Width, layer0Height;
   int32_t layer0RectX, layer0RectY;
   uint32_t layer0RectW, layer0RectH;
   bool layer0SrcKeyEnable;
   uint32_t layer0SrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layer0Rotate;
   bool layer0AlphaEnable;
   uint32_t layer0AlphaValue;

   uint32_t bgColor;
   bool outputAlphaEnable;
   uint32_t outputAlphaValue;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;

   g2d_color_format_t dstColorFormat;
   int32_t dstRectX, dstRectY;
   uint32_t dstWidth, dstHeight;

   int32_t loopCount;

   G2D_LOGI("g2d_overlay_roi_memory_coordinate_test()\n\r");

   {
      layer0Addr = (uint8_t *)&rgb565_120X120[0];
      layer0ColorFormat = G2D_COLOR_FORMAT_RGB565;
      layer0Width = 120;
      layer0Height = 120;
      layer0RectX = 0;
      layer0RectY = 0;
      layer0RectW = 120;
      layer0RectH = 120;
      layer0SrcKeyEnable = KAL_FALSE;
      layer0SrcKeyValue = 0xFFFFFFFF;
      layer0Rotate = G2D_CW_ROTATE_ANGLE_000;
      layer0AlphaEnable = KAL_FALSE;
      layer0AlphaValue = 0xFF;
      memset(layer0_image, 0x56, 120*120*4);

      bgColor = 0xF6;
      outputAlphaEnable = KAL_FALSE;
      //outputAlphaValue = 0x88;
      roiRectX = 0;
      roiRectY = 0;
      //roiRectW = 10;
      //roiRectH = 10;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      //dstRectX = 0;
      //dstRectY = 0;
      //dstWidth = 120;
      //dstHeight = 120;

      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      //g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      //hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)dstBuf, DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      //g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, 120 * 120 * 2, layer0Width, layer0Height, layer0ColorFormat);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);


      for(loopCount = 0; loopCount < 5; loopCount++)
      {
         if(loopCount == 5)
         {
            roiRectW = 10;
            roiRectH = 2048;

            dstRectX = -5;
            dstRectY = 2047;
            dstWidth = 10;
            dstHeight = 1;
         }
         else if(loopCount == 4)
         {
            roiRectW = 10;
            roiRectH = 10;

            dstRectX = -5;
            dstRectY = -5;
            dstWidth = 10;
            dstHeight = 10;
         }
         else if(loopCount == 3)
         {
            roiRectW = 10;
            roiRectH = 2048;

            dstRectX = -5;
            dstRectY = -2043;
            dstWidth = 10;
            dstHeight = 10;
         }
         else if(loopCount == 2)
         {
            roiRectW = 10;
            roiRectH = 2048;

            dstRectX = -5;
            dstRectY = -2048;
            dstWidth = 10;
            dstHeight = 10;
         }
         else if(loopCount == 1)
         {
            roiRectW = 2048;
            roiRectH = 10;

            dstRectX = -2043;
            dstRectY = -5;
            dstWidth = 10;
            dstHeight = 10;
         }
         else
         {
            roiRectW = 2048;
            roiRectH = 10;

            dstRectX = -2048;
            dstRectY = -5;
            dstWidth = 10;
            dstHeight = 10;
         }
/*       // the test items are reduced due to c model speed is too slow on the target
         if(loopCount >= 9)
         {
            dstRectX = (rand() % 4096) - 2048 - 2;
            if(dstRectX > 2048 - 9)
               dstRectX = 2048 - 9;
            else if(dstRectX < -2048)
               dstRectX = -2048;

            dstRectY = (rand() % 4096) - 2048 - 2;
            if(dstRectY > 2048 - 9)
               dstRectY = 2048 - 9;
            else if(dstRectY < -2048)
               dstRectY = -2048;

            dstWidth = 9;
            dstHeight = 9;
         }
         else if(loopCount == 13)
         {
            roiRectW = 2048;
            roiRectH = 2048;

            dstRectX = 2047;
            dstRectY = 2047;
            dstWidth = 1;
            dstHeight = 1;
         }
         else if(loopCount == 12)
         {
            roiRectW = 2048;
            roiRectH = 10;

            dstRectX = 2047;
            dstRectY = -5;
            dstWidth = 1;
            dstHeight = 10;
         }
         else if(loopCount == 11)
         {
            roiRectW = 2048;
            roiRectH = 2048;

            dstRectX = 2047;
            dstRectY = -2043;
            dstWidth = 1;
            dstHeight = 10;
         }
         else if(loopCount == 10)
         {
            roiRectW = 2048;
            roiRectH = 2048;

            dstRectX = 2047;
            dstRectY = -2048;
            dstWidth = 1;
            dstHeight = 10;
         }
         else if(loopCount == 9)
         {
            roiRectW = 10;
            roiRectH = 2048;

            dstRectX = -5;
            dstRectY = 2047;
            dstWidth = 10;
            dstHeight = 1;
         }
         else if(loopCount == 8)
         {
            roiRectW = 10;
            roiRectH = 10;

            dstRectX = -5;
            dstRectY = -5;
            dstWidth = 10;
            dstHeight = 10;
         }
         else if(loopCount == 7)
         {
            roiRectW = 10;
            roiRectH = 2048;

            dstRectX = -5;
            dstRectY = -2043;
            dstWidth = 10;
            dstHeight = 10;
         }
         else if(loopCount == 6)
         {
            roiRectW = 10;
            roiRectH = 2048;

            dstRectX = -5;
            dstRectY = -2048;
            dstWidth = 10;
            dstHeight = 10;
         }
         else if(loopCount == 5)
         {
            roiRectW = 2048;
            roiRectH = 2048;

            dstRectX = -2043;
            dstRectY = 2047;
            dstWidth = 10;
            dstHeight = 1;
         }
         else if(loopCount == 4)
         {
            roiRectW = 2048;
            roiRectH = 2048;

            dstRectX = -2048;
            dstRectY = 2047;
            dstWidth = 10;
            dstHeight = 1;
         }
         else if(loopCount == 3)
         {
            roiRectW = 2048;
            roiRectH = 10;

            dstRectX = -2043;
            dstRectY = -5;
            dstWidth = 10;
            dstHeight = 10;
         }
         else if(loopCount == 2)
         {
            roiRectW = 2048;
            roiRectH = 10;

            dstRectX = -2048;
            dstRectY = -5;
            dstWidth = 10;
            dstHeight = 10;
         }
         else if(loopCount == 1)
         {
            roiRectW = 2048;
            roiRectH = 2048;
            dstRectX = -2043;
            dstRectY = -2043;
            dstWidth = 10;
            dstHeight = 10;
         }
         else
         {
            roiRectW = 2048;
            roiRectH = 2048;
            dstRectX = -2048;
            dstRectY = -2048;
            dstWidth = 10;
            dstHeight = 10;
         }
*/
         G2D_LOGI("  X: %d, Y: %d, W: %d, H: %d\n\r", dstRectX, dstRectY, dstWidth, dstHeight);

         memset(dst_hw_image_120X120, 0xCD, 120*120*4);

         g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
         hal_set_destination_rgb_buffer_information(g2dHandle, ((uint8_t *)dst_hw_image_120X120), DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
         g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

         g2dOverlayStart(g2dHandle);
         while(hal_g2d_get_status(g2dHandle)) {};
         {
            /// C-model
            memset(dst_sw_image_120X120, 0xCD, 120*120*4);

            g2d_set_registers_to_c_model(&g2d_sw_register, ((uint8_t *)dst_sw_image_120X120));

            g2d_c_model(&g2d_sw_register);
         }

         {
            g2d_compare_sw_hw_result();
         }
      }

      hal_g2d_release_handle(g2dHandle);
   }
}



void g2d_overlay_roi_dithering_test(void)
{
   g2d_handle_t *g2dHandle;

   uint8_t *layer0Addr;
   g2d_color_format_t layer0ColorFormat;
   uint32_t layer0Width, layer0Height;
   int32_t layer0RectX, layer0RectY;
   uint32_t layer0RectW, layer0RectH;
   bool layer0SrcKeyEnable;
   uint32_t layer0SrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layer0Rotate;
   bool layer0AlphaEnable;
   uint32_t layer0AlphaValue;

   uint32_t bgColor;
   bool outputAlphaEnable;
   uint32_t outputAlphaValue;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;

   g2d_color_format_t dstColorFormat;
   int32_t dstRectX, dstRectY;
   uint32_t dstWidth, dstHeight;

   int32_t loopCount;

   G2D_LOGI("g2d_overlay_roi_dithering_test()\n\r");

   {
      layer0Addr = (uint8_t *)&rgb565_120X120[0];
      layer0ColorFormat = G2D_COLOR_FORMAT_RGB565;
      layer0Width = 100;
      layer0Height = 100;
      layer0RectX = 0;
      layer0RectY = 0;
      layer0RectW = 100;
      layer0RectH = 100;
      layer0SrcKeyEnable = KAL_FALSE;
      layer0SrcKeyValue = 0xFFFFFFFF;
      layer0Rotate = G2D_CW_ROTATE_ANGLE_000;
      layer0AlphaEnable = KAL_FALSE;
      layer0AlphaValue = 0xFF;
      memset(layer0_image, 0x56, 120*120*4);

      bgColor = 0xF6;
      outputAlphaEnable = KAL_TRUE;
      //outputAlphaValue = 0x88;
      roiRectX = 0;
      roiRectY = 0;
      roiRectW = 100;
      roiRectH = 100;

      dstColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = 100;
      dstHeight = 100;

      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)dst_hw_image_120X120, DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      //g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layer0Addr, 120 * 120 * 2, layer0Width, layer0Height, layer0ColorFormat);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layer0SrcKeyEnable, layer0SrcKeyValue);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layer0Rotate);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layer0AlphaEnable, layer0AlphaValue);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, layer0RectX, layer0RectY, layer0RectW, layer0RectH);
      g2dOverlaySetDstAlpha(g2dHandle, KAL_FALSE, 0xFF);
      
      for(loopCount = 0; loopCount < 10; loopCount++)
      {
         uint32_t rBit, gBit, bBit;

         for(rBit = 0; rBit < 4; rBit++)
         { 
         	  for(gBit = 0; gBit < 4; gBit++)
            {
            	 for(bBit = 0; bBit < 4; bBit++)        
               {
                  G2D_LOGI("\trBit = %d, gBit = %d, bBit = %d\n\r", rBit, gBit, bBit);
                  hal_g2d_set_ditheringInitialSeed(g2dHandle, rand(), rand());
                  
                  hal_g2d_set_dithering(g2dHandle, KAL_TRUE, G2D_DITHERING_MODE_RANDOM_ALGORITHM);
                  hal_g2d_set_ditheringBit(g2dHandle, (G2D_DITHERING_BIT_ENUM)rBit, (G2D_DITHERING_BIT_ENUM)gBit, (G2D_DITHERING_BIT_ENUM)bBit);
                  memset(dst_hw_image_120X120, 0xCD, 120*120*4);
               
                  g2dOverlayStart(g2dHandle);
                  while(hal_g2d_get_status(g2dHandle)) {};
                  {
                     /// C-model
                     memset(dst_sw_image_120X120, 0xCD, 120*120*4);
               
                     g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)&dst_sw_image_120X120[0]);
               
                     g2d_c_model(&g2d_sw_register);
                  }
                  {
                     _g2d_compare_sw_hw_result100x100();
                  }
               }
            }
         } 
                  
         {
            hal_g2d_set_dithering(g2dHandle, KAL_TRUE, G2D_DITHERING_MODE_FIXED_PATTERN);
            
            memset(dst_hw_image_120X120, 0xCD, 120*120*4);

            g2dOverlayStart(g2dHandle);
            while(hal_g2d_get_status(g2dHandle)) {};
            {
               /// C-model
               memset(dst_sw_image_120X120, 0xCD, 120*120*4);

               g2d_set_registers_to_c_model(&g2d_sw_register, (uint8_t *)&dst_sw_image_120X120[0]);

               g2d_c_model(&g2d_sw_register);
            }
            {
               _g2d_compare_sw_hw_result100x100();
            }
         }
      }

      hal_g2d_release_handle(g2dHandle);
   }	
}



static void _g2d_compare_sw_hw_result100x100(void)
{
   int i;

   for(i = 0; i < (100*100*4); i++ )
   {
      if(dst_sw_image_120X120[i] != dst_hw_image_120X120[i])
      {
         G2D_LOGI("Error in index = %d, HW: %x, SW: %x\n\r",i, dst_hw_image_120X120[i], dst_sw_image_120X120[i]);
         return;
      }
   }
}



static void _g2d_overlay_init_layer_sets(int num, int32_t *layerSets)
{
   int i;

   for(i = 0; i < num ; i++)
   {
     layerSets[i] = i;
   }
}



static void _g2d_overlay_enable_layer(int32_t *layerSets, int32_t layerNumberChoose)
{
   int i;

   for(i = 0; i < layerNumberChoose; i++)
   {
      G2D_LOGI("layer = %d\n\r", layerSets[i]);
   }
}



static void _g2d_overlay_set_layer(g2d_handle_t* g2dHandle, G2D_OVERLAY_LAYER_ENUM layer, G2D_OVERLAY_LAYER_FUNCTION_ENUM function)
{
   uint8_t *layerAddr = (uint8_t *)rgb565_120X120;
   uint8_t layerAlphaEnable, layerSrcKeyEnable;
   uint32_t layerAlphaValue, layerSrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layerRotate = G2D_CW_ROTATE_ANGLE_000;
   G2D_FONT_BIT_ENUM fontBit = 0;
   g2d_color_format_t layerColorFormat;

   g2dOverlaySetLayerFunction(g2dHandle, layer, function);
   g2dOverlaySetLayerWindow(g2dHandle, layer, 0, 0, 100, 100);

   // overlay lower layers with current layer
   g2dOverlayEnableLayer(g2dHandle, layer, KAL_TRUE);

   if(G2D_OVERLAY_LAYER_FUNCTION_NORMAL_FONT == function || G2D_OVERLAY_LAYER_FUNCTION_AA_FONT == function)
   {
      layerRotate = G2D_CW_ROTATE_ANGLE_000;
      g2dOverlaySetLayerRotation(g2dHandle, layer, layerRotate);

      g2dOverlaySetLayerSrcKey(g2dHandle, layer, KAL_FALSE, 0);

      if(G2D_OVERLAY_LAYER_FUNCTION_NORMAL_FONT == function)
      {
         layerAlphaEnable = 0;
      }
      else
      {
         layerAlphaEnable = 1;
         layerAlphaValue = rand() % 256;
         fontBit = (G2D_FONT_BIT_ENUM)(rand() % 4);
         g2dOverlaySetLayerFontBit(g2dHandle, layer, fontBit);
      }
      g2dOverlaySetLayerAlpha(g2dHandle, layer, (bool)layerAlphaEnable, layerAlphaValue);

      g2dOverlaySetLayerBufferInfo(g2dHandle, layer, layerAddr , LAYER0_SIZE);

      G2D_LOGI("  Layer: %d, Font Func: %d, Font Bit: %d, Alpha: %d(%d)\n\r", layer, function, fontBit, layerAlphaEnable, layerAlphaValue);
   }
   else if(G2D_OVERLAY_LAYER_FUNCTION_RECT == function)
   {
      layerRotate = G2D_CW_ROTATE_ANGLE_000;
      g2dOverlaySetLayerRotation(g2dHandle, layer, layerRotate);

      g2dOverlaySetLayerSrcKey(g2dHandle, layer, KAL_FALSE, 0);

      layerAlphaEnable = rand() % 2;
      layerAlphaValue = rand() % 256;
      g2dOverlaySetLayerAlpha(g2dHandle, layer, (bool)layerAlphaEnable, layerAlphaValue);

      //layerColorFormat = (g2d_color_format_t)(rand() % 9 + 1);
      //g2dOverlaySetLayerRGBBufferInfo(g2dHandle, layer, layerAddr, 120 * 120 * 2, 100, 100, layerColorFormat);

      G2D_LOGI("  Layer: %d, RECT Func, Alpha: %d(%d)\n\r", layer, layerAlphaEnable, layerAlphaValue);
   }
   else
   {
      {
         layerRotate = (G2D_CW_ROTATE_ANGLE_ENUM)(rand() % 8);
         g2dOverlaySetLayerRotation(g2dHandle, layer, layerRotate);

         layerSrcKeyEnable = rand() % 2;
         layerSrcKeyValue = rand();

         layerAlphaEnable = rand() % 2;
         layerAlphaValue = rand() % 256;
         g2dOverlaySetLayerAlpha(g2dHandle, layer, (bool)layerAlphaEnable, layerAlphaValue);

         layerColorFormat = (g2d_color_format_t)(rand() % 10 + 1);
         if(G2D_COLOR_FORMAT_UYVY422 != layerColorFormat)
            g2dOverlaySetLayerRGBBufferInfo(g2dHandle, layer, layerAddr, 120 * 120 * 2, 100, 100, layerColorFormat);
         else
         {
            uint8_t *bufferAddr[3];
            uint32_t bufferSize[3];

            bufferAddr[0] = layerAddr;
            bufferAddr[1] = NULL;
            bufferAddr[2] = NULL;
            bufferSize[0] = 120 * 120 * 2;
            bufferSize[1] = 0;
            bufferSize[2] = 0;

            g2dOverlaySetLayerYUVBufferInfo(g2dHandle, layer, bufferAddr, bufferSize, 100, 100, layerColorFormat);
            
            layerSrcKeyEnable = 0;
         }
         
         g2dOverlaySetLayerSrcKey(g2dHandle, layer, (bool)layerSrcKeyEnable, layerSrcKeyValue);
         
         G2D_LOGI("  Layer: %d, Buffer Func, SrcClrFmt: %d, SrcKey: %d(%x), Alpha: %d(%d)\n\r", layer, layerColorFormat, layerSrcKeyEnable, layerSrcKeyValue, layerAlphaEnable, layerAlphaValue);

      }
   }
}


#if 0 //Dream: Larkspur
void g2d_bitblt_rotation_test_WHQA_00011604(void)
{
   g2d_handle_t *g2dHandle;

   uint8_t *layerAddr = (uint8_t *)&rgb565_999X480[0];
   g2d_color_format_t layerColorFormat = G2D_COLOR_FORMAT_RGB565;
   uint32_t layerWidth = 480;
   uint32_t layerHeight = 120;
   uint32_t layerRectW = 480;
   uint32_t layerRectH = 120;
   bool layerSrcKeyEnable = KAL_FALSE;
   uint32_t layerSrcKeyValue = 0xFFFFFFFF;
   bool layerAlphaEnable = KAL_FALSE;
   uint32_t layerAlphaValue = 0x33;

   uint32_t bgColor = 0x0;
   bool outputAlphaEnable = KAL_FALSE;
   uint32_t outputAlphaValue = 0xFF;
   int32_t roiRectX = 0;
   int32_t roiRectY = 0;
   uint32_t roiRectW = 120;
   uint32_t roiRectH = 480;

   g2d_color_format_t dstColorFormat = G2D_COLOR_FORMAT_RGB565;
   int32_t dstRectX = 0;
   int32_t dstRectY = 0;
   uint32_t dstWidth = 120;
   uint32_t dstHeight = 480;

   {
      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      // layer 1
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, layerAddr, 240 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layerAlphaEnable, layerAlphaValue);

      G2D_LOGI("g2d_overlay_layer_rotation_test()\n\r");
      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, (G2D_CW_ROTATE_ANGLE_ENUM)G2D_CW_ROTATE_ANGLE_090);

      memset(dst_hw_image_120X120, 0xCD, 120*120*4);

      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      {
      }
      hal_g2d_release_handle(g2dHandle);
   }
}
#endif
void g2d_overlay_all_scenario_combination_test(void)
{
   /*****************************************
    * To test all scenario combinations
    * 1. For each layer (128)
    *    a. src key: enable, disable (2)
    *    b. function: buffer, normal font, aa font, rectangle fill (4)
    *    c. rotation: random (1) // 0, 90, 180, 270, with and without mirror (8)
    *    d. color format: random (1)
    *    e. alpha blending: enable, disable (2)
    * 2. All layer combination (16)
    * 3. ROI (4)
    *    a. bg color: enable, disable (2)
    *    b. color replacement: enable, disable (2)
    *    c. color format: random (1)
    *    d. output constant alpha: random (1)
    *    e. clipping window: always on (1)
    * 4. All offsets and sizes are fixed (1)
    *****************************************/

   g2d_handle_t *g2dHandle;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;
   int32_t dstRectX, dstRectY;
   uint32_t layer1stfunction, layer2ndfunction, layer3rdfunction, layer4thfunction;
   g2d_color_format_t dstColorFormat;
   uint32_t outputAlphaValue;
   uint32_t bgColor;
   uint8_t a0, r0, g0, b0, a1, r1, g1, b1;

   int32_t layerNumberChoose, layerPosition, layerSets[4];
   uint32_t layerNumberTotal = 4;
   //int i, loopCount, overallLoopCount = 1000;
   int i, loopCount, overallLoopCount = 5;

   ///_g2d_overlay_all_scenario_combination_init();


   /// G2D_STATUS_BUSY means someone is using G2D
   if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
   {
      G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
      return;
   }

   /// ROI region
   {
      roiRectX = 0;
      roiRectY = 0;
      roiRectW = 100;
      roiRectH = 100;
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      // Destination region
      dstRectX = 0;
      dstRectY = 0;
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);

      // Misc
      hal_g2d_set_callback_function(g2dHandle, NULL);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, 100, 100);
      g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);
   }

   while(--overallLoopCount >= 0)
   {            
      G2D_LOGI("Enable 4 layer #################\n\r");
      loopCount = 100;
      while(--loopCount >= 0)
      {
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_FALSE);
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_FALSE);
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER2, KAL_FALSE);
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER3, KAL_FALSE);
      
         G2D_LOGI("\n\rloopCount = %d\n\r",loopCount);
         // _g2d_overlay_enable_layer(layerSets,layerNumberChoose);
         for(layer1stfunction = 0; layer1stfunction < 4; layer1stfunction++)
         {
            G2D_LOGI("\n\r\n\rFirst\n\r");
            _g2d_overlay_set_layer(g2dHandle, G2D_OVERLAY_LAYER0, (G2D_OVERLAY_LAYER_FUNCTION_ENUM)layer1stfunction);
      
            for(layer2ndfunction = 0; layer2ndfunction < 4; layer2ndfunction++)
            {
               G2D_LOGI("\n\r\n\rSecond\n\r");
               _g2d_overlay_set_layer(g2dHandle, G2D_OVERLAY_LAYER1, (G2D_OVERLAY_LAYER_FUNCTION_ENUM)layer2ndfunction);
      
               for(layer3rdfunction = 0; layer3rdfunction < 4; layer3rdfunction++)
               {
                  G2D_LOGI("\n\r\n\rThird\n\r");
                  _g2d_overlay_set_layer(g2dHandle, G2D_OVERLAY_LAYER2, (G2D_OVERLAY_LAYER_FUNCTION_ENUM)layer3rdfunction);
      
                  for(layer4thfunction = 0; layer4thfunction < 4; layer4thfunction++)
                  {
                  	  G2D_LOGI("\n\r\n\rForth\n\r");
                     _g2d_overlay_set_layer(g2dHandle, G2D_OVERLAY_LAYER3, (G2D_OVERLAY_LAYER_FUNCTION_ENUM)layer4thfunction);
                     {
                        /// output alpha
                        {
                           if(0 == (rand() % 2))
                           {
                              g2dOverlaySetDstAlpha(g2dHandle, KAL_FALSE, outputAlphaValue);
                           }
                           else
                           {
                              outputAlphaValue = rand() % 256;
                              g2dOverlaySetDstAlpha(g2dHandle, KAL_TRUE, outputAlphaValue);
                              G2D_LOGI("OutAlpha: %d\n\r",outputAlphaValue);
                           }
                        }
      
                        /// bgColor
                        {
                            if(0 == (rand() % 2))
                            {
                               g2dOverlayDisableBg(g2dHandle, KAL_FALSE);
                               bgColor = rand();
                               g2dOverlaySetBgColor(g2dHandle, bgColor);
                               G2D_LOGI("BgClr: %x\n\r", bgColor);
                            }
                            else
                            {
                               g2dOverlayDisableBg(g2dHandle, KAL_TRUE);
                            }
                        }
      
                        /// output alpha
                        {
      
                           if(0 == (rand() % 2))
                           {
                              hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 0, 0, 0, 0, 0, 0, 0);
                           }
                           else
                           {
                              a0 = rand(); r0 = rand(); g0 = rand(); b0 = rand();
                              a1 = rand(); r1 = rand(); g1 = rand(); b0 = rand();
                              hal_g2d_set_color_replacement(g2dHandle, KAL_TRUE, a0, r0, g0, b0, a1, r1, g1, b1);
                              G2D_LOGI("ClrRep: (%x, %x)\n\r", (a0 << 24) | (r0 << 16) | (g0 << 8) | b0, (a1 << 24) | (r1 << 16) | (g1 << 8) | b1);
                           }
                        }
                        
                        /// dithering
                        {
                        
                           uint32_t rBit, gBit, bBit;
                           
                           if(0 == (rand() % 3))
                           {
                              hal_g2d_set_dithering(g2dHandle, KAL_FALSE, G2D_DITHERING_MODE_RANDOM_ALGORITHM);
                           }
                           else
                           {
                              if(0 == (rand() % 2))
                              {
                                 hal_g2d_set_dithering(g2dHandle, KAL_TRUE, G2D_DITHERING_MODE_RANDOM_ALGORITHM);
                                 hal_g2d_set_ditheringInitialSeed(g2dHandle, rand(), rand());
                              }
                              else
                              {
                              	 hal_g2d_set_dithering(g2dHandle, KAL_TRUE, G2D_DITHERING_MODE_FIXED_PATTERN);
                              }
                              
                              rBit = rand() % 4;
                              gBit = rand() % 4;
                              bBit = rand() % 4;
                              
                              hal_g2d_set_ditheringBit(g2dHandle, (G2D_DITHERING_BIT_ENUM)rBit, (G2D_DITHERING_BIT_ENUM)gBit, (G2D_DITHERING_BIT_ENUM)bBit);
                              G2D_LOGI("Dithering: (%x, %x, %x)\n\r", rBit, gBit, bBit);
                           }  
                        }
      
                        dstColorFormat = (g2d_color_format_t)(rand() % 9 + 1);
                        hal_set_destination_rgb_buffer_information(g2dHandle, ((uint8_t *)dst_hw_image_120X120), DST_HW_BUFFER_SIZE, 100, 100, dstColorFormat);
      
                        memset(dst_hw_image_120X120, 0xCD, 100*100*4);
                        g2dOverlayStart(g2dHandle);
                        while(hal_g2d_get_status(g2dHandle)) {};
      
                        /// C-model
                        memset(dst_sw_image_120X120, 0xCD, 100*100*4);
                        g2d_set_registers_to_c_model(&g2d_sw_register, ((uint8_t *)dst_sw_image_120X120));
                        g2d_c_model(&g2d_sw_register);
      
                        _g2d_compare_sw_hw_result100x100();
                     }
                  }
               }
            }
         }
      }
      
      G2D_LOGI("Enable 3 layer #################\n\r");
      
      _g2d_overlay_init_layer_sets(layerNumberTotal , layerSets);
      
      layerNumberChoose = 3;
      layerPosition = layerNumberChoose - 1 ;
      
      while(1)
      {
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_FALSE);
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_FALSE);
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER2, KAL_FALSE);
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER3, KAL_FALSE);
      
         // _g2d_overlay_enable_layer(layerSets,layerNumberChoose);
         for(layer1stfunction = 0; layer1stfunction < 4; layer1stfunction++)
         {
            G2D_LOGI("\n\r\n\rFirst\n\r");
            _g2d_overlay_set_layer(g2dHandle, (G2D_OVERLAY_LAYER_ENUM)layerSets[0], (G2D_OVERLAY_LAYER_FUNCTION_ENUM)layer1stfunction);
      
            for(layer2ndfunction = 0; layer2ndfunction < 4; layer2ndfunction++)
            {
               G2D_LOGI("\n\r\n\rSecond\n\r");
               _g2d_overlay_set_layer(g2dHandle, (G2D_OVERLAY_LAYER_ENUM)layerSets[1], (G2D_OVERLAY_LAYER_FUNCTION_ENUM)layer2ndfunction);
      
               for(layer3rdfunction = 0; layer3rdfunction < 4; layer3rdfunction++)
               {
                  G2D_LOGI("\n\r\n\rThird\n\r");
                  _g2d_overlay_set_layer(g2dHandle, (G2D_OVERLAY_LAYER_ENUM)layerSets[2], (G2D_OVERLAY_LAYER_FUNCTION_ENUM)layer3rdfunction);
                  {
                     /// output alpha
                     {
                        if(0 == (rand() % 2))
                        {
                           g2dOverlaySetDstAlpha(g2dHandle, KAL_FALSE, outputAlphaValue);
                        }
                        else
                        {
                           outputAlphaValue = rand() % 256;
                           g2dOverlaySetDstAlpha(g2dHandle, KAL_TRUE, outputAlphaValue);
                           G2D_LOGI("OutAlpha: %d\n\r",outputAlphaValue);
                        }
                     }
      
                     /// bgColor
                     {
                         if(0 == (rand() % 2))
                         {
                            g2dOverlayDisableBg(g2dHandle, KAL_FALSE);
                            bgColor = rand();
                            g2dOverlaySetBgColor(g2dHandle, bgColor);
                            G2D_LOGI("BgClr: %x\n\r", bgColor);
                         }
                         else
                         {
                            g2dOverlayDisableBg(g2dHandle, KAL_TRUE);
                         }
                     }
      
                     /// output alpha
                     {
      
                        if(0 == (rand() % 2))
                        {
                           hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 0, 0, 0, 0, 0, 0, 0);
                        }
                        else
                        {
                           a0 = rand(); r0 = rand(); g0 = rand(); b0 = rand();
                           a1 = rand(); r1 = rand(); g1 = rand(); b0 = rand();
                           hal_g2d_set_color_replacement(g2dHandle, KAL_TRUE, a0, r0, g0, b0, a1, r1, g1, b1);
                           G2D_LOGI("ClrRep: (%x, %x)\n\r", (a0 << 24) | (r0 << 16) | (g0 << 8) | b0, (a1 << 24) | (r1 << 16) | (g1 << 8) | b1);
                        }
                     }

                     /// dithering
                     {
                     
                        uint32_t rBit, gBit, bBit;
                        
                        if(0 == (rand() % 3))
                        {
                           hal_g2d_set_dithering(g2dHandle, KAL_FALSE, G2D_DITHERING_MODE_RANDOM_ALGORITHM);
                        }
                        else
                        {
                           if(0 == (rand() % 2))
                           {
                              hal_g2d_set_dithering(g2dHandle, KAL_TRUE, G2D_DITHERING_MODE_RANDOM_ALGORITHM);
                              hal_g2d_set_ditheringInitialSeed(g2dHandle, rand(), rand());
                           }
                           else
                           {
                           	 hal_g2d_set_dithering(g2dHandle, KAL_TRUE, G2D_DITHERING_MODE_FIXED_PATTERN);
                           }
                           
                           rBit = rand() % 4;
                           gBit = rand() % 4;
                           bBit = rand() % 4;
                           
                           hal_g2d_set_ditheringBit(g2dHandle, (G2D_DITHERING_BIT_ENUM)rBit, (G2D_DITHERING_BIT_ENUM)gBit, (G2D_DITHERING_BIT_ENUM)bBit);
                           G2D_LOGI("Dithering: (%x, %x, %x)\n\r", rBit, gBit, bBit);
                        }  
                     }      
                     dstColorFormat = (g2d_color_format_t)(rand() % 9 + 1);
                     hal_set_destination_rgb_buffer_information(g2dHandle, ((uint8_t *)dst_hw_image_120X120), DST_HW_BUFFER_SIZE, 100, 100, dstColorFormat);
      
                     memset(dst_hw_image_120X120, 0xCD, 100*100*4);
                     g2dOverlayStart(g2dHandle);
                     while(hal_g2d_get_status(g2dHandle)) {};
      
                     /// C-model
                     memset(dst_sw_image_120X120, 0xCD, 100*100*4);
                     g2d_set_registers_to_c_model(&g2d_sw_register, ((uint8_t *)dst_sw_image_120X120));
                     g2d_c_model(&g2d_sw_register);
      
                     _g2d_compare_sw_hw_result100x100();
                  }
               }
            }
         }
      
         if(layerSets[layerNumberChoose-1] == (layerNumberTotal -1) )
            layerPosition--;
         else
            layerPosition = layerNumberChoose-1;
      
         if(-1 == layerPosition)
            break;
      
         layerSets[layerPosition]++;
      
         for(i = layerPosition + 1 ; i < layerNumberTotal ; i++)
            layerSets[i] = layerSets[i - 1] + 1;
      
         if( layerSets[0] > (layerNumberTotal - layerNumberChoose))
            break;
      
         if( layerSets[layerNumberChoose - 1] > (layerNumberTotal - 1))
            break;
      }
      
      G2D_LOGI("Enable 2 layer #################\n\r");
      
      _g2d_overlay_init_layer_sets(layerNumberTotal , layerSets);
      
      layerNumberChoose = 2;
      layerPosition = layerNumberChoose - 1 ;
      
      while(1)
      {
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_FALSE);
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_FALSE);
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER2, KAL_FALSE);
         g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER3, KAL_FALSE);
      
         // _g2d_overlay_enable_layer(layerSets,layerNumberChoose);
         for(layer1stfunction = 0; layer1stfunction < 4; layer1stfunction++)
         {
            G2D_LOGI("\n\r\n\rFirst\n\r");
            _g2d_overlay_set_layer(g2dHandle, (G2D_OVERLAY_LAYER_ENUM)layerSets[0], (G2D_OVERLAY_LAYER_FUNCTION_ENUM)layer1stfunction);
      
            for(layer2ndfunction = 0; layer2ndfunction < 4; layer2ndfunction++)
            {
               G2D_LOGI("\n\r\n\rSecond\n\r");
               _g2d_overlay_set_layer(g2dHandle, (G2D_OVERLAY_LAYER_ENUM)layerSets[1], (G2D_OVERLAY_LAYER_FUNCTION_ENUM)layer2ndfunction);
      
               {
                  /// output alpha
                  {
                     if(0 == (rand() % 2))
                     {
                        g2dOverlaySetDstAlpha(g2dHandle, KAL_FALSE, outputAlphaValue);
                     }
                     else
                     {
                        outputAlphaValue = rand() % 256;
                        g2dOverlaySetDstAlpha(g2dHandle, KAL_TRUE, outputAlphaValue);
                        G2D_LOGI("OutAlpha: %d\n\r",outputAlphaValue);
                     }
                  }
      
                  /// bgColor
                  {
                      if(0 == (rand() % 2))
                      {
                         g2dOverlayDisableBg(g2dHandle, KAL_FALSE);
                         bgColor = rand();
                         g2dOverlaySetBgColor(g2dHandle, bgColor);
                         G2D_LOGI("BgClr: %x\n\r", bgColor);
                      }
                      else
                      {
                         g2dOverlayDisableBg(g2dHandle, KAL_TRUE);
                      }
                  }
      
                  /// output alpha
                  {
      
                     if(0 == (rand() % 2))
                     {
                        hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 0, 0, 0, 0, 0, 0, 0);
                     }
                     else
                     {
                        a0 = rand(); r0 = rand(); g0 = rand(); b0 = rand();
                        a1 = rand(); r1 = rand(); g1 = rand(); b0 = rand();
                        hal_g2d_set_color_replacement(g2dHandle, KAL_TRUE, a0, r0, g0, b0, a1, r1, g1, b1);
                        G2D_LOGI("ClrRep: (%x, %x)\n\r", (a0 << 24) | (r0 << 16) | (g0 << 8) | b0, (a1 << 24) | (r1 << 16) | (g1 << 8) | b1);
                     }
                  }
                  
                  /// dithering
                  {
      
                     uint32_t rBit, gBit, bBit;
                     
                     if(0 == (rand() % 3))
                     {
                        hal_g2d_set_dithering(g2dHandle, KAL_FALSE, G2D_DITHERING_MODE_RANDOM_ALGORITHM);
                     }
                     else
                     {
                        if(0 == (rand() % 2))
                        {
                           hal_g2d_set_dithering(g2dHandle, KAL_TRUE, G2D_DITHERING_MODE_RANDOM_ALGORITHM);
                           hal_g2d_set_ditheringInitialSeed(g2dHandle, rand(), rand());
                        }
                        else
                        {
                        	 hal_g2d_set_dithering(g2dHandle, KAL_TRUE, G2D_DITHERING_MODE_FIXED_PATTERN);
                        }
                        
                        rBit = rand() % 4;
                        gBit = rand() % 4;
                        bBit = rand() % 4;
                        
                        hal_g2d_set_ditheringBit(g2dHandle, (G2D_DITHERING_BIT_ENUM)rBit, (G2D_DITHERING_BIT_ENUM)gBit, (G2D_DITHERING_BIT_ENUM)bBit);
                        G2D_LOGI("Dithering: (%x, %x, %x)\n\r", rBit, gBit, bBit);
                     }  
                  }
      
                  dstColorFormat = (g2d_color_format_t)(rand() % 9 + 1);
                  hal_set_destination_rgb_buffer_information(g2dHandle, ((uint8_t *)dst_hw_image_120X120), DST_HW_BUFFER_SIZE, 100, 100, dstColorFormat);
      
                  memset(dst_hw_image_120X120, 0xCD, 100*100*4);
                  g2dOverlayStart(g2dHandle);
                  while(hal_g2d_get_status(g2dHandle)) {};
      
                  /// C-model
                  memset(dst_sw_image_120X120, 0xCD, 100*100*4);
                  g2d_set_registers_to_c_model(&g2d_sw_register, ((uint8_t *)dst_sw_image_120X120));
                  g2d_c_model(&g2d_sw_register);
      
                  _g2d_compare_sw_hw_result100x100();
               }
            }
         }
      
         if(layerSets[layerNumberChoose-1] == (layerNumberTotal -1) )
            layerPosition--;
         else
            layerPosition = layerNumberChoose-1;
      
         if(-1 == layerPosition)
            break;
      
         layerSets[layerPosition]++;
      
         for(i = layerPosition + 1 ; i < layerNumberTotal ; i++)
            layerSets[i] = layerSets[i - 1] + 1;
      
         if( layerSets[0] > (layerNumberTotal - layerNumberChoose))
            break;
      
         if( layerSets[layerNumberChoose - 1] > (layerNumberTotal - 1))
            break;
      }
   }
   hal_g2d_release_handle(g2dHandle);
}




uint32_t average_time;
uint32_t overlay_total_time;
uint32_t violet_time1, violet_time2;
uint32_t violet_time[1000];
uint32_t violet_count = 0;
void g2d_overlay_performance_test(void)
{

   g2d_handle_t *g2dHandle;

   uint32_t width, height;
   uint8_t *layerAddr = (uint8_t *)&layer0_image[0];
   g2d_color_format_t layerColorFormat;
   uint32_t layerWidth;
   uint32_t layerHeight;
   uint32_t layerRectW;
   uint32_t layerRectH;
   bool layerSrcKeyEnable = KAL_TRUE;
   uint32_t layerSrcKeyValue = 0xFFFFFFFF;
   bool layerAlphaEnable;
   uint32_t layerAlphaValue;

   uint32_t bgColor = 0x0;
   bool outputAlphaEnable = KAL_FALSE;
   uint32_t outputAlphaValue = 0xFF;
   int32_t roiRectX;
   int32_t roiRectY;
   uint32_t roiRectW;
   uint32_t roiRectH;

   g2d_color_format_t dstColorFormat;
   int32_t dstRectX;
   int32_t dstRectY;
   uint32_t dstWidth;
   uint32_t dstHeight;
   G2D_CW_ROTATE_ANGLE_ENUM layerRotate;

   {
      int i;
      for(i = 0; i < LAYER0_SIZE; i++)
      {
      	 layer0_image[i] = rand() & 0xFF;
      }	
      
      for(i = 0; i < LAYER1_SIZE; i++)
      {
      	 layer1_image[i] = rand() & 0xFF;
      }
   }

   /// bitblt 80x80 rotate 0
   {
      width = 80;
      height = 80;

      dstColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = KAL_FALSE;
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      roiRectW = layerWidth;
      roiRectH = layerHeight;
      layerRotate = G2D_CW_ROTATE_ANGLE_000;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layerAddr, 120 * 120 * 4, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }

   /// bitblt 80x80 rotate 90
   {
      width = 80;
      height = 80;

      dstColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = KAL_FALSE;
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      roiRectW = layerWidth;
      roiRectH = layerHeight;
      layerRotate = G2D_CW_ROTATE_ANGLE_090;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layerAddr, 120 * 120 * 4, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }

   /// bitblt 120x120 rotate 0
   {
      width = 120;
      height = 120;

      dstColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = KAL_FALSE;
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      roiRectW = layerWidth;
      roiRectH = layerHeight;
      layerRotate = G2D_CW_ROTATE_ANGLE_000;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layerAddr, 120 * 120 * 4, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }

   /// bitblt 120x120 rotate 90
   {
      width = 120;
      height = 120;

      dstColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      //hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstHeight, dstWidth, dstColorFormat);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = KAL_FALSE;
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      //roiRectW = layerWidth;
      //roiRectH = layerHeight;
      roiRectW =  layerHeight;
      roiRectH = layerWidth;
      layerRotate = G2D_CW_ROTATE_ANGLE_090;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      //g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layerAddr, 120 * 120 * 4, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layerAddr, 120 * 120 * 4, layerHeight, layerWidth, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }

   /// bitblt 80x80 rotate 0 enable alpha
   {
      width = 80;
      height = 80;

      dstColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = KAL_TRUE;
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      roiRectW = layerWidth;
      roiRectH = layerHeight;
      layerRotate = G2D_CW_ROTATE_ANGLE_000;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }

   /// bitblt 80x80 rotate 90 enable alpha
   {
      width = 80;
      height = 80;

      dstColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = KAL_TRUE;
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      roiRectW = layerWidth;
      roiRectH = layerHeight;
      layerRotate = G2D_CW_ROTATE_ANGLE_090;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }


   /// bitblt 120x120 rotate 0 enable alpha
   {
      width = 120;
      height = 120;

      dstColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = KAL_TRUE;
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      roiRectW = layerWidth;
      roiRectH = layerHeight;
      layerRotate = G2D_CW_ROTATE_ANGLE_000;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }


   /// bitblt 120x480 rotate 90 enable alpha
   {
      width = 120;
      height = 120;

      dstColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      //hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstHeight, dstWidth, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = KAL_TRUE;
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      //roiRectW = layerWidth;
      //roiRectH = layerHeight;
      roiRectW = layerHeight;
      roiRectH = layerWidth;
      layerRotate = G2D_CW_ROTATE_ANGLE_090;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }









}

void g2d_overlay_layer_alpha_blending_performance_test(void)
{
   /*****************************************
    * To test the correctness of alpha blending
    * 1. Fix Layer 0: ARGB8888 without alpha blending
    * 2. Layer 1
    *  I. Alpha value
    *    a. 0x00
    *    b. 0xFF
    *    c. random
    *  II. Color format
    *    a. RGB565
    *    b. BGR888
    *    c. RGB888
    *    d. ARGB8888
    *    e. PARGB8888
    *    f. YUV422
    *****************************************/

   g2d_handle_t *g2dHandle;

   uint8_t *layerAddr;

   uint32_t layerWidth, layer_height;
   bool layerSrcKeyEnable;
   uint32_t layerSrcKeyValue;
   G2D_CW_ROTATE_ANGLE_ENUM layerRotate;
   uint32_t layerAlphaValue;

   uint32_t bgColor;
   bool outputAlphaEnable;
   uint32_t outputAlphaValue;
   int32_t roiRectX, roiRectY;
   uint32_t roiRectW, roiRectH;

   g2d_color_format_t dstColorFormat;
   int32_t dstRectX, dstRectY;
   uint32_t dstWidth, dstHeight;

   int32_t colorLoop, alphaLoop;
   int32_t i0, i1, i2, i3;

   G2D_LOGI("g2d_overlay_layer_alpha_blending_performance_test()\n\r");

   {
      layerAddr = (uint8_t *)(uint8_t *)&layer0_image[0];
      //layer0ColorFormat = G2D_COLOR_FORMAT_RGB565;
      layerWidth = 120;
      layer_height = 120;
      layerSrcKeyEnable = KAL_FALSE;
      layerSrcKeyValue = 0xFFFFFFFF;
      layerRotate = G2D_CW_ROTATE_ANGLE_000;
      //layerAlphaValue = 0x56;

      bgColor = 0x66;
      outputAlphaEnable = KAL_FALSE;
      outputAlphaValue = 0xFF;
      roiRectX = 0;
      roiRectY = 0;
      roiRectW = 120;
      roiRectH = 120;

      dstColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = 120;
      dstHeight = 120;

      {
        uint32_t i;
        uint32_t *ptr = (uint32_t *)&layer0_image[0];

        for(i = 0; i < 120 * 120; i++)
        {
           *ptr = rand() & 0xFFFFFFFF;
           ptr++;
        }
      }

      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_TRUE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      /// Layer 0
      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, layerAddr, LAYER0_SIZE, layerWidth, layer_height, G2D_COLOR_FORMAT_ARGB8888);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, 120, 120);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, KAL_FALSE, 0);

      /// Layer 1
      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 0, 0, 120, 120);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

	  g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, layerAddr, LAYER0_SIZE, layerWidth, layer_height, G2D_COLOR_FORMAT_ARGB8888);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE, 0x80);

      memset(dst_hw_image_120X120, 0xCD, 120*120*4);
	  violet_count = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);

	  hal_g2d_release_handle(g2dHandle);
   }
}



void g2d_overlay_performance_test_PM_Require(void)
{

   g2d_handle_t *g2dHandle;

   uint32_t width, height;
   uint8_t *layerAddr = (uint8_t *)&layer0_image[0];
   g2d_color_format_t layerColorFormat;
   uint32_t layerWidth;
   uint32_t layerHeight;
   uint32_t layerRectW;
   uint32_t layerRectH;
   bool layerSrcKeyEnable = KAL_TRUE;
   //bool layerSrcKeyEnable = KAL_FALSE;
   uint32_t layerSrcKeyValue = 0xFFFFFFFF;
   bool layerAlphaEnable;
   bool layer1AlphaEnable;
   uint32_t layerAlphaValue;

   uint32_t bgColor = 0x0;
   bool outputAlphaEnable = KAL_FALSE;
   uint32_t outputAlphaValue = 0xFF;
   int32_t roiRectX;
   int32_t roiRectY;
   uint32_t roiRectW;
   uint32_t roiRectH;

   g2d_color_format_t dstColorFormat;
   int32_t dstRectX;
   int32_t dstRectY;
   uint32_t dstWidth;
   uint32_t dstHeight;
   G2D_CW_ROTATE_ANGLE_ENUM layerRotate;
   int k;
   //Test Case 1
   bool g_layerAlphaEnable = KAL_FALSE;
   bool g_layer1AlphaEnable = KAL_TRUE;
   layerSrcKeyEnable = KAL_TRUE;
   layerColorFormat = G2D_COLOR_FORMAT_RGB565;
   //Test Case 2
   //bool g_layerAlphaEnable = KAL_FALSE;
   //bool g_layer1AlphaEnable = KAL_FALSE;
   //layerSrcKeyEnable = KAL_TRUE;
   //layerColorFormat = G2D_COLOR_FORMAT_RGB565;
   //Test Case 3
   //bool g_layerAlphaEnable = KAL_FALSE;
   //bool g_layer1AlphaEnable = KAL_TRUE;
   //layerSrcKeyEnable = KAL_TRUE;
   //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
   //Test Case 4
   //bool g_layerAlphaEnable = KAL_FALSE;
   //bool g_layer1AlphaEnable = KAL_FALSE;
   //layerColorFormat = G2D_COLOR_FORMAT_RGB565;
   //layerSrcKeyEnable = KAL_FALSE;

   {
      int i;
      for(i = 0; i < LAYER0_SIZE; i++)
      {
      	 layer0_image[i] = rand() & 0xFF;
      }	
      
      for(i = 0; i < LAYER1_SIZE; i++)
      {
      	 layer1_image[i] = rand() & 0xFF;
      }
   }
   /// Overlay
   /// bitblt 80x80 rotate 0 enable alpha
   G2D_LOGI("g2d_overlay_performance_test_PM_Require start\n");//Dream
   for(k = 0; k < 5; k++)
   {
      width = 50;
      height = 50;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layer1AlphaEnable = g_layer1AlphaEnable; 
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      roiRectW = layerWidth;
      roiRectH = layerHeight;
      layerRotate = G2D_CW_ROTATE_ANGLE_000;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layer1AlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      //overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);
	  
      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);
   
   /// bitblt 80x80 rotate 90 enable alpha
   for(k = 0; k < 5; k++)
   {
      width = 50;
      height = 50;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);

	  
      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("5517 What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layer1AlphaEnable = g_layer1AlphaEnable; 
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      //roiRectW = layerWidth;
      //roiRectH = layerHeight;
      roiRectW = layerHeight;
      roiRectH = layerWidth;
      layerRotate = G2D_CW_ROTATE_ANGLE_090;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layer1AlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);
      //overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);
   
   for(k = 0; k < 5; k++)
   {
      width = 100;
      height = 100;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("5600: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layer1AlphaEnable = g_layer1AlphaEnable; 
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      roiRectW = layerWidth;
      roiRectH = layerHeight;
      layerRotate = G2D_CW_ROTATE_ANGLE_000;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layer1AlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      //overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);

   /// bitblt 80x80 rotate 90 enable alpha
   for(k = 0; k < 5; k++)
   {
      width = 100;
      height = 100;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("5682: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layer1AlphaEnable = g_layer1AlphaEnable; 
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      //roiRectW = layerWidth;
      //roiRectH = layerHeight;
      roiRectW = layerHeight;
      roiRectH = layerWidth;
      layerRotate = G2D_CW_ROTATE_ANGLE_090;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layer1AlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);
      overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);
   
   for(k = 0; k < 5; k++)
   {
      width = 200;
      height = 200;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("5765: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layer1AlphaEnable = g_layer1AlphaEnable; 
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      roiRectW = layerWidth;
      roiRectH = layerHeight;
      layerRotate = G2D_CW_ROTATE_ANGLE_000;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layer1AlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);
   
   /// bitblt 80x80 rotate 90 enable alpha
   for(k = 0; k < 5; k++)
   {
      width = 200;
      height = 200;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("5847: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layer1AlphaEnable = g_layer1AlphaEnable; 
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      //roiRectW = layerWidth;
      //roiRectH = layerHeight;
      roiRectW = layerHeight;
      roiRectH = layerWidth;
      layerRotate = G2D_CW_ROTATE_ANGLE_090;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layer1AlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);
      overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);

   for(k = 0; k < 5; k++)
   {
      width = 300;
      height = 300;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("5930: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layer1AlphaEnable = g_layer1AlphaEnable; 
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      roiRectW = layerWidth;
      roiRectH = layerHeight;
      layerRotate = G2D_CW_ROTATE_ANGLE_000;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layer1AlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);

   /// bitblt 80x80 rotate 90 enable alpha
   for(k = 0; k < 5; k++)
   {
      width = 300;
      height = 300;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("6012: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layer1AlphaEnable = g_layer1AlphaEnable; 
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      //roiRectW = layerWidth;
      //roiRectH = layerHeight;
      roiRectW = layerHeight;
      roiRectH = layerWidth;
      layerRotate = G2D_CW_ROTATE_ANGLE_090;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layer1AlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);
      overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);   
   
   /// bitblt 120x120 rotate 0 enable alpha
   for(k = 0; k < 5; k++)
   {
      width = 120;
      height = 120;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("6096: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layer1AlphaEnable = g_layer1AlphaEnable; 
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      roiRectW = layerWidth;
      roiRectH = layerHeight;
      layerRotate = G2D_CW_ROTATE_ANGLE_000;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layer1AlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);
   
   /// bitblt 120x180 rotate 90 enable alpha
   for(k = 0; k < 5; k++)
   {
      width = 120;
      height = 120;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         G2D_LOGI("6179: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      //hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstHeight, dstWidth, G2D_COLOR_FORMAT_RGB565);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layer1AlphaEnable = g_layer1AlphaEnable; 
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      //roiRectW = layerWidth;
      //roiRectH = layerHeight;
      roiRectW = layerHeight;
      roiRectH = layerWidth;
      layerRotate = G2D_CW_ROTATE_ANGLE_090;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layer1AlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);
      //violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);
   G2D_LOGI("g2d_overlay_performance_test_PM_Require test end\n");
}

uint32_t overlay_total_time;

void g2d_overlay_performance_test_PM_Require_2Buffers(void)
{

   g2d_handle_t *g2dHandle;

   uint32_t width, height;
   uint8_t *layerAddr = (uint8_t *)&layer0_image[0];
   g2d_color_format_t layerColorFormat;
   uint32_t layerWidth;
   uint32_t layerHeight;
   uint32_t layerRectW;
   uint32_t layerRectH;
   bool layerSrcKeyEnable = KAL_FALSE;
   uint32_t layerSrcKeyValue = 0xFFFFFFFF;
   bool layer1SrcKeyEnable = KAL_TRUE;
   uint32_t layer1SrcKeyValue = 0xFFFFFFFF;
   bool layerAlphaEnable;
   uint32_t layerAlphaValue;

   uint32_t bgColor = 0x0;
   bool outputAlphaEnable = KAL_FALSE;
   uint32_t outputAlphaValue = 0xFF;
   int32_t roiRectX;
   int32_t roiRectY;
   uint32_t roiRectW;
   uint32_t roiRectH;

   g2d_color_format_t dstColorFormat;
   int32_t dstRectX;
   int32_t dstRectY;
   uint32_t dstWidth;
   uint32_t dstHeight;
   G2D_CW_ROTATE_ANGLE_ENUM layerRotate;
   bool g_layerAlphaEnable = KAL_FALSE;
   int k;

   {
      int i;
      for(i = 0; i < LAYER0_SIZE; i++)
      {
      	 layer0_image[i] = rand() & 0xFF;
      }	
      
      for(i = 0; i < LAYER1_SIZE; i++)
      {
      	 layer1_image[i] = rand() & 0xFF;
      }
   }
   layerColorFormat = G2D_COLOR_FORMAT_RGB565;
   /// Overlay
   /// bitblt 80x80 rotate 0 enable alpha
   for(k = 0; k < 5; k++)
   {
      width = 50;
      height = 50;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         //G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&layer0_image[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      roiRectW = layerWidth;
      roiRectH = layerHeight;
      layerRotate = G2D_CW_ROTATE_ANGLE_000;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layer1SrcKeyEnable, layer1SrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);

   /// bitblt 80x80 rotate 90 enable alpha
   for(k = 0; k < 5; k++)
   {
      width = 50;
      height = 50;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         //G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&layer0_image[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      //roiRectW = layerWidth;
      //roiRectH = layerHeight;
      roiRectW = layerHeight;
      roiRectH = layerWidth;
      layerRotate = G2D_CW_ROTATE_ANGLE_090;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layer1SrcKeyEnable, layer1SrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);
      overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);

   for(k = 0; k < 5; k++)
   {
      width = 100;
      height = 100;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         //G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&layer0_image[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      roiRectW = layerWidth;
      roiRectH = layerHeight;
      layerRotate = G2D_CW_ROTATE_ANGLE_000;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layer1SrcKeyEnable, layer1SrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);

   /// bitblt 80x80 rotate 90 enable alpha
   for(k = 0; k < 5; k++)
   {
      width = 100;
      height = 100;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         //G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&layer0_image[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      //roiRectW = layerWidth;
      //roiRectH = layerHeight;
      roiRectW = layerHeight;
      roiRectH = layerWidth;
      layerRotate = G2D_CW_ROTATE_ANGLE_090;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layer1SrcKeyEnable, layer1SrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);
      overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);



   for(k = 0; k < 5; k++)
   {
      width = 200;
      height = 200;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         //G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&layer0_image[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      roiRectW = layerWidth;
      roiRectH = layerHeight;
      layerRotate = G2D_CW_ROTATE_ANGLE_000;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layer1SrcKeyEnable, layer1SrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);

   /// bitblt 80x80 rotate 90 enable alpha
   for(k = 0; k < 5; k++)
   {
      width = 200;
      height = 200;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         //G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&layer0_image[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      //roiRectW = layerWidth;
      //roiRectH = layerHeight;
      roiRectW = layerHeight;
      roiRectH = layerWidth;
      layerRotate = G2D_CW_ROTATE_ANGLE_090;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layer1SrcKeyEnable, layer1SrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);
      overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);

   for(k = 0; k < 5; k++)
   {
      width = 300;
      height = 300;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         //G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&layer0_image[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      roiRectW = layerWidth;
      roiRectH = layerHeight;
      layerRotate = G2D_CW_ROTATE_ANGLE_000;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layer1SrcKeyEnable, layer1SrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);

   /// bitblt 80x80 rotate 90 enable alpha
   for(k = 0; k < 5; k++)
   {
      width = 75;
      height = 75;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         //G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&layer0_image[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      //roiRectW = layerWidth;
      //roiRectH = layerHeight;
      roiRectW = layerHeight;
      roiRectH = layerWidth;
      layerRotate = G2D_CW_ROTATE_ANGLE_090;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layer1SrcKeyEnable, layer1SrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);
      overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);   

   /// bitblt 120x120 rotate 0 enable alpha
   for(k = 0; k < 5; k++)
   {
      width = 120;
      height = 120;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         //G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&layer0_image[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      roiRectW = layerWidth;
      roiRectH = layerHeight;
      layerRotate = G2D_CW_ROTATE_ANGLE_000;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layer1SrcKeyEnable, layer1SrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);


   /// bitblt 120x120 rotate 90 enable alpha
   for(k = 0; k < 5; k++)
   {
      width = 120;
      height = 120;

      dstColorFormat = G2D_COLOR_FORMAT_RGB565;
      dstRectX = 0;
      dstRectY = 0;
      dstWidth = width;
      dstHeight = height;
      memset(dst_hw_image_120X120, 0xCD, dstWidth*dstHeight*4);


      /// G2D_STATUS_BUSY means someone is using G2D
      if(G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, G2D_CODEC_TYPE_HW, G2D_GET_HANDLE_MODE_DIRECT_RETURN_HANDLE))
      {
         //G2D_LOGI("\tError: What are you doing!! I'm busy now.\n\r");
         return;
      }

      hal_g2d_set_callback_function(g2dHandle, NULL);
      g2dOverlaySetDstWindow(g2dHandle, dstRectX, dstRectY);
      //hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&dst_hw_image_120X120[0], DST_HW_BUFFER_SIZE, dstWidth, dstHeight, dstColorFormat);
      hal_set_destination_rgb_buffer_information(g2dHandle, (uint8_t *)&layer0_image[0], DST_HW_BUFFER_SIZE, dstHeight, dstWidth, G2D_COLOR_FORMAT_RGB565);
      hal_g2d_set_color_replacement(g2dHandle, KAL_FALSE, 0, 255, 0, 0, 0, 0, 0, 255);
      hal_g2d_set_destination_clip_window(g2dHandle, KAL_FALSE, 0, 0, dstWidth, dstHeight);

      g2dSetTileSize(g2dHandle, KAL_FALSE, G2D_TILE_SIZE_TYPE_8x8);
      g2dSetWriteBurstType(g2dHandle, G2D_WRITE_BURST_TYPE_16);
      g2dSetReadBurstType(g2dHandle, G2D_READ_BURST_TYPE_8);

      // layer 0
      //layerColorFormat = G2D_COLOR_FORMAT_ARGB8888;
      layerWidth = width;
      layerHeight = height;
      layerRectW = width;
      layerRectH = height;
      layerAlphaEnable = g_layerAlphaEnable;
      layerAlphaValue = 0x56;

      roiRectX = 0;
      roiRectY = 0;
      //roiRectW = layerWidth;
      //roiRectH = layerHeight;
      roiRectW = layerHeight;
      roiRectH = layerWidth;
      layerRotate = G2D_CW_ROTATE_ANGLE_090;

      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER0, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER0, (uint8_t *)&layer0_image[0], 120 * 120 * 2, layerWidth, layerHeight, layerColorFormat);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER0, 0, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER0, layerSrcKeyEnable, layerSrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER0, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER0, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER0, layerRotate);


      g2dOverlaySetLayerFunction(g2dHandle, G2D_OVERLAY_LAYER1, G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
      g2dOverlaySetLayerRGBBufferInfo(g2dHandle, G2D_OVERLAY_LAYER1, (uint8_t *)&layer1_image[0], 120 * 120 * 2, layerWidth, layerHeight, G2D_COLOR_FORMAT_RGB565);
      g2dOverlaySetLayerWindow(g2dHandle, G2D_OVERLAY_LAYER1, 30, 0, layerRectW, layerRectH);
      g2dOverlaySetLayerSrcKey(g2dHandle, G2D_OVERLAY_LAYER1, layer1SrcKeyEnable, layer1SrcKeyValue);
      g2dOverlaySetLayerAlpha(g2dHandle, G2D_OVERLAY_LAYER1, layerAlphaEnable, layerAlphaValue);

      g2dOverlayEnableLayer(g2dHandle, G2D_OVERLAY_LAYER1, KAL_TRUE);
      g2dOverlaySetLayerRotation(g2dHandle, G2D_OVERLAY_LAYER1, layerRotate);

      g2dOverlaySetBgColor(g2dHandle, bgColor);
      g2dOverlaySetDstAlpha(g2dHandle, outputAlphaEnable, outputAlphaValue);
      g2dOverlaySetRoiWindow(g2dHandle, roiRectX, roiRectY, roiRectW, roiRectH);

      overlay_total_time = 0;
      violet_time1 = drv_get_current_time();
      g2dOverlayStart(g2dHandle);
      while(hal_g2d_get_status(g2dHandle)) {};
      violet_time2 = drv_get_current_time();
      //overlay_total_time += drv_get_duration_tick(violet_time1, violet_time2);
      violet_time[violet_count++]= drv_get_duration_tick(violet_time1, violet_time2);
      //violet_time[violet_count++] = drv_get_duration_tick(violet_time1, violet_time2);

      hal_g2d_release_handle(g2dHandle);
   }
   //violet_time[violet_count++] = (overlay_total_time / 10);
   
}



