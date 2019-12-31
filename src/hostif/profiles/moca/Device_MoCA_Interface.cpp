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
 * @file Device_MoCA_Interface.c
 *
 * @brief MoCA_Interface API Implementation.
 *
 * This is the implementation of the MoCA_Interface API.
 *
 * @par Document
 * TBD Relevant design or API documentation.
 *
 */

/** @addtogroup MoCA_Interface Implementation
 *  This is the implementation of the Device Public API.
 *  @{
 */

/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/

#include <string.h>
#include <math.h>

#include "Device_MoCA_Interface.h"
#include "rdk_moca_hal.h"

#define MOCA_INTERFACE_NUMBER_OF_ENTRIES 1
#define MAX_WAIT_FOR_RMH_SECONDS         10
#define HIGHEST_MOCA_VERSION    0X20

MoCADevice* MoCADevice::Instance = NULL;


MoCADevice* MoCADevice::getInstance()
{
    if(!Instance) {
        Instance = new MoCADevice();
    }
    return Instance;
}

void* MoCADevice::getRmhContext()
{
    /* Initialized the RMH Context */
    static RMH_Handle rmhContext = NULL;
    uint32_t waitedSeconds = 0;
    bool alwayRecreate = true; /* XITHREE-7905 */

    if (alwayRecreate && rmhContext) {
        RMH_Destroy(rmhContext);
        rmhContext=NULL;
    }

    /* If we have an RMH handle, ensure it's still valid */
    if (rmhContext) {
        RMH_Result ret = RMH_ValidateHandle(rmhContext);
        if (ret == RMH_UNIMPLEMENTED || ret == RMH_NOT_SUPPORTED) {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"RMH_ValidateHandle returned %s. We will not be able to monitor to make sure the handle remains valid\n", RMH_ResultToString(ret));
        }
        else if (ret != RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"The RMH handle %p seems to no longer be valid. Closing\n", rmhContext);
            RMH_Destroy(rmhContext);
            rmhContext=NULL;
        }
    }

    /* If we don't have an RMH handle, create one*/
    while (rmhContext == NULL) {
        rmhContext = RMH_Initialize(NULL, NULL);
        if(NULL != rmhContext) {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"Created RMH handle %p\n", rmhContext);
            break;
        }

        if (waitedSeconds >= MAX_WAIT_FOR_RMH_SECONDS) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"(%s) Maximum wait for MoCA reached. Skipping RMH Initialization.!\n", __FUNCTION__);
            //throw 1; Leading to program termination. Better to return failure rather than terminating process to allow access to other profiles.
            break;
        }

        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"(%s)Failed in RMH_Initialize. Assuming MoCA isn't ready. Waiting...\n", __FUNCTION__);
        usleep(1*1000000);
        waitedSeconds++;
    }

    return rmhContext;
}

void* MoCADevice::closeRmhContext() {
    RMH_Handle rmhContext = (RMH_Handle)getRmhContext();
    if(rmhContext) {
        RMH_Destroy(rmhContext);
    }
}



void MoCADevice::closeInstance()
{
    closeRmhContext();
}

int MoCAInterface::get_Associated_Device_NumberOfEntries(int &numDevices)
{
    int nodeCount = 0;
    int retval = NOK;

    RMH_Handle rmh=(RMH_Handle)MoCADevice::getRmhContext();

    if (rmh) {
        RMH_NodeList_Uint32_t response;
        RMH_Result ret = RMH_Network_GetAssociatedIds(rmh, &response);
        if (ret != RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetAssociatedIds failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return retval;
        }
        else {
            for (int nodeId = 0; nodeId < RMH_MAX_MOCA_NODES; nodeId++) {
                if (true ==response.nodePresent[nodeId]) {
                    nodeCount++;
                }
            }
        }
    }
    if(nodeCount) {
        numDevices = nodeCount;
        m_ui32AssociatedDeviceNumberOfEntries = nodeCount;
        retval = OK;
    }
    return retval;
}


int MoCAInterface::get_MoCA_Mesh_NumberOfEntries(int &numOfEntries)
{
    int nodeCount = 0;
    int retval = NOK;

    RMH_Handle rmh=(RMH_Handle)MoCADevice::getRmhContext();

    if (rmh) {
        RMH_NodeList_Uint32_t response;
        RMH_Result ret = RMH_Network_GetNodeIds(rmh, &response);
        if (ret != RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetNodeIds failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return retval;
        }
        else {
            for (int nodeId = 0; nodeId < RMH_MAX_MOCA_NODES; nodeId++) {
                if (true ==response.nodePresent[nodeId]) {
                    nodeCount++;
                }
            }
        }
    }

    if(nodeCount) {
        m_ui32X_RDKCENTRAL_COM_MeshTableNumberOfEntries  = (int)((int)pow(nodeCount, 2) - (nodeCount));
        numOfEntries = m_ui32X_RDKCENTRAL_COM_MeshTableNumberOfEntries;
        retval = OK;
    }

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] MoCA Mesh number of entries : [%u]\n", __FILE__, __FUNCTION__, numOfEntries);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exit \n", __FILE__, __FUNCTION__);
    return retval;
}

