#ifndef __GNSS_TIMER_H__
#define __GNSS_TIMER_H__

typedef void (*gnss_timer_handle) (int32_t timer_id);

int32_t gnss_start_timer(int32_t msec, gnss_timer_handle handle);
int32_t gnss_start_repeat_timer(int32_t msec, gnss_timer_handle call_back);
void gnss_stop_timer(int32_t tid);
void gnss_timer_init(void);
void excute_timer(void);
void gnss_timer_deinit(void);

#endif
