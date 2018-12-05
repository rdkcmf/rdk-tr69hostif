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


#ifndef DEVSET_COMP_HDMI_HPP_
#define DEVSET_COMP_HDMI_HPP_

#include "host.hpp"
#include "videoResolution.hpp"
#include "dsVideoResolutionSettings.h"
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

#include "Components_DisplayDevice.h"
#include "videoOutputPort.hpp"

#define HDMI_RESOLUTION_MODE_AUTO       "Auto"
#define HDMI_RESOLUTION_MODE_MANUAL     "Manual"
#define HDMI_RESOLUTION_VALUE_DEFAULT   "1280x720/59.94Hz" //1280x720p/60Hz"

#ifndef PARAM_LEN
#define PARAM_LEN TR69HOSTIFMGR_MAX_PARAM_LEN
#endif

class  hostIf_STBServiceHDMI
{
    static GHashTable *ifHash;
    hostIf_STBServiceHDMI(int devid, device::VideoOutputPort& port);
    ~hostIf_STBServiceHDMI() {};
    static GMutex *m_mutex;
    int dev_id;
    device::VideoOutputPort& vPort;
    hostIf_STBServiceDisplayDevice *displayDevice;

    static char dsHDMIResolutionMode[10];
    bool backupEnable;
    char backupStatus[PARAM_LEN];
    char backupResolutionValue[PARAM_LEN];
    char backupName[PARAM_LEN];

    bool bCalledEnable;
    bool bCalledStatus;
    bool bCalledResolutionValue;
    bool bCalledName;

private:
    int setResolution(const HOSTIF_MsgData_t *stMsgData);
    int getResolutionValue(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
    int setEnableVideoPort(const HOSTIF_MsgData_t *stMsgData);
    int getEnable(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
    int getStatus(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
    int getName(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
    int setHDMIResolutionMode(const char* value);
    static const char* getHDMIResolutionMode();

public:
    static hostIf_STBServiceHDMI *getInstance(int dev_id);
    static void closeInstance(hostIf_STBServiceHDMI *);
    void checkForUpdates(updateCallback mUpdateCallback);
    static GList* getAllInstances();
    static void closeAllInstances();
    static void getLock();
    static void releaseLock();
    void doUpdates(updateCallback mUpdateCallback);
    int handleSetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData);
    int handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData);

};

#endif /* DEVSET_COMP_HDMI_HPP_ */


/** @} */
/** @} */
