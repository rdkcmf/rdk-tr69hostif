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


//#define HAVE_VALUE_CHANGE_EVENT
#include "hostIf_main.h"
#include "hostIf_IPClient_ReqHandler.h"
#include "Device_IP_ActivePort.h"
#include "Device_IP_Interface.h"
#include "Device_IP_Interface_IPv4Address.h"
#ifdef SPEEDTEST_SUPPORT
#include "Device_IP_Diagnostics_SpeedTest.h"
#endif //SPEEDTEST_SUPPORT
#ifdef IPV6_SUPPORT
#include "Device_IP_Interface_IPv6Address.h"
#endif // IPV6_SUPPORT
#include "Device_IP_Interface_Stats.h"
#include "Device_IP.h"
#include "Device_IP_Diagnostics_IPPing.h"
#include <mutex>

std::mutex IPClientReqHandler::m_mutex;
IPClientReqHandler* IPClientReqHandler::pInstance = NULL;
updateCallback IPClientReqHandler::mUpdateCallback = NULL;
int IPClientReqHandler::curNumOfIPInterface = 0;
int IPClientReqHandler::curNumOfInterfaceIPv4Addresses[20] = {0,};
#ifdef IPV6_SUPPORT
int IPClientReqHandler::curNumOfInterfaceIPv6Addresses[20] = {0,};
int IPClientReqHandler::curNumOfInterfaceIPv6Prefixes[20] = {0,};
#endif // IPV6_SUPPORT
int IPClientReqHandler::curNumOfIPv4Interface[20] = {0,};
int IPClientReqHandler::curNumOfActivePorts[20] = {0,};

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

void IPClientReqHandler::reset()
{
    std::lock_guard<std::mutex> lg (m_mutex);

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

    std::lock_guard<std::mutex> lg (m_mutex);

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
    else if (strncasecmp (stMsgData->paramName, hostIf_IP_Diagnostics_IPPing::PROFILE_NAME, strlen (hostIf_IP_Diagnostics_IPPing::PROFILE_NAME)) == 0)
    {
        hostIf_IP_Diagnostics_IPPing& ipping = hostIf_IP_Diagnostics_IPPing::getInstance ();
        ret = ipping.handleGetMsg (stMsgData);
    }
#ifdef SPEEDTEST_SUPPORT
    else if (strncasecmp (stMsgData->paramName, hostIf_IP_Diagnostics_SpeedTest::SpeedTestProfile, strlen (hostIf_IP_Diagnostics_SpeedTest::SpeedTestProfile)) == 0)
    {
        hostIf_IP_Diagnostics_SpeedTest& spdtst = hostIf_IP_Diagnostics_SpeedTest::getInstance ();
        ret = spdtst.handleGetMsg (stMsgData);
    }
#endif
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
    else
    {
         RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]  Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
         stMsgData->faultCode = fcInvalidParameterName;
         ret = NOK;
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

    std::lock_guard<std::mutex> lg (m_mutex);

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
    else if (strncasecmp (stMsgData->paramName, hostIf_IP_Diagnostics_IPPing::PROFILE_NAME, strlen (hostIf_IP_Diagnostics_IPPing::PROFILE_NAME)) == 0)
    {
        hostIf_IP_Diagnostics_IPPing& ipping = hostIf_IP_Diagnostics_IPPing::getInstance ();
        ret = ipping.handleSetMsg (stMsgData);
    }
#ifdef SPEEDTEST_SUPPORT
    else if (strncasecmp (stMsgData->paramName, hostIf_IP_Diagnostics_SpeedTest::SpeedTestProfile, strlen (hostIf_IP_Diagnostics_SpeedTest::SpeedTestProfile)) == 0)
    {
	hostIf_IP_Diagnostics_SpeedTest& spdtst = hostIf_IP_Diagnostics_SpeedTest::getInstance ();
	ret = spdtst.handleSetMsg (stMsgData);
    }
#endif
    else if (strncasecmp (stMsgData->paramName, "Device.IP", strlen ("Device.IP")))
    {
        stMsgData->instanceNum = instanceNumber;
        hostIf_IP *pIPIface = hostIf_IP::getInstance (instanceNumber);
        if (!pIPIface)
            return NOK;
        ret = pIPIface->handleSetMsg (stMsgData);
    }
    stMsgData->faultCode = (OK == ret)?fcNoFault:fcRequestDenied;
    return ret;
}

