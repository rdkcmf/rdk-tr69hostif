/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2018 RDK Management
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
 * @file request_handler.h
 *
 * @brief API to handle single multiple and wild card parameters in HTTP JSON Interface
 *
 * Local Interface to handler HTTP JSON requests.
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
 * - BE:       ig-Endian.
 * - cb:       allback function (suffix).
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
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/
#ifndef _REQUEST_HANDLER_H_
#define _REQUEST_HANDLER_H_

#include <wdmp-c.h>

#include "waldb.h"


#define MAX_PARAMETER_LEN 512
#define MAX_PARAMETERNAME_LEN 256

#ifdef __cplusplus
extern "C" {
#endif

res_struct* handleRequest(const char* pcCallerID, req_struct *reqSt);

#ifdef __cplusplus
}
#endif

#endif /* REQUEST_HANDLER_H_ */
