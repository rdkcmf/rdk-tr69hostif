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


#ifndef DEVICE_WIFI_ENDPOINT_H_
#define DEVICE_WIFI_ENDPOINT_H_

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"

/** 
 * @defgroup TR69_HOSTIF_WIFI_ENDPOINT TR-069 Object (Device.WiFi.EndPoint.{i}) Public APIs
 * This object models an 802.11 connection from the perspective of a wireless end point.
 * Each EndPoint entry is associated with a particular SSID interface instance via the SSIDReference parameter,
 * and an associated active Profile instance via the ProfileReference parameter.
 * The active profile is responsible for specifying the actual SSID and security settings used by the end point.
 *
 * For enabled table entries, if SSIDReference or ProfileReference is not a valid reference then the table entry
 * is inoperable and the CPE MUST set Status to Error_Misconfigured.
 *
 * Note: The EndPoint table includes a unique key parameter that is a strong reference.
 * If a strongly referenced object is deleted, the CPE will set the referencing parameter to an empty string.
 * However, doing so under these circumstances might cause the updated EndPoint row to then violate the table's
 * unique key constraint;
 *
 * if this occurs, the CPE MUST set Status to Error_Misconfigured and disable the offending EndPoint row.
 * At most one entry in this table (regardless of whether or not it is enabled) can exist with a given value for Alias.
 * On creation of a new table entry, the CPE MUST choose an initial value for Alias
 * such that the new entry does not conflict with any existing entries.
 *
 * At most one enabled entry in this table can exist with a given value for SSIDReference.
 *
 * @ingroup TR69_HOSTIF_WIFI
 */
class hostIf_WiFi_EndPoint {

    static  GHashTable *ifHash;
    int dev_id;
    hostIf_WiFi_EndPoint(int dev_id);
    ~hostIf_WiFi_EndPoint() {};

    int refreshCache ();

public:
    static class hostIf_WiFi_EndPoint *getInstance(int dev_id);
    static GList* getAllInstances();
    static void closeInstance(hostIf_WiFi_EndPoint *);
    static void closeAllInstances();

    bool Enable;
    char Status[64];
    char Alias[64];
    char ProfileReference[256];
    char SSIDReference[256]; // value of WiFi_EndPoint_Diag_Params.SSIDReference returned by IARM call to netsrvmgr
    unsigned int ProfileNumberOfEntries;
    struct {
        unsigned long LastDataDownlinkRate;
        unsigned long LastDataUplinkRate;
        int  SignalStrength;
        unsigned long Retransmissions;
    } stats;

   /**
    * @ingroup TR69_HOSTIF_WIFI_ENDPOINT
    * @{
    */
   /**
    * @brief Check wethen wifi endpoint is enabled or not.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_EndPoint_Enable(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Enables or disables this end point.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int set_Device_WiFi_EndPoint_Enable(HOSTIF_MsgData_t *stMsgData );

    /**
    * @brief Get the status of the wireless end point.
    *
    * The status of the wifi end point could be the enumeration of:
    * - Disabled
    * - Enabled
    * - Error_Misconfigured
    * - Error (OPTIONAL)
    *
    *  The Error_Misconfigured value indicates that a necessary configuration value is undefined or invalid.
    *  The Error value MAY be used by the CPE to indicate a locally defined error condition.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_EndPoint_Status(HOSTIF_MsgData_t *stMsgData );

    /**
    * @brief Get the alias name of the wireless endpoint.
    *
    * A non-volatile handle used to reference this instance. Alias provides a
    * mechanism for an ACS to label this instance for future reference.
    * An initial unique value MUST be assigned when the CPE creates an instance of
    * this object.
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_EndPoint_Alias(HOSTIF_MsgData_t *stMsgData );

    /** 
    * @brief Set the alias name for the wireless endpoint.
    *
    * A non-volatile handle used to reference this instance. Alias provides a mechanism for an ACS to
    * label this instance for future reference.
    * If the CPE supports the Alias-based Addressing feature as defined in [Section 3.6.1/TR-069a4] and
    * described in [Appendix II/TR-069a4], the following mandatory constraints MUST be enforced:
    * - Its value MUST NOT be empty.
    * - Its value MUST start with a letter.
    * - If its value is not assigned by the ACS, it MUST start with a "cpe-" prefix.
    * - The CPE MUST NOT change the parameter value. 
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int set_Device_WiFi_EndPoint_Alias (HOSTIF_MsgData_t *stMsgData);

    /** 
    * @brief Set the alias name for the wireless endpoint.
    *
    * A non-volatile handle used to reference this instance. Alias provides a mechanism for an ACS to
    * label this instance for future reference.
    * If the CPE supports the Alias-based Addressing feature as defined in [Section 3.6.1/TR-069a4] and
    * described in [Appendix II/TR-069a4], the following mandatory constraints MUST be enforced:
    * - Its value MUST NOT be empty.
    * - Its value MUST start with a letter.
    * - If its value is not assigned by the ACS, it MUST start with a "cpe-" prefix.
    * - The CPE MUST NOT change the parameter value. 
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_EndPoint_ProfileReference (HOSTIF_MsgData_t *stMsgData);

   /**
    * @brief Set the value MUST be the path name of a row in the Profile table.
    * If the referenced object is deleted, the parameter value MUST be set to an empty string.
    * This is the currently active profile, which specifies the SSID and security settings to be used by the end point.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int set_Device_WiFi_EndPoint_ProfileReference (HOSTIF_MsgData_t *stMsgData);

    /*
    * @brief Get the wireless endpoint reference. The value MUST be the path name of a row in the SSID table.
    * If the referenced object is deleted, the parameter value MUST be set to an empty string.
    * SSIDReference is determined based on the Profile.{i}.SSID within the associated ProfileReference)
    * endpoint profile. SSIDReference MUST be an empty string if ProfileReference is an empty string
    * (i.e. only when an active profile is assigned can the associated SSID interface be determined).
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_EndPoint_SSIDReference (HOSTIF_MsgData_t *stMsgData);

    /*
    * @brief Get the number of entries in the wireless endpoint Profile table.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_EndPoint_ProfileNumberOfEntries (HOSTIF_MsgData_t *stMsgData);

    /**
     * @brief Get the data transmit rate in kbps that was most recently used for transmission from the access point
     * to the end point device.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_Device_WiFi_EndPoint_Stats_LastDataDownlinkRate (HOSTIF_MsgData_t *stMsgData);

    /**
     * @brief The data transmit rate in kbps that was most recently used for transmission from the end point
     * to the access point device.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_Device_WiFi_EndPoint_Stats_LastDataUplinkRate (HOSTIF_MsgData_t *stMsgData);

    /**
     * @brief This function provide an indicator of radio signal strength of the downlink from the
     * access point to the end point, measured in dBm, as an average of the last 100 packets received from the device..
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_Device_WiFi_EndPoint_Stats_SignalStrength (HOSTIF_MsgData_t *stMsgData);

    /**
     * @brief Get the number of packets that had to be re-transmitted, from the last 100 packets sent
     * to the access point. Multiple re-transmissions of the same packet count as one.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_Device_WiFi_EndPoint_Stats_Retransmissions (HOSTIF_MsgData_t *stMsgData);

    /** @} */ //End of doxygen tag TR69_HOSTIF_WIFI_ENDPOINT
};

#endif /* DEVICE_WIFI_ENDPOINT_H_ */

/** @} */
/** @} */
