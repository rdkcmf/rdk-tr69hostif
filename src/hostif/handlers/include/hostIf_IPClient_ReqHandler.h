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
 * @file hostIf_IPClient_ReqHandler.h
 * @brief The header file provides HostIf IP client request handler information APIs.
 */

/**
 * @defgroup TR-069HOSTIF_IPCLIENT_REQHANDLER_CLASSES IP Client RequestHandler Public Classes
 * Describe the details about classes used in TR-069 IPClient request handler.
 * @ingroup TR-069HOSTIF_DEVICECLIENT_HANDLER
 */

/**
 * @file hostIf_IPClient_ReqHandler.h
 *
 * @brief host Interface for IPClient Request Handler API.
 *
 * This API defines the request handler operations for IP client
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
 * - BE:       ig-Endian.
 * - cb:       allback function (suffix).
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
 *  It allows ip client applications to communicate by sending Get operation
 *  from IP library.
 */


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef HOSTIF_IPCLIENT_REQHANDLER_H_
#define HOSTIF_IPCLIENT_REQHANDLER_H_

#include "hostIf_msgHandler.h"
#include "hostIf_updateHandler.h"

/**
 * @brief This class provides the interface for getting IP client request handler information.
 * @ingroup TR-069HOSTIF_IPCLIENT_REQHANDLER_CLASSES
 */
class IPClientReqHandler : public msgHandler
{
private:
    IPClientReqHandler ();
    ~IPClientReqHandler ();

    static class IPClientReqHandler *pInstance;
    static updateCallback mUpdateCallback;

    static int curNumOfIPInterface;
    static int curNumOfInterfaceIPv4Addresses[10];
#ifdef IPV6_SUPPORT
    static int curNumOfInterfaceIPv6Addresses[10];
    static int curNumOfInterfaceIPv6Prefixes[10];
#endif // IPV6_SUPPORT
    static int curNumOfIPv4Interface[10];
    static int curNumOfActivePorts[10];

    class hostIf_IPInterfaceLocker
    {
    public:
        hostIf_IPInterfaceLocker ();
        ~hostIf_IPInterfaceLocker ();
    };

    static void sendAddRemoveEvents (int newValue, int& savedValue, char* objectPath, char* instancePath);

public:
    virtual bool init();
    virtual bool unInit();
    virtual int handleSetMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleGetMsg(HOSTIF_MsgData_t *stMsgData);
    static msgHandler* getInstance();
    static void registerUpdateCallback(updateCallback cb);
    static void checkForUpdates();
    static void reset();
};

#endif /* HOSTIF_IPCLIENT_REQHANDLER_H_ */
/* End of HOSTIF_IPCLIENT_REQHANDLER_H_ doxygen group */
/**
 * @}
 */


/** @} */
/** @} */
