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
 * @file hostIf_EthernetClient_ReqHandler.cpp
 * @brief The header file provides HostIf Ethernet client request handler information APIs.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/
//#define HAVE_VALUE_CHANGE_EVENT

#include "hostIf_main.h"
#include "hostIf_EthernetClient_ReqHandler.h"
#include "Device_Ethernet_Interface.h"
#include "Device_Ethernet_Interface_Stats.h"

EthernetClientReqHandler* EthernetClientReqHandler::pInstance = NULL;
updateCallback EthernetClientReqHandler::mUpdateCallback = NULL;
int EthernetClientReqHandler::curNumOfDevices[10] = {0,};

msgHandler* EthernetClientReqHandler::getInstance()
{

    if(!pInstance)
        pInstance = new EthernetClientReqHandler();
    return pInstance;
}
void EthernetClientReqHandler::reset()
{
    hostIf_EthernetInterface::getLock();
    memset(curNumOfDevices,0,sizeof(curNumOfDevices));
    hostIf_EthernetInterface::releaseLock();
}

/**
 * @brief This function is used to initialize. Currently not implemented.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if initialization is successful.
 * @retval false if initialization is not successful.
 * @ingroup TR-069HOSTIF_ETHERNETCLIENT_REQHANDLER_CLASSES
 */
bool EthernetClientReqHandler::init()
{
//    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
//    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

/**
 * @brief This function is used to close all the instances of ethernet interface
 * and ethernet interface stats.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if it successfully close all the instances.
 * @retval false if not able to close all the instances.
 * @ingroup TR-069HOSTIF_ETHERNETCLIENT_REQHANDLER_CLASSES
 */
bool EthernetClientReqHandler::unInit()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    hostIf_EthernetInterface::closeAllInstances();
    hostIf_EthernetInterfaceStats::closeAllInstances();

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

/**
 * @brief This function is used to handle the set message request of ethernet interface
 * attribute such as "Enable", "Alias", "LowerLayers", "MaxBitRate" and "DuplexMode".
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if successful.
 * @retval ERR_INTERNAL_ERROR if not able to set the data to the device.
 * @ingroup TR-069HOSTIF_ETHERNETCLIENT_REQHANDLER_CLASSES
 */
int EthernetClientReqHandler::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *pSetting;
    int instanceNumber;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    if(matchComponent(stMsgData->paramName,
                      "Device.Ethernet.Interface",&pSetting,instanceNumber))
    {
        stMsgData->instanceNum = instanceNumber;
        hostIf_EthernetInterface::getLock();

        hostIf_EthernetInterface *pIface = hostIf_EthernetInterface::getInstance(instanceNumber);

        if(!pIface)
        {
            hostIf_EthernetInterface::releaseLock();
            return NOK;
        }
        if (strcasecmp(pSetting,"Enable") == 0)
        {
            ret = pIface->set_Device_Ethernet_Interface_Enable(stMsgData);
        }
        if (strcasecmp(pSetting,"Alias") == 0)
        {
            ret = pIface->set_Device_Ethernet_Interface_Alias(stMsgData);
        }
        if (strcasecmp(pSetting,"LowerLayers") == 0)
        {
            ret = pIface->set_Device_Ethernet_Interface_LowerLayers(stMsgData);
        }
        if (strcasecmp(pSetting,"MaxBitRate") == 0)
        {
            ret = pIface->set_Device_Ethernet_Interface_Enable(stMsgData);
        }
        if (strcasecmp(pSetting,"DuplexMode") == 0)
        {
            ret = pIface->set_Device_Ethernet_Interface_DuplexMode(stMsgData);
        }

    }
    hostIf_EthernetInterface::releaseLock();
    return ret;
}

/**
 * @brief This function is used to handle the get message request of ethernet interface
 * attributes such as "Enable", "Status", "Name", "LastChange", "Upstream", "MACAddress"
 * etc..
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it gets the data successfully.
 * @retval ERR_INTERNAL_ERROR if not able to get the data from the device.
 * @ingroup TR-069HOSTIF_ETHERNETCLIENT_REQHANDLER_CLASSES
 */
