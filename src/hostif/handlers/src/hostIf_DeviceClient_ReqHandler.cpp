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
 * @file hostIf_DeviceClient_ReqHandler.cpp
 * @brief The header file provides HostIf Device client request handler information APIs.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#include "hostIf_main.h"
#include "hostIf_DeviceClient_ReqHandler.h"
#include "hostIf_utils.h"
#include "host.hpp"
#include "dsError.h"
#include "libIBus.h"
#include "Device_DeviceInfo.h"
#include "Device_DeviceInfo_Processor.h"
#include "Device_DeviceInfo_ProcessStatus.h"
#include "Device_DeviceInfo_ProcessStatus_Process.h"
#include "hostIf_msgHandler.h"

#ifdef USE_XRDK_BT_PROFILE
#include "XrdkBlueTooth.h"
#endif

DeviceClientReqHandler* DeviceClientReqHandler::pInstance = NULL;
int DeviceClientReqHandler::curNumOfProcess[100] = {0,};
int DeviceClientReqHandler::curNumOfProcessor[10] = {0,};
updateCallback DeviceClientReqHandler::mUpdateCallback = NULL;

msgHandler* DeviceClientReqHandler::getInstance()
{

    if(!pInstance)
        pInstance = new DeviceClientReqHandler();
    return pInstance;
}

void DeviceClientReqHandler::reset()
{
    hostIf_DeviceInfo::getLock();
    memset(curNumOfProcess,0,sizeof(curNumOfProcess));
    memset(curNumOfProcessor,0,sizeof(curNumOfProcessor));
    hostIf_DeviceInfo::releaseLock();
}

/**
 * @brief This function is used to initialize all the current process and processor
 * to '0' using memset.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if initialization is successful.
 * @retval false if initialization is not successful.
 * @ingroup TR-069HOSTIF_DEVICECLIENT_REQHANDLER_CLASSES
 */
bool DeviceClientReqHandler::init()
{
//    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
//    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    memset(curNumOfProcess,0,sizeof(curNumOfProcess));
    memset(curNumOfProcessor,0,sizeof(curNumOfProcessor));
    return true;
}

/**
 * @brief This function is used to close all the instances of device info, device processor
 * interface, device process status interface.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if it successfully close all the instances.
 * @retval false if not able to close all the instances.
 * @ingroup TR-069HOSTIF_DEVICECLIENT_REQHANDLER_CLASSES
 */