GHashTable* MoCAInterface::m_notifyHash = NULL;
GMutex* MoCAInterface::m_mutex = NULL;
MoCAInterface* MoCAInterface::Instance = NULL;

MoCAInterface::MoCAInterface(int dev_id):
    m_i32Dev_id(dev_id),
    m_i32CurNumOfDevices(0),
    m_bEnable(false),
    m_ui32LastChange(0),
    m_bUpStream(false),
    m_ui32MaxBitRate(0),
    m_ui32MaxIngressBW(0),
    m_ui32MaxEgressBW(0),
    m_HighestVersion(0),
    m_ui32NetworkCoordinator(0),
    m_ui32NodeId(0),
    m_bMaxNodes(false),
    m_bPreferredNC(false),
    m_ui32BackupNC(0),
    m_bPrivacyEnabledSetting(false),
    m_bPrivacyEnabled(false),
    m_ui32CurrentOperFreq(0),
    m_ui32LastOperFreq(0),
    m_ui32TxPowerLimit(0),
    m_ui32PowerCntlPhyTarget(0),
    m_ui32BeaconPowerLimit(0),
    m_ui32TxBcastRate(0),
    m_ui32TxBcastPowerReduction(0),
    m_bQAM256Capable(false),
    m_ui32PacketAggregationCapability(0),
    m_ui32AssociatedDeviceNumberOfEntries(0),
    m_ui32X_RDKCENTRAL_COM_MeshTableNumberOfEntries(0)
{
    memset(m_i8Alias, 0,N_LENGTH);
    memset(m_i8Name, 0, N_LENGTH);
    memset(m_i8LowerLayers, 0,LL_LENGTH);
    memset(m_i8FreqCapabilityMask, 0, FREQ_LENGTH);
    memset(m_i8NetworkTabooMask, 0, FREQ_LENGTH);
    memset(m_i8NodeTabooMask, 0, FREQ_LENGTH);
}

MoCAInterface* MoCAInterface::getInstance(int _dev_Id)
{
    if(NULL == Instance) {
        Instance = new MoCAInterface(0);
    }
    return Instance;
}

void MoCAInterface::closeInstance()
{
    if(Instance) {
        delete Instance;
    }
}

void MoCAInterface::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void MoCAInterface::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

GHashTable*  MoCAInterface::getNotifyHash()
{
    if(m_notifyHash)
    {
        return m_notifyHash;
    }
    else
    {
        return m_notifyHash = g_hash_table_new(g_str_hash, g_str_equal);
    }
}


/****************************************************************************************************************************************************/
// Device.MoCA.Interface Table Profile. Getters:
/****************************************************************************************************************************************************/

int MoCAInterface::get_InterfaceNumberOfEntries(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    put_int(stMsgData->paramValue,MOCA_INTERFACE_NUMBER_OF_ENTRIES);
    stMsgData->paramtype=hostIf_IntegerType;
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] MOCA_INTERFACE_NUMBER_OF_ENTRIES :%d\n", __FUNCTION__, __FILE__, MOCA_INTERFACE_NUMBER_OF_ENTRIES);
    stMsgData->paramLen=sizeof(unsigned int);
    //RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int MoCAInterface::get_Enable(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=sizeof(bool);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        bool enable = false;
        RMH_Result ret = RMH_Self_GetEnabled(rmh, &enable);
        if (ret == RMH_SUCCESS) {
            put_boolean(stMsgData->paramValue, enable);
            m_bEnable = enable;
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetLinkStatus success with result %s value %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), enable );
        }
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int MoCAInterface::get_Status(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    stMsgData->paramtype = hostIf_StringType;
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        RMH_LinkStatus status;
        RMH_Result ret = RMH_Self_GetLinkStatus(rmh, &status);
        if (ret == RMH_SUCCESS) {

            switch (status) {
            case RMH_LINK_STATUS_DISABLED:
                snprintf(stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN-1, "%s", "Disable");
                break;
            case RMH_LINK_STATUS_NO_LINK:
                snprintf(stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN-1, "%s", "LowerLayerDown");
                break;
            case RMH_LINK_STATUS_INTERFACE_DOWN:
                snprintf(stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN-1, "%s", "Down");
                break;
            case RMH_LINK_STATUS_UP:
                snprintf(stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN-1, "%s", "Up");
                break;
            default:
                snprintf(stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN-1, "%s","Dormant");
                break;
            }

            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetLinkStatus success with result %s value %s. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), RMH_LinkStatusToString(status));
            stMsgData->paramLen = strlen(stMsgData->paramValue);
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetLinkStatus failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return NOK;
        }
    }
    return OK;
}

int MoCAInterface::get_Alias(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    snprintf(stMsgData->paramValue, N_LENGTH-1, "cpe-%s", m_i8Alias);
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    return OK;
}

