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
 * @file Device_WiFi_SSID.h
 */

/** 
 * @defgroup TR69_HOSTIF_WIFI_SSID TR-069 Object (Device.WiFi.SSID.{i}.) Public APIs
 *
 * WiFi SSID table (a stackable interface object as described in [Section 4.2/TR-181i2]), where table
 * entries model the MAC layer. A WiFi SSID entry is typically stacked on top of a Radio object.
 *
 * WiFi SSID is also a multiplexing layer, i.e. more than one SSID can be stacked above a single Radio.
 *
 * At most one entry in this table (regardless of whether or not it is enabled) can exist with a given
 * value for Alias, or with a given value for Name. On creation of a new table entry, the CPE MUST choose
 * initial values for Alias and Name such that the new entry does not conflict with any existing entries.
 *
 * At most one enabled entry in this table can exist with a given value for SSID, or with a given value for BSSID.
 *
 * @ingroup TR69_HOSTIF_WIFI
 */



/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef DEVICE_WIFI_SSID_H_
#define DEVICE_WIFI_SSID_H_

#ifdef USE_WIFI_PROFILE
/*****************************************************************************
 * TR069-DEVICE-WIFI API SPECIFIC INCLUDE FILES
 *****************************************************************************/

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"
#include "Device_WiFi.h"

/** @defgroup TR_069_DEVICE_WIFI_API TR-069 Device.WiFi object API.
 *  @ingroup TR_069_API
 *
 *  The The WiFi object is based on the WiFi Alliance 802.11 specifications ([802.11-2007]).
 *  It defines interface objects (Radio and SSID), and application objects (AccessPoint and EndPoint).
 *
 */

/** @addtogroup TR_069_DEVICE_WIFI_GETTER_API TR-069 Device.WiFi Getter API.
 *  @ingroup TR_069_DEVICE_WIFI_API
 *
 *  \section TR-069 Device.WiFi Getter API
 *
 *  This is the getter group of API for the <b>Device.WiFi</b> object.
 *
 *  The interface for all functions is identical and is described here.
 *
 *  @param[in]   HOSTIF_MsgData_t     This is the host IF Message Request data
 *
 *  @param[in]   bool *pChanged     Data type of parameter defined for TR-069. This is same as the
 *                         data type used in the Xi3 data-model.xml file.
 *
 *  @return The status of the operation.
 *  @retval OK            If parameter requested was successfully fetched. (Same as <b>NO_ERROR</b>).
 *  @retval NOK		      If parameter requested was successfully fetched. (Same as <b>OK</b>).
 *
 *
 *  @{
 */


class hostIf_WiFi_SSID {

    static  GHashTable *ifHash;
    int dev_id = 0;  //CID:103919 - UNINIT_CTOR
    hostIf_WiFi_SSID(int dev_id);
    ~hostIf_WiFi_SSID() {};

public:
    static class hostIf_WiFi_SSID *getInstance(int dev_id);
    static GList* getAllInstances();
    static void closeInstance(hostIf_WiFi_SSID *);
    static void closeAllInstances();
    int get_Device_WiFi_SSID_Fields(int ssidIndex);
    void checkWifiSSIDFetch(int radioIndex);

    bool enable;
    char status[BUFF_LENGTH_64];
    char alias[BUFF_LENGTH_64];
    char name[BUFF_LENGTH_64];
    unsigned int LastChange;
    char LowerLayers[BUFF_LENGTH_1024];
    char BSSID[BUFF_MAC];
    char MACAddress[BUFF_MAC];
    char SSID[BUFF_LENGTH_32];
    /**
     * @brief    Get the MAC Address of an Associated Device of a WiFi Interface.
     *
     * This function provides the MAC address of the WiFi interface of the device associated
     * with this WiFi interface.
     *
     * See @ref dev_wifi_if_assocdev_getter
     *
     */

