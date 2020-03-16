/**
 * @file webconfig_internal.h
 *
 * @description This header defines the webconfig apis
 *
 * Copyright (c) 2019  Comcast
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
