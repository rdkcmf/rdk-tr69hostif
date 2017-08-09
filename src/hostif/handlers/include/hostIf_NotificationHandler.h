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


#ifndef _HOSTIF_NOTIFICATION_HANDLER_H_
#define _HOSTIF_NOTIFICATION_HANDLER_H_


#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"

typedef void (*parodusNotificationCallback)();
class NotificationHandler
{
    NotificationHandler();
    ~NotificationHandler();
    static NotificationHandler *pInstance;
    static parodusNotificationCallback notifyUpdateCallback;
    static GAsyncQueue *notificationQueue;
public:
    static NotificationHandler* getInstance();
    static GAsyncQueue* GetNotificationQueue();
    void addNotificationToQueue(const char *owner, IARM_Bus_tr69HostIfMgr_EventId_t eventId, void *data, size_t len);
    static void registerUpdateCallback(parodusNotificationCallback cb);
};

#endif //_HOSTIF_NOTIFICATION_HANDLER_H_


/** @} */
/** @} */
