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
 * @file Device_MoCA_Interface.h
 * TR-069 Device.MoCA.Interface object Public API.
 */

/**
 * @defgroup TR69_HOSTIF_MOCA TR-069 Object (Device.MoCA)
 * MoCA object that contains the Interface table [MoCAv1.0] [MoCAv1.1].
 *
 * @par About TR-069 Object Device.MoCA.Interface.{i}.
 * @n
 * MoCA interface table (a stackable interface object as described in [Section 4.2/TR-181i2]).
 * Each table entry models the PHY and MAC levels of a MoCA interface [MoCAv1.0] [MoCAv1.1].
 * @n
 * At most one entry in this table can exist with a given value for Alias, or with a given value for Name.
 *
 * @par About TR-069 Object Device.MoCA.Interface.{i}.Stats.
 * @n
 * Throughput statistics for this interface.
 * @n
 * The CPE MUST reset the interface's Stats parameters (unless otherwise stated in individual object
 * or parameter descriptions) either when the interface becomes operationally down due to a previous
 * administrative down (i.e. the interface's Status parameter transitions to a down state after
 * the interface is disabled) or when the interface becomes administratively up (i.e. the interface's
 * Enable parameter transitions from false to true). Administrative and operational interface status
 * is discussed in [Section 4.2.2/TR-181i2].
 *
 * @par About TR-069 Object Device.MoCA.Interface.{i}.QoS.
 * @n
 * The QoS object provides information on MoCA parameterized QoS for this interface [MoCAv1.1].
 *
 * @par About TR-069 Object Device.MoCA.Interface.{i}.QoS.FlowStats.{i}.
 * @n
 * The flow statistics table provides information on the MoCA parameterized QoS flows this interface has
 * allocated onto the MoCA network.
 * @n
 * At most one entry in this table can exist with a given value for FlowID.
 *
 * @par About TR-069 Object Device.MoCA.Interface.{i}.AssociatedDevice.{i}.
 * @n
 * This table contains information about other MoCA devices currently associated with this MoCA interface.
 * @n At most one entry in this table can exist with a given value for MACAddress.
 *
 * @ingroup TR69_HOSTIF_PROFILE
 *
 * @defgroup TR69_HOSTIF_MOCA_INTERFACE TR-069 Object (Device.MoCA.) Public APIs
 * MoCA interface table (a stackable interface object as described in [Section 4.2/TR-181i2]).
 * Each table entry models the PHY and MAC levels of a MoCA interface [MoCAv1.0] [MoCAv1.1].
 * @n
 * At most one entry in this table can exist with a given value for Alias, or with a given value for Name.
 *
 * @ingroup TR69_HOSTIF_MOCA
 */




/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef HOSTIF_MOCA_INTERFACE_H_
#define HOSTIF_MOCA_INTERFACE_H_

#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "hostIf_updateHandler.h"

#define S_LENGTH 24
#define N_LENGTH 64
#define MAC_LENGTH 24
#define LL_LENGTH 1024
#define FREQ_LENGTH 9


class MoCADevice {
    static class MoCADevice *Instance;

    MoCADevice() {};
    ~MoCADevice() {};
    static std::mutex m_mutex;
public:
    static class MoCADevice *getInstance();
    static void closeInstance();

    static void* getRmhContext();
    static void* closeRmhContext();
};

class MoCAInterface {

