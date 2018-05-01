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
 * @file Device_WiFi.h
 * TR-069 Device.WiFi object Public API.
 */

/**
 * @defgroup TR69_HOSTIF_WIFI TR-069 Object (Device.WiFi)
 * The WiFi object is based on the WiFi Alliance 802.11 specifications ([802.11-2007]).
 * It defines interface objects (Radio and SSID), and application objects (AccessPoint and EndPoint).
 *
 * @par About TR-069 Object Device.WiFi.AccessPoint.{i}
 * @n
 * This object models an 802.11 connection from the perspective of a wireless access point.
 * Each AccessPoint entry is associated with a particular SSID interface instance via the SSIDReference parameter.
 * @n @n
 * For enabled table entries, if SSIDReference is not a valid reference then the table entry is inoperable
 * and the CPE MUST set Status to Error_Misconfigured.
 * @n
 * @note The AccessPoint table includes a unique key parameter that is a strong reference.
 * If a strongly referenced object is deleted, the CPE will set the referencing parameter to an empty string.
 * However, doing so under these circumstances might cause the updated AccessPoint row to then violate
 * the table's unique key constraint; if this occurs, the CPE MUST set Status to Error_Misconfigured
 * and disable the offending AccessPoint row.
 * @n @n
 * At most one entry in this table (regardless of whether or not it is enabled) can exist with a given value
 * for Alias. On creation of a new table entry, the CPE MUST choose an initial value for Alias such that the
 * new entry does not conflict with any existing entries.
 * @n @n
 * At most one enabled entry in this table can exist with a given value for SSIDReference.
 *
 * @par About TR-069 Object Device.WiFi.EndPoint.{i}
 * @n
 * This object models an 802.11 connection from the perspective of a wireless end point.
 * Each EndPoint entry is associated with a particular SSID interface instance via the SSIDReference parameter,
 * and an associated active Profile instance via the ProfileReference parameter.
 * The active profile is responsible for specifying the actual SSID and security settings used by the end point.
 * @n @n
 * For enabled table entries, if SSIDReference or ProfileReference is not a valid reference then the table entry
 * is inoperable and the CPE MUST set Status to Error_Misconfigured.
 * @n @n
 * Note: The EndPoint table includes a unique key parameter that is a strong reference.
 * If a strongly referenced object is deleted, the CPE will set the referencing parameter to an empty string.
 * However, doing so under these circumstances might cause the updated EndPoint row to then violate the table's
 * unique key constraint;
 * @n @n
 * if this occurs, the CPE MUST set Status to Error_Misconfigured and disable the offending EndPoint row.
 * At most one entry in this table (regardless of whether or not it is enabled) can exist with a given value for Alias.
 * On creation of a new table entry, the CPE MUST choose an initial value for Alias
 * such that the new entry does not conflict with any existing entries.
 * @n @n
 * At most one enabled entry in this table can exist with a given value for SSIDReference.
 *
 * @par About TR-069 Object Device.WiFi.Radio.{i}.
 * @n
 * This object models an 802.11 wireless radio on a device (a stackable interface object as described
 * in [Section 4.2/TR-181i2]).
 * @n @n
 * If the device can establish more than one connection simultaneously (e.g. a dual radio device),
 * a separate Radio instance MUST be used for each physical radio of the device. See [Appendix III.1/TR-181i2]
 * for additional information.
 * @n
 * @note A dual-band single-radio device (e.g. an 802.11a/b/g radio) can be configured to operate at 2.4 or
 * 5 GHz frequency bands, but only a single frequency band is used to transmit/receive at a given time.
 * Therefore, a single Radio instance is used even for a dual-band radio.
 * @n @n
 * At most one entry in this table can exist with a given value for Alias, or with a given value for Name.
 *
 * @par About TR-069 Object Device.WiFi.SSID.{i}.
 * @n
 * WiFi SSID table (a stackable interface object as described in [Section 4.2/TR-181i2]), where table
 * entries model the MAC layer. A WiFi SSID entry is typically stacked on top of a Radio object.
 * @n
 * WiFi SSID is also a multiplexing layer, i.e. more than one SSID can be stacked above a single Radio.
 * @n
 * At most one entry in this table (regardless of whether or not it is enabled) can exist with a given
 * value for Alias, or with a given value for Name. On creation of a new table entry, the CPE MUST choose
 * initial values for Alias and Name such that the new entry does not conflict with any existing entries.
 * @n
 * At most one enabled entry in this table can exist with a given value for SSID, or with a given value for BSSID.
 *
 * @ingroup TR69_HOSTIF_PROFILE
 * 
 * @defgroup TR69_HOSTIF_WIFI_API TR-069 Object (Device.WiFi.) Public APIs
 * The WiFi object is based on the WiFi Alliance 802.11 specifications ([802.11-2007]).
 * It defines interface objects (Radio and SSID), and application objects (AccessPoint and EndPoint).
 *
 * @ingroup TR69_HOSTIF_WIFI
 */



