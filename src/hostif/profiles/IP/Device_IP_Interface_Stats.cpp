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
 * @file Device_IP_Interface_Statss.cpp
 * @brief This source file contains the APIs of device IPv4 interface stats.
 */

/**
 * @file Device_IP_Interface_Stats.c
 *
 * @brief IP Interface Stats API Implementation.
 *
 * This is the implementation of the IP Interface Stats API.
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

#include "Device_IP_Interface_Stats.h"

IPInterfaceStats hostIf_IPInterfaceStats::curntIpStat = {0,};

GMutex* hostIf_IPInterfaceStats::m_mutex = NULL;

GHashTable *hostIf_IPInterfaceStats::ifHash = NULL;


int hostIf_IPInterfaceStats::readInterfacestat(int interfaceNo, EDeviceIPInterfaceStatsMembers statsMembers)
{
    char nameOfInterface[IFNAMSIZ] = { 0 };
    if_indextoname (interfaceNo, nameOfInterface);
    getStatFields (nameOfInterface, statsMembers);
    return 0;
}

int hostIf_IPInterfaceStats::getStatFields(char *interfaceName, EDeviceIPInterfaceStatsMembers statsMembers)
{
    FILE *fp = NULL;
    int status;
    char cmd[BUFF_LENGTH] = {'\0'};
    char result[BUFF_LENGTH];

    switch(statsMembers)
    {
    case eIPStatsBytesSent:
        sprintf(cmd,"%s%s%s%s","cat ", _PATH_SYS_CLASS_NET, interfaceName, "/statistics/tx_bytes");
        fp = popen(cmd ,"r");
        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"\n[%s(), %d] Error in popen\n",__FUNCTION__,__LINE__);

            return NOK;
        }

        if(fgets(result,BUFF_LENGTH,fp)!=NULL)
        {
            sscanf(result,"%ld",&curntIpStat.bytesSent);
        }

        pclose(fp);
        break;
    case eIPStatsBytesReceived:
        memset(cmd,'\0', sizeof(cmd));
        sprintf(cmd,"%s%s%s%s","cat ", _PATH_SYS_CLASS_NET, interfaceName, "/statistics/rx_bytes");
        fp = popen(cmd,"r");
        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"\n[%s(), %d] Error in popen\n",__FUNCTION__,__LINE__);

            return NOK;
        }

        if(fgets(result,BUFF_LENGTH,fp)!=NULL)
        {
            sscanf(result,"%ld",&curntIpStat.bytesReceived);
        }
        pclose(fp);
        break;
    case eIPStatsPacketsSent:
    case eIPStatsUnicastPacketsSent:
    case eIPStatsMulticastPacketsSent:
    case eIPStatsBroadcastPacketsSent:
        memset(cmd,'\0', sizeof(cmd));
        sprintf(cmd,"%s%s%s%s","cat ", _PATH_SYS_CLASS_NET, interfaceName, "/statistics/tx_packets");
        fp = popen(cmd,"r");

        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"\n[%s(), %d] Error in popen\n",__FUNCTION__,__LINE__);

            return NOK;
        }

        if(fgets(result,BUFF_LENGTH,fp)!=NULL)
        {
            sscanf(result,"%ld",&curntIpStat.packetsSent);
            sscanf(result,"%ld",&curntIpStat.unicastPacketsSent);
            sscanf(result,"%ld",&curntIpStat.multicastPacketsSent);
            sscanf(result,"%ld",&curntIpStat.broadcastPacketsSent);
        }

        pclose(fp);
        break;
    case eIPStatsPacketsReceived:
    case eIPStatsUnicastPacketsReceived:
    case eIPStatsMulticastPacketsReceived:
    case eIPStatsBroadcastPacketsReceived:
        memset(cmd,'\0', sizeof(cmd));
        sprintf(cmd,"%s%s%s%s","cat ", _PATH_SYS_CLASS_NET, interfaceName, "/statistics/rx_packets");
        fp = popen(cmd,"r");
        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"\n[%s(), %d] Error in popen\n",__FUNCTION__,__LINE__);

            return NOK;
        }

        if(fgets(result,BUFF_LENGTH,fp)!=NULL)
        {
            sscanf(result,"%ld",&curntIpStat.packetsReceived);
            sscanf(result,"%ld",&curntIpStat.unicastPacketsReceived);
            sscanf(result,"%ld",&curntIpStat.multicastPacketsReceived);
            sscanf(result,"%ld",&curntIpStat.broadcastPacketsReceived);
        }

        pclose(fp);
        break;
    case eIPStatsErrorsSent:
        memset(cmd,'\0', sizeof(cmd));
        sprintf(cmd,"%s%s%s%s","cat ", _PATH_SYS_CLASS_NET, interfaceName, "/statistics/tx_errors");
        fp = popen(cmd,"r");
        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"\n[%s(), %d] Error in popen\n",__FUNCTION__,__LINE__);

            return NOK;
        }

        if(fgets(result,BUFF_LENGTH,fp)!=NULL)
        {
            sscanf(result,"%u",&curntIpStat.errorsSent);
        }

        pclose(fp);
        break;
    case eIPStatsErrorsReceived:
        memset(cmd,'\0', sizeof(cmd));
        sprintf(cmd,"%s%s%s%s","cat ", _PATH_SYS_CLASS_NET, interfaceName, "/statistics/rx_errors");
        fp = popen(cmd,"r");
        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"\n[%s(), %d] Error in popen\n",__FUNCTION__,__LINE__);

            return NOK;
        }

        if(fgets(result,BUFF_LENGTH,fp)!=NULL)
        {
            sscanf(result,"%u",&curntIpStat.errorsReceived);
        }

        pclose(fp);
        break;
    case eIPStatsDiscardPacketsSent:
        memset(cmd,'\0', sizeof(cmd));
        sprintf(cmd,"%s%s%s%s","cat ", _PATH_SYS_CLASS_NET, interfaceName, "/statistics/tx_dropped");
        fp = popen(cmd,"r");

        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"\n[%s(), %d] Error in popen\n",__FUNCTION__,__LINE__);

            return NOK;
        }

        if(fgets(result,BUFF_LENGTH,fp)!=NULL)
        {
            sscanf(result,"%u",&curntIpStat.discardPacketsSent);
        }

        pclose(fp);
        break;
    case eIPStatsDiscardPacketsReceived:
    case eIPStatsUnknownProtoPacketsReceived:
        memset(cmd,'\0', sizeof(cmd));
        sprintf(cmd,"%s%s%s%s","cat ", _PATH_SYS_CLASS_NET, interfaceName, "/statistics/rx_dropped");
        fp = popen(cmd,"r");

        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"\n[%s(), %d] Error in popen\n",__FUNCTION__,__LINE__);

            return NOK;
        }

        if(fgets(result,BUFF_LENGTH,fp)!=NULL)
        {
            sscanf(result,"%u",&curntIpStat.discardPacketsReceived);
            sscanf(result,"%u",&curntIpStat.unknownProtoPacketsReceived);
        }

        pclose(fp);
        break;
    }

    return OK;
}

hostIf_IPInterfaceStats* hostIf_IPInterfaceStats::getInstance(int dev_id)
{
    hostIf_IPInterfaceStats* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_IPInterfaceStats *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_IPInterfaceStats(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create MoCA Interface instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_IPInterfaceStats::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_IPInterfaceStats::closeInstance(hostIf_IPInterfaceStats *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_IPInterfaceStats::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_IPInterfaceStats* pDev = (hostIf_IPInterfaceStats *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

void hostIf_IPInterfaceStats::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void hostIf_IPInterfaceStats::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

/**
 * @brief Class Constructor of the class hostIf_IPInterfaceStats.
 *
 * It will initialize the device id.
 *
 * @param[in] dev_id Identification number of the device to communicate.
 */
