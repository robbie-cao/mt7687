/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */
#include <stdio.h>
#include <string.h>

#include "lwip/opt.h"
#include <stdint.h>
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include <lwip/sockets.h>
#include <lwip/tcpip.h>
#include <lwip/dhcp.h>
#include "netif/etharp.h"
//#include "netif/ppp_oe.h"   //2015-7-21 bing.luo delete on@132599
#include "ethernetif.h"
#include "net_task.h"
#include "type_def.h"
#include "connsys_util.h"
#include "connsys_driver.h"
#include "inband_queue.h"
#include "mt_cmd_fmt.h"
#include "lwip/ethip6.h"
#if defined(MTK_HAL_LOWPOWER_ENABLE)
#include "hal_lp.h"
#endif
#include "syslog.h"
#include "wifi_scan.h"
#if (CFG_WIFI_HIF_GDMA_EN == 1)
#include "hal_gdma.h"
#endif
#include "lwip/netif.h"
#include "wifi_rx_desc.h"

/* Sanity check the configuration. */
#define ALIGN_4BYTE(size)       (((size+3)/4) * 4)

#ifndef IS_ALIGN_4
#define IS_ALIGN_4(_value)      (((_value) & 0x3) ? FALSE : TRUE)
#define IS_NOT_ALIGN_4(_value)  (((_value) & 0x3) ? TRUE : FALSE)
#endif /* IS_ALIGN_4 */

#ifndef IS_NOT_ALIGN_4
#define IS_NOT_ALIGN_4(_value)  (((_value) & 0x3) ? TRUE : FALSE)
#endif /* IS_NOT_ALIGN_4 */



#if defined(MTK_BSP_LOOPBACK_ENABLE)
#include "debug.h"
unsigned int g_loopback_start = 0;
unsigned int loopback_start() { return g_loopback_start;}
void loopback_start_set(unsigned int start) { g_loopback_start = start; }
#endif // MTK_BSP_LOOPBACK_ENABLE

/* Define those to better describe your network interface. Note: only 2-byte allowed */
#define IFNAME00 's'
#define IFNAME01 't'
#define IFNAME10 'a'
#define IFNAME11 'p'
#define IFNAME20 'l'
#define IFNAME21 'o'

static int g_lwip_eapol_rx_socket = -1;
static int g_lwip_eapol_rx_socket_second = -1;

struct netif sta_if, ap_if;//modified by Yong Chang

static netif_cmd_rx_filter_fn netif_inband_evt_filter = NULL;

static netif_rx_filter_fn netif_data_rx_filters[] =
{
    NULL, /* 80211 RAW packet */
    NULL  /* EAPOL packet */
};

#define DATA_RX_FILTER_COUNT (sizeof(netif_data_rx_filters) / sizeof(netif_rx_filter_fn))

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};

/* Forward declarations. */
void  ethernetif_input(struct netif *netif, int port);
void ethernetif_intr_enhance_mode_input_callback(void* pkt_ptr, int port, unsigned int len);
void ethernetif_intr_enhance_mode_allocate_pkt_callback(
                            unsigned int allocate_len,
                            void** pkt_ptr,
                            unsigned char** payload_ptr);

void inband_handle_func(void* pkt_ptr, unsigned char *payload, unsigned int len);

#if 0
static void InterruptSimulator( void *pvParameters )
{
   /*
    *  There is no real way of simulating an interrupt.
    *  Make sure other tasks can run.
    */

   do
   {
      vTaskDelay(3*configTICK_RATE_HZ);
   }
   while (1);
}
#endif

void enqueue_rx_pkt(int port)
{
    NetJobAddFromISR((PNETFUNC)connsys_util_intr_enhance_mode_receive_data, 0, 0);
}

static void register_netTask_callback(void)
{
    connsys_register_rx_handle((void *)&enqueue_rx_pkt);
    ethernet_set_inband_queue_evt_handler(inband_queue_evt_handler);
}

void register_eapol_rx_socket(int eapol_rx_socket)
{
	LOG_I(lwip, "%s\n", __FUNCTION__);

	if (eapol_rx_socket >= 0)
	g_lwip_eapol_rx_socket = eapol_rx_socket;
	g_lwip_eapol_rx_socket_second = -1;
}

void unregister_eapol_rx_socket()
{
    LOG_I(lwip, "%s\n", __FUNCTION__);
    if (g_lwip_eapol_rx_socket >= 0)
        close(g_lwip_eapol_rx_socket);
    g_lwip_eapol_rx_socket = -1;
}


void register_eapol_rx_socket_dual_intf(int eapol_rx_socket, int eapol_rx_socket_second)
{
    LOG_I(lwip, "%s\n", __FUNCTION__);
    if (eapol_rx_socket >= 0)
        g_lwip_eapol_rx_socket = eapol_rx_socket;

    if (eapol_rx_socket_second >= 0)
        g_lwip_eapol_rx_socket_second = eapol_rx_socket_second;
}

void unregister_eapol_rx_socket_dual_intf(int eapol_rx_socket, int eapol_rx_socket_second)
{
    LOG_I(lwip, "%s\n", __FUNCTION__);
    if (eapol_rx_socket >= 0)
        close(eapol_rx_socket);
    g_lwip_eapol_rx_socket = -1;

    if (eapol_rx_socket_second >= 0)
        close(eapol_rx_socket_second);
    g_lwip_eapol_rx_socket_second = -1;
}


void low_level_set_mac_addr(struct netif *netif, uint8_t *mac_addr)
{
    /* set MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* set MAC hardware address */
    memcpy(netif->hwaddr, mac_addr, ETHARP_HWADDR_LEN);
}

/**
 * Find a network interface by searching for its name
 *
 * @param name the name of the netif (like netif->name) NOT plus concatenated number
 * in ascii representation (e.g. 'en0')
 */
