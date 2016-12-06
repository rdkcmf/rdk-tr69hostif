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
  * @defgroup TR69_HOSTIF_WIFI_ENDPOINT_STATS TR-069 Object (Device.WiFi.EndPoint.{i}.Stats.) Public APIs
  * Throughput statistics for this end point.
  * @ingroup TR69_HOSTIF_WIFI_ENDPOINT
  */


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef DEVICE_WIFI_ENDPOINT_STATS_H_
#define DEVICE_WIFI_ENDPOINT_STATS_H_

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"

/** @defgroup TR_069_DEVICE_WIFI API TR-069 Device.WiFi.EndPoint object API.
 *  @ingroup TR_069_DEVICE_WIFI_ENDPOINT
 *
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
 * However, doing so under these circumstances might cause the updated EndPoint row to then violate the table's unique key constraint;
 * if this occurs, the CPE MUST set Status to Error_Misconfigured and disable the offending EndPoint row.
 * At most one entry in this table (regardless of whether or not it is enabled) can exist with a given value for Alias.
 * On creation of a new table entry, the CPE MUST choose an initial value for Alias
 * such that the new entry does not conflict with any existing entries.
 *
 * At most one enabled entry in this table can exist with a given value for SSIDReference.
 *
 *  @{
 */

class hostIf_WiFi_EndPoint_Stats {

    static  GHashTable *ifHash;
    static GMutex *m_mutex;
    int dev_id;
    hostIf_WiFi_EndPoint_Stats(int dev_id);
    ~hostIf_WiFi_EndPoint_Stats() {};

public:
    static class hostIf_WiFi_EndPoint_Stats *getInstance(int dev_id);
    static GList* getAllInstances();
    static void closeInstance(hostIf_WiFi_EndPoint_Stats *);
    static void closeAllInstances();
    static unsigned int EndPointNumberOfEntries;

    unsigned int LastDataDownlinkRate;
    unsigned int LastDataUplinkRate;
    int  SignalStrength;
    unsigned int Retransmissions;

   /**
    * @ingroup TR69_HOSTIF_WIFI_ENDPOINT_STATS
    * @{
    */
   /**
    * @brief Get the data transmit rate in kbps that was most recently used for transmission from the access point
    * to the end point device.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_EndPoint_Stats_LastDataDownlinkRate(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief The data transmit rate in kbps that was most recently used for transmission from the end point
    * to the access point device.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_EndPoint_Stats_LastDataUplinkRate(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief This function provide an indicator of radio signal strength of the downlink from the
    * access point to the end point, measured in dBm, as an average of the last 100 packets received from the device..
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_EndPoint_Stats_SignalStrength(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the number of packets that had to be re-transmitted, from the last 100 packets sent
    * to the access point. Multiple re-transmissions of the same packet count as one.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_EndPoint_Stats_Retransmissions(HOSTIF_MsgData_t *stMsgData );
    
    /** @} */ //End of Doxygen tag TR69_HOSTIF_WIFI_ENDPOINT_STATS
};


#endif /* DEVICE_WIFI_ENDPOINT_STATS_H_ */


/** @} */
/** @} */
