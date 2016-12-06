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
 * @file Device_Ethernet_Interface_Stats.cpp
 * @brief This source file contains the APIs of device ethernet interface stats.
 */

/**
 * @file Device_Ethernet_Interface.c
 *
 * @brief Device.Ethernet.Interface. API Implementation.
 *
 * This is the implementation of the Device.Ethernet.Interface. API.
 *
 * @par Document
 * TBD Relevant design or API documentation.
 *
 */


/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/



/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#include <sys/sysinfo.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "Device_Ethernet_Interface_Stats.h"

EthernetInterfaceStats hostIf_EthernetInterfaceStats::stEthInterfaceStats = {0,};

GMutex* hostIf_EthernetInterfaceStats::m_mutex = NULL;

GHashTable* hostIf_EthernetInterfaceStats::ifHash = NULL;

//Returns the number of ethernet interfaces.
extern int ethCount();

static int read_Device_Ethernet_Interface_Stats_Fields(unsigned int ethInterfaceNum, EEthInterfaceStatsMembers ethInterfaceStatMem)
{
    FILE *fp;
    char resultBuff[BUFF_LENGTH] = {'\0'};
    char cmd[BUFF_LENGTH] = {'\0'};
    int temp;

    switch(ethInterfaceStatMem)
    {
    case eBytesSent:
        sprintf(cmd,"cat /sys/class/net/eth%d/statistics/tx_bytes",ethInterfaceNum-1);

        fp = popen(cmd,"r");

        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"\n read_Device_Ethernet_Interface_Stats_Fields(): Error in popen eBytesSent\n");

            return 0;
        }

        if(fgets(resultBuff,1024,fp)!=NULL)
        {
            sscanf(resultBuff,"%lu",&hostIf_EthernetInterfaceStats::stEthInterfaceStats.bytesSent);
        }

        pclose(fp);
        break;

    case eBytesReceived:
        sprintf(cmd,"cat /sys/class/net/eth%d/statistics/rx_bytes",ethInterfaceNum-1);

        fp = popen(cmd,"r");

        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"\n read_Device_Ethernet_Interface_Stats_Fields(): Error in popen eBytesReceived\n");

            return 0;
        }

        if(fgets(resultBuff,1024,fp)!=NULL)
        {
            sscanf(resultBuff,"%lu",&hostIf_EthernetInterfaceStats::stEthInterfaceStats.bytesReceived);
        }

        pclose(fp);
        break;
    case ePacketsSent:
    case eUnicastPacketsSent:
    case eMulticastPacketsSent:
    case eBroadcastPacketsSent:
        sprintf(cmd,"cat /sys/class/net/eth%d/statistics/tx_packets",ethInterfaceNum-1);

        fp = popen(cmd,"r");

        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"\n read_Device_Ethernet_Interface_Stats_Fields(): Error in popen ePacketsSent\n");

            return 0;
        }

        if(fgets(resultBuff,1024,fp)!=NULL)
        {
            sscanf(resultBuff,"%lu",&hostIf_EthernetInterfaceStats::stEthInterfaceStats.packetsSent);
            sscanf(resultBuff,"%lu",&hostIf_EthernetInterfaceStats::stEthInterfaceStats.unicastPacketsSent);
            sscanf(resultBuff,"%lu",&hostIf_EthernetInterfaceStats::stEthInterfaceStats.multicastPacketsSent);
            sscanf(resultBuff,"%lu",&hostIf_EthernetInterfaceStats::stEthInterfaceStats.broadcastPacketsSent);
        }

        pclose(fp);
        break;

    case ePacketsReceived:
    case eUnicastPacketsReceived:
    case eMulticastPacketsReceived:
    case eBroadcastPacketsReceived:
        sprintf(cmd,"cat /sys/class/net/eth%d/statistics/rx_packets",ethInterfaceNum-1);

        fp = popen(cmd,"r");

        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"\n read_Device_Ethernet_Interface_Stats_Fields(): Error in popen ePacketsReceived\n");

            return 0;
        }

        if(fgets(resultBuff,1024,fp)!=NULL)
        {
            sscanf(resultBuff,"%lu",&hostIf_EthernetInterfaceStats::stEthInterfaceStats.packetsReceived);
            sscanf(resultBuff,"%lu",&hostIf_EthernetInterfaceStats::stEthInterfaceStats.unicastPacketsReceived);
            sscanf(resultBuff,"%lu",&hostIf_EthernetInterfaceStats::stEthInterfaceStats.multicastPacketsReceived);
            sscanf(resultBuff,"%lu",&hostIf_EthernetInterfaceStats::stEthInterfaceStats.broadcastPacketsReceived);
        }

        pclose(fp);
        break;

    case eErrorsSent:
        sprintf(cmd,"cat /sys/class/net/eth%d/statistics/tx_errors",ethInterfaceNum-1);

        fp = popen(cmd,"r");

        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"\n read_Device_Ethernet_Interface_Stats_Fields(): Error in popen eErrorsSent\n");

            return 0;
        }

        if(fgets(resultBuff,1024,fp)!=NULL)
        {
            sscanf(resultBuff,"%u",&hostIf_EthernetInterfaceStats::stEthInterfaceStats.errorsSent);
        }

        pclose(fp);
        break;

    case eErrorsReceived:
        sprintf(cmd,"cat /sys/class/net/eth%d/statistics/rx_errors",ethInterfaceNum-1);

        fp = popen(cmd,"r");

        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"\n read_Device_Ethernet_Interface_Stats_Fields(): Error in popen eErrorsReceived\n");

            return 0;
        }

        if(fgets(resultBuff,1024,fp)!=NULL)
        {
            sscanf(resultBuff,"%u",&hostIf_EthernetInterfaceStats::stEthInterfaceStats.errorsReceived);
        }

        pclose(fp);
        break;

    case eDiscardPacketsSent:
        sprintf(cmd,"cat /sys/class/net/eth%d/statistics/tx_dropped",ethInterfaceNum-1);

        fp = popen(cmd,"r");

        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"\n read_Device_Ethernet_Interface_Stats_Fields(): Error in popen eDiscardPacketsSent \n");

            return 0;
        }

        if(fgets(resultBuff,1024,fp)!=NULL)
        {
            sscanf(resultBuff,"%u",&hostIf_EthernetInterfaceStats::stEthInterfaceStats.discardPacketsSent);
        }

        pclose(fp);
        break;

    case eDiscardPacketsReceived:
    case eUnknownProtoPacketsReceived:
        sprintf(cmd,"cat /sys/class/net/eth%d/statistics/rx_dropped",ethInterfaceNum-1);

        fp = popen(cmd,"r");

        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"\n read_Device_Ethernet_Interface_Stats_Fields(): Error in popen eDiscardPacketsReceived \n");

            return 0;
        }

        if(fgets(resultBuff,1024,fp)!=NULL)
        {
            sscanf(resultBuff,"%u",&hostIf_EthernetInterfaceStats::stEthInterfaceStats.discardPacketsReceived);
            sscanf(resultBuff,"%u",&hostIf_EthernetInterfaceStats::stEthInterfaceStats.unknownProtoPacketsReceived);
        }

        pclose(fp);
        break;

    default:
        break;
    }

    return 0;
}


