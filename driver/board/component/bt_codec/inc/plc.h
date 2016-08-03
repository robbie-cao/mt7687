#ifndef __PLC_H__
#define __PLC_H__

#include <stdint.h>

typedef struct {
    uint32_t sampling_rate;         /* Unit: Hz
                                       8000   --> Narrow Band Speech Signal
                                       16000  --> Wide Band Speech Signal
                                       Others --> Undefined */
} plc_initial_parameter_t;

typedef struct {
    uint32_t bad_frame_indicator;   /* 1      --> Bad Frame
                                       0      --> Good Frame
                                       Others --> Undefined */
    uint32_t bt_ev3_half_bad_flag;  /* 1      --> BT EV3 Packet Half Bad Frame
                                       0      --> Normal
                                       Others --> Undefined */
} plc_runtime_parameter_t;

/*
    Output Parameter
        internal_buffer_size    PLC internal buffer size, unit: byte
*/
uint32_t plc_get_buffer_size (void);

/*
    Input Parameters
        internal_buffer     PLC internal buffer
        initial_parameter   PLC initial parameters
    Output Parameters
        handle_pointer      pointer to the PLC handle
    Return Value
        >= 0    Normal
        < 0     Error
*/
int32_t  plc_init (void **handle_pointer, uint8_t *internal_buffer, plc_initial_parameter_t *initial_parameter);

/*
    Input Parameters
        handle              PLC handle
        pcm_data            pointer to the PCM buffer
        runtime_parameter   PLC runtime parameters
    Return Value
        >= 0    Normal
        < 0     Error
*/
int32_t  plc_process (void *handle, int16_t *pcm_data, plc_runtime_parameter_t *runtime_parameter);

#endif  /* __PLC_H__ */
