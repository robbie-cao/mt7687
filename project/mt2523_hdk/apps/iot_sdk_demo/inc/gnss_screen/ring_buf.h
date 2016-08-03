#ifndef __RING_BUF_H__
#define __RING_BUF_H__
#include <stdint.h>
typedef struct ring_buf_sturct{
    int8_t* buf;
    int16_t buf_size;
    int16_t start_ind;
    int16_t end_ind;
} ring_buf_struct_t;

void ring_buf_init(ring_buf_struct_t *ring_buf, int8_t* buf, int16_t buf_size);
int32_t is_empty(ring_buf_struct_t *ring_buf);
int32_t put_data(ring_buf_struct_t *ring_buf, int8_t* buf, int32_t buf_len);
int32_t consume_data(ring_buf_struct_t *ring_buf, int8_t* buf, int32_t buf_len);
#endif