    int 			m_i32Dev_id;
    int 			m_i32CurNumOfDevices;
    bool 			m_bEnable;
    char 			m_i8Status[S_LENGTH];
    char 			m_i8Alias[N_LENGTH];
    char 			m_i8Name[N_LENGTH];
    unsigned int 	m_ui32LastChange;
    char 			m_i8LowerLayers[LL_LENGTH];
    bool 			m_bUpStream;
    char 			m_i8MacAddress[MAC_LENGTH];
    char 			m_i8FirmwareVersion[N_LENGTH];
    unsigned int 	m_ui32MaxBitRate;
    unsigned int 	m_ui32MaxIngressBW;
    unsigned int 	m_ui32MaxEgressBW;
    unsigned int 	m_HighestVersion;
    char 			m_i8HighestVersion[N_LENGTH];
    char 			m_i8currentVersion[N_LENGTH];
    unsigned int 	m_ui32NetworkCoordinator;
    unsigned int 	m_ui32NodeId;
    bool 			m_bMaxNodes;
    bool 			m_bPreferredNC;
    unsigned int 	m_ui32BackupNC;
    bool 			m_bPrivacyEnabledSetting;
    bool 			m_bPrivacyEnabled;
    char 			m_i8FreqCapabilityMask[FREQ_LENGTH];
    char 			m_i8FreqCurrentMaskSetting[FREQ_LENGTH];
    char 			m_i8FreqCurrentMask[FREQ_LENGTH];
    unsigned int 	m_ui32CurrentOperFreq;
    unsigned int 	m_ui32LastOperFreq;
    char 			m_i8KeyPassphrase[S_LENGTH];
    unsigned int 	m_ui32TxPowerLimit;
    unsigned int 	m_ui32PowerCntlPhyTarget;
    unsigned int 	m_ui32BeaconPowerLimit;
    char 			m_i8NetworkTabooMask[FREQ_LENGTH];
    char 			m_i8NodeTabooMask[FREQ_LENGTH];
    unsigned int 	m_ui32TxBcastRate;
    unsigned int 	m_ui32TxBcastPowerReduction;
    bool 			m_bQAM256Capable;
    unsigned int 	m_ui32PacketAggregationCapability;
    unsigned int 	m_ui32AssociatedDeviceNumberOfEntries;
    unsigned int 	m_ui32X_RDKCENTRAL_COM_MeshTableNumberOfEntries;

    static GHashTable  	*ifHash;
    static GMutex 		*m_mutex;
    static GHashTable  	*m_notifyHash;

    MoCAInterface(int dev_id);
    ~MoCAInterface() {};

    int get_Associated_Device_NumberOfEntries(int &numDevices );
    int get_MoCA_Mesh_NumberOfEntries(int &numOfEntries );
    static MoCAInterface *Instance;

public:

    static MoCAInterface *getInstance(int _dev_id);

    static void closeInstance();

    static void getLock();

    static void releaseLock();

    static GHashTable* getNotifyHash();
    /** @defgroup TR_069_DEVICE_MOCA_API TR-069 Device.Moca object API.
     *  @ingroup TR_069_API
     *
     *  @todo    The Device.MoCA object is specified in the Xi3 data-model.xml file but
     *           has its API defined here.  According to the standard used, this should
     *           be in Device_MoCA.h
     */

    /** @addtogroup TR_069_DEVICE_MOCA_GETTER_API TR-069 Device.MoCA Getter API.
     *  @ingroup TR_069_DEVICE_MOCA_API
     *
     *  \section dev_moca_getter TR-069 Device.MoCA object Getter API
     *
     *  This is the getter group of API for the <b>Device.MoCA</b> object.
     *
     *  The interface for all functions is identical and is described here.
     *
     *  @param[in]   *name     This is the complete path name of the parameter extracted from
     *                         soap message, e.g. ::get_Device_MoCA_InterfaceNumberOfEntries.
     *                         In this case, the path is "Device.MoCA.InterfaceNumberOfEntries".
     *  @param[in]   *type     Data type of parameter defined for TR-069. This is same as the
     *                         data type used in the Xi3 data-model.xml file.
     *                         (see parameter.h)
     *  @param[out]  *value    This is the value of the parameter requested by the ACS.
     *                         (see paramaccess.h)
     *
     *  @return The status of the operation.
     *  @retval OK            If parameter requested was successfully fetched. (Same as <b>NO_ERROR</b>).
     *  @retval NO_ERROR      If parameter requested was successfully fetched. (Same as <b>OK</b>).
     *  @retval DIAG_ERROR    Diagnostic error.
     *  @retval ERR_???       Appropriate error value otherwise (see dimark_globals.h).
     *
     *  @todo Clarify description of DIAG_ERROR.
     *
     *  @{
     */

