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
 * @file Components_DisplayDevice.h
 * @brief The header file provides components display device information APIs.
 */

/**
 * @defgroup TR69_HOSTIF_STBSERVICES_DISPLAYDEVICE TR-069 Object (Device.STBServices.Components.DisplayDevice)
 * @par Front End objects model network side interfaces.
 *  A Front End acts as an interface between the network and the inner functional blocks of the STB.
 * The network can be the home network or an external network e.g. DTT, IPTV. Depending on
 * the network type, connections modelled by Front End objects can be unidirectional e.g. DTT, or
 * bidirectional IP. An IP Front End can be bi-directional because the STB can be both a content
 * destination and a content source.
 *
 * @note Note that the IP Front End models the STBs LAN connection and is a logical rather than physical
 * concept, which means that an STB never has more than one IP Front End, even if it has more
 * than one LAN IP or physical interfaces. The IP Front End is typically capable of handling multiple
 * input and output streams at the same time.

 * @ingroup TR69_HOSTIF_STBSERVICES
 *
 * @defgroup TR69_HOSTIF_STBSERVICES_DISPLAYDEVICE_API TR-069 Object (Device.STBServices.Components.DisplayDevice) Public APIs
 * Describe the details about TR-069 components display device APIs specifications.
 * @ingroup TR69_HOSTIF_STBSERVICES_DISPLAYDEVICE
 *
 * @defgroup TR69_HOSTIF_STBSERVICES_DISPLAYDEVICE_CLASSES TR-069 Object (Device.STBServices.Components.DisplayDevice) Public Classes
 * Describe the details about classes used in TR069 components display device.
 * @ingroup TR69_HOSTIF_STBSERVICES_DISPLAYDEVICE
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef DEVSET_COMP_DISPLAYDEVICE_HPP_
#define DEVSET_COMP_DISPLAYDEVICE_HPP_

#include "host.hpp"
#include "dsTypes.h"
#include "videoOutputPortType.hpp"
#include "videoDevice.hpp"
#include "videoDFC.hpp"
#include "manager.hpp"
#include "dsUtl.h"
#include "dsError.h"
#include "list.hpp"
#include <exception>
#include <string.h>
#include "stdlib.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"
#include "hostIf_utils.h"
#include "videoOutputPort.hpp"

#ifndef PARAM_LEN
#define PARAM_LEN TR69HOSTIFMGR_MAX_PARAM_LEN
#endif

#define MAX_RESOLUTION_LENGTH         30
#define DISPLAYDEVICE_OBJECT_NAME "DisplayDevice."

/**
 * @brief This class provides the TR-069 components display device information.
 * @ingroup TR69_HOSTIF_STBSERVICES_DISPLAYDEVICE_CLASSES
 */
class  hostIf_STBServiceDisplayDevice
{

    int dev_id;
    device::VideoOutputPort& vPort;

    char backupDisplayDeviceStatus[_BUF_LEN_16];
    char backupEDID[_BUF_LEN_256];
    char backupEDIDBytes[_BUF_LEN_256];
    char backupSupportedResolution[_BUF_LEN_16];
    char backupPreferredResolution[_BUF_LEN_16];

    bool bCalledDisplayDeviceStatus;
    bool bCalledEDID;
    bool bCalledEDIDBytes;
    bool bCalledSupportedResolution;
    bool bCalledPreferredResolution;

    int getStatus(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
    int getEDID_BYTES(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
    int getX_COMCAST_COM_EDID(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
    int getSupportedResolutions(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
    int getPreferredResolution(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

public:
    hostIf_STBServiceDisplayDevice(int devId, device::VideoOutputPort& port);
    void doUpdates(const char *baseName, updateCallback mUpdateCallback);
    int handleSetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData);
    int handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData);
    ~hostIf_STBServiceDisplayDevice() {};

};

#endif /* DEVSET_COMP_DISPLAYDEVICE_HPP_ */


/** @} */
/** @} */
