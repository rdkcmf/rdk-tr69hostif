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
  * @defgroup TR69_HOSTIF_WIFI_ENDPOINT_SECURITY TR-069 Object (Device.WiFi.EndPoint.{i}.Security.) Public APIs
  * This object contains security related parameters that apply to a WiFi end point [802.11-2007].
  * @ingroup TR69_HOSTIF_WIFI
  */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef DEVICE_WIFI_ENDPOINT_SECURITY_H_
#define DEVICE_WIFI_ENDPOINT_SECURITY_H_

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"

class hostIf_WiFi_EndPoint_Security {

    static  GHashTable *ifHash;
    int dev_id;
    hostIf_WiFi_EndPoint_Security(int dev_id);
    ~hostIf_WiFi_EndPoint_Security() {};

public:
    static class hostIf_WiFi_EndPoint_Security *getInstance(int dev_id);
    static GList* getAllInstances();
    static void closeInstance(hostIf_WiFi_EndPoint_Security *);
    static void closeAllInstances();

    char ModesSupported[64];

   /**
    * @ingroup TR69_HOSTIF_WIFI_ENDPOINT_SECURITY
    * @{
    */
   /**
    * @brief This function provides the comma-separated list of strings contains which security modes
    * the wireless EndPoint instance is capable of supporting.
    *
    * Each list item is an enumeration of:
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
    int get_hostIf_WiFi_EndPoint_ModesSupported(HOSTIF_MsgData_t *stMsgData );

    /**
    * @brief Get the security mode enabled for wifi device. The value must be a member of the list
    * reported by the ModesSupported parameter, indicates which security mode is enabled.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_EndPoint_Security_ModesEnabled(HOSTIF_MsgData_t *stMsgData );

    /** @ */ //End of Doxygen tag TR69_HOSTIF_WIFI_ENDPOINT_SECURITY
};




#endif /* DEVICE_WIFI_ENDPOINT_H_ */


/** @} */
/** @} */
