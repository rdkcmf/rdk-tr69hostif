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
 * @defgroup TR69_HOSTIF_WIFI_ACP_ASSOCIATEDDEV TR-069 Object (Device.WiFi.AccessPoint.{i}.AssociatedDevice.{i}.) Public APIs
 * The module provide the interface specifications for TR-069 Object Access Point.
 * A table of the devices currently associated with the access point.
 * At most one entry in this table can exist with a given value for MACAddress.
 * @ingroup TR69_HOSTIF_WIFI
 */


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef DEVICE_WIFI_ACCESSPOINT_ASSOCIATEDDEVICE_H_
#define DEVICE_WIFI_ACCESSPOINT_ASSOCIATEDDEVICE_H_

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"


class hostIf_WiFi_AccessPoint_AssociatedDevice {

    static  GHashTable *ifHash;
    GMutex* m_mutex;
    int dev_id;
    hostIf_WiFi_AccessPoint_AssociatedDevice(int dev_id);
    ~hostIf_WiFi_AccessPoint_AssociatedDevice() {};

public:
    static class hostIf_WiFi_AccessPoint_AssociatedDevice *getInstance(int dev_id);
    static GList* getAllInstances();
    static void closeInstance(hostIf_WiFi_AccessPoint_AssociatedDevice *);
    static void closeAllInstances();

    char MACAddress[17];
    bool AuthenticationState;
    unsigned int LastDataDownlinkRate;
    unsigned int LastDataUplinkRate;
    int SignalStrength;
    unsigned int Retransmissions;
    bool Active;

    /** @addtogroup TR69_HOSTIF_WIFI_ACP_ASSOCIATEDDEV
      * @{
      */
   /**
    * @brief Get the MAC Address of an Associated Device of a WiFi Interface.
    *
    * This function provides the MAC address of the WiFi interface of the device associated
    * with this WiFi interface.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_AssociatedDevice_MACAddress(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Check Whether an associated device has authenticated (true) or not (false).
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_AssociatedDevice_AuthenticationState(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the data transmit rate in kbps that was most recently used for transmission from the access
    * point to the associated device.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_AssociatedDevice_LastDataDownlinkRate(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the data transmit rate in kbps that was most recently used for transmission from the associated
    * device to the access point.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_AssociatedDevice_LastDataUplinkRate(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the radio signal strength of the uplink from the associated device to the access point,
    * measured in dBm, as an average of the last 100 packets received from the device.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_AssociatedDevice_SignalStrength(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the number of packets that had to be re-transmitted, from the last 100 packets sent to the
    * associated device. Multiple re-transmissions of the same packet count as one.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_AssociatedDevice_Retransmissions(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Check whether or not this node is currently present in the WiFi AccessPoint network.
    *
    * The ability to list inactive nodes is optional. If the CPE includes inactive nodes in this table,
    * Active MUST be set to false for each inactive node. The length of time an inactive node remains listed
    * in this table is a local matter to the CPE.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_hostIf_WiFi_AccessPoint_AssociatedDevice_Active(HOSTIF_MsgData_t *stMsgData );

    /** @} */ //End of doxygen Tag TR69_HOSTIF_WIFI_ACP_ASSOCIATEDDEV
};



#endif /* DEVICE_WIFI_ACCESSPOINT_ASSOCIATEDDEVICE_H_ */


/** @} */
/** @} */
