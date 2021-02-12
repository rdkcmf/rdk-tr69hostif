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


#include <string>
#include <list>
#include "rdk_debug.h" 
#include "hostIf_main.h"
#include "Capabilities.h"

#include "host.hpp"
#include "dsTypes.h"
#include "videoDevice.hpp"

#define MAX_RESOLUTION_LENGTH 30

hostIf_STBServiceCapabilities* hostIf_STBServiceCapabilities::getInstance()
{
    hostIf_STBServiceCapabilities* pRet = NULL;

    if(!pRet)
    {
        try {
            pRet = new hostIf_STBServiceCapabilities();
        } catch(const std::exception& e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Caught exception \" %s\"\n", __FUNCTION__, e.what());
        }
    }
    return pRet;
}


void hostIf_STBServiceCapabilities::closeInstance(hostIf_STBServiceCapabilities *pDev)
{
    if(pDev)
    {
        delete pDev;
    }
}

hostIf_STBServiceCapabilities::hostIf_STBServiceCapabilities()
{
}

int hostIf_STBServiceCapabilities::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    stMsgData->faultCode = fcAttemptToSetaNonWritableParameter;
    return ret;
}

int hostIf_STBServiceCapabilities::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *path = NULL, *paramName = NULL, *attr = NULL;
    int index = 0;
    try {
        int str_len = strlen(CAPABILITIES_OBJ);
        path = stMsgData->paramName;

        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s]Entering...  \n", __FILE__, __FUNCTION__);

        if(NULL == path) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d]Failed : Parameter is NULL, %s  \n", __FILE__, __FUNCTION__, __LINE__, path);
            stMsgData->faultCode = fcInvalidParameterName;
            return ret;
        }

        if((strncasecmp(path, CAPABILITIES_OBJ, str_len) != 0)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d]Failed : Mismatch parameter path : %s  \n", __FILE__, __FUNCTION__, __LINE__, path);
            stMsgData->faultCode = fcInvalidParameterName;
            return ret;
        }

        /* Parse video decoder object.*/
        const char *tmp_ptr = strchr(path+str_len-1,'.');
        if(tmp_ptr == NULL)  {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Parameter is NULL  \n", __FILE__, __FUNCTION__);
            stMsgData->faultCode = fcInvalidParameterName;
            return ret;
        }

        tmp_ptr++;
        paramName = tmp_ptr; //Now points to STBService.1.Capabilities.*
        if (strcasecmp(paramName, VIDEO_STANDARDS_STRING) == 0)
        {
            ret = getVideoStandards(stMsgData);
        }
        else if(strcasecmp(paramName, PROFILE_NUM_ENTRIES_STRING) == 0)
        {
            ret = getNumHEVCProfileEntries(stMsgData); 
        }
        else if(matchComponent(stMsgData->paramName, HEVC_PROFILE_OBJ, &attr, index))
        {
            //Profile-specific details. One of many profiles.
            ret = getHEVCProfileDetails(stMsgData, attr, index);
        }
        else if(strcasecmp(stMsgData->paramName, HDMI_RESOLUTIONS_STRING) == 0)
        {
            ret = getSupportedResolutions(stMsgData);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Parameter \'%s\' is Not Supported  \n", __FILE__, __FUNCTION__, paramName);
            stMsgData->faultCode = fcInvalidParameterName;
            ret = NOK;
        }
    }
    catch (const std::exception& e )
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Exception caught %s   \n", __FILE__, __FUNCTION__, e.what());
        stMsgData->faultCode = fcInternalError;
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s]Exiting... \n", __FILE__, __FUNCTION__);
    return ret;
}

