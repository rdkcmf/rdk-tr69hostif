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


#ifndef DEVICE_WIFI_ENDPOINT_WPS_H_
#define DEVICE_WIFI_ENDPOINT_WPS_H_

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"
#include "Device_WiFi_EndPoint.h"

class hostIf_WiFi_EndPoint_WPS {

    static  GHashTable *ifHash;
//    GMutex* m_mutex = NULL;
    int dev_id;
    hostIf_WiFi_EndPoint_WPS(int dev_id);
    ~hostIf_WiFi_EndPoint_WPS() {};

public:
    static class hostIf_WiFi_EndPoint_WPS *getInstance(int dev_id);
    static GList* getAllInstances();
    static void closeInstance(hostIf_WiFi_EndPoint_WPS *);
    static void closeAllInstances();

    bool	Enable;
    char	ConfigMethodsSupported[64];
    char ConfigMethodsEnabled[64];


    int get_Device_WiFi_EndPoint_WPS_Enable(HOSTIF_MsgData_t *stMsgData );
    int get_Device_WiFi_EndPoint_WPS_ConfigMethodsSupported(HOSTIF_MsgData_t *stMsgData );
    int get_Device_WiFi_EndPoint_WPS_ConfigMethodsEnabled(HOSTIF_MsgData_t *stMsgData );
};


#endif /* DEVICE_WIFI_ENDPOINT_WPS_H_ */


/** @} */
/** @} */
