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
 * @file hostIf_IPClient_ReqHandler.cpp
 * @brief The header file provides HostIf IP client request handler information APIs.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#include "hostIf_main.h"
#include "hostIf_IPClient_ReqHandler.h"
#include "Device_IP_ActivePort.h"
#include "Device_IP_Interface.h"
#include "Device_IP_Interface_IPv4Address.h"
#ifdef IPV6_SUPPORT
#include "Device_IP_Interface_IPv6Address.h"
#endif // IPV6_SUPPORT
#include "Device_IP_Interface_Stats.h"
#include "Device_IP.h"


IPClientReqHandler* IPClientReqHandler::pInstance = NULL;
updateCallback IPClientReqHandler::mUpdateCallback = NULL;
int IPClientReqHandler::curNumOfIPInterface = 0;
int IPClientReqHandler::curNumOfInterfaceIPv4Addresses[10] = {0,};
#ifdef IPV6_SUPPORT
int IPClientReqHandler::curNumOfInterfaceIPv6Addresses[10] = {0,};
int IPClientReqHandler::curNumOfInterfaceIPv6Prefixes[10] = {0,};
#endif // IPV6_SUPPORT
int IPClientReqHandler::curNumOfIPv4Interface[10] = {0,};
int IPClientReqHandler::curNumOfActivePorts[10] = {0,};

IPClientReqHandler::IPClientReqHandler ()
{
}

IPClientReqHandler::~IPClientReqHandler ()
{
}

msgHandler* IPClientReqHandler::getInstance()
{
    if(!pInstance)
        pInstance = new IPClientReqHandler();
    return pInstance;
}

IPClientReqHandler::hostIf_IPInterfaceLocker::hostIf_IPInterfaceLocker ()
{
    hostIf_IPInterface::getLock ();
}

IPClientReqHandler::hostIf_IPInterfaceLocker::~hostIf_IPInterfaceLocker ()
{
    hostIf_IPInterface::releaseLock ();
}

void IPClientReqHandler::reset()
{
    hostIf_IPInterfaceLocker l;
    curNumOfIPInterface = 0;
    memset (curNumOfInterfaceIPv4Addresses, 0, sizeof(curNumOfInterfaceIPv4Addresses));
#ifdef IPV6_SUPPORT
    memset (curNumOfInterfaceIPv6Addresses, 0, sizeof(curNumOfInterfaceIPv6Addresses));
    memset (curNumOfInterfaceIPv6Prefixes, 0, sizeof(curNumOfInterfaceIPv6Prefixes));
#endif // IPV6_SUPPORT
    memset (curNumOfIPv4Interface, 0, sizeof(curNumOfIPv4Interface));
    memset (curNumOfActivePorts, 0, sizeof(curNumOfActivePorts));
}

/**
 * @brief This function is used to initialize. Currently not implemented.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if initialization is successful.
 * @retval false if initialization is not successful.
 * @ingroup TR-069HOSTIF_IPCLIENT_REQHANDLER_CLASSES
 */
bool IPClientReqHandler::init()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

/**
 * @brief This function is used to close all the instances of IP interface, IP interface stats
 * IPv4 Address, IPActivePort and IP.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if it successfully close all the instances.
 * @retval false if not able to close all the instances.
 * @ingroup TR-069HOSTIF_IPCLIENT_REQHANDLER_CLASSES
 */
bool IPClientReqHandler::unInit()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    hostIf_IPInterface::closeAllInstances();
    hostIf_IPInterfaceStats::closeAllInstances();
    hostIf_IPv4Address::closeAllInstances();
    hostIf_IPActivePort::closeAllInstances();
    hostIf_IP::closeAllInstances();

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

/**
 * @brief This function is used to handle the get message request of IP interface,
 * IPv4Address, IPActivePort and IP. Gets the attribute of IP interface such as "BytesSent",
 * "BytesReceived", "PacketsSent", "PacketsReceived" ect.. The attributes of IPv4Address such as
 * "Enable", "IPv4Enable", "ULAEnable" etc..
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if successful.
 * @retval ERR_INTERNAL_ERROR if not able to set the data to the device.
 * @ingroup TR-069HOSTIF_IPCLIENT_REQHANDLER_CLASSES
 */