int hostIf_STBServiceCapabilities::getVideoStandards(HOSTIF_MsgData_t *stMsgData)
{
    int bytes_written = 0;
    try {
        device::VideoDevice decoder = device::Host::getInstance().getVideoDevices().at(0);
        unsigned int supported_standards = decoder.getSupportedVideoCodingFormats();
        if(0 != (supported_standards & dsVIDEO_CODEC_MPEGHPART2))
        {
            bytes_written += snprintf(&(stMsgData->paramValue[bytes_written]), (TR69HOSTIFMGR_MAX_PARAM_LEN - bytes_written), "%s,", "MPEGH-Part2 ([ISO/IEC23008-1]])");
        }
        if(0 != (supported_standards & dsVIDEO_CODEC_MPEG2))
        {
            bytes_written += snprintf(&(stMsgData->paramValue[bytes_written]), (TR69HOSTIFMGR_MAX_PARAM_LEN - bytes_written), "%s,", "MPEG2-Part2 ([ISO/IEC13818-1])");
        }
        if(0 != (supported_standards & dsVIDEO_CODEC_MPEG4PART10))
        {
            bytes_written += snprintf(&(stMsgData->paramValue[bytes_written]), (TR69HOSTIFMGR_MAX_PARAM_LEN - bytes_written), "%s,", "MPEG4-Part10 ([ISO/IEC14496-10])");
        }
        stMsgData->paramValue[bytes_written -1] = '\0'; //substitue string terminator for the final comma.
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] : Value: %s \n",__FUNCTION__, stMsgData->paramValue);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\n",__FUNCTION__);
        stMsgData->faultCode = fcInternalError;
        return NOK;
    }

    return OK;
}

int hostIf_STBServiceCapabilities::getNumHEVCProfileEntries(HOSTIF_MsgData_t *stMsgData)
{
    try {
        device::VideoDevice decoder = device::Host::getInstance().getVideoDevices().at(0);
        dsVideoCodecInfo_t info = decoder.getVideoCodecInfo(dsVIDEO_CODEC_MPEGHPART2);

        if(0 == info.num_entries)
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Zero profile entries reported.\n",__FUNCTION__);
            stMsgData->faultCode = fcInternalError;
            return NOK;
        }
        put_int(stMsgData->paramValue, info.num_entries);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] : Value: %s \n",__FUNCTION__, stMsgData->paramValue);
        stMsgData->paramtype = hostIf_UnsignedIntType;
        stMsgData->paramLen = sizeof(unsigned int); 
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\n",__FUNCTION__);
        stMsgData->faultCode = fcInternalError;
        return NOK;
    }

    return OK;
}

static const char * convertHEVCProfileNameToString(dsVideoCodecHevcProfiles_t profile)
{
    switch(profile)
    {
        case dsVIDEO_CODEC_HEVC_PROFILE_MAIN:
            return "MAIN";
        case dsVIDEO_CODEC_HEVC_PROFILE_MAIN10:
            return "MAIN 10";
        case dsVIDEO_CODEC_HEVC_PROFILE_MAINSTILLPICTURE:
            return "MAIN STILL PICTURE";
        default:
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Unknown profile 0x%x!\n",__FUNCTION__, (unsigned int)profile);
            return "";
    }
}

static unsigned int getMaxHEVCDecodeKBitRate(dsVideoCodecProfileSupport_t &entry)
{
    unsigned int kbit_rate = 0; 
    if((dsVIDEO_CODEC_HEVC_PROFILE_MAIN10 == entry.profile) && (5.1f == entry.level))
    {
        kbit_rate = 40000;
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Unknown profile (0x%x) and level (%g) combination!\n",__FUNCTION__, entry.profile, entry.level);
    }
    return kbit_rate;
}

static const char* getTR181ResolutionString(std::string &resolution)
{
    if("720p" == resolution)
        return "1280x720p/59.94Hz";
    else if("1080i" == resolution)
        return "1920x1080i/59.94Hz";
    else if(("1080p60" == resolution) || ("1080p" == resolution))
        return "1920x1080p/59.94Hz";
    else if("2160p30" == resolution)
        return "3840x2160p/30Hz";
    else if("2160p60" == resolution)
        return "3840x2160p/59.94Hz";
    else if("480i" == resolution)
        return "720x480i/59.94Hz";
    else if("480p" == resolution)
        return "720x480p/59.94Hz";
    else if(("576p50" == resolution) || ("576p" == resolution))
        return "720x576p/50Hz";
    else if("720p50" == resolution)
        return "1280x720p/50Hz";
    else if("1080p30" == resolution)
        return "1920x1080p/30Hz";
    else if(("1080i50" == resolution) || ("1080i25" == resolution))
        return "1920x1080i/50Hz";
    else if("1080p24" == resolution)
        return "1920x1080p/24Hz";
    else if("1080p50" == resolution)
        return "1920x1080p/50Hz";
    else if("2160p50" == resolution)
        return "3840x2160p/50Hz";
    else if ("1080p25" == resolution)
        return "1920x1080p/25Hz";
    else
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Unhandled resolution: %s. Cannot translate!\n", __FUNCTION__, resolution.c_str());
        return "";
    }
}

