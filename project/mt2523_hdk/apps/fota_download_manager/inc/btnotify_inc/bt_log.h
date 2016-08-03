
#ifndef __BT_LOG_H__
#define __BT_LOG_H__

#include "syslog.h"

#ifdef __cplusplus
extern "C" {
#endif



#define log_bt_create(_level) log_create_module(bt, (_level))
#define log_bt_fatal(_message,...) LOG_E(bt, (_message), ##__VA_ARGS__)
#define log_bt_error(_message,...) LOG_W(bt, (_message), ##__VA_ARGS__)
#define log_bt_info(_message,...) LOG_I(bt, (_message), ##__VA_ARGS__)
#define log_bt_dump(_data, _len, _message, ...) module_dump(bt, (_data), (_len), (_message), ##__VA_ARGS__)
#define log_bt_config_print_switch(_switch) log_config_print_switch(bt, (_switch))
#define log_bt_config_print_level(_level) log_config_print_level(bt, (_level))
#define log_bt_config_print_func(_print_func) log_config_print_func(bt, (_print_func))
#define log_bt_config_dump_func(_dump_func) log_config_dump_func(bt, (_dump_func))


#ifdef __cplusplus
}
#endif

#endif /* __BT_LOG_H__ */

