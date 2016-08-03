#ifndef __HR_APP_H__
#define __HR_APP_H__

#include "hr_client.h"

#include <stdint.h>
#include <stdbool.h>
#include <FreeRTOS.h>

#include "queue.h"
#include "task.h"

#ifdef __BLE_GATTC_SRV__

#define SRV_HR_SERVICE_MAX_NUM  2

#define SRV_HR_MAX_DES_NUM  1
#ifdef __GATTC_SRV_TEST__
#define SRV_HRM_SER_UUID    0x1809

#else
#define SRV_HRM_SER_UUID    0x180D
#endif
#define SRV_DIS_UUID    0x180A
#define SRV_DIS_MAX_CHAR    9

/*app 's op state*/
typedef enum {
    GETTING_ENABLE,
    GETTING_CONNECT,
    GETTING_SERVICE,
    GETTING_CHAR,
    GETTING_DESCR,
    READING_DESCR,
    WRITING_DESCR,
    WRITING_DONE,
} app_hrp_in_state_enum;

/*primary service information saved in app*/
typedef struct {
    bool is_smatch;  // srv is match;
    gatt_srv_uuid_t srv_id;// srv uuid ;
} app_hrp_service_info_struct;

/*characteristic information saved in app*/
typedef struct {
    bool is_cvalid;//charactistic match
    bool has_descr;
    bool des_writed;
    bool des_read;
    gatt_uuid_t char_uuid;
    gatt_uuid_t descr_uuid;
    uint8_t descr_value[256];
} app_hrp_char_info_struct;

/*heartrate service's characteristics*/
typedef enum {
    HEART_RATE_MEASURE,
    BODY_SENSOR_LOCATION,
    HRART_RATE_CNT_POINT,
    HRART_RATE_TYPE_TOTAL
} srv_hrsp_char_type_enum;

/*heartrate profile's information*/
typedef struct {
    uint16_t conn_id;
    uint8_t dev_flag;/*address has exist*/
    uint8_t index; /*device's index*/
    ble_address_t adr; /*bt addreess*/
    app_hrp_in_state_enum status;// current status
    app_hrp_service_info_struct srv_info[SRV_HR_SERVICE_MAX_NUM];
    app_hrp_char_info_struct char_info[HRART_RATE_TYPE_TOTAL + 1];
    app_hrp_char_info_struct dis_char_info[SRV_DIS_MAX_CHAR];
} app_hrp_info_dev_struct;

typedef enum {
    TYPE_SRV,
    TYPE_CHAR,
    TYPE_DESC
} app_decl_type_enum;

/*device inforamtion saved in app*/
typedef struct {
    void *reg_ctx;
    uint8_t ctotal_count; /*current link number*/
    app_hrp_info_dev_struct dev_info[SRV_MAX_DEV];
} app_hrp_context_struct;

typedef enum {
    BY_ADDRESS,
    BY_CONN_ID,
    BY_INDEX
} app_query_info_type_enum;

/*characteristic */
typedef enum {
#ifdef __GATTC_SRV_TEST__
    HRM_CHAR_UUID = 0x2A1C,
#else
    HRM_CHAR_UUID = 0x2A37,/*only need change this char*/
#endif
    HBL_CHAR_UUID,
    HCP_CHAR_UUID,
    HR_CHAR_TOTAL
} char_uuid_enum;

typedef enum {
    CLIENT_CONFI_DESCRI = 0x2902,
    CLIENT_CONFI_TOTAL
} desc_uuid_enum;

/*heat rate's register*/
void heart_rate_init(void);

/*heat rate's deregister*/
void heart_rate_deinit(void);
typedef struct {
    ble_event_t event_id;
    int8_t param[512];
} hear_rate_message_struct;

extern hear_rate_message_struct app_queue_data;
extern QueueHandle_t app_queue;

void ble_hrc_app_callback(hear_rate_message_struct *msg);

#endif /*__BLE_GATTC_SRV__*/
#endif /*__HR_APP_H__*/
