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


#include "string.h"
#include "stdio.h"
#include <stddef.h>
#include <stdlib.h>
#include <sstream>
#include "hostIf_utils.h"

#if defined (RDK_DEVICE_CISCO_XI4) || defined (RDK_DEVICE_EMU)
#define INTERFACE_ETH          "eth0"
#else
#define INTERFACE_ETH          "eth1"
#endif

static bool gAcsConnStatus = false;
static bool gGatewayConnStatus = false;
#ifndef NEW_HTTP_SERVER_DISABLE
static bool legacyRFC = false;
#endif

EntryExitLogger::EntryExitLogger (const char* func, const char* file) :
        func (func), file (file)
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entry: %s [%s]\n", func, file);
}

EntryExitLogger::~EntryExitLogger ()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Exit: %s [%s]\n", func, file);
}

using namespace std;

const char * getStringFromEnum( EnumStringMapper stbMapperArray[], int stbMapperArraySize, int enumCode )
{
    register int i = 0;
    for ( i = 0; i != stbMapperArraySize; i++ )
    {
        if ( stbMapperArray[i].enumCode == enumCode )
        {
            return stbMapperArray[i].enumString;
        }
    }

    return NULL;
}

int getEnumFromString( EnumStringMapper stbMapperArray[], int stbMapperArraySize, const char * inputStr )
{
    register int i = 0;
    for ( i = 0; i != stbMapperArraySize; i++ )
    {
        if (!strncasecmp(stbMapperArray[i].enumString, inputStr, strlen(inputStr)))
        {
            return stbMapperArray[i].enumCode;
        }
    }

    return -1;
}

#define MAX_NUM_LEN 10
bool matchComponent(const char* pParam, const char *pKey, const char **pSetting, int &instanceNo)
{
    bool ret = false;
    int str_len = strlen(pKey);
    ret = (strncasecmp(pParam,pKey,str_len)==0?true:false);

    if(ret)
    {
        const char *tmp_ptr;
        int tmp_len;
        if((pParam[str_len] == '.') &&
                (tmp_ptr = strchr(pParam+str_len+1,'.')) &&
                (tmp_len = tmp_ptr - (pParam + str_len + 1)) < MAX_NUM_LEN)
        {
            char tmp_buff[MAX_NUM_LEN];
            memset(tmp_buff,0,MAX_NUM_LEN);
            strncpy(tmp_buff,pParam+str_len+1,tmp_len);
            instanceNo = atoi(tmp_buff);
            *pSetting = (tmp_ptr + 1);
        }
        else
            ret = false;
    }

    return ret;

}

std::string int_to_string(int d)
{
    char tmp_buff[10];
    memset(tmp_buff,0,10);
    sprintf(tmp_buff,"%d",d);
    return std::string(tmp_buff);
}

std::string uint_to_string(uint d)
{
    std::stringstream ss;
    ss << d;
    return ss.str();
}

std::string ulong_to_string(unsigned long d)
{
    std::stringstream ss;
    ss << d;
    return ss.str();
}

int get_int(const char* ptr)
{
    int *ret = (int *)ptr;
    return *ret;
}

void put_int(char *ptr, int val)
{
    int *tmp = (int *)ptr;
    *tmp = val;
}

uint get_uint(char *ptr)
{
    uint *ret = (uint *)ptr;
    return *ret;
}

void put_uint(char *ptr, uint val)
{
    uint *tmp = (uint *)ptr;
    *tmp = val;
}



int get_ulong(const char* ptr)
{
	unsigned long *ret = (unsigned long *)ptr;
    return *ret;
}

void put_ulong(char *ptr, unsigned long val)
{
	unsigned long *tmp = (unsigned long *)ptr;
    *tmp = val;
}


bool get_boolean(const char *ptr)
{
    bool *ret = (bool *)ptr;
    return *ret;
}

void put_boolean(char *ptr, bool val)
{
    bool *tmp = (bool *)ptr;
    *tmp = val;
}

std::string bool_to_string(bool value)
{
    if (value == true) {
        return "true";
    } else if (value == false) {
        return "false";
    }
    return "";
}

