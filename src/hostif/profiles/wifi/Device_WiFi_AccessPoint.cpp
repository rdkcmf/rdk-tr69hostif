/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2018 RDK Management
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
 * @file Device_WiFi_EndPoint_AccessPoint.cpp
 *
 * @brief Device_WiFi_EndPoint_AccessPoint API Implementation.
 *
 * This is the implementation of the WiFi API.
 *
 * @par Document
 */
/** @addtogroup TR-069 WiFi Implementation
 *  This is the implementation of the Device Public API.
 *  @{
 */
#ifdef USE_WIFI_PROFILE
/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/
#include "Device_WiFi_AccessPoint.h"

GHashTable* hostIf_WiFi_AccessPoint::ifHash = NULL;

hostIf_WiFi_AccessPoint* hostIf_WiFi_AccessPoint::getInstance(int dev_id)
{
    hostIf_WiFi_AccessPoint* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_WiFi_AccessPoint *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_WiFi_AccessPoint(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create hostIf_WiFi_AccessPoint instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}


GList* hostIf_WiFi_AccessPoint::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}


void hostIf_WiFi_AccessPoint::closeInstance(hostIf_WiFi_AccessPoint *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_WiFi_AccessPoint::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_WiFi_AccessPoint* pDev = (hostIf_WiFi_AccessPoint *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}
/*
void hostIf_WiFi_AccessPoint::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void hostIf_WiFi_AccessPoint::releaseLock()
{
    g_mutex_unlock(m_mutex);
}*/


hostIf_WiFi_AccessPoint::hostIf_WiFi_AccessPoint(int dev_id):
    dev_id(0),
    Enable(false),
    SSIDAdvertisementEnabled(false),
    RetryLimit(0),
    WMMCapability(false),
    UAPSDCapability(false),
    WMMEnable(false),
    UAPSDEnable(false),
    AssociatedDeviceNumberOfEntries(0)
{

    memset(Status , 0, 64);
    memset (Alias, 0,64);
    memset (SSIDReference, 0, 256);
}



int hostIf_WiFi_AccessPoint::get_hostIf_WiFi_AccessPoint_Enable(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_AccessPoint::get_hostIf_WiFi_AccessPoint_Status(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_AccessPoint::get_hostIf_WiFi_AccessPoint_Alias(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_AccessPoint::get_hostIf_WiFi_AccessPoint_SSIDReference(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_AccessPoint::get_hostIf_WiFi_AccessPoint_SSIDAdvertisementEnabled(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_AccessPoint::get_hostIf_WiFi_AccessPoint_RetryLimit(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_AccessPoint::get_hostIf_WiFi_AccessPoint_WMMCapability(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_AccessPoint::get_hostIf_WiFi_AccessPoint_UAPSDCapability(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_AccessPoint::get_hostIf_WiFi_AccessPoint_WMMEnable(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_AccessPoint::get_hostIf_WiFi_AccessPoint_UAPSDEnable(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_AccessPoint::get_hostIf_WiFi_AccessPoint_AssociatedDeviceNumberOfEntries(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}
#endif /* #ifdef USE_WIFI_PROFILE */
