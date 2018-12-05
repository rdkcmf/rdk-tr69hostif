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


#ifndef DEVICE_WIFI_ENDPOINT_PROFILE_SECURITY_H_
#define DEVICE_WIFI_ENDPOINT_PROFILE_SECURITY_H_

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"

class hostIf_WiFi_EndPoint_Profile_Security {

    static  GHashTable *ifHash;
    static GMutex* m_mutex;
    int dev_id;
    hostIf_WiFi_EndPoint_Profile_Security(int dev_id);
    ~hostIf_WiFi_EndPoint_Profile_Security() {};

public:
    static class hostIf_WiFi_EndPoint_Profile_Security *getInstance(int dev_id);
    static GList* getAllInstances();
    static void closeInstance(hostIf_WiFi_EndPoint_Profile_Security *);
    static void closeAllInstances();

    char	ModeEnabled[64];
    char	WEPKey[64];
    char PreSharedKey[64];
    char KeyPassphrase[64];

    int get_hostIf_WiFi_EndPoint_Profile_Security_ModeEnabled(HOSTIF_MsgData_t *stMsgData );
    int get_hostIf_WiFi_EndPoint_Profile_Security_WEPKey(HOSTIF_MsgData_t *stMsgData );
    int get_hostIf_WiFi_EndPoint_Profile_Security_PreSharedKey(HOSTIF_MsgData_t *stMsgData );
    int get_hostIf_WiFi_EndPoint_Profile_Security_KeyPassphrase(HOSTIF_MsgData_t *stMsgData );
};


#endif /* DEVICE_WIFI_ENDPOINT_PROFILE_SECURITY_H_ */


/** @} */
/** @} */
