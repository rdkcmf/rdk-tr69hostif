/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2018 RDK Management
 * Copyright 2018 Broadcom Inc.
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
 * @file Device_MoCA_Interface_AssociatedDevice.c
 *
 * @brief Device_MoCA_Interface_AssociatedDevice API Implementation.
 *
 * This is the implementation of the Device_MoCA_Interface_AssociatedDevice API.
 *
 * @par Document
 * TBD Relevant design or API documentation.
 *
 */

/** @addtogroup Device_MoCA_Interface_AssociatedDevice Implementation
 *  This is the implementation of the Device Public API.
 *  @{
 */

/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/
#include "Device_MoCA_Interface_AssociatedDevice.h"
#include "rdk_moca_hal.h"

MoCAInterfaceAssociatedDevice* MoCAInterfaceAssociatedDevice::Instance = NULL;

MoCAInterfaceAssociatedDevice::MoCAInterfaceAssociatedDevice(int _dev_id):
    m_ui32NodeId(0),
    m_bPreferredNC(false),
    m_ui32PhyTxRate(0),
    m_ui32PhyRxRate(0),
    m_ui32TxPowerControlReduction(0),
    m_i32RxPowerLevel(0),
    m_ui32TxBcastRate(0),
    m_i32RxBcastPowerLevel(0),
    m_ui32TxPackets(0),
    m_ui32RxPackets(0),
    m_ui32RxErroredAndMissedPackets(0),
    m_bQAM256Capable(false),
    m_ui32PacketAggregationCapability(0),
    m_ui32RxSNR(0),
    m_bActive(false)
{
    dev_id = _dev_id;
    memset(m_i8MacAddress,0, MAC_ADDRESS_LENGTH);
    memset(m_i8HighestVersion,0,VERSION_LENGTH);
}

MoCAInterfaceAssociatedDevice* MoCAInterfaceAssociatedDevice::getInstance()
{
    if(NULL == Instance)
    {
        Instance = new MoCAInterfaceAssociatedDevice(0);
    }
    return Instance;
}

MoCAInterfaceAssociatedDevice::~MoCAInterfaceAssociatedDevice()
{
    if(Instance)
    {
        delete Instance;
    }
}


/****************************************************************************************************************************************************/
// Device.MoCA.Interface.AssociatedDevice Table Profile. Getters:
/****************************************************************************************************************************************************/

int MoCAInterfaceAssociatedDevice::get_MACAddress(HOSTIF_MsgData_t *stMsgData,unsigned int associatedDeviceNum,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_StringType;
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();

    if(rmh) {
        /* Get NodeID from associate ID*/
        const uint32_t associatedId = associatedDeviceNum;
        uint32_t nodeID;
        if(RMH_SUCCESS != RMH_RemoteNode_GetNodeIdFromAssociatedId(rmh, associatedId, &nodeID)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in RMH_RemoteNode_GetNodeIdFromAssociatedId for AssociatedId (%d) \n",__FUNCTION__,associatedDeviceNum);
            return NOK;
        }
        RMH_MacAddress_t macAddr;
        if(RMH_SUCCESS == RMH_RemoteNode_GetMac(rmh, nodeID, &macAddr)) {
            const size_t macAddrBuffSize = 20;
            char macAddrBuff[macAddrBuffSize] = {'\0'};
            RMH_MacToString(macAddr, macAddrBuff, macAddrBuffSize);
            snprintf(stMsgData->paramValue, macAddrBuffSize-1, "%s", macAddrBuff);
            ret = OK;
        }
    }
    stMsgData->paramLen = sizeof(stMsgData->paramValue);
    return ret;
}

int MoCAInterfaceAssociatedDevice::get_NodeID(HOSTIF_MsgData_t *stMsgData,unsigned int associatedDeviceNum,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();

    if(rmh) {
        /* Get NodeID from associate ID*/
        const uint32_t associatedId = associatedDeviceNum;
        uint32_t nodeID;
        if(RMH_SUCCESS != RMH_RemoteNode_GetNodeIdFromAssociatedId(rmh, associatedId, &nodeID)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in RMH_RemoteNode_GetNodeIdFromAssociatedId for AssociatedId (%d) \n",__FUNCTION__,associatedDeviceNum);
            return NOK;
        }
        if(nodeID >= 0 && nodeID <=16) {
            put_int(stMsgData->paramValue, nodeID);
            ret = OK;
        }
    }
    return ret;
}

