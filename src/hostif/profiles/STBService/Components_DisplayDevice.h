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

class  hostIf_STBServiceDisplayDevice
{
    ~hostIf_STBServiceDisplayDevice() {};
    int dev_id;
    device::VideoOutputPort& vPort;

    char backupDisplayDeviceStatus[PARAM_LEN];
    char backupEDID[PARAM_LEN];
    char backupEDIDBytes[PARAM_LEN];
    char backupSupportedResolution[PARAM_LEN];
    char backupPreferredResolution[PARAM_LEN];

    bool bCalledDisplayDeviceStatus;
    bool bCalledEDID;
    bool bCalledEDIDBytes;
    bool bCalledSupportedResolution;
    bool bCalledPreferredResolution;

private:
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

};

#endif /* DEVSET_COMP_DISPLAYDEVICE_HPP_ */


/** @} */
/** @} */
