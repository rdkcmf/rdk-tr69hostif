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


#include "hostIf_main.h"
#include "hostIf_msgHandler.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_dsClient_ReqHandler.h"
#include "hostIf_DeviceClient_ReqHandler.h"
#include "hostIf_updateHandler.h"
#include "hostIf_utils.h"
#include "libIBus.h"
#include "libIARM.h"
#include "sysMgr.h"
#include "integrationSettings.h"

#ifdef USE_XRDK_BT_PROFILE
#include "btmgr.h"
#include "btmgr_iarm_interface.h"
#include "XrdkBlueTooth.h"
#endif

static bool TR69_HostIf_Mgr_Init();
static bool TR69_HostIf_Mgr_Connect();
static bool TR69_HostIf_Mgr_Get_RegisterCall();
static IARM_Result_t tr69hostIfMgr_Stop(void);
static IARM_Result_t _Gettr69HostIfMgr(void *arg);
static IARM_Result_t _Settr69HostIfMgr(void *arg);
static IARM_Result_t _RegisterForEventstr69HostIfMgr(void *arg);
static void _hostIf_EventHandler(const char *, IARM_EventId_t, void *, size_t);
#ifdef USE_XRDK_BT_PROFILE
static void BTMgr_EventHandler(const char *, IARM_EventId_t , void *, size_t );
#endif
//----------------------------------------------------------------------
// hostIf_IARM_IF_Start: This shall be use to initialize and register
// the  hostIf application to IARM bus.
//----------------------------------------------------------------------
bool hostIf_IARM_IF_Start()
{
    bool ret = false;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    if ( TR69_HostIf_Mgr_Init() &&
            TR69_HostIf_Mgr_Connect() &&
            TR69_HostIf_Mgr_Get_RegisterCall() )
    {
        ret = true;
        /* Initialize Managers */
        msgHandler *pMsgHandler;
        pMsgHandler = DSClientReqHandler::getInstance();
        pMsgHandler->init();
        pMsgHandler = DeviceClientReqHandler::getInstance();
        pMsgHandler->init();

    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);

    return ret;
}

//----------------------------------------------------------------------
//Initialization: This shall be initialized tr69 application to IARM bus.
//----------------------------------------------------------------------
static bool TR69_HostIf_Mgr_Init()
{
    IARM_Result_t err = IARM_RESULT_SUCCESS;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    err = IARM_Bus_Init(IARM_BUS_TR69HOSTIFMGR_NAME);

    if(IARM_RESULT_SUCCESS != err)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s()] Failed to 'IARM_Bus_Init(%s)', return with Error "\
                "code: (%d).\n", __FUNCTION__,IARM_BUS_TR69HOSTIFMGR_NAME, err);
        return false;
    }
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] Success 'IARM_Bus_Init(%s)'.\n", __FUNCTION__,    IARM_BUS_TR69HOSTIFMGR_NAME);

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}
//----------------------------------------------------------------------
// Connect to IARM Bus:
//----------------------------------------------------------------------
static bool TR69_HostIf_Mgr_Connect()
{
    IARM_Result_t err = IARM_RESULT_SUCCESS;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);

    err = IARM_Bus_Connect();

    if(IARM_RESULT_SUCCESS != err)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed to IARM_Bus_Connect(), return with Error code: %d\n", __FUNCTION__, err);
        IARM_Bus_Term();
        return false;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

static IARM_Result_t _RegisterForEventstr69HostIfMgr(void *arg)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    updateHandler::reset();
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return IARM_RESULT_SUCCESS;

}


