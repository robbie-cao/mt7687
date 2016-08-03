
#include "ring_buf.h"
#include <string.h>

// this implementation can not be used by multi-task. if you want, you need use mutex.
// but this implementation can be read by one task, and write by the other task.

void ring_buf_init(ring_buf_struct_t *ring_buf, int8_t* buf, int16_t buf_size)
{
    ring_buf->buf_size = buf_size;
    ring_buf->buf = buf;
    ring_buf->start_ind = ring_buf->end_ind = 0;
}


int32_t is_empty(ring_buf_struct_t *ring_buf)
{
    return ring_buf->end_ind == ring_buf->start_ind;
}


int32_t put_data(ring_buf_struct_t *ring_buf, int8_t* buf, int32_t buf_len)
{
    if (buf_len + ring_buf->end_ind >= ring_buf->buf_size){
        memcpy(ring_buf->buf + ring_buf->end_ind, buf, ring_buf->buf_size - ring_buf->end_ind);
        memcpy(ring_buf->buf, buf + ring_buf->buf_size - ring_buf->end_ind, buf_len - (ring_buf->buf_size - ring_buf->end_ind));
        ring_buf->end_ind = ring_buf->end_ind + buf_len - ring_buf->buf_size;
    } else {
        memcpy(ring_buf->buf + ring_buf->end_ind, buf, buf_len);
        ring_buf->end_ind += buf_len;
    }
    return buf_len;
}

int32_t consume_data(ring_buf_struct_t *ring_buf, int8_t* buf, int32_t buf_len)
{
    int32_t consume_len;
    int32_t end_ind  = ring_buf->end_ind;
    if (end_ind == ring_buf->start_ind) {
        return 0;
    }

    if (ring_buf->start_ind < end_ind) {
        if (end_ind - ring_buf->start_ind > buf_len) {
            consume_len = buf_len;
            memcpy(buf, ring_buf->buf, buf_len);
            ring_buf->start_ind += buf_len;
        } else {
            consume_len = end_ind - ring_buf->start_ind;
            memcpy(buf, ring_buf->buf, consume_len);
            ring_buf->start_ind = end_ind;
        }
    } else {
        if (ring_buf->buf_size - ring_buf->start_ind > buf_len) {
            consume_len = buf_len;
            memcpy(buf, ring_buf->buf, buf_len);
            ring_buf->start_ind += buf_len;
        } else {
            int32_t copyed_len = ring_buf->buf_size - ring_buf->start_ind;
            memcpy(buf, ring_buf->buf + ring_buf->start_ind, copyed_len);
            ring_buf->start_ind = 0;
            if (end_ind > buf_len - copyed_len) {
                consume_len = buf_len;
                memcpy(buf + copyed_len, ring_buf->buf, buf_len - copyed_len);
                ring_buf->start_ind += buf_len;
            } else {
                consume_len = end_ind;
                memcpy(buf + copyed_len, ring_buf->buf, end_ind);
                ring_buf->start_ind = end_ind;
            }
        }
    }
    return consume_len;
}
