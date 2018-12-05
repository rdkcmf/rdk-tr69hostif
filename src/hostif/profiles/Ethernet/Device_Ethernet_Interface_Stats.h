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
 * @file Device_Ethernet_Interface_Stats.h
 *
 * @brief TR-069 Device.Ethernet.Interface.Stats object Public API.
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


#ifndef DEVICE_ETHERNET_INTERFACE_STATS_H_
#define DEVICE_ETHERNET_INTERFACE_STATS_H_

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/

#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "hostIf_updateHandler.h"



#define LENGTH_PARAMETER 64

typedef enum
{
    eBytesSent=0,
    eBytesReceived,
    ePacketsSent,
    ePacketsReceived,
    eErrorsSent,
    eErrorsReceived,
    eUnicastPacketsSent,
    eUnicastPacketsReceived,
    eDiscardPacketsSent,
    eDiscardPacketsReceived,
    eMulticastPacketsSent,
    eMulticastPacketsReceived,
    eBroadcastPacketsSent,
    eBroadcastPacketsReceived,
    eUnknownProtoPacketsReceived
}
EEthInterfaceStatsMembers;

typedef struct Device_Ethernet_Interface_Stats
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

} EthernetInterfaceStats;


/** @defgroup TR_069_DEVICE_ETHERNET_INTERFACE_STATS_API TR-069 Device.Ethernet.Interface.Stats object API.
 *  @ingroup TR_069_DEVICE_ETHERNET_INTERFACE_API
 *
 *  The Device.Ethernet.Interface.Stats is the throughput statistics for this interface.
 *
 *  The CPE MUST reset the interface's Stats parameters (unless otherwise stated in
 *  individual object or parameter descriptions) either when the interface becomes
 *  operationally down due to a previous administrative down (i.e. the interface's
 *  Status parameter transitions to a down state after the interface is disabled) or
 *  when the interface becomes administratively up (i.e. the interface's Enable
 *  parameter transitions from false to true).
 *
 */