hostIf_EthernetInterfaceStats* hostIf_EthernetInterfaceStats::getInstance(int dev_id)
{
    hostIf_EthernetInterfaceStats* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_EthernetInterfaceStats *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_EthernetInterfaceStats(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create Ethernet Interface instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_EthernetInterfaceStats::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_EthernetInterfaceStats::closeInstance(hostIf_EthernetInterfaceStats *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_EthernetInterfaceStats::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_EthernetInterfaceStats* pDev = (hostIf_EthernetInterfaceStats *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

void hostIf_EthernetInterfaceStats::getLock()
{

    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);

}

void hostIf_EthernetInterfaceStats::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

/**
 * @brief Class Constructor of the class hostIf_EthernetInterfaceStats.
 *
 * It will initialize the device id.
 *
 * @param[in] dev_id Device identification Number.
 */
hostIf_EthernetInterfaceStats::hostIf_EthernetInterfaceStats(int dev_id):
    dev_id(dev_id),
    backupBytesSent(0),
    backupBytesReceived(0),
    backupPacketsSent(0),
    backupPacketsReceived(0),
    backupUnicastPacketsSent(0),
    backupUnicastPacketsReceived(0),
    backupMulticastPacketsReceived(0),
    backupBroadcastPacketsSent(0),
    backupBroadcastPacketsReceived(0),
    backupMulticastPacketsSent(0),
    backupErrorSent(0),
    backupErrorsReceived(0),
    backupErrorsSent(0),
    backupDiscardPacketsReceived(0),
    bCalledBytesSent(false),
    bCalledBytesReceived(false),
    bCalledPacketsSent(false),
    bCalledPacketsReceived(false),
    bCalledErrorSent(false),
    bCalledErrorsReceived(false),
    bCalledErrorsSent(false),
    bCalledUnicastPacketsSent(false),
    bCalledDiscardPacketsReceived(false),
    bCalledDiscardPacketsSent(false),
    bCalledUnicastPacketsReceived(false),
    bCalledMulticastPacketsReceived(false),
    bCalledBroadcastPacketsSent(false),
    bCalledBroadcastPacketsReceived(false),
    bCalledMulticastPacketsSent(false),
    bCalledUnknownProtoPacketsReceived(false)

{
}

/****************************************************************************************************************************************************/
// Device.Ethernet.Interface.Stats. Profile. Getters:
/****************************************************************************************************************************************************/

/**
 * @brief This function provides the total number of bytes transmitted out of this Ethernet
 * interface, including framing characters.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_STATS_API
 */
int hostIf_EthernetInterfaceStats::get_Device_Ethernet_Interface_Stats_BytesSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{

    read_Device_Ethernet_Interface_Stats_Fields(dev_id,eBytesSent);
    if(bCalledBytesSent && pChanged && (backupBytesSent != stEthInterfaceStats.bytesSent))
    {
        *pChanged = true;
    }
    bCalledBytesSent = true;
    backupBytesSent = stEthInterfaceStats.bytesSent;
    put_int(stMsgData->paramValue,stEthInterfaceStats.bytesSent);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function provides the total number of bytes received on this Ethernet
 * interface, including framing characters.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Return '0' if the method successfully received the total number of bytes
 * via ethernet interface else returns '-1'.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_STATS_API
 */
int hostIf_EthernetInterfaceStats::get_Device_Ethernet_Interface_Stats_BytesReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{

    read_Device_Ethernet_Interface_Stats_Fields(dev_id,eBytesReceived);
    if(bCalledBytesReceived && pChanged && (backupBytesReceived != stEthInterfaceStats.bytesReceived))
    {
        *pChanged = true;
    }
    bCalledBytesReceived = true;
    backupBytesReceived = stEthInterfaceStats.bytesReceived;
    put_int(stMsgData->paramValue,stEthInterfaceStats.bytesReceived);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function provides the total number of packets transmitted out of this Ethernet interface.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully sent the packets via ethernet interface
 * else returns '-1'.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_STATS_API
 */
int hostIf_EthernetInterfaceStats::get_Device_Ethernet_Interface_Stats_PacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{

    read_Device_Ethernet_Interface_Stats_Fields(dev_id,ePacketsSent);
    if(bCalledPacketsSent && pChanged && (backupPacketsSent != stEthInterfaceStats.packetsSent))
    {
        *pChanged = true;
    }
    bCalledPacketsSent = true;
    backupPacketsSent = stEthInterfaceStats.packetsSent;
    put_int(stMsgData->paramValue,stEthInterfaceStats.packetsSent);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function provides the total number of packets received on this Ethernet interface.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully receive the total number of packets via
 * ethernet interface else returns '1'.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_STATS_API
 */
int hostIf_EthernetInterfaceStats::get_Device_Ethernet_Interface_Stats_PacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{

    read_Device_Ethernet_Interface_Stats_Fields(dev_id,ePacketsReceived);
    if(bCalledPacketsReceived && pChanged && (backupPacketsReceived != stEthInterfaceStats.packetsReceived))
    {
        *pChanged = true;
    }
    bCalledPacketsReceived = true;
    backupPacketsReceived = stEthInterfaceStats.packetsReceived;
    put_int(stMsgData->paramValue,stEthInterfaceStats.packetsReceived);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function provides the total number of outbound packets that could not be
 * transmitted because of errors.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully sent the errors via ethernet
 * interface else returns '-1'.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_STATS_API
 */
int hostIf_EthernetInterfaceStats::get_Device_Ethernet_Interface_Stats_ErrorsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{

    read_Device_Ethernet_Interface_Stats_Fields(dev_id,eErrorsSent);
    if(bCalledErrorSent && pChanged && (backupErrorSent != stEthInterfaceStats.errorsSent))
    {
        *pChanged = true;
    }
    bCalledErrorSent = true;
    backupErrorSent = stEthInterfaceStats.errorsSent;
    put_int(stMsgData->paramValue,stEthInterfaceStats.errorsSent);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function provides the total number of inbound packets that contained errors
 * preventing them from being delivered to a higher-layer protocol.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully receive the errors via ethernet
 * interface stats else returns '-1'.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_STATS_API
 */
int hostIf_EthernetInterfaceStats::get_Device_Ethernet_Interface_Stats_ErrorsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{

    read_Device_Ethernet_Interface_Stats_Fields(dev_id,eErrorsReceived);
    if(bCalledErrorsReceived && pChanged && (backupErrorsReceived != stEthInterfaceStats.errorsReceived))
    {
        *pChanged = true;
    }
    bCalledErrorsReceived = true;
    backupErrorsReceived = stEthInterfaceStats.errorsReceived;
    put_int(stMsgData->paramValue,stEthInterfaceStats.errorsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function get the number of unicast packets for which a request is send on the IP
 * Interface was received. This function provides the total number of packets requested for
 * transmission which were not addressed to a multicast or broadcast address at this layer,
 * including those that were discarded or not sent.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully sent the unicast packets via
 * ethernet interface else returns '-1'.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_STATS_API
 */
int hostIf_EthernetInterfaceStats::get_Device_Ethernet_Interface_Stats_UnicastPacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{

    read_Device_Ethernet_Interface_Stats_Fields(dev_id,eUnicastPacketsSent);
    if(bCalledErrorsSent && pChanged && (backupErrorsSent != stEthInterfaceStats.unicastPacketsSent))
    {
        *pChanged = true;
    }
    bCalledErrorsSent = true;
    backupErrorsSent = stEthInterfaceStats.unicastPacketsSent;
    put_int(stMsgData->paramValue,stEthInterfaceStats.unicastPacketsSent);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function get the number of unicast packets received on an Ethernet Interface.
 * This function provides the total number of received packets, delivered by this layer to
 * a higher layer, which were not addressed to a multicast or broadcast address at this layer.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully receive the unicast packets via
 * ethernet interface else returns '-1'.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_STATS_API
 */
int hostIf_EthernetInterfaceStats::get_Device_Ethernet_Interface_Stats_UnicastPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{

    read_Device_Ethernet_Interface_Stats_Fields(dev_id,eUnicastPacketsReceived);
    if(bCalledUnicastPacketsReceived && pChanged && (backupUnicastPacketsReceived != stEthInterfaceStats.unicastPacketsReceived))
    {
        *pChanged = true;
    }
    bCalledUnicastPacketsReceived = true;
    backupUnicastPacketsReceived = stEthInterfaceStats.unicastPacketsReceived;
    put_int(stMsgData->paramValue,stEthInterfaceStats.unicastPacketsReceived);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function get the number of discarded outbound packets on an Ethernet Interface.
 * This function provides the total number of outbound packets which were chosen to be
 * discarded even though no errors had been detected to prevent their being transmitted.
 *
 * @note One possible reason for discarding such a packet could be to free up buffer
 *        space.
 *
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully discard the unicast packets sent via
 * ethernet interface else returns '-1'.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_STATS_API
 */
int hostIf_EthernetInterfaceStats::get_Device_Ethernet_Interface_Stats_DiscardPacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    bCalledDiscardPacketsSent;
    read_Device_Ethernet_Interface_Stats_Fields(dev_id,eDiscardPacketsSent);
    if(bCalledUnicastPacketsSent && pChanged && (backupUnicastPacketsSent != stEthInterfaceStats.discardPacketsSent))
    {
        *pChanged = true;
    }
    bCalledUnicastPacketsSent = true;
    backupUnicastPacketsSent = stEthInterfaceStats.discardPacketsSent;
    put_int(stMsgData->paramValue,stEthInterfaceStats.discardPacketsSent);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}


/**
 * @brief This function get the number of discarded inbound packets on an Ethernet Interface.
 * This function provides the total number of inbound packets which were chosen to be
 * discarded even though no errors had been detected to prevent their being delivered.
 *
 * @note One possible reason for discarding such a packet could be to free up buffer
 *       space.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully discard the unicast packets received via
 * ethernet interface else returns '-1'.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_STATS_API
 */
int hostIf_EthernetInterfaceStats::get_Device_Ethernet_Interface_Stats_DiscardPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{

    read_Device_Ethernet_Interface_Stats_Fields(dev_id,eDiscardPacketsReceived);
    if(bCalledDiscardPacketsReceived && pChanged && (backupDiscardPacketsReceived != stEthInterfaceStats.discardPacketsReceived))
    {
        *pChanged = true;
    }
    bCalledDiscardPacketsReceived = true;
    backupDiscardPacketsReceived = stEthInterfaceStats.discardPacketsReceived;
    put_int(stMsgData->paramValue,stEthInterfaceStats.discardPacketsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}


/**
 * @brief This function get the number of Multicast Packets sent on an Ethernet Interface.
 * This function provides the total number of packets that higher-level protocols
 * requested for transmission and which were addressed to a multicast address at
 * this layer, including those that were discarded or not sent.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully sent the multicast packets via
 * ethernet interface else returns '-1'.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_STATS_API
 */
int hostIf_EthernetInterfaceStats::get_Device_Ethernet_Interface_Stats_MulticastPacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{

    read_Device_Ethernet_Interface_Stats_Fields(dev_id,eMulticastPacketsSent);
    if(bCalledUnicastPacketsReceived && pChanged && (backupUnicastPacketsReceived != stEthInterfaceStats.multicastPacketsSent))
    {
        *pChanged = true;
    }
    bCalledUnicastPacketsReceived = true;
    backupUnicastPacketsReceived = stEthInterfaceStats.multicastPacketsSent;
    put_int(stMsgData->paramValue,stEthInterfaceStats.multicastPacketsSent);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = 4;

    return OK;
}


/**
 * @brief This function get the number of Multicast Packets received on an Ethernet Interface.
 * This function provides the total number of received packets, delivered by this layer
 * to a higher layer, which were addressed to a multicast address at this layer.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully received the multicast packets via
 * ethernet interface else returns '-1'.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_STATS_API
 */
int hostIf_EthernetInterfaceStats::get_Device_Ethernet_Interface_Stats_MulticastPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{

    read_Device_Ethernet_Interface_Stats_Fields(dev_id,eMulticastPacketsReceived);
    if(bCalledMulticastPacketsReceived && pChanged && (backupMulticastPacketsReceived != stEthInterfaceStats.multicastPacketsReceived))
    {
        *pChanged = true;
    }
    bCalledMulticastPacketsReceived = true;
    backupMulticastPacketsReceived = stEthInterfaceStats.multicastPacketsReceived;
    put_int(stMsgData->paramValue,stEthInterfaceStats.multicastPacketsReceived);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function get the number of Broadcast Packets sent on an Ethernet Interface.
 * This function provides the total number of packets that higher-level protocols
 * requested for transmission and which were addressed to a broadcast address at this
 * layer, including those that were discarded or not sent.
 *
 * @note IPv6 does not define broadcast addresses, so IPv6 packets will never cause
 *           this counter to increment.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully sent the broadcast packets via etherne
 * interface else returns '-1'.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_STATS_API
 *
 */
int hostIf_EthernetInterfaceStats::get_Device_Ethernet_Interface_Stats_BroadcastPacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{

    read_Device_Ethernet_Interface_Stats_Fields(dev_id,eBroadcastPacketsSent);
    if(bCalledBroadcastPacketsSent && pChanged && (backupBroadcastPacketsSent != stEthInterfaceStats.broadcastPacketsSent))
    {
        *pChanged = true;
    }
    bCalledBroadcastPacketsSent = true;
    backupBroadcastPacketsSent = stEthInterfaceStats.broadcastPacketsSent;
    put_int(stMsgData->paramValue,stEthInterfaceStats.broadcastPacketsSent);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function get the number of Broadcast Packets received on an Ethernet Interface.
 * This function provides the total number of received packets, delivered by this layer
 * to a higher layer, which were addressed to a broadcast address at this layer.
 *
 * @note  IPv6 does not define broadcast addresses, so IPv6 packets will never cause
 *           this counter to increment.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully received the broadcast packets via ethernet
 * interface else returns '-1'.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_STATS_API
 */
int hostIf_EthernetInterfaceStats::get_Device_Ethernet_Interface_Stats_BroadcastPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{

    read_Device_Ethernet_Interface_Stats_Fields(dev_id,eBroadcastPacketsReceived);
    if(bCalledBroadcastPacketsReceived && pChanged && (backupBroadcastPacketsReceived != stEthInterfaceStats.broadcastPacketsReceived))
    {
        *pChanged = true;
    }
    bCalledBroadcastPacketsReceived = true;
    backupBroadcastPacketsReceived = stEthInterfaceStats.broadcastPacketsReceived;
    put_int(stMsgData->paramValue,stEthInterfaceStats.broadcastPacketsReceived);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function get the number of Packets of unidentified protocol received on an Ethernet Interface.
 * This function provides the total number of packets received via this Ethernet interface which
 * were discarded because they were of an unknown or unsupported protocol.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully received the unknown proto packets via ethernet
 * interface else returns '-1'.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_STATS_API
 */
int hostIf_EthernetInterfaceStats::get_Device_Ethernet_Interface_Stats_UnknownProtoPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    bCalledUnknownProtoPacketsReceived;
    read_Device_Ethernet_Interface_Stats_Fields(dev_id,eUnknownProtoPacketsReceived);
    if(bCalledMulticastPacketsSent && pChanged && (backupMulticastPacketsSent != stEthInterfaceStats.unknownProtoPacketsReceived))
    {
        *pChanged = true;
    }
    bCalledMulticastPacketsSent = true;
    backupMulticastPacketsSent = stEthInterfaceStats.unknownProtoPacketsReceived;
    put_int(stMsgData->paramValue,stEthInterfaceStats.unknownProtoPacketsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}


/** @} */
/** @} */