int string_to_int(const char *value)
{
    char *end;
    long ret = strtol(value, &end, 10);
    return (int)ret;
}

uint string_to_uint(const char *value)
{
    char *end;
    unsigned long ret = strtoul(value, &end, 10);
    return (uint)ret;
}

unsigned long string_to_ulong(const char *value)
{
    char *end;
    unsigned long ret = strtoul(value, &end, 10);
    return ret;
}

bool string_to_bool(const char *value)
{
    bool ret = (strcmp(value, "true") == 0) ? true : false;
    return ret;
}

std::string getStringValue(HOSTIF_MsgData_t *stMsgData)
{
    switch (stMsgData->paramtype) {
    case hostIf_StringType:
        return string(stMsgData->paramValue);
    case hostIf_IntegerType:
        return int_to_string(get_int(stMsgData->paramValue));
    case hostIf_UnsignedIntType:
        return uint_to_string(get_uint(stMsgData->paramValue));
    case hostIf_BooleanType:
        return bool_to_string(get_boolean(stMsgData->paramValue));
    case hostIf_UnsignedLongType:
        return ulong_to_string(get_ulong(stMsgData->paramValue));
    case hostIf_DateTimeType:
        // we don't handle this one yet
    default:
        return "";
    }
}

void putValue(HOSTIF_MsgData_t *stMsgData, const string &value)
{
    // std::cout << "value ot be inserted is : " << value << std::endl;
    memset(stMsgData->paramValue, 0, TR69HOSTIFMGR_MAX_PARAM_LEN);

    switch (stMsgData->paramtype) {
    case hostIf_StringType:
        strcpy(stMsgData->paramValue, value.c_str());
        stMsgData->paramLen = strlen(value.c_str());
        break;
    case hostIf_IntegerType:
        put_int(stMsgData->paramValue, string_to_int(value.c_str()));
        break;
    case hostIf_UnsignedIntType:
        put_uint(stMsgData->paramValue, string_to_uint(value.c_str()));
        break;
    case hostIf_BooleanType:
        put_boolean(stMsgData->paramValue, string_to_bool(value.c_str()));
        break;
    case hostIf_UnsignedLongType:
        put_ulong(stMsgData->paramValue, string_to_ulong(value.c_str()));
        break;
    case hostIf_DateTimeType:
        // we don't handle this one yet
    default:
        break;
    }
}

void setResetState( eSTBResetState rebootFlag)
{
    gResetState = rebootFlag;
}

eSTBResetState getResetState( void )
{
    return gResetState;
}

void triggerResetScript()
{
    int ret = -1;
    char scriptbuff[100] = {'\0'};

    switch (gResetState) {
    case ColdReset:
        /* Excute Cold Factory Reset script */
        sprintf(scriptbuff,"%s %s/%s", "sh", SCR_PATH, "coldfactory-reset.sh");
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Executing : %s \n",__FUNCTION__, scriptbuff);

        /*System command */
        ret = system(scriptbuff);
        if (WEXITSTATUS(ret) != 0 )
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed to execute : %s. \n",__FUNCTION__, scriptbuff);
        }
        else {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Successfully executed %s Reset. \n",__FUNCTION__, scriptbuff);
            system(REBOOT_SCR);
        }
        break;

    case FactoryReset:
        /* Excute Factory Reset script */
        sprintf(scriptbuff,"%s %s/%s", "sh", SCR_PATH, "factory-reset.sh");

        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Executing : %s \n",__FUNCTION__, scriptbuff);

        /*System command */
        ret = system(scriptbuff);
        if (WEXITSTATUS(ret) != 0 )
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed to execute : %s. \n",__FUNCTION__, scriptbuff);
        }
        else {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Successfully executed %s Reset. \n",__FUNCTION__, scriptbuff);
            system(REBOOT_SCR);
        }
        break;
    case WarehouseReset:
        /* Excute Warehouse Reset script */
        sprintf(scriptbuff,"%s %s/%s", "sh", SCR_PATH, "warehouse-reset.sh");
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Executing : %s \n",__FUNCTION__, scriptbuff);

        /*System command */
        ret = system(scriptbuff);
        if (WEXITSTATUS(ret) != 0 )
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed to execute: %s. \n",__FUNCTION__, scriptbuff);
        }
        else {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Successfully executed %s Reset. \n",__FUNCTION__, scriptbuff);
            system(REBOOT_SCR);
        }
        break;
    case CustomerReset:
        /* Excute Customer Reset script */
        sprintf(scriptbuff,"%s %s/%s", "sh", SCR_PATH, "customer-reset.sh");
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Executing : %s \n",__FUNCTION__, scriptbuff);

        /*System command */
        ret = system(scriptbuff);
        if (WEXITSTATUS(ret) != 0 )
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed to execute: %s. \n",__FUNCTION__, scriptbuff);
        }
        else {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Successfully executed %s Reset. \n",__FUNCTION__, scriptbuff);
            system(REBOOT_SCR);
        }
        break;
    default:
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Invalid input for reset \n",__FUNCTION__);
        break;
    }

}


