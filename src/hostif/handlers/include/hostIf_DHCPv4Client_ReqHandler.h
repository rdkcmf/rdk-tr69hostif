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
 * @file hostIf_DHCPv4Client_ReqHandler.h
 *
 * @brief host Interface for DHCPV4 Client Request Handler.
 *
 * This API defines the request handler operations for the DHCPV4 Client
 *
 * @author vejuturu@cisco.com
 *
 */

/** @defgroup HOST_IF DHCPv4 CLIENT HANDLER
 *   @ingroup HOST_IF
 *
 *  DHCPv4Client interface is a platform agnostic IARM communication interface. It allows
 *  DHCPv4 client applications to communicate by sending Get and Set operation
 *
 */

/** @addtogroup HOST_IF DHCPv4 CLIENT HANDLER_API .
 *  @ingroup HOST_IF
 *
 *  Described herein are the functions that are part of the
 *  DHCPv4 Request handler .
 *
 *  @{
 */

#ifndef HOSTIF_DHCPv4CLIENT_REQHANDLER_H_
#define HOSTIF_DHCPv4CLIENT_REQHANDLER_H_

#include "hostIf_msgHandler.h"
#include "hostIf_updateHandler.h"


class DHCPv4ClientReqHandler : public msgHandler
{
    DHCPv4ClientReqHandler() {};
    ~DHCPv4ClientReqHandler() {};

    static class DHCPv4ClientReqHandler *pInstance;
    static updateCallback mUpdateCallback;

    static int curNumOfDHCPv4Clients;

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

#endif /* HOSTIF_DHCPv4CLIENT_REQHANDLER_H_ */
/* End of HOSTIF_DHCPv4CLIENT_REQHANDLER_H_ doxygen group */
/**
 * @}
 */
