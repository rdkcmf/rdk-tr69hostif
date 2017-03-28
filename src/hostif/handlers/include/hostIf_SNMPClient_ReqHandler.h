/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2017 RDK Management
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
 * @file hostIf_SNMPClient_ReqHandler.h
 * @brief The header file provides HostIf SNMP client request handler information APIs.
 */


/**
 * @file hostIf_SNMPClient_ReqHandler.h
 *
 * @brief HOST-IF SNMPClient Request Handler interface API.
 *
 * This API defines the device setting Client ReqHandler Interface operations for STBService defined under Host If
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
 *  deviceClient -interface is a platform agnostic IARM communication interface. It allows
 *  ds client applications to communicate by sending Get and Set operation
 *
 */


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/

#ifndef HOSTIF_SNMPCLIENT_REQHANDLER_H_
#define HOSTIF_SNMPCLIENT_REQHANDLER_H_

#include "hostIf_msgHandler.h"
#include "hostIf_updateHandler.h"

/**
 * @brief This class provides the interface for getting SNMP client request handler information.
 * @ingroup TR-069HOSTIF_SNMPCLIENT_REQHANDLER_CLASSES
 */
class SNMPClientReqHandler : public msgHandler
{
    SNMPClientReqHandler() {};
    ~SNMPClientReqHandler() {};
    static class SNMPClientReqHandler *pInstance;
    static updateCallback mUpdateCallback;

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

#endif /* HOSTIF_SNMPCLIENT_REQHANDLER_H_ */
/* End of HOSTIF_SNMPCLIENT_REQHANDLER_H_ doxygen group */
/**
 * @}
 */


/** @} */
/** @} */