/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef DEVICE_WIFI_H_
#define DEVICE_WIFI_H_

#ifdef USE_WIFI_PROFILE
/*****************************************************************************
 * TR069-DEVICE-WIFI API SPECIFIC INCLUDE FILES
 *****************************************************************************/
#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "hostIf_updateHandler.h"

#define QUERY_INTERVAL 10

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
 *  @return The status of the operation.
 *  @retval OK            If parameter requested was successfully fetched. (Same as <b>NO_ERROR</b>).
 *  @retval NOK		      If parameter requested was successfully fetched. (Same as <b>OK</b>).
 *
 *
 *  @{
 */

/**
 * @brief    Get the status of the time client.
 *
 * This function provides the status (enabled or disabled) of the NTP or SNTP time
 * client.
 *
 * See @ref dev_time_getter
 *
 */

class WiFiDevice {
    void *ctxt;
    static  GHashTable *devHash;

    int dev_id;

    WiFiDevice(int dev_id);
    ~WiFiDevice() {};
public:
    static class WiFiDevice *getInstance(int dev_id);
    static void closeInstance(WiFiDevice *);
    static void closeAllInstances();

    static int init();
    static void shutdown();

    void* getContext();
};


class hostIf_WiFi {

    static  GHashTable  *ifHash;

    int dev_id;
    unsigned int uiRadioNumberOfEntries;
    unsigned int uiSSIDNumberOfEntries;
    unsigned int uiAccessPointNumberOfEntries;
    unsigned int uiEndPointNumberOfEntries;

    hostIf_WiFi(int dev_id);
    ~hostIf_WiFi() {};

public:

    static hostIf_WiFi* getInstance(int dev_id);
    static void closeInstance(hostIf_WiFi*);
    static GList* getAllIntefaces();
    static void closeAllInstances();

   /**
     * @ingroup TR69_HOSTIF_WIFI_API
     * @{
     */
    /**
     * @brief This function provides the number of entries in the Radio table.
     */
    int get_Device_WiFi_RadioNumberOfEntries(HOSTIF_MsgData_t *);


    /**
     * @brief This function provides the number of entries in the SSID table.
     */
    int get_Device_WiFi_SSIDNumberOfEntries(HOSTIF_MsgData_t *);

    /**
     * @brief This function provides the number of entries in the AccessPoint table.
     */
    int get_Device_WiFi_AccessPointNumberOfEntries(HOSTIF_MsgData_t *);

    /**
     * @brief This function provides the number of entries in the EndPoint table.
     */
    int get_Device_WiFi_EndPointNumberOfEntries(HOSTIF_MsgData_t *);
    
    /**
     * @brief    Get the wifi enable status.
     *
     * This function gets the value of enable or disable wifi.
     *
     */
    int get_Device_WiFi_EnableWiFi(HOSTIF_MsgData_t *);
    /**
     * @brief    set the wifi enable status.
     *
     * This function sets the value for enable or disable wifi.
     *
     */
    int set_Device_WiFi_EnableWiFi(HOSTIF_MsgData_t *);

    /* End of TR_069_DEVICE_WIFI_GETTER_API doxygen group. */
    /**
     * @}
     */

};
/* End of TR_069_DEVICE_WIFI_SETTER_API doxygen group. */
/**
 * @}
 */
#endif /*#ifdef USE_WIFI_PROFILE*/
#endif /* DEVICE_WIFI_H_ */


/** @} */
/** @} */
