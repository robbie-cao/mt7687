#ifndef __MSBC_CODEC_H__
#define __MSBC_CODEC_H__

#include <stdint.h>

uint32_t msbc_decoder_get_buffer_size (void);
uint32_t msbc_encoder_get_buffer_size (void);
int32_t msbc_decode_init (void **handle_pointer, uint8_t *internal_buffer);
int32_t msbc_encode_init (void **handle_pointer, uint8_t *internal_buffer);
int32_t msbc_decode_process (void *hdl, uint8_t *p_in_buf, uint32_t *p_in_byte_cnt, int16_t *p_ou_buf, uint32_t *p_ou_byte_cnt);
int32_t msbc_encode_process (void *hdl, int16_t *p_in_buf, uint32_t *p_in_byte_cnt, uint8_t *p_ou_buf, uint32_t *p_ou_byte_cnt);

#endif  /* __MSBC_CODEC_H__ */
