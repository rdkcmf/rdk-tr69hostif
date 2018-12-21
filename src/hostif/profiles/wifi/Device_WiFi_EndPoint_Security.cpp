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
 * @file Device_WiFi_EndPoint_Stats.cpp
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

/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/
#ifdef USE_WIFI_PROFILE
#include "Device_WiFi_EndPoint_Security.h"

extern "C" {
#include "wifiSrvMgrIarmIf.h"
};

GHashTable* hostIf_WiFi_EndPoint_Security::ifHash = NULL;

hostIf_WiFi_EndPoint_Security* hostIf_WiFi_EndPoint_Security::getInstance(int dev_id)
{
    hostIf_WiFi_EndPoint_Security* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_WiFi_EndPoint_Security *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_WiFi_EndPoint_Security(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create hostIf_WiFi_EndPoint_Security instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}


GList* hostIf_WiFi_EndPoint_Security::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}


void hostIf_WiFi_EndPoint_Security::closeInstance(hostIf_WiFi_EndPoint_Security *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_WiFi_EndPoint_Security::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_WiFi_EndPoint_Security* pDev = (hostIf_WiFi_EndPoint_Security *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}


hostIf_WiFi_EndPoint_Security::hostIf_WiFi_EndPoint_Security(int dev_id)
{
	memset(ModesSupported, 0, 64);
}

int hostIf_WiFi_EndPoint_Security::get_hostIf_WiFi_EndPoint_Security_ModesEnabled(HOSTIF_MsgData_t *stMsgData )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering.. \n", __FUNCTION__, __FILE__);

    IARM_Result_t retVal = IARM_RESULT_IPCCORE_FAIL;
    IARM_Bus_WiFiSrvMgr_Param_t param;

    if(NULL == stMsgData)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Input arg stMsgData  is NULL\n", __FILE__, __FUNCTION__);
        return retVal;
    }

    memset(&param, 0, sizeof(param));
    retVal = IARM_Bus_Call(IARM_BUS_NM_SRV_MGR_NAME, IARM_BUS_WIFI_MGR_API_getPairedSSIDInfo, (void *)&param, sizeof(param));

    if(retVal != IARM_RESULT_SUCCESS) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] IARM BUS CALL failed with  : %d.\n", __FILE__, __FUNCTION__, retVal);
        return retVal;
    }else{
        strncpy(stMsgData->paramValue,param.data.getPairedSSIDInfo.security,sizeof(stMsgData->paramValue));
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        retVal = IARM_RESULT_SUCCESS;
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] WiFi Security Mode : %s\n",__FUNCTION__,stMsgData->paramValue);
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retVal;
}
int hostIf_WiFi_EndPoint_Security::get_hostIf_WiFi_EndPoint_ModesSupported(HOSTIF_MsgData_t *stMsgData )
{

}

#endif /* #ifdef USE_WIFI_PROFILE */


