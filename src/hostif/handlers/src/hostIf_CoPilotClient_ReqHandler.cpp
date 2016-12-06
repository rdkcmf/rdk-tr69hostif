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
#include "hostIf_utils.h"
#ifdef USE_XRESRC
#include "hostIf_CoPilotClient_ReqHandler.h"
#include "Device_XComcast_Xcalibur_Client_CoPilot.h"

CoPilotClientReqHandler* CoPilotClientReqHandler::pInstance = NULL;
GMutex* CoPilotClientReqHandler::m_mutex = NULL;
msgHandler* CoPilotClientReqHandler::getInstance()
{
    if(!pInstance)
        pInstance = new CoPilotClientReqHandler();
    return pInstance;
}
bool CoPilotClientReqHandler::init()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}
bool CoPilotClientReqHandler::unInit()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}
void CoPilotClientReqHandler::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}
void CoPilotClientReqHandler::releaseLock()
{
    g_mutex_unlock(m_mutex);
}
int CoPilotClientReqHandler::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    stMsgData->instanceNum = 0;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    getLock();
    if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.CoPilot.rssServerArgs") == 0)
    {
        ret = set_Device_X_COMCAST_COM_Xcalibur_Client_CoPilot_rssServerArgs(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.CoPilot.rssServerMode") == 0)
    {
        ret = set_Device_X_COMCAST_COM_Xcalibur_Client_CoPilot_rssServerMode(stMsgData);
    }

    releaseLock();
    return ret;
}
int CoPilotClientReqHandler::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *pSetting = NULL;
    int instanceNumber = 0;
    getLock();
    stMsgData->instanceNum = 0;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.CoPilot.rssServerArgs") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_CoPilot_rssServerArgs(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.CoPilot.rssServerMode") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_CoPilot_rssServerMode(stMsgData);
    }
    releaseLock();
    return ret;
}
#endif /*USE_XRESRC*/
/** @} */
/** @} */