int MoCAInterfaceAssociatedDevice::get_PreferredNC(HOSTIF_MsgData_t *stMsgData,unsigned int associatedDeviceNum,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();

    if(rmh) {
        /* Get NodeID from associate ID*/
        const uint32_t associatedId = associatedDeviceNum;
        uint32_t nodeID;
        if(RMH_SUCCESS != RMH_RemoteNode_GetNodeIdFromAssociatedId(rmh, associatedId, &nodeID)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in RMH_RemoteNode_GetNodeIdFromAssociatedId for AssociatedId (%d) \n",__FUNCTION__,associatedDeviceNum);
            return NOK;
        }
        bool isPrefNC = false;
        if(RMH_SUCCESS == RMH_RemoteNode_GetPreferredNC(rmh, nodeID, &isPrefNC)) {
            put_boolean(stMsgData->paramValue, isPrefNC);
            ret = OK;
        }
    }
    return ret;

}
int MoCAInterfaceAssociatedDevice::get_HighestVersion(HOSTIF_MsgData_t *stMsgData,unsigned int associatedDeviceNum,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_StringType;

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        /* Get NodeID from associate ID*/
        const uint32_t associatedId = associatedDeviceNum;
        uint32_t nodeID;
        if(RMH_SUCCESS != RMH_RemoteNode_GetNodeIdFromAssociatedId(rmh, associatedId, &nodeID)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in RMH_RemoteNode_GetNodeIdFromAssociatedId for AssociatedId (%d) \n",__FUNCTION__,associatedDeviceNum);
            return NOK;
        }
        RMH_MoCAVersion hVer;
        if(RMH_SUCCESS == RMH_RemoteNode_GetHighestSupportedMoCAVersion(rmh, nodeID, &hVer)) {
            snprintf(stMsgData->paramValue, (TR69HOSTIFMGR_MAX_PARAM_LEN-1),  "%02X", hVer);
            ret = OK;
        }
    }
    stMsgData->paramLen = sizeof(stMsgData->paramValue);
    return ret;
}