int IPClientReqHandler::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *positionAfterInstanceNumber = 0;
    int instanceNumber = 0;

    hostIf_IPInterfaceLocker locker;

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);

    if (matchComponent (stMsgData->paramName, "Device.IP.Interface", &positionAfterInstanceNumber, instanceNumber))
    {
        stMsgData->instanceNum = instanceNumber;

        if (!instanceNumber)
            return NOK;

        const char *positionAfterSubInstanceNumber = 0;
        int subInstanceNumber;

        if (strncasecmp (positionAfterInstanceNumber, "Stats", strlen ("Stats")) == 0)
        {
            hostIf_IPInterfaceStats* pIfaceStats = hostIf_IPInterfaceStats::getInstance (instanceNumber);
            ret = pIfaceStats->handleGetMsg (positionAfterInstanceNumber, stMsgData);
        }
        else if (matchComponent (positionAfterInstanceNumber, "IPv4Address", &positionAfterSubInstanceNumber, subInstanceNumber))
        {
            hostIf_IPv4Address* pIfaceIPv4 = hostIf_IPv4Address::getInstance (instanceNumber);
            if (!pIfaceIPv4)
                return NOK;
            ret = pIfaceIPv4->handleGetMsg (positionAfterSubInstanceNumber, subInstanceNumber, stMsgData);
        }
#ifdef IPV6_SUPPORT
        else if (matchComponent (positionAfterInstanceNumber, "IPv6Address", &positionAfterSubInstanceNumber, subInstanceNumber))
        {
            hostIf_IPv6Address* pIfaceIPv6 = hostIf_IPv6Address::getInstance (instanceNumber);
            if (!pIfaceIPv6)
                return NOK;
            ret = pIfaceIPv6->handleGetMsg (positionAfterSubInstanceNumber, subInstanceNumber, stMsgData, true);
        }
        else if (matchComponent (positionAfterInstanceNumber, "IPv6Prefix", &positionAfterSubInstanceNumber, subInstanceNumber))
        {
            hostIf_IPv6Address* pIfaceIPv6 = hostIf_IPv6Address::getInstance (instanceNumber);
            if (!pIfaceIPv6)
                return NOK;
            ret = pIfaceIPv6->handleGetMsg (positionAfterSubInstanceNumber, subInstanceNumber, stMsgData, false);
        }
#endif // IPV6_SUPPORT
        else
        {
            hostIf_IPInterface* pIface = hostIf_IPInterface::getInstance (instanceNumber);
            if (!pIface)
                return NOK;
            ret = pIface->handleGetMsg (positionAfterInstanceNumber, stMsgData);
        }
    }
    else if (matchComponent (stMsgData->paramName, "Device.IP.ActivePort", &positionAfterInstanceNumber, instanceNumber))
    {
        stMsgData->instanceNum = instanceNumber;
        hostIf_IPActivePort *pIfaceActivePort = hostIf_IPActivePort::getInstance (instanceNumber);
        if (!pIfaceActivePort)
            return NOK;
        ret = pIfaceActivePort->handleGetMsg (positionAfterInstanceNumber, stMsgData);
    }
    else if (strncasecmp (stMsgData->paramName, "Device.IP", strlen ("Device.IP")) == 0)
    {
        stMsgData->instanceNum = instanceNumber;
        hostIf_IP *pIPIface = hostIf_IP::getInstance (instanceNumber);
        if (!pIPIface)
            return NOK;
        ret = pIPIface->handleGetMsg (stMsgData);
    }

    return ret;
}


/**
 * @brief This function is used to handle the set message request of IP interface
 * IPv4Address and IP. Gets the attribute of IP interface such as "Enable",
 * "IPv4Enable", "Reset", "MaxMTUSize" . The attributes of IPv4Address such as
 * "Enable", "SubnetMask" and "IPAddress".
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if successful.
 * @retval ERR_INTERNAL_ERROR if not able to set the data to the device.
 * @ingroup TR-069HOSTIF_IPCLIENT_REQHANDLER_CLASSES
 */
