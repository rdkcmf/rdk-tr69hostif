/*
* ============================================================================
* RDK MANAGEMENT, LLC CONFIDENTIAL AND PROPRIETARY
* ============================================================================
* This file (and its contents) are the intellectual property of RDK Management, LLC.
* It may not be used, copied, distributed or otherwise  disclosed in whole or in
* part without the express written permission of RDK Management, LLC.
* ============================================================================
* Copyright (c) 2016 RDK Management, LLC. All rights reserved.
* ============================================================================
*/

/**
 * @file Device_WiFi_EndPoint_Profile.cpp
 *
 * @brief Device_WiFi_EndPoint_Stats API Implementation.
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
#include "Device_WiFi_EndPoint_Profile.h"

GHashTable* hostIf_WiFi_EndPoint_Profile::ifHash = NULL;

hostIf_WiFi_EndPoint_Profile* hostIf_WiFi_EndPoint_Profile::getInstance(int dev_id)
{
    hostIf_WiFi_EndPoint_Profile* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_WiFi_EndPoint_Profile *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_WiFi_EndPoint_Profile(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create hostIf_WiFi_EndPoint_Profile instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_WiFi_EndPoint_Profile::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_WiFi_EndPoint_Profile::closeInstance(hostIf_WiFi_EndPoint_Profile *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_WiFi_EndPoint_Profile::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_WiFi_EndPoint_Profile* pDev = (hostIf_WiFi_EndPoint_Profile *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}
/*

void hostIf_WiFi_EndPoint_Profile::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void hostIf_WiFi_EndPoint_Profile::releaseLock()
{
    g_mutex_unlock(m_mutex);
}
*/


hostIf_WiFi_EndPoint_Profile::hostIf_WiFi_EndPoint_Profile(int dev_id):
    Enable(0)
{
    memset(Status, 0, 64);
    memset(Alias, 0, 64);
    memset(SSID, 0,32);
    memset(Location, 0, 256);
    memset(Priority, 0, 256);
}

int hostIf_WiFi_EndPoint_Profile::get_hostIf_WiFi_EndPoint_Profile_Enable(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_EndPoint_Profile::get_hostIf_WiFi_EndPoint_Profile_Status(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_EndPoint_Profile::get_hostIf_WiFi_EndPoint_Profile_Alias(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_EndPoint_Profile::get_hostIf_WiFi_EndPoint_Profile_SSID(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_EndPoint_Profile::get_hostIf_WiFi_EndPoint_Profile_Location(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

int hostIf_WiFi_EndPoint_Profile::get_hostIf_WiFi_EndPoint_Profile_Priority(HOSTIF_MsgData_t *stMsgData )
{
    return OK;
}

#endif /* #ifdef USE_WIFI_PROFILE */