//----------------------------------------------------------------------
//Register RPC call: This shall be RPC call registered to IARM bus.
//----------------------------------------------------------------------
static bool TR69_HostIf_Mgr_Get_RegisterCall()
{
    bool ret = true;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    /* Get RPC: to get the value*/
    if( (IARM_Bus_RegisterCall(IARM_BUS_TR69HOSTIFMGR_API_GetParams, _Gettr69HostIfMgr) != IARM_RESULT_SUCCESS) ||
            (IARM_Bus_RegisterCall(IARM_BUS_TR69HOSTIFMGR_API_SetParams, _Settr69HostIfMgr) != IARM_RESULT_SUCCESS) ||
            (IARM_Bus_RegisterCall(IARM_BUS_TR69HOSTIFMGR_API_RegisterForEvents, _RegisterForEventstr69HostIfMgr) != IARM_RESULT_SUCCESS))
    {
        ret = false;
    }

    /* Notification RPC:*/
    IARM_Bus_RegisterEvent(IARM_BUS_TR69HOSTIFMGR_EVENT_MAX);

    /* Register Event Handler for IARM_TR69_CLIENT for ACS Connection status */
    IARM_Bus_RegisterEventHandler(IARM_TR69_CLIENT, IARM_BUS_TR69Agent_ACS_CONN_EVENT, _hostIf_EventHandler);

    /* Register Sys manager Event Handler for IARM_TR69_CLIENT for ACS Connection status */
    IARM_Bus_RegisterEventHandler(IARM_BUS_SYSMGR_NAME, IARM_BUS_SYSMGR_SYSSTATE_GATEWAY_CONNECTION, _hostIf_EventHandler);

#ifdef USE_XRDK_BT_PROFILE
    IARM_Bus_RegisterEventHandler(IARM_BUS_BTMGR_NAME, BTMGR_IARM_EVENT_DEVICE_DISCOVERY_COMPLETE, BTMgr_EventHandler);
    IARM_Bus_RegisterEventHandler(IARM_BUS_BTMGR_NAME, BTMGR_IARM_EVENT_DEVICE_PAIRING_COMPLETE, BTMgr_EventHandler);
    IARM_Bus_RegisterEventHandler(IARM_BUS_BTMGR_NAME, BTMGR_IARM_EVENT_DEVICE_UNPAIRING_COMPLETE, BTMgr_EventHandler);
    IARM_Bus_RegisterEventHandler(IARM_BUS_BTMGR_NAME, BTMGR_IARM_EVENT_DEVICE_CONNECTION_COMPLETE, BTMgr_EventHandler);
    IARM_Bus_RegisterEventHandler(IARM_BUS_BTMGR_NAME, BTMGR_IARM_EVENT_DEVICE_DISCONNECT_COMPLETE, BTMgr_EventHandler);
#endif /*	#ifdef USE_XRDK_BT_PROFILE*/

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return ret;
}



//----------------------------------------------------------------------
//Initialization: This shall be initialized tr69 application to IARM bus.
//----------------------------------------------------------------------
void hostIf_IARM_IF_Stop()
{
    /* Uninitialized Device setting client*/
    msgHandler *pMsgHandler = DSClientReqHandler::getInstance();
    pMsgHandler->unInit();
    tr69hostIfMgr_Stop();
}
//----------------------------------------------------------------------
//Initialization: This shall be initialized tr69 application to IARM bus.
//----------------------------------------------------------------------
static IARM_Result_t tr69hostIfMgr_Stop(void)
{
    IARM_Result_t err = IARM_RESULT_SUCCESS;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering...\n", __FUNCTION__, __FILE__);
    err = IARM_Bus_Disconnect();
    if(IARM_RESULT_SUCCESS != err)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed to IARM_Bus_Disconnect(), return with Error code: %d\n", __FUNCTION__, err);
    }

    err = IARM_Bus_Term();
    if(IARM_RESULT_SUCCESS != err)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed to IARM_Bus_Term(), return with Error code: %d\n", __FUNCTION__, err);
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return err;
}

void hostIf_SetReqHandler(void *arg)
{
    int ret;
    HOSTIF_MsgData_t *stMsgData = (HOSTIF_MsgData_t *) arg;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    if(stMsgData)
    {
        g_mutex_lock(request_handler_mutex);
        ret = hostIf_SetMsgHandler(stMsgData);
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[hostIf_MsgHandler()] Return value : %d\n", ret);
        g_mutex_unlock(request_handler_mutex);
        /*

                if(ret == OK)
                {
                    hostIf_Print_msgData(stMsgData);
        //            hostIf_Free_stMsgData(stMsgData);
                }
        */
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
}


//----------------------------------------------------------------------
//Initialization: This shall be initialized tr69 application to IARM bus.
//----------------------------------------------------------------------
static IARM_Result_t _Settr69HostIfMgr(void *arg)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);


    HOSTIF_MsgData_t *param = (HOSTIF_MsgData_t *) arg;
    if(param)
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] paramName :%s \n", __FUNCTION__, __FILE__, (char *)param->paramName);
        hostIf_SetReqHandler(arg);
    }


    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return IARM_RESULT_SUCCESS;
}

//----------------------------------------------------------------------
//Initialization: This shall be initialized tr69 application to IARM bus.
//----------------------------------------------------------------------
void hostIf_GetReqHandler(void *arg)
{
    int ret;
    HOSTIF_MsgData_t *stMsgData = (HOSTIF_MsgData_t *) arg;

//    hostIf_Init_Dummy_stMsgData (&stMsgData);

    if(stMsgData)
    {
        g_mutex_lock(request_handler_mutex);
        ret = hostIf_GetMsgHandler(stMsgData);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[hostIf_MsgHandler()] Return value : %d\n", ret);
        g_mutex_unlock(request_handler_mutex);

        if(ret == OK)
        {
//            hostIf_Print_msgData(stMsgData);
            //hostIf_Free_stMsgData(stMsgData);
        }
        else
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s()] Failed with returned Value : %d for [%s]\n",
                    __FUNCTION__, ret, stMsgData->paramName);
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
}

