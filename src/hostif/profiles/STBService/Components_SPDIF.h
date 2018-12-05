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


#ifndef DEVSET_COMP_SPDIF_HPP__
#define DEVSET_COMP_SPDIF_HPP__

#include <iostream>
#include "host.hpp"
#include "videoDevice.hpp"
#include "videoDFC.hpp"
#include "manager.hpp"
#include "dsUtl.h"
#include "dsError.h"
#include "list.hpp"
#include <exception>
#include <string.h>
#include <stdlib.h>
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"
#include "hostIf_main.h"
#include "audioOutputPort.hpp"

#ifndef PARAM_LEN
#define PARAM_LEN TR69HOSTIFMGR_MAX_PARAM_LEN
#endif

class hostIf_STBServiceSPDIF
{
    static GHashTable *ifHash;
    hostIf_STBServiceSPDIF(int dev_id, device::AudioOutputPort& port);
    ~hostIf_STBServiceSPDIF() {};
    static GMutex *m_mutex;
    int dev_id;
    device::AudioOutputPort& aPort;

    bool backupEnable;
    char backupStatus[PARAM_LEN];
    char backupAlias[PARAM_LEN];
    char backupName[PARAM_LEN];
    bool backupForcePCM;
    bool backupPassthrough;
    unsigned backupAudioDelay;

    bool bCalledEnable;
    bool bCalledStatus;
    bool bCalledAlias;
    bool bCalledName;
    bool bCalledForcePCM;
    bool bCalledPassthrough;
    bool bCalledAudioDelay;

private:
    int setEnable(HOSTIF_MsgData_t *stMsgData);
    int setAlias(HOSTIF_MsgData_t *stMsgData);
    int setForcePCM(HOSTIF_MsgData_t *stMsgData);

    int getEnable(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);
    int getStatus(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);
    int getAlias(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);
    int getName(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);
    int getForcePCM(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);
    int getPassthrough(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);
    int getAudioDelay(HOSTIF_MsgData_t *stMsgData, bool *pChanged = NULL);

public:
    static hostIf_STBServiceSPDIF *getInstance(int devid);
    static void closeInstance(hostIf_STBServiceSPDIF *);
    static GList* getAllInstances();
    static void closeAllInstances();
    static void getLock();
    static void releaseLock();
    static int getNumberOfInstances(HOSTIF_MsgData_t *stMsgData);
    void doUpdates(updateCallback mUpdateCallback);
    int handleSetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData);
    int handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData);

};
#endif /* DEVSET_COMP_AUDIOOUTPUT_HPP_ */


/** @} */
/** @} */