/** @addtogroup TR_069_DEVICE_ETHERNET_INTERFACE_STATS_GETTER_API TR-069 Device.Ethernet.Interface.Stats Getter API.
 *  @ingroup TR_069_DEVICE_ETHERNET_INTERFACE_STATS_API
 *
 *  \section dev_ethernet_interface_stats_getter TR-069 Device.Ethernet.Interface.Stats object Getter API.
 *
 *  This is the getter group of API for the <b>Device.Ethernet.Interface.{i}.Stats</b> object.
 *
 *  The interface for all functions is identical and is described here.
 *
 *  @param[in]   *name     This is the complete path name of the parameter extracted from
 *                         soap message, e.g. ::get_Device_Ethernet_Interface_Stats_BytesSent.
 *                         In this case, the path is "Device.Ethernet.Interface.{i}.Stats.BytesSent".
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

class hostIf_EthernetInterfaceStats {

    static  GHashTable  *ifHash;

    static GMutex *m_mutex;

    hostIf_EthernetInterfaceStats(int dev_id);

    ~hostIf_EthernetInterfaceStats() {};


    int dev_id;

    int backupBytesSent;
    int backupBytesReceived;
    int backupPacketsSent;
    int backupPacketsReceived;
    int backupUnicastPacketsSent;
    int backupUnicastPacketsReceived;
    int backupMulticastPacketsReceived;
    int backupBroadcastPacketsSent;
    int backupBroadcastPacketsReceived;
    int backupMulticastPacketsSent;
    int backupErrorSent;
    int backupErrorsReceived;
    int backupErrorsSent;
    int backupDiscardPacketsReceived;

    bool bCalledBytesSent ;
    bool bCalledBytesReceived ;
    bool bCalledPacketsSent ;
    bool bCalledPacketsReceived ;
    bool bCalledErrorSent ;
    bool bCalledErrorsReceived ;
    bool bCalledErrorsSent ;
    bool bCalledUnicastPacketsSent ;
    bool bCalledDiscardPacketsReceived ;
    bool bCalledDiscardPacketsSent;
    bool bCalledUnicastPacketsReceived ;
    bool bCalledMulticastPacketsReceived ;
    bool bCalledBroadcastPacketsSent ;
    bool bCalledBroadcastPacketsReceived ;
    bool bCalledMulticastPacketsSent ;
    bool bCalledUnknownProtoPacketsReceived;
public:

    static hostIf_EthernetInterfaceStats *getInstance(int dev_id);

    static void closeInstance(hostIf_EthernetInterfaceStats *);

    static GList* getAllInstances();

    static void closeAllInstances();

    static void getLock();

    static void releaseLock();

    static EthernetInterfaceStats stEthInterfaceStats;

    /**
     * @brief    Get the number of bytes sent on an Ethernet Interface.
     *
     * This function provides the total number of bytes transmitted out of this Ethernet
     * interface, including framing characters.
     *
     * See @ref dev_ethernet_interface_stats_getter
     *
     */

    int get_Device_Ethernet_Interface_Stats_BytesSent(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the number of bytes received on an Ethernet Interface.
     *
     * This function provides the total number of bytes received on this Ethernet
     * interface, including framing characters.
     *
     * See @ref dev_ethernet_interface_stats_getter
     *
     */

    int get_Device_Ethernet_Interface_Stats_BytesReceived(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the number of packets sent on an Ethernet Interface.
     *
     * This function provides the total number of packets transmitted out of this Ethernet interface.
     *
     * See @ref dev_ethernet_interface_stats_getter
     *
     */

    int get_Device_Ethernet_Interface_Stats_PacketsSent(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the number of packets received on an Ethernet Interface.
     *
     * This function provides the total number of packets received on this Ethernet interface.
     *
     * See @ref dev_ethernet_interface_stats_getter
     *
     */

    int get_Device_Ethernet_Interface_Stats_PacketsReceived(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the number of send errors on an Ethernet Interface.
     *
     * This function provides the total number of outbound packets that could not be
     * transmitted because of errors.
     *
     * See @ref dev_ethernet_interface_stats_getter
     *
     */

    int get_Device_Ethernet_Interface_Stats_ErrorsSent(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the number of receive errors on an Ethernet Interface.
     *
     * This function provides the total number of inbound packets that contained errors
     * preventing them from being delivered to a higher-layer protocol.
     *
     * See @ref dev_ethernet_interface_stats_getter
     *
     */

    int get_Device_Ethernet_Interface_Stats_ErrorsReceived(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the number of unicast packets for which a request to send on the IP
     *           Interface was received.
     *
     * This function provides the total number of packets requested for transmission which
     * were not addressed to a multicast or broadcast address at this layer, including
     * those that were discarded or not sent.
     *
     * See @ref dev_ethernet_interface_stats_getter
     *
     */

    int get_Device_Ethernet_Interface_Stats_UnicastPacketsSent(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the number of unicast packets received on an Ethernet Interface.
     *
     * This function provides the total number of received packets, delivered by this layer to
     * a higher layer, which were not addressed to a multicast or broadcast address at this layer.
     *
     * See @ref dev_ethernet_interface_stats_getter
     *
     */

    int get_Device_Ethernet_Interface_Stats_UnicastPacketsReceived(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the number of discarded outbound packets on an Ethernet Interface.
     *
     * This function provides the total number of outbound packets which were chosen to be
     * discarded even though no errors had been detected to prevent their being transmitted.
     *
     * @note     One possible reason for discarding such a packet could be to free up buffer
     *           space.
     *
     * See @ref dev_ethernet_interface_stats_getter
     *
     */

    int get_Device_Ethernet_Interface_Stats_DiscardPacketsSent(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the number of discarded inbound packets on an Ethernet Interface.
     *
     * This function provides the total number of inbound packets which were chosen to be
     * discarded even though no errors had been detected to prevent their being delivered.
     *
     * @note    One possible reason for discarding such a packet could be to free up buffer
     *          space.
     *
     * See @ref dev_ethernet_interface_stats_getter
     *
     */

    int get_Device_Ethernet_Interface_Stats_DiscardPacketsReceived(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the number of Multicast Packets sent on an Ethernet Interface.
     *
     * This function provides the total number of packets that higher-level protocols
     * requested for transmission and which were addressed to a multicast address at
     * this layer, including those that were discarded or not sent.
     *
     * See @ref dev_ethernet_interface_stats_getter
     *
     */

    int get_Device_Ethernet_Interface_Stats_MulticastPacketsSent(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the number of Multicast Packets received on an Ethernet Interface.
     *
     * This function provides the total number of received packets, delivered by this layer
     * to a higher layer, which were addressed to a multicast address at this layer.
     *
     * See @ref dev_ethernet_interface_stats_getter
     *
     */

    int get_Device_Ethernet_Interface_Stats_MulticastPacketsReceived(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the number of Broadcast Packets sent on an Ethernet Interface.
     *
     * This function provides the total number of packets that higher-level protocols
     * requested for transmission and which were addressed to a broadcast address at this
     * layer, including those that were discarded or not sent.
     *
     * @note     IPv6 does not define broadcast addresses, so IPv6 packets will never cause
     *           this counter to increment.
     *
     * See @ref dev_ethernet_interface_stats_getter
     *
     */

    int get_Device_Ethernet_Interface_Stats_BroadcastPacketsSent(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the number of Broadcast Packets received on an Ethernet Interface.
     *
     * This function provides the total number of received packets, delivered by this layer
     * to a higher layer, which were addressed to a broadcast address at this layer.
     *
     * @note     IPv6 does not define broadcast addresses, so IPv6 packets will never cause
     *           this counter to increment.
     *
     * See @ref dev_ethernet_interface_stats_getter
     *
     */

    int get_Device_Ethernet_Interface_Stats_BroadcastPacketsReceived(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the number of Packets of unidentified protocol received on an Ethernet Interface.
     *
     * This function provides the total number of packets received via this Ethernet interface which
     * were discarded because they were of an unknown or unsupported protocol.
     *
     * See @ref dev_ethernet_interface_stats_getter
     *
     */

    int get_Device_Ethernet_Interface_Stats_UnknownProtoPacketsReceived(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /* End of TR_069_DEVICE_ETHERNET_INTERFACE_STATS_GETTER_API doxygen group */
    /**
     * @}
     */
};

#endif /* DEVICE_ETHERNET_INTERFACE_STATS_H_ */


/** @} */
/** @} */
