#ifndef __MOTION_ST_PRIVATE_H__
#define __MOTION_ST_PRIVATE_H__

#ifdef _cpluscplus
extern "C"{
#endif

/* MODE 0: Run by Matlab */
/* MODE 1: RUN at Linux TS*/
#define RUN_MODE                   1

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
  #define MATLAB_DBG_MSG_SE        0
  #define MATLAB_DBG_MSG_FILTER    0
  #define MATLAB_DBG_MSG_WK_ENERGY 0
  #define MATLAB_DBG_MSG_VERSION   0
#elif (RUN_MODE == 1)
  #define MATLAB_FILEOUT           0
  #define MATLAB_DBG_SHOW_PARA     0
  #define MATLAB_DBG_SHOW_RESULTS  0
  //-----------------------------------------
  // definition for debug message
  //-----------------------------------------
  #define MATLAB_DBG_MSG           0
  #define MATLAB_DBG_MSG_SE        0
  #define MATLAB_DBG_MSG_FILTER    0
  #define MATLAB_DBG_MSG_WK        0
  #define MATLAB_DBG_MSG_VERSION   0
#endif

#define EVT_QUEUE_SIZE              64
#define EVT_SAFE_SIZE              (EVT_QUEUE_SIZE-10)
#define SE_QUEUE_SIZE               2
#define SE_TIME_TMP_QUEUE_SIZE      4
#define VIRTUAL_EVT_QUEUE_SIZE      500
#define VIRTUAL_SE_QUEUE_SIZE       20


#define VIRTUAL_FET_QUEUE_SIZE     1500000
#define FET_QUEUE_SIZE             60
#define FET_SAFE_SIZE              (FET_QUEUE_SIZE-20)


#define NELET(a)                   (sizeof(a)/sizeof(a[0]))
#define DUMP_VARIABLE              0
#define DUMP_SLEEP_EFFICIENCY      0
//---------------------------------------------------------
// type definition
//---------------------------------------------------------
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
// sleep state definition
//---------------------------------------------------------
#define AWAKE                      0
#define RESTLESS                   -2
#define ASLEEP                     -3
#define NONE_STATE                 -99
//---------------------------------------------------------
// state for buffer control
//---------------------------------------------------------
/** @brief  This macro defines numeric value for frame control. IC_STATE: Incomplete sleep duration
  */
#define IC_STATE                   -99
/** @brief  This macro defines numeric value for frame control. C_STATE: Complete sleep duration
  */
#define C_STATE                    0
//---------------------------------------------------------
// sleep detection status definition
//---------------------------------------------------------
#define SD_STATUS_OK               0
#define SD_STATUS_ERROR            -1
#define SLEEP_VERSION              "v22"
#define SLEEP_MODEL                2
//---------------------------------------------------------
// features ID
//---------------------------------------------------------
#define   EVENT_ID                 0x00
#define   SE_ID                    0x01

#define   MOTION_FET01_ID          0x11
#define   MOTION_FET02_ID          0x12
#define   MOTION_FET03_ID          0x13
#define   MOTION_FET04_ID          0x14
#define   MOTION_FET05_ID          0x15
#define   MOTION_FET06_ID          0x16
#define   MOTION_FET07_ID          0x17
#define   MOTION_FET08_ID          0x18
#define   MOTION_FET09_ID          0x19
#define   MOTION_FET10_ID          0x1A
#define   MOTION_FET11_ID          0x1B
#define   MOTION_FET12_ID          0x1C
#define   MOTION_FET13_ID          0x1D
#define   MOTION_FET14_ID          0x1E
#define   MOTION_FET15_ID          0x1F
//---------------------------------------------------------
// sleep state definition for false alarm
//---------------------------------------------------------
#define AWAKE_FA                    0
#define PRIOR_AWAKE_FA             -1
#define ASLEEP_FA                  -3

// definitions of posture state
#define MOTION                      0
#define ON_BED                      1 // sit and on-bed are hard to distinguish
#define SIT                         2 // sit and on-bed are hard to distinguish
#define STAND                       3
#define ON_DESK_CAN                 4 // possible to become ondesk
#define ON_DESK                     5 // ondesk
//---------------------------------------------------------
// enable features for falling asleep detection
//---------------------------------------------------------
#define FREQ_INDEX_EN                0
#define SPD_INDEX_EN                 0

//V20:change to 5 down sampling rate (DSR)
#define ACC_DSR                      5

// define DSP math
#define SHIFT_BIT                    10
#define SHIFT_VALUE                  (1<<SHIFT_BIT)
#define Q32_FRAC_BITS                32

#define ONDESK_ACC_PHASE0            0
#define ONDESK_ACC_PHASE1            1
#define ONDESK_ACC_PHASE2            2
#define ACC_FREQUENCY                10 /*(int)(50.0/ACC_DSR)*/
#define ACC_DTIME                    (1000/ACC_FREQUENCY) /*(int)(1000/ACC_FREQUENCY), millisec*/
#define IN_SAMPLE_DLY_MARGIN         2
#define GRAVITY                      9810
#define ACCNORM_BUFFER_SIZE          20  /* (int)(ACC_FREQUENCY*2) */
#define MOTION_WINDOW_SIZE           10  /* (int)(ACC_FREQUENCY) */

// SMD
#define RUN_DOWN_SAMPLE              1
#define WALK_DOWN_SAMPLE             2
#define STILL_DOWN_SAMPLE            8
#define UNKNOWN_DOWN_SAMPLE          -1

#define STILL_THRESHOLD              360000
#define NORM_DIFF_THRESHOLD          2200
#define STILL_NUM_THRESHOLD          100

#define ONDESK                       5
#define ONDESK_CAN                   4
#define NOT_ONDESK                   0
#define ACC_NOISE_BIT                8
#define ACC_NOISE_VALUE              384
#define MAX_SMD                      10

#define ROTATE_BUFFER_SIZE           (5)
#define ROTATE_SUM_TH                (6)
#define ROTATE_SUM_MAX               65536

//Filter out too close sleep event, 2 minutes = 120 sec = 120000 msec
#define ONBED_INTERVAL_FILTER        120000

#define COMPUTE_NUM                  (60*ACC_FREQUENCY)
#define COMPUTE_TIME                 (60*ACC_FREQUENCY)
#define FET_TIME                     ACC_FREQUENCY

// standard deviation by event
//keep from overflow, 2^30 = 1073741824
#define STD_EVT_ACC_MAX              1073741824
//keep from overflow, 2^20 = 1048576
#define STD_EVT_MAX                  1048576

//average every 5 minutes = 300 sec * 10.0 Hz
//#define STD_EVT_ACC_SEG              (600*ACC_FREQUENCY)

//average every 5 minutes = 1800 sec * 10.0 Hz
#define STD_EVT_ACC_SEG              (1800*ACC_FREQUENCY)

// larger std event as you shift more bits
//#define STD_EVT_SHIFT_BIT            4
// for 1-hour, 3600 seconds is 3600*10 = 36000 samples = shift 15 bits (32768)
// for 0.5-hour, 1800 seconds is 1800*10 = 18000 samples = shift 14 bits (16384)
#define HOUR_SHIFT_BIT               14
#define HOUR_NUM                     (1800*ACC_FREQUENCY)

//#define STD_EVT_BASE                 (1<<STD_EVT_SHIFT_BIT)
// truncate 5 bits, 2^5 = 32
#define ACCSTD_SHIFT_BIT             5

//average every 10 minutes = 600 sec * 10.0 Hz
#define POST_RESTLESS_COUNTDOWN     (600*ACC_FREQUENCY)


#define FET_NUM                      16
//---------------------------------------------------------
// This structure defines queue unit
//---------------------------------------------------------
typedef struct {
  int32_t      value[FET_QUEUE_SIZE]; /**<  feature value */
  unsigned int  timestamp[FET_QUEUE_SIZE]; /**<  feature timestamp */
  unsigned char idx; /**<  current queue write index */
} fqueue_t;

typedef struct {
  int8_t        value[EVT_QUEUE_SIZE]; /**<  event value */
  unsigned int  timestamp[EVT_QUEUE_SIZE]; /**<  event timestamp */
  unsigned char idx; /**<  current queue write index */
} lqueue_t;

/** @brief  Structure for short queue, which is used for storing sleep efficiency
  */
typedef struct {
  int8_t        value[SE_QUEUE_SIZE]; /**<  sleep efficiency value */
  unsigned int  timestamp[SE_QUEUE_SIZE]; /**<  on-bed timestamp */
  unsigned char idx; /**<  current write index of queue  */
} squeue_t;

//---------------------------------------------------------
// This structure defines the results from motion_st
//---------------------------------------------------------

typedef struct{
	int8_t         preSig_h;
	int8_t         motion_h;
	int8_t         stillstate_detect;
	int8_t         cur_evt;

	unsigned char  init;// default: 0
  unsigned char  awake_index;

	unsigned short timeCounter_h; // for 10 Hz, 2 bytes can count 6553 sec
	unsigned short timeCounter_l; // for 10 Hz, 2 bytes can count 6553 sec
	unsigned short pre_sleep_counter;//count the duration before sleep, change from sleep_counter
	unsigned short tsmd; // for 10 Hz, 2 bytes can count 6553 sec until overflow
  unsigned short tsmd_block[5]; // for 10 Hz, 2 bytes can count 6553 sec until overflow

  short          vertical_weight;
	short          pre_vertical_weight_out;
	int8_t         se_eva;
	int            rotate_sum;

	unsigned int   wakeup_evt_time_tmp; // temporary wakeup_evt_time
	unsigned char  awake_duration; // less than 255

  #if (RUN_MODE == 0)
    unsigned int smd_h_one_time[3];
  #endif
	unsigned int   se_start;
	unsigned int   se_numerator;
	unsigned int   se_time_tmp[SE_TIME_TMP_QUEUE_SIZE];
	int8_t         clear_sleep_counter_flag;
  unsigned int   pre_std_per_sample_by_evt;
  unsigned int   pre_rotate_per_sample_by_evt;

  unsigned int   prev_timestamp;
  
	lqueue_t       sleep_evt;
	squeue_t       se;
  fqueue_t       motion_fet;
  
  uint8_t        evt_load_idx;
  uint8_t        motion_fet_load_idx;

}sleep_para_t;

//---------------------------------------------------------
// This structure defines the parameter feed to motion_st
//---------------------------------------------------------
typedef struct{
  unsigned char  fs ;               /* coeff[0]  default: 50 (Hz) */
  unsigned char  init ;             /* coeff[1]  default: 0 */
  //unsigned short norm_diff_thh;     /* coeff[3]  default: 2000 (2 m/s^2) */
  int            still_th;          /* coeff[2]  default: 360000 */
  //unsigned short norm_diff_thl;     /* coeff[4]  default: 670 (0.67 m/s^2) */
  unsigned short still_num_th;      /* coeff[5]  default: 2 */
  unsigned short awake_window_th;   /* coeff[6]  default: 3 (minute_seg in count) */
  unsigned short interval_h;        /* coeff[7]  default: 30 (seconds) */
  unsigned short interval_l;        /* coeff[8]  default: 60 (seconds) */

  unsigned short move_interval;     /* coeff[9]  default: 180 (seconds) */
  unsigned short sleep_time_th;     /* coeff[10] default: 2700 (seconds) */

  unsigned short tsmd_fa;           /* coeff[11] default: 200 (count) */
  unsigned short tsmd_wk;           /* coeff[12] default: 200 (count) */
  unsigned short tsmd_rest;           /* coeff[13] default: 200 (count) */

  unsigned short ondesk_num_th;     /* coeff[14] default: 50   (count) */
  unsigned short ondesk_std_th;     /* coeff[15] default: 900(0.9 m/s^2) */
  unsigned short desk_std_avg;      /* coeff[16] */
  unsigned short vertical_norm_th;  /* coeff[17] default: 9800*3/4(m/s^2) */
  unsigned short sat_bound;         /* coeff[18] default: 20*50 */

  //20151006 Add from v20 ++
  unsigned short desk_std_avg_num;        /* coeff[16] * coeff[0] */
  unsigned short slope_m_c;               /* coeff[19] */
  unsigned short sleep_time_filter_short; /* coeff[20] seconds, default: 50 minutes = 3000 seconds */
  unsigned short sleep_time_filter_mid_start; /* coeff[21] seconds, default: 90 minutes = 5400 seconds */
  unsigned short sleep_time_filter_mid_end; /* coeff[22] seconds, default: 90 minutes = 5400 seconds */
  unsigned short evt_acc_per_sample_th;   /* coeff[23] */
  unsigned short evt_acc_per_sample_th2;  /* coeff[24] threshold for short sleep with small rotation */
  unsigned short ondesk_trig_time_num;    /* coeff[25] seconds, default: 10 minutes = 600 seconds */
  unsigned short rotate_sum_st_num;       /* coeff[26] seconds, default: 30 minutes = 1800 seconds */
  unsigned short se_search_time_th;       /* coeff[27] seconds, default; 70 minutes = 4200 seconds */
}coeff_t;

//---------------------------------------------------------
// functions for maths
//---------------------------------------------------------
int get_acc_norm_dsp(int x, int y, int z);
int std_statistics(int *ipSignal, int arraySize, int windowLength, int currentIdx);

//---------------------------------------------------------
// functions for sleep detection
//---------------------------------------------------------
void         init_sys_for_sleep(sleep_para_t *p_sleep_para); // init some variables for sleep
int          motion_st(int ax, int ay, int az, unsigned int timestamp);
int          check_sleep_results(void);
void         motion_detection(int8_t *motion_h, int ax, int ay, int az);
int          get_one_sleep_result(int *results, unsigned int *results_timestamp);
int          get_se(void);
unsigned int get_bedtime(void);
int          rearrange_queue_for_sleep(void);
//---------------------------------------------------------
// functions for motion features extraction
//---------------------------------------------------------
int          check_sleep_motion_fet(void);
int          get_one_motion_fet(int *results, unsigned int *results_timestamp);
int          rearrange_queue_for_motion_fet(void);

#if (RUN_MODE == 0)
  /* Matlab */
  void ondesk_detection(int8_t *stillstate_detect, int ondesk_num_th, int ondesk_std_th, int ondesk_std_avg_num, int ax, int ay, int az, int slope_m_c);
  void ondesk_detection2(int8_t *stillstate_detect, int ondesk_num_th, int ondesk_std_th, int ondesk_std_avg_num);
#endif


void vertical_detection(short *vertical_weight, int ax, int vertical_norm_th, unsigned short sat_bound);

void sleep_proc_run(int *tmp_coeff, int *ax, int *ay, int *az, int *gcc_time, int loop, int *sleep_evt, unsigned int *sleep_evt_time, int *se, int *onbed_time,
                    int *tsmd_data, int *posture_state, int *vertical_activity, int *accstd_array, int *rotate_sum_out_array, 
                    int *pre_sleep_counter_array, int *pre_std_per_sample_by_evt_array,
                    int *spd_x_array, int *spd_y_array, int *spd_z_array, int *dist_x_array, int *dist_y_array, int *dist_z_array,
                    int *freq_index_array, int *evt_array, int* motion_fet, unsigned int *motion_fet_time, int *all_evt_fet, unsigned int *all_evt_fet_time);

                    
void sleep_feature_extraction(sleep_para_t *p_sleep_para, int8_t *motion_h, int8_t *stillstate_detect, short *vertical_weight,
                                int ax, int ay, int az, unsigned char *init, int still_th, int still_num_th,
                                int ondesk_num_th, int ondesk_std_th, int vertical_norm_th, int sat_bound);
                    

#if (RUN_MODE == 0)
  /* Matlab */
  int motion_st_init(int *tmp_coeff);
#else
  /* Terminal Server */
  int motion_st_init(void);
#endif


#ifdef _cpluscplus
}
#endif


#endif /* __MOTION_ST_PRIVATE_H__ */
