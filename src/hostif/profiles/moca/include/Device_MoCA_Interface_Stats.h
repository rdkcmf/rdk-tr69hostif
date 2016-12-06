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
typedef struct Device_MoCA_Interface_Stats
{
    unsigned long bytesSent;
    unsigned long bytesReceived;
    unsigned long packetsSent;
    unsigned long packetsReceived;
    unsigned int errorsSent;
    unsigned int errorsReceived;
    unsigned long unicastPacketsSent;
    unsigned long unicastPacketsReceived;
    unsigned int discardPacketsSent;
    unsigned int discardPacketsReceived;
    unsigned long multicastPacketsSent;
    unsigned long multicastPacketsReceived;
    unsigned long broadcastPacketsSent;
    unsigned long broadcastPacketsReceived;
    unsigned int unknownProtoPacketsReceived;
} MoCAInterfaceStats;

typedef enum EMoCAStatsMembers
{
    eMocaBytesSent=0,
    eMocaBytesReceived,
    eMocaPacketsSent,
    eMocaPacketsReceived,
    eMocaErrorsSent,
    eMocaErrorsReceived,
    eMocaUnicastPacketsSent,
    eMocaUnicastPacketsReceived,
    eMocaDiscardPacketsSent,
    eMocaDiscardPacketsReceived,
    eMocaMulticastPacketsSent,
    eMocaMulticastPacketsReceived,
    eMocaBroadcastPacketsSent,
    eMocaBroadcastPacketsReceived,
    eMocaUnknownProtoPacketsReceived
} EMoCAStatsMembers;

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



class hostIf_MoCAInterfaceStats {

    static  GHashTable  *ifHash;

    int dev_id;

    MoCAInterfaceStats    mIfStats;

    int get_Device_MoCA_Interface_Stats_Fields(EMoCAStatsMembers, HOSTIF_MsgData_t *);

    ~hostIf_MoCAInterfaceStats() {};

    hostIf_MoCAInterfaceStats(int _dev_id);

    unsigned long backupBytesSent;
    unsigned long backupBytesReceived;
    unsigned long backupPacketsSent;
    unsigned long backupPacketsReceived;
    unsigned int backupErrorsSent;
    unsigned int backupErrorsReceived;
    unsigned long backupUnicastPacketsSent;
    unsigned long backupUnicastPacketsReceived;
    unsigned int backupDiscardPacketsSent;
    unsigned int backupDiscardPacketsReceived;
    unsigned long backupMulticastPacketsSent;
    unsigned long backupMulticastPacketsReceived;
    unsigned long backupBroadcastPacketsSent;
    unsigned long backupBroadcastPacketsReceived;
    unsigned int backupUnknownProtoPacketsReceived;


    bool bCalledBytesSent;
    bool bCalledBytesReceived;
    bool bCalledPacketsSent;
    bool bCalledPacketsReceived;
    bool bCalledErrorsSent;
    bool bCalledErrorsReceived;
    bool bCalledUnicastPacketsSent;
    bool bCalledUnicastPacketsReceived;
    bool bCalledDiscardPacketsSent;
    bool bCalledDiscardPacketsReceived;
    bool bCalledMulticastPacketsSent;
    bool bCalledMulticastPacketsReceived;
    bool bCalledBroadcastPacketsSent;
    bool bCalledBroadcastPacketsReceived;
    bool bCalledUnknownProtoPacketsReceived;


public:

    static class hostIf_MoCAInterfaceStats *getInstance(int dev_id);

    static void closeInstance(hostIf_MoCAInterfaceStats *);

    static void closeAllInstances();

    static GList* getAllIntefaceStats();

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
    int get_Device_MoCA_Interface_Stats_BytesSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

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
    int get_Device_MoCA_Interface_Stats_BytesReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

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
    int get_Device_MoCA_Interface_Stats_PacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

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
    int get_Device_MoCA_Interface_Stats_PacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

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
    int get_Device_MoCA_Interface_Stats_ErrorsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

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
    int get_Device_MoCA_Interface_Stats_ErrorsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

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
    int get_Device_MoCA_Interface_Stats_UnicastPacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

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
    int get_Device_MoCA_Interface_Stats_UnicastPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

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
    int get_Device_MoCA_Interface_Stats_DiscardPacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

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
    int get_Device_MoCA_Interface_Stats_DiscardPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

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
    int get_Device_MoCA_Interface_Stats_MulticastPacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

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
    int get_Device_MoCA_Interface_Stats_MulticastPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

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
    int get_Device_MoCA_Interface_Stats_BroadcastPacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

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
    int get_Device_MoCA_Interface_Stats_BroadcastPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

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
    int get_Device_MoCA_Interface_Stats_UnknownProtoPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

   /** @} */ // End of Doxygen tag TR69_HOSTIF_MOCA_INTERFACE_STATS

};
/* End of TR_069_DEVICE_MOCA_INTERFACE_STATS_GETTER_API doxygen group */
/**
 * @}
 */


#endif /* HOSTIF_MOCA_INTERFACE_STATS_H_ */


/** @} */
/** @} */
