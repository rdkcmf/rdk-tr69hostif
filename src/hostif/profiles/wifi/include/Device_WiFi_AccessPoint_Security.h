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


#ifndef DEVICE_WIFI_ACCESSPOINT_SECURITY_H_
#define DEVICE_WIFI_ACCESSPOINT_SECURITY_H_

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"


class hostIf_WiFi_AccessPoint_Security {
    static  GHashTable *ifHash;
    int dev_id;
    hostIf_WiFi_AccessPoint_Security(int dev_id);
    ~hostIf_WiFi_AccessPoint_Security() {};

public:
    static class hostIf_WiFi_AccessPoint_Security *getInstance(int dev_id);
    static GList* getAllInstances();
    static void closeInstance(hostIf_WiFi_AccessPoint_Security *);
    static void closeAllInstances();

    char	ModesSupported[20];
    char	ModeEnabled[20];
    char	WEPKey[64];
    char	PreSharedKey[64];
    char	KeyPassphrase[64];
    unsigned int	RekeyingInterval;
    char	RadiusServerIPAddr[45];
    unsigned int	RadiusServerPort;
    char	RadiusSecret[64];

    /**
     * @brief    Get the MAC Address of an Associated Device of a WiFi Interface.
     *
     * This function provides the MAC address of the WiFi interface of the device associated
     * with this WiFi interface.
     *
     * See @ref dev_wifi_if_assocdev_getter
     *
     */
    int get_hostIf_WiFi_AccessPoint_Security_ModesSupported(HOSTIF_MsgData_t *stMsgData );
    int get_hostIf_WiFi_AccessPoint_Security_ModeEnabled(HOSTIF_MsgData_t *stMsgData );
    int get_hostIf_WiFi_AccessPoint_Security_WEPKey(HOSTIF_MsgData_t *stMsgData );
    int get_hostIf_WiFi_AccessPoint_Security_PreSharedKey(HOSTIF_MsgData_t *stMsgData );
    int get_hostIf_WiFi_AccessPoint_Security_KeyPassphrase(HOSTIF_MsgData_t *stMsgData );
    int get_hostIf_WiFi_AccessPoint_Security_RekeyingInterval(HOSTIF_MsgData_t *stMsgData );
    int get_hostIf_WiFi_AccessPoint_Security_RadiusServerIPAddr(HOSTIF_MsgData_t *stMsgData );
    int get_hostIf_WiFi_AccessPoint_Security_RadiusServerPort(HOSTIF_MsgData_t *stMsgData );
    int get_hostIf_WiFi_AccessPoint_Security_RadiusSecret(HOSTIF_MsgData_t *stMsgData );
};



#endif /* DEVICE_WIFI_ACCESSPOINT_SECURITY_H_ */


/** @} */
/** @} */
