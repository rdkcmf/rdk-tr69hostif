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


#ifndef DEVSET_COMP_AUDIOOUTPUT_HPP__
#define DEVSET_COMP_AUDIOOUTPUT_HPP__

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

class hostIf_STBServiceAudioInterface
{
    static GHashTable *ifHash;
    hostIf_STBServiceAudioInterface(int dev_id, device::AudioOutputPort& port);
    ~hostIf_STBServiceAudioInterface() {};
    static GMutex *m_mutex;
    int dev_id;
    device::AudioOutputPort& aPort;

    char backupStatus[PARAM_LEN];
    bool backupCancelMute;
    char backupAudioStereoMode[PARAM_LEN];
    unsigned int backupAudioLevel;
    char backupAudioDB[PARAM_LEN];
    char backupAudioLoopThru[PARAM_LEN];
    char backupAudioCompression[PARAM_LEN];
    char backupAudioEncoding[PARAM_LEN];
    char backupAudioGain[PARAM_LEN];
    char backupMinAudioDB[PARAM_LEN];
    char backupMaxAudioDB[PARAM_LEN];
    char backupAudioOptimalLevel[PARAM_LEN];

    bool bCalledStatus;
    bool bCalledCancelMute;
    bool bCalledAudioStereoMode;
    bool bCalledAudioLevel;
    bool bCalledAudioDB;
    bool bCalledAudioLoopThru;
    bool bCalledAudioCompression;
    bool bCalledAudioEncoding;
    bool bCalledAudioGain;
    bool bCalledMinAudioDB;
    bool bCalledMaxAudioDB;
    bool bCalledAudioOptimalLevel;

private:
    int getStatus(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    int setCancelMute(const HOSTIF_MsgData_t *);
    int getCancelMute(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    int getEnable(HOSTIF_MsgData_t *);
    int getName(HOSTIF_MsgData_t *);

    int setX_COMCAST_COM_AudioStereoMode(const HOSTIF_MsgData_t *);
    int getX_COMCAST_COM_AudioStereoMode(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    int setAudioLevel(const HOSTIF_MsgData_t *);
    int getAudioLevel(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    int setX_COMCAST_COM_AudioDB(const HOSTIF_MsgData_t *);
    int getX_COMCAST_COM_AudioDB(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    int setX_COMCAST_COM_AudioLoopThru(const HOSTIF_MsgData_t *);
    int getX_COMCAST_COM_AudioLoopThru(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /* X_COMCAST_COM_AudioCompression */
    int setX_COMCAST_COM_AudioCompression(const HOSTIF_MsgData_t *);
    int getX_COMCAST_COM_AudioCompression(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /* X_COMCAST_COM_AudioFormat  - Read Only Data as per TR 135*/
    int getX_COMCAST_COM_AudioFormat(HOSTIF_MsgData_t *);

    /* X_COMCAST_COM_AudioEncoding */
    int setAudioEncoding(const HOSTIF_MsgData_t *);
    int getX_COMCAST_COM_AudioEncoding(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /* X_COMCAST_COM_AudioGain */
    /*     int setAudioGain(HOSTIF_MsgData_t *,bool *pChanged = NULL); */ // Not supported???
    int getX_COMCAST_COM_AudioGain(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    int getX_COMCAST_COM_MinAudioDB(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
    int getX_COMCAST_COM_MaxAudioDB(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
    int getX_COMCAST_COM_AudioOptimalLevel(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);


public:
    static hostIf_STBServiceAudioInterface *getInstance(int dev_id);
    static void closeInstance(hostIf_STBServiceAudioInterface *);
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
