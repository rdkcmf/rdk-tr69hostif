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
 * @file hostIf_jsonReqHandler.h
 * @brief The header file provides HostIf IP json request handler information APIs.
 */

/**
 *
 * @defgroup TR-069HOSTIF_JSON_REQHANDLER_API IP Json RequestHandler Public API
 * Describe the details about API used in TR-069 json request handler.
 * @ingroup TR-069HOSTIF_DEVICECLIENT_HANDLER
 */

 /**
 *  Json-interface is a platform agnostic http server communication interface. It allows
 *  json client applications to communicate by sending Get operation
 *
 */


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef HOSTIFJSONIFHANDLER_H_
#define HOSTIFJSONIFHANDLER_H_


#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>
#include "hostIf_main.h"
#include "hostIf_msgHandler.h"




void hostIf_HttpServerStart();

/**
 * @brief Handle the Post message from HTTP Json client.
 *
 * This API is used to listen the http messages and extract the Json payload.
 * Next will pass the Json message to common API
 *
 *
 * @return bool true/false.
 * @retval true on success.
*/
bool hostIf_HTTPJsonMsgHandler();


/**
 * @brief It handles the request and response  IARM member processes.
 *
 * This API handles the incoming request from http and forward to common Host IF API
 * to get the respective parameter data. It also create and return the respond
 * the Json object through http message.
 *
 * @param [in] incommingRequest This is Json incoming request object.
 * @param [ou] outResponse This is Json incoming response object.
 *
 * @return bool true/false.
 * @retval true on success
 */
void hostIf_HttpServerStop();

/**
 * @brief This is parsing the Json message and construct the respective structure
 * to send to common request handler API.
 *
 * This API is used to parse the Json messages and mapp to respective HOSTIF_MsgData_t
 * structure to pass the common request handler.
 *
 * @param [in] incommingReq The incoming request Json message.
 * @param [out] strMsgData This the data structure of parameter data.
 *
 * @return bool true/false.
 * @retval true on success
 */
//bool hostIf_ParseJsonReq(json_t *incommingReq, HOSTIF_MsgData_t *strMsgData);


/**
 * @brief Create the json object from response structure.
 *
 * This API is creating the Json object from response structure.
 *
 * @param [in] stMsgData The date structure of HOSTIF_MsgData_t structure.
 *
 * @return json object.
 * @retval json response object on success, NULL on failure
 */
//json_t * hostIf_CreateJsonMsgResponse(HOSTIF_MsgData_t *stMsgData);


#endif /* HOSTIFJSONIFHANDLER_H_ */

/* End of IARM_BUS_IARM_CORE_API doxygen group */
/**
 * @}
 */


/** @} */
/** @} */
