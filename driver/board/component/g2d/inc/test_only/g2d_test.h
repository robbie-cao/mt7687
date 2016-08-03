#ifndef __G2D_TEST_H__
#define __G2D_TEST_H__

#define LAYER0_SIZE (120*120*4)
#define LAYER1_SIZE (120*120*4)

#define DST_HW_BUFFER_SIZE (120*120*4)
#define DST_SW_BUFFER_SIZE (120*120*4)


///#include "g2d_drv_c_model_6276_series.h"
///
///extern void g2d_compare_sw_hw_result(void);
///extern void g2d_set_registers_to_c_model(G2D_REG* reg, unsigned char *dstBuf);
///extern void g2d_c_model(G2D_REG* reg);


//extern void dbg_print(char *fmt,...);;

extern void g2d_overlay_tile_scan_test(void);
extern void g2d_overlay_layer_color_format_test(void);
extern void g2d_overlay_layer_source_color_key_test(void);
extern void g2d_overlay_layer_rotation_test(void);
extern void g2d_overlay_layer_alpha_blending_test(void);
extern void g2d_overlay_layer_coordinate_test(void);
extern void g2d_overlay_layer_size_test(void);
extern void g2d_overlay_layer_pitch_test(void);
extern void g2d_overlay_layer_address_test(void);
extern void g2d_overlay_layer_rectangle_fill_test(void);
extern void g2d_overlay_layer_normal_font_test(void);
extern void g2d_overlay_layer_aa_font_test(void);
extern void g2d_overlay_roi_clipping_window_test(void);
extern void g2d_overlay_roi_memory_out_color_format_test(void);
extern void g2d_overlay_roi_color_replacement_test(void);
extern void g2d_overlay_roi_backgroud_color_test(void);
extern void g2d_overlay_roi_output_constant_alpha_test(void);
extern void g2d_overlay_roi_memory_coordinate_test(void);
extern void g2d_overlay_all_scenario_combination_test(void);
extern void g2d_overlay_roi_dithering_test(void);

extern void g2d_sad_basic_test(void);
extern void g2d_sad_coordinate_test(void);
extern void g2d_sad_size_test(void);
extern void g2d_sad_pitch_test(void);


extern void g2d_lt_basic_test(void);
extern void g2d_lt_tile_scan_test(void);
extern void g2d_lt_burst_cache_test(void);
extern void g2d_lt_color_format_test(void);
extern void g2d_lt_source_color_key_test(void);
extern void g2d_lt_alpha_blending_test(void);
extern void g2d_lt_roi_coordinate_test(void);
extern void g2d_lt_size_test(void);
extern void g2d_lt_pitch_test(void);
extern void g2d_lt_address_test(void);
extern void g2d_lt_sample_mode_test(void);
extern void g2d_lt_affine_rotate_test(void);
extern void g2d_lt_radom_matrix_test(void);
extern void g2d_lt_WHQA_00009054_test(void);

extern uint32_t drv_get_current_time(void);
extern uint32_t drv_get_duration_tick(uint32_t previous_time, uint32_t current_time);


#endif

