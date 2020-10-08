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
 * @defgroup TR69_HOSTIF_WIFI_ENDPOINT_PROFILE TR-069 Object (Device.WiFi.EndPoint.{i}.Profile.{i}.) Public APIs
 * This module provides the interface functions related to wireless end point profile table.
 *
 * At most one entry in this table (regardless of whether or not it is enabled) can exist with a given value for Alias.
 * On creation of a new table entry, the CPE MUST choose an initial value for Alias such that the new entry does not
 * conflict with any existing entries.
 * At most one enabled entry in this table can exist with all the same values for SSID, Location and Priority.
 * @ingroup TR69_HOSTIF_WIFI
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef DEVICE_WIFI_ENDPOINT_PROFILE_H_
#define DEVICE_WIFI_ENDPOINT_PROFILE_H_

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"
#include "Device_WiFi_EndPoint.h"

class hostIf_WiFi_EndPoint_Profile {

    static  GHashTable *ifHash;
    static GMutex* m_mutex;
    int dev_id = 0;  //CID:103090 - UNINIT_CTOR
    hostIf_WiFi_EndPoint_Profile(int dev_id);
    ~hostIf_WiFi_EndPoint_Profile() {};

public:
    static class hostIf_WiFi_EndPoint_Profile *getInstance(int dev_id);
    static GList* getAllInstances();
    static void closeInstance(hostIf_WiFi_EndPoint_Profile *);
    static void closeAllInstances();

    bool	Enable;
    char	Status[64];
    char Alias[64];
    char SSID[32];
    char Location[256];
    char Priority[256];

   /**
    * @ingroup TR69_HOSTIF_WIFI_ENDPOINT_PROFILE
    * @{
    */
   /**
    * @brief Enables or disables the wireless end point Profile table.
    * When there are multiple WiFi EndPoint Profiles, e.g. each instance supports a different SSID
    * and/or different security configuration, this parameter can be used to control which
    * of the instances are currently enabled.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_EndPoint_Profile_Enable(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the status of the wireless endpoint Profile.
    *
    * The following enumeration of the wireless endpoint profile:
    * - Active
    * - Available
    * - Error (OPTIONAL)
    * - Disabled 
    * The Active value is reserved for the instance that is actively connected. The Available value represents
    * an instance that is not currently active, but is also not disabled or in error.
    * The Error value MAY be used by the CPE to indicate a locally defined error condition.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_EndPoint_Profile_Status(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the alias name of the wireless end point.
    *
    * A non-volatile handle used to reference this instance. Alias provides a mechanism for an ACS to
    * label this instance for future reference.
    *
    * If the CPE supports the Alias-based Addressing feature as defined in [Section 3.6.1/TR-069a4]
    * and described in [Appendix II/TR-069a4], the following mandatory constraints MUST be enforced:
    * - Its value MUST NOT be empty.
    * - Its value MUST start with a letter.
    * - If its value is not assigned by the ACS, it MUST start with a "cpe-" prefix.
    * - The CPE MUST NOT change the parameter value. 
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_EndPoint_Profile_Alias(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the profile identifier in use by the connection.
    *
    * The SSID is an identifier that is attached to packets sent over the wireless LAN that functions
    * as an ID for joining a particular radio network (BSS).
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_EndPoint_Profile_SSID(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the location of the profile.
    *
    * This value serves as a reminder from the user, describing the location of the profile. For example: "Home",
    * "Office", "Neighbor House", "Airport", etc. An empty string is also valid.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_EndPoint_Profile_Location(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the profile priority defines one of the criteria used by the End Point to automatically
    * select the "best" access point when several APs with known profiles are simultaneously available
    * for association.
    *
    * In this situation, the End Point has to select the AP with the higher priority in its profile.
    * If there are several APs with the same priority, providing different SSID or the same SSID, then the wireless
    * end point has to select the APs according to other criteria like signal quality, SNR, etc.
    *
    * @note 0 is the highest priority.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_EndPoint_Profile_Priority(HOSTIF_MsgData_t *stMsgData );

    /** @} */ //End of Doxygen tag TR69_HOSTIF_WIFI_ENDPOINT_PROFILE
};


#endif /* DEVICE_WIFI_ENDPOINT_PROFILE_H_ */


/** @} */
/** @} */
