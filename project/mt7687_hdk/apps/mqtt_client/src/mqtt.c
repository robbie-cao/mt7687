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

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "network_init.h"
#include "wifi_api.h"

#include "MQTTClient.h"
#include "mqtt.h"
#include "syslog.h"


#define MQTT_SERVER		"iot.eclipse.org"//"test.mosquitto.org"
#define MQTT_PORT		"1883"
#define MQTT_TOPIC		"7687test"
#define MQTT_CLIENT_ID	"mqtt-7687-client"
#define MQTT_MSG_VER	"0.50"


static int arrivedcount = 0;

/**
* @brief          MQTT message RX handle
* @param[in]      MQTT received message data
* @return         None
*/
static void messageArrived(MessageData *md)
{
    MQTTMessage *message = md->message;

    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n", message->qos, message->retained, message->dup, message->id);
    printf("Payload %d.%s\n", (size_t)(message->payloadlen), (char *)(message->payload));
    ++arrivedcount;
}

/**
* @brief          MQTT client example entry function
* @return         None
*/
void mqtt_client_example(void)
{
    int rc = 0;
    unsigned char msg_buf[100];     //Buffer for outgoing messages, such as unsubscribe.
    unsigned char msg_readbuf[100]; //Buffer for incoming messages, such as unsubscribe ACK.
    char buf[100];                  //Buffer for application payload.

    Network n;  //TCP network
    Client c;   //MQTT client
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    char *topic = MQTT_TOPIC;
    MQTTMessage message;

    //Initialize MQTT network structure
    NewNetwork(&n);

    //Connect to remote server
    printf("Connect to %s:%s\n", MQTT_SERVER, MQTT_PORT);
    rc = ConnectNetwork(&n, MQTT_SERVER, MQTT_PORT);

    if (rc != 0) {
        printf("TCP connect failed,status -%4X\n", -rc);
        return;
    }

    //Initialize MQTT client structure
    MQTTClient(&c, &n, 12000, msg_buf, 100, msg_readbuf, 100);

    //The packet header of MQTT connection request
    data.willFlag = 0;
    data.MQTTVersion = 3;
    data.clientID.cstring = MQTT_CLIENT_ID;
    data.username.cstring = NULL;
    data.password.cstring = NULL;
    data.keepAliveInterval = 10;
    data.cleansession = 1;

    //Send MQTT connection request to the remote MQTT server
    rc = MQTTConnect(&c, &data);

    if (rc != 0) {
        printf("MQTT connect failed,status%d\n", rc);
    }

    printf("Subscribing to %s\n", topic);
    rc = MQTTSubscribe(&c, topic, QOS1, messageArrived);
    printf("Client Subscribed %d\n", rc);


    // QoS 0
    sprintf(buf, "Hello World! QoS 0 message from app version %s\n", MQTT_MSG_VER);
    message.qos = QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void *)buf;
    message.payloadlen = strlen(buf) + 1;
    rc = MQTTPublish(&c, topic, &message);

    while (arrivedcount < 1) {
        MQTTYield(&c, 1000);
    }

    // QoS 1
    sprintf(buf, "Hello World! QoS 1 message from app version %s\n", MQTT_MSG_VER);
    message.qos = QOS1; 
    message.payloadlen = strlen(buf) + 1;
    rc = MQTTPublish(&c, topic, &message);
    while (arrivedcount < 2) {
        MQTTYield(&c, 1000);
    }

    // QoS 2
    sprintf(buf, "Hello World! QoS 2 message from app version %s\n", MQTT_MSG_VER);
    message.qos = QOS2;
    message.payloadlen = strlen(buf) + 1;
    rc = MQTTPublish(&c, topic, &message);
    while (arrivedcount < 3) {
        MQTTYield(&c, 1000);
    }

    if ((rc = MQTTUnsubscribe(&c, topic)) != 0) {
        printf("The return from unsubscribe was %d\n", rc);
    }
    printf("MQTT unsubscribe done\n");

    if ((rc = MQTTDisconnect(&c)) != 0) {
        printf("The return from disconnect was %d\n", rc);
    }
    printf("MQTT disconnect done\n");

    n.disconnect(&n);
    printf("Network disconnect done\n");

}



