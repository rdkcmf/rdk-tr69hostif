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
 * @defgroup TR69_HOSTIF_WIFI_ENDPOINT_PROFILE_SECURITY TR-069 Object (Device.WiFi.EndPoint.{i}.Profile.{i}.Security.) Public APIs
 * This object contains security related parameters that apply to a WiFi End Point profile [802.11-2007].
 * @ingroup TR69_HOSTIF_WIFI
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

   /**
    * @ingroup TR69_HOSTIF_WIFI_ENDPOINT_PROFILE_SECURITY
    * @{
    */

   /**
    * @brief This function is used to get which security mode is enabled for wireless end point.
    * The value MUST be a member of the list reported by the Security.ModesSupported parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */ 
    int get_hostIf_WiFi_EndPoint_Profile_Security_ModeEnabled(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get a WEP key expressed as a hexadecimal string.
    *
    * WEPKey is used only if ModeEnabled is set to WEP-64 or WEP-128.
    * @n A 5 byte WEPKey corresponds to security mode WEP-64 and a 13 byte WEPKey corresponds to security mode WEP-128.
    * @n When read, this parameter returns an empty string, regardless of the actual value.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */ 
    int get_hostIf_WiFi_EndPoint_Profile_Security_WEPKey(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get a literal PreSharedKey (PSK) expressed as a hexadecimal string.
    *
    * PreSharedKey is only used if ModeEnabled is set to WPA-Personal or WPA2-Personal or WPA-WPA2-Personal.
    * @n If KeyPassphrase is written, then PreSharedKey is immediately generated. The ACS SHOULD NOT set both
    * the KeyPassphrase and the PreSharedKey directly (the result of doing this is undefined).
    * @n When read, this parameter returns an empty string, regardless of the actual value.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    */ 
    int get_hostIf_WiFi_EndPoint_Profile_Security_PreSharedKey(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get a passphrase from which the PreSharedKey is to be generated, for WPA-Personal or WPA2-Personal
    * or WPA-WPA2-Personal security modes.
    *
    * If KeyPassphrase is written, then PreSharedKey is immediately generated. The ACS SHOULD NOT set both the
    * KeyPassphrase and the PreSharedKey directly (the result of doing this is undefined). The key is generated
    * as specified by WPA, which uses PBKDF2 from PKCS #5: Password-based Cryptography Specification Version 2.0 [RFC2898].
    * @n When read, this parameter returns an empty string, regardless of the actual value.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */ 
    int get_hostIf_WiFi_EndPoint_Profile_Security_KeyPassphrase(HOSTIF_MsgData_t *stMsgData );

    /** @} */ // End of Doxygen tag TR69_HOSTIF_WIFI_ENDPOINT_PROFILE_SECURITY
};


#endif /* DEVICE_WIFI_ENDPOINT_PROFILE_SECURITY_H_ */


/** @} */
/** @} */