int MoCAInterface::get_Name(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        int resBufSize = 20;
        char responseBuf[resBufSize];

        RMH_Result ret = RMH_Interface_GetName(rmh,responseBuf, resBufSize);
        if (ret == RMH_SUCCESS) {
            snprintf(stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN-1, "%s", responseBuf );
            stMsgData->paramLen = strlen(stMsgData->paramValue);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Interface_GetName success with result %s value %s. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), responseBuf);
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Interface_GetName failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return NOK;
        }
    }
    stMsgData->paramtype = hostIf_StringType;
    return OK;
}

int MoCAInterface::get_LastChange(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=sizeof(unsigned int);
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t ui32Response = 0;
        RMH_Result ret = RMH_Network_GetLinkUptime(rmh,&ui32Response);
        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetLinkUptime success with result %s and value as %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), ui32Response);
            put_int(stMsgData->paramValue,ui32Response);
            retval = OK;
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetLinkUptime failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    return retval;
}

int MoCAInterface::get_LowerLayers(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    /*Retrieving value */
    // Implementation for SOC vendor
    // value->out_cval =
    return OK;
}
int MoCAInterface::get_Upstream(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    /* As per Broadband refer: http://www.broadband-forum.org/cwmp/tr-181-2-2-0.html#D.Device.MoCA.Interface.{i}.Upstream
     * Indicates whether the interface points towards the Internet (true) or towards End Devices (false).
     * For example:
     *  For an Internet Gateway Device, Upstream will be true for all WAN interfaces and false for all LAN interfaces.
     *  For a standalone WiFi Access Point that is connected via Ethernet to an Internet Gateway Device, Upstream will
     *  be true for the Ethernet interface and false for the WiFi Radio interface.
     *  For an End Device, Upstream will be true for all interfaces.
    */
    bool bUpsteam = false;
    bUpsteam = (m_bEnable)?true:false;
    put_boolean(stMsgData->paramValue, bUpsteam);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=1;
    //RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..", __FUNCTION__, __FILE__);
    return OK;
}
int MoCAInterface::get_MACAddress(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();

    if(rmh) {
        RMH_MacAddress_t response;
        RMH_Result ret = RMH_Interface_GetMac(rmh, &response);

        if (ret == RMH_SUCCESS) {
            char macBuff[18] = {'\0'};
            RMH_MacToString(response, macBuff, sizeof(macBuff));
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%u] RMH_Interface_GetMac success with result %s value %s. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), macBuff);
            snprintf(stMsgData->paramValue,  TR69HOSTIFMGR_MAX_PARAM_LEN-1, "%s", macBuff);
            stMsgData->paramLen = strlen(macBuff);
            retval = OK;
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Interface_GetMac failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    stMsgData->paramtype = hostIf_StringType;
    return retval;
}
int MoCAInterface::get_FirmwareVersion(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();

    if(rmh) {
        int resBufSize = 20;
        char responseBuf[resBufSize];

        RMH_Result ret = RMH_Self_GetSoftwareVersion(rmh,responseBuf, resBufSize);
        if (ret == RMH_SUCCESS) {
            snprintf(stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN-1, "%s", responseBuf );
            stMsgData->paramLen = strlen(stMsgData->paramValue);
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetSoftwareVersion success with result %s value %s. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), responseBuf );
            retval = OK;
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetSoftwareVersion failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    stMsgData->paramtype = hostIf_StringType;
    return retval;
}
int MoCAInterface::get_MaxBitRate(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=sizeof(unsigned int);
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t ui32Response = 0;
        RMH_Result ret = RMH_Self_GetMaxBitrate(rmh,&ui32Response);
        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetMaxBitrate success with result %s and value as %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), ui32Response);
            put_int(stMsgData->paramValue,ui32Response);
            retval = OK;
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetMaxBitrate failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    return retval;
}
int MoCAInterface::get_MaxIngressBW(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=sizeof(unsigned int);
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t ui32Response = 0;
        RMH_Result ret = RMH_PQOS_GetMaxIngressFlows(rmh,&ui32Response);

        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_PQOS_GetMaxIngressFlows success with result %s and value as %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), ui32Response);
            put_int(stMsgData->paramValue,ui32Response);
            retval = OK;
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_PQOS_GetMaxIngressFlows failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    return retval;
}

int MoCAInterface::get_MaxEgressBW(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=sizeof(unsigned int);
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t ui32Response;
        RMH_Result ret = RMH_PQoS_GetMaxEgressBandwidth(rmh, &ui32Response);

        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_PQoS_GetMaxEgressBandwidth success with result %s and value as %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), ui32Response);
            put_int(stMsgData->paramValue,ui32Response);
            retval = OK;
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_PQoS_GetMaxEgressBandwidth failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    return retval;
}
int MoCAInterface::get_HighestVersion(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_StringType;
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        RMH_MoCAVersion hVer;
        RMH_Result ret = RMH_Self_GetHighestSupportedMoCAVersion(rmh, &hVer);

        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetHighestSupportedMoCAVersion success with result %s and value as %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), hVer);
            m_HighestVersion = hVer;
            snprintf(stMsgData->paramValue, (TR69HOSTIFMGR_MAX_PARAM_LEN-1),  "%s", RMH_MoCAVersionToString(hVer));
            stMsgData->paramLen = strlen(stMsgData->paramValue);
            retval = OK;
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetHighestSupportedMoCAVersion failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    return retval;
}

