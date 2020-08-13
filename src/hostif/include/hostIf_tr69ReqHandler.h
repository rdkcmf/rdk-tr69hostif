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
* @file hostIf_tr69MsgHandler.h
*
* @brief hostIf Common Message Handler API.
*
* This API defines the core operations for Host interface
*
* @par Document
* Document reference.
*
* @par Open Issues (in no particular order)
* -# None
*
* @par Assumptions
* -# None
*
* @par Abbreviations
* - BE:      ig-Endian.
* - cb:      allback function (suffix).
* - hostIf:  Host Interface.
* - DS:      Device Settings.
* - FPD:     Front-Panel Display.
* - HAL:     Hardware Abstraction Layer.
* - LE:      Little-Endian.
* - LS:      Least Significant.
* - MBZ:     Must be zero.
* - MS:      Most Significant.
* - RDK:     Reference Design Kit.
* - _t:      Type (suffix).
*
* @par Implementation Notes
* -# None
*
*/

/**
*  HOST-IF is a platform agnostic Inter-process communication (IPC) interface. It allows
*  applications to communicate with each other by sending Events or invoking Remote
*  Procedure Calls. The common programming APIs offered by the RDK IARM-Bus interface is
*  independent of the operating system or the underlying IPC mechanism.
*
*/

/** @addtogroup IARM_BUS_IARM_CORE_API IARM-Core library.
*  @ingroup IARM_BUS
*
*  Described herein are the functions that are part of the
*  IARM Core library.
*
*  @{
*/


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef HOSTIF_TR69REQHANDLER_H_
#define HOSTIF_TR69REQHANDLER_H_


#define IARM_BUS_TR69HOSTIFMGR_NAME                                            "tr69HostIfMgr"  /*!< Host-If manager IARM bus name */
#define IARM_TR69_CLIENT        												"tr69Client"
bool hostIf_IARM_IF_Start();
void hostIf_IARM_IF_Stop();
/*
 * Declare RPC API names and their arguments
 */
#define IARM_BUS_TR69HOSTIFMGR_API_SetParams               "tr69HostIfSetParams" /*!< Sets the parameter values to the tr69hostIf*/

#define IARM_BUS_TR69HOSTIFMGR_API_GetParams               "tr69HostIfGetParams" /*!< Retrives parameter values fom the tr69hostIf*/

#define IARM_BUS_TR69HOSTIFMGR_API_SetAttributes               "tr69HostIfGetAttributes" /*!< Retrives attribute values fom the tr69hostIf*/
#define IARM_BUS_TR69HOSTIFMGR_API_GetAttributes               "tr69HostIfSetAttributes" /*!< Sets attribute values fom the tr69hostIf*/


#define IARM_BUS_TR69HOSTIFMGR_API_RegisterForEvents       "tr69HostIfRegisterForEvents" /*!< Registers for events from tr69hostIf, on this call, add/remove events will be re-broadcasted*/

#define TR69HOSTIFMGR_MAX_PARAM_LEN     (2*1024)

#define _BUF_LEN_16 16

#define _BUF_LEN_32 32

#define _BUF_LEN_64 64

#define _BUF_LEN_128 128

#define _BUF_LEN_256 256

#define _BUF_LEN_512 512

#define _BUF_LEN_1024 1024

/*! Parameter for Getpowerstate call*/
/*! Host IF Message Parameter data-type*/
typedef enum _HostIf_ParamType
{
    hostIf_StringType = 0,
    hostIf_IntegerType,
    hostIf_UnsignedIntType,
    hostIf_BooleanType,
    hostIf_DateTimeType,
    hostIf_UnsignedLongType
}
HostIf_ParamType_t;

/*! Host IF request type*/
typedef enum _HostIf_ReqType
{
    HOSTIF_INVALID_TYPE = 0,
    HOSTIF_GET = 1,
    HOSTIF_SET = 2,
    HOSTIF_GETATTRIB = 3,
    HOSTIF_SETATTRIB = 4
} HostIf_ReqType_t;

/*! Host IF Error Fault Code type*/
typedef enum _faultCodes
{
    fcNoFault = 0,
    fcMethodNotSupported = 9000,
    fcRequestDenied,
    fcInternalError,
    fcInvalidArguments,
    fcResourcesExceeded,
    fcInvalidParameterName,
    fcInvalidParameterType,
    fcInvalidParameterValue,
    fcAttemptToSetaNonWritableParameter = 9008,
} faultCode_t;

/*! Host IF source type used for various purposes*/
typedef enum _HostIf_Source_Type_t
{
    HOSTIF_NONE = 0,
    HOSTIF_SRC_ALL,
    HOSTIF_SRC_WEBPA,
    HOSTIF_SRC_RFC,
    HOSTIF_SRC_IARM,
    HOSTIF_SRC_DEFAULT
} HostIf_Source_Type_t;

/*! Host IF Message Request data*/
typedef struct _HostIf_MsgData_t {
    char paramName[TR69HOSTIFMGR_MAX_PARAM_LEN];   			/*!< Parameter name with complete request path */
    char paramValue[TR69HOSTIFMGR_MAX_PARAM_LEN];   			/*!< Parameter Value */
    char transactionID[_BUF_LEN_256];   			/*!< transaction identifier Value */
    short paramLen;             	/*!< Size of Response parameter data*/
    short instanceNum;              /*!< Instances number of Response parameter data*/
    HostIf_ParamType_t paramtype;	/*!< Parameter type */
    HostIf_ReqType_t reqType;		/*!< Host interface request type [HOSTIF_GET/HOSTIF_SET]*/
    faultCode_t faultCode;          /*!< Fault codes as per BroadBand cwmp amendment*/
    HostIf_Source_Type_t requestor;   /*!< Requestor that made the set/get request*/
    HostIf_Source_Type_t bsUpdate; /*!< Bootstrap update level*/
} HOSTIF_MsgData_t;

/*! Events published from TR69 host interface */
typedef enum _tr69HostIfMgr_EventId_t {
    IARM_BUS_TR69HOSTIFMGR_EVENT_ADD,   	    /*!< Add Event  */
    IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE,        /*!< Remove Event  */
    IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,  /*!< Value changed Event  */
    IARM_BUS_TR69HOSTIFMGR_EVENT_MAX,           /*!< Maximum event id*/
} IARM_Bus_tr69HostIfMgr_EventId_t;


/*! tr69HostIfMgr Event Data */
typedef struct _tr69HostIfMgr_EventData_t {

    char paramName[TR69HOSTIFMGR_MAX_PARAM_LEN];
    char paramValue[TR69HOSTIFMGR_MAX_PARAM_LEN];
    HostIf_ParamType_t paramtype;	/*!< Parameter type */

} IARM_Bus_tr69HostIfMgr_EventData_t;


#endif /* HOSTIF_TR69REQHANDLER_H_*/

/* End of HOST_IF_API doxygen group */
/**
 * @}
 */


/** @} */
/** @} */
