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
 * @file webpa_notification.cpp
 *
 * @description This file describes the Webpa Parameter Request Handling
 *
 */

/*
 * Notification Structure to be filled
 */



#include "webpa_adapter.h"
/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
/**
 * @brief Initializes WebPA configuration file
 *
 * @return void.
 */
void setNotifyConfigurationFile(const char* nofityConfigFile);

/**
 * @brief Initializes WebPA configuration file parameters
 *
 * @return void.
 */
int getnotifyparamList(char ***notifyParamList,int *ptrnotifyListSize);

/**
 * @brief Gets WebPA notification source
 *
 * @return void.
 */
char * getNotifySource();
