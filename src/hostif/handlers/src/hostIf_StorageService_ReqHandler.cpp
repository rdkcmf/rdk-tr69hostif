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
 /*
 * @brief Provides implementation for Storage Service Request Handler Interface
 *
 * @author karsomas@cisco.com
 */

/**
 * @file hostIf_StorageService_ReqHandler.cpp
 * @brief The header file provides HostIf IP Storage service request handler information APIs.
 */
#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "Service_Storage.h"
#include "hostIf_StorageSrvc_ReqHandler.h"


StorageSrvcReqHandler* StorageSrvcReqHandler::pInstance = NULL;

msgHandler* StorageSrvcReqHandler::getInstance()
{
    hostIf_StorageSrvc::getLock();
    if(!pInstance)
    {
        pInstance = new StorageSrvcReqHandler();
    }    
    hostIf_StorageSrvc::releaseLock();

    return pInstance;
}

/**
 * @brief This function is use to initialize. Currently not implemented.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if initialization is successfully .
 * @retval false if initialization is not successful.
 * @ingroup TR-069HOSTIF_STORAGESERVICE_REQHANDLER_CLASSES
 */
bool StorageSrvcReqHandler::init()
{
    return true;
}

/**
 * @brief This function is used to close all the instances of storage service.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if it is successfully close all the instances.
 * @retval false if not able to close all the instances.
 * @ingroup TR-069HOSTIF_STORAGESERVICE_REQHANDLER_CLASSES
 */
bool StorageSrvcReqHandler::unInit()
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] Storage Service manager DeInitializing\n", __FUNCTION__);
    hostIf_StorageSrvc::closeAllInstances();

    return true;
}

/**
 * @brief This function use to handle the set message request of storage service.
 * Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully.
 * @retval ERR_INTERNAL_ERROR if not able to set the data to the device.
 * @ingroup TR-069HOSTIF_STORAGESERVICE_REQHANDLER_CLASSES
 */
int StorageSrvcReqHandler::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] Found string as %s. Set command not supported.\n",
             __FUNCTION__, __FILE__, stMsgData->paramName);

    return ret;
}

/**
 * @brief This function use to handle the get message request of storage service.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully.
 * @retval ERR_INTERNAL_ERROR if not able to get data from the device.
 * @ingroup TR-069HOSTIF_STORAGESERVICE_REQHANDLER_CLASSES
 */
int StorageSrvcReqHandler::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *pSetting = NULL;
    int instanceNumber = 0;
    char lcParam[TR69HOSTIFMGR_MAX_PARAM_LEN]={'\0'};
    hostIf_StorageSrvc::getLock();

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    if(strcasecmp(stMsgData->paramName, "Device.services.StorageServiceNumberOfEntries") == 0)
    {
        ret = hostIf_StorageSrvc::get_Device_StorageSrvc_ClientNumberOfEntries(stMsgData);
    }
    else if(matchComponent(stMsgData->paramName,"Device.services.StorageService",&pSetting,instanceNumber))
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"psettings:%s %d\n",pSetting, instanceNumber);
        hostIf_StorageSrvc *pStorageSrvc = hostIf_StorageSrvc::getInstance(instanceNumber);
        if(pStorageSrvc)
        {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Getting response from StorageService object\n");
            ret = pStorageSrvc->handleGetMsg(stMsgData);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"After Getting response from StorageService object\n");
        }
        else
        {
            ret = NOK;
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] hostIf_StorageSrvc::getInstance returned NULL for instance %d,\n",
                    __FUNCTION__, __FILE__, __LINE__, instanceNumber);
        }
    }

    hostIf_StorageSrvc::releaseLock();
    return ret;
}
