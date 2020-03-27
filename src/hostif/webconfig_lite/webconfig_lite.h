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
 * @file webconfig_internal.h
 *
 * @description This header defines the webconfig apis
 *
 */

#ifndef _WEBCONFIG_LITE_H_
#define _WEBCONFIG_LITE_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <waldb.h>
#include "rdk_debug.h"

#define LOG_TR69HOSTIF  "LOG.RDK.TR69HOSTIF"

void * initWebConfigTask(void *);
#endif /* _WEBCONFIG_LITE_H_ */