void set_ACSStatus( bool enabled)
{
    gAcsConnStatus = enabled;
}

bool get_ACSStatus()
{
    return gAcsConnStatus;
}

void set_GatewayConnStatus( bool enabled)
{
    gGatewayConnStatus = enabled;
}

bool get_GatewayConnStatus()
{
    return gGatewayConnStatus;
}

/**
 * Returns:
 * the specified environment variable's value if it is not NULL.
 * the specified default value otherwise.
 */
char* getenvOrDefault (const char* name, char* defaultValue)
{
    char* value = getenv (name);
    return value ? value : defaultValue;
}

int read_command_output (char* cmd, char* resultBuff, int length)
{
    FILE* fp = popen (cmd, "r");
    if (fp == NULL)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s]: cannot run command [%s]\n", __FUNCTION__, cmd);
        return NOK;
    }
    if (fgets (resultBuff, length, fp) == NULL)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s]: cannot read output from command [%s]\n", __FUNCTION__, cmd);
        pclose (fp);
        return NOK;
    }
    pclose (fp);

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s]: command [%s] returned [%s]\n", __FUNCTION__, cmd, resultBuff);

    return OK;
}

/**
 * @brief This function reads the complete output(upto 1k bytes) as string and return to caller.
 * It returns output with new line character \n if the console output has new line.
 */
int GetStdoutFromCommand(char *cmd, string &consoleString)
{
    FILE * stream;
    char buffer[BUFF_LENGTH_1024];

    memset(buffer, 0, sizeof(buffer));
    consoleString.clear();
    stream = popen(cmd, "r");
    if (stream == NULL) {
        return -1;
    }
    else
    {
        while (!feof(stream))
        {
            if (fgets(buffer, BUFF_LENGTH_1024, stream) != NULL)
            {
                consoleString.append(buffer);
            }
        }
        pclose(stream);
    }
    return 0;
}

#ifndef NEW_HTTP_SERVER_DISABLE
/**
 * @brief Get Current time
 *
 * @param[in] Time spec timer
 */
void getCurrentTime(struct timespec *timer)
{
    clock_gettime(CLOCK_REALTIME, timer);
}


long timeValDiff(struct timespec *starttime, struct timespec *finishtime)
{
    long msec;
    msec=(finishtime->tv_sec-starttime->tv_sec)*1000;
    msec+=(finishtime->tv_nsec-starttime->tv_nsec)/1000000;
    return msec;
}

void setLegacyRFCEnabled(bool value)
{
    legacyRFC = value;
}

bool legacyRFCEnabled()
{
    return legacyRFC;
}
#endif

HostIf_Source_Type_t getBSUpdateEnum(const char *bsUpdate)
{
    if (!bsUpdate)
        return HOSTIF_NONE;

    if (strcasecmp(bsUpdate, "allUpdate") == 0)
        return HOSTIF_SRC_ALL;
    else if (strcasecmp(bsUpdate, "rfcUpdate") == 0)
        return HOSTIF_SRC_RFC;
    else if (strcasecmp(bsUpdate, "default") == 0)
        return HOSTIF_SRC_DEFAULT;
    return HOSTIF_NONE;
}

/** @} */
/** @} */
