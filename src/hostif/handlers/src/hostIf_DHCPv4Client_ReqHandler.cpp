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
 /*
 * @brief Provides implementation for DHCPv4 Client Request Handler Interface
 *
 * @author vejuturu@cisco.com
 */

/**
 * @file hostIf_DHCPv4Client_ReqHandler.cpp
 * @brief The header file provides HostIf DHCPv4 client request handler information APIs.
 */
#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "Device_DHCPv4_Client.h"
#include "hostIf_DHCPv4Client_ReqHandler.h"

DHCPv4ClientReqHandler* DHCPv4ClientReqHandler::pInstance = NULL;

updateCallback DHCPv4ClientReqHandler::mUpdateCallback = NULL;
int DHCPv4ClientReqHandler::curNumOfDHCPv4Clients = 0;

msgHandler* DHCPv4ClientReqHandler::getInstance()
{
    hostIf_DHCPv4Client::getLock();
    if(!pInstance)
        pInstance = new DHCPv4ClientReqHandler();
    hostIf_DHCPv4Client::releaseLock();

    return pInstance;
}

void DHCPv4ClientReqHandler::reset()
{
    hostIf_DHCPv4Client::getLock();
    curNumOfDHCPv4Clients = 0;
    hostIf_DHCPv4Client::releaseLock();
}

/**
 * @brief This function is used to initialize. Currently not implemented.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if initialization is successful.
 * @retval false if initialization is not successful.
 * @ingroup TR-069HOSTIF_DHCPV4CLIENT_REQHANDLER_CLASSES
 */
bool DHCPv4ClientReqHandler::init()
{
    return true;
}

/**
 * @brief This function is used to close all the instances of DHCPv4 client.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if it successfully close all the instances.
 * @retval false if not able to close all the instances.
 * @ingroup TR-069HOSTIF_DHCPV4CLIENT_REQHANDLER_CLASSES
 */
bool DHCPv4ClientReqHandler::unInit()
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] DHCPv4 manager DeInitializing\n", __FUNCTION__);
    hostIf_DHCPv4Client::closeAllInstances();

    return true;
}

/**
 * @brief This function is used to handle the set message request of DHCPv4 Client.
 * Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successful.
 * @retval ERR_INTERNAL_ERROR if not able to set the data to the device.
 * @ingroup TR-069HOSTIF_DHCPV4CLIENT_REQHANDLER_CLASSES
 */
int DHCPv4ClientReqHandler::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;

    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%s] Found string as %s. Set command not supported.\n",
             __FUNCTION__, __FILE__, stMsgData->paramName);

    return ret;
}

/**
 * @brief This function is used to handle the get message request of DHCPv4 Client information
 * such as "Interface", "IPRouters" and "DNSServers".
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it gets the data successfully.
 * @retval ERR_INTERNAL_ERROR if not able to get the data from the device.
 * @ingroup TR-069HOSTIF_DHCPV4CLIENT_REQHANDLER_CLASSES
 */
int DHCPv4ClientReqHandler::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *pSetting;
    int instanceNumber = 0;
    hostIf_DHCPv4Client::getLock();

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    if(strcasecmp(stMsgData->paramName, "Device.DHCPv4.ClientNumberOfEntries") == 0)
    {
        ret = hostIf_DHCPv4Client::get_Device_DHCPv4_ClientNumberOfEntries(stMsgData);
    }
    else if(matchComponent(stMsgData->paramName,"Device.DHCPv4.Client",&pSetting,instanceNumber))
    {
        hostIf_DHCPv4Client *pDHCPv4Client = hostIf_DHCPv4Client::getInstance(instanceNumber);
        stMsgData->instanceNum = instanceNumber;
        if(pDHCPv4Client)
        {
            if(strcasecmp(pSetting,"Interface") == 0)
            {
                ret = pDHCPv4Client->get_Device_DHCPv4_Client_InterfaceReference(stMsgData);
            }
            else if(strcasecmp(pSetting,"IPRouters") == 0)
            {
                ret = pDHCPv4Client->get_Device_DHCPv4_Client_IPRouters(stMsgData);
            }
            else if(strcasecmp(pSetting,"DNSServers") == 0)
            {
                ret = pDHCPv4Client->get_Device_DHCPv4_Client_DnsServer(stMsgData);
            }
        }
        else
        {
            ret = NOK;
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%s:%d] hostIf_DHCPv4Client::getInstance returned NULL for instance %d,\n",
                    __FUNCTION__, __FILE__, __LINE__, instanceNumber);
        }
    }

    hostIf_DHCPv4Client::releaseLock();
    return ret;
}
int DHCPv4ClientReqHandler::handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    hostIf_DHCPv4Client::getLock();
    // TODO: Retrieve notification value from DeviceInfo structure for given parameter
    hostIf_DHCPv4Client::releaseLock();
    return ret;
}

