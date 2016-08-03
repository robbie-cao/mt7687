#ifndef __HR_FET_PRIVATE_H__
#define __HR_FET_PRIVATE_H__

#ifdef _cpluscplus
extern "C"{
#endif

/* MODE 0: Run by Matlab */
/* MODE 1: RUN at Linux TS*/
#ifndef RUN_MODE
  #define RUN_MODE                   1
#endif

#if (RUN_MODE == 0)
#endif

#if (RUN_MODE == 0)
  #define MATLAB_FILEOUT           1
  #define MATLAB_DBG_SHOW_PARA     0
  #define MATLAB_DBG_SHOW_RESULTS  0
  //-----------------------------------------
  // definition for debug message
  //-----------------------------------------
  #define MATLAB_DBG_MSG           1
#elif (RUN_MODE == 1)
  #define MATLAB_FILEOUT           0
  #define MATLAB_DBG_SHOW_PARA     0
  #define MATLAB_DBG_SHOW_RESULTS  0
  //-----------------------------------------
  // definition for debug message
  //-----------------------------------------
  #define MATLAB_DBG_MSG           0
#endif

#define VIRTUAL_FET_QUEUE_SIZE     512
#define FET_QUEUE_SIZE             60
#define FET_SAFE_SIZE              (FET_QUEUE_SIZE-20)

#define CACHE_QUEUE_SIZE           64
#define CACHE_SAFE_SIZE            (CACHE_QUEUE_SIZE-20)

#define NELET(a)                   (sizeof(a)/sizeof(a[0]))

//---------------------------------------------------------
// type definition
//---------------------------------------------------------
#define PPG_TYPE                  0
#define BISI_TYPE                  1
#define NONE_STATE                 -99

#define TRUE                       1
#define FALSE                      0

#if 1
/* just for user reference */
typedef signed char		      int8_t;
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef short               int16_t;
typedef int                 int32_t;
#endif

//---------------------------------------------------------
// sleep detection status definition
//---------------------------------------------------------
#define HR_FET_STATUS_OK               0
#define HR_FET_STATUS_ERROR           -1
#define HR_FET_VERSION               "v1"
#define HR_FET_MODEL                  2

//---------------------------------------------------------
// features ID
//---------------------------------------------------------
#define   HR_FET_BEAT_TIME_ID        0x21
#define   HR_FET_BEAT_VALUE_ID       0x22
//---------------------------------------------------------
// enable features for falling asleep detection
//---------------------------------------------------------

//V20:change to 5
#define PPG_FREQUENCY                125 /* Hz */
#define PPG_DTIME                   (1000/PPG_FREQUENCY) /*(int)(1000/PPG_FREQUENCY), millisec*/
#define IN_SAMPLE_DLY_MARGIN         2
#define FET_TIME                     PPG_FREQUENCY
#define FET_NUM                      10
//---------------------------------------------------------
// This structure defines queue unit
//---------------------------------------------------------
typedef struct {
  int32_t       value[FET_QUEUE_SIZE]; /**<  feature value */
  unsigned int  timestamp[FET_QUEUE_SIZE]; /**<  feature timestamp */
  unsigned char idx; /**<  current queue write index */
} fqueue_t;

typedef struct {
  int32_t       value[CACHE_QUEUE_SIZE]; /**<  feature value */
  unsigned int  timestamp[CACHE_QUEUE_SIZE]; /**<  feature timestamp */
  unsigned char idx; /**<  current queue write index */
} cqueue_t;
//---------------------------------------------------------
// This structure defines the results from hr_fet
//---------------------------------------------------------

typedef struct{
	unsigned char  init;// default: 0
  unsigned int   prev_timestamp;
  cqueue_t       ppg_cache;
  fqueue_t       hr_fet;
  uint8_t        hr_fet_load_idx;
}hr_fet_para_t;

//---------------------------------------------------------
// functions for heart rate features extraction
//---------------------------------------------------------
int    hr_fet_init(void);
int    hr_fet_ext(int ppg_bisi, unsigned int timestamp, uint8_t sensor_type);
int    check_sleep_hr_fet(void);
int    get_one_hr_fet(int *results, unsigned int *results_timestamp);
int    rearrange_queue_for_hr_fet(void);


void   hr_fet_proc_run(int *ppg_bisi, int *sensor_type, unsigned int *sensor_time, int loop, int* hr_fet, unsigned int *hr_fet_time);

#ifdef _cpluscplus
}
#endif


#endif /* __HR_FET_PRIVATE_H__ */
