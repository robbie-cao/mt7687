#ifndef __hal_pmu_internal_H__
#define __hal_pmu_internal_H__

#include "hal_mt2523_pmu_platform.h"
#include "hal_platform.h"

#ifdef HAL_PMU_MODULE_ENABLED

typedef enum {
    PMIC_VCLDO,
    PMIC_VIO18,
    PMIC_VSF,
    PMIC_VIO28,
    PMIC_VDIG18,
    PMIC_VA18,
    PMIC_VUSB,
    PMIC_VBT,
    PMIC_VA28,
    PMIC_VMC,
    PMIC_VCAMA,
    PMIC_VIBR,
    PMIC_SWXM,
    PMIC_SWDP,
    PMIC_SWMP
} PMIC_VR;

typedef enum {
    PMIC_PSI_OVR,
    PMIC_PSI_HP,
    PMIC_PSI_LP,
    PMIC_PSI_S0,
    PMIC_PSI_S1,
    PMIC_PSI_OFF,
    PMIC_PSI_SLP,
    PMIC_PSI_ACT
} PMIC_PSI;

typedef enum {
    PMIC_LDO_PMOD_NORMAL  = 0,
    PMIC_LDO_PMOD_LITE    = 1,
    PMIC_LDO_PMOD_LP      = 2,
    PMIC_LDO_PMOD_ULP     = 3
} PMIC_LDO_PMOD;

typedef enum {
    PMIC_SLP_WKUP_HW_CTL  = 0,
    PMIC_SLP_WKUP_SW_CTL  = 1,
    PMIC_SLP_WKUP_AON     = 2
} PMIC_SLP_WKUP_MODE;

typedef enum {
    PMIC_VR_CTL_DISABLE   = 0,
    PMIC_VR_CTL_ENABLE    = 1
} PMIC_VR_CTL;

typedef enum {
    PMIC_VCORE_0P7V	= 0,
    PMIC_VCORE_0P8V	= 1,
    PMIC_VCORE_0P9V	= 2,
    PMIC_VCORE_1P0V	= 3,
    PMIC_VCORE_1P1V	= 4,
    PMIC_VCORE_1P2V	= 5,
    PMIC_VCORE_1P3V	= 6,
    PMIC_VCORE_ERROR  = 255
} PMIC_VCORE_VOSEL;

typedef enum {
    PMIC_LDO_1P3V = 0,
    PMIC_LDO_1P5V = 1,
    PMIC_LDO_1P8V = 2,
    PMIC_LDO_2P0V = 3,
    PMIC_LDO_2P5V = 4,
    PMIC_LDO_2P8V = 5,
    PMIC_LDO_3P0V = 6,
    PMIC_LDO_3P3V = 7
} PMIC_LDO_VOSEL;

typedef enum {
    PMIC_VCORE_UNLOCK = 0,
    PMIC_VCORE_LOCK = 1
} PMIC_VCORE_LOCK_CTRL;

void pmu_control_mtcmos(void);
void pmu_ctrl_va28_ldo(unsigned char enable);
void pmu_ctrl_vbt_ldo(unsigned char enable);
int  pmu_ctrl_vcore(PMIC_VCORE_LOCK_CTRL lock, PMIC_VCORE_VOSEL vcore_vosel);
void PMIC_VR_PSI_PMOD_CONFIG(PMIC_VR vr_name, PMIC_PSI psi_mode, PMIC_LDO_PMOD pmod);
void PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VR vr_name, PMIC_PSI psi_mode, PMIC_SLP_WKUP_MODE ctl_mode, int wk_slot);
void PMIC_VR_CONTROL(PMIC_VR vr_name, PMIC_VR_CTL enable);
void PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI psi_mode, PMIC_VCORE_VOSEL vcore_vosel);
void PMIC_VR_VOSEL_INIT_CONFIG(PMIC_VR vr_name, PMIC_LDO_VOSEL ldo_vosel);
void pmu_set_mtcmos(unsigned char setting);
PMIC_VCORE_VOSEL pmu_get_vcore_voltage(void);
void pmu_set_vcore_s1_buck(void);

#endif /* HAL_PMU_MODULE_ENABLED */
#endif
