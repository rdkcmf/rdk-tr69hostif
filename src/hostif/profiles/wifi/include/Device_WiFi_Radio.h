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
 * @file Device_WiFi_Radio.h
 *
 * @brief TR-069 Device.WiFi.Radio object Public API.
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


#ifndef HOSTIF_DEVICE_WIFI_RADIO_H_
#define HOSTIF_DEVICE_WIFI_RADIO_H_

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"
#include "Device_WiFi.h"


/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/

/** @defgroup TR_069_DEVICE_WIFI API TR-069 Device.WiFi.Radio object API.
 *  @ingroup TR_069_DEVICE_WIFI_RADIO_API
 *
 *  The Device.WiFi.Radio object table. This object models an 802.11 wireless radio
 *  on a device (a stackable interface object as described in [Section 4.2/TR-181i2])..
 *
 *	If the device can establish more than one connection simultaneously (e.g. a dual radio device),
 *	a separate Radio instance MUST be used for each physical radio of the device.
 *	See [Appendix III.1/TR-181i2] for additional information.
 *
 *	Note: A dual-band single-radio device (e.g. an 802.11a/b/g radio) can be configured to
 *	operate at 2.4 or 5 GHz frequency bands, but only a single frequency band is used to
 *	transmit/receive at a given time. Therefore, a single Radio instance is used even for a dual-band radio.
 *
 *	At most one entry in this table can exist with a given value for Alias, or with a given value for Name.
 *
 *  @{
 */
class hostIf_WiFi_Radio {

    static  GHashTable *ifHash;

    int dev_id;
    time_t radioFirstExTime;
    hostIf_WiFi_Radio(int dev_id);
    ~hostIf_WiFi_Radio() {};

public:
    static class hostIf_WiFi_Radio *getInstance(int dev_id);
    static GList* getAllAssociateDevs();
    static void closeInstance(hostIf_WiFi_Radio *);
    static void closeAllInstances();
    int get_Device_WiFi_Radio_Props_Fields(int radioIndex);
    void checkWifiRadioFetch(int radioIndex);

    bool Enable;
    char Status[BUFF_LENGTH_64];
    char Alias[BUFF_LENGTH_64];
    char Name[BUFF_LENGTH_64];
    unsigned int LastChange;
    char LowerLayers[BUFF_LENGTH_1024];
    bool Upstream;
    unsigned int MaxBitRate;
    char SupportedFrequencyBands[BUFF_LENGTH_256];
    char OperatingFrequencyBand[BUFF_LENGTH_64];
    char SupportedStandards[BUFF_LENGTH_64];
    char OperatingStandards[BUFF_LENGTH_64];
    char PossibleChannels[BUFF_LENGTH_64];
    char ChannelsInUse[BUFF_LENGTH_1024];
    unsigned int Channel;
    bool AutoChannelSupported;
    bool AutoChannelEnable;
    unsigned int AutoChannelRefreshPeriod;
    char OperatingChannelBandwidth[BUFF_MIN_16];
    char ExtensionChannel[BUFF_LENGTH_64];
    char GuardInterval[BUFF_LENGTH_64];
    int mcs;
    char TransmitPowerSupported[BUFF_LENGTH_64];
    int TransmitPower;
    bool IEEE80211hSupported;
    bool IEEE80211hEnabled;
    char RegulatoryDomain[BUFF_MIN_16];

