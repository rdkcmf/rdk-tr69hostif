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


#ifndef TR69BUSAGENT_H
#define TR69BUSAGENT_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "hostIf_tr69ReqHandler.h"
#define TR69_BUF_MIN 64
#define TR69_BUF_MAX 1024
#define IARM_BUS_TR69_COMMON_API_AgentParameterHandler      "agentParameterHandler"

    faultCode_t TR69Bus_ProcessSharedMalloc(size_t , void **);

    /**
     * @brief TR69 Incoming Request function pointer.
     *
     * This function passes the request info structure, parameter type and value .
     *
     * @return bool true/false.
     */

    typedef bool (*fpIncomingTR69Request) (HOSTIF_MsgData_t *);
    /**
     * @brief Starts the TR69 Register Callback.
     *
     * This function registers and connects TR69 Bus Agent.
     *
     * @return bool true/false.
     */

    bool tr69Register( const char*,  fpIncomingTR69Request);

    /**
     * @brief Function Wrapper around fpIncomingTR69Request.
     *
     * @return bool true/false.
     */

    IARM_Result_t _TR69AgentCallback_FuncWrapper(void *);

    /**
     * @brief TR69 UnRegister function.
     *
     * This function unregisters and disconnects TR69 Bus Agent.
     *
     * @return bool true/false.
     */

    bool tr69UnRegister( const char *);

    /**
     * @brief TR69 Request Complete function.
     *
     * This function is responsible to map the heap memory to process
     *  of requestinfo pointer from TR69 Bus Agents.
     *
     * @return bool true/false.
     */

    bool tr69RequestComplete(HOSTIF_MsgData_t *);

#ifdef __cplusplus
}
#endif

#endif // TR69BUSAGENT_H


/** @} */
/** @} */
