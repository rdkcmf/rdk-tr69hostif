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
 * @file Device_IP_Interface_Stats.h
 * @brief The header file provides TR069 device IP interface stats information APIs.
 */

/**
 * @defgroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS TR-069 Object (Device.IP.Interface.Stats)
 * @par Throughput statistics for this interface.
 * The CPE MUST reset the interface's Stats parameters unless otherwise stated in individual object
 * or parameter descriptions either
 * - when the interface becomes operationally down due to a previous administrative down
 * i.e. the interface's Status parameter transitions to a down state after the interface is disabled or
 * - when the interface becomes administratively up i.e. the interface's Enable parameter transitions
 * from false to true.
 * @ingroup TR69_HOSTIF_DEVICE_IP
 *
 * @defgroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_API TR-069 Object (Device.IP.Interface.Stats) Public APIs
 * Describe the details about TR069 Device IP Interface stats APIs specifications.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS
 *
 * @defgroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_CLASSES TR-069 Object (Device.IP.Interface.Stats) Public Classes
 * Describe the details about classes used in TR069 Device IP Interface stats.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS
 *
 * @defgroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_DSSTRUCT TR-069 Object (Device.IP.Interface.Stats) Public DataStructure
 * Describe the details about data structure used in TR069 Device IP Interface stats.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS
 */


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef DEVICE_IP_INTERFACE_STATS_H_
#define DEVICE_IP_INTERFACE_STATS_H_

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/
#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "hostIf_updateHandler.h"

#define         _LENGTH_PARAMETER               64
#define         _PATH_SYS_CLASS_NET             "/sys/class/net/"

/**
 * @addtogroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_DSSTRUCT
 * @{
 */
/**
 * @brief It contains the members variables of the Device_IP_Interface_stats structure.
 */
typedef struct Device_IP_Interface_Stats {
    unsigned long bytesSent;
    unsigned long bytesReceived;
    unsigned long packetsSent;
    unsigned long packetsReceived;
    unsigned long errorsSent;
    unsigned long errorsReceived;
    unsigned long unicastPacketsSent;
    unsigned long unicastPacketsReceived;
    unsigned long discardPacketsSent;
    unsigned long discardPacketsReceived;
    unsigned long multicastPacketsSent;
    unsigned long multicastPacketsReceived;
    unsigned long broadcastPacketsSent;
    unsigned long broadcastPacketsReceived;
    unsigned long unknownProtoPacketsReceived;
} IPInterfaceStats;

/** @defgroup TR_069_DEVICE_IP_INTERFACE_STATS_API TR-069 Device.IP.Interface.Stats object API.
 *  @ingroup TR_069_DEVICE_IP_INTERFACE_API
 *
 *  The Device.IP.Interface.Stats is the throughput statistics for this interface.
 *
 *  The CPE MUST reset the interface's Stats parameters (unless otherwise stated in
 *  individual object or parameter descriptions) either when the interface becomes
 *  operationally down due to a previous administrative down (i.e. the interface's
 *  Status parameter transitions to a down state after the interface is disabled) or
 *  when the interface becomes administratively up (i.e. the interface's Enable parameter
 *  transitions from false to true).
 *
 */

/** @addtogroup TR_069_DEVICE_IP_INTERFACE_STATS_GETTER_API TR-069 Device.IP.Interface.Stats Getter API.
 *  @ingroup TR_069_DEVICE_IP_INTERFACE_STATS_API
 *
 *  \section dev_ip_stats_getter TR-069 Device.IP.Interface.Stats object Getter API
 *
 *  This is the getter group of API for the <b>Device.IP.Interface.{i}.Stats</b> object.
 *
 *  The interface for all functions is identical and is described here.
 *
 *  @param[in]   *name     This is the complete path name of the parameter extracted from
 *                         soap message, e.g. ::get_Device_IP_Interface_Stats_BytesSent.
 *                         In this case, the path is "Device.IP.Interface.{i}.Stats.BytesSent".
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
 * @brief This class provides the hostIf IP interface stats for getting IP interface stats information.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_STATS_CLASSES
 */
class hostIf_IPInterfaceStats {

    static  GHashTable  *ifHash;

    static GMutex *m_mutex;

    static IPInterfaceStats curntIpStat;

    int dev_id;
    char nameOfInterface[IF_NAMESIZE];

    hostIf_IPInterfaceStats(int dev_id);
    ~hostIf_IPInterfaceStats() {};

    void refreshInterfaceName ();
    int getSysClassNetStatistic (char* statistic, unsigned long* result);

    int backupBytesSent;
    int backupBytesReceived;
    int backupPacketsSent;
    int backupPacketsReceived;
    int backupErrorsSent;
    int backupErrorsReceived;
    int backupUnicastPacketsSent;
    int backupUnicastPacketsReceived;
    int backupDiscardPacketsSent;
    int backupDiscardPacketsReceived;
    int backupMulticastPacketsSent;
    int backupMulticastPacketsReceived;
    int backupBroadcastPacketsSent;
    int backupBroadcastPacketsReceived;
    int backupUnknownProtoPacketsReceived;
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

    static hostIf_IPInterfaceStats* getInstance(int dev_id);

    static void closeInstance(hostIf_IPInterfaceStats*);

    static GList* getAllInstances();

    static void closeAllInstances();

    static void getLock();

    static void releaseLock();

    int handleGetMsg (const char* pSetting, HOSTIF_MsgData_t* stMsgData);


    int get_Device_IP_Interface_Stats_BytesSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);


    int get_Device_IP_Interface_Stats_BytesReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);

    /**
     * @brief    Get the number of packets sent on the IP Interface.
     *
     * This function provides the total number of packets transmitted out of the interface.
     *
     * See @ref dev_ip_stats_getter
     *
     */

    int get_Device_IP_Interface_Stats_PacketsSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);


    int get_Device_IP_Interface_Stats_PacketsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);


    int get_Device_IP_Interface_Stats_ErrorsSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);


    int get_Device_IP_Interface_Stats_ErrorsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);


    int get_Device_IP_Interface_Stats_UnicastPacketsSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);


    int get_Device_IP_Interface_Stats_UnicastPacketsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);


    int get_Device_IP_Interface_Stats_DiscardPacketsSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);


    int get_Device_IP_Interface_Stats_DiscardPacketsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);


    int get_Device_IP_Interface_Stats_MulticastPacketsSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);


    int get_Device_IP_Interface_Stats_MulticastPacketsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);


    int get_Device_IP_Interface_Stats_BroadcastPacketsSent(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);


    int get_Device_IP_Interface_Stats_BroadcastPacketsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);


    int get_Device_IP_Interface_Stats_UnknownProtoPacketsReceived(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);

    /* End of TR_069_DEVICE_IP_INTERFACE_STATS_GETTER_API doxygen group */
    /**
     * @}
     */

};

#endif /* DEVICE_IP_INTERFACE_STATS_H_ */


/** @} */
/** @} */
