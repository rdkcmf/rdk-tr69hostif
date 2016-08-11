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
 * @file hostIf_msgHandler.h
 * @brief The header file provides HostIf message handler information APIs.
 */

/**
 *
 * @defgroup TR-069HOSTIF_MESSAGE_REQHANDLER_CLASSES Message Handler Public Classes
 * Describe the details about classes used in TR-069 message handler.
 * @ingroup TR-069HOSTIF_DEVICECLIENT_HANDLER
 */


/**
*  HOST-IF is a platform agnostic Inter-process communication (IPC) interface. It allows
*  applications to communicate with each other by sending Events or invoking Remote
*  Procedure Calls. The common programming APIs offered by the RDK IARM-Bus interface is
*  independent of the operating system or the underlying IPC mechanism.
*
*/


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef HOSTIF_MSGHANDLER_H_
#define HOSTIF_MSGHANDLER_H_

#include "hostIf_tr69ReqHandler.h"

#define TEST_STR "Device.Service.STBService.HDMI.status"

#define HOSTIF_MGR_GROUP "HOSTIF_DM_PROFILE_MGR"
#define HOSTIF_JSON_CONG_GROUP "HOSTIF_JSON_CONFIG"
#define HOSTIF_GENERAL_CONG_GROUP "HOSTIF_CONFIG"

/*! Host IF request type*/
typedef enum _HostIf_ParamMgr
{
    HOSTIF_INVALID_Mgr = 0,
    HOSTIF_DeviceMgr,
    HOSTIF_XREMgr,
    HOSTIF_DSMgr,
    HOSTIF_MoCAMgr,
    HOSTIF_EthernetMgr,
    HOSTIF_IPMgr,
    HOSTIF_TimeMgr,
    HOSTIF_WiFiMgr,
    HOSTIF_DHCPv4,
    HOSTIF_InterfaceStack,
    HOSTIF_StorageSrvcMgr
#ifdef SNMP_ADAPTER_ENABLED
    , HOSTIF_SNMPAdapterMgr
#endif
} HostIf_ParamMgr_t;


int hostIf_GetMsgHandler(HOSTIF_MsgData_t *stMsgData);

int hostIf_SetAttributesMsgHandler(HOSTIF_MsgData_t *stMsgData);

int hostIf_GetAttributesMsgHandler(HOSTIF_MsgData_t *stMsgData);

int hostIf_SetMsgHandler(HOSTIF_MsgData_t *stMsgData);

void hostIf_Init_Dummy_stMsgData (HOSTIF_MsgData_t **stMsgData);

void hostIf_Print_msgData(HOSTIF_MsgData_t *stMsgData);

void hostIf_Free_stMsgData (HOSTIF_MsgData_t *stMsgData);

bool hostIf_initalize_ConfigManger();
bool hostIf_ConfigProperties_Init();
class msgHandler {

protected:

    msgHandler() {};

public:
/**
 * @brief This is a pure virtual function used for initialization. Currently not implemented.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if initialization is successful .
 * @retval false if initialization is not successful.
 * @ingroup TR-069HOSTIF_MESSAGE_REQHANDLER_CLASSES
 */
    virtual bool init() = 0;
	
/**
 * @brief This is a pure virtual function used for un initialization. Currently not implemented.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if initialization is successful.
 * @retval false if initialization is not successful.
 * @ingroup TR-069HOSTIF_MESSAGE_REQHANDLER_CLASSES
 */
    virtual bool unInit() = 0;

    virtual int handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData) = 0;
    virtual int handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData) = 0;
	
/**
 * @brief This is a pure virtual function used to handle the set message request.
 * Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if successful.
 * @retval ERR_INTERNAL_ERROR if not able to set the data to the device.
 * @ingroup TR-069HOSTIF_MESSAGE_REQHANDLER_CLASSES
 */
    virtual int handleSetMsg(HOSTIF_MsgData_t *stMsgData) = 0;

/**
 * @brief This is a pure virtual function used to handle the get message request.
 * Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if successful.
 * @retval ERR_INTERNAL_ERROR if not able to get the data from the device.
 * @ingroup TR-069HOSTIF_MESSAGE_REQHANDLER_CLASSES
 */
    virtual int handleGetMsg(HOSTIF_MsgData_t *stMsgData) = 0;
};

msgHandler* HostIf_GetMgr(HOSTIF_MsgData_t *stMsgHandlerData);

#endif /* HOSTIF_MSGHANDLER_H_ */

/* End of HOSTIF_MSGHANDLER_H_ API doxygen group */
/**
 * @}
 */


/** @} */
/** @} */
