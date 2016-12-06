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
 * @file hostIf_MoCAClient_ReqHandler.cpp
 * @brief The header file provides HostIf IP json request handler information APIs.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#include "hostIf_main.h"
#ifdef USE_MoCA_PROFILE
#include "hostIf_MoCAClient_ReqHandler.h"
#include "Device_MoCA_Interface.h"
#include "Device_MoCA_Interface_QoS_FlowStats.h"
#include "Device_MoCA_Interface_QoS.h"
#include "Device_MoCA_Interface_Stats.h"
#include "Device_MoCA_Interface_AssociatedDevice.h"
#include "Device_MoCA_Interface_X_RDKCENTRAL_COM_MeshTable.h"


MoCAClientReqHandler* MoCAClientReqHandler::pInstance = NULL;
updateCallback MoCAClientReqHandler::mUpdateCallback = NULL;
int MoCAClientReqHandler::curNumOfDevices[10] = {0,};
int MoCAClientReqHandler::numOfMocaMeshEntries = 0;

msgHandler* MoCAClientReqHandler::getInstance()
{

    if(!pInstance)
        pInstance = new MoCAClientReqHandler();
    return pInstance;
}

void MoCAClientReqHandler::reset()
{
    hostIf_MoCAInterface::getLock();
    memset(curNumOfDevices,0,sizeof(curNumOfDevices));
    numOfMocaMeshEntries = 0;
    hostIf_MoCAInterface::releaseLock();
}

/**
 * @brief This function is use to initialize. Currently not implemented.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if initialization is successfully .
 * @retval false if initialization is not successful.
 * @ingroup TR-069HOSTIF_MOCACLIENT_REQHANDLER_CLASSES
 */
bool MoCAClientReqHandler::init()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

/**
 * @brief This function is used to close all the instances of MoCA interface, MoCA interface stats
 * MoCA interface QoS, MoCA interface QoSFlowStats and MoCA interface associated device.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if it is successfully close all the instances.
 * @retval false if not able to close all the instances.
 * @ingroup TR-069HOSTIF_MOCACLIENT_REQHANDLER_CLASSES
 */
bool MoCAClientReqHandler::unInit()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    hostIf_MoCAInterface::closeAllInstances();
    hostIf_MoCAInterfaceStats::closeAllInstances();
    hostIf_MoCAInterfaceQoS::closeAllInstances();
    hostIf_MoCAInterfaceQoSFlowStats::closeAllInstances();
    hostIf_MoCAInterfaceAssociatedDevice::closeAllInstances();

    MoCADevice::closeAllInstances();

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

/**
 * @brief This function is use to handle the set message request of MoCA interface
 * and set the attributes of the components such as "Enable", "Alias", "LowerLayers"
 * etc..
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully.
 * @retval ERR_INTERNAL_ERROR if not able to set the data to the device.
 * @ingroup TR-069HOSTIF_MOCACLIENT_REQHANDLER_CLASSES
 */
int MoCAClientReqHandler::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *pSetting;
    const char *pSubSetting;
    int instanceNumber;
    int subInstanceNumber;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    if(matchComponent(stMsgData->paramName,
                      "Device.MoCA.Interface",&pSetting,instanceNumber))
    {
        stMsgData->instanceNum = instanceNumber;
        hostIf_MoCAInterface::getLock();

        hostIf_MoCAInterface *pIface = hostIf_MoCAInterface::getInstance(instanceNumber);

        if(!pIface)
        {
            hostIf_MoCAInterface::releaseLock();
            return NOK;
        }

        if (strcasecmp(pSetting,"Enable") == 0)
        {
            ret = pIface->set_Device_MoCA_Interface_Enable(stMsgData);
        }
        else if (strcasecmp(pSetting,"Alias") == 0)
        {
            ret = pIface->set_Device_MoCA_Interface_Alias(stMsgData);
        }
        else if (strcasecmp(pSetting,"LowerLayers") == 0)
        {
            ret = pIface->set_Device_MoCA_Interface_LowerLayers(stMsgData);
        }
        else if (strcasecmp(pSetting,"PreferredNC") == 0)
        {
            ret = pIface->set_Device_MoCA_Interface_PreferredNC(stMsgData);
        }
        else if (strcasecmp(pSetting,"PrivacyEnabledSetting") == 0)
        {
            ret = pIface->set_Device_MoCA_Interface_PrivacyEnabledSetting(stMsgData);
        }
        else if (strcasecmp(pSetting,"FreqCurrentMaskSetting") == 0)
        {
            ret = pIface->set_Device_MoCA_Interface_FreqCurrentMaskSetting(stMsgData);
        }
        else if (strcasecmp(pSetting,"KeyPassphrase") == 0)
        {
            ret = pIface->set_Device_MoCA_Interface_KeyPassphrase(stMsgData);
        }
        else if (strcasecmp(pSetting,"TxPowerLimit") == 0)
        {
            ret = pIface->set_Device_MoCA_Interface_TxPowerLimit(stMsgData);
        }
        else if (strcasecmp(pSetting,"PowerCntlPhyTarget") == 0)
        {
            ret = pIface->set_Device_MoCA_Interface_PowerCntlPhyTarget(stMsgData);
        }
        else if (strcasecmp(pSetting,"BeaconPowerLimit") == 0)
        {
            ret = pIface->set_Device_MoCA_Interface_BeaconPowerLimit(stMsgData);
        }
    }
    hostIf_MoCAInterface::releaseLock();
    return ret;
}

