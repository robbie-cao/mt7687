
/**
 * @file hci_log.h
 *
 *  HCI log out related function. 
 *
 */
#ifndef __HCI_LOG_H__
#define __HCI_LOG_H__

#include "stdint.h"

typedef enum
{
    HCI_COMMAND = 1,
    HCI_EVENT = 2,
    HCI_ACL_IN = 4,
    HCI_ACL_OUT = 8,
}ENUM_HCI_DATA_TYPE;


/**
 * @brief  record HCI command.
 *
 * @param  buf [IN] HCI command data.
 * @param  length [IN] indicate length of buf.
 *
 * @return size of buf had recorded, <0 means fail.
 *
 */
int32_t hci_log_cmd(unsigned char* buf, int32_t length);

/**
 * @brief  record HCI event.
 *
 * @param  buf [IN] HCI event data.
 * @param  length [IN] indicate length of buf.
 *
 * @return size of buf had recorded, <0 means fail.
 *
 */
int32_t hci_log_event(unsigned char* buf, int32_t length);

/**
 * @brief  record HCI ACL data out.
 *
 * @param  buf [IN] HCI ACL data out.
 * @param  length [IN] indicate length of buf.
 *
 * @return size of buf had recorded, <0 means fail.
 *
 */
int32_t hci_log_acl_out(unsigned char* buf, int32_t length);

/**
 * @brief  record HCI ACL data in.
 *
 * @param  buf [IN] HCI ACL data in.
 * @param  length [IN] indicate length of buf.
 *
 * @return size of buf had recorded, <0 means fail.
 *
 */
int32_t hci_log_acl_in(unsigned char* buf, int32_t length);





#endif