int MoCAInterface::get_CurrentVersion(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_StringType;
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        RMH_MoCAVersion response;
        RMH_Result ret = RMH_Network_GetMoCAVersion(rmh, &response);

        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetMoCAVersion success with result %s and value as %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), response);
            snprintf(stMsgData->paramValue, (TR69HOSTIFMGR_MAX_PARAM_LEN-1),  "%s", RMH_MoCAVersionToString(response));
            stMsgData->paramLen = strlen(stMsgData->paramValue);
            retval = OK;
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetMoCAVersion failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    return retval;
}
int MoCAInterface::get_NetworkCoordinator(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=sizeof(unsigned int);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t ui32Response;
        RMH_Result ret = RMH_Network_GetNCNodeId(rmh, &ui32Response);

        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetNCNodeId success with result %s and value as %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), ui32Response);
            put_int(stMsgData->paramValue,ui32Response);
            retval = OK;
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetNCNodeId failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    return retval;
}
int MoCAInterface::get_NodeID(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=sizeof(unsigned int);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t ui32Response;
        RMH_Result ret = RMH_Network_GetNodeId(rmh, &ui32Response);

        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetNodeId success with result %s and value as %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), ui32Response);
            put_int(stMsgData->paramValue,ui32Response);
            retval = OK;
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetNodeId failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    return retval;
}
int MoCAInterface::get_MaxNodes(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    bool maxNodes = (m_HighestVersion == HIGHEST_MOCA_VERSION)? true : false;
    put_boolean(stMsgData->paramValue, maxNodes);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=sizeof(bool);
    return OK;
}
int MoCAInterface::get_PreferredNC(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        bool enable;
        RMH_Result ret = RMH_Self_GetPreferredNCEnabled(rmh, &enable);

        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetNodeId success with result %s and value as %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), enable);
            put_boolean(stMsgData->paramValue,enable);
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetNodeId failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return NOK;
        }
    }
    return OK;
}

int MoCAInterface::get_BackupNC(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=sizeof(unsigned int);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t backupNC;
        RMH_Result ret = RMH_Network_GetBackupNCNodeId(rmh, &backupNC);

        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetNodeId success with result %s and value as %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), backupNC);
            put_int(stMsgData->paramValue,backupNC);
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetNodeId failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return NOK;
        }
    }
    return OK;
}
int MoCAInterface::get_PrivacyEnabledSetting(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    bool bEnable = false;
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=sizeof(bool);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        RMH_Result ret = RMH_Self_GetPrivacyEnabled(rmh, &bEnable);

        if (ret == RMH_SUCCESS) {
            put_boolean(stMsgData->paramValue,bEnable);
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetPrivacyEnabled failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return NOK;
        }
    }
    return OK;
}
int MoCAInterface::get_PrivacyEnabled(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    bool bEnable = false;
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=sizeof(bool);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        RMH_Result ret = RMH_Self_GetPrivacyEnabled(rmh, &bEnable);

        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetPrivacyEnabled success with result %s and value as %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), bEnable);
            put_boolean(stMsgData->paramValue,bEnable);
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetPrivacyEnabled failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return NOK;
        }
    }
    return OK;
}
int MoCAInterface::get_FreqCapabilityMask(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    stMsgData->paramtype = hostIf_StringType;
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        size_t responseBufUsed;
        const size_t bufLen = 256;
        uint32_t responseBuf[bufLen] = {'\0'};

        RMH_Result ret = RMH_Self_GetSupportedFrequencies (rmh, responseBuf, bufLen, &responseBufUsed);

        if (ret == RMH_SUCCESS) {
            int length = 0;
            length += snprintf(stMsgData->paramValue + length, TR69HOSTIFMGR_MAX_PARAM_LEN-1, "0x" );
            //RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%u] responseBufUsed : %d. \n", __FUNCTION__, __LINE__, responseBufUsed);
            for (int i=0; i < responseBufUsed; i++) {
                length += snprintf(stMsgData->paramValue + length, TR69HOSTIFMGR_MAX_PARAM_LEN-1, "%02X", responseBuf[i] );
            }
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetSupportedFrequencies success with result %s and value as %s. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret),stMsgData->paramValue);
            stMsgData->paramLen = strlen(stMsgData->paramValue);
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u]RMH_Self_GetSupportedFrequencies failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return NOK;
        }
    }
    return OK;
}

