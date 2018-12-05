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

#include "Device_IP_Diagnostics_IPPing.h"
#include "hostIf_utils.h"
#include <string>
#include <fstream>

const char* hostIf_IP_Diagnostics_IPPing::PROFILE_NAME = "Device.IP.Diagnostics.IPPing.";

hostIf_IP_Diagnostics_IPPing::hostIf_IP_Diagnostics_IPPing ()
{
    std::string filename;
    filename.append ("/opt/tr-181/").append (PROFILE_NAME).append ("dat");

    // if file read fails or file doesn't exist, create empty file
    if (!store.load (filename))
    {
        std::ofstream output_stream (filename.c_str());
    }
}

hostIf_IP_Diagnostics_IPPing& hostIf_IP_Diagnostics_IPPing::getInstance ()
{
    static hostIf_IP_Diagnostics_IPPing instance;
    return instance;
}

int hostIf_IP_Diagnostics_IPPing::handleGetMsg (HOSTIF_MsgData_t* stMsgData)
{
    LOG_ENTRY_EXIT;

    int ret = NOK;
    std::string ipping_parameter_name (stMsgData->paramName + strlen (PROFILE_NAME));

    if (ipping_parameter_name.compare ("Host") &&
            ipping_parameter_name.compare ("NumberOfRepetitions") &&
            ipping_parameter_name.compare ("DataBlockSize"))
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Unsupported parameter '%s'\n", __FUNCTION__, stMsgData->paramName);
        stMsgData->faultCode = fcInvalidParameterName;
    }
    else
    {
        std::string value = store.value (stMsgData->paramName, "");

        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = value.length ();
        strncpy (stMsgData->paramValue, value.c_str (), stMsgData->paramLen);
        stMsgData->faultCode = fcNoFault;
        ret = OK;
    }

    return ret;
}

int hostIf_IP_Diagnostics_IPPing::handleSetMsg (HOSTIF_MsgData_t* stMsgData)
{
    LOG_ENTRY_EXIT;

    int ret = NOK;
    std::string ipping_parameter_name (stMsgData->paramName + strlen (PROFILE_NAME));

    if (ipping_parameter_name.compare ("Host") &&
            ipping_parameter_name.compare ("NumberOfRepetitions") &&
            ipping_parameter_name.compare ("DataBlockSize"))
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Unsupported parameter '%s'\n", __FUNCTION__, stMsgData->paramName);
        stMsgData->faultCode = fcInvalidParameterName;
    }
    else if (false == store.setValue (stMsgData->paramName, stMsgData->paramValue))
    {
        stMsgData->faultCode = fcInternalError;
    }
    else
    {
        stMsgData->faultCode = fcNoFault;
        ret = OK;
    }

    return ret;
}
