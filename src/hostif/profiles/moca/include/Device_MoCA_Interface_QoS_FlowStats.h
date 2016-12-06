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
 * @file Device_MoCA_Interface_QoS_FlowStats.h
 *
 * TR-069 Device.Moca.Interface.QoS.FlowStats object Public API.
 */

/**
 * @defgroup TR69_HOSTIF_MOCA_INTERFACE_QOS_FLOWSTATS TR-069 Object (Device.MoCA.Interface.{i}.QoS.FlowStats) Public APIs
 * The flow statistics table provides information on the MoCA parameterized QoS flows this interface has
 * allocated onto the MoCA network.
 *
 * At most one entry in this table can exist with a given value for FlowID.
 * @ingroup TR69_HOSTIF_MOCA
 */




/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef HOSTIF_MOCA_INTERFACE_QOS_FLOWSTATS_H_
#define HOSTIF_MOCA_INTERFACE_QOS_FLOWSTATS_H_

#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "Device_MoCA_Interface.h"


/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/

/** @defgroup TR_069_DEVICE_MOCA_INTERFACE_QOS_FLOWSTATS_API TR-069 Device.Moca.Interface.QoS.FlowStats object API.
 *  @ingroup TR_069_DEVICE_MOCA_INTERFACE_QOS_API
 *
 *  The Device.Moca.Interface.QoS.FlowStats object is a flow statistics table which
 *  provides information on the MoCA parameterized QoS flows this interface has allocated
 *  onto the MoCA network.
 *
 *  @note    At most one entry in this table can exist with a given value for FlowID.
 *
 */

/** @addtogroup TR_069_DEVICE_MOCA_INTERFACE_QOS_FLOWSTATS_GETTER_API TR-069 Device.Moca.Interface.QoS.FlowStats Getter API.
 *  @ingroup TR_069_DEVICE_MOCA_INTERFACE_QOS_FLOWSTATS_API
 *
 *  \section dev_moca_if_qos_flowstats_getter TR-069 Device.MoCA.Interface.QoS.FlowStats object Getter API
 *
 *  This is the getter group of API for the <b>Device.MoCA.Interface.{i}.QoS.FlowStats.{j}</b> object.
 *
 *  The interface for all functions is identical and is described here.
 *
 *  @param[in]   *name     This is the complete path name of the parameter extracted from
 *                         soap message, e.g. ::get_Device_MoCA_Interface_QoS_FlowStats_FlowID.
 *                         In this case, the path is "Device.MoCA.Interface.{i}.QoS.FlowStats.{j}.FlowID".
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
class hostIf_MoCAInterfaceQoSFlowStats {

    static  GHashTable  *ifHash;
    int dev_id;
    hostIf_MoCAInterfaceQoSFlowStats(int dev_id):dev_id(dev_id) {};
    ~hostIf_MoCAInterfaceQoSFlowStats() {};

public:
    static class hostIf_MoCAInterfaceQoSFlowStats *getInstance(int dev_id);
    static void closeInstance(hostIf_MoCAInterfaceQoSFlowStats *);
    static void closeAllInstances();
    static GList* getAllInstances();

   /**
    * @ingroup TR69_HOSTIF_MOCA_INTERFACE_QOS_FLOWSTATS
    * @{
    */

   /**
    * @brief Get the flow ID used to identify a flow in the network.
    *
    * This parameter is based on mocaIfFlowID from [MOCA11-MIB].
    *
    * This function provides the output as a numeric value available
    * in Device.MoCA.Interface.{i}.QoS.FlowStats.FlowID parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    * @param[in] entryNum Entry Number.
    * @param[out] pChanged pChange set to not NULL if the object value got changed.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_MoCA_Interface_QoS_FlowStats_FlowID(HOSTIF_MsgData_t *stMsgData ,unsigned int entryNum,bool *pChanged = NULL);

   /**
    * @brief Get the Destination Address (DA) for the packets in this flow.
    *
    * This function provides the output as a string value available
    * in Device.MoCA.Interface.{i}.QoS.FlowStats.PacketDA parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    * @param[in] entryNum Entry Number.
    * @param[out] pChanged pChange set to not NULL if the object value got changed.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_MoCA_Interface_QoS_FlowStats_PacketDA(HOSTIF_MsgData_t *stMsgData ,unsigned int entryNum,bool *pChanged = NULL);

   /**
    * @brief Get the maximum required rate in Kbps.
    *
    * This function provides the output as a numeric value available
    * in Device.MoCA.Interface.{i}.QoS.FlowStats.MaxRate parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    * @param[in] entryNum Entry Number.
    * @param[out] pChanged pChange set to not NULL if the object value got changed.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_MoCA_Interface_QoS_FlowStats_MaxRate(HOSTIF_MsgData_t *stMsgData ,unsigned int entryNum,bool *pChanged = NULL);

   /**
    * @brief Get Maximum burst size of MoCA interface.
    *
    * This function provides the output as a numeric value available
    * in Device.MoCA.Interface.{i}.QoS.FlowStats.MaxBurstSize parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    * @param[in] entryNum Entry Number.
    * @param[out] pChanged pChange set to not NULL if the object value got changed.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_MoCA_Interface_QoS_FlowStats_MaxBurstSize(HOSTIF_MsgData_t *stMsgData ,unsigned int entryNum,bool *pChanged = NULL);

   /**
    * @brief Get the flow lease time (expressed in seconds). A LeaseTime of 0 means unlimited lease time.
    *
    * This function provides the output as a numeric value available
    * in Device.MoCA.Interface.{i}.QoS.FlowStats.LeaseTime parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    * @param[in] entryNum Entry Number.
    * @param[out] pChanged pChange set to not NULL if the object value got changed.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_MoCA_Interface_QoS_FlowStats_LeaseTime(HOSTIF_MsgData_t *stMsgData ,unsigned int entryNum,bool *pChanged = NULL);

   /**
    * @brief Get the Flow lease time remaining (expressed in seconds).
    *
    * If LeaseTime is 0 then a LeaseTimeLeft of 0 means unlimited lease time; otherwise, a LeaseTimeLeft of 0 means expired.
    *
    * This parameter is based on mocaIfLeaseTimeLeft from [MOCA11-MIB].
    *
    * This function provides the output as a numeric value available
    * in Device.MoCA.Interface.{i}.QoS.FlowStats.LeaseTimeLeft parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    * @param[in] entryNum Entry Number.
    * @param[out] pChanged pChange set to not NULL if the object value got changed.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_MoCA_Interface_QoS_FlowStats_LeaseTimeLeft(HOSTIF_MsgData_t *stMsgData ,unsigned int entryNum,bool *pChanged = NULL);

   /**
    * @brief Get the number of packets transmitted for this flow.
    *
    * This parameter is based on mocaIfTxPacketsFlow from [MOCA11-MIB].
    *
    * This function provides the output as a numeric value available
    * in Device.MoCA.Interface.{i}.QoS.FlowStats.FlowPackets parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    * @param[in] entryNum Entry Number.
    * @param[out] pChanged pChange set to not NULL if the object value got changed.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_MoCA_Interface_QoS_FlowStats_FlowPackets(HOSTIF_MsgData_t *stMsgData ,unsigned int entryNum,bool *pChanged = NULL);

    /* End of TR_069_DEVICE_MOCA_INTERFACE_QOS_FLOWSTATS_GETTER_API doxygen group */
    /**
     * @}
     */
};

#endif /* HOSTIF_MOCA_INTERFACE_QOS_FLOWSTATS_H_ */


/** @} */
/** @} */
