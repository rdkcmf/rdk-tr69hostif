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
 * @file hostIf_DeviceClient_ReqHandler.h
 * @brief The header file provides HostIf Device client request handler information APIs.
 */

/**
 * @defgroup TR-069HOSTIF_DEVICECLIENT_HANDLER TR-069 Host Interface Handler
 * The role of the protocol handler is to identify and classify flows based on application awareness.
 * - For example, a SIP protocol handler might identify a call-control flow, an audio flow, and a
 * video flow. The App and Flow tables are used to specify the classification outcome associated
 * with each such flow.
 *
 * For each App table entry there can be one or more associated Flow table entries. Each flow table
 * entry identifies a type of flow associated with the protocol handler. The Type parameter is used
 * to identify the specific type of flow associated with each entry.
 * - For example, a Flow table entry for a SIP protocol handler might refer only to the audio
 * flows associated with that protocol handler.
 *
 * A protocol handler can be defined as being fed from the output of a Classification table entry.
 * That is, a Classification entry can be used to single out control traffic to be passed to the protocol
 * handler, which then subsequently identifies associated flows. Doing so allows more than one
 * instance of a protocol handler associated with distinct traffic.
 * - For example, one could define two App table entries associated with SIP protocol handlers.
 * If the classifier distinguished control traffic to feed into each handler based on the destination
 * IP address of the SIP server, this could be used to separately classify traffic for different
 * SIP service providers. In this case, each instance of the protocol handler would identify only
 * those flows associated with a given service.
 *
 * @note Note that the Classification table entry that feeds each protocol handler wouldnt encompass all
 * of the flows, only the traffic needed by the protocol handler to determine the flowstypically
 * only the control traffic.
 * @ingroup TR69_HOSTIF
 *
 * @defgroup TR-069HOSTIF_DEVICECLIENT_REQHANDLER_CLASSES Device Client RequestHandler Public Classes
 * Describe the details about classes used in TR-069 DeviceClient request handler.
 * @ingroup TR-069HOSTIF_DEVICECLIENT_HANDLER
 */

/**
 * @file hostIf_deviceClient_ReqHandler.h
 *
 * @brief HOST-IF deviceClient Request Handler interface API.
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


#ifndef HOSTIF_DEVICECLIENT_REQHANDLER_H_
#define HOSTIF_DEVICECLIENT_REQHANDLER_H_

#include "hostIf_msgHandler.h"
#include "hostIf_updateHandler.h"

/**
 * @brief This class provides the interface for getting device client request handler information.
 * @ingroup TR-069HOSTIF_DEVICECLIENT_REQHANDLER_CLASSES
 */
class DeviceClientReqHandler : public msgHandler
{
    DeviceClientReqHandler() {};
    ~DeviceClientReqHandler() {};
    static class DeviceClientReqHandler *pInstance;
    static updateCallback mUpdateCallback;

    static int curNumOfProcess[100];
    static int curNumOfProcessor[10];

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

#endif /* HOSTIF_DEVICECLIENT_REQHANDLER_H_ */
/* End of HOSTIF_DEVICECLIENT_REQHANDLER_H_ doxygen group */
/**
 * @}
 */


/** @} */
/** @} */
