/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2020 RDK Management
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
 * @file XrdkCentralT2_ReqHandler.h
 * @brief The header file provides Telemetry 2.0 Data Model request handler information APIs.
 */

/**
 * @defgroup Telemetry2.0 DML RequestHandler Public Classes
 * Describe the details about classes used in TR-069 Telemetry dml request handler.
 */


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef _X_RDK_CENTRAL_T2_DML_REQHANDLER_H_
#define _X_RDK_CENTRAL_T2_DML_REQHANDLER_H_

#include "hostIf_msgHandler.h"
#include "hostIf_updateHandler.h"

/**
 * @brief This class provides the interface for getting Telemetry DML request handler information.
 * @ingroup TELEMETRYG_DML_REQHANDLER_CLASSES
 */
class XRdkCentralT2 : public msgHandler
{
private:
    XRdkCentralT2() {};
    ~XRdkCentralT2() {};
    static class XRdkCentralT2 *pInstance;

public:
    virtual bool init();
    virtual bool unInit();
    virtual int handleSetMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleGetMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData);
    static msgHandler* getInstance();
};

#endif /* _X_RDK_CENTRAL_T2_DML_REQHANDLER_H_ */
/* End of _X_RDK_CENTRAL_T2_DML_REQHANDLER_H_ doxygen group */
/**
 * @}
 */


/** @} */
/** @} */