    /**
    * 	@brief Enables or disables the radio.
    *
    * 	This function provides to true/false value based on the
    * 	Device.WiFi.Radio.Enable status.
    *
    * 	This parameter is based on ifAdminStatus from [RFC2863].
    * 	@note     This parameter is based on wifiNodeIndex from [<a href=http://www.broadband-forum.org/cwmp/tr-181-2-2-0.html#R.RFC2863</a>].
    */
    int get_Device_WiFi_Radio_Enable(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
     * 	@brief set Enables/disables the radio.
     *
     * 	This function set to true/false to  'Device.WiFi.Radio.Enable status' parameter.
     *
     * 	This parameter is based on ifAdminStatus from [RFC2863].
     * 	See @ref get_Device_WiFi_Radio_Enable()
     */
    int set_Device_WiFi_Radio_Enable(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
     * 	@brief get_Device_WiFi_Radio_Status
     *
     * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
     * 	Enumeration of:
     * 	   Up
     * 	   Down
     * 	   Unknown
     * 	   Dormant
     * 	   NotPresent
     * 	   LowerLayerDown
     * 	   Error (OPTIONAL)
     *
     * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
     * 	if there is a fault condition on the interface).
     *
     * 	This parameter is based on ifOperStatus from [RFC2863].
     *
     */
    int get_Device_WiFi_Radio_Status(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
     * @brief  A non-volatile handle used to reference this instance.
     * Alias provides a mechanism for an ACS to label this instance for future reference.
     * An initial unique value MUST be assigned when the CPE creates an instance of this object.
     *
     * This function get/set the 'Device.WiFi.Radio.Alias' parameter.
     *
     */
    int get_Device_WiFi_Radio_Alias(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
      * 	@brief get_Device_WiFi_Radio_Status
      *
      * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
      * 	Enumeration of:
      * 	   Up
      * 	   Down
      * 	   Unknown
      * 	   Dormant
      * 	   NotPresent
      * 	   LowerLayerDown
      * 	   Error (OPTIONAL)
      *
      * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
      * 	if there is a fault condition on the interface).
      *
      * 	This parameter is based on ifOperStatus from [RFC2863].
      *
      */
    int set_Device_WiFi_Radio_Alias(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
      * 	@brief get_Device_WiFi_Radio_Status
      *
      * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
      * 	Enumeration of:
      * 	   Up
      * 	   Down
      * 	   Unknown
      * 	   Dormant
      * 	   NotPresent
      * 	   LowerLayerDown
      * 	   Error (OPTIONAL)
      *
      * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
      * 	if there is a fault condition on the interface).
      *
      * 	This parameter is based on ifOperStatus from [RFC2863].
      *
      */
    int get_Device_WiFi_Radio_Name(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
      * 	@brief get_Device_WiFi_Radio_Status
      *
      * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
      * 	Enumeration of:
      * 	   Up
      * 	   Down
      * 	   Unknown
      * 	   Dormant
      * 	   NotPresent
      * 	   LowerLayerDown
      * 	   Error (OPTIONAL)
      *
      * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
      * 	if there is a fault condition on the interface).
      *
      * 	This parameter is based on ifOperStatus from [RFC2863].
      *
      */
    int get_Device_WiFi_Radio_LastChange(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
      * 	@brief get_Device_WiFi_Radio_Status
      *
      * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
      * 	Enumeration of:
      * 	   Up
      * 	   Down
      * 	   Unknown
      * 	   Dormant
      * 	   NotPresent
      * 	   LowerLayerDown
      * 	   Error (OPTIONAL)
      *
      * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
      * 	if there is a fault condition on the interface).
      *
      * 	This parameter is based on ifOperStatus from [RFC2863].
      *
      */
    int get_Device_WiFi_Radio_LowerLayers(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
      * 	@brief get_Device_WiFi_Radio_Status
      *
      * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
      * 	Enumeration of:
      * 	   Up
      * 	   Down
      * 	   Unknown
      * 	   Dormant
      * 	   NotPresent
      * 	   LowerLayerDown
      * 	   Error (OPTIONAL)
      *
      * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
      * 	if there is a fault condition on the interface).
      *
      * 	This parameter is based on ifOperStatus from [RFC2863].
      *
      */
    int set_Device_WiFi_Radio_LowerLayers(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
      * 	@brief get_Device_WiFi_Radio_Status
      *
      * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
      * 	Enumeration of:
      * 	   Up
      * 	   Down
      * 	   Unknown
      * 	   Dormant
      * 	   NotPresent
      * 	   LowerLayerDown
      * 	   Error (OPTIONAL)
      *
      * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
      * 	if there is a fault condition on the interface).
      *
      * 	This parameter is based on ifOperStatus from [RFC2863].
      *
      */
    int get_Device_WiFi_Radio_Upstream(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
      * 	@brief get_Device_WiFi_Radio_Status
      *
      * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
      * 	Enumeration of:
      * 	   Up
      * 	   Down
      * 	   Unknown
      * 	   Dormant
      * 	   NotPresent
      * 	   LowerLayerDown
      * 	   Error (OPTIONAL)
      *
      * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
      * 	if there is a fault condition on the interface).
      *
      * 	This parameter is based on ifOperStatus from [RFC2863].
      *
      */
    int get_Device_WiFi_Radio_MaxBitRate(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
      * 	@brief get_Device_WiFi_Radio_Status
      *
      * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
      * 	Enumeration of:
      * 	   Up
      * 	   Down
      * 	   Unknown
      * 	   Dormant
      * 	   NotPresent
      * 	   LowerLayerDown
      * 	   Error (OPTIONAL)
      *
      * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
      * 	if there is a fault condition on the interface).
      *
      * 	This parameter is based on ifOperStatus from [RFC2863].
      *
      */
    int get_Device_WiFi_Radio_SupportedFrequencyBands(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
      * 	@brief get_Device_WiFi_Radio_Status
      *
      * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
      * 	Enumeration of:
      * 	   Up
      * 	   Down
      * 	   Unknown
      * 	   Dormant
      * 	   NotPresent
      * 	   LowerLayerDown
      * 	   Error (OPTIONAL)
      *
      * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
      * 	if there is a fault condition on the interface).
      *
      * 	This parameter is based on ifOperStatus from [RFC2863].
      *
      */
    int get_Device_WiFi_Radio_OperatingFrequencyBand(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
      * 	@brief get_Device_WiFi_Radio_Status
      *
      * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
      * 	Enumeration of:
      * 	   Up
      * 	   Down
      * 	   Unknown
      * 	   Dormant
      * 	   NotPresent
      * 	   LowerLayerDown
      * 	   Error (OPTIONAL)
      *
      * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
      * 	if there is a fault condition on the interface).
      *
      * 	This parameter is based on ifOperStatus from [RFC2863].
      *
      */
    int set_Device_WiFi_Radio_OperatingFrequencyBand(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
      * 	@brief get_Device_WiFi_Radio_Status
      *
      * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
      * 	Enumeration of:
      * 	   Up
      * 	   Down
      * 	   Unknown
      * 	   Dormant
      * 	   NotPresent
      * 	   LowerLayerDown
      * 	   Error (OPTIONAL)
      *
      * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
      * 	if there is a fault condition on the interface).
      *
      * 	This parameter is based on ifOperStatus from [RFC2863].
      *
      */
    int get_Device_WiFi_Radio_SupportedStandards(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
      * 	@brief get_Device_WiFi_Radio_Status
      *
      * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
      * 	Enumeration of:
      * 	   Up
      * 	   Down
      * 	   Unknown
      * 	   Dormant
      * 	   NotPresent
      * 	   LowerLayerDown
      * 	   Error (OPTIONAL)
      *
      * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
      * 	if there is a fault condition on the interface).
      *
      * 	This parameter is based on ifOperStatus from [RFC2863].
      *
      */
    int get_Device_WiFi_Radio_OperatingStandards(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
      * 	@brief get_Device_WiFi_Radio_Status
      *
      * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
      * 	Enumeration of:
      * 	   Up
      * 	   Down
      * 	   Unknown
      * 	   Dormant
      * 	   NotPresent
      * 	   LowerLayerDown
      * 	   Error (OPTIONAL)
      *
      * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
      * 	if there is a fault condition on the interface).
      *
      * 	This parameter is based on ifOperStatus from [RFC2863].
      *
      */
    int set_Device_WiFi_Radio_OperatingStandards(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
      * 	@brief get_Device_WiFi_Radio_Status
      *
      * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
      * 	Enumeration of:
      * 	   Up
      * 	   Down
      * 	   Unknown
      * 	   Dormant
      * 	   NotPresent
      * 	   LowerLayerDown
      * 	   Error (OPTIONAL)
      *
      * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
      * 	if there is a fault condition on the interface).
      *
      * 	This parameter is based on ifOperStatus from [RFC2863].
      *
      */
    int get_Device_WiFi_Radio_PossibleChannels(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
      * 	@brief get_Device_WiFi_Radio_Status
      *
      * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
      * 	Enumeration of:
      * 	   Up
      * 	   Down
      * 	   Unknown
      * 	   Dormant
      * 	   NotPresent
      * 	   LowerLayerDown
      * 	   Error (OPTIONAL)
      *
      * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
      * 	if there is a fault condition on the interface).
      *
      * 	This parameter is based on ifOperStatus from [RFC2863].
      *
      */
    int get_Device_WiFi_Radio_ChannelsInUse(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
      * 	@brief get_Device_WiFi_Radio_Status
      *
      * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
      * 	Enumeration of:
      * 	   Up
      * 	   Down
      * 	   Unknown
      * 	   Dormant
      * 	   NotPresent
      * 	   LowerLayerDown
      * 	   Error (OPTIONAL)
      *
      * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
      * 	if there is a fault condition on the interface).
      *
      * 	This parameter is based on ifOperStatus from [RFC2863].
      *
      */
    int get_Device_WiFi_Radio_Channel(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
      * 	@brief get_Device_WiFi_Radio_Status
      *
      * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
      * 	Enumeration of:
      * 	   Up
      * 	   Down
      * 	   Unknown
      * 	   Dormant
      * 	   NotPresent
      * 	   LowerLayerDown
      * 	   Error (OPTIONAL)
      *
      * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
      * 	if there is a fault condition on the interface).
      *
      * 	This parameter is based on ifOperStatus from [RFC2863].
      *
      */
    int set_Device_WiFi_Radio_Channel(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
      * 	@brief get_Device_WiFi_Radio_Status
      *
      * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
      * 	Enumeration of:
      * 	   Up
      * 	   Down
      * 	   Unknown
      * 	   Dormant
      * 	   NotPresent
      * 	   LowerLayerDown
      * 	   Error (OPTIONAL)
      *
      * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
      * 	if there is a fault condition on the interface).
      *
      * 	This parameter is based on ifOperStatus from [RFC2863].
      *
      */
    int get_Device_WiFi_Radio_AutoChannelSupported(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
      * 	@brief get_Device_WiFi_Radio_Status
      *
      * 	The current operational state of the radio (see [Section 4.2.2/TR-181i2]).
      * 	Enumeration of:
      * 	   Up
      * 	   Down
      * 	   Unknown
      * 	   Dormant
      * 	   NotPresent
      * 	   LowerLayerDown
      * 	   Error (OPTIONAL)
      *
      * 	When Enable is false then Status SHOULD normally be Down (or NotPresent or Error
      * 	if there is a fault condition on the interface).
      *
      * 	This parameter is based on ifOperStatus from [RFC2863].
      *
      */
    int get_Device_WiFi_Radio_AutoChannelEnable(HOSTIF_MsgData_t *stMsgData,int radioIndex );
    int set_Device_WiFi_Radio_AutoChannelEnable(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    int get_Device_WiFi_Radio_AutoChannelRefreshPeriod(HOSTIF_MsgData_t *stMsgData,int radioIndex );
    int set_Device_WiFi_Radio_AutoChannelRefreshPeriod(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    int get_Device_WiFi_Radio_OperatingChannelBandwidth(HOSTIF_MsgData_t *stMsgData,int radioIndex );
    int set_Device_WiFi_Radio_OperatingChannelBandwidth(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    int get_Device_WiFi_Radio_ExtensionChannel(HOSTIF_MsgData_t *stMsgData,int radioIndex );
    int set_Device_WiFi_Radio_ExtensionChannel(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    int get_Device_WiFi_Radio_GuardInterval(HOSTIF_MsgData_t *stMsgData,int radioIndex );
    int set_Device_WiFi_Radio_GuardInterval(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    int get_Device_WiFi_Radio_MCS(HOSTIF_MsgData_t *stMsgData,int radioIndex );
    int set_Device_WiFi_Radio_MCS(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    int get_Device_WiFi_Radio_TransmitPowerSupported(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    int get_Device_WiFi_Radio_TransmitPower(HOSTIF_MsgData_t *stMsgData,int radioIndex );
    int set_Device_WiFi_Radio_TransmitPower(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    int get_Device_WiFi_Radio_IEEE80211hSupported(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    int get_Device_WiFi_Radio_IEEE80211hEnabled(HOSTIF_MsgData_t *stMsgData,int radioIndex );
    int set_Device_WiFi_Radio_IEEE80211hEnabled(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    int get_Device_WiFi_Radio_RegulatoryDomain(HOSTIF_MsgData_t *stMsgData,int radioIndex );
    int set_Device_WiFi_Radio_RegulatoryDomain(HOSTIF_MsgData_t *stMsgData,int radioIndex );
};

/* End of TR069_HOSTIF_DEVICE_WIFI_RADIO_H_ doxygen group */
/**
 * @}
 */

#endif /* HOSTIF_DEVICE_WIFI_RADIO_H_ */


/** @} */
/** @} */
