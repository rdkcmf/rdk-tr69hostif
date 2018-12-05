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


#ifndef _HOSTIF_UPDATE_HANDLER_H_
#define _HOSTIF_UPDATE_HANDLER_H_

#include <pthread.h>
#include "hostIf_tr69ReqHandler.h"
#include "stdio.h"
#include "hostIf_main.h"
//#include <unistd.h>
#include "libIBus.h"

typedef void (*updateCallback)(IARM_Bus_tr69HostIfMgr_EventId_t, const char* paramName, const char* paramValue, HostIf_ParamType_t paramtype);

class updateHandler {

    static bool stopped;
    static GThread *thread;
public:
    static int Init();
    static void stop();
    static void reset();
    static gpointer run(gpointer);
    static void notifyCallback(IARM_Bus_tr69HostIfMgr_EventId_t, const char* paramName, const char* paramVal, HostIf_ParamType_t paramtype);
};

void sendAddRemoveEvents (updateCallback callbackFn, int newValue, int& savedValue, char* objectPath);

#endif //_HOSTIF_UPDATE_HANDLER_H_


/** @} */
/** @} */
