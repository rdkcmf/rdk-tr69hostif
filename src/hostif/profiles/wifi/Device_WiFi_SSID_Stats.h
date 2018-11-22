/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef DEVICE_WIFI_SSID_STATS_H_
#define DEVICE_WIFI_SSID_STATS_H_

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"


class hostIf_WiFi_SSID_Stats {

    static  GHashTable *ifHash;
    int dev_id;
    hostIf_WiFi_SSID_Stats(int dev_id);
    ~hostIf_WiFi_SSID_Stats() {};

public:
    static class hostIf_WiFi_SSID_Stats *getInstance(int dev_id);
    GList* getAllInstances();
    static void closeInstance(hostIf_WiFi_SSID_Stats *);
    static void closeAllInstances();

    unsigned long	bytesSent;
    unsigned long	bytesReceived;
    unsigned long	packetsSent;
    unsigned long	packetsReceived;
    unsigned int	ErrorsSent;
    unsigned int	ErrorsReceived;
    unsigned long	UnicastPacketsSent;
    unsigned long	UnicastPacketsReceived;
    unsigned int	DiscardPacketsSent;
    unsigned int	DiscardPacketsReceived;
    unsigned long	MulticastPacketsSent;
    unsigned long	MulticastPacketsReceived;
    unsigned long	BroadcastPacketsSent;
    unsigned long	BroadcastPacketsReceived;
    unsigned int	UnknownProtoPacketsReceived;

    /**
     * @brief    Get the MAC Address of an Associated Device of a WiFi Interface.
     *
     * This function provides the MAC address of the WiFi interface of the device associated
     * with this WiFi interface.
     *
     * See @ref dev_wifi_if_assocdev_getter
     *
     */

    int get_Device_WiFi_SSID_Stats_BytesSent(HOSTIF_MsgData_t *stMsgData );
    int get_Device_WiFi_SSID_Stats_BytesReceived(HOSTIF_MsgData_t *stMsgData );
    int get_Device_WiFi_SSID_Stats_PacketsSent(HOSTIF_MsgData_t *stMsgData );
    int get_Device_WiFi_SSID_Stats_PacketsReceived(HOSTIF_MsgData_t *stMsgData );
    int get_Device_WiFi_SSID_Stats_ErrorsSent(HOSTIF_MsgData_t *stMsgData );
    int get_Device_WiFi_SSID_Stats_ErrorsReceived(HOSTIF_MsgData_t *stMsgData );
    int get_Device_WiFi_SSID_Stats_UnicastPacketsSent(HOSTIF_MsgData_t *stMsgData );
    int get_Device_WiFi_SSID_Stats_UnicastPacketsReceived(HOSTIF_MsgData_t *stMsgData );
    int get_Device_WiFi_SSID_Stats_DiscardPacketsSent(HOSTIF_MsgData_t *stMsgData );
    int get_Device_WiFi_SSID_Stats_DiscardPacketsReceived(HOSTIF_MsgData_t *stMsgData );
    int get_Device_WiFi_SSID_Stats_MulticastPacketsSent(HOSTIF_MsgData_t *stMsgData );
    int get_Device_WiFi_SSID_Stats_MulticastPacketsReceived(HOSTIF_MsgData_t *stMsgData );
    int get_Device_WiFi_SSID_Stats_BroadcastPacketsSent(HOSTIF_MsgData_t *stMsgData );
    int get_Device_WiFi_SSID_Stats_BroadcastPacketsReceived(HOSTIF_MsgData_t *stMsgData );
    int get_Device_WiFi_SSID_Stats_UnknownProtoPacketsReceived(HOSTIF_MsgData_t *stMsgData );
};


#endif /* DEVICE_WIFI_SSID_STATS_H_ */


/** @} */
/** @} */
