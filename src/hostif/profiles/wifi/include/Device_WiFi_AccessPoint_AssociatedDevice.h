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


#ifndef DEVICE_WIFI_ACCESSPOINT_ASSOCIATEDDEVICE_H_
#define DEVICE_WIFI_ACCESSPOINT_ASSOCIATEDDEVICE_H_

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"


class hostIf_WiFi_AccessPoint_AssociatedDevice {

    static  GHashTable *ifHash;
    GMutex* m_mutex;
    int dev_id;
    hostIf_WiFi_AccessPoint_AssociatedDevice(int dev_id);
    ~hostIf_WiFi_AccessPoint_AssociatedDevice() {};

public:
    static class hostIf_WiFi_AccessPoint_AssociatedDevice *getInstance(int dev_id);
    static GList* getAllInstances();
    static void closeInstance(hostIf_WiFi_AccessPoint_AssociatedDevice *);
    static void closeAllInstances();

    char MACAddress[17];
    bool AuthenticationState;
    unsigned int LastDataDownlinkRate;
    unsigned int LastDataUplinkRate;
    int SignalStrength;
    unsigned int Retransmissions;
    bool Active;

    /**
     * @brief    Get the MAC Address of an Associated Device of a WiFi Interface.
     *
     * This function provides the MAC address of the WiFi interface of the device associated
     * with this WiFi interface.
     *
     * See @ref dev_wifi_if_assocdev_getter
     *
     */
    int get_hostIf_WiFi_AccessPoint_AssociatedDevice_MACAddress(HOSTIF_MsgData_t *stMsgData );
    int get_hostIf_WiFi_AccessPoint_AssociatedDevice_AuthenticationState(HOSTIF_MsgData_t *stMsgData );
    int get_hostIf_WiFi_AccessPoint_AssociatedDevice_LastDataDownlinkRate(HOSTIF_MsgData_t *stMsgData );
    int get_hostIf_WiFi_AccessPoint_AssociatedDevice_LastDataUplinkRate(HOSTIF_MsgData_t *stMsgData );
    int get_hostIf_WiFi_AccessPoint_AssociatedDevice_SignalStrength(HOSTIF_MsgData_t *stMsgData );
    int get_hostIf_WiFi_AccessPoint_AssociatedDevice_Retransmissions(HOSTIF_MsgData_t *stMsgData );
    int get_hostIf_WiFi_AccessPoint_AssociatedDevice_Active(HOSTIF_MsgData_t *stMsgData );
};



#endif /* DEVICE_WIFI_ACCESSPOINT_ASSOCIATEDDEVICE_H_ */


/** @} */
/** @} */
