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
* @file hostIf_MsgHandler.h
*
* @brief hostIf Common Message Handler API.
*
* This API defines the core operations for Host interface
*
* @par Document
* Document reference.
*
* @par Open Issues (in no particular order)
* -# None
*
* @par Assumptions
* -# None
*
* @par Abbreviations
* - BE:      ig-Endian.
* - cb:      allback function (suffix).
* - hostIf:  Host Interface.
* - DS:      Device Settings.
* - FPD:     Front-Panel Display.
* - HAL:     Hardware Abstraction Layer.
* - LE:      Little-Endian.
* - LS:      Least Significant.
* - MBZ:     Must be zero.
* - MS:      Most Significant.
* - RDK:     Reference Design Kit.
* - _t:      Type (suffix).
*
* @par Implementation Notes
* -# None
*
*/

/** @defgroup HOST_IF
*    @ingroup HOST_IF
*
*  HOST-IF is a platform agnostic Inter-process communication (IPC) interface. It allows
*  applications to communicate with each other by sending Events or invoking Remote
*  Procedure Calls. The common programming APIs offered by the RDK IARM-Bus interface is
*  independent of the operating system or the underlying IPC mechanism.
*
*/

/** @addtogroup IARM_BUS_IARM_CORE_API IARM-Core library.
*  @ingroup IARM_BUS
*
*  Described herein are the functions that are part of the
*  IARM Core library.
*
*  @{
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
    virtual bool init() = 0;
    virtual bool unInit() = 0;

    virtual int handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData) = 0;
    virtual int handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData) = 0;

    virtual int handleSetMsg(HOSTIF_MsgData_t *stMsgData) = 0;
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
