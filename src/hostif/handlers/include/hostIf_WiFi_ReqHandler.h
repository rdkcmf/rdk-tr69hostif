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
 * @file hostIf_WiFi_ReqHandler.h
 * @brief The header file provides HostIf WiFi request handler information APIs.
 */

/**
 * @defgroup TR-069HOSTIF_WIFI_REQHANDLER_CLASSES WiFi RequestHandler Public Classes
 * Describe the details about classes used in TR-069 WiFi request handler.
 * @ingroup TR-069HOSTIF_DEVICECLIENT_HANDLER
 */

/**
 * @file hostIf_timeClient_ReqHandler.h
 *
 * @brief host Interface for timeClient Request Handler API.
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
 * - LE:      Little-Endian.    virtual int handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData);
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
 *  It allows moca client applications to communicate by sending Get operation
 *  from Time library.
 */


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef HOSTIF_WIFI_REQHANDLER_H_
#define HOSTIF_WIFI_REQHANDLER_H_

#ifdef USE_WIFI_PROFILE

#include "hostIf_msgHandler.h"
#include "hostIf_updateHandler.h"


#define DEVICE_WIFI_SSID_PROFILE        "Device.WiFi.SSID."
#define DEVICE_WIFI_RADIO_PROFILE       "Device.WiFi.Radio."
#define DEVICE_WIFI_ENDPOINT_PROFILE    "Device.WiFi.EndPoint."

/**
 * @brief This class provides the interface for getting WiFi request handler information.
 * @ingroup TR-069HOSTIF_WIFI_REQHANDLER_CLASSES
 */
class WiFiReqHandler : public msgHandler
{
//private:
    WiFiReqHandler() {};
    ~WiFiReqHandler() {};
    static class WiFiReqHandler *pInstance;
    static updateCallback mUpdateCallback;
private:
    static int savedSSIDNumberOfEntries;
    static int savedRadioNumberOfEntries;
    static int savedEndPointNumberOfEntries;

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

#endif /* #ifdef USE_WIFI_PROFILE*/

#endif /* HOSTIF_WIFI_REQHANDLER_H_ */
/* End of HOSTIF_WIFI_REQHANDLER_H_ doxygen group */
/**
 * @}
 */


/** @} */
/** @} */
