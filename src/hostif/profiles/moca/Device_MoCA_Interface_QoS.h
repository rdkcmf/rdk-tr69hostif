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
 * @file Device_MoCA_Interface_QoS.h
 *
 * TR-069 Device.Moca.Interface.QoS object Public API.
 */

/**
 * @defgroup TR69_HOSTIF_MOCA_INTERFACE_QOS TR-069 Object (Device.MoCA.Interface.{i}.QoS) Public APIs
 * The Device.Moca.Interface.QoS object provides information on MoCA parameterized QoS for this interface [MoCAv1.1].
 * @ingroup TR69_HOSTIF_MOCA
 */


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef HOSTIF_MOCA_INTERFACE_QOS_H_
#define HOSTIF_MOCA_INTERFACE_QOS_H_

#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "Device_MoCA_Interface.h"


/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/
/** @defgroup TR_069_DEVICE_MOCA_INTERFACE_QOS_API TR-069 Device.Moca.Interface.QoS object API.
*  @ingroup TR_069_DEVICE_MOCA_INTERFACE_API
*
*  The Device.Moca.Interface.QoS object provides information on MoCA parameterized QoS for this interface.
*
*  @todo    The Device.Moca.Interface.QoS object is not specified in the Xi3
*           data-model.xml file.
*
*/

/** @addtogroup TR_069_DEVICE_MOCA_INTERFACE_QOS_GETTER_API TR-069 Device.Moca.Interface.QoS Getter API.
 *  @ingroup TR_069_DEVICE_MOCA_INTERFACE_QOS_API
 *
 *  \section dev_moca_if_qos_getter TR-069 Device.MoCA.Interface.QoS object Getter API
 *
 *  This is the getter group of API for the <b>Device.MoCA.Interface.{i}.QoS</b> object.
 *
 *  The interface for all functions is identical and is described here.
 *
 *  @param[in]   *name     This is the complete path name of the parameter extracted from
 *                         soap message, e.g. ::get_Device_MoCA_Interface_QoS_EgressNumFlows.
 *                         In this case, the path is "Device.MoCA.Interface.{i}.QoS.EgressNumFlows".
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
class MoCAInterfaceQoS {

    static  GHashTable  *ifHash;
    int dev_id;
    static class MoCAInterfaceQoS *Instance;
    MoCAInterfaceQoS() {};
    ~MoCAInterfaceQoS() {};

public:
    static class MoCAInterfaceQoS *getInstance();

    /**
     * @ingroup TR69_HOSTIF_MOCA_INTERFACE_QOS
     * @{
     */

    /**
     * @brief Get the number of QoS flows that this interface has from the MoCA network.
     *
     * This parameter is based on mocaIfEgressNodeNumFlows from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.QoS.EgressNumFlow parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_EgressNumFlows(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

    /**
     * @brief Get the number of QoS flows that this interface has onto the MoCA network.
     *
     * This parameter is based on mocaIfIngressNodeNumFlows from [MOCA11-MIB].
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.QoS.IngressNumFlows parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_IngressNumFlows(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

    /**
     * @brief Get the number of entries in the FlowStats table.
     *
     * This function provides the output as a numeric value available in Device.MoCA.Interface.{i}.QoS.FlowStatsNumberOfEntries parameter.
     *
     * @param[in] stMsgData TR-069 Host interface message request.
     * @param[out] pChanged pChange set to not NULL if the object value got changed.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_FlowStatsNumberOfEntries(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

    /** @} */ //End of Doxygen tag TR69_HOSTIF_MOCA_INTERFACE_QOS
};
/* End of TR_069_DEVICE_MOCA_INTERFACE_QOS_GETTER_API doxygen group */
/**
 * @}
 */

#endif /* HOSTIF_MOCA_INTERFACE_QOS_H_ */


/** @} */
/** @} */
