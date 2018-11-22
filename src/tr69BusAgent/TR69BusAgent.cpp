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
* @file
*
* @brief TR69 Bus Agent Public API.
*
*/



/**
* @defgroup tr69hostif
* @{
* @defgroup tr69BusAgent
* @{
**/


#ifdef __cplusplus
extern "C"
{
#endif
#include <cstdio>
#include <unistd.h>
#include <cstring>

#include "libIBus.h"
#include "libIARMCore.h"
#include "TR69BusAgent.h"
#include "rdk_debug.h"
#define LOG_TR69HOSTIF  "LOG.RDK.TR69HOSTIF"

#define MAX_RETRY_LOOP 15

    fpIncomingTR69Request tr69AgentCallback;

    faultCode_t TR69Bus_ProcessSharedMalloc(size_t size, void **ptr)
    {
        IARM_Result_t retCode = IARM_Malloc(IARM_MEMTYPE_PROCESSLOCAL, size, ptr);

        if (IARM_RESULT_SUCCESS != retCode)
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s: Failed to malloc from shared mem, IARM err code = %d\n", __FUNCTION__, retCode);

        switch (retCode)
        {
        case IARM_RESULT_SUCCESS:
            return fcNoFault;
        case IARM_RESULT_OOM:
            return fcRequestDenied;
        case IARM_RESULT_INVALID_PARAM:
        case IARM_RESULT_IPCCORE_FAIL:
        default:
            return fcInternalError;
        }
    }

    IARM_Result_t _TR69AgentCallback_FuncWrapper(void *arg)
    {
        HOSTIF_MsgData_t *requestInfo = (HOSTIF_MsgData_t *) arg;
//        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] Enter \n", __FILE__, __FUNCTION__, __LINE__);
        if (NULL == requestInfo)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s: Null RequestInfo Arguments Caught !!!\n", __FUNCTION__);
            return IARM_RESULT_IPCCORE_FAIL;
        }

        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] paramName:%s \n", __FILE__, __FUNCTION__, __LINE__, requestInfo->paramName);

        if (false == tr69AgentCallback(requestInfo))
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s: tr69AgentCallback returned error !!!\n", __FUNCTION__);
            return IARM_RESULT_IPCCORE_FAIL;
        }

        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s: tr69AgentCallback returned success !!!\n", __FUNCTION__);
//        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Exit \n", __FILE__, __FUNCTION__, __LINE__);
        return IARM_RESULT_SUCCESS;
    }

    bool tr69Register( const char *pOwnerName, fpIncomingTR69Request incomingTr69RequestCallback)
    {
        IARM_Result_t ret = IARM_RESULT_SUCCESS;


        if (!pOwnerName)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s: Owner's name cannot be null \r\n", __FUNCTION__);
            return false;
        }

        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"%s: Registering %s callback function (%s) \r\n", __FUNCTION__, pOwnerName, IARM_BUS_TR69_COMMON_API_AgentParameterHandler);

        tr69AgentCallback = incomingTr69RequestCallback;
        if (NULL == tr69AgentCallback)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error registering callback for %s, incomingTr69RequestCallback is null\n", pOwnerName);
            return false;
        }

        for(int loop = 0; loop < MAX_RETRY_LOOP; loop++)
        {
        	ret = IARM_Bus_RegisterCall(IARM_BUS_TR69_COMMON_API_AgentParameterHandler, _TR69AgentCallback_FuncWrapper);
        	if(IARM_RESULT_SUCCESS == ret)
        	{
//        		RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] Success in IARM_Bus_RegisterCall with RPC: %s \r\n", __FILE__, __FUNCTION__, __LINE__, IARM_BUS_TR69_COMMON_API_AgentParameterHandler);
//        		RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] Success to register %s with %s \r\n", __FILE__, __FUNCTION__, __LINE__, pOwnerName, IARM_BUS_TR69_BUS_MGR_NAME);
        		break;
        	}
        	else{
        		RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] Failed in IARM_Bus_RegisterCall with RPC: %s, so retrying [ %d ] times. \r\n", __FILE__, __FUNCTION__, __LINE__, IARM_BUS_TR69_COMMON_API_AgentParameterHandler, loop);
        	}
        	sleep (2);
		}
        return true;
    }

    bool tr69UnRegister(const char *pOwnerName)
    {
        IARM_Result_t ret = IARM_RESULT_SUCCESS;
        if (!pOwnerName)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s: Owner's name cannot be null \r\n", __FUNCTION__);
            return false;
        }
        return true;
    }

    bool tr69RequestComplete(HOSTIF_MsgData_t *requestInfo)
    {
        char *paramValue = NULL;

        if (fcNoFault != requestInfo->faultCode)
            return false;

        if (HOSTIF_GET == requestInfo->reqType)
        {
            if ('\0' == requestInfo->paramValue[0])
            {
                requestInfo->faultCode = fcInvalidParameterValue;
                return false;
            }
        }
        //RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Exit \n", __FILE__, __FUNCTION__, __LINE__);
        return true;
    }

#ifdef __cplusplus
}
#endif



/** @} */
/** @} */
