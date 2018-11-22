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
 * Description of Device_WiFi module.
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
    int dev_id;
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

    int get_Device_WiFi_SSID_Enable(HOSTIF_MsgData_t *stMsgData );
    int set_Device_WiFi_SSID_Enable(HOSTIF_MsgData_t *stMsgData );

    int get_Device_WiFi_SSID_Status(HOSTIF_MsgData_t *stMsgData );

    int get_Device_WiFi_SSID_Alias(HOSTIF_MsgData_t *stMsgData );
    int set_Device_WiFi_SSID_Alias(HOSTIF_MsgData_t *stMsgData );

    int get_Device_WiFi_SSID_Name(HOSTIF_MsgData_t *stMsgData );
    int get_Device_WiFi_SSID_LastChange(HOSTIF_MsgData_t *stMsgData );

    int get_Device_WiFi_SSID_LowerLayers(HOSTIF_MsgData_t *stMsgData );
    int set_Device_WiFi_SSID_LowerLayers(HOSTIF_MsgData_t *stMsgData );

    int get_Device_WiFi_SSID_BSSID(HOSTIF_MsgData_t *stMsgData );
    int get_Device_WiFi_SSID_MACAddress(HOSTIF_MsgData_t *stMsgData );

    int get_Device_WiFi_SSID_SSID(HOSTIF_MsgData_t *stMsgData );
    int set_Device_WiFi_SSID_SSID(HOSTIF_MsgData_t *stMsgData );

};

#endif /* #ifdef USE_WIFI_PROFILE */
#endif /* DEVICE_WIFI_SSID_H_ */


/** @} */
/** @} */
