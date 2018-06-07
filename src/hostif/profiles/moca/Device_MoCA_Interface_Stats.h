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
 * @file Device_MoCA_Interface_Stats.h
 *
 * @brief TR-069 Device.Moca.Interface.Stats object Public API.
 */

/**
 * @defgroup TR69_HOSTIF_MOCA_INTERFACE_STATS TR-069 Object (Device.MoCA.Interface.{i}.Stats) Public APIs
 *
 * Provides Throughput statistics for this interface.
 *
 * The CPE MUST reset the MoCA interface's Stats parameters (unless otherwise stated in individual object or
 * parameter descriptions) either when the MoCA interface becomes operationally down due to a previous
 * administrative down (i.e. the MoCA interface's Status parameter transitions to a down state after the
 * interface is disabled) or when the MoCA interface becomes administratively up (i.e. the MoCA interface's
 * Enable parameter transitions from false to true). Administrative and operational interface status
 * is discussed in [Section 4.2.2/TR-181i2].
 *
 * @ingroup TR69_HOSTIF_MOCA
 */




/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef HOSTIF_MOCA_INTERFACE_STATS_H_
#define HOSTIF_MOCA_INTERFACE_STATS_H_

#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "Device_MoCA_Interface.h"


/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/

/** @defgroup TR_069_DEVICE_MOCA_INTERFACE_STATS_API TR-069 Device.Moca.Interface.Stats object API.
 *  @ingroup TR_069_DEVICE_MOCA_INTERFACE_API
 *
 *  The Device.Moca.Interface.Stats object provides throughput statistics for this interface.
 *
 *  @note    The CPE MUST reset the interface's Stats parameters (unless otherwise stated
 *           in individual object or parameter descriptions) either when the interface
 *           becomes operationally down due to a previous administrative down (i.e. the
 *           interface's Status parameter transitions to a down state after the interface
 *           is disabled) or when the interface becomes administratively up (i.e. the
 *           interface's Enable parameter transitions from <tt>false</tt> to <tt>true</tt>).
 */