struct netif *
netif_find_name(char *name)
{
  struct netif *netif;

  if (name == NULL) {
    return NULL;
  }

  for(netif = netif_list; netif != NULL; netif = netif->next) {
    if (name[0] == netif->name[0] &&
       name[1] == netif->name[1]) {
      LWIP_DEBUGF(NETIF_DEBUG, ("netif_find: found %c%c\n", name[0], name[1]));
      return netif;
    }
  }
  LWIP_DEBUGF(NETIF_DEBUG, ("netif_find: didn't find %c%c\n", name[0], name[1]));
  return NULL;
}



int low_level_get_sta_mac_addr(unsigned char *mac_addr)
{
    char name[3];
    struct netif *netif = NULL;

    name[0] = IFNAME00;
    name[1] = IFNAME01;

    netif = netif_find_name(name);
    if (netif)
    {
        memcpy(mac_addr, netif->hwaddr, ETHARP_HWADDR_LEN);
    }
    else
    {
        return -1;
    }

    return 0;
}

int low_level_get_ap_mac_addr(unsigned char *mac_addr)
{
    char name[3];
    struct netif *netif = NULL;

    name[0] = IFNAME10;
    name[1] = IFNAME11;

    netif = netif_find_name(name);
    if (netif)
    {
        memcpy(mac_addr, netif->hwaddr, ETHARP_HWADDR_LEN);
    }
    else
    {
        return -1;
    }

    //snowpin test
    LOG_I(lwip, "%s - %02x:%02x:%02x:%02x:%02x:%02x\n",
        __FUNCTION__, mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    //snowpin test

    return 0;
}



/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void
low_level_init1(struct netif *netif)
{
#if 0 // init MAC address in upper layer
  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */
  netif->hwaddr[0] = 0x00;
  netif->hwaddr[1] = 0x0c;
  netif->hwaddr[2] = 0x43;
  netif->hwaddr[3] = 0x76;
  netif->hwaddr[4] = 0x62;
  netif->hwaddr[5] = 0x02;
#endif

  /* maximum transfer unit */
  netif->mtu = 1500;

  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_IGMP | NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

  /* Do whatever else is needed to initialize interface. */
}

static void
low_level_init2(struct netif *netif)
{
#if 0 // init MAC address in upper layer
  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */
  netif->hwaddr[0] = 0x00;
  netif->hwaddr[1] = 0x0c;
  netif->hwaddr[2] = 0x43;
  netif->hwaddr[3] = 0x76;
  netif->hwaddr[4] = 0x62;
  netif->hwaddr[5] = 0x04;
#endif

  /* maximum transfer unit */
  netif->mtu = 1500;

  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_IGMP | NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

  /* Do whatever else is needed to initialize interface. */
}


/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
#if 0
static int low_level_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *q;
    u32_t total_len = 0;
    int ret = ERR_OK;
    int connsys_ret = CONNSYS_STATUS_SUCCESS;
    u32_t pbuf_num = 0;
    unsigned char *pBuffer = NULL;
    unsigned char *pBuffer_4byte_align = NULL;
    unsigned char *pTmp = NULL;
    taskENTER_CRITICAL();


    for(q = p; q != NULL; q = q->next) {
        total_len = total_len + (q->len);
        pbuf_num = pbuf_num + 1;
        DBG_CONNSYS(CONNSYS_DBG_TX_Q1,("==>low_level_output, q->len = %u\n",
            (unsigned int)q->len));
    }
    DBG_CONNSYS(CONNSYS_DBG_TX_Q1,("==>low_level_output, pbuf_num = %u, total_len = %u\n",
        (unsigned int)pbuf_num, (unsigned int)total_len));

    if (pbuf_num > 1)
    {
        /* concate pbuf_chain into single buffer */
        u32_t allocate_len = total_len +
            CFG_CONNSYS_IOT_TX_ZERO_COPY_PAD_LEN +
            WIFI_HIF_TX_BYTE_CNT_LEN;

        u32_t payload_offset = sizeof(INIT_HIF_TX_HEADER_T);

#if (CFG_CONNSYS_IOT_TX_ZERO_COPY_EN == 1)
        payload_offset += CFG_CONNSYS_IOT_TX_ZERO_COPY_PAD_LEN;
#endif

        if (IS_NOT_ALIGN_4(total_len))
            allocate_len += 4;

        pBuffer = os_malloc(allocate_len);
        if (NULL == pBuffer)
        {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
            DBG_CONNSYS(CONNSYS_DBG_TX_Q1,("==>low_level_output, out of memory\n"));
            taskEXIT_CRITICAL();
            return ERR_MEM;
        }
        pBuffer_4byte_align = (unsigned char *)((unsigned int)pBuffer & (~0x3UL));
        pTmp = pBuffer_4byte_align + payload_offset;

        for(q = p; q != NULL; q = q->next) {
            memcpy(pTmp, q->payload, q->len);
            pTmp += (q->len);
        }

        if (netif == &ap_if)
        {
            connsys_ret = connsys_util_low_level_output(pBuffer_4byte_align, total_len, IOT_PACKET_TYPE_INF_1_IDX);
        }
        else
        {
            connsys_ret = connsys_util_low_level_output(pBuffer_4byte_align, total_len, IOT_PACKET_TYPE_INF_0_IDX);
        }
        os_free(pBuffer);
    }
    else
    {
#if CFG_CONNSYS_TXD_PAD_SIZE
        pbuf_header(p, CFG_CONNSYS_TXD_PAD_SIZE);
#endif

        if (netif == &ap_if)
        {
            connsys_ret = connsys_util_low_level_output(p->payload, total_len, IOT_PACKET_TYPE_INF_1_IDX);
        }
        else
        {
            connsys_ret = connsys_util_low_level_output(p->payload, total_len, IOT_PACKET_TYPE_INF_0_IDX);
        }
    }

    if (connsys_ret == CONNSYS_STATUS_FAIL)
        ret = ERR_BUF;

  LINK_STATS_INC(link.xmit);
  taskEXIT_CRITICAL();
  return ret;
}
#endif

