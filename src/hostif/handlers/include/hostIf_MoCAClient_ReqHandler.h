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
 * @file hostIf_MoCAClient_ReqHandler.h
 * @brief The header file provides HostIf MoCA client request handler information APIs.
 */

/**
 * @defgroup TR-069HOSTIF_MOCACLIENT_REQHANDLER_CLASSES MoCA Client RequestHandler Public Classes
 * Describe the details about classes used in TR-069 IPClient request handler.
 * @ingroup TR-069HOSTIF_DEVICECLIENT_HANDLER
 */


/**
 *  It allows moca client applications to communicate by sending Get operation
 *  from MoCA library.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef HOSTIF_MOCACLIENT_REQHANDLER_H_
#define HOSTIF_MOCACLIENT_REQHANDLER_H_

#include "hostIf_msgHandler.h"
#include "hostIf_updateHandler.h"

/**
 * @brief This class provides the interface for getting MoCA client request handler information.
 * @ingroup TR-069HOSTIF_MOCACLIENT_REQHANDLER_CLASSES
 */
class MoCAClientReqHandler : public msgHandler
{
    MoCAClientReqHandler() {};
    ~MoCAClientReqHandler() {};
    static class MoCAClientReqHandler *pInstance;
    static updateCallback mUpdateCallback;

    static int curNumOfDevices[10];
    static int numOfMocaMeshEntries;
public:
    virtual bool init();
    virtual bool unInit();
    virtual int handleSetMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleGetMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData);
    static msgHandler* getInstance();
    static void registerUpdateCallback(updateCallback cb);
    static void checkForUpdates();
    static void reset();
};

#endif /* HOSTIF_MOCACLIENT_REQHANDLER_H_ */
/* End of HOSTIF_MOCACLIENT_REQHANDLER_H_ doxygen group */
/**
 * @}
 */


/** @} */
/** @} */
