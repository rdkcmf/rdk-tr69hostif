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
* @brief Provides implementation for DHCPv4 Client Request Handler Interface
*
* @author vejuturu@cisco.com
*/

/**
 * @file hostIf_Device_X_RDK_ReqHandler..cpp
 * @brief The header file provides HostIf X_RDK_ReqHandler. client request handler information APIs.
 */
//#define HAVE_VALUE_CHANGE_EVENT
#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "x_rdk_req_handler.h"
#include "safec_lib.h"

X_rdk_req_hdlr* X_rdk_req_hdlr::pInstance = NULL;
std::mutex X_rdk_req_hdlr::m;


msgHandler* X_rdk_req_hdlr::getInstance()
{
    if(!pInstance)
        pInstance = new X_rdk_req_hdlr();
    return pInstance;
}


/**
 * @brief This function is used to initialize. Currently not implemented.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if initialization is successful.
 * @retval false if initialization is not successful.
 * @ingroup TR-069X_rdk_req_hdlrCLIENT_REQHANDLER_CLASSES
 */
bool X_rdk_req_hdlr::init()
{
    return true;
}

/**
 * @brief This function is used to close all the instances of  interface stack.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if it successfully close all the instances.
 * @retval false if not able to close all the instances.
 * @ingroup TR-069X_rdk_req_hdlrCLIENT_REQHANDLER_CLASSES
 */
bool X_rdk_req_hdlr::unInit()
{
    return true;
}


/**
 * @brief This function is used to handle the set message request of InterfaceStack Client.
 * Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if successful.
 * @retval ERR_INTERNAL_ERROR if not able to set the data to the device.
 * @ingroup TR-069X_rdk_req_hdlrCLIENT_REQHANDLER_CLASSES
 */
int X_rdk_req_hdlr::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    std::lock_guard<std::mutex> lg(m);

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);

    if(strncasecmp(stMsgData->paramName, X_RDK_PREFIX_STR, strlen(X_RDK_PREFIX_STR)) == 0)
    {
        X_rdk_profile *pIface  = X_rdk_profile::getInstance();
        ret = pIface->handleSetMsg(stMsgData);
    }
    else {
        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Found string as %s. Set command not supported.\n",
                __FUNCTION__, __FILE__, stMsgData->paramName);
        stMsgData->faultCode = fcAttemptToSetaNonWritableParameter;
    }
    return ret;
}

/**
 * @brief This function is used to handle the get message request of InterfaceStack Client.
 * Gets the total number of active interface stack entries or get the attributes of
 * interface stack such as "HigherLayer" and "LowerLayer".
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if successful.
 * @retval ERR_INTERNAL_ERROR if not able to set the data to the device.
 * @ingroup TR-069X_rdk_req_hdlrCLIENT_REQHANDLER_CLASSES
 */
int X_rdk_req_hdlr::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = OK;
    std::lock_guard<std::mutex> lg(m);

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);

    if(strncasecmp(stMsgData->paramName, X_RDK_PREFIX_STR, strlen(X_RDK_PREFIX_STR)) == 0)
    {
        X_rdk_profile *pIface  = X_rdk_profile::getInstance();
        ret = pIface->handleGetMsg(stMsgData);
    }
    else
    {
        ret = NOK;
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%s:%d] Wrong Parameter.\n",
                __FUNCTION__, __FILE__, __LINE__);
    }
    return ret;
}
int X_rdk_req_hdlr::handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    return ret;
}

int X_rdk_req_hdlr::handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    return ret;
}