static int32_t low_level_output_scatter(struct netif *netif, struct pbuf *p)
{
    P_HIF_TX_HEADER_PORT1_T p_txd;
    int32_t ret = CONNSYS_STATUS_SUCCESS;
    int connsys_ret = CONNSYS_STATUS_SUCCESS;
    struct pbuf *q;
    int32_t inf_num;
    uint32_t total_len = 0;
    connsys_tx_scatter_info_t tx_info;
    signed int flow_control_status;
#if (CONNSYS_DEBUG_MODE_EN == 1)
    static uint32_t tx_sequence = 0;
#endif

#ifdef MTK_WIFI_REPEATER_ENABLE
    int    i = 0;
    struct eth_hdr *ethhdr;
    struct etharp_hdr *hdr;

    int      sndAllInf = 0;
    unsigned char op_mode = WIFI_MODE_STA_ONLY;

    ethhdr = (struct eth_hdr *)p->payload;
    // Check if OpMode is Repeater Mode. Broadcast to all interfaces only at Repeater Mode.
    if (__g_wpa_supplicant_api.wpa_supplicant_entry_op_mode_get)
    {
        __g_wpa_supplicant_api.wpa_supplicant_entry_op_mode_get(&op_mode);
        if (op_mode == WIFI_MODE_REPEATER)
        {
           sndAllInf = (memcmp(&(ethhdr->dest.addr), "\xff\xff\xff\xff\xff\xff", 6) == 0)?1:0;
        }
    }

    for (i=0; i<=sndAllInf; i++)
    {
        total_len = 0;
        // Change Source Mac for Ether/Arp packets if it is need
        if (op_mode == WIFI_MODE_REPEATER)
        {
            if ( sndAllInf && (i==sndAllInf) )
            {
                if (   (memcmp(&ethhdr->src, (struct eth_addr*)(sta_if.hwaddr), 6)==0)
                    || (memcmp(&ethhdr->src, (struct eth_addr*)(ap_if.hwaddr), 6)==0)
                   )
                {
                   //LOG_I(lwip, "src - %2x:%2x:%2x:%2x:%2x:%2x, dest - %2x:%2x:%2x:%2x:%2x:%2x, sndAllInf(%d)\n", ethhdr->src.addr[0], ethhdr->src.addr[1], ethhdr->src.addr[2], ethhdr->src.addr[3], ethhdr->src.addr[4], ethhdr->src.addr[5], ethhdr->dest.addr[0], ethhdr->dest.addr[1], ethhdr->dest.addr[2], ethhdr->dest.addr[3], ethhdr->dest.addr[4], ethhdr->dest.addr[5], sndAllInf);
                    netif = (netif == &ap_if)?&sta_if:&ap_if;
                    ETHADDR16_COPY(&ethhdr->src, (struct eth_addr*)(netif->hwaddr));
                    if ( ethhdr->type == PP_HTONS(ETHTYPE_ARP) )
                   {
                       hdr = (struct etharp_hdr *)((u8_t*)ethhdr + SIZEOF_ETH_HDR);
                       #if ETHARP_SUPPORT_VLAN
                         if (ethhdr->type == PP_HTONS(ETHTYPE_VLAN)) {
                           hdr = (struct etharp_hdr *)(((u8_t*)ethhdr) + SIZEOF_ETH_HDR + SIZEOF_VLAN_HDR);
                         }
                       #endif /* ETHARP_SUPPORT_VLAN */
                       ETHADDR16_COPY(&(hdr->shwaddr), (struct eth_addr*)(netif->hwaddr));
                   }
                }
            }
            // Check if this remote STA mac belong to this AP
            else if ( (__g_wpa_supplicant_api.get_ap_sta) && (__g_wpa_supplicant_api.get_ap_sta((char *)&(ethhdr->dest.addr))) )
            {
                netif = &ap_if;
                if (memcmp(&ethhdr->src, (struct eth_addr*)(sta_if.hwaddr), 6)==0)
                {
                    ETHADDR16_COPY(&ethhdr->src, (struct eth_addr*)(netif->hwaddr));
                }
                LOG_I(lwip,"Change netif to ap_if : src - %2x:%2x:%2x:%2x:%2x:%2x, dest - %2x:%2x:%2x:%2x:%2x:%2x\n", ethhdr->src.addr[0], ethhdr->src.addr[1], ethhdr->src.addr[2], ethhdr->src.addr[3], ethhdr->src.addr[4], ethhdr->src.addr[5],ethhdr->dest.addr[0], ethhdr->dest.addr[1], ethhdr->dest.addr[2], ethhdr->dest.addr[3], ethhdr->dest.addr[4], ethhdr->dest.addr[5]);
            }
        }
#endif

#if (CONNSYS_DEBUG_MODE_EN == 1)
    if (CONNSYS_TEST_DEBUG(CONNSYS_DBG_TX_Q1))
    {
        LOG_I(lwip, "== TX Start ==  tx_sequence: %u\n", (unsigned int)tx_sequence);
    }
    tx_sequence++;
#endif

    memset(&tx_info, 0, sizeof(connsys_tx_scatter_info_t));
    if (netif == &ap_if)
    {
        inf_num = IOT_PACKET_TYPE_INF_1_IDX;
    }
    else
    {
        inf_num = IOT_PACKET_TYPE_INF_0_IDX;
    }
    //LOG_I(lwip, "sendto inf_num(%d)...%s-%d\n",inf_num,__FUNCTION__,__LINE__);
    for(q = p; q != NULL; q = q->next) {
        if (tx_info.buf_num >= MAX_TX_BUF)
        {
            LOG_E(lwip, "ERROR! ==> low_level_output_scatter, buf_num exceed MAX_TX_BUF\n");
            LOG_E(lwip, "tx_info.buf_num = %u, MAX_TX_BUF = %u\n",
                (unsigned int)tx_info.buf_num,
                (unsigned int)MAX_TX_BUF);
            connsys_dump_tx_scatter_info(&tx_info);
            return ERR_BUF;
        }
        
        total_len = total_len + (q->len);
        tx_info.buff[tx_info.buf_num].buf_ptr = q->payload;
        tx_info.buff[tx_info.buf_num].buf_len= q->len;

#if (CONNSYS_DEBUG_MODE_EN == 1)
        DBG_CONNSYS(CONNSYS_DBG_TX_Q1,("pbuf chain len[%u]: %u\n",
            (unsigned int)tx_info.buf_num,
            (unsigned int)q->len));
#endif

        tx_info.buf_num ++;
    }
#if (CFG_CONNSYS_IOT_TX_ZERO_COPY_EN == 1)
#ifdef MTK_MINISUPP_ENABLE
    if (__g_wpa_supplicant_api.get_sta_qos_bit) {
        tx_info.fg_wmm = __g_wpa_supplicant_api.get_sta_qos_bit(inf_num, (char *)p->payload);
    }
    if (tx_info.fg_wmm == 1)
        tx_info.reserved_headroom_len = connsys_get_headroom_offset(CONNSYS_HEADROOM_OFFSET_QOS); //EXTRA_HEADROOM_LEN_FOR_QOS_ENABLE;
    else
        tx_info.reserved_headroom_len = connsys_get_headroom_offset(CONNSYS_HEADROOM_OFFSET_NON_QOS); // EXTRA_HEADROOM_LEN_FOR_NON_QOS;
#endif
#else
    tx_info.reserved_headroom_len = 0;
#endif
    tx_info.tx_len = sizeof(INIT_HIF_TX_HEADER_T)
                        + tx_info.reserved_headroom_len
                        + total_len;

    tx_info.real_tx_len = ALIGN_4BYTE(tx_info.tx_len);

    p_txd = (P_HIF_TX_HEADER_PORT1_T)&tx_info.txd;
    p_txd->u2TxByteCount = tx_info.tx_len;
    p_txd->u2PQ_ID = P1_Q1;
    p_txd->ucPktTypeID = PKT_ID_CMD;
    p_txd->ucPacketOffset = tx_info.reserved_headroom_len;

    if (inf_num == IOT_PACKET_TYPE_INF_1_IDX)
        p_txd->ucReserved = IOT_PACKET_TYPE_INF_1;
    else
        p_txd->ucReserved = IOT_PACKET_TYPE_INF_0;


    if (CONNSYS_TEST_DEBUG(CONNSYS_DBG_TX_Q1))
    {
        connsys_dump_tx_scatter_info(&tx_info);
    }

#if (CFG_CONNSYS_TRX_BALANCE_EN == 1)
    if (g_balance_ctr.balance_en == TRUE)
    {
        if (connsys_balance_check_tx_yield())
        {
            vTaskPrioritySet(NULL, (TCPIP_THREAD_PRIO - 1));
            vTaskPrioritySet((TaskHandle_t)g_balance_ctr.rx_handle, TCPIP_THREAD_PRIO);
        }
        else
        {
            vTaskPrioritySet(NULL, TCPIP_THREAD_PRIO);
            vTaskPrioritySet((TaskHandle_t)g_balance_ctr.rx_handle, (TCPIP_THREAD_PRIO - 1));
        }
    }
#endif

    flow_control_status =
        connsys_tx_flow_control_check_and_update_tx(WIFI_HIF_TX_PORT_IDX, tx_info.real_tx_len);

    if (flow_control_status == CONNSYS_STATUS_SUCCESS)
    {
        connsys_ret = connsys_write_data(&tx_info);
    }
    else
    {
        connsys_ret = CONNSYS_STATUS_FAIL;
    }

    if (connsys_ret == CONNSYS_STATUS_FAIL)
        ret = ERR_BUF;
    else
        LINK_STATS_INC(link.xmit);

#if (CFG_CONNSYS_TRX_BALANCE_EN == 1)
    connsys_balance_clear_tx_wait();
#endif

#if (CONNSYS_DEBUG_MODE_EN == 1)
    DBG_CONNSYS(CONNSYS_DBG_TX_Q1,("== TX End ==\n"));
#endif

#ifdef MTK_WIFI_REPEATER_ENABLE
}
#endif
    return ret;
}

