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
  * @defgroup TR69_HOSTIF_WIFI_ENDPOINT_WPS TR-069 Object (Device.WiFi.EndPoint.{i}.WPS.) Public APIs
  * This object contains parameters related to Wi-Fi Protected Setup [WPSv1.0] for this end point.
  * @ingroup TR69_HOSTIF_WIFI
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

   /**
    * @ingroup TR69_HOSTIF_WIFI_ENDPOINT_WPS
    * @{
    */
   /**
    * @brief Enables or disables WPS functionality for this end point.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_EndPoint_WPS_Enable(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the comma-separated list of strings, containing the WPS configuration methods
    * supported by the device.
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
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_EndPoint_WPS_ConfigMethodsSupported(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Check the WPS configuration methods enabled on the device.
    *
    * Comma-separated list of strings. Each list item MUST be a member of the list reported
    * by the ConfigMethodsSupported parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_EndPoint_WPS_ConfigMethodsEnabled(HOSTIF_MsgData_t *stMsgData );

   /** @} */ //End of Doxygen Tag TR69_HOSTIF_WIFI_ENDPOINT_WPS
};


#endif /* DEVICE_WIFI_ENDPOINT_WPS_H_ */


/** @} */
/** @} */