int IPClientReqHandler::handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    int instanceNumber = 0;

    std::lock_guard<std::mutex> lg (m_mutex);

    hostIf_IP *pIface = hostIf_IP::getInstance(instanceNumber);
    stMsgData->instanceNum = instanceNumber;
    if(!pIface)
    {
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

    return ret;
}

int IPClientReqHandler::handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    int instanceNumber = 0;

    std::lock_guard<std::mutex> lg (m_mutex);

    hostIf_IP *pIface = hostIf_IP::getInstance(instanceNumber);
    stMsgData->instanceNum = instanceNumber;
    if(!pIface)
    {
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
    return ret;
}
void IPClientReqHandler::registerUpdateCallback(updateCallback cb)
{
    mUpdateCallback = cb;
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
        sendAddRemoveEvents (mUpdateCallback, interfaceNumberOfEntries, curNumOfIPInterface, objectPath);
    }

    for (int i = 1; i <= interfaceNumberOfEntries; i++)
    {
        int ipv4AddressNumberOfEntries = hostIf_IPInterface::getInstance (i)->getIPv4AddressNumberOfEntries ();
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s:%s:%d] ipv4AddressNumberOfEntries = %d, curNumOfInterfaceIPv4Addresses[%d] = %d\n",
                __FILE__, __FUNCTION__, __LINE__, ipv4AddressNumberOfEntries, i, curNumOfInterfaceIPv4Addresses[i]);
        sprintf (objectPath, "Device.IP.Interface.%d.IPv4Address.", i);
        sendAddRemoveEvents (mUpdateCallback, ipv4AddressNumberOfEntries, curNumOfInterfaceIPv4Addresses[i], objectPath);

#ifdef IPV6_SUPPORT
        int ipv6AddressNumberOfEntries = hostIf_IPInterface::getInstance (i)->getIPv6AddressNumberOfEntries ();
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s:%s:%d] ipv6AddressNumberOfEntries = %d, curNumOfInterfaceIPv6Addresses[%d] = %d\n",
                __FILE__, __FUNCTION__, __LINE__, ipv6AddressNumberOfEntries, i, curNumOfInterfaceIPv6Addresses[i]);
        sprintf (objectPath, "Device.IP.Interface.%d.IPv6Address.", i);
        sendAddRemoveEvents (mUpdateCallback, ipv6AddressNumberOfEntries, curNumOfInterfaceIPv6Addresses[i], objectPath);

        int ipv6PrefixNumberOfEntries = hostIf_IPInterface::getInstance (i)->getIPv6PrefixNumberOfEntries ();
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s:%s:%d] ipv6PrefixNumberOfEntries = %d, curNumOfInterfaceIPv6Prefixes[%d] = %d\n",
                __FILE__, __FUNCTION__, __LINE__, ipv6PrefixNumberOfEntries, i, curNumOfInterfaceIPv6Prefixes[i]);
        sprintf (objectPath, "Device.IP.Interface.%d.IPv6Prefix.", i);
        sendAddRemoveEvents (mUpdateCallback, ipv6PrefixNumberOfEntries, curNumOfInterfaceIPv6Prefixes[i], objectPath);

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
        sendAddRemoveEvents (mUpdateCallback, activePortNumberOfEntries, curNumOfActivePorts[index], objectPath);
    }

    hostIf_IP::releaseLock();

#endif /*HAVE_ADD_REMOVE_EVENT */

