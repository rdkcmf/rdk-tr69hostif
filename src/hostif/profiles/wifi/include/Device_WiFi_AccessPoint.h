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
 * @defgroup TR69_HOSTIF_WIFI_ACCESSPOINT TR-069 Object (Device.WiFi.AccessPoint.{i}) API
 *
 * This object models an 802.11 connection from the perspective of a wireless access point.
 * Each AccessPoint entry is associated with a particular SSID interface instance via the SSIDReference parameter.
 *
 * For enabled table entries, if SSIDReference is not a valid reference then the table entry is inoperable
 * and the CPE MUST set Status to Error_Misconfigured.
 *
 * @note The AccessPoint table includes a unique key parameter that is a strong reference.
 * If a strongly referenced object is deleted, the CPE will set the referencing parameter to an empty string.
 * However, doing so under these circumstances might cause the updated AccessPoint row to then violate
 * the table's unique key constraint; if this occurs, the CPE MUST set Status to Error_Misconfigured
 * and disable the offending AccessPoint row.
 *
 * At most one entry in this table (regardless of whether or not it is enabled) can exist with a given value
 * for Alias. On creation of a new table entry, the CPE MUST choose an initial value for Alias such that the
 * new entry does not conflict with any existing entries.
 *
 * At most one enabled entry in this table can exist with a given value for SSIDReference.
 * @ingroup TR69_HOSTIF_WIFI
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef DEVICE_WIFI_ACCESSPOINT_H_
#define DEVICE_WIFI_ACCESSPOINT_H_

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"

class hostIf_WiFi_AccessPoint {

    static  GHashTable *ifHash;
    int dev_id;
    hostIf_WiFi_AccessPoint(int dev_id);
    ~hostIf_WiFi_AccessPoint() {};

public:
    static class hostIf_WiFi_AccessPoint *getInstance(int dev_id);
    static GList* getAllInstances();
    static void closeInstance(hostIf_WiFi_AccessPoint *);
    static void closeAllInstances();

    static unsigned int AccessPointNumberOfEntries;

    bool	Enable;
    char	Status[64];
    char Alias[64];
    char SSIDReference[256];
    bool SSIDAdvertisementEnabled;
    unsigned int	RetryLimit;
    bool	WMMCapability;
    bool	UAPSDCapability;
    bool	WMMEnable;
    bool	UAPSDEnable;
    unsigned int	AssociatedDeviceNumberOfEntries;

   /**
    * @ingroup TR69_HOSTIF_WIFI_ACCESSPOINT
    * @{
    */
   /**
    * @brief Enables or disables this access point.
    *
    * @param[in] stMsgData TR-069 Host interface message request.
    */
    int get_hostIf_WiFi_AccessPoint_Enable(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the status of the access point which is currently being set.
    * The status of the access point could be the following types,
    * - Disabled
    * - Enabled
    * - Error_Misconfigured
    * - Error (OPTIONAL)
    * The Error_Misconfigured value indicates that a necessary configuration value is undefined or invalid.
    * The Error value may be used by the CPE to indicate a locally defined error condition.
    *
    * @param[in] stMsgData TR-069 Host interface message request.
    */
    int get_hostIf_WiFi_AccessPoint_Status(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the access point alias based addressing. Alias provides a mechanism for an ACS to label
    * the instance for future reference. The following mandatory constraints MUST be enforced:
    * - Its value MUST NOT be empty.
    * - Its value MUST start with a letter.
    * - If its value is not assigned by the ACS, it MUST start with a "cpe-" prefix.
    * - The CPE MUST NOT change the parameter value. 
    * @param[in] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_Alias(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get path name of a row in the SSID table. If the parameter value is set to Null indicate that
    * the reference object is deleted.
    *
    * @param[in] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_SSIDReference(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Check whether or not beacons include the SSID name.
    *
    * @param[in] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_SSIDAdvertisementEnabled(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get thee maximum number of retransmission for a packet. This corresponds to IEEE 802.11
    * parameter dot11ShortRetryLimit.
    *
    * @param[in] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_RetryLimit(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Check whether this access point supports WiFi Multimedia (WMM) Access Categories (AC).
    *
    * @param[in] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_WMMCapability(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Check whether this access point supports WMM Unscheduled Automatic Power Save Delivery (U-APSD).
    * @note U-APSD support implies WMM support.
    *
    * @param[in] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_UAPSDCapability(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Check whether WMM support is currently enabled. When enabled, this is indicated in beacon frames.
    *
    * @param[in] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_WMMEnable(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Whether U-APSD support is currently enabled. When enabled, this is indicated in beacon frames.
    * @note U-APSD can only be enabled if WMM is also enabled.
    *
    * @param[in] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_UAPSDEnable(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the number of entries in the AssociatedDevice table. The AssociatedDevice table contains
    * information about other Wifi devices currently associated with this Wifi interface.
    *
    * @param[in] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_AssociatedDeviceNumberOfEntries(HOSTIF_MsgData_t *stMsgData );

 /** @} */ //End of Doxygen tag TR69_HOSTIF_WIFI_ACCESSPOINT
};




#endif /* DEVICE_WIFI_ACCESSPOINT_H_ */


/** @} */
/** @} */