/**
 * @brief This function is use to handle the get message request of MoCA interface,
 * QoS.FlowStats, Associated devices and get the attributes of the components.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully.
 * @retval ERR_INTERNAL_ERROR if not able to set the data to the device.
 * @ingroup TR-069HOSTIF_MOCACLIENT_REQHANDLER_CLASSES
 */
int MoCAClientReqHandler::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *pSetting;
    const char *pSubSetting;
    int instanceNumber;
    int subInstanceNumber;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    hostIf_MoCAInterface::getLock();
    if(strcasecmp(stMsgData->paramName,"Device.MoCA.InterfaceNumberOfEntries") == 0)
    {
        ret = hostIf_MoCAInterface::get_Device_MoCA_InterfaceNumberOfEntries(stMsgData);
    }
    else if(matchComponent(stMsgData->paramName,
                           "Device.MoCA.Interface",&pSetting,instanceNumber))
    {
        stMsgData->instanceNum = instanceNumber;
        hostIf_MoCAInterface *pIface = hostIf_MoCAInterface::getInstance(instanceNumber);
        hostIf_MoCAInterfaceStats *pIfaceStats
        = hostIf_MoCAInterfaceStats::getInstance(instanceNumber);
        hostIf_MoCAInterfaceQoS *pIfaceQoS = hostIf_MoCAInterfaceQoS::getInstance(instanceNumber);
        hostIf_MoCAInterfaceQoSFlowStats *pIfaceQoSFS
        = hostIf_MoCAInterfaceQoSFlowStats::getInstance(instanceNumber);
        hostIf_MoCAInterfaceAssociatedDevice *pIfaceAsstDev
        = hostIf_MoCAInterfaceAssociatedDevice::getInstance(instanceNumber);

        hostIf_MoCAInterfaceMeshTable *pIfaceMeshTableDev
        = hostIf_MoCAInterfaceMeshTable::getInstance(instanceNumber);

        if(!pIface)
        {
            hostIf_MoCAInterface::releaseLock();
            return NOK;
        }

        if (strcasecmp(pSetting,"Enable") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_Enable(stMsgData);
        }
        else if (strcasecmp(pSetting,"Status") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_Status(stMsgData);
        }
        else if (strcasecmp(pSetting,"Alias") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_Alias(stMsgData);
        }
        else if (strcasecmp(pSetting,"Name") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_Name(stMsgData);
        }
        else if (strcasecmp(pSetting,"LastChange") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_LastChange(stMsgData);
        }
        else if (strcasecmp(pSetting,"LowerLayers") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_LowerLayers(stMsgData);
        }
        else if (strcasecmp(pSetting,"Upstream") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_Upstream(stMsgData);
        }
        else if (strcasecmp(pSetting,"MACAddress") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_MACAddress(stMsgData);
        }
        else if (strcasecmp(pSetting,"FirmwareVersion") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_FirmwareVersion(stMsgData);
        }
        else if (strcasecmp(pSetting,"MaxBitRate") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_MaxBitRate(stMsgData);
        }
        else if (strcasecmp(pSetting,"MaxIngressBW") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_MaxIngressBW(stMsgData);
        }
        else if (strcasecmp(pSetting,"MaxEgressBW") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_MaxEgressBW(stMsgData);
        }
        else if (strcasecmp(pSetting,"HighestVersion") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_HighestVersion(stMsgData);
        }
        else if (strcasecmp(pSetting,"CurrentVersion") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_CurrentVersion(stMsgData);
        }
        else if (strcasecmp(pSetting,"NetworkCoordinator") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_NetworkCoordinator(stMsgData);
        }
        else if (strcasecmp(pSetting,"NodeID") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_NodeID(stMsgData);
        }
        else if (strcasecmp(pSetting,"MaxNodes") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_MaxNodes(stMsgData);
        }
        else if (strcasecmp(pSetting,"PreferredNC") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_PreferredNC(stMsgData);
        }
        else if (strcasecmp(pSetting,"BackupNC") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_BackupNC(stMsgData);
        }
        else if (strcasecmp(pSetting,"PrivacyEnabledSetting") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_PrivacyEnabledSetting(stMsgData);
        }
        else if (strcasecmp(pSetting,"PrivacyEnabled") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_PrivacyEnabled(stMsgData);
        }
        else if (strcasecmp(pSetting,"FreqCapabilityMask") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_FreqCapabilityMask(stMsgData);
        }
        else if (strcasecmp(pSetting,"FreqCurrentMaskSetting") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_FreqCurrentMaskSetting(stMsgData);
        }
        else if (strcasecmp(pSetting,"FreqCurrentMask") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_FreqCurrentMask(stMsgData);
        }
        else if (strcasecmp(pSetting,"CurrentOperFreq") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_CurrentOperFreq(stMsgData);
        }
        else if (strcasecmp(pSetting,"LastOperFreq") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_LastOperFreq(stMsgData);
        }
        else if (strcasecmp(pSetting,"KeyPassphrase") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_KeyPassphrase(stMsgData);
        }
        else if (strcasecmp(pSetting,"TxPowerLimit") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_TxPowerLimit(stMsgData);
        }
        else if (strcasecmp(pSetting,"PowerCntlPhyTarget") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_PowerCntlPhyTarget(stMsgData);
        }
        else if (strcasecmp(pSetting,"BeaconPowerLimit") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_BeaconPowerLimit(stMsgData);
        }
        else if (strcasecmp(pSetting,"NetworkTabooMask") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_NetworkTabooMask(stMsgData);
        }
        else if (strcasecmp(pSetting,"NodeTabooMask") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_NodeTabooMask(stMsgData);
        }
        else if (strcasecmp(pSetting,"TxBcastRate") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_TxBcastRate(stMsgData);
        }
        else if (strcasecmp(pSetting,"TxBcastPowerReduction") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_TxBcastPowerReduction(stMsgData);
        }
        else if (strcasecmp(pSetting,"QAM256Capable") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_QAM256Capable(stMsgData);
        }
        else if (strcasecmp(pSetting,"PacketAggregationCapability") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_PacketAggregationCapability(stMsgData);
        }
        else if (strcasecmp(pSetting,"AssociatedDeviceNumberOfEntries") == 0)
        {
            ret = pIface->get_Device_MoCA_Interface_AssociatedDeviceNumberOfEntries(stMsgData);
        }
        else if (strcasecmp(pSetting,"X_RDKCENTRAL-COM_MeshTableNumberOfEntries") == 0)
		{
			ret = pIface->get_Device_MoCA_Interface_X_RDKCENTRAL_COM_MeshTableNumberOfEntries(stMsgData);
		}
        else if (strcasecmp(pSetting,"Stats.BytesSent") == 0)
        {
            ret = pIfaceStats->get_Device_MoCA_Interface_Stats_BytesSent(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.BytesReceived") == 0)
        {
            ret = pIfaceStats->get_Device_MoCA_Interface_Stats_BytesReceived(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.PacketsSent") == 0)
        {
            ret = pIfaceStats->get_Device_MoCA_Interface_Stats_PacketsSent(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.PacketsReceived") == 0)
        {
            ret = pIfaceStats->get_Device_MoCA_Interface_Stats_PacketsReceived(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.ErrorsSent") == 0)
        {
            ret = pIfaceStats->get_Device_MoCA_Interface_Stats_ErrorsSent(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.ErrorsReceived") == 0)
        {
            ret = pIfaceStats->get_Device_MoCA_Interface_Stats_ErrorsReceived(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.UnicastPacketsSent") == 0)
        {
            ret = pIfaceStats->get_Device_MoCA_Interface_Stats_UnicastPacketsSent(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.UnicastPacketsReceived") == 0)
        {
            ret = pIfaceStats->get_Device_MoCA_Interface_Stats_UnicastPacketsReceived(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.DiscardPacketsSent") == 0)
        {
            ret = pIfaceStats->get_Device_MoCA_Interface_Stats_DiscardPacketsSent(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.DiscardPacketsReceived") == 0)
        {
            ret = pIfaceStats->get_Device_MoCA_Interface_Stats_DiscardPacketsReceived(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.MulticastPacketsSent") == 0)
        {
            ret = pIfaceStats->get_Device_MoCA_Interface_Stats_MulticastPacketsSent(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.MulticastPacketsReceived") == 0)
        {
            ret = pIfaceStats->get_Device_MoCA_Interface_Stats_MulticastPacketsReceived(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.BroadcastPacketsSent") == 0)
        {
            ret = pIfaceStats->get_Device_MoCA_Interface_Stats_BroadcastPacketsSent(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.BroadcastPacketsReceived") == 0)
        {
            ret = pIfaceStats->get_Device_MoCA_Interface_Stats_BroadcastPacketsReceived(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.UnknownProtoPacketsReceived") == 0)
        {
            ret = pIfaceStats->get_Device_MoCA_Interface_Stats_UnknownProtoPacketsReceived(stMsgData);
        }
        else if (strcasecmp(pSetting,"QoS.EgressNumFlows") == 0)
        {
            ret = pIfaceQoS->get_Device_MoCA_Interface_QoS_EgressNumFlows(stMsgData);
        }
        else if (strcasecmp(pSetting,"QoS.IngressNumFlows") == 0)
        {
            ret = pIfaceQoS->get_Device_MoCA_Interface_QoS_IngressNumFlows(stMsgData);
        }
        else if (strcasecmp(pSetting,"QoS.FlowStatsNumberOfEntries") == 0)
        {
            ret = pIfaceQoS->get_Device_MoCA_Interface_QoS_FlowStatsNumberOfEntries(stMsgData);
        }
        else if (matchComponent(pSetting,"QoS.FlowStats",
                                &pSubSetting,subInstanceNumber))
        {
            if (strcasecmp(pSubSetting,"FlowID") == 0)
            {
                ret = pIfaceQoSFS->get_Device_MoCA_Interface_QoS_FlowStats_FlowID(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"PacketDA") == 0)
            {
                ret = pIfaceQoSFS->get_Device_MoCA_Interface_QoS_FlowStats_PacketDA(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"MaxRate") == 0)
            {
                ret = pIfaceQoSFS->get_Device_MoCA_Interface_QoS_FlowStats_MaxRate(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"MaxBurstSize") == 0)
            {
                ret = pIfaceQoSFS->get_Device_MoCA_Interface_QoS_FlowStats_MaxBurstSize(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"LeaseTime") == 0)
            {
                ret = pIfaceQoSFS->get_Device_MoCA_Interface_QoS_FlowStats_LeaseTime(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"LeaseTimeLeft") == 0)
            {
                ret = pIfaceQoSFS->get_Device_MoCA_Interface_QoS_FlowStats_LeaseTimeLeft(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"FlowPackets") == 0)
            {
                ret = pIfaceQoSFS->get_Device_MoCA_Interface_QoS_FlowStats_FlowPackets(stMsgData,subInstanceNumber);
            }

        }
        else if(matchComponent(pSetting,"AssociatedDevice",
                               &pSubSetting,subInstanceNumber))
        {
            if (strcasecmp(pSubSetting,"MACAddress") == 0)
            {
                ret = pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_MACAddress(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"NodeID") == 0)
            {
                ret = pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_NodeID(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"PreferredNC") == 0)
            {
                ret = pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_PreferredNC(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"HighestVersion") == 0)
            {
                ret = pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_HighestVersion(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"PHYTxRate") == 0)
            {
                ret = pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_PHYTxRate(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"PHYRxRate") == 0)
            {
                ret = pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_PHYRxRate(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"TxPowerControlReduction") == 0)
            {
                ret = pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_TxPowerControlReduction(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"RxPowerLevel") == 0)
            {
                ret = pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_RxPowerLevel(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"TxBcastRate") == 0)
            {
                ret = pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_TxBcastRate(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"RxBcastPowerLevel") == 0)
            {
                ret = pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_RxBcastPowerLevel(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"TxPackets") == 0)
            {
                ret = pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_TxPackets(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"RxPackets") == 0)
            {
                ret = pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_RxPackets(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"RxErroredAndMissedPackets") == 0)
            {
                ret = pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_RxErroredAndMissedPackets(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"QAM256Capable") == 0)
            {
                ret = pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_QAM256Capable(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"PacketAggregationCapability") == 0)
            {
                ret = pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_PacketAggregationCapability(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"RxSNR") == 0)
            {
                ret = pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_RxSNR(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"Active") == 0)
            {
                ret = pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_Active(stMsgData,subInstanceNumber);
            }
        }
        else if(matchComponent(pSetting,"X_RDKCENTRAL-COM_MeshTable", &pSubSetting, subInstanceNumber))
        {
            if(strcasecmp(pSubSetting,"MeshTxNodeId") == 0)
            {
                ret = pIfaceMeshTableDev->get_Device_MoCA_Interface_X_RDKCENTRAL_COM_MeshTable_MeshTxNodeId(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"MeshRxNodeId") == 0)
            {
                ret = pIfaceMeshTableDev->get_Device_MoCA_Interface_X_RDKCENTRAL_COM_MeshTable_MeshRxNodeId(stMsgData,subInstanceNumber);
            }
            else if(strcasecmp(pSubSetting,"MeshPHYTxRate") == 0)
            {
                ret = pIfaceMeshTableDev->get_Device_MoCA_Interface_X_RDKCENTRAL_COM_MeshTable_MeshPHYTxRate(stMsgData,subInstanceNumber);
            }
        }

    }
    hostIf_MoCAInterface::releaseLock();
    return ret;
}

void MoCAClientReqHandler::registerUpdateCallback(updateCallback cb)
{
    mUpdateCallback = cb;
}

void MoCAClientReqHandler::checkForUpdates()
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    GList *elem;
    int index = 1;
    char tmp_buff[TR69HOSTIFMGR_MAX_PARAM_LEN];
    hostIf_MoCAInterface::getLock();

    //RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s]Entering \n", __FILE__, __FUNCTION__ );

    hostIf_MoCAInterface *pIface = hostIf_MoCAInterface::getInstance((int)0);

    if(pIface)
    {
        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        if(pIface->get_Device_MoCA_Interface_AssociatedDeviceNumberOfEntries(&msgData) == OK)
        {
            int tmpNoDev = get_int(msgData.paramValue);
            char tmp[TR69HOSTIFMGR_MAX_PARAM_LEN] = "";
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"AssociatedDevice");
            while(curNumOfDevices[index] > tmpNoDev)
            {
                sprintf(tmp,"%s.%d.",tmp_buff,tmpNoDev);
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE,tmp, NULL, hostIf_IntegerType);
                tmpNoDev++;
            }
            while(curNumOfDevices[index] < tmpNoDev)
            {
                sprintf(tmp,"%s.",tmp_buff);
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_ADD,tmp, NULL, hostIf_IntegerType);
                RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%s]Sending ADD Event for params: [%s] \n", __FILE__, __FUNCTION__ ,tmp);
                tmpNoDev--;
            }
            curNumOfDevices[index] = get_int(msgData.paramValue);
        }

        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        if(pIface->get_Device_MoCA_Interface_X_RDKCENTRAL_COM_MeshTableNumberOfEntries(&msgData) == OK)
        {
            int tmpNoDev = get_int(msgData.paramValue);
            char tmp[TR69HOSTIFMGR_MAX_PARAM_LEN] = "";
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"X_RDKCENTRAL-COM_MeshTable");
            while(numOfMocaMeshEntries > tmpNoDev)
            {
                sprintf(tmp,"%s.%d.",tmp_buff,tmpNoDev);
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE,tmp, NULL, hostIf_IntegerType);
                tmpNoDev++;
            }
            while(numOfMocaMeshEntries < tmpNoDev)
            {
                sprintf(tmp,"%s.",tmp_buff);
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_ADD,tmp, NULL, hostIf_IntegerType);
                RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%s]Sending ADD Event for params: [%s] \n", __FILE__, __FUNCTION__ ,tmp);
                tmpNoDev--;
            }
            numOfMocaMeshEntries = get_int(msgData.paramValue);
        }


    }
#ifdef HAVE_VALUE_CHANGE_EVENT
    GList *devList = hostIf_MoCAInterface::getAllIntefaces();
    for(elem = devList; elem; elem = elem->next,index++)
    {
        hostIf_MoCAInterface *pIface = hostIf_MoCAInterface::getInstance((int)elem->data);
        if(pIface)
        {
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_Enable(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"Enable");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_Status(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"Status");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_Alias(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"Alias");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_Name(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"Name");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_LastChange(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"LastChange");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_LowerLayers(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"LowerLayers");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_Upstream(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"Upstream");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_MACAddress(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"MACAddress");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_FirmwareVersion(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"FirmwareVersion");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_MaxBitRate(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"MaxBitRate");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_MaxIngressBW(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"MaxIngressBW");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_MaxEgressBW(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"MaxEgressBW");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_HighestVersion(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"HighestVersion");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_CurrentVersion(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"CurrentVersion");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_NetworkCoordinator(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"NetworkCoordinator");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_NodeID(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"NodeID");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_MaxNodes(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"MaxNodes");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_PreferredNC(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"PreferredNC");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_BackupNC(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"BackupNC");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_PrivacyEnabledSetting(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"PrivacyEnabledSetting");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_PrivacyEnabled(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"PrivacyEnabled");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_FreqCapabilityMask(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"FreqCapabilityMask");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_FreqCurrentMaskSetting(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"FreqCurrentMaskSetting");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_FreqCurrentMask(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"FreqCurrentMask");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_CurrentOperFreq(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"CurrentOperFreq");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_LastOperFreq(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"LastOperFreq");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_KeyPassphrase(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"KeyPassphrase");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_TxPowerLimit(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"TxPowerLimit");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_PowerCntlPhyTarget(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"PowerCntlPhyTarget");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_BeaconPowerLimit(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"BeaconPowerLimit");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_NetworkTabooMask(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"NetworkTabooMask");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_NodeTabooMask(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"NodeTabooMask");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_TxBcastRate(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"TxBcastRate");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_TxBcastPowerReduction(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"TxBcastPowerReduction");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_QAM256Capable(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"QAM256Capable");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            pIface->get_Device_MoCA_Interface_PacketAggregationCapability(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.%s",index,"PacketAggregationCapability");
                if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
            }
        }
    }
    g_list_free(devList);

    devList = hostIf_MoCAInterfaceAssociatedDevice::getAllAssociateDevs(); //hostIf_MoCAInterfaceStats::getAllIntefaceStats();
    index = 0;
    for(elem = devList; elem; elem = elem->next,index++)
    {
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);

        hostIf_MoCAInterfaceAssociatedDevice *pIfaceAsstDev = hostIf_MoCAInterfaceAssociatedDevice::getInstance((int)elem->data);

        for(int devNum = 1; devNum <= curNumOfDevices[index]; devNum++)
        {
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_MACAddress(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.AssociatedDevice.%d.%s",index,devNum,"MACAddress");
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_NodeID(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.AssociatedDevice.%d.%s",index,devNum,"NodeID");
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_PreferredNC(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.AssociatedDevice.%d.%s",index,devNum,"PreferredNC");
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_HighestVersion(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.AssociatedDevice.%d.%s",index,devNum,"HighestVersion");
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_PHYTxRate(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.AssociatedDevice.%d.%s",index,devNum,"PHYTxRate");
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_PHYRxRate(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.AssociatedDevice.%d.%s",index,devNum,"PHYRxRate");
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_TxPowerControlReduction(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.AssociatedDevice.%d.%s",index,devNum,"TxPowerControlReduction");
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_RxPowerLevel(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.AssociatedDevice.%d.%s",index,devNum,"RxPowerLevel");
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_TxBcastRate(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.AssociatedDevice.%d.%s",index,devNum,"TxBcastRate");
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_RxBcastPowerLevel(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.AssociatedDevice.%d.%s",index,devNum,"RxBcastPowerLevel");
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_TxPackets(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.AssociatedDevice.%d.%s",index,devNum,"TxPackets");
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_RxPackets(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.AssociatedDevice.%d.%s",index,devNum,"RxPackets");
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_RxErroredAndMissedPackets(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.AssociatedDevice.%d.%s",index,devNum,"RxErroredAndMissedPackets");
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_QAM256Capable(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.AssociatedDevice.%d.%s",index,devNum,"QAM256Capable");
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_PacketAggregationCapability(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.AssociatedDevice.%d.%s",index,devNum,"PacketAggregationCapability");
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_RxSNR(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.AssociatedDevice.%d.%s",index,devNum,"RxSNR");
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceAsstDev->get_Device_MoCA_Interface_AssociatedDevice_Active(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.AssociatedDevice.%d.%s",index,devNum,"Active");
            }

        }

    }
    g_list_free(devList);

    devList = hostIf_MoCAInterfaceStats::getAllIntefaceStats();
    index = 0;
    for(elem = devList; elem; elem = elem->next,index++)
    {

        hostIf_MoCAInterfaceStats *pIfaceStats = hostIf_MoCAInterfaceStats::getInstance((int)elem->data);

        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        bChanged = false;
        pIfaceStats->get_Device_MoCA_Interface_Stats_BytesSent(&msgData,&bChanged);
        if(bChanged)
        {
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.Stats.%s",index,"BytesSent");
        }
        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        bChanged = false;
        pIfaceStats->get_Device_MoCA_Interface_Stats_BytesReceived(&msgData,&bChanged);
        if(bChanged)
        {
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.Stats.%s",index,"BytesReceived");
        }
        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        bChanged = false;
        pIfaceStats->get_Device_MoCA_Interface_Stats_PacketsSent(&msgData,&bChanged);
        if(bChanged)
        {
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.Stats.%s",index,"PacketsSent");
        }
        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        bChanged = false;
        pIfaceStats->get_Device_MoCA_Interface_Stats_PacketsReceived(&msgData,&bChanged);
        if(bChanged)
        {
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.Stats.%s",index,"PacketsReceived");
        }
        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        bChanged = false;
        pIfaceStats->get_Device_MoCA_Interface_Stats_ErrorsSent(&msgData,&bChanged);
        if(bChanged)
        {
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.Stats.%s",index,"ErrorsSent");
        }
        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        bChanged = false;
        pIfaceStats->get_Device_MoCA_Interface_Stats_ErrorsReceived(&msgData,&bChanged);
        if(bChanged)
        {
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.Stats.%s",index,"ErrorsReceived");
        }
        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        bChanged = false;
        pIfaceStats->get_Device_MoCA_Interface_Stats_UnicastPacketsSent(&msgData,&bChanged);
        if(bChanged)
        {
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.Stats.%s",index,"UnicastPacketsSent");
        }
        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        bChanged = false;
        pIfaceStats->get_Device_MoCA_Interface_Stats_UnicastPacketsReceived(&msgData,&bChanged);
        if(bChanged)
        {
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.Stats.%s",index,"UnicastPacketsReceived");
        }
        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        bChanged = false;
        pIfaceStats->get_Device_MoCA_Interface_Stats_DiscardPacketsSent(&msgData,&bChanged);
        if(bChanged)
        {
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.Stats.%s",index,"DiscardPacketsSent");
        }
        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        bChanged = false;
        pIfaceStats->get_Device_MoCA_Interface_Stats_DiscardPacketsReceived(&msgData,&bChanged);
        if(bChanged)
        {
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.Stats.%s",index,"DiscardPacketsReceived");
        }
        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        bChanged = false;
        pIfaceStats->get_Device_MoCA_Interface_Stats_MulticastPacketsSent(&msgData,&bChanged);
        if(bChanged)
        {
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.Stats.%s",index,"MulticastPacketsSent");
        }
        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        bChanged = false;
        pIfaceStats->get_Device_MoCA_Interface_Stats_MulticastPacketsReceived(&msgData,&bChanged);
        if(bChanged)
        {
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.Stats.%s",index,"MulticastPacketsReceived");
        }
        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        bChanged = false;
        pIfaceStats->get_Device_MoCA_Interface_Stats_BroadcastPacketsSent(&msgData,&bChanged);
        if(bChanged)
        {
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.Stats.%s",index,"BroadcastPacketsSent");
        }
        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        bChanged = false;
        pIfaceStats->get_Device_MoCA_Interface_Stats_BroadcastPacketsReceived(&msgData,&bChanged);
        if(bChanged)
        {
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.Stats.%s",index,"BroadcastPacketsReceived");
        }
        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        bChanged = false;
        pIfaceStats->get_Device_MoCA_Interface_Stats_UnknownProtoPacketsReceived(&msgData,&bChanged);
        if(bChanged)
        {
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.Stats.%s",index,"UnknownProtoPacketsReceived");
        }

    }

    g_list_free(devList);

    int fsNumEntries = 0;

    devList = hostIf_MoCAInterfaceQoS::getAllInstances();

    for(elem = devList; elem; elem = elem->next,index++)
    {

        hostIf_MoCAInterfaceQoS *pIfaceQoS = hostIf_MoCAInterfaceQoS::getInstance((int)elem->data);

        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        bChanged = false;
        pIfaceQoS->get_Device_MoCA_Interface_QoS_EgressNumFlows(&msgData,&bChanged);
        if(bChanged)
        {
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.QoS.%s",index,"EgressNumFlows");
        }
        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        bChanged = false;
        pIfaceQoS->get_Device_MoCA_Interface_QoS_IngressNumFlows(&msgData,&bChanged);
        if(bChanged)
        {
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.QoS.%s",index,"IngressNumFlows");
        }
        memset(&msgData,0,sizeof(msgData));
        memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
        bChanged = false;
        if(pIfaceQoS->get_Device_MoCA_Interface_QoS_FlowStatsNumberOfEntries(&msgData,&bChanged) == OK)
        {
            fsNumEntries = get_int(msgData.paramValue);
        }
        if(bChanged)
        {
            sprintf(tmp_buff,"Device.MoCA.Interface.%d.QoS.%s",index,"FlowStatsNumberOfEntries");
        }

    }

    g_list_free(devList);

    devList = hostIf_MoCAInterfaceQoSFlowStats::getAllInstances();

    for(elem = devList; elem; elem = elem->next,index++)
    {

        hostIf_MoCAInterfaceQoSFlowStats *pIfaceQoSFS = hostIf_MoCAInterfaceQoSFlowStats::getInstance((int)elem->data);

        for(int devNum = 1; devNum <= fsNumEntries; devNum++)
        {

            memset(&msgData,devNum,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceQoSFS->get_Device_MoCA_Interface_QoS_FlowStats_FlowID(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.QoS.FlowStats.%d.%s",index,devNum,"FlowID");
            }
            memset(&msgData,devNum,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceQoSFS->get_Device_MoCA_Interface_QoS_FlowStats_PacketDA(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.QoS.FlowStats.%d.%s",index,devNum,"PacketDA");
            }
            memset(&msgData,devNum,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceQoSFS->get_Device_MoCA_Interface_QoS_FlowStats_MaxRate(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.QoS.FlowStats.%d.%s",index,devNum,"MaxRate");
            }
            memset(&msgData,devNum,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceQoSFS->get_Device_MoCA_Interface_QoS_FlowStats_MaxBurstSize(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.QoS.FlowStats.%d.%s",index,devNum,"MaxBurstSize");
            }
            memset(&msgData,devNum,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceQoSFS->get_Device_MoCA_Interface_QoS_FlowStats_LeaseTime(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.QoS.FlowStats.%d.%s",index,devNum,"LeaseTime");
            }
            memset(&msgData,devNum,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceQoSFS->get_Device_MoCA_Interface_QoS_FlowStats_LeaseTimeLeft(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.QoS.FlowStats.%d.%s",index,devNum,"LeaseTimeLeft");
            }
            memset(&msgData,devNum,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged = false;
            pIfaceQoSFS->get_Device_MoCA_Interface_QoS_FlowStats_FlowPackets(&msgData,devNum,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.MoCA.Interface.%d.QoS.FlowStats.%d.%s",index,devNum,"FlowPackets");
            }

        }
    }

    g_list_free(devList);
#endif /* HAVE_VALUE_CHANGE_EVENT */
    hostIf_MoCAInterface::releaseLock();
}
#endif /* USE_MoCA_PROFILE */


/** @} */
/** @} */
