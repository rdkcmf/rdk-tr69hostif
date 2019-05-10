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
 * @file Components_VideoOutput.h
 * @brief The header file provides capabilities video output information APIs.
 */

/**
 * @defgroup TR69_HOSTIF_STBSERVICES_VIDEOOUTPUT TR-069 Object (Device.STBServices.{i}.Capabilities.VideoOutput)
 * @par VideoOutput objects
 * It describe the functional blocks in charge of video rendering. A Video Output receives uncompressed
 * video streams from one or more Video Decoders, and performs format adaptations as required by the
 * relevant presentation standard e.g. analog or digital displays, possibly with an encrypted link between
 * STB and display.
 * Video Outputs can also provide colour bar test patterns to check the operation of the display device
 * and the existence and quality of the connection between STB and display. Adaptation of the video to
 * the specified output format can include various actions like
 * - Digital-to-analog conversion,
 * - Resizing,
 * - Aspect ratio conversion and
 * - Addition of analog or digital protection.
 *
 * Each Video Output is mapped to one or more physical output connectors. Where a Video Output
 * is mapped directly to a SCART connector, this is indicated in the data model via a reference
 * from the Video Output to the corresponding SCART object instance other types of physical
 * connector are not modelled.
 * @ingroup TR69_HOSTIF_STBSERVICES
 *
 * @defgroup TR69_HOSTIF_STBSERVICES_VIDEOOUTPUT_API TR-069 Object (Device.STBServices.{i}.Capabilities.VideoOutput) Public APIs
 * Describe the details about RDK TR069 capabilities of video output APIs specifications.
 * @ingroup TR69_HOSTIF_STBSERVICES_VIDEOOUTPUT
 *
 * @defgroup TR69_HOSTIF_STBSERVICES_VIDEOOUTPUT_CLASSES TR-069 Object (Device.STBServices.{i}.Capabilities.VideoOutput) Public Classes
 * Describe the details about classes used in TR069 capabilities of video output.
 * @ingroup TR69_HOSTIF_STBSERVICES_VIDEOOUTPUT
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef DEVSET_COMP_VIDEOOUTPUT_HPP_
#define DEVSET_COMP_VIDEOOUTPUT_HPP_

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
 * @brief This class provides the TR-069 components video output information.
 * @ingroup TR69_HOSTIF_STBSERVICES_VIDEOOUTPUT_CLASSES
 */
class  hostIf_STBServiceVideoOutput
{
    static GHashTable *ifHash;
    hostIf_STBServiceVideoOutput(int devid, device::VideoOutputPort& port);
    ~hostIf_STBServiceVideoOutput() {};
    static GMutex *m_mutex;
    int dev_id;
    device::VideoOutputPort& vPort;

    char backupAspectRatioBehaviour[_BUF_LEN_16];
    char backupDisplayFormat[_BUF_LEN_16];
    char backupVideoFormat[_BUF_LEN_16];
    bool backupHDCP;
    char backupVideoOutputStatus[_BUF_LEN_16];

    bool bCalledAspectRatioBehaviour;
    bool bCalledDisplayFormat;
    bool bCalledVideoFormat;
    bool bCalledHDCP;
    bool bCalledVideoOutputStatus;

private:
    int getAspectRatioBehaviour(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
    int getDisplayFormat(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
    int getVideoFormat(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
    int getHDCP(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
    int getStatus(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);

public:
    static hostIf_STBServiceVideoOutput *getInstance(int dev_id);
    static void closeInstance(hostIf_STBServiceVideoOutput *);
/**
 * @brief This function checks for the host IF video interface updates such as status, display format, video format,
 * aspect ratio and HDPC (High-Bandwidth Digital Content Protection) in a connected video port. Currently
 * not implemented.
 *
 * @param[in] mUpdateCallback  Callback function pointer to check for the host IF video interface update.
 * @ingroup TR69_HOSTIF_STBSERVICES_VIDEOOUTPUT_API
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

#endif /* DEVSET_COMP_VIDEOOUTPUT_HPP_ */


/** @} */
/** @} */
