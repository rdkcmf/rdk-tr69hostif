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
 * @file hostIf_StorageSrvc_ReqHandler.h
 * @brief The header file provides HostIf Storage service request handler information APIs.
 */

/**
 * @defgroup TR-069HOSTIF_STORAGESERVICE_REQHANDLER_CLASSES StorageService RequestHandler Public Classes
 * Describe the details about classes used in TR-069 storage service handler.
 * @ingroup TR-069HOSTIF_DEVICECLIENT_HANDLER
 */

/**
 * @file hostIf_StorageSrvc_ReqHandler.h
 *
 * @brief host Interface for Storage Service Request Handler
 *
 * This exposes APIs for get and set operations for the Storage service
 *
 * @author karsomas@cisco.com
 *
 */

/**
 *  StorageService interface is a platform agnostic IARM communication interface. It allows
 *  StorageService applications to communicate by sending Get and Set operation
 *
 */


#ifndef HOSTIF_STORAGESRVC_REQHANDLER_H_
#define HOSTIF_STORAGESRVC_REQHANDLER_H_

#include "hostIf_msgHandler.h"
#include "hostIf_updateHandler.h"

/**
 * @brief This class provides the interface for getting StorageService request handler information.
 * @ingroup TR-069HOSTIF_STORAGESERVICE_REQHANDLER_CLASSES
 */
class StorageSrvcReqHandler : public msgHandler
{
    StorageSrvcReqHandler() {};

    static class StorageSrvcReqHandler *pInstance;
protected:
    virtual ~StorageSrvcReqHandler() {};
public:
    virtual bool init();
    virtual bool unInit();
    virtual int handleSetMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleGetMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData);
    static msgHandler* getInstance();

};

#endif /* HOSTIF_StorageSrvc_REQHANDLER_H_ */
/* End of HOSTIF_StorageSrvc_REQHANDLER_H_ doxygen group */
/**
 * @}
 */
