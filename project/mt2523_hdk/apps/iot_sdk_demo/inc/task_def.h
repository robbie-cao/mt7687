#ifndef __TASK_DEF_H__
#define __TASK_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
 * API Functions
 *****************************************************************************/

void task_def_init(void);

void task_def_create(void);

void task_def_delete_wo_curr_task(void);
#ifdef __cplusplus
}
#endif

#endif /* __TASK_DEF_H__ */
