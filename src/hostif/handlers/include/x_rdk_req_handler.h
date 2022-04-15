/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2022 RDK Management
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
 * @file x_rdk_req_handler.h
 * @brief The header file provides Device.X_RDK_<Parameter>  client request handler information APIs.
 */

/**
 * @defgroup x_rdk_req_handler Client RequestHandler Public Classes
 * Describe the details about classes used in Device.X_RDK_ request handler.
 * @ingroup x_rdk_req_handler
 */

/**
 * @file x_rdk_req_handler.h
 *
 * @brief host Interface for Device.X_RDK_<Parameter>  Client Request Handler.
 *
 * This API defines the request handler operations for the Device.X_RDK_<Parameter>  Client
 *
 * @author
 *
 */

/**
 *  x_rdk_req_handler stack client applications to communicate by sending Get and Set operation
 *
 */


#ifndef X_RDK_REQ_HANDLER_H_
#define X_RDK_REQ_HANDLER_H_

#include "hostIf_msgHandler.h"
#include "hostIf_updateHandler.h"
#include "x_rdk_profile.h"

/**
 * @brief This class provides the interface for getting Device.X_RDK_<Parameter> client request handler information.
 * @ingroup Device.X_RDK_ CLIENT_REQHANDLER_CLASSES
 */
class X_rdk_req_hdlr : public msgHandler
{
    X_rdk_req_hdlr() {};
    ~X_rdk_req_hdlr() {};

    static std::mutex m;
    static class X_rdk_req_hdlr *pInstance;

public:
    virtual bool init();
    virtual bool unInit();
    virtual int handleSetMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleGetMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData);
    static msgHandler* getInstance();
};

#endif /* X_RDK_REQ_HANDLER_H_ */
/* End of X_RDK_REQ_HANDLER_H_ doxygen group */
/**
 * @}
 */
