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
 * @file Device_MoCA_Interface_X_RDKCENTRAL_COM_MeshTable.h
 *
 * TR-069 Device_MoCA_Interface_X_RDKCENTRAL_COM_MeshTable object Public API.
 */



/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef DEVICE_MOCA_INTERFACE_X_RDKCENTRAL_COM_MESHTABLE_H_
#define DEVICE_MOCA_INTERFACE_X_RDKCENTRAL_COM_MESHTABLE_H_

#include <sys/time.h>
#include <errno.h>

#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "Device_MoCA_Interface.h"

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/


/**
 * @defgroup TR_069_DEVICE_MOCA_INTERFACE_ASSOCIATEDDEVICE_API TR-069 Device.Moca.Interface.AssociatedDevice object API.
 *  @ingroup TR_069_DEVICE_MOCA_INTERFACE_API
 *
 *  This table contains the unicast transmit PHY rate between all pair of nodes in the MoCA Network.
 *  Since MeshPHYTxRate is the transmit PHY rate from MeshTxNodeId to MeshRxNodeId,
 *  this table does not contain any entries with MeshTxNodeId equals
 *
 *
 */

/** @addtogroup TR_069_DEVICE_MOCA_INTERFACE_X_RDKCENTRAL-COM_MeshTable_GETTER_API TR-069 Device.Moca.Interface.X_RDKCENTRAL-COM_MeshTable Getter API.
 *  @ingroup TR_069_DEVICE_MOCA_INTERFACE_ASSOCIATEDDEVICE_API
 *
 *  \section dev_moca_if_assocdev_getter TR-069 Device.MoCA.Interface.X_RDKCENTRAL-COM.MeshTable object Getter API
 *
 *  This is the getter group of API for the <b>Device.MoCA.Interface.{i}.X_RDKCENTRAL-COM_MeshTable.{j}</b> object.
 *
 *  The interface for all functions is identical and is described here.
 *
 *  @param[in]   *name     This is the complete path name of the parameter extracted from
 *                         soap message, e.g. ::get_Device_MoCA_Interface_X_RDKCENTRAL-COM_MeshTable_MeshTxNodeId.
 *                         In this case, the path is "Device.MoCA.Interface.1.X_RDKCENTRAL-COM_MeshTable.MeshTxNodeId.
 *  @param[in]   *type     Data type of parameter defined for TR-069. This is same as the
 *                         data type used in the Xi3 data-model.xml file.
 *                         (see parameter.h)
 *  @param[out]  *value    This is the value of the parameter requested by the ACS.
 *                         (see paramaccess.h)
 *
 *  @return The status of the operation.
 *  @retval OK            If parameter requested was successfully fetched.
 *  @retval NOK      	  If parameter requested was not successfully fetched.
 *  @retval NOT_HANDLED   If parameter requested is not handled.
 *
 *  @{
 */

class X_RDKCENTRAL_COM_MeshTable {

    static  GHashTable  *ifHash;
    int dev_id;

    X_RDKCENTRAL_COM_MeshTable(int dev_id);
    ~X_RDKCENTRAL_COM_MeshTable() {};

    unsigned int backupTxNodeID;
    unsigned int backupRxNodeID;
    unsigned int backupPHYTxRate;

    bool bCalledTxNodeID;
    bool bCalledRxNodeID;
    bool bCalledPHYTxRate;

public:
    static class X_RDKCENTRAL_COM_MeshTable *getInstance(int dev_id);
    static void closeInstance(X_RDKCENTRAL_COM_MeshTable *);
    static void closeAllInstances();
    static GList* getAllAssociateDevs();

    /**
     * @brief   Get the node ID of the transmit MoCA node from MoCA Mesh table of MoCA connected device.
     *
     * This function provides the transmit node ID of connected MoCA nodes.
     *
     * @note	This parameter is based on mocaMeshTxNodeIndex from [MOCA11-MIB]..
     *
     * See @ref dev_moca_if_assocdev_getter
     *
     */
    int get_MeshTxNodeId(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

    /**
     * @brief   Get the node ID of the receive MoCA node from MoCA mesh table of MoCA connected device.
     *
     * This function provides the The node ID of the receive MoCA node.
     *
     * @note   	This parameter is based on mocaMeshRxNodeIndex from [MOCA11-MIB]..
     *
     * See @ref dev_moca_if_assocdev_getter
     *
     */
    int get_MeshRxNodeId(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

    /**
     * @brief   Get the Associated Device's presence indicator.
     *
     * This function provides a the PHY transmit rate (in Mbps) from the MoCA node identified by MeshTxNodeId to the MoCA node identified by MeshRxNodeId.
     *  This associated remote device is currently present in this MoCA network.
     *
     * @note  	This parameter is based on mocaMeshTxRate from [MOCA11-MIB].
     *
     * For 50MHz (MoCA 1.1 profile):
     *
     * phy rate = (OFDMb *46) / (256 + ((2 * GAP) + 10))
     * 		ex: OFDMb = 1677, GAP = 12
     * 		phy rate = (1677 * 46) / (256 + ((2 * 12) + 10)) = 77142 / 290 = 266 Mbps
     *
     * See @ref dev_moca_if_assocdev_getter
     *
     */
    int get_MeshPHYTxRate(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);
    int get_BondedChannel(HOSTIF_MsgData_t *stMsgData ,unsigned int associatedDeviceNum,bool *pChanged = NULL);

};

/* End of TR_069_DEVICE_MOCA_INTERFACE_X_RDKCENTRAL_COM_MESHTABLE_GETTER_API doxygen group */
/**
 * @}
 */
#endif /* DEVICE_MOCA_INTERFACE_X_RDKCENTRAL_COM_MESHTABLE_H_ */


/** @} */
/** @} */
