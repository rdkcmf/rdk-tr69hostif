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
#include "hostIf_utils.h"

#if defined (RDK_DEVICE_CISCO_XI4) || defined (RDK_DEVICE_EMU)
#define INTERFACE_ETH          "eth0"
#else
#define INTERFACE_ETH          "eth1"
#endif


static bool gAcsConnStatus = false;
static bool gGatewayConnStatus = false;

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

char *get_Eth_If_Name( void )
{
    static char *ethIf = NULL;
    if ( NULL == ethIf )
    {
        ethIf = getenv( "MOCA_INTERFACE" );
        if ( NULL == ethIf )
        {
            ethIf = INTERFACE_ETH;
        }
    }

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"get_Eth_If_Name(): ethIf=%s\n", ethIf);
    return ethIf;
}
/** @} */
/** @} */

int read_command_output (char* cmd, char* resultBuff, int length)
{
    FILE* fp = popen (cmd, "r");
    if (fp == NULL)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s(): cannot run command '%s'\n", __FUNCTION__, cmd);
        return NOK;
    }
    if (fgets (resultBuff, length, fp) == NULL)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s(): cannot read output from command '%s'\n", __FUNCTION__, cmd);
        pclose (fp);
        return NOK;
    }
    pclose (fp);
    return OK;
}
