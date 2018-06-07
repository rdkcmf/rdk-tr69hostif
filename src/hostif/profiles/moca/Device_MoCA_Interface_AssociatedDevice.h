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
 * @file Device_MoCA_Interface_AssociatedDevice.h
 *
 * TR-069 Device.Moca.Interface.AssociatedDevice object Public API.
 */

/**
 * @defgroup TR69_HOSTIF_MOCA_INTERFACE_AD TR-069 Object (Device.MoCA.Interface.{i}.AssociatedDevice.{i}) Public APIs
 * The object Device.MoCA.Interface.{i}.AssociatedDevice.{i}. provides the table contains information about
 * other MoCA devices currently associated with this MoCA interface..
 * @note At most one entry in this table can exist with a given value for MACAddress.
 *
 * @ingroup TR69_HOSTIF_MOCA
 */



/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef HOSTIF_MOCA_INTERFACE_ASSOCIATEDDEVICE_H_
#define HOSTIF_MOCA_INTERFACE_ASSOCIATEDDEVICE_H_

#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "Device_MoCA_Interface.h"

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/
#define MAC_ADDRESS_LENGTH 24
#define VERSION_LENGTH 64


/** @defgroup TR_069_DEVICE_MOCA_INTERFACE_ASSOCIATEDDEVICE_API TR-069 Device.Moca.Interface.AssociatedDevice object API.
 *  @ingroup TR_069_DEVICE_MOCA_INTERFACE_API
 *
 *  The Device.Moca.Interface.AssociatedDevice object table contains information about
 *  other MoCA devices currently associated with this MoCA interface.
 *
 *  @note    At most one entry in this table can exist with a given value for MACAddress.
 *
 */

