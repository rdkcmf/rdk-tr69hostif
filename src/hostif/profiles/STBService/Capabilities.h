/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2017 RDK Management
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

#ifndef STBSERVICE_CAPABILITIES_H_
#define STBSERVICE_CAPABILITIES_H_

#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_main.h"


#define CAPABILITIES_OBJ "Device.Services.STBService.1.Capabilities."
#define VIDEO_DECODER_OBJ "STBService.1.Capabilities.VideoDecoder."
#define HEVC_OBJ "Device.Services.STBService.1.Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2."
#define HEVC_PROFILE_OBJ "Device.Services.STBService.1.Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel"

#define VIDEO_STANDARDS_STRING "VideoDecoder.VideoStandards"
#define HEVC_AUDIO_STDS_STRING "VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.AudioStandards"
#define PROFILE_NUM_ENTRIES_STRING "VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevelNumberOfEntries"
#define AUDIO_STANDARDS_STRING "AudioStandards"

//HEVC profile parameters
#define PROFILE_NAME_STRING "Profile"
#define PROFILE_LEVEL_STRING "Level"
#define PROFILE_MAX_DECODE_CAPABILITY_STRING "MaximumDecodingCapability"

#define HDMI_RESOLUTIONS_STRING "Device.Services.STBService.1.Capabilities.HDMI.SupportedResolutions"
class hostIf_STBServiceCapabilities
{
    hostIf_STBServiceCapabilities();
    ~hostIf_STBServiceCapabilities() {};

private:
    int getVideoStandards(HOSTIF_MsgData_t *);
    int getNumHEVCProfileEntries(HOSTIF_MsgData_t *);
    int getHEVCProfileDetails(HOSTIF_MsgData_t *, const char* attr, unsigned int index);
    int getSupportedResolutions(HOSTIF_MsgData_t *);

public:
    static hostIf_STBServiceCapabilities *getInstance();
    static void closeInstance(hostIf_STBServiceCapabilities *);
    int handleGetMsg(HOSTIF_MsgData_t *);
    int handleSetMsg(HOSTIF_MsgData_t *);
};
#endif /* STBSERVICE_CAPABILITIES_H_ */


/** @} */
/** @} */
