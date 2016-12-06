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
 * @file hostIf_TimeClient_ReqHandler.cpp
 * @brief The header file provides HostIf time client request handler information APIs.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#include "hostIf_main.h"
#include "hostIf_TimeClient_ReqHandler.h"

TimeClientReqHandler* TimeClientReqHandler::pInstance = NULL;
updateCallback TimeClientReqHandler::mUpdateCallback = NULL;
msgHandler* TimeClientReqHandler::getInstance()
{

    if(!pInstance)
        pInstance = new TimeClientReqHandler();
    return pInstance;
}

/**
 * @brief This function is use to initialize. Currently not implemented.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if initialization is successfully .
 * @retval false if initialization is not successful.
 * @ingroup TR-069HOSTIF_TIMECLIENT_REQHANDLER_CLASSES
 */
bool TimeClientReqHandler::init()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

/**
 * @brief This function is used to close all the instances of time client.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if it is successfully close all the instances.
 * @retval false if not able to close all the instances.
 * @ingroup TR-069HOSTIF_TIMECLIENT_REQHANDLER_CLASSES
 */
bool TimeClientReqHandler::unInit()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    hostIf_Time::closeAllInstances();

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

/**
 * @brief This function is use to handle the set message request of time client interface
 * and set the attributes of the time client component.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully.
 * @retval ERR_INTERNAL_ERROR if not able to set the data to the device.
 * @ingroup TR-069HOSTIF_TIMECLIENT_REQHANDLER_CLASSES
 */
int TimeClientReqHandler::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *pSetting;
    int instanceNumber;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    if(strncasecmp(stMsgData->paramName,"Device.Time",strlen("Device.Time"))==0)
    {
        stMsgData->instanceNum = 0;
        hostIf_Time::getLock();

        hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);

        if(!pIface)
        {
            hostIf_Time::releaseLock();
            return NOK;
        }

        if (strcasecmp(stMsgData->paramName,"Device.Time.Enable") == 0)
        {
            ret = pIface->set_Device_Time_Enable(stMsgData);
        }
    }
    hostIf_Time::releaseLock();
    return ret;
}

/**
 * @brief This function use to handle the get message request of time client interface
 * and get the attributes of the time client component "LocalTimeZone" and "CurrentLocalTime".
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully.
 * @retval ERR_INTERNAL_ERROR if not able to get data from the device.
 * @ingroup TR-069HOSTIF_TIMECLIENT_REQHANDLER_CLASSES
 */
int TimeClientReqHandler::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *pSetting;
    int instanceNumber;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    hostIf_Time::getLock();

    if(strncasecmp(stMsgData->paramName,"Device.Time",strlen("Device.Time"))==0)
    {
        stMsgData->instanceNum = 0;
        hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);

        if(!pIface)
        {
            hostIf_Time::releaseLock();
            return NOK;
        }

        if (strcasecmp(stMsgData->paramName,"Device.Time.LocalTimeZone") == 0)
        {
            ret = pIface->get_Device_Time_LocalTimeZone(stMsgData);
        }
        if (strcasecmp(stMsgData->paramName,"Device.Time.CurrentLocalTime") == 0)
        {
            ret = pIface->get_Device_Time_CurrentLocalTime(stMsgData);
        }
    }
    hostIf_Time::releaseLock();
    return ret;
}

void TimeClientReqHandler::registerUpdateCallback(updateCallback cb)
{
    mUpdateCallback = cb;
}

void TimeClientReqHandler::checkForUpdates()
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    GList *elem;
    int index = 1;
    char tmp_buff[TR69HOSTIFMGR_MAX_PARAM_LEN];
    hostIf_Time::getLock();
    GList *devList = hostIf_Time::getAllInstances();

    for(elem = devList; elem; elem = elem->next,index++)
    {

        hostIf_Time *pIface = hostIf_Time::getInstance((int)elem->data);

        if(pIface)
        {
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_Time_LocalTimeZone(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.Time.Interface.%d.%s",index,"Enable");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
        }
    }

    g_list_free(devList);

    hostIf_Time::releaseLock();
}


/** @} */
/** @} */
