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
 * @defgroup TR69_HOSTIF_WIFI_AP_WPS TR-069 Object (Device.WiFi.AccessPoint.{i}.WPS.) Public APIs
 * This module provides interface functions related to Wi-Fi Protected Setup [WPSv1.0] for this access point.
 * @ingroup TR69_HOSTIF_WIFI
 */


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef DEVICE_WIFI_ACCESSPOINT_WPS_H_
#define DEVICE_WIFI_ACCESSPOINT_WPS_H_

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"


class hostIf_WiFi_AccessPoint_WPS {

    static  GHashTable *ifHash;
    static GMutex* m_mutex;
    int dev_id;
    hostIf_WiFi_AccessPoint_WPS(int dev_id);
    ~hostIf_WiFi_AccessPoint_WPS() {};

public:
    static class hostIf_WiFi_AccessPoint_WPS *getInstance(int dev_id);
    static GList* getAllInstances();
    static void closeInstance(hostIf_WiFi_AccessPoint_WPS *);
    static void closeAllInstances();

    bool Enable;
    char	ConfigMethodsSupported[100];
    char	ConfigMethodsEnabled[64];


   /**
     * @ingroup TR69_HOSTIF_WIFI_AP_WPS
     * @{
     */
   /**
    * @brief Enables or disables WPS functionality for this access point.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_WPS_Enable(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief This function provides the comma-separated list of strings, which indicates WPS configuration
    * methods supported by the device.
    *
    * Each list item is an enumeration of:
    * - USBFlashDrive
    * - Ethernet
    * - ExternalNFCToken
    * - IntegratedNFCToken
    * - NFCInterface
    * - PushButton
    * - PIN 
    * This parameter corresponds directly to the "Config Methods" attribute of the WPS specification [WPSv1.0].
    * The PushButton and PIN methods MUST be supported.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_WPS_ConfigMethodsSupported(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief This function provides the comma-separated list of strings.
    * Each list item MUST be a member of the list reported by the ConfigMethodsSupported parameter.
    * Indicates WPS configuration methods enabled on the device.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_WPS_ConfigMethodsEnabled(HOSTIF_MsgData_t *stMsgData );

    /** @} */ // End of doxygen tag TR69_HOSTIF_WIFI_AP_WPS
};



#endif /* DEVICE_WIFI_ACCESSPOINT_WPS_H_ */


/** @} */
/** @} */