    /**
     * @brief    Get the number of entries in the MoCA Interface table.
     *
     * This function provides the number of entries in the MoCA Interface table.
     *
     * See @ref dev_moca_getter
     *
     */

    static int get_InterfaceNumberOfEntries(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @ingroup TR69_HOSTIF_MOCA_INTERFACE
     * @{
     */
    /**
     * @brief Get the MoCA status for BootStatus.
    *
    * This function provides the infromation about:
    * - Coax connection confirmed, MoCA enabled
    * - Discovering MoCA Network Coordinator: MoCA MAC: xx:xx:xx:xx:xx:xx
    * - Joining MoCA Network
    */
    int check_MoCABootStatus(char *);

    /**
     * @brief Get the status (enabled/disabled) of the MoCA Interface.
     *
     * This function provides true/false value based on the Device.MoCA.Interface.{i}.Enable status.
     *
     * @param[in] stMsgData TR-069 Host interface message request.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_Enable(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the current operational state of the interface (see [Section 4.2.2/TR-181i2]). Enumeration of:
     *
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
     * - When Enable is changed to true then Status SHOULD change to Up if and only if the interface is able to
     * transmit and receive network traffic;
     * -It SHOULD change to Dormant if and only if the interface is operable but is waiting for external actions
     * before it can transmit and receive network traffic (and subsequently change to Up if still operable when
     * the expected actions have completed);
     * It SHOULD change to LowerLayerDown if and only if the interface is prevented from entering the Up state
     * because one or more of the interfaces beneath it is down;
     * It SHOULD remain in the Error state if there is an error or other fault condition detected on the interface;
     * it SHOULD remain in the NotPresent state if the interface has missing (typically hardware) components;
     * It SHOULD change to Unknown if the state of the interface can not be determined for some reason.
     *
     * This parameter is based on ifOperStatus from [RFC2863].
     *
     * This function provides the output as a string available in Device.MoCA.Interface.{i}.Status parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_Status(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the Alias-based addressing used for MoCA interface.
     *
     * A non-volatile handle used to reference this instance. Alias provides a mechanism for an ACS to label this
     * instance for future reference.
     *
     * If the CPE supports the Alias-based Addressing feature as defined in [Section 3.6.1/TR-069a4] and described
     * in [Appendix II/TR-069a4], the following mandatory constraints MUST be enforced:
     *
     * - Its value MUST NOT be empty.
     * - Its value MUST start with a letter.
     * - If its value is not assigned by the ACS, it MUST start with a "cpe-" prefix.
     * - The CPE MUST NOT change the parameter value.
     *
     * This function provides the output as a string available in Device.MoCA.Interface.{i}.Alias parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_Alias(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the textual name of the interface as assigned by the CPE.
     *
     * This function provides the output as a string available in Device.MoCA.Interface.{i}.Name parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_Name(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the accumulated time in seconds since the interface entered its current operational state.
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.LastChange parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_LastChange(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the path name of the MoCA interface object.
     *
     * Comma-separated list (maximum list length 1024) of strings. Each list item MUST be the path name of an interface
     * object that is stacked immediately below this interface object.
     *
     * @note Since Interface is a layer 1 interface, it is expected that LowerLayers will not be used.
     *
     * This function provides the output as a string available in Device.MoCA.Interface.{i}.LowerLayers parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_LowerLayers(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Check whether the interface points towards the Internet (true) or towards End Devices (false).
     *
     * For example:
     * - For an Internet Gateway Device, Upstream will be true for all WAN interfaces and false for all LAN interfaces.
     *
     * For a standalone WiFi Access Point that is connected via Ethernet to an Internet Gateway Device,
     * Upstream will be true for the Ethernet interface and false for the WiFi Radio interface.
     *
     * For an End Device, Upstream will be true for all interfaces.
     *
     * This function provides true/false value based on the Device.MoCA.Interface.{i}.Upstream status.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_Upstream(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the MAC Address of the interface.
     *
     * @note This is not necessarily the same as the Ethernet header source or destination MAC address,
     * which is associated with the IP interface and is modeled via the Ethernet.Link.{i}.MACAddress parameter.
     *
     * This function provides the output as a string value available in Device.MoCA.Interface.{i}.MACAddress parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_MACAddress(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the MoCA interface's firmware version.
     *
     * This parameter is based on mocaIfSoftwareVersion from [MOCA11-MIB].
     *
     * This function provides the output as a string available in Device.MoCA.Interface.{i}.FirmwareVersion parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_FirmwareVersion(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the maximum MoCA PHY bit rate (expressed in Mbps).
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.FirmwareVersion parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_MaxBitRate(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the maximum bandwidth of this interface for flows onto the MoCA network in Mbps.
     *
     * This parameter is based on mocaIfMaxIngressNodeBw from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.MaxIngressBW parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_MaxIngressBW(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the maximum bandwidth of this interface for flows from the MoCA network in Mbps.
     *
     * This parameter is based on mocaIfMaxEgressNodeBw from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.MaxEgressBW parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_MaxEgressBW(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the identifies the highest MoCA version that this interface supports.
     *
     * This element MUST be in the form of dot-delimited integers, where each successive integer represents a
     * more minor category of variation. For example, 1.0, where the components mean major.minor revision number. Possible patterns:
     * - d+(.d+)*
     * This parameter is based on mocaIfMocaVersion from [MOCA11-MIB].
     *
     * This function provides the output as a string available in Device.MoCA.Interface.{i}.HighestVersion parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_HighestVersion(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the MoCA version that the MoCA network is currently running.
     *
     * This element MUST be in the form of dot-delimited integers, where each successive integer represents
     * a more minor category of variation. For example, 1.0, where the components mean major.minor revision number.
     * @n Possible patterns:
     * - \\d+(\\.\\d+)*
     *
     * This parameter is based on mocaIfNetworkVersion from [MOCA11-MIB].
     *
     * This function provides the output as a string available in Device.MoCA.Interface.{i}.CurrentVersion parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_CurrentVersion(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the Node ID of the current Network Coordinator (NC) for the MoCA network.
     *
     * This parameter is based on mocaIfNC from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.NetworkCoordinator parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_NetworkCoordinator(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the Node ID of the MoCA interface.
     *
     * This parameter is based on mocaIfNodeID from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.NodeID parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_NodeID(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the maximum network node capability supported by the interface.
     * If MaxNodes is true then the interface supports 16 nodes (the maximum for a MoCA 1.1 network).
     * If MaxNodes is false then the interface supports 8 nodes (the maximum for a MoCA 1.0 network).
     *
     * This function provides true/false value based on the Device.MoCA.Interface.{i}.MaxNodes status.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_MaxNodes(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Check Whether this interface is a preferred Network Coordinator (NC).
     *
     * This parameter is based on mocaIfPreferredNC from [MOCA11-MIB].
     *
     * This function provides true/false value based on the Device.MoCA.Interface.{i}.PreferredNC status.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_PreferredNC(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the Node ID of the backup Network Coordinator node.
     *
     * This parameter is based on mocaIfBackupNC from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.BackupNC parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_BackupNC(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the configured privacy mode. This indicates whether link-layer security is enabled (true)
     * or disabled (false) for network admission.
     *
     * The configured privacy setting MAY NOT match the current operational state (PrivacyEnabled),
     * since this setting is only applied during network formation or admission.
     *
     * This function provides true/false value based on the Device.MoCA.Interface.{i}.PrivacyEnabledSettting status.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_PrivacyEnabledSetting(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Check whether link-layer security is enabled or disabled.
     *
     * This parameter is based on mocaIfPrivacyEnable from [MOCA11-MIB].
     *
     * This function provides true/false value based on the Device.MoCA.Interface.{i}.PrivacyEnabled status.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_PrivacyEnabled(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the hexadecimal encoded 64-bit mask of supported frequencies.
     * This is the bit map of the spectrum that the interface supports, and each bit represents
     * 25 MHz of spectrum. The least significant bit of the rightmost character corresponds to 800MHz,
     * which is the lowest frequency.
     *
     * For example, an interface that supports 1150 MHz through 1500 MHz would have a value of 0x000000001FFFC000.
     *
     * This parameter is based on mocaIfCapabilityMask from [MOCA11-MIB].
     *
     * This function provides the output as hex binary value available in Device.MoCA.Interface.{i}.FreqCapabilityMask parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_FreqCapabilityMask(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the configured hexadecimal encoded 64-bit mask of enabled frequencies for network admission.
     *
     * The configured frequencies MAY NOT match the current operational state (FreqCurrentMask),
     * since this setting is only applied during network formation or admission.
     *
     * This function provides the output as a hex binary value available in Device.MoCA.Interface.{i}.FreqCapabilityMask parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_FreqCurrentMaskSetting(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the hexadecimal encoded 64-bit mask of used frequencies.
     * This is the bit map of the spectrum that can be used and is a subset of the FreqCapabilityMask.
     * Each bit represents 25 MHz of spectrum. The least significant bit of the rightmost character
     * corresponds to 800MHz, which is the lowest frequency.
     *
     * This parameter is based on mocaIfChannelMask from [MOCA11-MIB].
     *
     * This function provides the output as a hex binary value available in Device.MoCA.Interface.{i}.FreqCurrentMask parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_FreqCurrentMask(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the current Operational Frequency. The RF frequency in Hz to which the MoCA interface
     * is currently tuned. This parameter is only valid when Status is Up.
     *
     * This parameter is based on mocaIfRFChannel from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.CurrentOperFreq parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_CurrentOperFreq(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the last Operational Frequency. The RF frequency in Hz to which the MoCA interface was
     * tuned when last in the Up state.
     *
     * This parameter is based on mocaIfLOF from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.LastOperFreq parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_LastOperFreq(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the MoCA Password. The value consists of numeric characters (0-9).
     *
     * The possible patterns is:
     * - \\d+
     * This parameter is based on mocaIfPassword from [MOCA11-MIB].
     *
     * When read, this parameter returns an empty string, regardless of the actual value.
     *
     * This function provides the output as a string available in Device.MoCA.Interface.{i}.KeyPassphrase parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_KeyPassphrase(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the transmit Power attenuation in dB relative to the maximum transmit power.
     *
     * The MoCA interface SHOULD have Enable set to false for any change in this configuration.
     * If the parameter is modified when Enable is true then this change might take several minutes to complete.
     *
     * This parameter is based on mocaIfTxPowerLimit from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.TxPowerLimit parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_TxPowerLimit(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the target PHY rate in Mbps for the power control algorithm.
     *
     * The MoCA interface SHOULD have Enable set to false for any change in this configuration.
     * If the parameter is modified when Enable is true then this change might take several minutes to complete.
     *
     * This parameter is based on mocaIfPowerControlTargetRate from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.PowerCntlPhyTarget parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_PowerCntlPhyTarget(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the Beacon Transmit Power attenuation in dB relative to the maximum transmit power.
     *
     * The MoCA interface SHOULD have Enable set to false for any change in this configuration.
     * If the parameter is modified when Enable is true then this change might take several minutes to complete.
     *
     * This parameter is based on mocaIfBeaconPowerLimit from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.BeaconPowerLimit parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_BeaconPowerLimit(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the hexadecimal encoded 64-bit mask of MoCA taboo channels identified for the home network.
     * This is the bit map of the spectrum that the interface supports, and each bit represents 25 MHz
     * of spectrum. The least significant bit of the rightmost character corresponds to 800MHz, which is the lowest frequency.
     *
     * This parameter is based on mocaIfTabooChannelMask from [MOCA11-MIB].
     *
     * This function provides the output as a hex binary value available in Device.MoCA.Interface.{i}.NetworkTabooMask parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_NetworkTabooMask(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the hexadecimal encoded 64-bit mask of supported frequencies.
     * This is the bit map of the spectrum that the interface supports, and each bit represents 25 MHz of spectrum.
     * The least significant bit of the rightmost character corresponds to 800MHz, which is the lowest frequency.
     *
     * This parameter is based on mocaIfNodeTabooChannelMask from [MOCA11-MIB].
     *
     * This function provides the output as a hex binary value available in Device.MoCA.Interface.{i}.NodeTabooMask parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_NodeTabooMask(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the broadcast PHY transmit rate in Mbps for this interface.
     *
     * This parameter is based on mocaIfTxGcdRate from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.TxBcastRate parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_TxBcastRate(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the transmit Power attenuation in dB relative to the maximum transmit power for broadcast transmissions.
     *
     * This parameter is based on mocaIfTxGcdPowerReduction from [MOCA11-MIB].
     *
     * This function provides the output as numeric value available in Device.MoCA.Interface.{i}.TxBcastPowerReduction parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_TxBcastPowerReduction(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Check whether this interface supports the 256 QAM feature.
     *
     * This parameter is based on mocaIfQAM256Capable from [MOCA11-MIB].
     *
     * This function provides true/false value based on the Device.MoCA.Interface.{i}.QAM256Capable parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_QAM256Capable(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief Get the packet aggregation capability supported by the interface.
     * Standard values are 0 (no support), 6 (6 packets) or 10 (10 packets).
     *
     * This parameter is based on mocaIfPacketsAggrCapability from [MOCA11-MIB].
     *
     * This function provides the output as a numeric values available in
     *  Device.MoCA.Interface.{i}.PacketAggregationCapability parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_PacketAggregationCapability(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief The number of entries in the AssociatedDevice table.
     *
     * This function provides the output as a numeric value available in
     * Device.MoCA.Interface.{i}.AssociatedDeviceNumberOfEntries parameter.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_AssociatedDeviceNumberOfEntries(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the number of entries in the X_RDKCENTRAL_COM_MeshTable table of a MoCA Interface.
     *
     * This function provides the number of entries in the X_RDKCENTRAL_COM_MeshTableNumberOfEntries table.
     *
     * See @ref dev_moca_if_getter
     *
     */
    int get_X_RDKCENTRAL_COM_MeshTableNumberOfEntries(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the X_RDKCENTRAL_COM_PrimaryChannelFreq of a MoCA Interface.
     *
     * This function provides the X_RDKCENTRAL_COM_PrimaryChannelFreq value.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     *
     */
    int get_X_RDKCENTRAL_COM_PrimaryChannelFreq(HOSTIF_MsgData_t *);
    int get_X_RDKCENTRAL_COM_SecondaryChannelFreq(HOSTIF_MsgData_t *);
    int get_X_RDKCENTRAL_COM_TurboMode(HOSTIF_MsgData_t *);
    int get_X_RDKCENTRAL_COM_NodePowerState(HOSTIF_MsgData_t *);
    /* End of TR_069_DEVICE_MOCA_GETTER_API doxygen group */
    /**
     * @}
     */

    /**
     * @brief Enables or disables the interface.
     *
     * This parameter is based on ifAdminStatus from [RFC2863].
     *
     * This function will update true/false value to Device.MoCA.Interface.{i}.Enable status based on the input.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int set_Enable(HOSTIF_MsgData_t *);

    /**
     * @brief Set the Alias-based addressing for MoCA interface.
     *
     * A non-volatile handle used to reference this instance. Alias provides a mechanism for an ACS to label
     * this instance for future reference.
     *
     * If the CPE supports the Alias-based Addressing feature as defined in [Section 3.6.1/TR-069a4] and
     * described in [Appendix II/TR-069a4], the following mandatory constraints MUST be enforced:
     *
     * - Its value MUST NOT be empty.
     * - Its value MUST start with a letter.
     * - If its value is not assigned by the ACS, it MUST start with a "cpe-" prefix.
     * - The CPE MUST NOT change the parameter value.
     *
     * This function will update the configuration variable Device.MoCA.Interface.{i}.Alias.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int set_Alias(HOSTIF_MsgData_t *);

    /**
     * @brief Set the MoCA Interface LowerLayers list.
     *
     * A comma-separated list (maximum list length 1024) of strings.
     * Set the instance handle for a MoCA Interface. Each list item MUST be the path name of an interface object
     * that is stacked immediately below this interface object.
     * If the referenced object is deleted, the corresponding item MUST be removed from the list. See [Section 4.2.1/TR-181i2].
     *
     * @note Since Interface is a layer 1 interface, it is expected that LowerLayers will not be used.
     *
     * This function will update a string to the configuration variable Device.MoCA.Interface.{i}.LowerLayers.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int set_LowerLayers(HOSTIF_MsgData_t *);

    /**
     * @brief Set the interface is a preferred Network Coordinator (NC).
     *
     * This parameter is based on mocaIfPreferredNC from [MOCA11-MIB].
     *
     * This function will update true/false value to Device.MoCA.Interface.{i}.PreferredNC status based on the input.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int set_PreferredNC(HOSTIF_MsgData_t *);

    /**
     * @brief Set the configured privacy mode. This indicates whether link-layer security is enabled (true)
     * or disabled (false) for network admission.
     *
     * The configured privacy setting MAY NOT match the current operational state (PrivacyEnabled),
     * since this setting is only applied during network formation or admission.
     *
     * This function will update true/false value to Device.MoCA.Interface.{i}.PrivacyEnabledSetting
     * status based on the input.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int set_PrivacyEnabledSetting(HOSTIF_MsgData_t *);

    /**
     * @brief Set the configured hexadecimal encoded 64-bit mask of enabled frequencies for network admission.
     *
     * The configured frequencies MAY NOT match the current operational state (FreqCurrentMask), since this
     * setting is only applied during network formation or admission.
     *
     * This function will update the hex binary value into the configuration variable
     * Device.MoCA.Interface.FreqCurrentMaskSetting.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int set_FreqCurrentMaskSetting(HOSTIF_MsgData_t *);

    /**
     * @brief Set the transmit Power attenuation in dB relative to the maximum transmit power.
     *
     * The MoCA interface SHOULD have Enable set to false for any change in this configuration.
     * If the parameter is modified when Enable is true then this change might take several minutes to complete.
     *
     * This parameter is based on mocaIfTxPowerLimit from [MOCA11-MIB].
     *
     * This function will update with a numeric value to the configuration variable
     * Device.MoCA.Interface.TxPowerLimit.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int set_TxPowerLimit(HOSTIF_MsgData_t *);

    /**
     * @brief Set the target PHY rate in Mbps for the power control algorithm.
     *
     * The MoCA interface SHOULD have Enable set to false for any change in this configuration.
     * If the parameter is modified when Enable is true then this change might take several minutes to complete.
     *
     * This parameter is based on mocaIfPowerControlTargetRate from [MOCA11-MIB].
     *
     * This function will update with a numeric value to the configuration variable
     * Device.MoCA.Interface.PowerCntlPhyTarget.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int set_PowerCntlPhyTarget(HOSTIF_MsgData_t *);

    /**
     * @brief Set the MoCA Password.
     * The value consists of numeric characters (0-9). Possible patterns:
     *
     * - \\d+
     *
     * This parameter is based on mocaIfPassword from [MOCA11-MIB].
     *
     * When read, this parameter returns an empty string, regardless of the actual value.
     *
     * This function will update with a string value to the configuration variable
     * Device.MoCA.Interface.KeyPassphrase.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int set_KeyPassphrase(HOSTIF_MsgData_t *);

    /**
     * @brief Set the Beacon Transmit Power attenuation in dB relative to the maximum transmit power.
     *
     * The MoCA interface SHOULD have Enable set to false for any change in this configuration.
     * If the parameter is modified when Enable is true then this change might take several minutes to complete.
     *
     * This parameter is based on mocaIfBeaconPowerLimit from [MOCA11-MIB].
     *
     * This function will update with a numeric value to the configuration variable
     * Device.MoCA.Interface.BeaconPowerLimit.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int set_BeaconPowerLimit(HOSTIF_MsgData_t *);

    /** @} */ //End of Doxygen tag TR69_HOSTIF_MOCA_INTERFACE
};

#endif /* HOSTIF_MOCA_INTERFACE_H_ */


/** @} */
/** @} */
