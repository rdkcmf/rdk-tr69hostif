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
 * @file Device_WiFi_EndPoint_AccessPoint_WPS.cpp
 *
 * @brief Device_WiFi_EndPoint_AccessPoint_WPS API Implementation.
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
#include "Device_WiFi_AccessPoint_WPS.h"

GHashTable* hostIf_WiFi_AccessPoint_WPS::ifHash = NULL;

hostIf_WiFi_AccessPoint_WPS* hostIf_WiFi_AccessPoint_WPS::getInstance(int dev_id)
{
    hostIf_WiFi_AccessPoint_WPS* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_WiFi_AccessPoint_WPS *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_WiFi_AccessPoint_WPS(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create hostIf_WiFi_AccessPoint_WPS instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_WiFi_AccessPoint_WPS::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_WiFi_AccessPoint_WPS::closeInstance(hostIf_WiFi_AccessPoint_WPS *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_WiFi_AccessPoint_WPS::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_WiFi_AccessPoint_WPS* pDev = (hostIf_WiFi_AccessPoint_WPS *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

hostIf_WiFi_AccessPoint_WPS::hostIf_WiFi_AccessPoint_WPS(int dev_id):
    dev_id(0),
    Enable(false)
{
    memset(ConfigMethodsSupported, 0, 100);
    memset(ConfigMethodsEnabled, 0, 64);
}

int hostIf_WiFi_AccessPoint_WPS::get_hostIf_WiFi_AccessPoint_WPS_Enable(HOSTIF_MsgData_t *stMsgData )
{
    return 0;
}
int hostIf_WiFi_AccessPoint_WPS::get_hostIf_WiFi_AccessPoint_WPS_ConfigMethodsSupported(HOSTIF_MsgData_t *stMsgData )
{
    return 0;
}

int hostIf_WiFi_AccessPoint_WPS::get_hostIf_WiFi_AccessPoint_WPS_ConfigMethodsEnabled(HOSTIF_MsgData_t *stMsgData )
{
    return 0;
}

#endif /* #ifdef USE_WIFI_PROFILE */


