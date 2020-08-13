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
 * @file X_RDK_WebConfig_Dml_ReqHandler.cpp
 * @brief The header file provides HostIf WebConfig dml request handler information APIs.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/

#include "x_rdk_webconfig_dml_req_handler.h"
#include "x_rdk_webconfig_dml.h"
#include "hostIf_main.h"

X_RDK_WebConfig_Dml_ReqHandler* X_RDK_WebConfig_Dml_ReqHandler::pInstance = NULL;


msgHandler* X_RDK_WebConfig_Dml_ReqHandler::getInstance()
{

    if(!pInstance)
        pInstance = new X_RDK_WebConfig_Dml_ReqHandler();
    return pInstance;
}


bool X_RDK_WebConfig_Dml_ReqHandler::init()
{
    return true;
}


bool X_RDK_WebConfig_Dml_ReqHandler::unInit()
{
    return true;
}


int X_RDK_WebConfig_Dml_ReqHandler::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    if (NULL == stMsgData)
    {
        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting as MsgData is NULL ..\n", __FUNCTION__, __FILE__);
        return NOK;
    }
    if(strncasecmp(stMsgData->paramName,X_RDK_WEBCONFIG_OBJ,strlen(X_RDK_WEBCONFIG_OBJ))==0)
    {
        X_RDK_WebConfig_dml *pIface = X_RDK_WebConfig_dml::getInstance();
        if(NULL != pIface)
        {
            ret = pIface->handleSetReq(stMsgData);
        }
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return ret;
}


int X_RDK_WebConfig_Dml_ReqHandler::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);

    if (NULL == stMsgData)
    {
        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting as MsgData is NULL ..\n", __FUNCTION__, __FILE__);
        return NOK;
    }
    if(strncasecmp(stMsgData->paramName,X_RDK_WEBCONFIG_OBJ,strlen(X_RDK_WEBCONFIG_OBJ))==0)
    {
        X_RDK_WebConfig_dml *pIface = X_RDK_WebConfig_dml::getInstance();

        if(NULL != pIface)
        {
            ret = pIface->handleGetReq(stMsgData);
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] parameter : \'%s\' Not handled \n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
        stMsgData->faultCode = fcInvalidParameterName;
        ret = NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return ret;
}


int X_RDK_WebConfig_Dml_ReqHandler::handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    return ret;
}

int X_RDK_WebConfig_Dml_ReqHandler::handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    return ret;
}

/** @} */
/** @} */
