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
* @file hostIf_CoPilotClient_ReqHandler.h
*
* @brief hostIf CoPilot Client Request Message Handler API.
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
#ifndef HOSTIF_COPILOTCLIENT_REQHANDLER_H_
#define HOSTIF_COPILOTCLIENT_REQHANDLER_H_
#include "hostIf_msgHandler.h"
#include "hostIf_main.h"
class CoPilotClientReqHandler : public msgHandler
{
    CoPilotClientReqHandler() {};
    ~CoPilotClientReqHandler() {};
    static class CoPilotClientReqHandler *pInstance;
    static GMutex *m_mutex;
    static void getLock();
public:
    virtual bool init();
    virtual bool unInit();
    virtual int handleSetMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleGetMsg(HOSTIF_MsgData_t *stMsgData);
    static void releaseLock();
    static msgHandler* getInstance();
};
#endif /* HOSTIF_COPILOTCLIENT_REQHANDLER_H_ */
/* End of HOSTIF_COPILOTCLIENT_REQHANDLER_H_ doxygen group */
/**
 * @}
 */
/** @} */
/** @} */
