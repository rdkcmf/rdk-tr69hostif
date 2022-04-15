/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2022 RDK Management
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
 * @file X_rdk_profile.h
 * @brief The header file provides components Xrdk SDCard information APIs.
 */

/**
 * @defgroup DEVICE_X_RDK Profile Object (Device.X_RDK_<>)
 * @par
 * @ingroup DEVICE.X_rdk_profile
 *
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/

#ifndef X_RDK_PROFILE_H_
#define X_RDK_PROFILE_H_


#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"
#include "XrdkCentralComBSStore.h"


#define X_RDK_PREFIX_STR                            "Device.X_RDK_"
#define X_RDK_WebPA_SERVER_URL_STPRING              "Device.X_RDK_WebPA_Server.URL"
#define X_RDK_WebPA_TokenServer_URL_STRING          "Device.X_RDK_WebPA_TokenServer.URL"
#define X_RDK_WebPA_DNSText_URL_STRING              "Device.X_RDK_WebPA_DNSText.URL"


/**
 * @brief This class provides the TR-069 components Bluetooth devices information.
 * @ingroup DEVICE.X_RDK_PROFILE_CLASSES
 */
class X_rdk_profile
{
private:
    X_rdk_profile();
    ~X_rdk_profile() {};

    static std::mutex m;
    static X_rdk_profile* m_instance;
    static XBSStore *m_bsStore;

    /* Discovered Device Methods */
    int get_WebPA_Server_URL(HOSTIF_MsgData_t *);
    int get_WebPA_TokenServer_URL(HOSTIF_MsgData_t *);
    int get_WebPA_DNSText_URL(HOSTIF_MsgData_t *);
    int get_WebConfig_URL(HOSTIF_MsgData_t *);
    int get_WebConfig_SupplementaryServiceUrls_Telemetry(HOSTIF_MsgData_t *);
    int getWebcfgData(HOSTIF_MsgData_t *);

    int setWebcfgForceSync(HOSTIF_MsgData_t *);
    int set_WebConfig_URL(HOSTIF_MsgData_t *);
    int set_WebPA_DNSText_URL(HOSTIF_MsgData_t *);
    int set_WebConfig_SupplementaryServiceUrls_Telemetry(HOSTIF_MsgData_t *);
public:
    static X_rdk_profile *getInstance();
    static void closeInstance();
    int handleGetMsg(HOSTIF_MsgData_t *);
    int handleSetMsg(HOSTIF_MsgData_t *);
};

#endif /* X_rdk_profile_H_ */


/** @} */
/** @} */
