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
 * @file Device_Time.c
 *
 * @brief DeviceTime API Implementation.
 *
 * This is the implementation of the DeviceTime API.
 *
 * @par Document
 * TBD Relevant design or API documentation.
 *
 */


/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "Device_Time.h"

#define TIME_ZONE_LENGTH 8

GHashTable* hostIf_Time::ifHash = NULL;
GMutex* hostIf_Time::m_mutex = NULL;
GHashTable* hostIf_Time::m_notifyHash = NULL;

/****************************************************************************************************************************************************/
// Device.DeviceInfo Profile. Getters:
/****************************************************************************************************************************************************/
hostIf_Time::hostIf_Time(int dev_id):
    dev_id(dev_id),
    bCalledLocalTimeZone(false),
    bCalledCurrentLocalTime(false)
{
    strcpy(backupLocalTimeZone,"");
    strcpy(backupCurrentLocalTime,"");
}

hostIf_Time* hostIf_Time::getInstance(int dev_id)
{
    hostIf_Time* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_Time *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_Time(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create MoCA Interface instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_Time::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_Time::closeInstance(hostIf_Time *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_Time::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_Time* pDev = (hostIf_Time *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

void hostIf_Time::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void hostIf_Time::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

GHashTable* hostIf_Time::getNotifyHash()
{
    if(m_notifyHash)
    {
        return m_notifyHash;
    }
    else
    {
        return m_notifyHash = g_hash_table_new(g_str_hash, g_str_equal);
    }
}

hostIf_Time::~hostIf_Time()
{
    if(m_notifyHash)
    {
        g_hash_table_destroy(m_notifyHash);
    }
}

int hostIf_Time::get_Device_Time_LocalTimeZone(HOSTIF_MsgData_t *stMsgData, bool *pChanged )
{
    struct timeval time_now;
    struct tm *newtime = NULL;

    char tmp[TR69HOSTIFMGR_MAX_PARAM_LEN];

    gettimeofday(&time_now, NULL);

    newtime = localtime((const time_t*)&(time_now.tv_sec));

    if(0 == strftime(tmp,sizeof(tmp),"%Z", newtime))
    {
        return NOK;
    }

    if(bCalledLocalTimeZone && pChanged && strncmp(tmp, backupLocalTimeZone,TR69HOSTIFMGR_MAX_PARAM_LEN ))
    {
        *pChanged = true;
    }

    bCalledLocalTimeZone = true;
    strcpy(stMsgData->paramValue,tmp);
    strcpy(backupLocalTimeZone,tmp);

    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    return OK;
}
int hostIf_Time::get_Device_Time_Enable(HOSTIF_MsgData_t *, bool *pChanged )
{
    return NOK;
}
int hostIf_Time::get_Device_Time_Status(HOSTIF_MsgData_t *, bool *pChanged )
{
    return NOK;
}
int hostIf_Time::get_Device_Time_NTPServer1(HOSTIF_MsgData_t *, bool *pChanged )
{
    return NOK;
}
int hostIf_Time::get_Device_Time_NTPServer2(HOSTIF_MsgData_t *, bool *pChanged )
{
    return NOK;
}
int hostIf_Time::get_Device_Time_NTPServer3(HOSTIF_MsgData_t *, bool *pChanged )
{
    return NOK;
}
int hostIf_Time::get_Device_Time_NTPServer4(HOSTIF_MsgData_t *, bool *pChanged )
{
    return NOK;
}
int hostIf_Time::get_Device_Time_NTPServer5(HOSTIF_MsgData_t *, bool *pChanged )
{
    return NOK;
}

int hostIf_Time::get_Device_Time_CurrentLocalTime(HOSTIF_MsgData_t *stMsgData, bool *pChanged )
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [36] = {'\0'};
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FILE__, __FUNCTION__);
    char timeZoneTmp[7];
    memset(timeZoneTmp, 0, sizeof(timeZoneTmp));

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    strftime(buffer, 80,  "%Y-%m-%dT%H:%M:%S", timeinfo);
    strftime(timeZoneTmp, sizeof(timeZoneTmp),  "%z", timeinfo);
    sprintf(buffer + strlen(buffer), ".%0.6d%s", timeinfo->tm_sec, timeZoneTmp);

    if(bCalledCurrentLocalTime && pChanged && strncmp(buffer, backupCurrentLocalTime,TR69HOSTIFMGR_MAX_PARAM_LEN ))
    {
        *pChanged = true;
    }

    bCalledCurrentLocalTime = true;
    strcpy(stMsgData->paramValue, buffer);
    strcpy(backupCurrentLocalTime,buffer);

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] buffer : %s stMsgData->paramValue: %s\n", __FILE__, __FUNCTION__, buffer, stMsgData->paramValue);

    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FILE__, __FUNCTION__);
    return OK;
}

int hostIf_Time::set_Device_Time_Enable(HOSTIF_MsgData_t* stMsgData)
{
    return NOK;
}
int hostIf_Time::set_Device_Time_NTPServer1(HOSTIF_MsgData_t* stMsgData)
{
    return NOK;
}
int hostIf_Time::set_Device_Time_NTPServer2(HOSTIF_MsgData_t* stMsgData)
{
    return NOK;
}
int hostIf_Time::set_Device_Time_NTPServer3(HOSTIF_MsgData_t* stMsgData)
{
    return NOK;
}
int hostIf_Time::set_Device_Time_NTPServer4(HOSTIF_MsgData_t* stMsgData)
{
    return NOK;
}
int hostIf_Time::set_Device_Time_NTPServer5(HOSTIF_MsgData_t* stMsgData)
{
    return NOK;
}
int hostIf_Time::set_Device_Time_LocalTimeZone(HOSTIF_MsgData_t* stMsgData)
{
    return NOK;
}


/** @} */
/** @} */