int IPClientReqHandler::handleSetMsg (HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *positionAfterInstanceNumber = 0;
    int instanceNumber;

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);

    hostIf_IPInterfaceLocker l;

    if (matchComponent (stMsgData->paramName, "Device.IP.Interface", &positionAfterInstanceNumber, instanceNumber))
    {
        stMsgData->instanceNum = instanceNumber;
        const char *positionAfterSubInstanceNumber = 0;
        int subInstanceNumber;
        if (matchComponent (positionAfterInstanceNumber, "IPv4Address", &positionAfterSubInstanceNumber, subInstanceNumber))
        {
            hostIf_IPv4Address* pIfaceIPv4 = hostIf_IPv4Address::getInstance (instanceNumber);
            if (!pIfaceIPv4)
                return NOK;
            ret = pIfaceIPv4->handleSetMsg (positionAfterSubInstanceNumber, subInstanceNumber, stMsgData);
        }
#ifdef IPV6_SUPPORT
        else if (matchComponent (positionAfterInstanceNumber, "IPv6Address", &positionAfterSubInstanceNumber, subInstanceNumber))
        {
            hostIf_IPv6Address* pIfaceIPv6 = hostIf_IPv6Address::getInstance (instanceNumber);
            if (!pIfaceIPv6)
                return NOK;
            ret = pIfaceIPv6->handleSetMsg (positionAfterSubInstanceNumber, subInstanceNumber, stMsgData, true);
        }
        else if (matchComponent (positionAfterInstanceNumber, "IPv6Prefix", &positionAfterSubInstanceNumber, subInstanceNumber))
        {
            hostIf_IPv6Address* pIfaceIPv6 = hostIf_IPv6Address::getInstance (instanceNumber);
            if (!pIfaceIPv6)
                return NOK;
            ret = pIfaceIPv6->handleSetMsg (positionAfterSubInstanceNumber, subInstanceNumber, stMsgData, false);
        }
#endif // IPV6_SUPPORT
        else
        {
            hostIf_IPInterface *pIface = hostIf_IPInterface::getInstance (instanceNumber);
            if (!pIface)
                return NOK;

            ret = pIface->handleSetMsg (positionAfterInstanceNumber, stMsgData);
        }
    }
    else if (strncasecmp (stMsgData->paramName, "Device.IP", strlen ("Device.IP")))
    {
        stMsgData->instanceNum = instanceNumber;
        hostIf_IP *pIPIface = hostIf_IP::getInstance (instanceNumber);
        if (!pIPIface)
            return NOK;
        ret = pIPIface->handleSetMsg (stMsgData);
    }

    return ret;
}

void IPClientReqHandler::registerUpdateCallback(updateCallback cb)
{
    mUpdateCallback = cb;
}

void IPClientReqHandler::sendAddRemoveEvents (int newValue, int& savedValue, char* objectPath, char* instancePath)
{
    while (savedValue > newValue)
    {
        sprintf (instancePath, "%s%d.", objectPath, savedValue);
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s:%s:%d] IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE %s\n",
                __FILE__, __FUNCTION__, __LINE__, instancePath);
        mUpdateCallback (IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE, instancePath, NULL, hostIf_IntegerType);
        savedValue--;
//        sleep(1);
    }
    while (savedValue < newValue)
    {
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s:%s:%d] IARM_BUS_TR69HOSTIFMGR_EVENT_ADD %s\n",
                __FILE__, __FUNCTION__, __LINE__, objectPath);
        mUpdateCallback (IARM_BUS_TR69HOSTIFMGR_EVENT_ADD, objectPath, NULL, hostIf_IntegerType);
        savedValue++;
//        sleep(1);
    }
}

