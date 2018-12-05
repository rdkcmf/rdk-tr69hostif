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


#include "hostIf_updateHandler.h"
#include "hostIf_MoCAClient_ReqHandler.h"
#include "hostIf_XREClient_ReqHandler.h"
#include "hostIf_EthernetClient_ReqHandler.h"
#include "hostIf_IPClient_ReqHandler.h"
#include "hostIf_TimeClient_ReqHandler.h"
#include "hostIf_dsClient_ReqHandler.h"
#include "hostIf_DeviceClient_ReqHandler.h"
#include "hostIf_NotificationHandler.h"
#include <mutex>

#ifdef USE_WIFI_PROFILE
#include "hostIf_WiFi_ReqHandler.h"
#endif

#ifdef USE_DHCPv4_PROFILE
#include "hostIf_DHCPv4Client_ReqHandler.h"
#endif /* WITH_DHCP_PROFILE*/

#ifdef USE_STORAGESERVICE_PROFILE
#include "hostIf_StorageSrvc_ReqHandler.h"
#endif /* USE_STORAGESERVICE_PROFILE */

#ifdef USE_INTFSTACK_PROFILE
#include "hostIf_InterfaceStackClient_ReqHandler.h"
#endif /* USE_INTFSTACK_PROFILE */

#ifdef USE_XRESRC
#include "Device_XComcast_Xcalibur_Client_XRE_ConnectionTable.h"
#endif

#ifdef USE_XRDK_BT_PROFILE
#include "XrdkBlueTooth.h"
#endif

GThread * updateHandler::thread = NULL;
bool updateHandler::stopped = false;

int updateHandler::Init()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FILE__, __FUNCTION__);

#ifdef USE_MoCA_PROFILE
    MoCAClientReqHandler::registerUpdateCallback(notifyCallback);
#endif /*USE_MoCA_PROFILE*/

    /* XREClientReqHandler::registerUpdateCallback(notifyCallback); */
    EthernetClientReqHandler::registerUpdateCallback(notifyCallback);
    /* DSClientReqHandler::registerUpdateCallback(notifyCallback);*/
    IPClientReqHandler::registerUpdateCallback(notifyCallback);
    DeviceClientReqHandler::registerUpdateCallback(notifyCallback);
    /*TimeClientReqHandler::registerUpdateCallback(notifyCallback);*/

#ifdef USE_WIFI_PROFILE
    WiFiReqHandler::registerUpdateCallback(notifyCallback);
#endif

#ifdef USE_DHCPv4_PROFILE
    DHCPv4ClientReqHandler::registerUpdateCallback(notifyCallback);
#endif /* USE_DHCPv4_PROFILE*/	
#ifdef USE_INTFSTACK_PROFILE		
    InterfaceStackClientReqHandler::registerUpdateCallback(notifyCallback);
#endif /* USE_STORAGESERVICE_PROFILE */	

#ifdef USE_XRESRC
    XREClientReqHandler::registerUpdateCallback(notifyCallback);
#endif
    thread=g_thread_create(run,NULL,TRUE,NULL);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FILE__, __FUNCTION__);
}

void updateHandler::stop()
{
    stopped = true;
}

void updateHandler::reset()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n",  __FILE__, __FUNCTION__);
#ifdef USE_MoCA_PROFILE
    MoCAClientReqHandler::reset();
#endif /* USE_MoCA_PROFILE */
    EthernetClientReqHandler::reset();
    IPClientReqHandler::reset();
    DeviceClientReqHandler::reset();
#ifdef USE_WIFI_PROFILE
    WiFiReqHandler::reset();
#endif

#ifdef USE_INTFSTACK_PROFILE
    InterfaceStackClientReqHandler::reset();
#endif
#if 0
#ifdef USE_XRESRC
   XREClientReqHandler::reset();
#endif
#endif
#ifdef USE_XRDK_BT_PROFILE
    hostIf_DeviceInfoRdk_xBT::reset();
#endif

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FILE__, __FUNCTION__);
}