/** @addtogroup TR_069_DEVICE_MOCA_INTERFACE_STATS_GETTER_API TR-069 Device.Moca.Interface.Stats Getter API.
 *  @ingroup TR_069_DEVICE_MOCA_INTERFACE_STATS_API
 *
 *  \section dev_moca_if_stats_getter TR-069 Device.MoCA.Interface.Stats object Getter API
 *
 *  This is the getter group of API for the <b>Device.MoCA.Interface.{i}.Stats</b> object.
 *
 *  The interface for all functions is identical and is described here.
 *
 *  @param[in]   *name     This is the complete path name of the parameter extracted from
 *                         soap message, e.g. ::get_Device_MoCA_Interface_Stats_BytesSent.
 *                         In this case, the path is "Device.MoCA.Interface.{i}.Stats.BytesSent".
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



class MoCAInterfaceStats {


    int                     m_i32DevID;
    unsigned long           m_ulBytesSent;
    unsigned long           m_ulBytesReceived;
    unsigned long           m_ulPacketsSent;
    unsigned long           m_ulPacketsReceived;
    unsigned int            m_ulErrorsSent;
    unsigned int            m_ulErrorsReceived;
    unsigned long           m_ulUnicastPacketsSent;
    unsigned long           m_ulUnicastPacketsReceived;
    unsigned int            m_ulDiscardPacketsSent;
    unsigned int            m_ulDiscardPacketsReceived;
    unsigned long           m_ulMulticastPacketsSent;
    unsigned long           m_ulMulticastPacketsReceived;
    unsigned long           m_ulBroadcastPacketsSent;
    unsigned long           m_ulBroadcastPacketsReceived;
    unsigned int            m_ui32UnknownProtoPacketsReceived;

    static class MoCAInterfaceStats *pInstance;
    ~MoCAInterfaceStats() {};
    MoCAInterfaceStats(int _dev_id);

public:

    static class MoCAInterfaceStats *getInstance();

    /**
     * @ingroup TR69_HOSTIF_MOCA_INTERFACE_STATS
     * @{
     */
    /**
     * @brief Get the total number of bytes transmitted out of the MoCA interface, including framing characters.
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.Stats.BytesSent parameter.
     *
     * @param[in] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_BytesSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

    /**
     * @brief Get the total number of bytes received on the MoCA interface, including framing characters.
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.Stats.BytesReceived parameter.
     *
     * @param[in] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_BytesReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

    /**
     * @brief Get the total number of packets transmitted out of the MoCA interface.
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.Stats.PacketsSent parameter.
     *
     * @param[in] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_PacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

    /**
     * @brief Get the total number of packets received on the MoCA interface.
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.Stats.PacketsReceived parameter.
     *
     * @param[in] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_PacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

    /**
     * @brief Get the total number of outbound packets that could not be transmitted because of errors.
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.Stats.ErrorsSent parameter.
     *
     * @param[in] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_ErrorsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

    /**
     * @brief Get the total number of inbound packets that contained errors preventing them from
     * being delivered to a higher-layer protocol.
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.Stats.ErrorsReceived parameter.
     *
     * @param[in] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_ErrorsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

    /**
     * @brief Get the total number of packets requested for transmission which were not addressed to a multicast or
     * broadcast address at this layer, including those that were discarded or not sent.
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.Stats.UnicastPacketsSent parameter.
     *
     * @param[in] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_UnicastPacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

    /**
     * @brief Get the total number of received packets, delivered by this layer to a higher layer,
     * which were not addressed to a multicast or broadcast address at this layer.
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.Stats.UnicastPacketsReceived parameter.
     *
     * @param[in] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_UnicastPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

    /**
     * @brief Get the total number of outbound packets which were chosen to be discarded even though no
     * errors had been detected to prevent their being transmitted. One possible reason for discarding such a
     * packet could be to free up buffer space.
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.Stats.DiscardPacketsSent parameter.
     *
     * @param[in] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_DiscardPacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

    /**
     * @brief Get the total number of inbound packets which were chosen to be discarded even though no errors
     * had been detected to prevent their being delivered. One possible reason for discarding such a packet
     * could be to free up buffer space.
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.Stats.DiscardPacketsReceived parameter.
     *
     * @param[in] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_DiscardPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

    /**
     * @brief Get the total number of packets that higher-level protocols requested for transmission and which were
     * addressed to a multicast address at this layer, including those that were discarded or not sent.
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.Stats.MulticastPacketsSent parameter.
     *
     * @param[in] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_MulticastPacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

    /**
     * @brief Get the total number of received packets, delivered by this layer to a higher layer,
     * which were addressed to a multicast address at this layer.
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.Stats.MulticastPacketsReceived parameter.
     *
     * @param[in] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_MulticastPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

    /**
     * @brief Get the total number of packets that higher-level protocols requested for transmission and which were
     * addressed to a broadcast address at this layer, including those that were discarded or not sent.
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.Stats.BroadcastPacketsSent parameter.
     *
     * @param[in] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_BroadcastPacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

    /**
     * @brief Get the total number of received packets, delivered by this layer to a higher layer,
     * which were addressed to a broadcast address at this layer.
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.Stats.BroadcastPacketsReceived parameter.
     *
     * @param[in] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_BroadcastPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

    /**
     * @brief Get the total number of packets received via the MoCA interface which were discarded because
     * of an unknown or unsupported protocol.
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.Stats.UnknownProtoPacketsReceived parameter.
     *
     * @param[in] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_UnknownProtoPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

    /** @} */ // End of Doxygen tag TR69_HOSTIF_MOCA_INTERFACE_STATS

};
/* End of TR_069_DEVICE_MOCA_INTERFACE_STATS_GETTER_API doxygen group */
/**
 * @}
 */


#endif /* HOSTIF_MOCA_INTERFACE_STATS_H_ */


/** @} */
/** @} */