void ethernetif_init_callback(void)
{
    register_netTask_callback();
    connsys_register_allocate_pkt_callback(ethernetif_intr_enhance_mode_allocate_pkt_callback);
    connsys_register_input_pkt_callback(ethernetif_intr_enhance_mode_input_callback);
    inband_queue_register_callback(inband_handle_func);
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
ethernetif_init1(struct netif *netif)
{
  struct ethernetif *ethernetif;

  LWIP_ASSERT("netif != NULL", (netif != NULL));

  LOG_I(lwip, "=> %s\n", __FUNCTION__);

  ethernetif = mem_malloc(sizeof(struct ethernetif));
  if (ethernetif == NULL) {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
    return ERR_MEM;
  }

  LOG_I(lwip, "%s : malloc ok : %p\n", __FUNCTION__, ethernetif);
#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  netif->state = ethernetif;
  netif->name[0] = IFNAME00;
  netif->name[1] = IFNAME01;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  #if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
  #endif /* LWIP_IPV6 */
  netif->linkoutput = (netif_linkoutput_fn)low_level_output_scatter;

  ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

  /* initialize the hardware */
  low_level_init1(netif);

#if 0
  /* Create a task that simulates an interrupt in a real system.  This will
  block waiting for packets, then send a message to the uIP task when data
  is available. */

  LOG_I(lwip, "Create RX task\n\r");
  xTaskCreate(InterruptSimulator, "RX", 400, (void *)netif, 3, NULL );
#endif
  return ERR_OK;
}

err_t
ethernetif_init2(struct netif *netif)
{
  struct ethernetif *ethernetif;

  LWIP_ASSERT("netif != NULL", (netif != NULL));

  LOG_I(lwip, "=> %s\n", __FUNCTION__);

  ethernetif = mem_malloc(sizeof(struct ethernetif));
  if (ethernetif == NULL) {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
    return ERR_MEM;
  }

  LOG_I(lwip, "%s : malloc ok : %p\n", __FUNCTION__, ethernetif);
#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  netif->state = ethernetif;
  netif->name[0] = IFNAME10;
  netif->name[1] = IFNAME11;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  #if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
  #endif /* LWIP_IPV6 */
  netif->linkoutput = (netif_linkoutput_fn)low_level_output_scatter;

  ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

  /* initialize the hardware */
  low_level_init2(netif);

#if 0
  /* Create a task that simulates an interrupt in a real system.  This will
  block waiting for packets, then send a message to the uIP task when data
  is available. */

  LOG_I(lwip, "Create RX task\n\r");
  xTaskCreate(InterruptSimulator, "RX", 400, (void *)netif, 3, NULL );
#endif
  return ERR_OK;
}


/**
 * Set in-band-cmd handler for in-band-cmd received
 */
void ethernet_set_inband_queue_evt_handler(netif_cmd_rx_filter_fn handler)
{
    netif_inband_evt_filter = handler;
}

void ethernet_set_data_rx_handler(DATA_RX_FILTER_IDX idx, netif_rx_filter_fn handler)
{
    if (idx > DATA_RX_FILTER_COUNT)
    {
        LOG_E(lwip, "ERROR: Invalid data rx filter ID(%d) > %d\n", idx, DATA_RX_FILTER_COUNT);
        return;
    }
    netif_data_rx_filters[idx] = handler;
}

int ethernet_raw_pkt_sender(unsigned char *buf, unsigned int len, struct netif *netif)
{
    struct pbuf *p;
    int ret = 0;

    p = pbuf_alloc(PBUF_RAW_TX, len, PBUF_POOL);
    if (p == NULL)
    {
        LOG_E(lwip, "%s pbuf_alloc fail\n\r", __FUNCTION__);
        return -1;
    }
    memcpy(p->payload, buf, len);

    ret = low_level_output_scatter(netif, p);
    pbuf_free(p);
    return ret;
}

void ethernetif_intr_enhance_mode_allocate_pkt_callback(
                            unsigned int allocate_len,
                            void** pkt_ptr,
                            unsigned char** payload_ptr)
{
    struct pbuf *p;

    /* We allocate a pbuf chain of pbufs from the pool. */
#if (CFG_CONNSYS_TRX_BALANCE_EN == 1)

    if (g_balance_ctr.balance_en == TRUE)
    {
        if (connsys_balance_check_rx_yield())
        {
            vTaskPrioritySet(NULL, (TCPIP_THREAD_PRIO - 1));
            if (g_balance_ctr.tx_handle)
                vTaskPrioritySet((TaskHandle_t)g_balance_ctr.tx_handle, TCPIP_THREAD_PRIO);
        }
        else
        {
            vTaskPrioritySet(NULL, TCPIP_THREAD_PRIO);
        }
    p = pbuf_alloc(PBUF_RAW, (ssize_t)allocate_len, PBUF_POOL);

    if (p)
    {
        (*pkt_ptr) = p;
        (*payload_ptr) = p->payload;
    }
    else
    {
        (*pkt_ptr) = NULL;
        (*payload_ptr) = NULL;
            vTaskPrioritySet(NULL, (TCPIP_THREAD_PRIO - 1));
            if (g_balance_ctr.tx_handle)
                vTaskPrioritySet((TaskHandle_t)g_balance_ctr.tx_handle, TCPIP_THREAD_PRIO);
        }
    }
    else
    {
        p = pbuf_alloc(PBUF_RAW, (ssize_t)allocate_len, PBUF_POOL);
        if (p)
        {
            (*pkt_ptr) = p;
            (*payload_ptr) = p->payload;
            vTaskPrioritySet(NULL, 3);
        }
        else
        {
            (*pkt_ptr) = NULL;
            (*payload_ptr) = NULL;
            vTaskPrioritySet(NULL, 2);
        }
    }
#else
    p = pbuf_alloc(PBUF_RAW, (ssize_t)allocate_len, PBUF_POOL);
    if (p)
    {
        (*pkt_ptr) = p;
        (*payload_ptr) = p->payload;
        vTaskPrioritySet(NULL, TCPIP_THREAD_PRIO);
    }
    else
    {
        (*pkt_ptr) = NULL;
        (*payload_ptr) = NULL;
        vTaskPrioritySet(NULL, (TCPIP_THREAD_PRIO - 1));
    }
#endif /* (CFG_CONNSYS_TRX_BALANCE_EN == 1) */
}


void
ethernetif_intr_enhance_mode_dispatch(struct pbuf *p, struct netif *netif)
{
  //struct ethernetif *ethernetif;

  struct eth_hdr *ethhdr;

  //ethernetif = netif->state;
  /* move received packet into a new pbuf */

  /* no packet could be read, silently ignore this */
  if (p == NULL) return;
  /* points to packet payload, which starts with an Ethernet header */
  ethhdr = p->payload;

  switch (htons(ethhdr->type)) {
  /* IP or ARP packet? */
  case ETHTYPE_IP:
#if LWIP_IPV6
  case ETHTYPE_IPV6:
#endif
  case ETHTYPE_ARP:
#if PPPOE_SUPPORT
  /* PPPoE packet? */
  case ETHTYPE_PPPOEDISC:
  case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
    /* full packet send to tcpip_thread to process */
    if (netif->input(p, netif) != ERR_OK)
     { LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
       pbuf_free(p);
       p = NULL;
     }
    break;
#if 1 // for wpa_supplicant eapol packets
  case 0x888E:
#ifndef RELAY
    if((p->len > 12) && (g_lwip_eapol_rx_socket >= 0) && (g_lwip_eapol_rx_socket_second >= 0)) {
            struct sockaddr_in    to;
            ssize_t               len;

            LOG_I(lwip,"<<Dual interface RX EAPOL (Len=%d)>>\n", p->len);

            to.sin_family      = PF_INET;
            to.sin_addr.s_addr = htonl((127 << 24) | 1);

            if ((IFNAME10 == netif->name[0]) && (IFNAME11 == netif->name[1])) {
                to.sin_port = htons(66);
                LOG_I(lwip,"send to AP socket[%d]...\n", g_lwip_eapol_rx_socket);
                len = lwip_sendto(g_lwip_eapol_rx_socket, p->payload, p->len, 0, (struct sockaddr *)&to, sizeof(to));
            } else {
                to.sin_port = htons(76);
                LOG_I(lwip,"send to STA socket[%d]...\n", g_lwip_eapol_rx_socket_second);
                len = lwip_sendto(g_lwip_eapol_rx_socket_second, p->payload, p->len, 0, (struct sockaddr *)&to, sizeof(to));
            }

            if (len != p->len)
                LOG_E(lwip,"Dual interface eapol-rx relay sendto failed!\n");
    }//dual interface
    else if ((p->len > 12) && (g_lwip_eapol_rx_socket >= 0))
    {
        struct sockaddr_in    to;
        ssize_t               len;

        LOG_I(lwip, "<<RX EAPOL (Len=%d)>>\n", p->len);

        to.sin_family      = PF_INET;
        to.sin_addr.s_addr = htonl((127 << 24) | 1);

        to.sin_port        = htons(66);
        LOG_I(lwip, "send to socket[%d]...\n", g_lwip_eapol_rx_socket);
        len = lwip_sendto(g_lwip_eapol_rx_socket, p->payload, p->len, 0, (struct sockaddr *)&to, sizeof(to));

        if (len != p->len)
            LOG_E(lwip, "Single interface eapol-rx relay sendto failed!\n");
    }

    pbuf_free(p);
    p = NULL;
#else
    if (p->len > 0) {
        // TODO: relay
    }
#endif
    break;
#endif
  default:
    pbuf_free(p);
    p = NULL;
    break;
  }
}

#define MAX_CONNSYS_RX_ZERO_CP_PKT_CONTENT 128

uint8_t rssi_threshold_enable = 0;
int8_t rssi_threshold = 0;

uint8_t raw_packet_handler_enabled(void)
{
 return (netif_data_rx_filters[WLAN_RAW_PKT_HANDLER_IDX] != NULL);
}

void wifi_set_raw_rssi_threshold(uint8_t enable, int8_t rssi)
{
    rssi_threshold_enable = enable;
    rssi_threshold = rssi;
}

void wifi_get_raw_rssi_threshold(uint8_t *enable, int8_t *rssi)
{
    *enable = rssi_threshold_enable;
    *rssi = rssi_threshold;
}

uint32_t get_rx_vector(uint8_t group_vld)
{
    uint32_t grp3_offset = 0;
    
    // RX Status Group
    grp3_offset = sizeof(HW_MAC_RX_DESC_T);
    if (group_vld & BIT(RX_GROUP_VLD_4)) {
        grp3_offset += sizeof(HW_MAC_RX_STS_GROUP_4_T);
    }
    if (group_vld & BIT(RX_GROUP_VLD_1)) {
        grp3_offset += sizeof(HW_MAC_RX_STS_GROUP_1_T);
    }
    if (group_vld & BIT(RX_GROUP_VLD_2)) {
        grp3_offset += sizeof(HW_MAC_RX_STS_GROUP_2_T);
    }
    if (group_vld & BIT(RX_GROUP_VLD_3)) {
        return (uint32_t)grp3_offset;
    } else {
        return 0;
    }
}
int32_t check_raw_pkt_rssi_threshold(uint8_t *payload, uint32_t len)
{
    if(rssi_threshold_enable == 0)
        return 0;

    int32_t ret = 0;
    P_HW_MAC_RX_STS_GROUP_3_T grp3;
    uint32_t grp3_offset = 0;
    int8_t rssi_info;
    uint8_t grp_vld;

    /* grp3_offset = get_rx_vector(((*(first_dw)) >> 25) & 0xf);
     * same to the following two line
     */
    grp_vld = HAL_RX_STATUS_GET_GROUP_VLD((P_HW_MAC_RX_DESC_T)payload);
    grp3_offset = get_rx_vector(grp_vld);
    if (grp3_offset != 0) {
        grp3 = (P_HW_MAC_RX_STS_GROUP_3_T)(payload + grp3_offset);
        /* RSSI = RCPI/2 - 110
         * rssi_info = ((((grp3[2]) >> 9) & 0x7f) - 110) & 0xff;
         * The same to the following line code
         */
        rssi_info = (((HAL_RX_STATUS_GET_RCPI(grp3) >> 2) & 0xff) - 110) & 0xff;
    } else {
        rssi_info = -110;
    }

/*
    hex_dump("pkt rssi:", payload, grp3_offset+40);
    if(grp3_offset != 0){
        hex_dump("grp3:", grp3, sizeof(HW_MAC_RX_STS_GROUP_3_T));
    }
    printf("rssi:%d\n", rssi_info);
    */

    if(rssi_info < rssi_threshold)
        ret = -1;

    return ret;
}
void ethernetif_intr_enhance_mode_input_callback(void* pkt_ptr, int port, unsigned int len)
{
    struct pbuf *p = (struct pbuf *) pkt_ptr;
    struct netif *netif = NULL;
    int handled;

    if(p == NULL)
        return;
    WIFI_EVENT_DATA_PORT_T *wifi_event_hdr = p->payload;
    unsigned int packet_total_offset;

    if (p != NULL)
    {
        netif = &sta_if;

#if (CONNSYS_DEBUG_MODE_EN == 1)
        if (CONNSYS_TEST_DEBUG(CONNSYS_DBG_RX))
        {
#if 0
            uint32_t log_length = (len < 128 ? len : 128);
            LOG_I(lwip, "== RX Start ==:\n");
            LOG_I(lwip, "pkt_ptr = 0x%x, port = %d, len = %u\n",
                  (unsigned int)pkt_ptr,
                  (int)port,
                  (unsigned int)len);

            LOG_HEXDUMP_I(lwip, "pkt before process", p->payload, log_length);

            LOG_I(lwip, "wifi_event_hdr:\n");
            LOG_I(lwip, "\tu2Length = 0x%x, u2PktTypeID = 0x%x, ucPacketOffset = 0x%x\n",
                  wifi_event_hdr->u2Length,
                  wifi_event_hdr->u2PktTypeID,
                  wifi_event_hdr->ucPacketOffset);
#else
            DBG_CONNSYS(CONNSYS_DBG_RX, ("\n== Connsys_RX Start ==:\n"));
            DBG_CONNSYS(CONNSYS_DBG_RX, ("pkt_ptr = 0x%x, port = %d, len = %u\n",
                       (unsigned int)pkt_ptr,
                       (int)port,
                       (unsigned int)len));
            DBG_CONNSYS(CONNSYS_DBG_RX,("wifi_event_hdr:\n"));
            DBG_CONNSYS(CONNSYS_DBG_RX,("\tu2Length = 0x%x, u2PktTypeID = 0x%x, ucPacketOffset = 0x%x\n",
                        wifi_event_hdr->u2Length,
                        wifi_event_hdr->u2PktTypeID,
                        wifi_event_hdr->ucPacketOffset));
#endif
        }
#endif

        if (len >= WIFI_HIF_HEADER_LEN)
        {
            /* cmd packet */
            if (port == WIFI_HIF_RX_PORT0_IDX)
            {
                handled = INBAND_HANDLE_NON_HANDLE;
                if (!netif_inband_evt_filter)
                    LOG_E(lwip, "ERROR! No in-band event handler found!\n");
                else
                {
                    handled = netif_inband_evt_filter(pkt_ptr, p->payload, len);
                }

                if (handled != INBAND_HANDLE_EVENT_HANDLER)
                {
                    /* The packet is inband command and handle by rsp method. */
                    pbuf_free(p);
                    p = NULL;
                }
            }
            else  /* data packet */
            {
                int i, inf = 0;

                //struct eth_hdr *ethhdr;
                //struct etharp_hdr *hdr;

                handled = DATA_HANDLE_LWIP;
                inf = connsys_util_get_inf_number(p->payload);
                if (inf == 1)
                    netif = &ap_if;

                for (i = 0; i < DATA_RX_FILTER_COUNT; i++)
                {
                    netif_rx_filter_fn filter = netif_data_rx_filters[i];
                    if (filter != NULL)
                    {
                        if(check_raw_pkt_rssi_threshold(p->payload, len) < 0){
                            handled = DATA_HANDLE_RX_FILTER;
                            break;
                        }
                        handled = filter(p->payload, len);
                        if (handled) break;
                    }
                }
                if (handled == DATA_HANDLE_LWIP)
                {
#if (CFG_CONNSYS_IOT_RX_ZERO_COPY_EN == 1)
                    packet_total_offset = WIFI_HIF_HEADER_LEN + wifi_event_hdr->ucPacketOffset;
#else
                    packet_total_offset = WIFI_HIF_HEADER_LEN;
#endif
#if defined(MTK_BSP_LOOPBACK_ENABLE)
                    if (g_loopback_start)
                    {
                        //LOG_I(lwip, "offset=%d\n", packet_total_offset);
                        static unsigned int cnt = 0;
                        static unsigned int now = 0, start = 0;
                        cnt += (len- (WIFI_HIF_HEADER_LEN + wifi_event_hdr->aucReserved2[0]));
                        //LOG_I(lwip, "Loopback: %d bytes\n", cnt);
                        if (now == 0)
                        {
                            now = GPT_return_current_count(2); //DRV_Reg32(0x83050068);
                            start = GPT_return_current_count(2); //DRV_Reg32(0x83050068);
                        }
                        //LOG_I(lwip, "free pkt (%d) bytes\n", len);
                        //pbuf_free(p);
                        //p = NULL;
                        if (cnt > 1024000)
                        {
                            now = GPT_return_current_count(2); //DRV_Reg32(0x83050068);
                            LOG_I(lwip, "[LB] %d bytes, %d ms, %d Mbps\n", (int)cnt, (now - start)/32, (int)((cnt*8)*32/(now - start)/1000));
                            start = now;
                            cnt = 0;
                        }
                        packet_total_offset = WIFI_HIF_HEADER_LEN + wifi_event_hdr->aucReserved2[0]; // Use TX offset field, due to loopback
                    }
#endif // MTK_BSP_LOOPBACK_ENABLE
                    if (len >= packet_total_offset)
                    {
            /* skip HIF header 3DW = 12 bytes */
            p->payload = (void *)(((unsigned int)p->payload) + packet_total_offset);
            p->len = (len - packet_total_offset);
#if (CONNSYS_DEBUG_MODE_EN == 1)
        if (CONNSYS_TEST_DEBUG(CONNSYS_DBG_RX))
        {
#if 0
            uint32_t log_length = (len < 128 ? len : 128);
            LOG_HEXDUMP_I(lwip, "pkt after process", p->payload, log_length);
            /*
                            hex_dump_limit_len((char*)"pkt after processing ",
            (unsigned char*)p->payload,
                                (unsigned int)p->len,
                                (unsigned int)CONNSYS_MAX_RX_PKT_SIZE);
            */
            LOG_I(lwip, "== RX End ==\n");
#else
            DBG_CONNSYS(CONNSYS_DBG_RX, ("== Connsys_RX End ==\n\n"));

#endif
        }
#endif
        LINK_STATS_INC(link.recv);

              //ethhdr = (struct eth_hdr *)p->payload;
              //LOG_I(lwip, "Receive from inf(%d),type(%x vs %x),src[%2x:%2x:%2x:%2x:%2x:%2x], dest[%2x:%2x:%2x:%2x:%2x:%2x]...%s-%d\n",inf,ethhdr->type,PP_HTONS(ETHTYPE_ARP),ethhdr->src.addr[0], ethhdr->src.addr[1], ethhdr->src.addr[2], ethhdr->src.addr[3], ethhdr->src.addr[4], ethhdr->src.addr[5], ethhdr->dest.addr[0], ethhdr->dest.addr[1], ethhdr->dest.addr[2], ethhdr->dest.addr[3], ethhdr->dest.addr[4], ethhdr->dest.addr[5],__FUNCTION__,__LINE__);
                        ethernetif_intr_enhance_mode_dispatch(p, netif);
    }
    else
    {
                        p->len = len;
                        LOG_W(lwip, "WARN! len is less than WIFI HIF RX offset length..\n");
                        /*
                        hex_dump_limit_len((char*)"pkt",
                            (unsigned char*)p->payload,
                            (unsigned int)p->len,
                            CONNSYS_MAX_RX_PKT_SIZE);
                            */
                        pbuf_free(p);
                        p = NULL;
    }
                }
                else
    {
                    /* The packet is data packet and processed by data rx filter. */
                    pbuf_free(p);
                    p = NULL;
    }
    }
        }
        else
    {
            p->len = len;
            LOG_W(lwip, "WARN! len is less than WIFI HIF header length..\n");
#if (CONNSYS_DEBUG_MODE_EN == 1)
            LOG_HEXDUMP_I(lwip, "pkt", p->payload,len);
#endif
        pbuf_free(p);
        p = NULL;
    }
    }
    else
    {
        LOG_E(lwip, "%s pbuf_alloc fail\n\r", __FUNCTION__);
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
    }
}

void inband_handle_func(void* pkt_ptr, unsigned char *payload, unsigned int len)
{
    struct pbuf *p = (struct pbuf *) pkt_ptr;

    inband_queue_handler(pkt_ptr, payload, len);
    pbuf_free(p);
    pkt_ptr = NULL;
}


void ethernetif_init(sta_ip_mode_t sta_ip_mode,
                     uint8_t *sta_mac_addr,
                     uint8_t *ap_mac_addr,
                     ip4_addr_t *sta_ip_addr, ip4_addr_t *sta_net_mask, ip4_addr_t *sta_gw,
                     ip4_addr_t *ap_ip_addr, ip4_addr_t *ap_net_mask, ip4_addr_t *ap_gw,
                     uint8_t opmode)
{
    uint8_t sta_mac_address[6];
    uint8_t ap_mac_address[6];

    /* for patch and fw download */
    ethernetif_init_callback();

    memset(&sta_if, 0, sizeof(sta_if));
    memset(&ap_if,  0, sizeof(ap_if));

    // Note: *MUST* first add AP, then STA interface, to make STA the first
    //       interface in the link-list: STA -> AP -> NULL.
    if (0 > wifi_config_get_mac_address(WIFI_PORT_STA, (uint8_t *)&sta_mac_address) ||
        0 > wifi_config_get_mac_address(WIFI_PORT_AP, (uint8_t *)&ap_mac_address)) {
        LOG_E(lwip, "get mac fail\n\r");
        return;
    }

    netif_add(&ap_if, ap_ip_addr, ap_net_mask, ap_gw, NULL,
              ethernetif_init2, tcpip_input);
    netif_add(&sta_if, sta_ip_addr, sta_net_mask, sta_gw,
              NULL, ethernetif_init1, tcpip_input);

    low_level_set_mac_addr(&ap_if,  ap_mac_address);
    low_level_set_mac_addr(&sta_if, sta_mac_address);

    //netif_set_default(&sta_if);
    netif_set_up(&sta_if);
    netif_set_up(&ap_if);

    //install default route
    switch (opmode) {
        case WIFI_MODE_AP_ONLY:
            netif_set_default(&ap_if);
            netif_set_link_down(&sta_if);
            break;
        case WIFI_MODE_STA_ONLY:
            netif_set_default(&sta_if);
            netif_set_link_down(&ap_if);
            break;
        case WIFI_MODE_REPEATER:
            netif_set_default(&sta_if);
            break;
    }
}

void lwip_tcpip_init(lwip_tcpip_config_t *tcpip_config, uint8_t opmode)
{
    lwip_socket_init();
    tcpip_init(NULL, NULL);
    ethernetif_init(STA_IP_MODE_DHCP,
                    NULL,
                    NULL,
                    &tcpip_config->sta_addr,
                    &tcpip_config->sta_mask,
                    &tcpip_config->sta_gateway,
                    &tcpip_config->ap_addr,
                    &tcpip_config->ap_mask,
                    &tcpip_config->ap_gateway,
                    opmode);
}


static int lwip_get_netif_name(netif_type_t netif_type, char *name)
{
    struct netif *netif = NULL;

    if (name == NULL) {
        return 0;
    }

    for (netif = netif_list; netif != NULL; netif = netif->next) {
        if (netif_type == NETIF_TYPE_AP &&
                IFNAME10 == netif->name[0] &&
                IFNAME11 == netif->name[1]) {
            name[0] = IFNAME10;
            name[1] = IFNAME11;
            name[2] = '0' + netif->num;
            return 1;
        } else if (netif_type == NETIF_TYPE_STA &&
                   IFNAME00 == netif->name[0] &&
                   IFNAME01 == netif->name[1]) {
            name[0] = IFNAME00;
            name[1] = IFNAME01;
            name[2] = '0' + netif->num;
            return 1;
        } else if (netif_type == NETIF_TYPE_LOOPBACK &&
                   IFNAME20 == netif->name[0] &&
                   IFNAME21 == netif->name[1]) {
            name[0] = IFNAME20;
            name[1] = IFNAME21;
            name[2] = '0' + netif->num;
            return 1;
        }
    }

    return 0;
}

struct netif *netif_find_by_type(netif_type_t netif_type)
{
    char name[4] = {0};

    if (lwip_get_netif_name(netif_type, (char *)name) == 1) {
        LOG_I(lwip, "name=%s\n\r", name);
        return netif_find(name);
    } else {
        return NULL;
    }
}

void tcpip_stack_init(tcpip_config_t *tcpip_config, uint8_t opmode)
{
    lwip_socket_init();
    tcpip_init(NULL, NULL);
    ethernetif_init(tcpip_config->sta_ip_mode,
                    tcpip_config->sta_mac_addr,
                    tcpip_config->ap_mac_addr,
                    &tcpip_config->sta_addr,
                    &tcpip_config->sta_mask,
                    &tcpip_config->sta_gateway,
                    &tcpip_config->ap_addr,
                    &tcpip_config->ap_mask,
                    &tcpip_config->ap_gateway,
                    opmode);
}