hostIf_IPInterfaceStats::hostIf_IPInterfaceStats(int dev_id):
    dev_id(dev_id),
    backupBytesSent(0),
    backupBytesReceived(0),
    backupPacketsSent(0),
    backupPacketsReceived(0),
    backupErrorsSent(0),
    backupErrorsReceived(0),
    backupUnicastPacketsSent(0),
    backupUnicastPacketsReceived(0),
    backupDiscardPacketsSent(0),
    backupDiscardPacketsReceived(0),
    backupMulticastPacketsSent(0),
    backupMulticastPacketsReceived(0),
    backupBroadcastPacketsSent(0),
    backupBroadcastPacketsReceived(0),
    backupUnknownProtoPacketsReceived(0),
    bCalledBytesSent(false),
    bCalledBytesReceived(false),
    bCalledPacketsSent(false),
    bCalledPacketsReceived(false),
    bCalledErrorsSent(false),
    bCalledErrorsReceived(false),
    bCalledUnicastPacketsSent(false),
    bCalledUnicastPacketsReceived(false),
    bCalledDiscardPacketsSent(false),
    bCalledDiscardPacketsReceived(false),
    bCalledMulticastPacketsSent(false),
    bCalledMulticastPacketsReceived(false),
    bCalledBroadcastPacketsSent(false),
    bCalledBroadcastPacketsReceived(false),
    bCalledUnknownProtoPacketsReceived(false)
{
}

