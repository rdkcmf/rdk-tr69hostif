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
 * @defgroup TR69_HOSTIF_WIFI_AP_SECURITY TR-069 Object (Device.WiFi.AccessPoint.{i}.Security.) Public APIs
 * This module contains security related parameters that apply to a CPE acting as an Access Point [802.11-2007].
 * @ingroup TR69_HOSTIF_WIFI
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
     * @ingroup TR69_HOSTIF_WIFI_AP_SECURITY
     * @{
     */
   /**
    * @brief Get the comma-separated list of strings, indicate which security modes this AccessPoint instance
    * is capable of supporting. Each list item is an enumeration of.
    * - None
    * - WEP-64
    * - WEP-128
    * - WPA-Personal
    * - WPA2-Personal
    * - WPA-WPA2-Personal
    * - WPA-Enterprise
    * - WPA2-Enterprise
    * - WPA-WPA2-Enterprise 
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_Security_ModesSupported(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the security mode enabled for wifi device. The value must be a member of the list
    * reported by the ModesSupported parameter, indicates which security mode is enabled.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_Security_ModeEnabled(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Provide A WEP key expressed as a hexadecimal string.
    *
    * WEPKey is used only if ModeEnabled is set to WEP-64 or WEP-128.
    * A 5 byte WEPKey corresponds to security mode WEP-64 and a 13 byte WEPKey corresponds to security mode WEP-128.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_Security_WEPKey(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief A literal PreSharedKey (PSK) expressed as a hexadecimal string.
    *
    * PreSharedKey is only used if ModeEnabled is set to WPA-Personal or WPA2-Personal or WPA-WPA2-Personal.
    * If KeyPassphrase is written, then PreSharedKey is immediately generated.
    * The ACS SHOULD NOT set both the KeyPassphrase and the PreSharedKey directly (the result of doing this is undefined).
    * When read, this parameter returns an empty string, regardless of the actual value.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_Security_PreSharedKey(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Provide a passphrase from which the PreSharedKey is to be generated,
    * for WPA-Personal or WPA2-Personal or WPA-WPA2-Personal security modes.
    *
    * If KeyPassphrase is written, then PreSharedKey is immediately generated.
    * The ACS SHOULD NOT set both the KeyPassphrase and the PreSharedKey directly
    * (the result of doing this is undefined). The key is generated as specified by WPA,
    * which uses PBKDF2 from PKCS #5: Password-based Cryptography Specification Version 2.0 ([RFC2898]).
    *
    * When read, this parameter returns an empty string, regardless of the actual value.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_Security_KeyPassphrase(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the interval (expressed in seconds) in which the keys are re-generated.
    *
    * This is applicable to WPA, WPA2 and Mixed (WPA-WPA2) modes in Personal or Enterprise mode
    * (i.e. when ModeEnabled is set to a value other than None or WEP-64 or WEP-128.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_Security_RekeyingInterval(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the IP Address of the RADIUS server used for WLAN security.
    * RadiusServerIPAddr is only applicable when ModeEnabled is an Enterprise type
    * (i.e. WPA-Enterprise, WPA2-Enterprise or WPA-WPA2-Enterprise).
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_Security_RadiusServerIPAddr(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the port number of the RADIUS server used for WLAN security.
    * RadiusServerPort is only applicable when ModeEnabled is an Enterprise type
    * (i.e. WPA-Enterprise, WPA2-Enterprise or WPA-WPA2-Enterprise).
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_Security_RadiusServerPort(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief The secret used for handshaking with the RADIUS server [RFC2865].
    * When read, this parameter returns an empty string, regardless of the actual value.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_Security_RadiusSecret(HOSTIF_MsgData_t *stMsgData );

    /** @} */ // End of Doxygen Tag TR69_HOSTIF_WIFI_AP_SECURITY
};



#endif /* DEVICE_WIFI_ACCESSPOINT_SECURITY_H_ */


/** @} */
/** @} */