   /**
    * @ingroup TR69_HOSTIF_WIFI_SSID
    * @{
    */
   /**
    * @brief Get the status of SSID entry.
    *
    * This function provides true/false value based on the Device.WiFi.SSID.Enable parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_Enable(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Set the enable or disable status for the SSID entry.
    * This parameter is based on ifAdminStatus from [RFC2863].
    *
    * This function will update true/false value for the Device.WiFi.SSID.Enable parameter.
    *
    * @param[in] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int set_Device_WiFi_SSID_Enable(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the current operational state of the SSID entry (see [Section 4.2.2/TR-181i2]).
    *
    * The enumuration for SSID entry is:
    * - Up
    * - Down
    * - Unknown
    * - Dormant
    * - NotPresent
    * - LowerLayerDown
    * - Error (OPTIONAL) 
    *
    * When Enable is false then Status SHOULD normally be Down (or NotPresent or Error if there is a fault
    * condition on the interface).
    *
    * - When Enable is changed to true then Status SHOULD change to Up if and only if the interface is able
    * to transmit and receive network traffic; it SHOULD change to Dormant if and only if the interface
    * is operable but is waiting for external actions before it can transmit and receive network traffic
    * (and subsequently change to Up if still operable when the expected actions have completed);
    * - It SHOULD change to LowerLayerDown if and only if the interface is prevented from entering the Up
    * state because one or more of the interfaces beneath it is down; it SHOULD remain in the Error state
    * if there is an error or other fault condition detected on the interface; it SHOULD remain in the
    * NotPresent state if the interface has missing (typically hardware) components;
    * - It SHOULD change to Unknown if the state of the interface can not be determined for some reason.
    *
    * This parameter is based on ifOperStatus from [RFC2863]. 
    *
    * This function provides the output as a string available in Device.WiFi.SSID.Status.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_Status(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the Alias based addresing given for SSID.
    *
    * This function provides the output as string available in Device.WiFi.SSID.Alias parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_Alias(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Set the Alias based addresing for SSID.
    *
    * A non-volatile handle used to reference this instance. Alias provides a mechanism for an ACS to
    * label this instance for future reference.
    *
    * If the CPE supports the Alias-based Addressing feature as defined in [Section 3.6.1/TR-069a4] and
    * described in [Appendix II/TR-069a4], the following mandatory constraints MUST be enforced:
    *
    * - Its value MUST NOT be empty.
    * - Its value MUST start with a letter.
    * - If its value is not assigned by the ACS, it MUST start with a "cpe-" prefix.
    * - The CPE MUST NOT change the parameter value. 
    *
    * This function will update the Alias-based addresing in Device.WiFi.SSID.Alias parameter.
    *
    * @param[in] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int set_Device_WiFi_SSID_Alias(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the textual name of the SSID entry as assigned by the CPE.
    *
    * This function provides the output as string available in Device.WiFi.SSID.Name parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_Name(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the accumulated time in seconds since the SSID entered its current operational state.
    *
    * This function provides the output as unsigned integer value available in Device.WiFi.SSID.LastChange parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_LastChange(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get a comma-separated list (maximum list length 1024) of strings.
    * Each list item MUST be the path name of an interface object that is stacked immediately below
    * this interface object.
    *
    * This function provides the output as string available in Device.WiFi.SSID.LowerLayers parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_LowerLayers(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Set a comma-separated list (maximum list length 1024) of strings.
    * Each list item MUST be the path name of an interface object that is stacked immediately below
    * this interface object. If the referenced object is deleted, the corresponding item MUST be
    * removed from the list.
    *
    * @see [Section 4.2.1/TR-181i2].
    *
    * This function will update the string valu the the Device.WiFi.SSID.LowerLayers parameter.
    *
    * @param[in] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int set_Device_WiFi_SSID_LowerLayers(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the Basic Service Set ID.
    *
    * This is the MAC address of the access point, which can either be local (when this instance models an
    * access point SSID) or remote (when this instance models an end point SSID).
    *
    * This function provides the output as string available in Device.WiFi.SSID.BSSID parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_BSSID(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the MAC address of this interface.
    *
    * If this instance models an access point SSID, MAC Address is the same as MAC Address.
    *
    * @note This is not necessarily the same as the Ethernet header source or destination MAC address,
    * which is associated with the IP interface and is modeled via the Ethernet.Link.{i}.MACAddress parameter.
    *
    * This function provides the output as string available in Device.WiFi.SSID.MACAddress parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_MACAddress(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the current service set identifier in use by the connection.
    * The SSID is an identifier that is attached to packets sent over the wireless LAN that functions as an
    * ID for joining a particular radio network (BSS).
    *
    * This function provides the output as string available in Device.WiFi.SSID.SSID parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_SSID(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Set the current service set identifier in use by the connection.
    * The SSID is an identifier that is attached to packets sent over the wireless LAN that functions as an
    * ID for joining a particular radio network (BSS).
    *
    * This function will update the SSID value in string format in Device.WiFi.SSID.SSID parameter.
    *
    * @param[in] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int set_Device_WiFi_SSID_SSID(HOSTIF_MsgData_t *stMsgData );

   /** @} */ //End of Doxygen tag TR69_HOSTIF_WIFI_SSID
};

#endif /* #ifdef USE_WIFI_PROFILE */
#endif /* DEVICE_WIFI_SSID_H_ */


/** @} */
/** @} */