/** @addtogroup TR_069_DEVICE_MOCA_INTERFACE_ASSOCIATEDDEVICE_GETTER_API TR-069 Device.Moca.Interface.AssociatedDevice Getter API.
 *  @ingroup TR_069_DEVICE_MOCA_INTERFACE_ASSOCIATEDDEVICE_API
 *
 *  \section dev_moca_if_assocdev_getter TR-069 Device.MoCA.Interface.AssociatedDevice object Getter API
 *
 *  This is the getter group of API for the <b>Device.MoCA.Interface.{i}.AssociatedDevice.{j}</b> object.
 *
 *  The interface for all functions is identical and is described here.
 *
 *  @param[in]   *name     This is the complete path name of the parameter extracted from
 *                         soap message, e.g. ::get_Device_MoCA_Interface_AssociatedDevice_MACAddress.
 *                         In this case, the path is "Device.MoCA.Interface.{i}.AssociatedDevice.{j}.MACAddress".
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
class MoCAInterfaceAssociatedDevice {

    int dev_id;
    static class MoCAInterfaceAssociatedDevice *Instance;

    MoCAInterfaceAssociatedDevice(int dev_id);
    ~MoCAInterfaceAssociatedDevice();

    char            m_i8MacAddress[MAC_ADDRESS_LENGTH];
    unsigned int    m_ui32NodeId;
    bool            m_bPreferredNC;
    char            m_i8HighestVersion[VERSION_LENGTH];
    unsigned int    m_ui32PhyTxRate;
    unsigned int    m_ui32PhyRxRate;
    unsigned int    m_ui32TxPowerControlReduction;
    int             m_i32RxPowerLevel;
    unsigned int    m_ui32TxBcastRate;
    int             m_i32RxBcastPowerLevel;
    unsigned int    m_ui32TxPackets;
    unsigned int    m_ui32RxPackets;
    unsigned int    m_ui32RxErroredAndMissedPackets;
    bool            m_bQAM256Capable;
    unsigned int    m_ui32PacketAggregationCapability;
    unsigned int    m_ui32RxSNR;
    bool            m_bActive;

public:

    static class MoCAInterfaceAssociatedDevice *getInstance();
    static void closeInstance();

    /**
     * @ingroup TR69_HOSTIF_MOCA_INTERFACE_AD
     * @{
     */

    /**
     * @brief Get the MAC address of the associated device's MoCA interface.
     *
     * This function provides the output as a string available
     * in Device.MoCA.Interface.{i}.AssociatedDevice.{i}.MACAddress parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_MACAddress(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

    /**
     * @brief Get the Node ID of this remote device.
     *
     * This parameter is based on mocaNodeIndex from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available
     * in Device.MoCA.Interface.{i}.AssociatedDevice.{i}.NodeID parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_NodeID(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

    /**
     * @brief Check whether the MoCA remote device is a preferred Network Coordinator (NC).
     *
     * This parameter is based on mocaNodePreferredNC from [MOCA11-MIB].
     *
     * This function provides the output as true/false value based
     * on Device.MoCA.Interface.{i}.AssociatedDevice.{i}.PreferredNC status.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_PreferredNC(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

    /**
     * @brief Get the identifies the highest MoCA version that this remote device supports.
     *
     * This element MUST be in the form of dot-delimited integers, where each successive integer
     * represents a more minor category of variation. For example, 1.0, where the components mean
     * major.minor revision number. Possible patterns:
     * \\d+(\\.\\d+)*
     *
     * This parameter is based on mocaNodeMocaVersion from [MOCA11-MIB].
     *
     * This function provides the output as a string value available
     * in Device.MoCA.Interface.{i}.AssociatedDevice.{i}.HighestVersion parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_HighestVersion(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

    /**
     * @brief Get the PHY transmit rate (in Mbps) to this remote device.
     *
     * This parameter is based on mocaMeshTxRate from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available
     * in Device.MoCA.Interface.{i}.AssociatedDevice.{i}.PHYTxRate parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_PHYTxRate(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

    /**
     * @brief Get the PHY receive rate (in Mbps) from this remote device.
     *
     * This function provides the output as a numeric value available
     * in Device.MoCA.Interface.{i}.AssociatedDevice.{i}.PHYRxRate parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_PHYRxRate(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

    /**
     * @brief Get the reduction in transmitter level (in dB) due to power control.
     *
     * This parameter is based on mocaNodeTxPowerReduction from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available
     * in Device.MoCA.Interface.{i}.AssociatedDevice.{i}.TxPowerControlReduction parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_TxPowerControlReduction(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

    /**
     * @brief Get the power level (in dBm) received at the MoCA interface from this remote device.
     *
     * This parameter is based on mocaNodeRxPower from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available
     * in Device.MoCA.Interface.{i}.AssociatedDevice.{i}.RxPowerLevel parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_RxPowerLevel(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

    /**
     * @brief Get the broadcast PHY transmit rate (in Mbps) from this remote device.
     *
     * This parameter is based on mocaNodeTxGcdRate from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available
     * in Device.MoCA.Interface.{i}.AssociatedDevice.{i}.TxBcastRate parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_TxBcastRate(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

    /**
     * @brief Get the power level (in dBm) received at the MoCA interface from this remote device.
     *
     * This parameter is based on mocaNodeRxGcdPower from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available
     * in Device.MoCA.Interface.{i}.AssociatedDevice.{i}.RxBcastPowerLevel parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_RxBcastPowerLevel(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

    /**
     * @brief Get the number of packets transmitted to this remote device (Note: Includes Broadcast, Multicast and Unicast packets).
     *
     * This function provides the output as a numerc value available
     * in Device.MoCA.Interface.{i}.AssociatedDevice.{i}.TxPackets parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_TxPackets(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

    /**
     * @brief Get the number of packets received from this remote device (Note: Includes Broadcast, Multicast and Unicast packets).
     *
     * This parameter is based on mocaNodeRxPackets from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available
     * in Device.MoCA.Interface.{i}.AssociatedDevice.{i}.RxPackets parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_RxPackets(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

    /**
     * @brief Get the number of errored and missed packets received from this remote device.
     *
     * This parameter is based on mocaNodeRxDrops from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available
     * in Device.MoCA.Interface.{i}.AssociatedDevice.{i}.RxErroredAndMissedPackets parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_RxErroredAndMissedPackets(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

    /**
     * @brief Check whether this remote device supports the 256 QAM feature.
     *
     * This parameter is based on mocaNodeQAM256Capable from [MOCA11-MIB].
     *
     * This function provides the output as a true/false value based
     * on Device.MoCA.Interface.{i}.AssociatedDevice.{i}.QAM256Capable parameter status.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_QAM256Capable(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

    /**
     * @brief Get the packet aggregation capability supported by the remote device. Standard values are 0
     * (no support), 6 (6 packets) or 10 (10 packets).
     *
     * This function provides the output as a array of integer value available
     * in Device.MoCA.Interface.{i}.AssociatedDevice.{i}.PacketAggregationCapability parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_PacketAggregationCapability(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

    /**
     * @brief Get the signal to noise level (in dBm) received at this interface from this remote device.
     *
     * This parameter is based on mocaNodeSNR from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available
     * in Device.MoCA.Interface.{i}.AssociatedDevice.{i}.RxSNR parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_RxSNR(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

    /**
     * @brief Check whether or not this remote device is currently present in the MoCA network.
     *
     * The ability to list inactive nodes is OPTIONAL. If the CPE includes inactive nodes in this table,
     * Active MUST be set to false for each inactive node. The length of time an inactive node remains listed in this
     * table is a local matter to the CPE.
     *
     * This function provides the output as true/false value based
     * on Device.MoCA.Interface.{i}.AssociatedDevice.{i}.Active parameter status.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_Active(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

    /** @} */ //End of Doxygen Tag TR69_HOSTIF_MOCA_INTERFACE_AD
};

/* End of TR_069_DEVICE_MOCA_INTERFACE_ASSOCIATEDDEVICE_GETTER_API doxygen group */
/**
 * @}
 */

#endif /* HOSTIF_MOCA_INTERFACE_ASSOCIATEDDEVICE_H_ */


/** @} */
/** @} */
