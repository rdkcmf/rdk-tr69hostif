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
 *
 * @brief TR-069 Device.WiFi object Public API.
 *
 * Description of WiFi module.
 *
 *
 * @par Document
 * Document reference.
 *
 *
 * @par Open Issues (in no particular order)
 * -# Issue 1
 * -# Issue 2
 *
 *
 * @par Assumptions
 * -# Assumption
 * -# Assumption
 *
 *
 * @par Abbreviations
 * - ACK:     Acknowledge.
 * - BE:      Big-Endian.
 * - cb:      Callback function (suffix).
 * - config:  Configuration.
 * - desc:    Descriptor.
 * - dword:   Double word quantity, i.e., four bytes or 32 bits in size.
 * - intfc:   Interface.
 * - LE:      Little-Endian.
 * - LS:      Least Significant.
 * - MBZ:     Must be zero.
 * - MS:      Most Significant.
 * - _t:      Type (suffix).
 * - word:    Two byte quantity, i.e. 16 bits in size.
 * - xfer:    Transfer.
 *
 *
 * @par Implementation Notes
 * -# Note
 * -# Note
 *
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
     * @brief    Get the RadioNumberOfEntries.
     *
     * This function provides the number of entries in the Radio table.
     *
     */
    int get_Device_WiFi_RadioNumberOfEntries(HOSTIF_MsgData_t *);


    /**
     * @brief    Get the SSIDNumberOfEntries.
     *
     * This function provides the number of entries in the SSID table.
     *
     */
    int get_Device_WiFi_SSIDNumberOfEntries(HOSTIF_MsgData_t *);

    /**
     * @brief  Get the AccessPointNumberOfEntries.
     *
     * This function provides the number of entries in the AccessPoint table.
     *
     */
    int get_Device_WiFi_AccessPointNumberOfEntries(HOSTIF_MsgData_t *);

    /**
     * @brief    Get the EndPointNumberOfEntries.
     *
     * This function provides the number of entries in the EndPoint table.
     *
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
