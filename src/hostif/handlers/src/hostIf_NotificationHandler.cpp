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
#include "webpa_notification.h"
#include "Device_DeviceInfo.h"
#include "cJSON.h"

#define NOTIFY_PARAM_VALUE_CHANGE  ""
NotificationHandler* NotificationHandler::pInstance = NULL;
parodusNotificationCallback NotificationHandler::notifyUpdateCallback = NULL;
GAsyncQueue* NotificationHandler::notificationQueue = NULL;

static void addToJsonObject (cJSON* json, const char* key, const char* value, const WAL_DATA_TYPE type);
static void converttoWalType(HostIf_ParamType_t paramType,WAL_DATA_TYPE* pwalType);

typedef struct _notify_params
{
    char * delay;
    char * time;
    char * status;
    char * download_status;
    char * system_ready_time;
} notify_params_t;


//Constructor
NotificationHandler::NotificationHandler()
{
    // Initialize notificationQueue
    notificationQueue = g_async_queue_new();

}
//Destructor
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

void NotificationHandler::registerUpdateCallback(parodusNotificationCallback cb)
{
    notifyUpdateCallback = cb;
}

void NotificationHandler::pushNotification(const char* destination, const char* payload)
{
    NotifyData* notifyDataPtr = NULL;
    Notify_Data* notify_data = NULL;
    ParamNotify* paramNotify = NULL;

    if (NULL == notificationQueue || NULL == notifyUpdateCallback)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: notificationQueue = %p, notifyUpdateCallback %p \n",
                 __FUNCTION__, notificationQueue, notifyUpdateCallback);
    }
    else if (NULL == destination || NULL == payload)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Input error: destination %s payload %s \n", __FUNCTION__, destination, payload);
    }
    else if (NULL == (notifyDataPtr = (NotifyData *) calloc(1, sizeof(NotifyData))))
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Could not allocate notifyDataPtr\n", __FUNCTION__);
    }
    else if (NULL == (notify_data = (Notify_Data*) calloc(1, sizeof(Notify_Data))))
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Could not allocate notify_data\n", __FUNCTION__);
        free (notifyDataPtr);
    }
    else if (NULL == (paramNotify = (ParamNotify *) calloc(1, sizeof(ParamNotify))))
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Could not allocate paramNotify\n", __FUNCTION__);
        free (notify_data);
        free (notifyDataPtr);
    }
    else
    {
        paramNotify->notifyDest = destination ? strdup (destination) : NULL;
        paramNotify->notifyPayload = (char*) payload;

        notify_data->notify = paramNotify;

        notifyDataPtr->type = PARAM_VALUE_CHANGE_NOTIFY;
        notifyDataPtr->data = notify_data;

        // Add the notification to queue and call Webpa Callback
        g_async_queue_push(notificationQueue, notifyDataPtr);
        (*notifyUpdateCallback)();
    }
}

void NotificationHandler::pushValueChangeNotification(IARM_Bus_tr69HostIfMgr_EventData_t& event)
{
    cJSON* notifyPayload = cJSON_CreateObject ();
    cJSON_AddStringToObject(notifyPayload, "device_id", getNotifySource ()); // Device ID and Notification source are same

    WAL_DATA_TYPE pwalType;
    converttoWalType(event.paramtype, &pwalType);

    cJSON_AddNumberToObject(notifyPayload, "datatype", pwalType);
    cJSON_AddStringToObject(notifyPayload, "paramName", event.paramName);
    cJSON_AddStringToObject(notifyPayload, "notificationType", "VALUE_CHANGE_NOTIFICATION");
    addToJsonObject (notifyPayload, "paramValue", event.paramValue, pwalType);

    char* payload = cJSON_PrintUnformatted (notifyPayload);
    cJSON_Delete (notifyPayload);
    RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "%s: Generated payload: %s\n", __FUNCTION__, payload);

    NotificationHandler::getInstance()->pushNotification("event:VALUE_CHANGE_NOTIFICATION", payload);
}