gpointer updateHandler::run(gpointer ptr)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] updateHandler::run Entering..\n", __FILE__, __FUNCTION__);
//	sleep(120);
    while(!stopped)
    {
    	RDK_LOG(RDK_LOG_TRACE2,LOG_TR69HOSTIF,"[%s:%s:%d] Entering..\n", __FILE__, __FUNCTION__, __LINE__);
#ifdef USE_MoCA_PROFILE
        MoCAClientReqHandler::checkForUpdates();
#endif /* USE_MoCA_PROFILE*/
        EthernetClientReqHandler::checkForUpdates();
        IPClientReqHandler::checkForUpdates();
        DeviceClientReqHandler::checkForUpdates();
#ifdef USE_WIFI_PROFILE
        WiFiReqHandler::checkForUpdates();
#endif


#ifdef USE_DHCPv4_PROFILE
        DHCPv4ClientReqHandler::checkForUpdates();
#endif /* USE_DHCPv4_PROFILE*/	
#ifdef USE_INTFSTACK_PROFILE		
        InterfaceStackClientReqHandler::checkForUpdates();
#endif /* USE_STORAGESERVICE_PROFILE */		

        /* No need to update now, since it has one instances */
        /*XREClientReqHandler::checkForUpdates();*/

        /* Here no need to add event by agent. The agent will not
         * be responsible for creating instances. The ACS can create
         * instance and get the value for that instances. */
        /*DSClientReqHandler::checkForUpdates();*/
        /*TimeClientReqHandler::checkForUpdates(); */
#if 0
#ifdef USE_XRESRC
        XREClientReqHandler::checkForUpdates();
#endif
#endif
        sleep(60);
        RDK_LOG(RDK_LOG_TRACE2,LOG_TR69HOSTIF,"[%s:%s:%d] Exiting..\n", __FILE__, __FUNCTION__, __LINE__);
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FILE__, __FUNCTION__);
    return ptr;
}

void updateHandler::notifyCallback(IARM_Bus_tr69HostIfMgr_EventId_t event_type, const char* paramName, const char* paramValue, HostIf_ParamType_t paramtype)
{
    IARM_Bus_tr69HostIfMgr_EventData_t eventData;
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] updateHandler::notifyCallback for Parameter :- %s..\n", __FILE__, __FUNCTION__,paramName);
    strncpy(eventData.paramName,paramName, TR69HOSTIFMGR_MAX_PARAM_LEN);

    if(paramValue)
    {
        strncpy(eventData.paramValue, paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN);
    }

    eventData.paramtype = paramtype;
    IARM_Bus_BroadcastEvent(IARM_BUS_TR69HOSTIFMGR_NAME, (IARM_EventId_t) event_type, (void *)&eventData, sizeof(eventData));
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] Inside Parodus :- %s..\n", __FILE__, __FUNCTION__,paramName);
    NotificationHandler* pInstance =  NULL;
    pInstance = NotificationHandler::getInstance();
    if(pInstance)
    pInstance->addNotificationToQueue(IARM_BUS_TR69HOSTIFMGR_NAME,event_type, (void *)&eventData, sizeof(eventData));
}

void sendAddRemoveEvents (updateCallback callbackFn, int newValue, int& savedValue, char* objectPath)
{
    // function lock to serialize sending add/remove events
    static std::mutex m;

    static char instancePath[TR69HOSTIFMGR_MAX_PARAM_LEN];

    // grab function lock
    std::lock_guard<std::mutex> lg(m);

    while (savedValue > newValue)
    {
        sprintf (instancePath, "%s%d.", objectPath, savedValue);
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE %s\n", __FUNCTION__, instancePath);
        callbackFn (IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE, instancePath, NULL, hostIf_IntegerType);
        savedValue--;
//        sleep(1);
    }
    while (savedValue < newValue)
    {
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] IARM_BUS_TR69HOSTIFMGR_EVENT_ADD %s\n", __FUNCTION__, objectPath);
        callbackFn (IARM_BUS_TR69HOSTIFMGR_EVENT_ADD, objectPath, NULL, hostIf_IntegerType);
        savedValue++;
//        sleep(1);
    }
}

/** @} */
/** @} */
