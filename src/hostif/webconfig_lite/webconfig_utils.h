#ifndef _WEBCONFIG_UTILS_H_
#define _WEBCONFIG_UTILS_H_

#include<stdio.h>
#include<string.h>
#include "rdk_debug.h"
#define LOG_TR69HOSTIF  "LOG.RDK.TR69HOSTIF"
#define ERR 1
#define SUCCESS 0
int getKeyValue(char *key, char *value);

#define DEV_DETAILS_FILE "/tmp/.deviceDetails.cache"

#endif
