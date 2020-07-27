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
 * @file Components_VideoDecoder.h
 * @brief The header file provides capabilities video output information APIs.
 */

/**
 * @defgroup TR69_HOSTIF_STBSERVICES_VIDEODECODER TR-069 Object (Device.STBServices.{i}.Components.VideoDecoder.{i})
 * @par VideoDecoder objects
 *  - It describe the functional blocks in charge of video decoding.
 *  - A Video Decoder receives an elementary video stream, decodes the video, and outputs an
 * uncompressed native video stream to a Video Output object.
 * @ingroup TR69_HOSTIF_STBSERVICES
 *
 * @defgroup TR69_HOSTIF_STBSERVICES_VIDEODECODER_API TR-069 Object (Device.STBServices.{i}.Components.VideoDecoder.{i}) Public APIs
 * Describe the details about TR-069 components video decoder APIs specifications.
 * @ingroup TR69_HOSTIF_STBSERVICES_VIDEODECODER
 *
 * @defgroup TR69_HOSTIF_STBSERVICES_VIDEODECODER_CLASSES TR-069 Object (Device.STBServices.{i}.Components.VideoDecoder.{i}) Public Classes
 * Describe the details about classes used in TR069 components video decoder.
 * @ingroup TR69_HOSTIF_STBSERVICES_VIDEODECODER
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef DEVSET_COMP_VIDEODECODER_HPP_
#define DEVSET_COMP_VIDEODECODER_HPP_

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

#ifndef PARAM_LEN
#define PARAM_LEN TR69HOSTIFMGR_MAX_PARAM_LEN
#endif

/**
 * @brief This class provides the TR069 components video decoder information.
 * @ingroup TR69_HOSTIF_STBSERVICES_VIDEODECODER_CLASSES
 */
class  hostIf_STBServiceVideoDecoder
{
    static GHashTable *ifHash;
    hostIf_STBServiceVideoDecoder(int devid);
    ~hostIf_STBServiceVideoDecoder() {};
    static GMutex *m_mutex;
    int dev_id;

    char backupContentAspectRatio[_BUF_LEN_16];
    bool backupStandby;
    char backupVideoDecoderStatus[_BUF_LEN_32];

    bool bCalledContentAspectRatio;
    bool bCalledStandby;
    bool bCalledVideoDecoderStatus;

private:
    int getContentAspectRatio(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
    int getX_COMCAST_COM_Standby(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
    int getStatus(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
    int setX_COMCAST_COM_Standby(const HOSTIF_MsgData_t *stMsgData);

public:
    static hostIf_STBServiceVideoDecoder *getInstance(int dev_id);
    static void closeInstance(hostIf_STBServiceVideoDecoder *);
/**
 * @brief This function checks for the host IF video decoder interface updates such as Status,
 * ContentAspectRatio, X_COMCAST-COM_Standby, Name in a connected video decoder. Currently
 * not implemented.
 *
 * @param[in] mUpdateCallback  Callback function pointer to check for the host IF video decoder
 * interface update.
 * @ingroup TR69_HOSTIF_STBSERVICES_VIDEODECODER_API
 */
    void checkForUpdates(updateCallback mUpdateCallback);
    static GList* getAllInstances();
    static void closeAllInstances();
    static void getLock();
    static void releaseLock();
    void doUpdates(updateCallback mUpdateCallback);
    int handleSetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData);
    int handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData);

};

#endif /* DEVSET_COMP_VIDEODECODER_HPP_ */


/** @} */
/** @} */
