#ifdef CONFIG_INCLUDE_HEADER
#include "gnss_demo.h"
#include "bt_audio.h"
#include "sensor_demo.h"
#include "ble_pxp_main.h"
#include "wf_image.h"
#endif

#ifdef CONFIG_INCLUD_BODY
// add your screen here
{show_gnss_screen, gnss_event_handler, "GNSS demo"},
{show_bt_audio_screen, bt_audio_event_handler, "BT Audio"},
{ble_pxp_show_unconnected_screen, ble_pxp_screen_event_handler, "BLE PXP"},
#ifdef SENSOR_DEMO
{show_sensor_ready_to_connect_screen, sensor_event_handler, "Sensor Subsystem"},
#endif
{wf_app_task_enable_show, NULL, "Watch Face"},

#endif

#ifdef CONFIG_MESSAGE_ID_ENUM
// define your own message bellowing here
//GNSS message
MESSAGE_ID_GNSS_POSITION,
MESSAGE_ID_GNSS_NMEA,
MESSAGE_ID_BT_AUDIO,
#endif