int hostIf_STBServiceCapabilities::getHEVCProfileDetails(HOSTIF_MsgData_t * stMsgData, const char* attr, unsigned int index)
{
    int ret = OK;
    int bytes_written = 0;
    try {
        device::VideoDevice decoder = device::Host::getInstance().getVideoDevices().at(0);
        dsVideoCodecInfo_t info = decoder.getVideoCodecInfo(dsVIDEO_CODEC_MPEGHPART2);

        if((0 == info.num_entries) || (index > info.num_entries))
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Could not find profiles matching index %d.\n",__FUNCTION__, index);
            stMsgData->faultCode = ((0 == info.num_entries) ? fcInternalError : fcInvalidParameterName );
            return NOK;
        }

        if(strcasecmp(attr, PROFILE_NAME_STRING) == 0)
        {
            bytes_written = snprintf(stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN, "%s", convertHEVCProfileNameToString(info.entries[index - 1].profile));
            stMsgData->paramValue[bytes_written] = '\0';
            stMsgData->paramtype = hostIf_StringType;
            stMsgData->paramLen = bytes_written;
        }
        else if(strcasecmp(attr, PROFILE_LEVEL_STRING) == 0)
        {
            bytes_written = snprintf(stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN, "L%g", info.entries[index - 1].level);
            stMsgData->paramValue[bytes_written] = '\0';
            stMsgData->paramtype = hostIf_StringType;
            stMsgData->paramLen = bytes_written;
        }
        else if(strcasecmp(attr, PROFILE_MAX_DECODE_CAPABILITY_STRING) == 0)
        {
            put_int(stMsgData->paramValue,getMaxHEVCDecodeKBitRate(info.entries[index -1])); 
            stMsgData->paramtype = hostIf_UnsignedIntType;
            stMsgData->paramLen = sizeof(unsigned int);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Parameter \'%s\' is Not Supported  \n", __FILE__, __FUNCTION__, attr);
            stMsgData->faultCode = fcInvalidParameterName;
            ret = NOK;
        }

    }
    catch (...) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\n",__FUNCTION__);
        stMsgData->faultCode = fcInternalError;
        return NOK;
    }

    return OK;

}

int hostIf_STBServiceCapabilities::getSupportedResolutions(HOSTIF_MsgData_t *stMsgData)
{
    size_t iElementInList = 0;
    size_t iResolutionsListSize = 0;
    char aiResolution[MAX_RESOLUTION_LENGTH] = {'\0'};
    try
    {
        std::list <std::string> supported_resolutions;
        device::VideoDevice decoder = device::Host::getInstance().getVideoDevices().at(0);
        decoder.getSettopSupportedResolutions(supported_resolutions);
        iResolutionsListSize = supported_resolutions.size();

        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] : List Size: %d \n",__FUNCTION__, iResolutionsListSize);
        memset(stMsgData->paramValue, 0, TR69HOSTIFMGR_MAX_PARAM_LEN);

        std::list<std::string>::iterator entry;
        for(entry = supported_resolutions.begin(); entry != supported_resolutions.end(); entry++)
        {
            iElementInList++;
            snprintf(aiResolution, MAX_RESOLUTION_LENGTH, "%s", getTR181ResolutionString(*entry));
            strncat(stMsgData->paramValue, aiResolution, TR69HOSTIFMGR_MAX_PARAM_LEN-strlen(stMsgData->paramValue)-1);
            if(iElementInList < iResolutionsListSize)
                strncat(stMsgData->paramValue,",", TR69HOSTIFMGR_MAX_PARAM_LEN-strlen(stMsgData->paramValue)-1);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] : resolution: %s\n",__FUNCTION__, (*entry).c_str());
        }
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s]  : Value: %s \n",__FUNCTION__, stMsgData->paramValue);
    }
    catch (...) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\n",__FUNCTION__);
        stMsgData->faultCode = fcInternalError;
        return NOK;
    }
    return OK;

}
