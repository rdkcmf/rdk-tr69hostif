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
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef HOSTIF_UTILS_H_
#define HOSTIF_UTILS_H_


#include <string>
#include "hostIf_main.h"

#define REBOOT_SCR "sh /rebootNow.sh"
#define SCR_PATH "/lib/rdk"
#define NTP_FILE_NAME "/opt/persistent/firstNtpTime"
#define FW_DWN_FILE_PATH "/opt/fwdnldstatus.txt"
#define TEMP_FW_DWN_FILE_PATH "/opt/fwdnldstatus_tmp.txt"

#define BUFF_MAC 18
#define BUFF_MIN_16 16
#define BUFF_LENGTH_16 16
#define BUFF_LENGTH_32 32
#define BUFF_LENGTH_64 64
#define BUFF_LENGTH_128 128
#define BUFF_LENGTH_256 256
#define BUFF_LENGTH_1024 1024
#define BUFF_LENGTH BUFF_LENGTH_1024

static const char* NOT_IMPLEMENTED = "Not Implemented";

static const char* STATE_UP = "Up";
static const char* STATE_DOWN = "Down";

static const char* TIME_UNKNOWN = "0001-01-01T00:00:00Z";
static const char* TIME_INFINITY = "9999-12-31T23:59:59Z";

typedef enum __eSTBResetState
{
    NoReset = 0,
    ColdReset,
    FactoryReset,
    WarehouseReset,
    CustomerReset
} eSTBResetState;

typedef struct
{
    int		enumCode;
    const char	*enumString;
} EnumStringMapper;

class EntryExitLogger
{
    const char* func;
    const char* file;
public:
    EntryExitLogger (const char* func, const char* file);
    ~EntryExitLogger ();
};

#define LOG_ENTRY_EXIT EntryExitLogger logger (__FUNCTION__, __FILE__)

static volatile eSTBResetState gResetState = NoReset;

void setResetState( eSTBResetState );

eSTBResetState getResetState( void );

void triggerResetScript(void);

const char * getStringFromEnum( EnumStringMapper *, int, int );

int getEnumFromString( EnumStringMapper *, int, const char * );

bool matchComponent(const char* pParam, const char *pKey, const char **pSetting, int &instanceNo);

int get_int(const char *ptr);

void put_int(char *ptr, int val);

bool get_boolean(const char *ptr);

void put_boolean(char *ptr, bool val);

int get_ulong(const char* ptr);

void put_ulong(char *ptr, unsigned long val);

std::string int_to_string(int d);

/*
 *  Set the flag for ACS connection status
 */
void set_ACSStatus(bool enabled);

/*
 *  Get the flag for ACS connection status
 */
bool get_ACSStatus();

bool set_GatewayConnStatus();
bool get_GatewayConnStatus();

/* 
 * Get the configured Moca interface name
 */
char *get_Eth_If_Name( void );

int read_command_output (char* cmd, char* resultBuff, int length);

#endif /* HOSTIF_UTILS_H_*/


/** @} */
/** @} */