//----------------------------------------------------------------------
//Initialization: This shall be initialized tr69 application to IARM bus.
//----------------------------------------------------------------------
static IARM_Result_t _Gettr69HostIfMgr(void *arg)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);


    HOSTIF_MsgData_t *param = (HOSTIF_MsgData_t *) arg;
    if(param)
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] paramName :%s \n", __FUNCTION__, __FILE__, (char *)param->paramName);
    }
    hostIf_GetReqHandler(arg);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return IARM_RESULT_SUCCESS;
}


//----------------------------------------------------------------------
//_hostIf_EventHandler: This is to listen the IARM events and handles.
//----------------------------------------------------------------------
static void _hostIf_EventHandler(const char *owner, IARM_EventId_t eventId, void *data, size_t len)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    if (0 == strcmp(owner, IARM_TR69_CLIENT))
    {
        IARM_Bus_TR69Agent_EventData_t *tr69AgentData = (IARM_Bus_TR69Agent_EventData_t *)data;
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] EventId: %d value : %d \n", __FILE__, __FUNCTION__, (IARM_Bus_tr69Agent_EventId_t)eventId, tr69AgentData->value);

        switch (eventId)
        {
        case IARM_BUS_TR69Agent_ACS_CONN_EVENT:
            set_ACSStatus((tr69AgentData->value == ACS_CONNECTED) ? true: false);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] get_ACSStatus :%d \n", __FUNCTION__, __FILE__, get_ACSStatus());
            break;
        default:
            break;
        }
    }
    else if (0 == strcmp(owner, IARM_BUS_SYSMGR_NAME))
    {
        IARM_Bus_SYSMgr_EventData_t *eventData = (IARM_Bus_SYSMgr_EventData_t*)data;
        int gatewayConnState = eventData->data.systemStates.state;
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] Getting events from %s  for Gateway Connection State : %d\n", __FILE__, __FUNCTION__, __LINE__, IARM_BUS_SYSMGR_NAME, gatewayConnState);

        set_ACSStatus((gatewayConnState) ? true: false);

    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
}

#ifdef USE_XRDK_BT_PROFILE
void BTMgr_EventHandler(const char *owner, IARM_EventId_t eventId, void *data, size_t len)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    if (0 == strcmp(owner, IARM_BUS_BTMGR_NAME)) {
        if (NULL == data)
        {
            RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF, "%s : Input is invalid\n", __FUNCTION__);
        }
        else
        {
//            BTMGR_EventMessage_t eventType = eventId ;
            unsigned short numOfDevices = 0;

            switch (eventId) {
            case BTMGR_IARM_EVENT_DEVICE_DISCOVERY_COMPLETE :
                hostIf_DeviceInfoRdk_xBT::notifyAddDelEvent(numOfDevices, (char *)X_BT_DISCOVERED_DEV_OBJ);
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s:%d] Received \"BTMGR_IARM_EVENT_DEVICE_DISCOVERY_COMPLETE\" (%d).\n", __FUNCTION__, __LINE__, eventId);
                break;
            case BTMGR_IARM_EVENT_DEVICE_PAIRING_COMPLETE :
            case BTMGR_IARM_EVENT_DEVICE_UNPAIRING_COMPLETE :
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s:%d] Received \"%s\" (%d).\n", __FUNCTION__, __LINE__,
                        BTMGR_EVENT_DEVICE_PAIRING_COMPLETE?"BTMGR_EVENT_DEVICE_PAIRING_COMPLETE":"BTMGR_EVENT_DEVICE_UPAIRING_COMPLETE",
                        		eventId);
                hostIf_DeviceInfoRdk_xBT::notifyAddDelEvent(numOfDevices, (char *)X_BT_PAIRED_DEV_OBJ);
                break;
            case BTMGR_IARM_EVENT_DEVICE_CONNECTION_COMPLETE :
            case BTMGR_IARM_EVENT_DEVICE_DISCONNECT_COMPLETE :
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s:%d] Received \"%s\" (%d).\n", __FUNCTION__, __LINE__,
                        BTMGR_IARM_EVENT_DEVICE_CONNECTION_COMPLETE?"BTMGR_IARM_EVENT_DEVICE_CONNECTION_COMPLETE":"BTMGR_IARM_EVENT_DEVICE_DISCONNECT_COMPLETE",
                        		eventId);
                hostIf_DeviceInfoRdk_xBT::notifyAddDelEvent(numOfDevices, (char *)X_BT_CONNECTED_DEV_OBJ);
                break;
            default:
                RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,"[%s:%d] Invalid BTMGR_Event (%d) received.\n",	__FUNCTION__, __LINE__, eventId);
                break;
            }
        }
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
}
#endif
/** @} */
/** @} */