int MoCAInterface::get_FreqCurrentMaskSetting(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_StringType;

    RMH_Handle rmh=(RMH_Handle)MoCADevice::getRmhContext();
    memset(stMsgData->paramValue, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN);

    if (rmh) {
        uint32_t response;
        RMH_Result ret = RMH_Self_GetFrequencyMask(rmh, &response);

        if (ret == RMH_SUCCESS) {
            snprintf(stMsgData->paramValue, (TR69HOSTIFMGR_MAX_PARAM_LEN-1), "0x%08x", response);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetFrequencyMask success with result %s value %s. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), stMsgData->paramValue);
            retval = OK;
        } else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetFrequencyMask failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    return retval;
}

int MoCAInterface::get_FreqCurrentMask(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_StringType;

    RMH_Handle rmh=(RMH_Handle)MoCADevice::getRmhContext();
    memset(stMsgData->paramValue, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN);

    if (rmh) {
        uint32_t response;
        RMH_Result ret = RMH_Self_GetFrequencyMask(rmh, &response);

        if (ret == RMH_SUCCESS) {
            snprintf(stMsgData->paramValue, (TR69HOSTIFMGR_MAX_PARAM_LEN-1), "0x%08x", response);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetFrequencyMask success with result %s value %s. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), stMsgData->paramValue);
            retval = OK;
        } else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetFrequencyMask failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    return retval;
}

int MoCAInterface::get_CurrentOperFreq(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_IntegerType;
    stMsgData->paramLen=sizeof(int);

    RMH_Handle rmh=(RMH_Handle)MoCADevice::getRmhContext();
    if (rmh) {
        uint32_t response;
        RMH_Result ret = RMH_Network_GetRFChannelFreq(rmh, &response);

        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetRFChannelFreq success with result %s value %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), response);
            put_int(stMsgData->paramValue, response);
            retval = OK;
        } else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetRFChannelFreq failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    return retval;
}

int MoCAInterface::get_LastOperFreq(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_IntegerType;
    stMsgData->paramLen=sizeof(int);

    RMH_Handle rmh=(RMH_Handle)MoCADevice::getRmhContext();
    if (rmh) {
        uint32_t response;
        RMH_Result ret = RMH_Self_GetLOF(rmh, &response);

        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetLOF success with result %s value %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), response);
            put_int(stMsgData->paramValue, response);
            retval = OK;
        } else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetLOF failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    return retval;
}

int MoCAInterface::get_KeyPassphrase(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen=0;
    /* As per specs, "When read, this parameter returns an empty string, regardless of the actual value." */
    memset(stMsgData->paramValue, 0, TR69HOSTIFMGR_MAX_PARAM_LEN);
    return OK;
}

int MoCAInterface::get_TxPowerLimit(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_IntegerType;
    stMsgData->paramLen=sizeof(int);
    RMH_Handle rmh=(RMH_Handle)MoCADevice::getRmhContext();
    memset(stMsgData->paramValue, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN);

    if (rmh) {
        int32_t response;
        RMH_Result ret = RMH_Self_GetTxPowerLimit(rmh, &response);
        if (ret == RMH_SUCCESS) {
            put_int(stMsgData->paramValue, response);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetTxPowerLimit success with result %s value %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), response);
            retval = OK;
        } else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetTxPowerLimit failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    else {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s:%u] Failed in RMH_Initialize. Not updating cache.\n", __FUNCTION__, __LINE__);
    }
    return retval;
}

int MoCAInterface::get_PowerCntlPhyTarget(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=sizeof(unsigned int);

    RMH_Handle rmh=(RMH_Handle)MoCADevice::getRmhContext();
    memset(stMsgData->paramValue, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN);

    if (rmh) {
        uint32_t response = 0;
        RMH_Result ret = RMH_Self_GetPrimaryChannelTargetPhyRate(rmh, &response);
        if (ret == RMH_SUCCESS) {
            put_int(stMsgData->paramValue, response);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u]RMH_Self_GetPrimaryChannelTargetPhyRate success with result %s value %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), response);
            retval = OK;
        } else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u]RMH_Self_GetPrimaryChannelTargetPhyRate failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    return retval;
}

int MoCAInterface::get_BeaconPowerLimit(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    RMH_Handle rmh=(RMH_Handle)MoCADevice::getRmhContext();
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=sizeof(unsigned int);

    unsigned int response;
    if (rmh) {
        RMH_Result ret = RMH_Power_GetTxBeaconPowerReduction(rmh, &response);

        if (ret == RMH_SUCCESS) {
            put_int(stMsgData->paramValue, response);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Power_GetTxBeaconPowerReduction success with result %s value %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), response);
            retval = OK;
        } else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Power_GetTxBeaconPowerReduction failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    else {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s:%u] Failed in RMH_Initialize. Not updating cache.\n", __FUNCTION__, __LINE__);
    }

    return retval;
}

