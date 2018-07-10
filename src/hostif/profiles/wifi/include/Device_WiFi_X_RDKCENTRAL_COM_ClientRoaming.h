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
 * @file Device_WiFi_X_RDKCENTRAL_COM_ClientRoaming.h
 *
 * @brief TR-069 Device.WiFi.Radio object Public API.
 *
 * Description of Device_WiFi_X_RDKCENTRAL_COM_ClientRoaming module.
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


#ifndef HOSTIF_DEVICE_WIFI_X_RDKCENTRAL_COM_CLIENTROAMING_H_
#define HOSTIF_DEVICE_WIFI_X_RDKCENTRAL_COM_CLIENTROAMING_H_

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"
#include "Device_WiFi.h"


/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/

/** @defgroup TR_069_DEVICE_WIFI_X_RDKCENTRAL_COM_CLIENTROAMING API TR-069 Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming object API.
 *  @ingroup TR_069_DEVICE_WIFI_RDKCENTRAL_COM_CLIENTROAMING
 *
 *  The Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming object table. This object models an 802.11 wireless radio
 *  on a device (a stackable interface object as described in [Section 4.2/TR-181i2])..
 *
 *
 *  @{
 */
#ifdef WIFI_CLIENT_ROAMING
class hostIf_WiFi_Xrdk_ClientRoaming {

    static  GHashTable *ifHash;

    int dev_id;
    hostIf_WiFi_Xrdk_ClientRoaming(int dev_id);
    ~hostIf_WiFi_Xrdk_ClientRoaming() {};
    int checkWifiClientRoamingropsFetch();

public:
    static class hostIf_WiFi_Xrdk_ClientRoaming *getInstance(int dev_id);
    static void closeInstance(hostIf_WiFi_Xrdk_ClientRoaming *);
    static void closeAllInstances();
    
    bool roamingEnable;
    bool roamingConfigEnable;
    int preassnProbeRetryCnt;
    int preassnBestThreshold;
    int preassnBestDelta;

    /**
     * 	@brief Enables or disables the Client Romaing
     *
     * 	This function provides to true/false value based on the
     * 	Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.Enable
     *
     */
    int get_Device_WiFi_X_Rdkcentral_clientRoaming_Enable(HOSTIF_MsgData_t *stMsgData);

    /**
     * 	@brief set Enables/disables the Client Roaming
     *
     * 	This function set to true/false to  'Device.WiFi.X_RDKCENTRAL-COM_ClientRoaming.Enable' parameter.
     *
     */
    int set_Device_WiFi_X_Rdkcentral_clientRoaming_Enable(HOSTIF_MsgData_t *stMsgData);

    /**
     * 	@brief get_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_ProbeRetryCnt
     *
     *
     */
    int get_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_ProbeRetryCnt(HOSTIF_MsgData_t *stMsgData);

    /**
     *  @brief set_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_ProbeRetryCnt
     *
     *
     */
    int set_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_ProbeRetryCnt(HOSTIF_MsgData_t *stMsgData);

    /**
     *  @brief get_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_BestThresholdLevel
     *
     *
     */
    int get_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_BestThresholdLevel(HOSTIF_MsgData_t *stMsgData);

    /**
     *  @brief set_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_BestThresholdLevel
     *
     *
     */
    int set_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_BestThresholdLevel(HOSTIF_MsgData_t *stMsgData);

    /**
     *  @brief get_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_BestDeltaLevel
     *
     *
     */
    int get_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_BestDeltaLevel(HOSTIF_MsgData_t *stMsgData);

    /**
     *  @brief set_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_BestDeltaLevel
     *
     *
     */
    int set_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_BestDeltaLevel(HOSTIF_MsgData_t *stMsgData);
};

/* End of TR069_HOSTIF_DEVICE_WIFI_X_RDKCENTRAL_COM_CLIENTROAMING_H_ doxygen group */
/**
 * @}
 */
#endif // WIFI_CLIENT_ROAMING
#endif /* HOSTIF_DEVICE_WIFI_X_RDKCENTRAL_COM_CLIENTROAMING */


/** @} */
/** @} */
