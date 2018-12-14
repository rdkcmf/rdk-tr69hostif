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
 * @file Device_WiFi_EndPoint_Profile_Security.cpp
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
#include "Device_WiFi_EndPoint_Profile_Security.h"

GHashTable* hostIf_WiFi_EndPoint_Profile_Security::ifHash = NULL;

hostIf_WiFi_EndPoint_Profile_Security* hostIf_WiFi_EndPoint_Profile_Security::getInstance(int dev_id)
{
    hostIf_WiFi_EndPoint_Profile_Security* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_WiFi_EndPoint_Profile_Security *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_WiFi_EndPoint_Profile_Security(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create hostIf_WiFi_EndPoint_Profile_Security instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}


GList* hostIf_WiFi_EndPoint_Profile_Security::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}


void hostIf_WiFi_EndPoint_Profile_Security::closeInstance(hostIf_WiFi_EndPoint_Profile_Security *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_WiFi_EndPoint_Profile_Security::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_WiFi_EndPoint_Profile_Security* pDev = (hostIf_WiFi_EndPoint_Profile_Security *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}


hostIf_WiFi_EndPoint_Profile_Security::hostIf_WiFi_EndPoint_Profile_Security(int dev_id):
    dev_id(0)
{
    memset(ModeEnabled, 0, 64);
    memset(WEPKey, 0, 64);
    memset(PreSharedKey, 0, 64);
    memset(KeyPassphrase, 0, 64);
}

int hostIf_WiFi_EndPoint_Profile_Security::get_hostIf_WiFi_EndPoint_Profile_Security_ModeEnabled(HOSTIF_MsgData_t *stMsgData )
{

}
int hostIf_WiFi_EndPoint_Profile_Security::get_hostIf_WiFi_EndPoint_Profile_Security_WEPKey(HOSTIF_MsgData_t *stMsgData )
{

}
int hostIf_WiFi_EndPoint_Profile_Security::get_hostIf_WiFi_EndPoint_Profile_Security_PreSharedKey(HOSTIF_MsgData_t *stMsgData )
{

}
int hostIf_WiFi_EndPoint_Profile_Security::get_hostIf_WiFi_EndPoint_Profile_Security_KeyPassphrase(HOSTIF_MsgData_t *stMsgData )
{

}
#endif /* #ifdef USE_WIFI_PROFILE */

