/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2020 RDK Management
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
 * @file hostIf_rbus_Dml_Provider.h
 * @brief The header file provides HostIf message handler information APIs.
 */

/**
 *
 * @defgroup TR-HOSTIF_RBUS_DML_PROVIDER Rbus interface
 */


/**
*  HOST-IF is a platform agnostic Inter-process communication (IPC) interface. It allows
*  applications to communicate with each other by sending Events or invoking Remote
*  Procedure Calls.
*  This is rbus interface to provide TR181 data through rbus.
*
*/


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef HOSTIF_RBUS_DML_PROVIDER_H_
#define HOSTIF_RBUS_DML_PROVIDER_H_

void init_rbus_dml_provider();

int setRbusStringParam(char *paramName, char* paramValue);
int getRbusStringParam(char *paramName, char** paramValue);

#endif /* HOSTIF_RBUS_DML_PROVIDER_H_ */

/* End of HOSTIF_RBUS_DML_PROVIDER_H_ API doxygen group */
/**
 * @}
 */


/** @} */
/** @} */
