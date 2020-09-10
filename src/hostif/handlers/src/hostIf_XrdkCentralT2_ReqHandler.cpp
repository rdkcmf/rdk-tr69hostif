/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2020 RDK Management
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
 * @file hostIf_XrdkCentralT2_ReqHandler.cpp
 * @brief The header file provides HostIf Telemetry 2.0 service request handler information.
 */
#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_rbus_Dml_Provider.h"
#include "hostIf_XrdkCentralT2_ReqHandler.h"

#define T2_REPORT_PROFILE_PARAM "Device.X_RDKCENTRAL-COM_T2.ReportProfiles"


XRdkCentralT2* XRdkCentralT2::pInstance = NULL;

msgHandler* XRdkCentralT2::getInstance()
{
    if(!pInstance)
    {
        pInstance = new XRdkCentralT2();
    }

    return pInstance;
}

/**
 * @brief This function is use to initialize. Currently not implemented.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if initialization is successfully .
 * @retval false if initialization is not successful.
 * @ingroup TR-069HOSTIF_TELEMETRY_REQHANDLER_CLASSES
 */
bool XRdkCentralT2::init()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] IN \n", __FUNCTION__, __FILE__);
    return true;
}

/**
 * @brief This function is used to close all the instances of telemetry service.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if it is successfully close all the instances.
 * @retval false if not able to close all the instances.
 * @ingroup TR-069HOSTIF_TELEMETRY_REQHANDLER_CLASSES
 */
bool XRdkCentralT2::unInit()
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] XRdkCentralT2 un-initialization ...\n", __FUNCTION__);
    return true;
}

/**
 * @brief This function use to handle the set message request of telemetry multiprofile .
 * This will be a pass through request to actual module having implementation registered
 * with rbus .
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully.
 * @retval ERR_INTERNAL_ERROR if not able to set the data to the device.
 * @ingroup TR-069HOSTIF_STORAGESERVICE_REQHANDLER_CLASSES
 */
int XRdkCentralT2::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s:%s] Found string as %s.\n", __FUNCTION__, __FILE__, stMsgData->paramName);
    if(strcasecmp(stMsgData->paramName, "Device.X_RDKCENTRAL-COM_T2.ReportProfiles") == 0) {
        if(stMsgData->paramValue) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s:%s] Found parameter value as %s.\n", __FUNCTION__, __FILE__, stMsgData->paramValue);
            setRbusStringParam(T2_REPORT_PROFILE_PARAM, stMsgData->paramValue);
        }
        ret = OK ;

    }
    else if(strcasecmp(stMsgData->paramName, "Device.X_RDKCENTRAL-COM_T2.ReportProfilesMsgPack") == 0) {
        if(stMsgData->paramValue) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s:%s] Found parameter value as %s.\n", __FUNCTION__, __FILE__, stMsgData->paramValue);
            setRbusStringParam("Device.X_RDKCENTRAL-COM_T2.ReportProfilesMsgPack", stMsgData->paramValue);
        }
        ret = OK ;

    }
    else {
        stMsgData->faultCode = fcInvalidParameterName;
        ret = NOT_HANDLED;
    }
    return ret;
}

/**
 * @brief This function use to handle the get message request of telemetry multiprofile .
 * This will be a pass through request to actual module having implementation registered
 * with rbus .
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully.
 * @retval ERR_INTERNAL_ERROR if not able to get data from the device.
 * @ingroup TR-069HOSTIF_STORAGESERVICE_REQHANDLER_CLASSES
 */
int XRdkCentralT2::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    //const char *pSetting = NULL;
    int instanceNumber = 0;
    //char lcParam[TR69HOSTIFMGR_MAX_PARAM_LEN] = {'\0'};
    char* paramValue = NULL;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    if(strcasecmp(stMsgData->paramName, "Device.X_RDKCENTRAL-COM_T2.ReportProfiles") == 0)
    {
        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Pass through function to be called over RBUS for %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
        if(OK == getRbusStringParam(T2_REPORT_PROFILE_PARAM, &paramValue)) {
            strncpy(stMsgData->paramValue, paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN-1);
            stMsgData->paramValue[TR69HOSTIFMGR_MAX_PARAM_LEN-1] = '\0';
            ret = OK ;
        }

    }
    else if(strcasecmp(stMsgData->paramName, "Device.X_RDKCENTRAL-COM_T2.ReportProfilesMsgPack") == 0)
    {
        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Pass through function to be called over RBUS for %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
        if(OK == getRbusStringParam("Device.X_RDKCENTRAL-COM_T2.ReportProfilesMsgPack", &paramValue)) {
            strncpy(stMsgData->paramValue, paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN-1);
            stMsgData->paramValue[TR69HOSTIFMGR_MAX_PARAM_LEN-1] = '\0';
            ret = OK ;
        }

    }
    else
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Invalid parameter.\n", __FUNCTION__);
        ret = NOK;
    }
    return ret;
}

int XRdkCentralT2::handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    return NOT_HANDLED;
}

int XRdkCentralT2::handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    return NOT_HANDLED;
}
