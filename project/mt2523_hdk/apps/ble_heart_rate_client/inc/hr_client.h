#ifndef __HR_CLIENT_H__
#define __HR_CLIENT_H__
#include <stdbool.h>
#include <stdint.h>
#include "ble_gattc_srv.h"
#include "ble_gap.h"
#include "hr_app.h"

#ifdef __BLE_GATTC_SRV__
typedef struct {
    uint8_t flag;
    uint16_t val;
    uint16_t en_expend;
    uint16_t RR_inteval;
} hr_data_t;

typedef struct {
    uint8_t len;
    uint16_t desc_value;
} descr_cfg_t;

/* authentication requirement */
typedef uint8_t GATTC_AUTH_REQ_TYPE;
#define GATTC_AUTH_REQ_TYPE_NONE              0
#define GATTC_AUTH_REQ_TYPE_NO_MITM           1   /* unauthenticated encryption */
#define GATTC_AUTH_REQ__TYPE_MITM              2   /* authenticated encryption */
#define GATTC_AUTH_REQ_TYPE_SIGNED_NO_MITM    3
#define GATTC_AUTH_REQ_TYPE_SIGNED_MITM       4


uint16_t gattc_convert_srv_uuid_to_u16(gatt_uuid_t *uu);

void gattc_decode_char_data(gattc_value_t *value, hr_data_t *data);

void gattc_config_encode_data(descr_cfg_t *desc_value, gattc_value_t *value);

uint16_t gattc_convert_array_to_uuid16(app_uuid_t *uu);

bool gattc_decode_scan_data(uint8_t *data, uint16_t len);
#endif
#endif
