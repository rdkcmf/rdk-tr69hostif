/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2017 RDK Management
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
#include "hostIf_NotificationHandler.h"
#include "libpd.h"
#include "hostIf_tr69ReqHandler.h"

#define NOTIFY_PARAM_VALUE_CHANGE  ""
NotificationHandler* NotificationHandler::pInstance = NULL;
parodusNotificationCallback NotificationHandler::notifyUpdateCallback = NULL;
GAsyncQueue* NotificationHandler::notificationQueue = NULL;

static void converttoWalType(HostIf_ParamType_t paramType,WAL_DATA_TYPE* pwalType);
static int isValidParameter(char* paramName);

//Constructor
NotificationHandler::NotificationHandler()
{
    // Initialize notificationQueue
    notificationQueue = g_async_queue_new();

}
//Distructor
NotificationHandler::~NotificationHandler()
{
    g_async_queue_unref(notificationQueue);
}
NotificationHandler* NotificationHandler::getInstance()
{

    if(!pInstance)
        pInstance = new NotificationHandler();
    return pInstance;
}

GAsyncQueue* NotificationHandler::GetNotificationQueue()
{
    return notificationQueue;
}

void NotificationHandler::addNotificationToQueue(const char *owner, IARM_Bus_tr69HostIfMgr_EventId_t eventId, void *data, size_t len)
{
    IARM_Bus_tr69HostIfMgr_EventData_t *tr69EventData = (IARM_Bus_tr69HostIfMgr_EventData_t *)data;
    ParamNotify *paramNotify = NULL;
    int isNotificationEnabled = 0;

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Inside addNotificationToQueue \n");

    if (NULL != tr69EventData && (0 == strncmp(owner, IARM_BUS_TR69HOSTIFMGR_NAME,strlen(IARM_BUS_TR69HOSTIFMGR_NAME))))
    {
        switch (eventId)
        {
        case IARM_BUS_TR69HOSTIFMGR_EVENT_ADD:
        case IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE:
        case IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED:
        {
            if(isValidParameter(tr69EventData->paramName))
            {
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Inside  IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED\n");
                paramNotify = (ParamNotify *) malloc(sizeof(ParamNotify));
                memset(paramNotify,0,sizeof(ParamNotify));
                if(tr69EventData->paramName)
                {
                    paramNotify->paramName = tr69EventData->paramName;
                    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Notify param Name = %s \n",paramNotify->paramName);
                }
                if(tr69EventData->paramValue)
                {
                    paramNotify->newValue = tr69EventData->paramValue;
                    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"INotify param Value = %s \n",paramNotify->newValue);
                }
                converttoWalType(tr69EventData->paramtype,&(paramNotify->type));
            }

        }
        break;
        default:
            break;
        }

        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Before Notify Callback\n");
        if((notifyUpdateCallback != NULL) && (eventId == IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED) && isValidParameter(tr69EventData->paramName))
        {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Valid Callback and Valid Event\n");
            NotifyData *notifyDataPtr = (NotifyData *) malloc(sizeof(NotifyData) * 1);
            if(NULL == notifyDataPtr)
            {
                if(paramNotify) free(paramNotify);
            }
            else
            {
                notifyDataPtr->type = PARAM_VALUE_CHANGE_NOTIFY;
                Notify_Data *notify_data = (Notify_Data *) malloc(sizeof(Notify_Data) * 1);
                if(NULL != notify_data)
                {
                    notify_data->notify = paramNotify;
                    notifyDataPtr->data = notify_data;
                    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Notification forwarded for Parameter Name (%s) with Data type (%d).\n",
                            paramNotify->paramName, paramNotify->type); // Not printing value due to security reasons

                    // Add the notification to queue and call Webpa Callback
                    if(NULL != notificationQueue)
                    {
                        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Pushing to Queue \n");
                        g_async_queue_push(notificationQueue,notifyDataPtr);
                        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Pushed ... Calling Notify Callback \n");
                        (*notifyUpdateCallback)();
                    }
                    else
                    {
                        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Queue is not initialized \n");
                    }
                }
                else
                {
                    if(paramNotify) free(paramNotify);
                }
            }
        }
    }
}
void NotificationHandler::registerUpdateCallback(parodusNotificationCallback cb)
{
    notifyUpdateCallback = cb;
}

static void converttoWalType(HostIf_ParamType_t paramType,WAL_DATA_TYPE* pwalType)
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Inside converttoWalType \n");
    switch(paramType)
    {
    case hostIf_StringType:
        *pwalType = WAL_STRING;
        break;
    case hostIf_UnsignedIntType:
        *pwalType = WAL_UINT;
        break;
    case hostIf_IntegerType:
        *pwalType = WAL_INT;
        break;
    case hostIf_BooleanType:
        *pwalType = WAL_BOOLEAN;
        break;
    case hostIf_UnsignedLongType:
        *pwalType = WAL_ULONG;
        break;
    case hostIf_DateTimeType:
        *pwalType = WAL_DATETIME;
        break;
    default:
        *pwalType = WAL_STRING;
        break;
    }
}

static int isValidParameter(char* paramName)
{
    int isValid = 0;
    if(NULL != paramName)
    {
        // Make sure that the parameter is not ending with .
        if(strcmp(paramName+strlen(paramName)-1,"."))
        {
            isValid =1;
        }
    }
    return isValid;
}
/** @} */
/** @} */