int MoCAInterface::get_NetworkTabooMask(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    stMsgData->paramtype = hostIf_StringType;
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t start = 0;
        uint32_t mask = 0;
        RMH_Result ret = RMH_Network_GetTabooChannels(rmh, &start, &mask);

        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetTabooChannels success with result %s and value as 0x%08x. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), mask);
            snprintf(stMsgData->paramValue,TR69HOSTIFMGR_MAX_PARAM_LEN-1, "0x%08x", mask );
            stMsgData->paramLen = strlen(stMsgData->paramValue);
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u]RMH_Network_GetTabooChannels failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return NOK;
        }
    }
    return OK;
}

int MoCAInterface::get_NodeTabooMask(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    stMsgData->paramtype = hostIf_StringType;
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t start = 0;
        uint32_t mask = 0;
        RMH_Result ret = RMH_Self_GetTabooChannels(rmh, &start, &mask);

        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u]RMH_Self_GetTabooChannels success with result %s and value as 0x%08x. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), mask);
            snprintf(stMsgData->paramValue,TR69HOSTIFMGR_MAX_PARAM_LEN-1, "0x%08x", mask );
            stMsgData->paramLen = strlen(stMsgData->paramValue);
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u]RMH_Self_GetTabooChannels failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return NOK;
        }
    }
    return OK;
}

int MoCAInterface::get_TxBcastRate(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=sizeof(unsigned int);
    RMH_Handle rmh=(RMH_Handle)MoCADevice::getRmhContext();
    if (rmh) {
        uint32_t ui32Response;
        RMH_Result ret = RMH_Network_GetTxBroadcastPhyRate(rmh, &ui32Response);
        if (ret == RMH_SUCCESS) {
            put_int(stMsgData->paramValue,ui32Response);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetTxBroadcastPhyRate success with result %s value %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), ui32Response);
            retval = OK;
        } else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetTxBroadcastPhyRate failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    return retval;
}
int MoCAInterface::get_TxBcastPowerReduction(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=sizeof(unsigned int);

    RMH_Handle rmh=(RMH_Handle)MoCADevice::getRmhContext();
    if (rmh) {
        uint32_t response;
        RMH_Result ret = RMH_Power_GetTxBeaconPowerReduction(rmh, &response);
        if (ret == RMH_SUCCESS) {
            put_int(stMsgData->paramValue,response);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Power_GetTxBeaconPowerReduction success with result %s value %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), response);
            retval = OK;
        } else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Power_GetTxBeaconPowerReduction failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    return retval;
}

int MoCAInterface::get_QAM256Capable(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=sizeof(bool);

    RMH_Handle rmh=(RMH_Handle)MoCADevice::getRmhContext();
    if (rmh) {
        bool response = false;
        RMH_Result ret = RMH_Self_GetQAM256Enabled(rmh, &response);
        if (ret == RMH_SUCCESS) {
            put_boolean(stMsgData->paramValue,response);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetQAM256Enabled success with result %s value %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), response);
            retval = OK;
        } else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetQAM256Enabled failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    return retval;
}

int MoCAInterface::get_PacketAggregationCapability(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=sizeof(unsigned int);
    RMH_Handle rmh=(RMH_Handle)MoCADevice::getRmhContext();
    if (rmh) {
        uint32_t response = 0;
        RMH_Result ret = RMH_Self_GetMaxPacketAggregation(rmh, &response);
        if (ret == RMH_SUCCESS) {
            put_int(stMsgData->paramValue,response);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_RemoteNode_GetMaxPacketAggregation success with result %s value %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), response);
            retval = OK;
        } else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_RemoteNode_GetMaxPacketAggregation failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    return retval;
}

int MoCAInterface::get_AssociatedDeviceNumberOfEntries(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int num_entries;
    int ret  = get_Associated_Device_NumberOfEntries(num_entries);
    put_int(stMsgData->paramValue,num_entries);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=sizeof(unsigned int);
    return ret;
}

int MoCAInterface::get_X_RDKCENTRAL_COM_MeshTableNumberOfEntries(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int num_entries;
    int ret  = get_MoCA_Mesh_NumberOfEntries(num_entries);
    put_int(stMsgData->paramValue,num_entries);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=sizeof(unsigned int);
    return ret;
}

int MoCAInterface::get_X_RDKCENTRAL_COM_PrimaryChannelFreq(HOSTIF_MsgData_t *stMsgData)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=sizeof(unsigned int);

    RMH_Handle rmh=(RMH_Handle)MoCADevice::getRmhContext();
    if (rmh) {
        uint32_t response = 0;
        RMH_Result ret = RMH_Network_GetPrimaryChannelFreq(rmh, &response);
        if (ret == RMH_SUCCESS) {
            put_int(stMsgData->paramValue,response);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetPrimaryChannelFreq success with result %s value %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), response);
            retval = OK;
        } else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetPrimaryChannelFreq failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    return retval;
}