#ifdef HAVE_VALUE_CHANGE_EVENT

    std::lock_guard<std::mutex> lg (m_mutex);

    int instanceNumber = 0;
    GHashTable* notifyhash = NULL;


    //Get Notify Hash from device Info
    hostIf_IP *dIface = hostIf_IP::getInstance(instanceNumber);
    if(NULL != dIface)
    {
        notifyhash = dIface->getNotifyHash();
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Unable to get Device IP Instance\n", __FUNCTION__, __FILE__);
    }

    // Iterate through Ghash Table
    if(NULL != notifyhash)
    {
        GHashTableIter notifyHashIterator;
        gpointer paramName;
        gpointer notifyEnable;
        bool  bChanged;
        const char *pSetting;
        int instanceNumber;
        const char *positionAfterSubInstanceNumber = 0;
        const char *positionAfterInstanceNumber = 0;
        int subInstanceNumber;

        g_hash_table_iter_init (&notifyHashIterator, notifyhash);
        while (g_hash_table_iter_next (&notifyHashIterator, &paramName, &notifyEnable))
        {
            int* isNotifyEnabled = (int *)notifyEnable;
            instanceNumber = 0;
            if(matchComponent((const char*)paramName,"Device.IP.Interface",&pSetting,instanceNumber))
            {
                if(!instanceNumber)
                {   // Ethernet settings not found in Notify Hash Table
                    continue;
                }
                hostIf_IPInterface *pIface = hostIf_IPInterface::getInstance(instanceNumber);
                if(pIface)
                {
                    if (strcasecmp(pSetting,"Enable") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Interface_Enable(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"IPv4Enable") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Interface_IPv4Enable(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"IPv6Enable") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Interface_IPv6Enable(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"ULAEnable") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Interface_ULAEnable(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }

                    else if (strcasecmp(pSetting,"Status") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Interface_Status(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Alias") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Interface_Alias(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Name") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Interface_Name(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"LastChange") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Interface_LastChange(&msgData,&bChanged);
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
                        pIface->get_Interface_LowerLayers(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Router") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Interface_Router(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Reset") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Interface_Reset(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if(strcasecmp(pSetting,"MaxMTUSize") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Interface_MaxMTUSize(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if(strcasecmp(pSetting,"Type") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Interface_Type(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if(strcasecmp(pSetting,"Loopback") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Interface_Loopback(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if(strcasecmp(pSetting,"AutoIPEnable") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Interface_AutoIPEnable(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if(strncasecmp (pSetting, "Stats", strlen ("Stats")) == 0)
                    {
                        hostIf_IPInterfaceStats *pIfaceStats = hostIf_IPInterfaceStats::getInstance(instanceNumber);
                        if(strcasecmp(pSetting,"Stats.BytesSent") == 0)
                        {
                            memset(&msgData,0,sizeof(msgData));
                            bChanged =  false;
                            pIfaceStats->get_Device_IP_Interface_Stats_BytesSent(&msgData,&bChanged);
                            if(bChanged)
                            {
                                if(mUpdateCallback && (*isNotifyEnabled == 1))
                                {
                                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                }
                            }
                        }
                        else if(strcasecmp(pSetting,"Stats.BytesReceived") == 0)
                        {
                            memset(&msgData,0,sizeof(msgData));
                            bChanged =  false;
                            pIfaceStats->get_Device_IP_Interface_Stats_BytesReceived(&msgData,&bChanged);
                            if(bChanged)
                            {
                                if(mUpdateCallback && (*isNotifyEnabled == 1))
                                {
                                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                }
                            }
                        }
                        else if(strcasecmp(pSetting,"Stats.PacketsSent") == 0)
                        {
                            memset(&msgData,0,sizeof(msgData));
                            bChanged =  false;
                            pIfaceStats->get_Device_IP_Interface_Stats_PacketsSent(&msgData,&bChanged);
                            if(bChanged)
                            {
                                if(mUpdateCallback && (*isNotifyEnabled == 1))
                                {
                                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                }
                            }
                        }
                        else if(strcasecmp(pSetting,"Stats.PacketsReceived") == 0)
                        {
                            memset(&msgData,0,sizeof(msgData));
                            bChanged =  false;
                            pIfaceStats->get_Device_IP_Interface_Stats_PacketsReceived(&msgData,&bChanged);
                            if(bChanged)
                            {
                                if(mUpdateCallback && (*isNotifyEnabled == 1))
                                {
                                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                }
                            }
                        }
                        else if(strcasecmp(pSetting,"Stats.ErrorsSent") == 0)
                        {
                            memset(&msgData,0,sizeof(msgData));
                            bChanged =  false;
                            pIfaceStats->get_Device_IP_Interface_Stats_ErrorsSent(&msgData,&bChanged);
                            if(bChanged)
                            {
                                if(mUpdateCallback && (*isNotifyEnabled == 1))
                                {
                                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                }
                            }
                        }
                        else if(strcasecmp(pSetting,"Stats.ErrorsReceived") == 0)
                        {
                            memset(&msgData,0,sizeof(msgData));
                            bChanged =  false;
                            pIfaceStats->get_Device_IP_Interface_Stats_ErrorsReceived(&msgData,&bChanged);
                            if(bChanged)
                            {
                                if(mUpdateCallback && (*isNotifyEnabled == 1))
                                {
                                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                }
                            }
                        }
                        else if(strcasecmp(pSetting,"Stats.UnicastPacketsSent") == 0)
                        {
                            memset(&msgData,0,sizeof(msgData));
                            bChanged =  false;
                            pIfaceStats->get_Device_IP_Interface_Stats_UnicastPacketsSent(&msgData,&bChanged);
                            if(bChanged)
                            {
                                if(mUpdateCallback && (*isNotifyEnabled == 1))
                                {
                                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                }
                            }
                        }
                        else if(strcasecmp(pSetting,"Stats.UnicastPacketsSent") == 0)
                        {
                            memset(&msgData,0,sizeof(msgData));
                            bChanged =  false;
                            pIfaceStats->get_Device_IP_Interface_Stats_UnicastPacketsSent(&msgData,&bChanged);
                            if(bChanged)
                            {
                                if(mUpdateCallback && (*isNotifyEnabled == 1))
                                {
                                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                }
                            }
                        }
                        else if(strcasecmp(pSetting,"Stats.UnicastPacketsReceived") == 0)
                        {
                            memset(&msgData,0,sizeof(msgData));
                            bChanged =  false;
                            pIfaceStats->get_Device_IP_Interface_Stats_UnicastPacketsReceived(&msgData,&bChanged);
                            if(bChanged)
                            {
                                if(mUpdateCallback && (*isNotifyEnabled == 1))
                                {
                                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                }
                            }
                        }
                        else if(strcasecmp(pSetting,"Stats.DiscardPacketsSent") == 0)
                        {
                            memset(&msgData,0,sizeof(msgData));
                            bChanged =  false;
                            pIfaceStats->get_Device_IP_Interface_Stats_DiscardPacketsSent(&msgData,&bChanged);
                            if(bChanged)
                            {
                                if(mUpdateCallback && (*isNotifyEnabled == 1))
                                {
                                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                }
                            }
                        }
                        else if(strcasecmp(pSetting,"Stats.DiscardPacketsReceived") == 0)
                        {
                            memset(&msgData,0,sizeof(msgData));
                            bChanged =  false;
                            pIfaceStats->get_Device_IP_Interface_Stats_DiscardPacketsReceived(&msgData,&bChanged);
                            if(bChanged)
                            {
                                if(mUpdateCallback && (*isNotifyEnabled == 1))
                                {
                                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                }
                            }
                        }
                        else if(strcasecmp(pSetting,"Stats.MulticastPacketsSent") == 0)
                        {
                            memset(&msgData,0,sizeof(msgData));
                            bChanged =  false;
                            pIfaceStats->get_Device_IP_Interface_Stats_MulticastPacketsSent(&msgData,&bChanged);
                            if(bChanged)
                            {
                                if(mUpdateCallback && (*isNotifyEnabled == 1))
                                {
                                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                }
                            }
                        }
                        else if(strcasecmp(pSetting,"Stats.MulticastPacketsReceived") == 0)
                        {
                            memset(&msgData,0,sizeof(msgData));
                            bChanged =  false;
                            pIfaceStats->get_Device_IP_Interface_Stats_MulticastPacketsReceived(&msgData,&bChanged);
                            if(bChanged)
                            {
                                if(mUpdateCallback && (*isNotifyEnabled == 1))
                                {
                                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                }
                            }
                        }
                        else if(strcasecmp(pSetting,"Stats.PacketsSent") == 0)
                        {
                            memset(&msgData,0,sizeof(msgData));
                            bChanged =  false;
                            pIfaceStats->get_Device_IP_Interface_Stats_BroadcastPacketsSent(&msgData,&bChanged);
                            if(bChanged)
                            {
                                if(mUpdateCallback && (*isNotifyEnabled == 1))
                                {
                                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                }
                            }
                        }
                        else if(strcasecmp(pSetting,"Stats.BroadcastPacketsReceived") == 0)
                        {
                            memset(&msgData,0,sizeof(msgData));
                            bChanged =  false;
                            pIfaceStats->get_Device_IP_Interface_Stats_BroadcastPacketsSent(&msgData,&bChanged);
                            if(bChanged)
                            {
                                if(mUpdateCallback && (*isNotifyEnabled == 1))
                                {
                                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                }
                            }
                        }
                        else if(strcasecmp(pSetting,"Stats.UnknownProtoPacketsReceived") == 0)
                        {
                            memset(&msgData,0,sizeof(msgData));
                            bChanged =  false;
                            pIfaceStats->get_Device_IP_Interface_Stats_UnknownProtoPacketsReceived(&msgData,&bChanged);
                            if(bChanged)
                            {
                                if(mUpdateCallback && (*isNotifyEnabled == 1))
                                {
                                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                }
                            }
                        }

                    }
                    else if((positionAfterInstanceNumber != NULL) && (matchComponent(positionAfterInstanceNumber, "IPv4Address", &positionAfterSubInstanceNumber, subInstanceNumber)))  //CID:80424 - Forward Null
                    {
                        hostIf_IPv4Address *pIfaceIPv4 = hostIf_IPv4Address::getInstance(instanceNumber);
                        if(pIfaceIPv4)
                        {
                            if(strcasecmp(pSetting,"Enable") == 0)
                            {
                                memset(&msgData,0,sizeof(msgData));
                                bChanged =  false;
                                pIfaceIPv4->get_IPv4Address_Enable(&msgData,subInstanceNumber,&bChanged);
                                if(bChanged)
                                {
                                    if(mUpdateCallback && (*isNotifyEnabled == 1))
                                    {
                                        mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                    }
                                }
                                else if(strcasecmp(pSetting,"Status") == 0)
                                {
                                    memset(&msgData,0,sizeof(msgData));
                                    bChanged =  false;
                                    pIfaceIPv4->get_IPv4Address_Status(&msgData,subInstanceNumber,&bChanged);
                                    if(bChanged)
                                    {
                                        if(mUpdateCallback && (*isNotifyEnabled == 1))
                                        {
                                            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                        }
                                    }
                                }
                                else if(strcasecmp(pSetting,"Alias") == 0)
                                {
                                    memset(&msgData,0,sizeof(msgData));
                                    bChanged =  false;
                                    pIfaceIPv4->get_IPv4Address_Alias(&msgData,subInstanceNumber,&bChanged);
                                    if(bChanged)
                                    {
                                        if(mUpdateCallback && (*isNotifyEnabled == 1))
                                        {
                                            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                        }
                                    }
                                }
                                else if(strcasecmp(pSetting,"SubnetMask") == 0)
                                {
                                    memset(&msgData,0,sizeof(msgData));
                                    bChanged =  false;
                                    pIfaceIPv4->get_IPv4Address_SubnetMask(&msgData,subInstanceNumber,&bChanged);
                                    if(bChanged)
                                    {
                                        if(mUpdateCallback && (*isNotifyEnabled == 1))
                                        {
                                            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                        }
                                    }
                                }
                                else if(strcasecmp(pSetting,"AddressingType") == 0)
                                {
                                    memset(&msgData,0,sizeof(msgData));
                                    bChanged =  false;
                                    pIfaceIPv4->get_IPv4Address_AddressingType(&msgData,subInstanceNumber,&bChanged);
                                    if(bChanged)
                                    {
                                        if(mUpdateCallback && (*isNotifyEnabled == 1))
                                        {
                                            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                                        }
                                    }
                                }
                                else if(strcasecmp(pSetting,"IPAddress") == 0)
                                {
                                    memset(&msgData,0,sizeof(msgData));
                                    bChanged =  false;
                                    pIfaceIPv4->get_IPv4Address_IPAddress(&msgData,subInstanceNumber,&bChanged);
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

                }
            }
            else if(matchComponent((const char*)paramName,"Device.IP.ActivePort",&pSetting,instanceNumber))
            {
                hostIf_IPActivePort *pIfaceActive = hostIf_IPActivePort::getInstance(instanceNumber);
                if(pIfaceActive)
                {
                    if (strcasecmp(pSetting,"LocalIPAddress") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceActive->get_Device_IP_ActivePort_LocalIPAddress(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"LocalPort") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceActive->get_Device_IP_ActivePort_LocalPort(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"RemoteIPAddress") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceActive->get_Device_IP_ActivePort_RemoteIPAddress(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"RemotePort") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceActive->get_Device_IP_ActivePort_RemotePort(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"Status") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIfaceActive->get_Device_IP_ActivePort_Status(&msgData,&bChanged);
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
            else if(matchComponent((const char*)paramName,"Device.IP",&pSetting,instanceNumber))
            {
                hostIf_IP *pIPIface = hostIf_IP::getInstance(instanceNumber);
                if(pIPIface)
                {
                    if (strcasecmp(pSetting,"IPv4Capable") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIPIface->get_Device_IP_IPv4Capable(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"IPv4Enable") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIPIface->get_Device_IP_IPv4Enable(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"IPv4Status") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIPIface->get_Device_IP_IPv4Status(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    else if (strcasecmp(pSetting,"ULAPrefix") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIPIface->get_Device_IP_ULAPrefix(&msgData,&bChanged);
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

#endif  /* HAVE_VALUE_CHANGE_EVENT */
}








/** @} */
/** @} */
