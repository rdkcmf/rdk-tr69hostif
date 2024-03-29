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
 * @file webconfig_utils.h
 *
 * @description This header defines APIs of webconfig module
 *
 */

#ifndef _WEBCFG_UTILS_H_
#define _WEBCFG_UTILS_H_

#include<stdio.h>
#include<string.h>
#include "rdk_debug.h"
#define LOG_TR69HOSTIF  "LOG.RDK.TR69HOSTIF"
#define ERR 1
#define SUCCESS 0

#define BLE_DETECTION_WEBCFG_ENDPOINT "https://cpe-config.xdp.comcast.net/api/v1/device/{mac}/config?group_id=ble"
#define BLE_DETECTION_WEBCFG_SUFIX "?group_id=ble"
#define DEV_DETAILS_FILE "/tmp/.deviceDetails.cache"

#endif