void IPClientReqHandler::checkForUpdates()
{
    if (mUpdateCallback == 0)
        return;

    HOSTIF_MsgData_t msgData;
    bool bChanged;
    GList *elem;
    int index = 1;
    char objectPath[TR69HOSTIFMGR_MAX_PARAM_LEN];
    char instancePath[TR69HOSTIFMGR_MAX_PARAM_LEN];

    hostIf_IP::getLock();

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] \n",__FILE__, __FUNCTION__,__LINE__);

    memset(&msgData,0,sizeof(msgData));

    int interfaceNumberOfEntries = 0;
    if (hostIf_IP::get_Device_IP_InterfaceNumberOfEntries (&msgData) == OK)
    {
        interfaceNumberOfEntries = get_int (msgData.paramValue);
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s:%s:%d] interfaceNumberOfEntries = %d, curNumOfIPInterface = %d\n",
                __FILE__, __FUNCTION__, __LINE__, interfaceNumberOfEntries, curNumOfIPInterface);
        sprintf (objectPath, "Device.IP.Interface.");
        sendAddRemoveEvents (interfaceNumberOfEntries, curNumOfIPInterface, objectPath, instancePath);
    }

    for (int i = 1; i <= interfaceNumberOfEntries; i++)
    {
        int ipv4AddressNumberOfEntries = hostIf_IPInterface::getInstance (i)->getIPv4AddressNumberOfEntries ();
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s:%s:%d] ipv4AddressNumberOfEntries = %d, curNumOfInterfaceIPv4Addresses[%d] = %d\n",
                __FILE__, __FUNCTION__, __LINE__, ipv4AddressNumberOfEntries, i, curNumOfInterfaceIPv4Addresses[i]);
        sprintf (objectPath, "Device.IP.Interface.%d.IPv4Address.", i);
        sendAddRemoveEvents (ipv4AddressNumberOfEntries, curNumOfInterfaceIPv4Addresses[i], objectPath, instancePath);

#ifdef IPV6_SUPPORT
        int ipv6AddressNumberOfEntries = hostIf_IPInterface::getInstance (i)->getIPv6AddressNumberOfEntries ();
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s:%s:%d] ipv6AddressNumberOfEntries = %d, curNumOfInterfaceIPv6Addresses[%d] = %d\n",
                __FILE__, __FUNCTION__, __LINE__, ipv6AddressNumberOfEntries, i, curNumOfInterfaceIPv6Addresses[i]);
        sprintf (objectPath, "Device.IP.Interface.%d.IPv6Address.", i);
        sendAddRemoveEvents (ipv6AddressNumberOfEntries, curNumOfInterfaceIPv6Addresses[i], objectPath, instancePath);

        int ipv6PrefixNumberOfEntries = hostIf_IPInterface::getInstance (i)->getIPv6PrefixNumberOfEntries ();
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s:%s:%d] ipv6PrefixNumberOfEntries = %d, curNumOfInterfaceIPv6Prefixes[%d] = %d\n",
                __FILE__, __FUNCTION__, __LINE__, ipv6PrefixNumberOfEntries, i, curNumOfInterfaceIPv6Prefixes[i]);
        sprintf (objectPath, "Device.IP.Interface.%d.IPv6Prefix.", i);
        sendAddRemoveEvents (ipv6PrefixNumberOfEntries, curNumOfInterfaceIPv6Prefixes[i], objectPath, instancePath);

#endif // IPV6_SUPPORT
    }

    hostIf_IP::releaseLock();

#ifdef HAVE_ADD_REMOVE_EVENT

    hostIf_IP::getLock();

    index = 1;

    memset(&msgData,0,sizeof(msgData));

    if (hostIf_IP::get_Device_IP_ActivePortNumberOfEntries (&msgData) == OK)
    {
        int activePortNumberOfEntries = get_int (msgData.paramValue);
        sprintf (objectPath, "Device.IP.ActivePort.");
        sendAddRemoveEvents (activePortNumberOfEntries, curNumOfActivePorts[index], objectPath, instancePath);
    }

    hostIf_IP::releaseLock();

#endif /*HAVE_ADD_REMOVE_EVENT */