int hostIf_IPInterfaceStats::handleGetMsg (const char* pSetting, HOSTIF_MsgData_t* stMsgData)
{
    int ret = NOT_HANDLED;

    if (!strcasecmp (pSetting, "Stats.BytesSent"))
    {
        ret = get_Device_IP_Interface_Stats_BytesSent (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.BytesReceived"))
    {
        ret = get_Device_IP_Interface_Stats_BytesReceived (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.PacketsSent"))
    {
        ret = get_Device_IP_Interface_Stats_PacketsSent (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.PacketsReceived"))
    {
        ret = get_Device_IP_Interface_Stats_PacketsReceived (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.ErrorsSent"))
    {
        ret = get_Device_IP_Interface_Stats_ErrorsSent (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.ErrorsReceived"))
    {
        ret = get_Device_IP_Interface_Stats_ErrorsReceived (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.UnicastPacketsSent"))
    {
        ret = get_Device_IP_Interface_Stats_UnicastPacketsSent (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.UnicastPacketsReceived"))
    {
        ret = get_Device_IP_Interface_Stats_UnicastPacketsReceived (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.DiscardPacketsSent"))
    {
        ret = get_Device_IP_Interface_Stats_DiscardPacketsSent (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.DiscardPacketsReceived"))
    {
        ret = get_Device_IP_Interface_Stats_DiscardPacketsReceived (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.MulticastPacketsSent"))
    {
        ret = get_Device_IP_Interface_Stats_MulticastPacketsSent (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.MulticastPacketsReceived"))
    {
        ret = get_Device_IP_Interface_Stats_MulticastPacketsReceived (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.BroadcastPacketsSent"))
    {
        ret = get_Device_IP_Interface_Stats_BroadcastPacketsSent (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.BroadcastPacketsReceived"))
    {
        ret = get_Device_IP_Interface_Stats_BroadcastPacketsReceived (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Stats.UnknownProtoPacketsReceived"))
    {
        ret = get_Device_IP_Interface_Stats_UnknownProtoPacketsReceived (stMsgData);
    }

    return ret;
}

/****************************************************************************************************************************************************/
// Device_IP_Interface_Stats Profile. Getters:
/****************************************************************************************************************************************************/

/**
 * @brief This function gets the number of bytes sent on the IP Interface. It provides the
 * total number of bytes transmitted out of the interface, including framing characters.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_BytesSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    readInterfacestat(dev_id,eIPStatsBytesSent);
    if(bCalledBytesSent && pChanged && (backupBytesSent != curntIpStat.bytesSent))
    {
        *pChanged = true;
    }
    bCalledBytesSent = true;
    backupBytesSent = curntIpStat.bytesSent;
    put_int(stMsgData->paramValue,curntIpStat.bytesSent);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function gets the number of bytes received on the IP Interface. It provides
 * the total number of bytes received on the interface, including framing characters.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_BytesReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    readInterfacestat(dev_id,eIPStatsBytesReceived);
    if(bCalledBytesReceived && pChanged && (backupBytesReceived != curntIpStat.bytesReceived))
    {
        *pChanged = true;
    }
    bCalledBytesReceived = true;
    backupBytesReceived = curntIpStat.bytesReceived;
    put_int(stMsgData->paramValue,curntIpStat.bytesReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function gets the number of packets sent on the IP Interface. It provides
 * the total number of packets transmitted out of the interface.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_PacketsSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    readInterfacestat(dev_id,eIPStatsPacketsSent);
    if(bCalledPacketsSent && pChanged && (backupPacketsSent != curntIpStat.packetsSent))
    {
        *pChanged = true;
    }
    bCalledPacketsSent = true;
    backupPacketsSent = curntIpStat.packetsSent;
    put_int(stMsgData->paramValue,curntIpStat.packetsSent);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function gets the total number of packets received on the IP Interface.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_PacketsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    readInterfacestat(dev_id,eIPStatsPacketsReceived);
    if(bCalledPacketsReceived && pChanged && (backupPacketsReceived != curntIpStat.packetsReceived))
    {
        *pChanged = true;
    }
    bCalledPacketsReceived = true;
    backupPacketsReceived = curntIpStat.packetsReceived;
    put_int(stMsgData->paramValue,curntIpStat.packetsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief  This function gets the number of send errors on the IP Interface. It provides
 * the total number of outbound packets that could not be transmitted because of errors.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_ErrorsSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    readInterfacestat(dev_id,eIPStatsErrorsSent);
    if(bCalledErrorsSent && pChanged && (backupErrorsSent != curntIpStat.errorsSent))
    {
        *pChanged = true;
    }
    bCalledErrorsSent = true;
    backupErrorsSent = curntIpStat.errorsSent;
    put_int(stMsgData->paramValue,curntIpStat.errorsSent);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function gets the number of receive errors on the IP Interface. It provides
 * the total number of inbound packets that contained errors preventing them from being
 * delivered to a higher-layer protocol.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_ErrorsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    readInterfacestat(dev_id,eIPStatsErrorsReceived);
    if(bCalledErrorsReceived && pChanged && (backupErrorsReceived != curntIpStat.errorsReceived))
    {
        *pChanged = true;
    }
    bCalledErrorsReceived = true;
    backupErrorsReceived = curntIpStat.errorsReceived;
    put_int(stMsgData->paramValue,curntIpStat.errorsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function gets the number of unicast packets for which a request to send on the IP
 * Interface was received. It provides the total number of packets requested for transmission which
 * were not addressed to a multicast or broadcast address at this layer, including those that
 * were discarded or not sent.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_UnicastPacketsSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    readInterfacestat(dev_id,eIPStatsUnicastPacketsSent);
    if(bCalledUnicastPacketsSent && pChanged && (backupUnicastPacketsSent != curntIpStat.unicastPacketsSent))
    {
        *pChanged = true;
    }
    bCalledUnicastPacketsSent = true;
    backupUnicastPacketsSent = curntIpStat.unicastPacketsSent;
    put_int(stMsgData->paramValue,curntIpStat.unicastPacketsSent);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function gets the number of unicast packets received on the IP Interface. Provides
 * the total number of received packets, delivered by this layer to a higher layer, which were
 * not addressed to a multicast or broadcast address at this layer.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_UnicastPacketsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    readInterfacestat(dev_id,eIPStatsUnicastPacketsReceived);
    if(bCalledUnicastPacketsReceived && pChanged && (backupUnicastPacketsReceived != curntIpStat.unicastPacketsReceived))
    {
        *pChanged = true;
    }
    bCalledUnicastPacketsReceived = true;
    backupUnicastPacketsReceived = curntIpStat.unicastPacketsReceived;
    put_int(stMsgData->paramValue,curntIpStat.unicastPacketsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function gets the number of discarded outbound packets on the IP Interface. It
 * provides the total number of outbound packets which were chosen to be discarded even though
 * no errors had been detected to prevent their being transmitted.
 *
 * @note One possible reason for discarding such a packet could be to free up buffer space.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_DiscardPacketsSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    readInterfacestat(dev_id,eIPStatsDiscardPacketsSent);
    if(bCalledDiscardPacketsSent && pChanged && (backupDiscardPacketsSent != curntIpStat.discardPacketsSent))
    {
        *pChanged = true;
    }
    bCalledDiscardPacketsSent = true;
    backupDiscardPacketsSent = curntIpStat.discardPacketsSent;
    put_int(stMsgData->paramValue,curntIpStat.discardPacketsSent);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function gets the number of discarded inbound packets on the IP Interface. It provides
 * the total number of inbound packets which were chosen to be discarded even though no errors had
 * been detected to prevent their being delivered.
 *
 * @note One possible reason for discarding such a packet could be to free up buffer space.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_DiscardPacketsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    readInterfacestat(dev_id,eIPStatsDiscardPacketsReceived);
    if(bCalledDiscardPacketsReceived && pChanged && (backupDiscardPacketsReceived != curntIpStat.discardPacketsReceived))
    {
        *pChanged = true;
    }
    bCalledDiscardPacketsReceived = true;
    backupDiscardPacketsReceived = curntIpStat.discardPacketsReceived;
    put_int(stMsgData->paramValue,curntIpStat.discardPacketsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief  This function gets the number of Multicast Packets sent on the IP Interface. It provides
 * the total number of packets that higher-level protocols requested for transmission and which were
 * addressed to a multicast address at this layer, including those that were discarded or not sent.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_MulticastPacketsSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    readInterfacestat(dev_id,eIPStatsMulticastPacketsSent);
    if(bCalledMulticastPacketsSent && pChanged && (backupMulticastPacketsSent != curntIpStat.multicastPacketsSent))
    {
        *pChanged = true;
    }
    bCalledMulticastPacketsSent = true;
    backupMulticastPacketsSent = curntIpStat.multicastPacketsSent;
    put_int(stMsgData->paramValue,curntIpStat.multicastPacketsSent);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function gets the number of Multicast Packets received on the IP Interface. It provides the
 * total number of received packets, delivered by this layer to a higher layer, which were addressed
 * to a multicast address at this layer.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_MulticastPacketsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    readInterfacestat(dev_id,eIPStatsMulticastPacketsReceived);
    if(bCalledMulticastPacketsReceived && pChanged && (backupMulticastPacketsReceived != curntIpStat.multicastPacketsReceived))
    {
        *pChanged = true;
    }
    bCalledMulticastPacketsReceived = true;
    backupMulticastPacketsReceived = curntIpStat.multicastPacketsReceived;
    put_int(stMsgData->paramValue,curntIpStat.multicastPacketsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function gets the number of broadcast packets sent on the IP Interface. It provides
 * the total number of packets that higher-level protocols requested for transmission and which
 * were addressed to a broadcast address at this layer, including those that were discarded or not sent.
 *
 * @note IPv6 does not define broadcast addresses, so IPv6 packets will never cause this counter to increment.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_BroadcastPacketsSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    readInterfacestat(dev_id,eIPStatsBroadcastPacketsSent);
    if(bCalledBroadcastPacketsSent && pChanged && (backupBroadcastPacketsSent != curntIpStat.broadcastPacketsSent))
    {
        *pChanged = true;
    }
    bCalledBroadcastPacketsSent = true;
    backupBroadcastPacketsSent = curntIpStat.broadcastPacketsSent;
    put_int(stMsgData->paramValue,curntIpStat.broadcastPacketsSent);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function gets the number of broadcast packets received on the IP interface.
 * It provides the total number of received packets, delivered by this layer to a higher layer,
 * which were addressed to a broadcast address at this layer.
 *
 * @note IPv6 does not define broadcast addresses, so IPv6 packets will never cause this counter
 * to increment.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_BroadcastPacketsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    readInterfacestat(dev_id,eIPStatsBroadcastPacketsReceived);
    if(bCalledBroadcastPacketsReceived && pChanged && (backupBroadcastPacketsReceived != curntIpStat.broadcastPacketsReceived))
    {
        *pChanged = true;
    }
    bCalledBroadcastPacketsReceived = true;
    backupBroadcastPacketsReceived = curntIpStat.broadcastPacketsReceived;
    put_int(stMsgData->paramValue,curntIpStat.broadcastPacketsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function gets the number of Packets of unidentified protocol received on
 * the IP Interface. It provides the total number of packets received via the interface
 * which were discarded because they were of an unknown or unsupported protocol.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API
 */
int hostIf_IPInterfaceStats::get_Device_IP_Interface_Stats_UnknownProtoPacketsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    readInterfacestat(dev_id,eIPStatsUnknownProtoPacketsReceived);
    if(bCalledUnknownProtoPacketsReceived && pChanged && (backupUnknownProtoPacketsReceived != curntIpStat.unknownProtoPacketsReceived))
    {
        *pChanged = true;
    }
    bCalledUnknownProtoPacketsReceived = true;
    backupUnknownProtoPacketsReceived = curntIpStat.unknownProtoPacketsReceived;
    put_int(stMsgData->paramValue,curntIpStat.unknownProtoPacketsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}
/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */


/** @} */
/** @} */