bool DeviceClientReqHandler::unInit()
{
//    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] Device manager DeInitializing\n", __FUNCTION__);
    hostIf_DeviceInfo::closeAllInstances();
    hostIf_DeviceProcessorInterface::closeAllInstances();
    hostIf_DeviceProcessStatusInterface::closeAllInstances();
//    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

/**
 * @brief This function is used to set the Device reset, FirmwareDownload and FirmwareDownload Status.
 * The possible reset states of the device are "Cold", "Factory", "Warehouse" and "Customer".
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successful.
 * @retval ERR_INTERNAL_ERROR if not able to set the data to the device.
 * @ingroup TR-069HOSTIF_DEVICECLIENT_REQHANDLER_CLASSES
 */
int DeviceClientReqHandler::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *pSetting;
    int instanceNumber;
    hostIf_DeviceInfo::getLock();

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);

    if(strncasecmp(stMsgData->paramName,"Device.DeviceInfo",strlen("Device.DeviceInfo"))==0)
    {
        hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
        stMsgData->instanceNum = instanceNumber;
        if(!pIface)
        {
            hostIf_DeviceInfo::releaseLock();
            return NOK;
        }

        if ((strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_Reset") == 0) ||
                (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM.Reset") == 0))
        {
            ret = pIface->set_Device_DeviceInfo_X_COMCAST_COM_Reset(stMsgData);
        }
        if ((strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_FirmwareToDownload") == 0) ||
                (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM.FirmwareToDownload") == 0))
        {
            ret = pIface->set_Device_DeviceInfo_X_COMCAST_COM_FirmwareToDownload(stMsgData);
        }
        if ((strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_FirmwareDownloadStatus") == 0)||
                (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM.FirmwareDownloadStatus") == 0))
        {
            ret = pIface->set_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadStatus(stMsgData);
        }

        /* Added as per RDK Requirement RDK-12720 */
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol") == 0)
            ret = pIface->set_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadProtocol (stMsgData);
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL") == 0)
            ret = pIface->set_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadURL (stMsgData);
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload") == 0)
            ret = pIface->set_Device_DeviceInfo_X_COMCAST_COM_FirmwareToDownload(stMsgData);
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadStatus") == 0)
            ret = pIface->set_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadStatus (stMsgData);
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_PreferredGatewayType") == 0)
        {
            ret = pIface->set_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName, xOpsDMUploadLogsNow_STR) == 0)
        {
            ret = pIface->set_xOpsDMUploadLogsNow(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName, xOpsDMMoCALogEnabled) == 0)
        {
            ret = pIface->set_xOpsDMMoCALogEnabled(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName, xOpsDMMoCALogPeriod) == 0)
        {
            ret = pIface->set_xOpsDMMoCALogPeriod(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,xOpsReverseSshTrigger_STR) == 0)
        {
            ret = pIface->set_xOpsReverseSshTrigger(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,xOpsReverseSshArgs_STR) == 0)
        {
            ret = pIface->set_xOpsReverseSshArgs(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,xOpsRPCRebootNow) == 0)
        {
            ret = pIface->set_xOpsDeviceMgmtRPCRebootNow (stMsgData);
        }
#ifdef USE_XRDK_BT_PROFILE
        else if(strncasecmp(stMsgData->paramName,X_BT_ROOT_OBJ,strlen(X_BT_ROOT_OBJ))==0)
        {
            hostIf_DeviceInfoRdk_xBT *pIface = hostIf_DeviceInfoRdk_xBT::getInstance();

            if(!pIface)  {
                hostIf_DeviceInfo::releaseLock();
                return NOK;
            }
            ret = pIface->handleSetMsg(stMsgData);
        }
#endif
    }
    hostIf_DeviceInfo::releaseLock();
    return ret;
}

/**
 * @brief This function is used to get the Process information such as "PID", "Command"
 * "Size", "Priority", "CPUTime" and "State". It is also used to get the processor information,
 * process status such as total number of active process entries and CPUUsage and device info
 * such as "BootStatus", "Manufacturer", "ManufacturerOUI", "ModelName", "Description", "ProductClass"
 * "SerialNumber", "HardwareVersion", "SoftwareVersion", "AdditionalHardwareVersion" etc..
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is gets the data successfully.
 * @retval ERR_INTERNAL_ERROR if not able to get the data from the device.
 * @ingroup TR-069HOSTIF_DEVICECLIENT_REQHANDLER_CLASSES
 */
int DeviceClientReqHandler::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *pSetting;
    int instanceNumber = 0;
    hostIf_DeviceInfo::getLock();

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    if(matchComponent(stMsgData->paramName,"Device.DeviceInfo.ProcessStatus.Process",&pSetting,instanceNumber))
    {
        hostIf_DeviceProcess *pIfaceProcess = hostIf_DeviceProcess::getInstance(instanceNumber);
        stMsgData->instanceNum = instanceNumber;
        if(!pIfaceProcess)
        {
            hostIf_DeviceInfo::releaseLock();
            return NOK;
        }
        if(strcasecmp(pSetting,"PID") == 0)
        {
            ret = pIfaceProcess->get_Device_DeviceInfo_ProcessStatus_Process_PID(stMsgData);
        }
        else if(strcasecmp(pSetting,"Command") == 0)
        {
            ret = pIfaceProcess->get_Device_DeviceInfo_ProcessStatus_Process_Command(stMsgData);
        }
        else if(strcasecmp(pSetting,"Size") == 0)
        {
            ret = pIfaceProcess->get_Device_DeviceInfo_ProcessStatus_Process_Size(stMsgData);
        }
        else if(strcasecmp(pSetting,"Priority") == 0)
        {
            ret = pIfaceProcess->get_Device_DeviceInfo_ProcessStatus_Process_Priority(stMsgData);
        }
        else if(strcasecmp(pSetting,"CPUTime") == 0)
        {
            ret = pIfaceProcess->get_Device_DeviceInfo_ProcessStatus_Process_CPUTime(stMsgData);
        }
        else if(strcasecmp(pSetting,"State") == 0)
        {
            ret = pIfaceProcess->get_Device_DeviceInfo_ProcessStatus_Process_State(stMsgData);
        }
    }
    else if(matchComponent(stMsgData->paramName,"Device.DeviceInfo.Processor",&pSetting,instanceNumber))
    {
        hostIf_DeviceProcessorInterface *pIfaceProcessor = hostIf_DeviceProcessorInterface::getInstance(instanceNumber);
        stMsgData->instanceNum = instanceNumber;
        if(!pIfaceProcessor)
        {
            hostIf_DeviceInfo::releaseLock();
            return NOK;
        }
        if(strcasecmp(pSetting,"Architecture")==0)
        {
            ret = pIfaceProcessor->get_Device_DeviceInfo_Processor_Architecture(stMsgData);
        }
    }
    else if(strncasecmp(stMsgData->paramName,"Device.DeviceInfo.ProcessStatus",strlen("Device.DeviceInfo.ProcessStatus"))==0)
    {
        hostIf_DeviceProcessStatusInterface *pIfaceStatus = hostIf_DeviceProcessStatusInterface::getInstance(instanceNumber);
        stMsgData->instanceNum = instanceNumber;
        if(!pIfaceStatus)
        {
            hostIf_DeviceInfo::releaseLock();
            ret = NOK;
        }
        if(strcasecmp(stMsgData->paramName,"Device.DeviceInfo.ProcessStatus.ProcessNumberOfEntries")==0)
        {
            ret = hostIf_DeviceProcess::getNumOfProcessEntries(stMsgData);
        }
        if(strcasecmp(stMsgData->paramName,"Device.DeviceInfo.ProcessStatus.CPUUsage")==0)
        {
            ret = pIfaceStatus->get_Device_DeviceInfo_ProcessStatus_CPUUsage(stMsgData);
        }
    }
    else if(strncasecmp(stMsgData->paramName,xOpsReverseSshStatus_STR,strlen(xOpsReverseSshStatus_STR)) == 0)
    {
        hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
        ret = pIface->get_xOpsReverseSshStatus(stMsgData);
    }
#ifdef USE_XRDK_BT_PROFILE
    else if(strncasecmp(stMsgData->paramName,X_BT_ROOT_OBJ,strlen(X_BT_ROOT_OBJ))==0)
    {
        hostIf_DeviceInfoRdk_xBT *pIface = hostIf_DeviceInfoRdk_xBT::getInstance();

        if(!pIface)  {
            hostIf_DeviceInfo::releaseLock();
            return NOK;
        }
        ret = pIface->handleGetMsg(stMsgData);
    }
#endif
    else if(strncasecmp(stMsgData->paramName,"Device.DeviceInfo",strlen("Device.DeviceInfo"))==0)
    {
        hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance(instanceNumber);
        stMsgData->instanceNum = instanceNumber;
        if(!pIface)
        {
            hostIf_DeviceInfo::releaseLock();
            return NOK;
        }
        if(strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM.BootStatus") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_BootStatus(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_PreferredGatewayType") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.Manufacturer") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_Manufacturer(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.ManufacturerOUI") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_ManufacturerOUI(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.ModelName") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_ModelName(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.Description") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_Description(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.ProductClass") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_ProductClass(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.SerialNumber") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_SerialNumber(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.HardwareVersion") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_HardwareVersion(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.SoftwareVersion") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_SoftwareVersion(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.AdditionalHardwareVersion") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_AdditionalHardwareVersion(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.AdditionalSoftwareVersion") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_AdditionalSoftwareVersion(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.ProvisioningCode") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_ProvisioningCode(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.UpTime") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_UpTime(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.FirstUseDate") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_FirstUseDate(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_STB_MAC") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_STB_MAC(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_STB_IP") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_STB_IP(stMsgData);
        }
        else if ((strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_FirmwareFilename") == 0) ||
                 (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM.FirmwareFilename") == 0) ||
                 (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareFilename") == 0))
        {
            ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_FirmwareFilename(stMsgData);
        }
        else if ((strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_FirmwareToDownload") == 0) ||
                 (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM.FirmwareToDownload") == 0) ||
                 (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareToDownload") == 0))
        {
            ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_FirmwareToDownload(stMsgData);
        }
        else if ((strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_FirmwareDownloadStatus") == 0) ||
                 (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM.FirmwareDownloadStatus") == 0) ||
                 (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadStatus") == 0))
        {
            ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadStatus(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadURL") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadURL (stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadProtocol") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadProtocol(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_RDKVersion") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_SoftwareVersion(stMsgData);
        }
        else if ((strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_Reset") == 0) ||
                 (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM.Reset") == 0) ||
                 (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_Reset") == 0))
        {
            ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_Reset(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_COMCAST-COM_PowerStatus") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_COMCAST_COM_PowerStatus(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.VendorConfigFileNumberOfEntries") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_VendorConfigFileNumberOfEntries(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.SupportedDataModelNumberOfEntries") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_SupportedDataModelNumberOfEntries(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.ProcessorNumberOfEntries") == 0)
        {
            ret = hostIf_DeviceInfo::get_Device_DeviceInfo_ProcessorNumberOfEntries(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.VendorLogFileNumberOfEntries") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_VendorLogFileNumberOfEntries(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.MemoryStatus.Total") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_MemoryStatus_Total(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.MemoryStatus.Free") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_MemoryStatus_Free(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM.CPUTemp") == 0)
        {
            ret = pIface->get_Device_DeviceInfo_X_RDKCENTRAL_COM_CPUTemp(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,xOpsDMLogsUploadStatus_STR) == 0)
        {
            ret = pIface->get_xOpsDMLogsUploadStatus(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,xOpsDMUploadLogsNow_STR) == 0)
        {
            ret = pIface->get_xOpsDMUploadLogsNow(stMsgData);
        }
        else if (strcasecmp(stMsgData->paramName,XRDK_BOOT_TIME) == 0)
        {
            ret = pIface->get_X_RDKCENTRAL_COM_BootTime(stMsgData);
        }
    }
    hostIf_DeviceInfo::releaseLock();
    return ret;
}

void DeviceClientReqHandler::registerUpdateCallback(updateCallback cb)
{
    mUpdateCallback = cb;
}



void DeviceClientReqHandler::checkForUpdates()
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    GList *elem;
    int index = 1;
    char tmp_buff[TR69HOSTIFMGR_MAX_PARAM_LEN];

    hostIf_DeviceInfo::getLock();

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);

    if(hostIf_DeviceInfo::get_Device_DeviceInfo_ProcessorNumberOfEntries(&msgData) == OK)
    {
        int tmpNoDev = get_int(msgData.paramValue);
        char tmp[TR69HOSTIFMGR_MAX_PARAM_LEN] = "";
        sprintf(tmp_buff,"Device.DeviceInfo.Processor");
        while(curNumOfProcessor[index] > tmpNoDev)
        {
            sprintf(tmp,"%s.%d.",tmp_buff,tmpNoDev);
            if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE,tmp, NULL, hostIf_IntegerType);
            tmpNoDev++;
//            sleep(1);
        }
        while(curNumOfProcessor[index] < tmpNoDev)
        {
            sprintf(tmp,"%s.",tmp_buff);
            if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_ADD,tmp, NULL, hostIf_IntegerType);
            tmpNoDev--;
//            sleep(1);
        }
        curNumOfProcessor[index] = get_int(msgData.paramValue);
    }
    hostIf_DeviceInfo::releaseLock();
#ifdef HAVE_VALUE_CHANGE_EVENT
    GList *devList = hostIf_DeviceInfo::getAllInstances();

    for(elem = devList; elem; elem = elem->next,index++)
    {
        hostIf_DeviceInfo *pIface = hostIf_DeviceInfo::getInstance((int)elem->data);
        if(pIface)
        {
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_Manufacturer(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"Manufacturer");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_ManufacturerOUI(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"ManufacturerOUI");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_ModelName(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"ModelName");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_Description(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"Description");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_ProductClass(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"ProductClass");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_SerialNumber(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"SerialNumber");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_HardwareVersion(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"HardwareVersion");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_SoftwareVersion(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"SoftwareVersion");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_AdditionalHardwareVersion(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"AdditionalHardwareVersion");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_AdditionalSoftwareVersion(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"AdditionalSoftwareVersion");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_ProvisioningCode(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"ProvisioningCode");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_UpTime(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"UpTime");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_FirstUseDate(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"FirstUseDate");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_X_COMCAST_COM_STB_MAC(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"X_COMCAST-COM_STB_MAC");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_X_COMCAST_COM_STB_IP(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"X_COMCAST-COM_STB_IP");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_X_COMCAST_COM_FirmwareFilename(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"X_COMCAST-COM_FirmwareFilename");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_X_COMCAST_COM_Reset(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"X_COMCAST-COM_Reset");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_VendorConfigFileNumberOfEntries(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"VendorConfigFileNumberOfEntries");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_SupportedDataModelNumberOfEntries(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"SupportedDataModelNumberOfEntries");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIface->get_Device_DeviceInfo_VendorLogFileNumberOfEntries(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.%d.%s",index,"VendorLogFileNumberOfEntries");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
        }
    }

    g_list_free(devList);


    devList = hostIf_DeviceProcessorInterface::getAllInstances();

    for(elem = devList; elem; elem = elem->next,index++)
    {
        hostIf_DeviceProcessorInterface *pIfaceProcessor = hostIf_DeviceProcessorInterface::getInstance((int)elem->data);

        if(pIfaceProcessor)
        {
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIfaceProcessor->get_Device_DeviceInfo_Processor_Architecture(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.Processor.%d.%s",index,"Architecture");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
        }
    }


    g_list_free(devList);

    devList = hostIf_DeviceProcessStatusInterface::getAllInstances();

    for(elem = devList; elem; elem = elem->next,index++)
    {
        hostIf_DeviceProcessStatusInterface *pIfaceStatus = hostIf_DeviceProcessStatusInterface::getInstance((int)elem->data);

        if(pIfaceStatus)
        {
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIfaceStatus->get_Device_DeviceInfo_ProcessStatus_CPUUsage(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.ProcessStatus.%d.%s",index,"CPUUsage");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
        }
    }
    g_list_free(devList);
#endif    /*HAVE_VALUE_CHANGE_EVENT*/


    hostIf_DeviceProcess::getLock();
    index = 1;
    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);

    if(hostIf_DeviceProcess::getNumOfProcessEntries(&msgData) == OK)
    {
        int tmpNoDev = get_int(msgData.paramValue);
        char tmp[TR69HOSTIFMGR_MAX_PARAM_LEN] = "";
        sprintf(tmp_buff,"Device.DeviceInfo.ProcessStatus.Process");
        while(curNumOfProcess[index] > tmpNoDev)
        {
            sprintf(tmp,"%s.%d.",tmp_buff,tmpNoDev);
            if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE,tmp, NULL, hostIf_IntegerType);
            tmpNoDev++;
//            sleep(1);
        }
        while(curNumOfProcess[index] < tmpNoDev)
        {
            sprintf(tmp,"%s.",tmp_buff);
            if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_ADD,tmp, NULL, hostIf_IntegerType);
            tmpNoDev--;
//            sleep(1);
        }
        curNumOfProcess[index] = get_int(msgData.paramValue);
    }
    hostIf_DeviceProcess::releaseLock();


#ifdef HAVE_VALUE_CHANGE_EVENT
    devList = hostIf_DeviceProcess::getAllInstances();

    for(elem = devList; elem; elem = elem->next,index++)
    {
        hostIf_DeviceProcess *pIfaceProcess = hostIf_DeviceProcess::getInstance((int)elem->data);

        if(pIfaceProcess)
        {
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIfaceProcess->get_Device_DeviceInfo_ProcessStatus_Process_PID(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.ProcessStatus.Process.%d.%s",index,"PID");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIfaceProcess->get_Device_DeviceInfo_ProcessStatus_Process_Command(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.ProcessStatus.Process.%d.%s",index,"Command");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIfaceProcess->get_Device_DeviceInfo_ProcessStatus_Process_Size(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.ProcessStatus.Process.%d.%s",index,"Size");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIfaceProcess->get_Device_DeviceInfo_ProcessStatus_Process_Priority(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.ProcessStatus.Process.%d.%s",index,"Priority");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIfaceProcess->get_Device_DeviceInfo_ProcessStatus_Process_CPUTime(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.ProcessStatus.Process.%d.%s",index,"CPUTime");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
            memset(&msgData,0,sizeof(msgData));
            memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
            bChanged =  false;
            msgData.instanceNum=(int)elem->data;
            pIfaceProcess->get_Device_DeviceInfo_ProcessStatus_Process_State(&msgData,&bChanged);
            if(bChanged)
            {
                sprintf(tmp_buff,"Device.DeviceInfo.ProcessStatus.Process.%d.%s",index,"State");
                if(mUpdateCallback)
                {
                    mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
                }
            }
        }
    }

    hostIf_DeviceInfo::releaseLock();
#endif    /*HAVE_VALUE_CHANGE_EVENT*/
}






/** @} */
/** @} */
