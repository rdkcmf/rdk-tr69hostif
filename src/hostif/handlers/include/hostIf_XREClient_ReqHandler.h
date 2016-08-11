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
 * @file hostIf_XREClient_ReqHandler.h
 * @brief The header file provides HostIf XRE Client request handler information APIs.
 */

/**
 * @defgroup TR-069HOSTIF_XRECLIENT_REQHANDLER_CLASSES XREClient RequestHandler Public Classes
 * Describe the details about classes used in TR-069 XREClient request handler.
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


#ifndef HOSTIF_XRECLIENT_REQHANDLER_H_
#define HOSTIF_XRECLIENT_REQHANDLER_H_

#include "hostIf_msgHandler.h"
#include "hostIf_updateHandler.h"
#include "hostIf_main.h"

/**
 * @brief This class provides the interface for getting XRE request handler information.
 * @ingroup TR-069HOSTIF_XRECLIENT_REQHANDLER_CLASSES
 */
class XREClientReqHandler : public msgHandler
{
    XREClientReqHandler() {};
    ~XREClientReqHandler() {};
    static class XREClientReqHandler *pInstance;
    static updateCallback mUpdateCallback;
    static GMutex *m_mutex;
    static void getLock();
    static int numOfEntries;
public:
    virtual bool init();
    virtual bool unInit();
    virtual int handleSetMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleGetMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData);
    static void releaseLock();
    static void checkForUpdates();
    static void reset();
    static void registerUpdateCallback(updateCallback cb);
    static msgHandler* getInstance();
};

#endif /* HOSTIF_XRECLIENT_REQHANDLER_H_ */
/* End of HOSTIF_XRECLIENT_REQHANDLER_H_ doxygen group */
/**
 * @}
 */


/** @} */
/** @} */