int DHCPv4ClientReqHandler::handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    hostIf_DHCPv4Client::getLock();
    // TODO: Set notification value from DeviceInfo structure for given parameter
    hostIf_DHCPv4Client::releaseLock();
    return ret;
}

void DHCPv4ClientReqHandler::registerUpdateCallback(updateCallback cb)
{
    mUpdateCallback = cb;
}

void DHCPv4ClientReqHandler::checkForUpdates()
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    GList *elem;
    int index = 1;
    char tmp_buff[TR69HOSTIFMGR_MAX_PARAM_LEN];
    hostIf_DHCPv4Client::getLock();

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FILE__, __FUNCTION__);


    if(hostIf_DHCPv4Client::get_Device_DHCPv4_ClientNumberOfEntries(&msgData) == OK)
    {
        int tmpNoDev = get_int(msgData.paramValue);
        char tmp[TR69HOSTIFMGR_MAX_PARAM_LEN] = "";
        sprintf(tmp_buff,"Device.DHCPv4.Client");
        while(curNumOfDHCPv4Clients > tmpNoDev)
        {
            sprintf(tmp,"%s.%d.",tmp_buff,tmpNoDev);
            if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE,tmp, NULL, hostIf_IntegerType);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] ClientNumberOfEntries Removed Event..\n", __FILE__, __FUNCTION__);

            tmpNoDev++;
        }
        while(curNumOfDHCPv4Clients < tmpNoDev)
        {
            sprintf(tmp,"%s.",tmp_buff);
            if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_ADD,tmp, NULL, hostIf_IntegerType);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] ClientNumberOfEntries Added Event..\n", __FILE__, __FUNCTION__);

            tmpNoDev--;
        }
        curNumOfDHCPv4Clients = get_int(msgData.paramValue);
    }
#ifdef HAVE_VALUE_CHANGE_EVENT
    GList *devList = hostIf_DHCPv4Client::getAllInstances();

    for(elem = devList; elem; elem = elem->next,index++)
    {
        hostIf_DHCPv4Client *pDHCPv4Client = hostIf_DHCPv4Client::getInstance((int)elem->data);

        if(pDHCPv4Client)
        {
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pDHCPv4Client->get_Device_DHCPv4_Client_InterfaceReference(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DHCPv4.Client.%d.%s",index,"Interface");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] Interface change Event..\n", __FILE__, __FUNCTION__);
                }
            }

            memset(&msgData,0,sizeof(msgData)); 
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pDHCPv4Client->get_Device_DHCPv4_Client_IPRouters(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DHCPv4.Client.%d.%s",index,"IPRouters");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] IPRouters change Event..\n", __FILE__, __FUNCTION__);
                }
            }

            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pDHCPv4Client->get_Device_DHCPv4_Client_DnsServer(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DHCPv4.Client.%d.%s",index,"DNSServers");
                if(mUpdateCallback)
                {
                    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] DNSServers change Event..\n", __FILE__, __FUNCTION__);
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
        }
    }

    g_list_free(devList);
#endif

    hostIf_DHCPv4Client::releaseLock();

}
