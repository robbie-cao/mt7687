#include <string.h>
#include <stdio.h>

#include "gnss_app.h"

#ifdef GNSS_SUPPORT_EPO

#include "gnss_api.h"
#include "epo_file.h"

#define MTKEPO_SV_NUMBER 32
#define MTKEPO_RECORD_SIZE 72
#define MTKEPO_SEGMENT_NUM (30 * 4)

int32_t epo_read_data(void* buf, int32_t buf_len, int32_t start_record, int32_t record_num)
{
    static int8_t* epo_data;
    if (buf_len < record_num * MTKEPO_RECORD_SIZE) {
        return 0;
    }
    if (epo_data == NULL) {
        //init epo data
        epo_data = (int8_t*) g_epo_data;
    }
    memcpy( buf, epo_data + MTKEPO_RECORD_SIZE * start_record, record_num * MTKEPO_RECORD_SIZE);
    return record_num * MTKEPO_RECORD_SIZE;
}

static uint8_t calc_nmea_checksum (const int8_t* sentence)
{
    uint8_t checksum = 0;
    while (*sentence) {
        checksum ^= (uint8_t)*sentence++;
    }
    return checksum;
}

// translate UTC to GNSS_Hour
int utc_to_gnss_hour (int iYr, int iMo, int iDay, int iHr)
{
    int iYearsElapsed; // Years since 1980
    int iDaysElapsed; // Days elapsed since Jan 6, 1980
    int iLeapDays; // Leap days since Jan 6, 1980
    int i;
    // Number of days into the year at the start of each month (ignoring leap years)
    const unsigned short doy[12] = {0,31,59,90,120,151,181,212,243,273,304,334};
    iYearsElapsed = iYr - 1980;
    i = 0;
    iLeapDays = 0;
    while (i <= iYearsElapsed) {
        if ((i % 100) == 20) {
            if ((i % 400) == 20) {
                iLeapDays++;
            }
        } else if ((i % 4) == 0) {
            iLeapDays++;
        }
        i++;
    }
    if ((iYearsElapsed % 100) == 20) {
        if (((iYearsElapsed % 400) == 20) && (iMo <= 2)) {
            iLeapDays--;
        }
    } else if (((iYearsElapsed % 4) == 0) && (iMo <= 2)) {
        iLeapDays--;
    }
    iDaysElapsed = iYearsElapsed * 365 + (int)doy[iMo - 1] + iDay + iLeapDays - 6;
    // Convert time to GNSS weeks and seconds
    return (iDaysElapsed * 24 + iHr);
}

void epo_demo_send_assistance_data (int iYr, int iMo, int iDay, int iHr)
{
    int i, segment, epo_gnss_hour, current_gnss_hour;
    unsigned epobuf[MTKEPO_RECORD_SIZE/sizeof(unsigned)];
    int8_t strbuf[200], outbuf[200];

    // read the header (assume EPO file has passed integrity check)
    epo_read_data(epobuf, sizeof(epobuf), 0, 1);
    memcpy(&epo_gnss_hour, epobuf, sizeof(epo_gnss_hour));
    epo_gnss_hour &= 0x00FFFFFF;
    // determine the segment to use
    current_gnss_hour = utc_to_gnss_hour(iYr, iMo, iDay, iHr);
    segment = (current_gnss_hour - epo_gnss_hour) / 6;
    if ((segment < 0) || (segment >= MTKEPO_SEGMENT_NUM)) {
        return;
    }

    // read binary EPO data and sent it to GNSS chip

    for (i = 0; i < MTKEPO_SV_NUMBER; i++) {
        epo_read_data(epobuf, sizeof(epobuf), segment*(MTKEPO_SV_NUMBER) + i, 1);
        // assume host system is little-endian
        sprintf((char*) strbuf,
        "PMTK721,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X",
        i+1,
        epobuf[0], epobuf[1], epobuf[2], epobuf[3], epobuf[4], epobuf[5],
        epobuf[6], epobuf[7], epobuf[8], epobuf[9], epobuf[10], epobuf[11],
        epobuf[12], epobuf[13], epobuf[14], epobuf[15], epobuf[16], epobuf[17]);
        //sprintf((char*) outbuf, "$%s*%02X\r\n", (char*) strbuf, calc_nmea_checksum(strbuf));
        // send them by UART
        //gnss_send_command(outbuf, strlen((char*)outbuf));
        gnss_app_send_cmd(strbuf, strlen(strbuf));
        //GNSSLOGD("%s", outbuf); // for demo
    }
}

#endif
