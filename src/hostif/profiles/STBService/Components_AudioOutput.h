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
 * @file Components_AudioOutput.h
 * @brief The header file provides capabilities audio output information APIs.
 */

/**
 * @defgroup TR69_HOSTIF_STBSERVICES TR-069 Object (Device.STBServices.{i})
 * STBService defines as the container associated with the remote management of
 * objects for STB devices. The STBService object is a service object.
 *
 * As such, individual CPE devices can contain one or more of these objects
 * within their Services object alongside the generic data objects. The presence of
 * more than one STBService object would be appropriate primarily where a CPE device serves as a
 * management proxy for other non TR-069 capable STBService devices.
 *
 * @par For example,
 * An Internet Gateway Device might serve as a management proxy for one or more non TR-069 capable STBs.
 *
 * @par Components
 * The STB data model contains a Components object that describes the devices functional blocks.
 * It contains the following components:
 * - Display Devices
 * - HDMI
 * - AudioDecoder
 * - VideoDecoder
 * - AudioOutput
 * - VideoOutput etc..
 *
 * each of which can be multi-instance.
 *
 * @ingroup TR69_HOSTIF_PROFILE
 * @defgroup TR69_HOSTIF_STBSERVICES_AUDIOOUTPUT TR-069 Object (Device.STBServices.{i}.Capabilities.AudioOutput)
 * AudioOutput objects describe the functional blocks in charge of audio rendering.
 * An Audio Output receives uncompressed audio streams from one or more Audio Decoders and
 * performs format adaptations as required by the relevant presentation standard e.g., analog mono
 * or stereo audio as needed for speakers or headphones, S/PDIF or HDMI/HDCP as needed by
 * specific digital devices.
 * - Adaptation of the audio to the specified output format can include digital-to-analog conversion
 * or other analog or digital processing, including encryption.
 * - Each Audio Output is mapped to one or more physical output connectors. Where an Audio
 * Output is mapped directly to a SCART connector, this is indicated in the data model via a
 * reference from the Audio Output to the corresponding SCART instance other types of physical
 * connector are not modelled.
 *
 * @ingroup TR69_HOSTIF_STBSERVICES
 *
 * @defgroup TR69_HOSTIF_STBSERVICES_AUDIOOUTPUT_API TR-069 Object (Device.STBServices.{i}.Capabilities.AudioOutput) Public APIs
 * Describe the details about RDK TR069 capabilities audio output APIs specifications.
 * @ingroup TR69_HOSTIF_STBSERVICES_AUDIOOUTPUT
 *
 * @defgroup TR69_HOSTIF_STBSERVICES_AUDIOOUTPUT_CLASSES TR-069 Object (Device.STBServices.{i}.Capabilities.AudioOutput) Public Classes
 * Describe the details about classes used in TR069 capabilities audio output.
 * @ingroup TR69_HOSTIF_STBSERVICES_AUDIOOUTPUT
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

/**
 * @brief This class provides the TR069 components audio output information.
 * @ingroup TR69_HOSTIF_STBSERVICES_AUDIOOUTPUT_CLASSES
 */
class hostIf_STBServiceAudioInterface
{
    static GHashTable *ifHash;
    hostIf_STBServiceAudioInterface(int dev_id, device::AudioOutputPort& port);
    ~hostIf_STBServiceAudioInterface() {};
    static GMutex *m_mutex;
    int dev_id;
    device::AudioOutputPort& aPort;

    char backupStatus[_BUF_LEN_16];
    bool backupCancelMute;
    char backupAudioStereoMode[_BUF_LEN_16];
    unsigned int backupAudioLevel;
    char backupAudioDB[_BUF_LEN_16];
    char backupAudioLoopThru[_BUF_LEN_16];
    char backupAudioCompression[_BUF_LEN_16];
    char backupAudioEncoding[_BUF_LEN_16];
    char backupAudioGain[_BUF_LEN_16];
    char backupMinAudioDB[_BUF_LEN_16];
    char backupMaxAudioDB[_BUF_LEN_16];
    char backupAudioOptimalLevel[_BUF_LEN_16];

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