int MoCAInterface::get_X_RDKCENTRAL_COM_SecondaryChannelFreq(HOSTIF_MsgData_t *stMsgData)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=sizeof(unsigned int);

    RMH_Handle rmh=(RMH_Handle)MoCADevice::getRmhContext();
    if (rmh) {
        uint32_t response = 0;
        RMH_Result ret = RMH_Network_GetSecondaryChannelFreq(rmh, &response);
        if (ret == RMH_SUCCESS) {
            put_int(stMsgData->paramValue,response);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetSecondaryChannelFreq success with result %s value %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), response);
            retval = OK;
        } else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetSecondaryChannelFreq failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    return retval;
}

int MoCAInterface::get_X_RDKCENTRAL_COM_NodePowerState(HOSTIF_MsgData_t *stMsgData)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=sizeof(unsigned int);
    RMH_Handle rmh=(RMH_Handle)MoCADevice::getRmhContext();
    if (rmh) {
        RMH_PowerMode response;
        RMH_Result ret = RMH_Power_GetMode(rmh, &response);
        if (ret == RMH_SUCCESS) {
            put_int(stMsgData->paramValue,response);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u]RMH_Power_GetMode success with result %s value %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), response);
            retval = OK;
        } else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u]RMH_Power_GetMode failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    return retval;
}

int MoCAInterface::get_X_RDKCENTRAL_COM_TurboMode(HOSTIF_MsgData_t *stMsgData)
{
    int retval = NOK;
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=sizeof(unsigned int);

    RMH_Handle rmh=(RMH_Handle)MoCADevice::getRmhContext();
    if (rmh) {
        bool response = 0;
        RMH_Result ret = RMH_Self_GetTurboEnabled(rmh, &response);
        if (ret == RMH_SUCCESS) {
            put_boolean(stMsgData->paramValue,response);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetSecondaryChannelFreq success with result %s value %d. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), response);
            retval = OK;
        } else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetSecondaryChannelFreq failed with result %s.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
        }
    }
    return retval;
}

int MoCAInterface::check_MoCABootStatus(char *statusBuf)
{
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        RMH_LinkStatus status;
        RMH_Result ret = RMH_Self_GetLinkStatus(rmh, &status);

        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_GetLinkStatus success with result %s value %s. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret), RMH_LinkStatusToString(status));
            // Check MoCA link status
            if (status != RMH_LINK_STATUS_UP)
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Coax connection failed, MoCA disabled.\n", __FILE__, __FUNCTION__);
                return NOK;
            }
            else {
                sprintf(statusBuf, "%s", "Coax connection confirmed, MoCA enabled");
                RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Status: %s\n", __FILE__, __FUNCTION__, statusBuf);
            }

            uint32_t ncId;
            if(RMH_SUCCESS != RMH_Network_GetNCNodeId(rmh, &ncId)) {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to discovering MoCA Network Coordinator.\n", __FILE__, __FUNCTION__);
                return NOK;
            }

            RMH_MacAddress_t ncMac;
            if (RMH_SUCCESS != RMH_Network_GetNCMac(rmh, &ncMac)) {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to discovering MoCA Network Coordinator.\n", __FILE__, __FUNCTION__);
                return NOK;
            }
            else {
                char macBuff[20] = {'\0'};
                RMH_MacToString(ncMac, macBuff, sizeof(macBuff));
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%u] RMH_Interface_GetMac success with Mac Address : %s. \n", __FUNCTION__, __LINE__, macBuff);
                sprintf(statusBuf, "%s %d %s %s", "Discovering MoCA Network Coordinator:", ncId, " MoCA MAC: ",  macBuff);
            }

            int numOfNodes;
            if(OK == get_MoCA_Mesh_NumberOfEntries(numOfNodes))
            {
                if(numOfNodes > 1)
                {
                    sprintf(statusBuf, "%s", "Joined MoCA Network");
                    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] Status: %s\n", __FILE__, __FUNCTION__, statusBuf);
                }
                else
                {
                    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s]", "Failed to Join MoCA Network.\n", __FILE__, __FUNCTION__);
                    return NOK;
                }
            }
        }
    }
    return OK;
}

/****************************************************************************************************************************************************/
// Device.MoCA.Interface Table Profile. Setters:
/****************************************************************************************************************************************************/

int MoCAInterface::set_Enable(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    if(stMsgData->paramtype == hostIf_BooleanType) {
        stMsgData->faultCode = fcInvalidParameterType;
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed due to wrong data type. This should be boolean.\n", __FUNCTION__, __FILE__);
        return NOK;
    }

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        const bool value = get_boolean(stMsgData->paramValue);
        RMH_Result ret = RMH_Self_SetEnabled(rmh, value);
        if (ret == RMH_SUCCESS) {
            m_bEnable = value;
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_SetEnabled successfully set. \n", __FUNCTION__, __LINE__);
        }
        else {
            stMsgData->faultCode = fcRequestDenied;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_SetEnabled failed with result %s. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return NOK;
        }
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    stMsgData->faultCode = fcNoFault;
    return OK;
}


int MoCAInterface::set_Alias(HOSTIF_MsgData_t *stMsgData)
{
    stMsgData->paramtype = hostIf_StringType;
    const char* alias = stMsgData->paramName;
    if(alias) {
        snprintf(m_i8Alias, N_LENGTH-1, "cpe-%s", alias);
    }
    return OK;
}