int EthernetClientReqHandler::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *pSetting;
    int instanceNumber;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    hostIf_EthernetInterface::getLock();
    if(strcasecmp(stMsgData->paramName,"Device.Ethernet.InterfaceNumberOfEntries") == 0)
    {
        ret = hostIf_EthernetInterface::get_Device_Ethernet_InterfaceNumberOfEntries(stMsgData);
    }
    else if(matchComponent(stMsgData->paramName,
                           "Device.Ethernet.Interface",&pSetting,instanceNumber))
    {
        stMsgData->instanceNum = instanceNumber;
        if(!instanceNumber)	{
            hostIf_EthernetInterface::releaseLock();
            return NOK;
        }
        hostIf_EthernetInterface *pIface = hostIf_EthernetInterface::getInstance(instanceNumber);
        hostIf_EthernetInterfaceStats *pIfaceStats = hostIf_EthernetInterfaceStats::getInstance(instanceNumber);
        if(!pIface)
        {
            hostIf_EthernetInterface::releaseLock();
            return NOK;
        }

        if (strcasecmp(pSetting,"Enable") == 0)
        {
            ret = pIface->get_Device_Ethernet_Interface_Enable(stMsgData);
        }
        else if (strcasecmp(pSetting,"Status") == 0)
        {
            ret = pIface->get_Device_Ethernet_Interface_Status(stMsgData);
        }
        else if (strcasecmp(pSetting,"Name") == 0)
        {
            ret = pIface->get_Device_Ethernet_Interface_Name(stMsgData);
        }
        else if (strcasecmp(pSetting,"LastChange") == 0)
        {
            ret = pIface->get_Device_Ethernet_Interface_LastChange(stMsgData);
        }
        else if (strcasecmp(pSetting,"Upstream") == 0)
        {
            ret = pIface->get_Device_Ethernet_Interface_Upstream(stMsgData);
        }
        else if (strcasecmp(pSetting,"MACAddress") == 0)
        {
            ret = pIface->get_Device_Ethernet_Interface_MACAddress(stMsgData);
        }
        else if (strcasecmp(pSetting,"LowerLayers") == 0)
        {
            ret = pIface->get_Device_Ethernet_Interface_LowerLayers(stMsgData);
        }
        else if (strcasecmp(pSetting,"MaxBitRate") == 0)
        {
            ret = pIface->get_Device_Ethernet_Interface_MaxBitRate(stMsgData);
        }
        else if (strcasecmp(pSetting,"DuplexMode") == 0)
        {
            ret = pIface->get_Device_Ethernet_Interface_DuplexMode(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.BytesSent") == 0)
        {
            ret = pIfaceStats->get_Device_Ethernet_Interface_Stats_BytesSent(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.BytesReceived") == 0)
        {
            ret = pIfaceStats->get_Device_Ethernet_Interface_Stats_BytesReceived(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.PacketsSent") == 0)
        {
            ret = pIfaceStats->get_Device_Ethernet_Interface_Stats_PacketsSent(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.PacketsReceived") == 0)
        {
            ret = pIfaceStats->get_Device_Ethernet_Interface_Stats_PacketsReceived(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.ErrorsSent") == 0)
        {
            ret = pIfaceStats->get_Device_Ethernet_Interface_Stats_ErrorsSent(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.ErrorsReceived") == 0)
        {
            ret = pIfaceStats->get_Device_Ethernet_Interface_Stats_ErrorsReceived(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.UnicastPacketsSent") == 0)
        {
            ret = pIfaceStats->get_Device_Ethernet_Interface_Stats_UnicastPacketsSent(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.UnicastPacketsReceived") == 0)
        {
            ret = pIfaceStats->get_Device_Ethernet_Interface_Stats_UnicastPacketsReceived(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.DiscardPacketsSent") == 0)
        {
            ret = pIfaceStats->get_Device_Ethernet_Interface_Stats_DiscardPacketsSent(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.DiscardPacketsReceived") == 0)
        {
            ret = pIfaceStats->get_Device_Ethernet_Interface_Stats_DiscardPacketsReceived(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.MulticastPacketsSent") == 0)
        {
            ret = pIfaceStats->get_Device_Ethernet_Interface_Stats_MulticastPacketsSent(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.MulticastPacketsReceived") == 0)
        {
            ret = pIfaceStats->get_Device_Ethernet_Interface_Stats_MulticastPacketsReceived(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.BroadcastPacketsSent") == 0)
        {
            ret = pIfaceStats->get_Device_Ethernet_Interface_Stats_BroadcastPacketsSent(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.BroadcastPacketsReceived") == 0)
        {
            ret = pIfaceStats->get_Device_Ethernet_Interface_Stats_BroadcastPacketsReceived(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.UnknownProtoPacketsReceived") == 0)
        {
            ret = pIfaceStats->get_Device_Ethernet_Interface_Stats_UnknownProtoPacketsReceived(stMsgData);
        }

    }
    hostIf_EthernetInterface::releaseLock();
    return ret;
}

int EthernetClientReqHandler::handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    int instanceNumber = 0;

    hostIf_EthernetInterface *pIface = hostIf_EthernetInterface::getInstance(instanceNumber);
    stMsgData->instanceNum = instanceNumber;
    if(!pIface)
    {
        hostIf_EthernetInterface::releaseLock();
        return NOK;
    }

    GHashTable* notifyhash = pIface->getNotifyHash();
    if(notifyhash != NULL)
    {
        int* notifyvalue = (int*) g_hash_table_lookup(notifyhash,stMsgData->paramName);
        put_int(stMsgData->paramValue, *notifyvalue);
        stMsgData->paramtype = hostIf_IntegerType;
        ret = OK;
    }
    else
    {
        ret = NOK;
    }
    hostIf_EthernetInterface::releaseLock();
    return ret;
}

int EthernetClientReqHandler::handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    int instanceNumber = 0;
    const char *pSetting;
    hostIf_EthernetInterface::getLock();
    hostIf_EthernetInterface *pIface = hostIf_EthernetInterface::getInstance(instanceNumber);
    stMsgData->instanceNum = instanceNumber;
    if(!pIface)
    {
        hostIf_EthernetInterface::releaseLock();
        return NOK;
    }
    GHashTable* notifyhash = pIface->getNotifyHash();
    if(notifyhash != NULL)
    {
        int *notifyValuePtr;
        notifyValuePtr = (int*) malloc(1 * sizeof(int));

        // Inserting Notification parameter to Notify Hash Table,
        // Note that neither keys nor values are copied when inserted into the GHashTable, so they must exist for the lifetime of the GHashTable
        // There for allocating a memory for both Param name and param value. This should be freed whenever we disable Notification.
        char *notifyKey;
        notifyKey = (char*) malloc(sizeof(char)*strlen(stMsgData->paramName)+1);
        if(NULL != notifyValuePtr)
        {
            *notifyValuePtr = 1;
            strcpy(notifyKey,stMsgData->paramName);
            g_hash_table_insert(notifyhash,notifyKey,notifyValuePtr);
            int* notifyvalue = (int*) g_hash_table_lookup(notifyhash,stMsgData->paramName);
            ret = OK;
        }
        else
        {
            ret = NOK;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] EthernetClientReqHandler Not able to allocate Notify pointer %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
        }
    }
    else
    {
        ret = NOK;
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] EthernetClientReqHandler Not able to get notifyhash  %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    }
    hostIf_EthernetInterface::releaseLock();
    return ret;
}

void EthernetClientReqHandler::registerUpdateCallback(updateCallback cb)
{
    mUpdateCallback = cb;
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] Callback Registered .. Exiting..\n", __FILE__, __FUNCTION__);
}
void EthernetClientReqHandler::checkForUpdates()
{
    const char *pSetting;
    int instanceNumber = 0;
    GHashTable* notifyhash;

    HOSTIF_MsgData_t msgData;
    int index = 1;
    char tmp_buff[TR69HOSTIFMGR_MAX_PARAM_LEN];
    hostIf_EthernetInterface::getLock();

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    if(hostIf_EthernetInterface::get_Device_Ethernet_InterfaceNumberOfEntries(&msgData) == OK)
    {
        int tmpNoDev = get_int(msgData.paramValue);
        char tmp[TR69HOSTIFMGR_MAX_PARAM_LEN] = "";
        sprintf(tmp_buff,"Device.Ethernet.Interface");
        while(curNumOfDevices[index] > tmpNoDev)
        {
            sprintf(tmp,"%s.%d.",tmp_buff,tmpNoDev);
            if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE,tmp, NULL, hostIf_IntegerType);
            tmpNoDev++;
        }
        while(curNumOfDevices[index] < tmpNoDev)
        {
            sprintf(tmp,"%s.",tmp_buff);
            if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_ADD,tmp, NULL, hostIf_IntegerType);
            tmpNoDev--;
        }
        curNumOfDevices[index] = get_int(msgData.paramValue);
    }

#ifdef HAVE_VALUE_CHANGE_EVENT

    //Get Notify Hash from device Info
    hostIf_EthernetInterface *dIface = hostIf_EthernetInterface::getInstance(instanceNumber);
    if(NULL != dIface)
    {
        notifyhash = dIface->getNotifyHash();
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Unable to get Device Info Instance\n", __FUNCTION__, __FILE__);
    }

    // Iterate through Ghash Table
    if(NULL != notifyhash)
    {
        GHashTableIter notifyHashIterator;
        gpointer paramName;
        gpointer notifyEnable;
        bool  bChanged;

        g_hash_table_iter_init (&notifyHashIterator, notifyhash);
        while (g_hash_table_iter_next (&notifyHashIterator, &paramName, &notifyEnable))
        {
            int* isNotifyEnabled = (int *)notifyEnable;
            instanceNumber = 0;
            if(matchComponent((const char*)paramName,"Device.Ethernet.Interface",&pSetting,instanceNumber))
            {
                if(!instanceNumber)
                {   // Ethernet settings not found in Notify Hash Table
                    hostIf_EthernetInterface::releaseLock();
                    continue;
                }
                hostIf_EthernetInterface *pIface = hostIf_EthernetInterface::getInstance(instanceNumber);
                if(pIface)
                {
                    if (strcasecmp(pSetting,"Status") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_Ethernet_Interface_Status(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if(strcasecmp(pSetting,"Enable") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_Ethernet_Interface_Enable(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if(strcasecmp(pSetting,"Name") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_Ethernet_Interface_Name(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if(strcasecmp(pSetting,"LastChange") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_Ethernet_Interface_LastChange(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Upstream") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_Ethernet_Interface_Upstream(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"MACAddress") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_Ethernet_Interface_MACAddress(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"LowerLayers") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_Ethernet_Interface_LowerLayers(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"MaxBitRate") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_Ethernet_Interface_MaxBitRate(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"DuplexMode") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_Ethernet_Interface_DuplexMode(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                }
                hostIf_EthernetInterfaceStats *pIfaceStats = hostIf_EthernetInterfaceStats::getInstance(instanceNumber);
                if(pIfaceStats)
                {
                    if (strcasecmp(pSetting,"Stats.BytesSent") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceStats->get_Device_Ethernet_Interface_Stats_BytesSent(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Stats.BytesReceived") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceStats->get_Device_Ethernet_Interface_Stats_BytesReceived(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Stats.PacketsSent") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceStats->get_Device_Ethernet_Interface_Stats_PacketsSent(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Stats.PacketsReceived") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceStats->get_Device_Ethernet_Interface_Stats_PacketsReceived(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Stats.ErrorsSent") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceStats->get_Device_Ethernet_Interface_Stats_ErrorsSent(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Stats.ErrorsReceived") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceStats->get_Device_Ethernet_Interface_Stats_ErrorsReceived(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Stats.UnicastPacketsSent") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceStats->get_Device_Ethernet_Interface_Stats_UnicastPacketsSent(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Stats.UnicastPacketsReceived") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceStats->get_Device_Ethernet_Interface_Stats_UnicastPacketsReceived(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Stats.DiscardPacketsSent") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceStats->get_Device_Ethernet_Interface_Stats_DiscardPacketsSent(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Stats.DiscardPacketsReceived") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceStats->get_Device_Ethernet_Interface_Stats_DiscardPacketsReceived(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Stats.MulticastPacketsSent") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceStats->get_Device_Ethernet_Interface_Stats_MulticastPacketsSent(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Stats.MulticastPacketsReceived") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceStats->get_Device_Ethernet_Interface_Stats_MulticastPacketsReceived(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Stats.BroadcastPacketsSent") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceStats->get_Device_Ethernet_Interface_Stats_BroadcastPacketsSent(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Stats.BroadcastPacketsReceived") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceStats->get_Device_Ethernet_Interface_Stats_BroadcastPacketsReceived(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Stats.UnknownProtoPacketsReceived") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceStats->get_Device_Ethernet_Interface_Stats_UnknownProtoPacketsReceived(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] Unable to get Notify Hash table\n", __FUNCTION__, __FILE__);
    }
#endif /*HAVE_VALUE_CHANGE_EVENT */
    hostIf_EthernetInterface::releaseLock();

}



/** @} */
/** @} */
