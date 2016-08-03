#ifndef __CVSD_CODEC_H__
#define __CVSD_CODEC_H__

#include <stdint.h>

/*
    Output Parameter
        internal_buffer_size    CVSD decoder internal buffer size, unit: byte
*/

uint32_t cvsd_decoder_get_buffer_size (void);

/*
    Output Parameter
        internal_buffer_size    CVSD encoder internal buffer size, unit: byte
*/

uint32_t cvsd_encoder_get_buffer_size (void);

/*
    Input Parameters
        internal_buffer     CVSD decoder internal buffer
    Output Parameters
        handle_pointer      pointer to the CVSD decoder handle
    Return Value
        >= 0    Normal
        < 0     Error
*/

int32_t cvsd_decode_init (void **handle_pointer, uint8_t *internal_buffer);

/*
    Input Parameters
        internal_buffer     CVSD encoder internal buffer
    Output Parameters
        handle_pointer      pointer to the CVSD encoder handle
    Return Value
        >= 0    Normal
        < 0     Error
*/

int32_t cvsd_encode_init (void **handle_pointer, uint8_t *internal_buffer);

/*
    Input Parameters
        handle              CVSD decoder handle
        input_bitstream     pointer to the input bitstream buffer
        input_byte_count    available input bitstream buffer size, unit: byte
        output_pcm          pointer to the output PCM buffer
        output_byte_count   available output PCM buffer size, unit: byte
    Output Parameters
        input_byte_count    consumed input bitstream buffer size, unit: byte
        output_byte_count   produced output PCM buffer size, unit: byte
    Return Value
        >= 0    Normal
        < 0     Error
*/

int32_t cvsd_decode_process (
    void *handle,
    uint8_t *input_bitstream,
    uint32_t *input_byte_count,
    int16_t *output_pcm,
    uint32_t *output_byte_count
);

/*
    Input Parameters
        handle              CVSD encoder handle
        input_pcm           pointer to the input PCM buffer
        input_byte_count    available input PCM buffer size, unit: byte
        output_bitstream    pointer to the output bitstream buffer
        output_byte_count   available output bitstream buffer size, unit: byte
    Output Parameters
        input_byte_count    consumed input PCM buffer size, unit: byte
        output_byte_count   produced output bitstream buffer size, unit: byte
    Return Value
        >= 0    Normal
        < 0     Error
*/

int32_t cvsd_encode_process (
    void *handle,
    int16_t *input_pcm,
    uint32_t *input_byte_count,
    uint8_t *output_bitstream,
    uint32_t *output_byte_count
);

uint32_t cvsd_src_down_sampling_get_buffer_size (void);
uint32_t cvsd_src_up_sampling_get_buffer_size   (void);
int32_t cvsd_src_down_sampling_init (void **handle_pointer, uint8_t *internal_buffer);
int32_t cvsd_src_up_sampling_init   (void **handle_pointer, uint8_t *internal_buffer);
int32_t cvsd_src_down_sampling_process (void *hdl, int16_t *p_tmp_buf, int16_t *p_in_buf, int16_t *p_ou_buf, uint32_t in_byte_cnt);
int32_t cvsd_src_up_sampling_process   (void *hdl, int16_t *p_tmp_buf, int16_t *p_in_buf, int16_t *p_ou_buf, uint32_t in_byte_cnt);

#endif  /* __CVSD_CODEC_H__ */
