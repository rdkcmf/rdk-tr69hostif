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
 * @file hostIf_mfrClient_ReqHandler.h
 *
 * @brief host Interface for mfrClient Request Handler API.
 *
 * This API defines the request handler operations for mfr client
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

/** @defgroup HOST_IF MOCA CLIENT REQUEST HANDLER
 *   @ingroup HOST_IF
 *
 *  It allows moca client applications to communicate by sending Get operation
 *  from Ethernet library.
 *
 */

/** @addtogroup HOST_IF MOCA CLIENT REQUEST HANDLER API .
 *  @ingroup HOST_IF
 *
 *  Described herein are the functions that are part of the
 *  host interface binary.
 *
 *  @{
 */




/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef HOSTIF_ETHERNETCLIENT_REQHANDLER_H_
#define HOSTIF_ETHERNETCLIENT_REQHANDLER_H_

#include "hostIf_msgHandler.h"
#include "hostIf_updateHandler.h"

class EthernetClientReqHandler : public msgHandler
{
    EthernetClientReqHandler() {};
    ~EthernetClientReqHandler() {};
    static class EthernetClientReqHandler *pInstance;
    static updateCallback mUpdateCallback;

    static int curNumOfDevices[10];
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

#endif /* HOSTIF_ETHERNETCLIENT_REQHANDLER_H_ */
/* End of HOSTIF_ETHERNETCLIENT_REQHANDLER_H_ doxygen group */
/**
 * @}
 */


/** @} */
/** @} */