void NotificationHandler::pushKeyValueNotification(const char* destination, const char* key, const char* value)
{
    cJSON* notifyPayload = cJSON_CreateObject ();
    cJSON_AddStringToObject(notifyPayload, "device_id", getNotifySource ()); // Device ID and Notification source are same

    cJSON_AddStringToObject(notifyPayload, key, value);

    char* payload = cJSON_PrintUnformatted (notifyPayload);
    cJSON_Delete (notifyPayload);
    RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "%s: Generated payload: %s\n", __FUNCTION__, payload);

    NotificationHandler::getInstance()->pushNotification(destination, payload);
}


void addToJsonObject (cJSON* json, const char* key, const char* value, const WAL_DATA_TYPE type)
{
    switch (type)
    {
    case WAL_STRING:
    {
        cJSON_AddStringToObject(json, key, value);
        break;
    }
    case WAL_INT:
    {
        const int* paramNewValue = (const int*) ((value));
        cJSON_AddNumberToObject(json, key, *paramNewValue);
        break;
    }
    case WAL_UINT:
    {
        const unsigned int* paramNewValue = (const unsigned int*) ((value));
        cJSON_AddNumberToObject(json, key, *paramNewValue);
        break;
    }
    case WAL_BOOLEAN:
    {
        const bool* paramNewValue = (const bool*) ((value));
        cJSON_AddBoolToObject(json, key, *paramNewValue);
        break;
    }
    case WAL_ULONG:
    {
        const unsigned long *paramNewValue = (const unsigned long *) ((value));
        cJSON_AddNumberToObject(json, key, *paramNewValue);
        break;
    }
    default:
    {
        cJSON_AddStringToObject(json, key, value);
        break;
    }
    }
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

void NotificationHandler::push_device_mgmt_notifications(char* delay, char* time, char* download_status, char* status, char* system_ready_time)
{
    int sendStatus = -1;
    cJSON *notifyPayload = NULL;
    unsigned long bootTime = 0;

    LOG_ENTRY_EXIT;

    notifyPayload = cJSON_CreateObject();

    if(notifyPayload != NULL)
    {
        cJSON_AddStringToObject(notifyPayload,"device_id", getNotifySource ());
        if(status !=NULL)
        {
            cJSON_AddStringToObject(notifyPayload,"status", status);
        }
        if(time !=NULL)
        {
            cJSON_AddStringToObject(notifyPayload,"start-time", time);
        }

        if(download_status !=NULL)
        {
            cJSON_AddStringToObject(notifyPayload,"download-status", (strcmp(download_status, "true") == 0)?"success":"failure");
        }

        if ((status !=NULL) && (strcmp(status, "reboot-pending") == 0))
        {
            HOSTIF_MsgData_t stBootUpData = {0};
            hostIf_DeviceInfo::getInstance(0)->get_X_RDKCENTRAL_COM_BootTime(&stBootUpData);
            bootTime = get_ulong(stBootUpData.paramValue);
            cJSON_AddNumberToObject(notifyPayload,"boot-time", bootTime);

            char* lastRebootReason = NULL;
            HOSTIF_MsgData_t stMgsData = {0};
            hostIf_DeviceInfo::getInstance(0)->get_X_RDKCENTRAL_COM_LastRebootReason(&stMgsData);
            lastRebootReason = stMgsData.paramValue;
            RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "LastRebootReason is %s\n", lastRebootReason);

            if(lastRebootReason !=NULL)
            {
                cJSON_AddStringToObject(notifyPayload,"reboot-reason", lastRebootReason);
            }

            if(delay !=NULL)
            {
                cJSON_AddNumberToObject(notifyPayload,"delay", atoi(delay));
            }
        }

        if ((status !=NULL) && (strcmp(status, "fully-manageable") == 0) && (system_ready_time != NULL))
        {
            HOSTIF_MsgData_t stBootUpData = {0};
            hostIf_DeviceInfo::getInstance(0)->get_X_RDKCENTRAL_COM_BootTime(&stBootUpData);
            bootTime = get_ulong(stBootUpData.paramValue);
            cJSON_AddNumberToObject(notifyPayload,"boot-time", bootTime);
            cJSON_AddNumberToObject(notifyPayload,"system-ready-time", atoi(system_ready_time));
        }

        char* payload = cJSON_PrintUnformatted (notifyPayload);
        cJSON_Delete(notifyPayload);

        const char *destination = "event:device-status";
        NotificationHandler::getInstance()->pushNotification(destination, payload);
    }
}

/** @} */
/** @} */
