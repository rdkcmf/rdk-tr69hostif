/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2018 RDK Management
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
 * @file Device_WiFi_SSID_Stats.cpp
 *
 * @brief Device_WiFi_SSID API Implementation.
 *
 * This is the implementation of the WiFi API.
 *
 * @par Document
 */
/** @addtogroup TR-069 WiFi Implementation
 *  This is the implementation of the Device Public API.
 *  @{
 */

/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/
#ifdef USE_WIFI_PROFILE
#include "Device_WiFi_SSID_Stats.h"

GHashTable* hostIf_WiFi_SSID_Stats::ifHash = NULL;

hostIf_WiFi_SSID_Stats* hostIf_WiFi_SSID_Stats::getInstance(int dev_id)
{
    hostIf_WiFi_SSID_Stats* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_WiFi_SSID_Stats *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_WiFi_SSID_Stats(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create hostIf_WiFi_SSID_Stats instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_WiFi_SSID_Stats::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_WiFi_SSID_Stats::closeInstance(hostIf_WiFi_SSID_Stats *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_WiFi_SSID_Stats::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_WiFi_SSID_Stats* pDev = (hostIf_WiFi_SSID_Stats *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}


hostIf_WiFi_SSID_Stats::hostIf_WiFi_SSID_Stats(int dev_id):
    bytesSent(0),
    bytesReceived(0),
    packetsSent(0),
    packetsReceived(0),
    ErrorsSent(0),
    ErrorsReceived(0),
    UnicastPacketsSent(0),
    UnicastPacketsReceived(0),
    DiscardPacketsSent(0),
    DiscardPacketsReceived(0),
    MulticastPacketsSent(0),
    MulticastPacketsReceived(0),
    BroadcastPacketsSent(0),
    BroadcastPacketsReceived(0),
    UnknownProtoPacketsReceived(0)
{

}

/**
 * @brief    Get the MAC Address of an Associated Device of a MoCA Interface.
 *
 * This function provides the MAC address of the MoCA interface of the device associated
 * with this MoCA interface.
 *
 * See @ref dev_moca_if_assocdev_getter
 *
 */

int hostIf_WiFi_SSID_Stats::get_Device_WiFi_SSID_Stats_BytesSent(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}
int hostIf_WiFi_SSID_Stats::get_Device_WiFi_SSID_Stats_BytesReceived(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}
int hostIf_WiFi_SSID_Stats::get_Device_WiFi_SSID_Stats_PacketsSent(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}
int hostIf_WiFi_SSID_Stats::get_Device_WiFi_SSID_Stats_PacketsReceived(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}
int hostIf_WiFi_SSID_Stats::get_Device_WiFi_SSID_Stats_ErrorsSent(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}
int hostIf_WiFi_SSID_Stats::get_Device_WiFi_SSID_Stats_ErrorsReceived(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}
int hostIf_WiFi_SSID_Stats::get_Device_WiFi_SSID_Stats_UnicastPacketsSent(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}
int hostIf_WiFi_SSID_Stats::get_Device_WiFi_SSID_Stats_UnicastPacketsReceived(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}
int hostIf_WiFi_SSID_Stats::get_Device_WiFi_SSID_Stats_DiscardPacketsSent(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}
int hostIf_WiFi_SSID_Stats::get_Device_WiFi_SSID_Stats_DiscardPacketsReceived(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}
int hostIf_WiFi_SSID_Stats::get_Device_WiFi_SSID_Stats_MulticastPacketsSent(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}
int hostIf_WiFi_SSID_Stats::get_Device_WiFi_SSID_Stats_MulticastPacketsReceived(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}
int hostIf_WiFi_SSID_Stats::get_Device_WiFi_SSID_Stats_BroadcastPacketsSent(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}
int hostIf_WiFi_SSID_Stats::get_Device_WiFi_SSID_Stats_BroadcastPacketsReceived(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}
int hostIf_WiFi_SSID_Stats::get_Device_WiFi_SSID_Stats_UnknownProtoPacketsReceived(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

#endif /* #ifdef USE_WIFI_PROFILE */