int MoCAInterface::set_LowerLayers(HOSTIF_MsgData_t *stMsgData)
{
    /*Retrieving value */
    // Implementation for SOC vendor
    // value->out_cval =

    return OK;
}
int MoCAInterface::set_PreferredNC(HOSTIF_MsgData_t *stMsgData)
{
    if(stMsgData->paramtype == hostIf_BooleanType) {
        stMsgData->faultCode = fcInvalidParameterType;
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed due to wrong data type. This should be boolean.\n", __FUNCTION__, __FILE__);
        return NOK;
    }

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        const bool value = get_boolean(stMsgData->paramValue);
        RMH_Result ret = RMH_Self_SetPreferredNCEnabled(rmh, value);
        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_SetPreferredNCEnabled successfully set with value %d. \n", __FUNCTION__, __LINE__, value);
        }
        else {
            stMsgData->faultCode = fcRequestDenied;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_SetPreferredNCEnabled failed with result %s. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return NOK;
        }
    }
    stMsgData->faultCode = fcNoFault;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int MoCAInterface::set_PrivacyEnabledSetting(HOSTIF_MsgData_t *stMsgData)
{
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();

    if(rmh) {
        const bool value = get_boolean(stMsgData->paramValue);
        RMH_Result ret = RMH_Self_SetPrivacyEnabled(rmh, value);
        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_SetPrivacyEnabled successfully set with value %d. \n", __FUNCTION__, __LINE__, value);
        }
        else {
            stMsgData->faultCode = fcRequestDenied;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_SetPrivacyEnabled failed with result %s. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return NOK;
        }
    }
    stMsgData->faultCode = fcNoFault;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int MoCAInterface::set_FreqCurrentMaskSetting(HOSTIF_MsgData_t *stMsgData)
{
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();

    if(rmh) {
        const uint32_t value = atoi(stMsgData->paramValue);
        RMH_Result ret = RMH_Self_SetFrequencyMask(rmh, value);
        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_SetFrequencyMask successfully set with value %d. \n", __FUNCTION__, __LINE__, value);
        }
        else {
            stMsgData->faultCode = fcRequestDenied;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_SetFrequencyMask failed with result %s. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return NOK;
        }
    }
    stMsgData->faultCode = fcNoFault;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int MoCAInterface::set_KeyPassphrase(HOSTIF_MsgData_t *stMsgData)
{
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();

    if(rmh) {
        const char* value = stMsgData->paramValue;
        RMH_Result ret = RMH_Self_SetPrivacyPassword(rmh, value);
        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_SetPrivacyPassword successfully set with value %d. \n", __FUNCTION__, __LINE__, value);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s,%d] KeyPassphrase set Complete \n",__FUNCTION__,__LINE__);
        }
        else {
            stMsgData->faultCode = fcRequestDenied;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_SetPrivacyPassword failed with result %s. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return NOK;
        }
    }
    stMsgData->faultCode = fcNoFault;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int MoCAInterface::set_TxPowerLimit(HOSTIF_MsgData_t *stMsgData)
{
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        const int32_t value = get_int(stMsgData->paramValue);
        RMH_Result ret = RMH_Self_SetTxPowerLimit(rmh, value);
        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_SetTxPowerLimit successfully set with value %d. \n", __FUNCTION__, __LINE__, value);
        }
        else {
            stMsgData->faultCode = fcRequestDenied;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_SetTxPowerLimit failed with result %s. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return NOK;
        }
    }
    stMsgData->faultCode = fcNoFault;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int MoCAInterface::set_PowerCntlPhyTarget(HOSTIF_MsgData_t *stMsgData)
{
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        const int32_t value = get_int(stMsgData->paramValue);
        RMH_Result ret = RMH_Self_SetPrimaryChannelTargetPhyRate(rmh, value);
        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%u]RMH_Self_SetPrimaryChannelTargetPhyRate successfully set with value %d. \n", __FUNCTION__, __LINE__, value);
        }
        else {
            stMsgData->faultCode = fcRequestDenied;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Self_SetPrimaryChannelTargetPhyRate failed with result %s. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return NOK;
        }
    }
    stMsgData->faultCode = fcNoFault;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int MoCAInterface::set_BeaconPowerLimit(HOSTIF_MsgData_t *stMsgData)
{
    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        const uint32_t value = get_int(stMsgData->paramValue);
        RMH_Result ret = RMH_Power_SetTxBeaconPowerReduction(rmh, value);
        if (ret == RMH_SUCCESS) {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%u] RMH_Power_SetTxBeaconPowerReduction successfully set with value %d. \n", __FUNCTION__, __LINE__, value);
        }
        else {
            stMsgData->faultCode = fcRequestDenied;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Power_SetTxBeaconPowerReduction failed with result %s. \n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            return NOK;
        }
    }
    stMsgData->faultCode = fcNoFault;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}


/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */
