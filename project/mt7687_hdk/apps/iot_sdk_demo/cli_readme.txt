/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/**
  * @page MT7687 iot_sdk_demo CLI introduction
  * @{    
@par Command Overview
   This document provides details on how to set up and use Command Line Interface (CLI) and the supported command set.



   The four groups of commands are shown below. 
   - wifi api:         The Wi-Fi related commands,            e.g., "wifi config set opmode 1".
   - smart connection: The Smart Connection related commands, e.g., "smart connect".
   - ping:             The PING related commands,             e.g., "ping 192.168.0.1".
   - iperf:            The iperf related commands,            e.g., "iperf -s".
   For more details, please refer to the section, "Supported commands".

@par Environment setup and the first steps to CLI
   - Connect the board to PC with a serial port cable.
     Use Hyper-Terminal, SecureCRT or Tera Term as the terminal emulator to connect the serial port,
     to input the commands and display the response.
   - Use '?' and enter to list the available commands and their options. The result is:
         +-----------------------+----------------------------------------+
         | user input            | ?                                      |
         +-----------------------+----------------------------------------+
         | output of the command | wifi   - wifi api                      |
         |                       | smart  - smart connection              |
         |                       | ping   - ping <addr> <count> <pkt_len> |
         |                       | iperf  - iperf                         |
         +-----------------------+----------------------------------------+

   - Type in a particular command to get the details of the the command options.
     Type in "wifi", for example, to get the details of the Wi-Fi command options, as shown below.
         +-----------------------+-------------------------------------+
         | user input            | wifi                                |
         +-----------------------+-------------------------------------+
         | output of the command | incomplete command, more options:   |
         |                       | config - wifi config                |
         |                       | connect - wifi connect              |
         +-----------------------+-------------------------------------+

@par Supported commands
   - 'wifi' commands

      - wifi config set opmode <mode>
      - wifi config get opmode
        <mode>: 1: Station ; 2: AP;  3: Repeater
        Example:
        wifi config set opmode 1    : Set the current mode to Station mode.
        wifi config set opmode 2    : Set the current mode to AP mode.
        wifi config set opmode 3    : Set the current mode to Repeater mode.
        wifi config get opmode      : Display the current mode.


      - wifi config set ssid <port> <ssid>  
      - wifi config get ssid  <port>
        <port>: 0: STA;    1: AP
        <ssid>: The SSID name to set, the maximum length is 32 characters.
        Example:
        wifi config set ssid 0 TargetAP : In Station mode, set the target SSID as "TargetAP" that 
                                          the station connects to.
        wifi config set ssid 1 MyAP     : In AP mode, set the SSID of the AP as "MyAP".
        wifi config get ssid 0          : In Station mode, display the current SSID that the station 
                                          is connecting to.
        wifi config get ssid 1          : In AP mode, display the SSID of the AP.


      - wifi config set psk <port> <auth> <encrypt> <pwd>
      - wifi config set psk <port> <pwd>
      - wifi config get psk <port>
        <port>: 0: STA;    1: AP
        <pwd>: The configured password,
               the length of <pwd> must be between 8 and 64 characters.
               If auth=0 and encrypt=1/0, the value of <pwd> will be ignored,
               but the user still needs to input a value for <pwd>.
        The supported values for the options of <auth> and <encrypt>:
        For STA Port, the <auth> and <encrypt> are optional.
        For AP Port, the <auth> and <encrypt> are mandatory.
         +---+-------------------------------------+
         | 0 | open, no security                   |
         +---+-------------------------------------+
         | 4 | WPA PSK                             |
         +---+-------------------------------------+
         | 7 | WPA2 PSK                            |
         +---+-------------------------------------+
         | 9 | Support both WPA and WPA2 PSK       |
         +---+-------------------------------------+
         Table 1. Supported <auth>

         +---+------------------------+
         | 0 | WEP                    |
         +---+------------------------+
         | 1 | No encryption          |
         +---+------------------------+
         | 4 | TKIP                   |
         +---+------------------------+
         | 6 | AES                    |
         +---+------------------------+
         | 8 | Support TKIP and AES   |
         +---+------------------------+
         Table 2. Supported <encrypt>
        Example: 
        wifi config set psk 0 0 1 12345678 : In Station mode, set the security mode as open,
                                             and the station uses this setting to connect to the AP with Open mode.
                                             The password of "12345678" is ignored.
        wifi config set psk 0 0 0 12345678 : In Station mode, set the security mode as WEP-Open
                                             and the station uses this setting to connect to the AP with WEP-Open mode.
                                             The password of "12345678" is ignored.
                                             Input the following command: wifi config set wep <port> <key_id> <key_string> to set the wep key
        wifi config set psk 0 7 6 12345678 : In Station mode, set the security mode as WPA2-PSK AES
                                             and the station uses this setting to connect to the AP with WPA2-PSK AES mode.
                                             The password of "12345678" will be used for the connection.
        wifi config set psk 0 12345678     : In Station mode, the station will auto detect the target AP's security mode
                                             and use "12345678" as password if AP is WPA-PSK,WPA2-PSK or WPA/WPA2-PSK.
                                             The password of "12345678" is ignored if the AP is in Open or WEP-Open mode,
                                             use the password set by the following command: wifi config set wep <port> <key_id> <key_string>.
        wifi config set psk 1 0 1 12345678 : In AP mode, set the security mode as open, and the "12345678" is ignored.
        wifi config set psk 1 9 8 12345678 : In AP mode, set the security mode as WPA/WPA2-PSK mix, and the password as "12345678".
        
        
      - wifi config set pmk <port> <pmk>
      - wifi config get pmk <port>
        <port>: 0: STA;    1: AP
        <pmk>: Hex:  00, 05, 03 ¡­¡­(size 32)
        Example:
        wifi config set pmk 0 00,11,22,33,44,55,66,77,88,99,aa,bb,cc,dd,ee,ff,00,11,22,33,44,55,66,77,88,99,aa,bb,cc,dd,ee,ff
                                           : In Station mode, set the PMK value, and the station will use this value to connect to the AP. 
        wifi config get pmk                : In Station mode, get the PMK used in current connection.
        
        
      - wifi config set wep <port> <key_id> <key_string>
      - wifi config get wep <port>
        <port>: 0: STA;    1: AP
        <key_id>: WEP key index  (0~3)
        <key_string>: four group WEP key, ex: 12345,12345,12345,12345
        Example:
        wifi config set wep 0 0 12345,12345,12345,12345 
                                           : In Station mode, set the WEP key and key index, and the station will use these values to connect AP. 
        wifi config get wep 0              : In Station mode, get the WEP key used in current connection.
        
        
      - wifi config set ch <port> <ch>
      - wifi config get ch <port>
        <port>: 0: STA;    1: AP
        <ch>:1~14 are supported for 2.4G only.
        Example:
        wifi config set ch 0 12            : In Station mode, set the current channel.
        wifi config get ch 0               : In Station mode, get the current channel.
        
        
      - wifi config set bw <port> <bw>
      - wifi config get bw <port>  
      - wifi config set bw_extended <port> <bw> <extend_ch>
      - wifi config get bw_extended <port> 
        <port>: 0: STA;    1: AP
        <bw>: 0: 20MHZ; 1: 20/40MHZ
        <extend_ch>: 0: none; 1: below; 3: above      
                     the option of above and below are only available when bandwidth is 40MHz
        Example:
        wifi config set bw 0 1             : In Station mode, set the current bandwidth as 20MHz.
        wifi config set bw_extended 0 1 3  : In Station mode, set the current bandwidth as 40MHz-above.
        wifi config get bw 0               : In Station mode, get the current bandwidth.
        wifi config get bw_extended 0      : In Station mode, get the current bandwidth with extend channel attribute.
        
      - wifi config set wirelessmode <port> <wirelessmode>
      - wifi config get wirelessmode <port>
        configuration options for <wirelessmode>:
         +---+------------------------------+
         | 0 | 11BG mixed                   |
         +---+------------------------------+
         | 1 | 11B only                     |
         +---+------------------------------+
         | 2 | 11A only                     |
         +---+------------------------------+
         | 3 | 11A/B/G mixed                |
         +---+------------------------------+
         | 4 | 11G only                     |
         +---+------------------------------+
         | 5 | 11ABGN mixed                 |
         +---+------------------------------+
         | 6 | 11N only in 2.4G             |
         +---+------------------------------+
         | 7 | 11GN mixed                   |
         +---+------------------------------+
         | 8 | 11AN mixed                   |
         +---+------------------------------+
         | 9 | 11BGN mixed                  |
         +---+------------------------------+
         | 10| 11AGN mixed                  |
         +---+------------------------------+
         | 11| 11N  5G                      |
         +---+------------------------------+
         Table 3. Supported <wirelessmode>
        Example:
        wifi config set wirelessmode 0 1   : In Station mode, set the current wireless mode as 11B only.
        wifi config get wirelessmode 0     : In Station mode, get the current wireless mode.


      - wifi config set country <band> <region>
      - wifi config get country <band>
        <band>: 0: 2.4G band; 1: 5G band
        <region>: 2.4G: region 0-7,31-33
                  5G: region 0-22, 30-37
        Example:
        wifi config set country 0 1   : Set the current country region as 1 for 2.4G band.
        wifi config set country 1 22  : Set the current country region as 22 for 5G band.
        wifi config get country 0     : get the current country region of 2.4G band.
        wifi config get country 1     : get the current country region of 5G band.
        
        
      - wifi config set dtim <dtim interval>  
      - wifi config get dtim 
        <dtim interval>: 1-255 


      - wifi config set listen <listen interval>
      - wifi config get listen
        <listen interval>: 1-255


      - wifi config set mtksmart <flag>
      - wifi config get mtksmart
        <flag>: 0:  disable; 1:  enable.   Set  MediaTek Smart Connection Filter.
        Example:
        wifi config set mtksmart 1    : Enable MediaTek Smart Connection Filter to cooperate with the 
                                        RX filter set by the command: wifi config set rxfilter <flag>.
        wifi config set mtksmart 0    : Disable MediaTek Smart Connection Filter.
        
        
      - wifi config set radio <on/off>
      - wifi config get radio
        <on/off>: 0:  OFF; 1:  ON


      - wifi config set rxfilter <flag>
      - wifi config get rxfilter 
        <flag>: is defined in wifi_rx_filter_t
        wifi config set rxfilter 0x1de00a : Set the RX filter settings. To find more on the setting "0x1de00a", 
                                            please refer to the structure wifi_rx_filter_t.
        wifi config get rxfilter          : Get current RX filter settings.

      - wifi config set rxraw <enable>
        <enable>: 0: Unregister the RX raw handler;    1: Register the RX raw handler
        Example:
        wifi config set rxraw 1 : Register an example handler and enable receive the RX raw packets.
                                  when one raw packet is received, the handler is triggered.
        wifi config set rxraw 0 : Unregister the example handler and disable receive the RX raw packets.
 
 
      - wifi config set txraw <raw packets>
        Send TX RAW packet
        <raw packets>: User-defined packet which is a complete Wi-Fi 802.11 packet including the 802.11 header and the payload.
        Example:
        wifi config set txraw 48113C00D0C7C08200437C1DD97038BFD0C7C08200437001B3015194 : send a NULL packet.
        
        
      - wifi config get mac <port>
        Get MAC address


      - wifi config set reload
        Reload configuration


      - wifi connect set scan <start/stop> <scan mode> <scan operation> [ssid] [bssid]
        <start/stop>: 0: stop; 1: start
        <scan mode>: 0:  full; 1:  partial
        <scan operation>: 0: active(passive in regulatory channel); 
                          1: passive in all channel; 
                          2: force active(active in all channel)
        [ssid]:  not specified
        [bssid]: not specified
        Example:
        wifi connect set scan 1 0 0  : Start full active scan.
        wifi connect set scan 0      : Stop scan.
 

      - wifi connect set connection <link>
        <link>: 0: link down (wifi connection disconnect ap)
                1: link up   (wifi config reload setting)
        Example:
        wifi connect set connection 0  : In Station mode, disconnect the AP.
        wifi connect set connection 1  : To reload Wi-Fi configuration, Use a command : wifi config set reload.
        

      - wifi connect set deauth <MAC> 
        <MAC>: STA  mac address
        Example:
        wifi connect set deauth 01:02:03:04:05:06  : In AP mode, disconnect the station with the MAC address of 01:02:03:04:05:06.
        
        
      - wifi connect set eventcb <enable> <enable ID>
        <enable>: 0: Unregister;  1:Register
        <enable ID>: 0: connected event
                     1: scan complete event
                     2: disconnect event
                     3: port secure event
                     4: report beacon/probe response frames.
        Example:
        wifi connect set eventcb 1 0  : Register the example handler for connected event, when the event is reported,the handler will be triggered.
        wifi connect set eventcb 0 0  : Unregister the example handler for the connected event.
     

      - wifi connect get linkstatus
        Return the link status only for station mode.
        0:  disconnected
        1:  connected

      - wifi connect get stalist
        Return the number of associated stations only for AP and repeater mode.

      - wifi connect get rssi
        Get the RSSI of the connected AP, only used for STA mode and when the station is connected to the AP.

      - wifi connect get max_num_sta
        Get the maximum number of supported stations in AP mode or dual mode.

   - 'smart' commands

      - smart connect
        Start Smart Connection.

      - smart stop
        Stop Smart Connection.

   - 'ping' commands
   
      - ping <addr> <count> <pkt_len>


   - 'iperf' commands

      - iperf [-h]

      - iperf [-s|-c] [options]
        
        Client/Server:
          -u,    use UDP rather than TCP.
          -p,    #    server port to listen on/connect to (default 5001).
          -n,    #[kmKM]    number of bytes to transmit.
          -b,    #[kmKM]    for UDP, bandwidth to send at in bits/sec.
          -i,    10 seconds between periodic bandwidth reports.

        Server specific:
          -s,    run in server mode.
          -B,    <ip>    bind to <ip>, and join to a multicast group (only supports UDP).
          -r,    for UDP, run iperf in tradeoff testing mode, connecting back to client.

        Client specific:
          -c,    <ip>    run in client mode, connecting to <ip>.
          -w,    #[kmKM]    TCP window size.
          -l,    #[kmKM]    UDP datagram size.
          -t,    #    time in seconds to transmit for (default 10 secs).
          -S,    #    the type-of-service of outgoing packets.

        Miscellaneous:
          -h,    print this message and quit.

        [kmKM] Indicates options that support a k/K or m/M suffix for kilo- or mega-.

        TOS options for -S parameter:.
        BE: -S 0
        BK: -S 32
        VI: -S 160
        VO: -S 224

        Tradeoff Testing Mode:
        Command: iperf -s -u -n <bits/bytes> -r

        Example:
        Iperf TCP Server: iperf -s
        Iperf UDP Server: iperf -s -u
        Iperf TCP Client: iperf -c <ip> -w <window size> -t <duration> -p <port>
        Iperf UDP Client: iperf -c <ip> -u -l <datagram size> -t <duration> -p <port>


* @}
*/