#ifdef HAVE_VALUE_CHANGE_EVENT
    hostIf_IPInterface::getLock();
    GList *devList = hostIf_IPInterface::getAllInstances();

    for(elem = devList; elem; elem = elem->next,index++)
    {
        hostIf_IPInterface *pIface = hostIf_IPInterface::getInstance((int)elem->data);

        if(pIface)
        {
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIface->get_Interface_Enable(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Enable");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIface->get_Interface_IPv4Enable(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"IPv4Enable");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIface->get_Interface_IPv6Enable(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"IPv6Enable");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIface->get_Interface_ULAEnable(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"ULAEnable");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIface->get_Interface_Status(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Status");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIface->get_Interface_Alias(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Alias");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIface->get_Interface_Name(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Name");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIface->get_Interface_LastChange(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"LastChange");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIface->get_Interface_LowerLayers(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"LowerLayers");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIface->get_Interface_Router(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Router");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIface->get_Interface_Reset(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Reset");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIface->get_Interface_MaxMTUSize(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"MaxMTUSize");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIface->get_Interface_Type(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Type");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIface->get_Interface_Loopback(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Loopback");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            if(pIface->get_Interface_IPv4AddressNumberOfEntries(&msgData) == OK)
            {
                int tmpNoDev = get_int(msgData.paramValue);
                char tmp[TR69HOSTIFMGR_MAX_PARAM_LEN] = "";
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"IPv4AddressNumberOfEntries");
                while(curNumOfIPv4Interface[index] > tmpNoDev)
                {
                    sprintf(tmp,"%s.%d.",objectPath,tmpNoDev);
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE,tmp, NULL, hostIf_IntegerType);
                    tmpNoDev++;
                }
                while(curNumOfIPv4Interface[index] < tmpNoDev)
                {
                    sprintf(tmp,"%s.",objectPath);
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_ADD,tmp, NULL, hostIf_IntegerType);
                    tmpNoDev--;
                }
                curNumOfIPv4Interface[index] = get_int(msgData.paramValue);
            }
            // TODO: similarly for IPv6AddressNumberOfEntries
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIface->get_Interface_AutoIPEnable(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"AutoIPEnable");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
        }
    }

    g_list_free(devList);

    devList = hostIf_IPInterfaceStats::getAllInstances();

    for(elem = devList; elem; elem = elem->next,index++)
    {
        hostIf_IPInterfaceStats *pIfaceStats = hostIf_IPInterfaceStats::getInstance((int)elem->data);
        if(pIfaceStats)
        {
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceStats->get_Device_IP_Interface_Stats_BytesSent(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Stats.BytesSent");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceStats->get_Device_IP_Interface_Stats_BytesReceived(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Stats.BytesReceived");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceStats->get_Device_IP_Interface_Stats_PacketsSent(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Stats.PacketsSent");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceStats->get_Device_IP_Interface_Stats_PacketsReceived(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Stats.PacketsReceived");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceStats->get_Device_IP_Interface_Stats_ErrorsSent(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Stats.ErrorsSent");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceStats->get_Device_IP_Interface_Stats_ErrorsReceived(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Stats.ErrorsReceived");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceStats->get_Device_IP_Interface_Stats_UnicastPacketsSent(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Stats.UnicastPacketsSent");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceStats->get_Device_IP_Interface_Stats_UnicastPacketsReceived(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Stats.UnicastPacketsReceived");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceStats->get_Device_IP_Interface_Stats_DiscardPacketsSent(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Stats.DiscardPacketsSent");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceStats->get_Device_IP_Interface_Stats_DiscardPacketsReceived(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Stats.DiscardPacketsReceived");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceStats->get_Device_IP_Interface_Stats_MulticastPacketsSent(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Stats.MulticastPacketsSent");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceStats->get_Device_IP_Interface_Stats_MulticastPacketsReceived(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Stats.MulticastPacketsReceived");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceStats->get_Device_IP_Interface_Stats_BroadcastPacketsSent(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Stats.PacketsSent");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceStats->get_Device_IP_Interface_Stats_BroadcastPacketsReceived(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Stats.BroadcastPacketsReceived");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceStats->get_Device_IP_Interface_Stats_UnknownProtoPacketsReceived(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.Interface.%d.%s",index,"Stats.UnknownProtoPacketsReceived");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
        }
    }

    g_list_free(devList);

    devList = hostIf_IPActivePort::getAllInstances();

    for(elem = devList; elem; elem = elem->next,index++)
    {
        hostIf_IPActivePort *pIfaceActive = hostIf_IPActivePort::getInstance((int)elem->data);
        if(pIfaceActive)
        {
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceActive->get_Device_IP_ActivePort_LocalIPAddress(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.ActivePort.%d.%s",index,"LocalIPAddress");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceActive->get_Device_IP_ActivePort_LocalPort(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.ActivePort.%d.%s",index,"LocalPort");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceActive->get_Device_IP_ActivePort_RemoteIPAddress(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.ActivePort.%d.%s",index,"RemoteIPAddress");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceActive->get_Device_IP_ActivePort_RemotePort(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.ActivePort.%d.%s",index,"RemotePort");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIfaceActive->get_Device_IP_ActivePort_Status(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.ActivePort.%d.%s",index,"Status");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
        }
    }

    g_list_free(devList);

    devList = hostIf_IP::getAllInstances();

    for(elem = devList; elem; elem = elem->next,index++)
    {
        hostIf_IP *pIPIface = hostIf_IP::getInstance((int)elem->data);
        if(pIPIface)
        {
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIPIface->get_Device_IP_IPv4Capable(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.%d.%s",index,"IPv4Capable");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIPIface->get_Device_IP_IPv4Enable(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.%d.%s",index,"IPv4Enable");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
/*
// TODO: Device.IP.IPv6Enable
#ifdef IPV6_SUPPORT
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIPIface->get_Device_IP_IPv6Enable(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.%d.%s",index,"IPv6Enable");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
#endif // IPV6_SUPPORT
*/
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIPIface->get_Device_IP_IPv4Status(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.%d.%s",index,"IPv4Status");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            bChanged =  false;
            pIPIface->get_Device_IP_ULAPrefix(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(objectPath,"Device.IP.%d.%s",index,"ULAPrefix");
                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
            }
        }
    }

    g_list_free(devList);

    devList = hostIf_IPv4Address::getAllInstances();
    for(elem = devList; elem; elem = elem->next,index++)
    {
        hostIf_IPv4Address *pIfaceIPv4 = hostIf_IPv4Address::getInstance(int(elem->data));
        if(pIfaceIPv4)
        {
            for(int devNum = 1; devNum <= curNumOfIPInterface; devNum++)
            {
                memset(&msgData,0,sizeof(msgData));
                bChanged = false;
                pIfaceIPv4->get_IPv4Address_Enable(&msgData,devNum,&bChanged);
                if(bChanged)
                {
                    sprintf(objectPath,"Device.IP.Interface.%d.IPv4Address.%d.%s",index,devNum,"Enable");
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
                }
                memset(&msgData,0,sizeof(msgData));
                bChanged = false;
                pIfaceIPv4->get_IPv4Address_Status(&msgData,devNum,&bChanged);
                if(bChanged)
                {
                    sprintf(objectPath,"Device.IP.Interface.%d.IPv4Address.%d.%s",index,devNum,"Status");
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
                }
                memset(&msgData,0,sizeof(msgData));
                bChanged = false;
                pIfaceIPv4->get_IPv4Address_Alias(&msgData,devNum,&bChanged);
                if(bChanged)
                {
                    sprintf(objectPath,"Device.IP.Interface.%d.IPv4Address.%d.%s",index,devNum,"Alias");
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
                }
                memset(&msgData,0,sizeof(msgData));
                bChanged = false;
                pIfaceIPv4->get_IPv4Address_SubnetMask(&msgData,devNum,&bChanged);
                if(bChanged)
                {
                    sprintf(objectPath,"Device.IP.Interface.%d.IPv4Address.%d.%s",index,devNum,"SubnetMask");
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
                }
                memset(&msgData,0,sizeof(msgData));
                bChanged = false;
                pIfaceIPv4->get_IPv4Address_AddressingType(&msgData,devNum,&bChanged);
                if(bChanged)
                {
                    sprintf(objectPath,"Device.IP.Interface.%d.IPv4Address.%d.%s",index,devNum,"AddressingType");
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
                }
                memset(&msgData,0,sizeof(msgData));
                bChanged = false;
                pIfaceIPv4->get_IPv4Address_IPAddress(&msgData,devNum,&bChanged);
                if(bChanged)
                {
                    sprintf(objectPath,"Device.IP.Interface.%d.IPv4Address.%d.%s",index,devNum,"IPAddress");
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,objectPath, msgData.paramValue, msgData.paramtype);
                }
            }
        }
    }

    g_list_free(devList);

    hostIf_IPInterface::releaseLock();
#endif  /* HAVE_VALUE_CHANGE_EVENT */
}








/** @} */
/** @} */
