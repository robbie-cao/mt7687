#include "hal_bsi.h"
#include "hal_platform.h"
#include "hal_sleep_manager_platform.h"
#include "hal_log.h"
#include "hal_gpt.h"

void bsi_write(uint8_t address , uint32_t data)
{
//	log_hal_info("bsi_write address:%d  data:%#x \n",address,data);
//	log_hal_info("[BSI init MODEM_CG_SET4:%d  CG_chck2:%#x] \n",*MODEM_CG_SET4,*PDN_COND0);
    uint32_t check;
    uint32_t rw_data;
    *BSI_WRDAT_CON = 0x1d00;
    data = data & 0x0007FFFFF;
    rw_data = address << 20;
    rw_data = rw_data | data;
    *BSI_WRDAT = rw_data;
    *BSI_CON = 0x80401;
    while (1) {
        check = *BSI_CON;
        if ((check & 0x80000000) == 0x80000000) {
            break;
        }
    }
    log_hal_info("[BSI_WRDAT : %x]\n", (unsigned int)*BSI_WRDAT);
}

void bsi_read(uint8_t address, uint32_t *get_data)
{
    uint32_t read_data;
    uint32_t rw_data;
    *BSI_RDCON = 0x9f8b;
    *BSI_RDADDR_CON = 0x0902;
    rw_data = 0x100;
    rw_data = rw_data + address;
    *BSI_RDADDR = rw_data;
    *BSI_RDCS_CON = 0x0;
    *BSI_RDCON = 0x89f8b;
    *get_data = *BSI_RDDAT;
    while (1) {
        read_data = *BSI_CON;
        if ((read_data & 0x80000000) == 0x80000000) {
            break;
        }
    }
    log_hal_info("[BSI_RDDAT : %x]\n", (unsigned int)*get_data);
}

void bsi_init(void)
{
    *MODEM_CG_CLR4 = *MODEM_CG_CLR4 | 0x4;
    *PDN_CLRD0 = *PDN_CLRD0 | (1 << 23);
}

