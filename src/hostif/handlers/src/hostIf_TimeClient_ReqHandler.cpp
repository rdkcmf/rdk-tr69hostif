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
//#define HAVE_VALUE_CHANGE_EVENT
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

        if (stMsgData->bsUpdate != HOSTIF_NONE)
        {
           if ( (stMsgData->bsUpdate == HOSTIF_SRC_RFC && stMsgData->requestor == HOSTIF_SRC_RFC) ||
             (stMsgData->bsUpdate == HOSTIF_SRC_ALL && (stMsgData->requestor == HOSTIF_SRC_RFC || stMsgData->requestor == HOSTIF_SRC_WEBPA)) )
         {
            ret = pIface->set_xRDKCentralComBootstrap(stMsgData);
         }
         else
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] Not setting the bootstrap param:%s [bsUpdate=%d, requestor=%d]\n", __FUNCTION__, stMsgData->paramName, stMsgData->bsUpdate, stMsgData->requestor);

        }
        else if (strcasecmp(stMsgData->paramName,"Device.Time.Enable") == 0)
        {
            ret = pIface->set_Device_Time_Enable(stMsgData);
        }
        else
        {
           RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] parameter : \'%s\' Not handled \n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
           stMsgData->faultCode = fcInvalidParameterName;
           ret = NOK;
        }
        hostIf_Time::releaseLock();
    }
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

        if (stMsgData->bsUpdate != HOSTIF_NONE)
        {
           ret = pIface->get_xRDKCentralComBootstrap(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.Time.LocalTimeZone") == 0)
        {
            ret = pIface->get_Device_Time_LocalTimeZone(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.Time.CurrentLocalTime") == 0)
        {
            ret = pIface->get_Device_Time_CurrentLocalTime(stMsgData);
        }
        else
        {
           RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] parameter : \'%s\' Not handled \n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
           stMsgData->faultCode = fcInvalidParameterName;
           ret = NOK;
        }
        hostIf_Time::releaseLock();
    }
    else
    {
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] parameter : \'%s\' Not handled \n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
       stMsgData->faultCode = fcInvalidParameterName;
       ret = NOK;
    }
    return ret;
}

void TimeClientReqHandler::registerUpdateCallback(updateCallback cb)
{
    mUpdateCallback = cb;
}

int TimeClientReqHandler::handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    int instanceNumber =0;

    hostIf_Time::getLock();
    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    stMsgData->instanceNum = instanceNumber;
    if(!pIface)
    {
        hostIf_Time::releaseLock();
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
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] Not able to allocate Notify pointer %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
        }
    }
    else
    {
        ret = NOK;
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d]  Not able to get notifyhash  %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    }
    hostIf_Time::releaseLock();
    return ret;
}

int TimeClientReqHandler::handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    int instanceNumber = 0;

    hostIf_Time::getLock();
    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    stMsgData->instanceNum = instanceNumber;
    if(!pIface)
    {
        hostIf_Time::releaseLock();
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
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] Not able to allocate Notify pointer %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
        }
    }
    else
    {
        ret = NOK;
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] EthernetClientReqHandler Not able to get notifyhash  %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    }
    hostIf_Time::releaseLock();
    return ret;
}
void TimeClientReqHandler::checkForUpdates()
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    int instanceNumber;
    GHashTable* notifyhash = NULL;

    hostIf_Time::getLock();

#ifdef HAVE_VALUE_CHANGE_EVENT
    instanceNumber = 0;
    hostIf_Time *pIface = hostIf_Time::getInstance(instanceNumber);
    if(NULL != pIface)
    {
        notifyhash = pIface->getNotifyHash();
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Unable to get Time Instance\n", __FUNCTION__, __FILE__);
    }

// Iterate through Ghash Table
    if(NULL != notifyhash)
    {
        GHashTableIter notifyHashIterator;
        gpointer paramName;
        gpointer notifyEnable;
        const char *pSetting;

        g_hash_table_iter_init (&notifyHashIterator, notifyhash);
        while (g_hash_table_iter_next (&notifyHashIterator, &paramName, &notifyEnable))
        {
            int* isNotifyEnabled = (int *)notifyEnable;
            instanceNumber = 0;
            if(matchComponent((const char*)paramName,"Device.Time.Interface",&pSetting,instanceNumber))
            {
                if(!instanceNumber)
                {   // Time settings not found in Notify Hash Table
                    hostIf_Time::releaseLock();
                    continue;
                }
                hostIf_Time *pTimeIface = hostIf_Time::getInstance(instanceNumber);
                if(pTimeIface)
                {
                    if (strcasecmp(pSetting,"Enable") == 0)
                    {
                        pTimeIface->get_Device_Time_LocalTimeZone(&msgData,&bChanged);
                        if(mUpdateCallback && (*isNotifyEnabled == 1))
                        {
                            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                        }

                    }
                }
            }
        }

    }
#endif /*HAVE_VALUE_CHANGE_EVENT */

    hostIf_Time::releaseLock();
}


/** @} */
/** @} */
