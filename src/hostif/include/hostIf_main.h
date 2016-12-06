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
* @file hostIf_main.h
*
* @brief hostIf_main API.
*
* This API defines the core operations for hostIf
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

/** @defgroup IARM_BUS IARM_BUS
*    @ingroup IARM_BUS
*
*  IARM-Bus is a platform agnostic Inter-process communication (IPC) interface. It allows
*  applications to communicate with each other by sending Events or invoking Remote
*  Procedure Calls. The common programming APIs offered by the RDK IARM-Bus interface is
*  independent of the operating system or the underlying IPC mechanism.
*
*  Two applications connected to the same instance of IARM-Bus are able to exchange events
*  or RPC calls. On a typical system, only one instance of IARM-Bus instance is needed. If
*  desired, it is possible to have multiple IARM-Bus instances. However, applications
*  connected to different buses will not be able to communicate with each other.
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


#ifndef HOSTIF_MAIN_H_
#define HOSTIF_MAIN_H_



#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <exception>
#include <glib.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
 #include <sys/time.h>
#include "rdk_debug.h"

extern GMutex *request_handler_mutex;
extern gchar *date_str;

void tr69hostIf_logger (const gchar *log_domain, GLogLevelFlags log_level,const gchar *message, gpointer user_data);

#define G_LOG_DOMAIN    ((gchar*) 0)
#define LOG_TR69HOSTIF  "LOG.RDK.TR69HOSTIF"

using namespace std;

enum {
    OK = 0,
    NOK = -1,
    NOT_HANDLED = -2
};

typedef struct argsList {
    char logFileName[64];
    char confFile[100];
    int httpPort;
} T_ARGLIST;

static volatile sig_atomic_t time_to_quit = 0;

void quit_handler (int sig_received);
void exit_gracefully (int sig_received);

void *tr69IfHandlerThread(void *);
void *jsonIfHandlerThread(void *);
pid_t getTid();


#endif /* HOSTIF_MAIN_H_ */

/* End of IARM_BUS_IARM_CORE_API doxygen group */
/**
 * @}
 */




/** @} */
/** @} */
