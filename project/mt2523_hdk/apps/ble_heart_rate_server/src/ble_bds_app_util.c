#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "ble_bds_app_util.h"

uint8_t bds_uint16_encode(uint8_t *p_encoded_data, uint16_t *value)
{
    p_encoded_data[0] = (uint8_t) ((*value & 0x00FF) >> 0);
    p_encoded_data[1] = (uint8_t) ((*value & 0xFF00) >> 8);
    return sizeof(uint16_t);
}


uint8_t bds_uint24_encode(uint8_t *p_encoded_data, uint32_t *value)
{
    p_encoded_data[0] = (uint8_t) ((*value & 0x000000FF) >> 0);
    p_encoded_data[1] = (uint8_t) ((*value & 0x0000FF00) >> 8);
    p_encoded_data[2] = (uint8_t) ((*value & 0x00FF0000) >> 16);
    return (3);
}


uint8_t bds_uint40_encode(uint8_t *p_encoded_data, uint64_t *value)
{
    p_encoded_data[0] = (uint8_t) ((*value & 0x00000000000000FF) >> 0);
    p_encoded_data[1] = (uint8_t) ((*value & 0x000000000000FF00) >> 8);
    p_encoded_data[2] = (uint8_t) ((*value & 0x0000000000FF0000) >> 16);
    p_encoded_data[3] = (uint8_t) ((*value & 0x00000000FF000000) >> 24);
    p_encoded_data[4] = (uint8_t) ((*value & 0x000000FF00000000) >> 32);
    return 5;
}


uint8_t bds_ble_srv_utf8_str_encode(uint8_t *p_encoded_data, ble_srv_utf8_str_t *p_value)
{
    memcpy(p_encoded_data, p_value->p_str, p_value->length);
    return p_value->length;
}


uint8_t bds_regcertdatalist_encode(uint8_t *p_encoded_data, regcertdatalist_t *p_value)
{
    memcpy(p_encoded_data, p_value->p_list, p_value->list_len);
    return p_value->list_len;
}


uint8_t bds_uint16_decode(uint16_t *p_decoded_val, uint8_t *p_encoded_data, uint8_t len)
{
    *p_decoded_val = (((uint16_t)((uint8_t *)p_encoded_data)[0])) | 
                     (((uint16_t)((uint8_t *)p_encoded_data)[1]) << 8 );
    return (sizeof(uint16_t));
}

