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

#define TR69_BUF_MIN 64
#define TR69_BUF_MAX 1024
    /*! Types of errors that could be Reported by TRBus Agent (Can be extended)*/
    typedef enum _TR69_Error_Type_t {
        TR69BUSAGENT_RESULT_SUCCESS,
        TR69BUSAGENT_RESULT_NO_SUCH_PARAM,
        TR69BUSAGENT_RESULT_INVALID_PARAM_TYPE,
        TR69BUSAGENT_RESULT_NULL_VALUE,
        TR69BUSAGENT_RESULT_NO_MEM,
        TR69BUSAGENT_RESULT_UNSUPPORTED_MODE,
        TR69BUSAGENT_RESULT_GENERAL_ERROR
    }
    TR69BusAgent_Error_Type_t;

    /* Parameter type
     */
    typedef enum _ParamterType
    {
        TR69_TYPE_String,
        TR69_TYPE_Int,
        TR69_TYPE_UnsignedInt,
        TR69_TYPE_Boolean,
        TR69_TYPE_DateTime
    } TR69BusAgent_Param_Type_t;

    typedef enum _TR69_Request_Type_t
    {
        TR69_MODE_GET,
        TR69_MODE_SET
    } TR69BusAgent_Request_Type_t;

    typedef struct _TR69BUSAgent_RequestInfo_Param_t {
        char ownerName[TR69_BUF_MIN];                                  /*!< [in] Pointer to shared memory location having null terminated name of the Agent owner (This will be deleted by the caller, like "XRE", "UPNP")*/
        char paramName[TR69_BUF_MIN];                                   /*!< [in] Pointer to shared memory location having null terminated name of the param (This will be deleted by the caller)*/
        TR69BusAgent_Param_Type_t paramType;                /*!< [in] er to shared memory location having data (This data will be allocated by callee and will be deleted by the caller)*/
        char paramValue[TR69_BUF_MAX];                                  /*!< [out] Pointer to shared memory location having data (This data will be allocated by callee and will be deleted by the caller)*/
        short  paramLen;                                    /*!< [out] size of the data pointed by pData*/
        TR69BusAgent_Request_Type_t mode;                   /*!< [in] Request type (set/get)*/
        short instanceNumber;				/*!< [in] Instance Number [Scalar =0, Tabular = 1..n] */
        TR69BusAgent_Error_Type_t err_no;          		/*!< [out] error number will be set by the callee on the failure of the call */
    } TR69RequestInfo_t;


    TR69BusAgent_Error_Type_t TR69Bus_ProcessSharedMalloc(size_t , void **);

    /**
     * @brief TR69 Incoming Request function pointer.
     *
     * This function passes the request info structure, parameter type and value .
     *
     * @return bool true/false.
     */

    typedef bool (*fpIncomingTR69Request) (TR69RequestInfo_t *);
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

    bool tr69RequestComplete(TR69RequestInfo_t * );

#ifdef __cplusplus
}
#endif

#endif // TR69BUSAGENT_H


/** @} */
/** @} */