int MoCAInterfaceAssociatedDevice::get_PHYTxRate(HOSTIF_MsgData_t *stMsgData,unsigned int associatedDeviceNum,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        /* Get NodeID from associate ID*/
        const uint32_t associatedId = associatedDeviceNum;
        uint32_t nodeID;
        if(RMH_SUCCESS != RMH_RemoteNode_GetNodeIdFromAssociatedId(rmh, associatedId, &nodeID)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in RMH_RemoteNode_GetNodeIdFromAssociatedId for AssociatedId (%d) \n",__FUNCTION__,associatedDeviceNum);
            return NOK;
        }
        uint32_t response;
        if(RMH_SUCCESS ==RMH_RemoteNode_GetTxUnicastPhyRate(rmh, nodeID, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    stMsgData->paramLen = sizeof(unsigned int);
    return ret;
}

int MoCAInterfaceAssociatedDevice::get_PHYRxRate(HOSTIF_MsgData_t *stMsgData,unsigned int associatedDeviceNum,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        /* Get NodeID from associate ID*/
        const uint32_t associatedId = associatedDeviceNum;
        uint32_t nodeID;
        if(RMH_SUCCESS != RMH_RemoteNode_GetNodeIdFromAssociatedId(rmh, associatedId, &nodeID)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in RMH_RemoteNode_GetNodeIdFromAssociatedId for AssociatedId (%d) \n",__FUNCTION__,associatedDeviceNum);
            return NOK;
        }
        uint32_t response;
        if(RMH_SUCCESS ==RMH_RemoteNode_GetRxUnicastPhyRate(rmh, nodeID, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    stMsgData->paramLen = sizeof(unsigned int);
    return ret;
}

int MoCAInterfaceAssociatedDevice::get_TxPowerControlReduction(HOSTIF_MsgData_t *stMsgData,unsigned int associatedDeviceNum,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        /* Get NodeID from associate ID*/
        const uint32_t associatedId = associatedDeviceNum;
        uint32_t nodeID;
        if(RMH_SUCCESS != RMH_RemoteNode_GetNodeIdFromAssociatedId(rmh, associatedId, &nodeID)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in RMH_RemoteNode_GetNodeIdFromAssociatedId for AssociatedId (%d) \n",__FUNCTION__,associatedDeviceNum);
            return NOK;
        }
        uint32_t response;
        if(RMH_SUCCESS == RMH_RemoteNode_GetTxPowerReduction(rmh, nodeID, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}

int MoCAInterfaceAssociatedDevice::get_RxPowerLevel(HOSTIF_MsgData_t *stMsgData,unsigned int associatedDeviceNum,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        /* Get NodeID from associate ID*/
        const uint32_t associatedId = associatedDeviceNum;
        uint32_t nodeID;
        if(RMH_SUCCESS != RMH_RemoteNode_GetNodeIdFromAssociatedId(rmh, associatedId, &nodeID)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in RMH_RemoteNode_GetNodeIdFromAssociatedId for AssociatedId (%d) \n",__FUNCTION__,associatedDeviceNum);
            return NOK;
        }
        float response;
        if(RMH_SUCCESS == RMH_RemoteNode_GetRxMapPower(rmh, nodeID, &response)) {
            put_int(stMsgData->paramValue, (int)response);
            ret = OK;
        }
    }
    return ret;
}
int MoCAInterfaceAssociatedDevice::get_TxBcastRate(HOSTIF_MsgData_t *stMsgData,unsigned int associatedDeviceNum,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        /* Get NodeID from associate ID*/
        const uint32_t associatedId = associatedDeviceNum;
        uint32_t nodeID;
        if(RMH_SUCCESS != RMH_RemoteNode_GetRxBroadcastPhyRate(rmh, associatedId, &nodeID)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in RMH_RemoteNode_GetNodeIdFromAssociatedId for AssociatedId (%d) \n",__FUNCTION__,associatedDeviceNum);
            return NOK;
        }
        float response;
        if(RMH_SUCCESS == RMH_RemoteNode_GetRxMapPower(rmh, nodeID, &response)) {
            put_int(stMsgData->paramValue, (int)response);
            ret = OK;
        }
    }
    return ret;
}
int MoCAInterfaceAssociatedDevice::get_RxBcastPowerLevel(HOSTIF_MsgData_t *stMsgData,unsigned int associatedDeviceNum,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        /* Get NodeID from associate ID*/
        const uint32_t associatedId = associatedDeviceNum;
        uint32_t nodeID;
        if(RMH_SUCCESS != RMH_RemoteNode_GetNodeIdFromAssociatedId(rmh, associatedId, &nodeID)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in RMH_RemoteNode_GetNodeIdFromAssociatedId for AssociatedId (%d) \n",__FUNCTION__,associatedDeviceNum);
            return NOK;
        }
        float response;
        if(RMH_SUCCESS == RMH_RemoteNode_GetRxBroadcastPower(rmh, nodeID, &response)) {
            put_int(stMsgData->paramValue, (int)response);
            ret = OK;
        }
    }
    return ret;
}

int MoCAInterfaceAssociatedDevice::get_TxPackets(HOSTIF_MsgData_t *stMsgData,unsigned int associatedDeviceNum,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        /* Get NodeID from associate ID*/
        const uint32_t associatedId = associatedDeviceNum;
        uint32_t nodeID;
        if(RMH_SUCCESS != RMH_RemoteNode_GetNodeIdFromAssociatedId(rmh, associatedId, &nodeID)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in RMH_RemoteNode_GetNodeIdFromAssociatedId for AssociatedId (%d) \n",__FUNCTION__,associatedDeviceNum);
            return NOK;
        }
        uint32_t response;
        if(RMH_SUCCESS == RMH_RemoteNode_GetTxPackets(rmh, nodeID, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}

int MoCAInterfaceAssociatedDevice::get_RxPackets(HOSTIF_MsgData_t *stMsgData,unsigned int associatedDeviceNum,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        /* Get NodeID from associate ID*/
        const uint32_t associatedId = associatedDeviceNum;
        uint32_t nodeID;
        if(RMH_SUCCESS != RMH_RemoteNode_GetNodeIdFromAssociatedId(rmh, associatedId, &nodeID)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in RMH_RemoteNode_GetNodeIdFromAssociatedId for AssociatedId (%d) \n",__FUNCTION__,associatedDeviceNum);
            return NOK;
        }
        uint32_t response;
        if(RMH_SUCCESS == RMH_RemoteNode_GetRxPackets(rmh, nodeID, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}
int MoCAInterfaceAssociatedDevice::get_RxErroredAndMissedPackets(HOSTIF_MsgData_t *stMsgData,unsigned int associatedDeviceNum,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        /* Get NodeID from associate ID*/
        const uint32_t associatedId = associatedDeviceNum;
        uint32_t nodeID;
        if(RMH_SUCCESS != RMH_RemoteNode_GetNodeIdFromAssociatedId(rmh, associatedId, &nodeID)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in RMH_RemoteNode_GetNodeIdFromAssociatedId for AssociatedId (%d) \n",__FUNCTION__,associatedDeviceNum);
            return NOK;
        }
        uint32_t response;
        if(RMH_SUCCESS == RMH_RemoteNode_GetRxTotalErrors(rmh, nodeID, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}

int MoCAInterfaceAssociatedDevice::get_QAM256Capable(HOSTIF_MsgData_t *stMsgData,unsigned int associatedDeviceNum,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        /* Get NodeID from associate ID*/
        const uint32_t associatedId = associatedDeviceNum;
        uint32_t nodeID;
        if(RMH_SUCCESS != RMH_RemoteNode_GetNodeIdFromAssociatedId(rmh, associatedId, &nodeID)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in RMH_RemoteNode_GetNodeIdFromAssociatedId for AssociatedId (%d) \n",__FUNCTION__,associatedDeviceNum);
            return NOK;
        }
        bool response;
        if(RMH_SUCCESS == RMH_RemoteNode_GetQAM256Capable(rmh, nodeID, &response)) {
            put_boolean(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}

int MoCAInterfaceAssociatedDevice::get_PacketAggregationCapability(HOSTIF_MsgData_t *stMsgData,unsigned int associatedDeviceNum,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        /* Get NodeID from associate ID*/
        const uint32_t associatedId = associatedDeviceNum;
        uint32_t nodeID;
        if(RMH_SUCCESS != RMH_RemoteNode_GetNodeIdFromAssociatedId(rmh, associatedId, &nodeID)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in RMH_RemoteNode_GetNodeIdFromAssociatedId for AssociatedId (%d) \n",__FUNCTION__,associatedDeviceNum);
            return NOK;
        }
        uint32_t response;
        if(RMH_SUCCESS == RMH_RemoteNode_GetMaxPacketAggregation(rmh, nodeID, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}

int MoCAInterfaceAssociatedDevice::get_RxSNR(HOSTIF_MsgData_t *stMsgData,unsigned int associatedDeviceNum,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        /* Get NodeID from associate ID*/
        const uint32_t associatedId = associatedDeviceNum;
        uint32_t nodeID;
        if(RMH_SUCCESS != RMH_RemoteNode_GetNodeIdFromAssociatedId(rmh, associatedId, &nodeID)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in RMH_RemoteNode_GetNodeIdFromAssociatedId for AssociatedId (%d) \n",__FUNCTION__,associatedDeviceNum);
            return NOK;
        }
        float response;
        if(RMH_SUCCESS == RMH_RemoteNode_GetRxSNR(rmh, nodeID, &response)) {
            put_int(stMsgData->paramValue, (int)response);
            ret = OK;
        }
    }
    return ret;
}

int MoCAInterfaceAssociatedDevice::get_Active(HOSTIF_MsgData_t *stMsgData,unsigned int associatedDeviceNum,bool *pChanged)
{
    int retval = NOK;

    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        /* Get NodeID from associate ID*/
        const uint32_t associatedId = associatedDeviceNum;
        uint32_t nodeID;
        if(RMH_SUCCESS != RMH_RemoteNode_GetNodeIdFromAssociatedId(rmh, associatedId, &nodeID)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in RMH_RemoteNode_GetNodeIdFromAssociatedId for AssociatedId (%d) \n",__FUNCTION__,associatedDeviceNum);
            return NOK;
        }

        RMH_NodeList_Uint32_t response;
        RMH_Result ret = RMH_Network_GetNodeIds(rmh, &response);
        if (ret != RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetNodeIds failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return retval;
        }

        for (int nodeId = 0; nodeId < RMH_MAX_MOCA_NODES; nodeId++) {
            if ((true ==response.nodePresent[nodeId]) && (nodeId == nodeID)) {
                put_boolean(stMsgData->paramValue, true);
                retval = OK;
                break;
            }
        }
    }
    return retval;
}

/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */
