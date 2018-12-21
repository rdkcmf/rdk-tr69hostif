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
 * @file Device_WiFi_EndPoint.cpp
 *
 * @brief Device_WiFi_SSID API Implementation.
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

#include "Device_WiFi_EndPoint.h"
#include <mutex>

extern "C" {
#include "wifiSrvMgrIarmIf.h"
};

GHashTable* hostIf_WiFi_EndPoint::ifHash = NULL;

hostIf_WiFi_EndPoint* hostIf_WiFi_EndPoint::getInstance(int dev_id)
{
    hostIf_WiFi_EndPoint* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_WiFi_EndPoint *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_WiFi_EndPoint(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create hostIf_WiFi_EndPoint instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_WiFi_EndPoint::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_WiFi_EndPoint::closeInstance(hostIf_WiFi_EndPoint *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_WiFi_EndPoint::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_WiFi_EndPoint* pDev = (hostIf_WiFi_EndPoint *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}


hostIf_WiFi_EndPoint::hostIf_WiFi_EndPoint (int dev_id) :
    dev_id (dev_id),
    Enable(false),
    ProfileNumberOfEntries(0)
{
    memset(Status, 0, 64);
    memset(Alias, 0, 64);
    memset(ProfileReference, 0, 256);
    memset(SSIDReference, 0, 256);
    stats.LastDataDownlinkRate = 0;
    stats.LastDataUplinkRate = 0;
    stats.SignalStrength = 0;
    stats.Retransmissions = 0;
}


/**
* @brief Enables/disables this end point.
*/
int hostIf_WiFi_EndPoint::get_Device_WiFi_EndPoint_Enable (HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;
    if (OK != refreshCache ())
        return NOK;
    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Enable = [%d]\n", __FUNCTION__, Enable);
    put_int (stMsgData->paramValue, Enable);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof (bool);
    return OK;
}

int hostIf_WiFi_EndPoint::set_Device_WiFi_EndPoint_Enable (HOSTIF_MsgData_t *stMsgData)
{
    return OK;
}

int hostIf_WiFi_EndPoint::get_Device_WiFi_EndPoint_Status (HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;
    if (OK != refreshCache ())
        return NOK;
    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Status = [%s]\n", __FUNCTION__, Status);
    strcpy (stMsgData->paramValue, Status);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (Status);
    return OK;
}

int hostIf_WiFi_EndPoint::get_Device_WiFi_EndPoint_Alias (HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;
    if (OK != refreshCache ())
        return NOK;
    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Alias = [%s]\n", __FUNCTION__, Alias);
    strcpy (stMsgData->paramValue, Alias);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (Alias);
    return OK;
}

int hostIf_WiFi_EndPoint::set_Device_WiFi_EndPoint_Alias (HOSTIF_MsgData_t *stMsgData)
{
    return OK;
}

int hostIf_WiFi_EndPoint::get_Device_WiFi_EndPoint_ProfileReference (HOSTIF_MsgData_t *stMsgData)
{
    return OK;
}

int hostIf_WiFi_EndPoint::set_Device_WiFi_EndPoint_ProfileReference (HOSTIF_MsgData_t *stMsgData)
{
    return OK;
}

int hostIf_WiFi_EndPoint::get_Device_WiFi_EndPoint_SSIDReference (HOSTIF_MsgData_t *stMsgData)
{
/*
    From the spec:

    The value MUST be the path name of a row in the SSID table. If the
    referenced object is deleted, the parameter value MUST be set to an empty
    string. SSIDReference is determined based on the Profile.{i}.SSID within the
    associated ProfileReference) endpoint profile. SSIDReference MUST be an
    empty string if ProfileReference is an empty string (i.e. only when an
    active profile is assigned can the associated SSID interface be determined).
*/
    // not same as value of SSIDReference returned by netsrvmgr, captured in member SSIDReference

    return OK;
}

/*
 * @brief The number of entries in the Profile table.
 */
int hostIf_WiFi_EndPoint::get_Device_WiFi_EndPoint_ProfileNumberOfEntries (HOSTIF_MsgData_t *stMsgData)
{
    return OK;
}

int hostIf_WiFi_EndPoint::get_Device_WiFi_EndPoint_Stats_LastDataDownlinkRate (HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;
    if (OK != refreshCache ())
        return NOK;
    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Stats.LastDataDownlinkRate = [%lu]\n", __FUNCTION__, stats.LastDataDownlinkRate);
    put_int (stMsgData->paramValue, stats.LastDataDownlinkRate);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = sizeof (unsigned long);
    return OK;
}

int hostIf_WiFi_EndPoint::get_Device_WiFi_EndPoint_Stats_LastDataUplinkRate (HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;
    if (OK != refreshCache ())
        return NOK;
    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Stats.LastDataUplinkRate = [%lu]\n", __FUNCTION__, stats.LastDataUplinkRate);
    put_int (stMsgData->paramValue, stats.LastDataUplinkRate);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = sizeof (unsigned long);
    return OK;
}

int hostIf_WiFi_EndPoint::get_Device_WiFi_EndPoint_Stats_SignalStrength (HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;
    if (OK != refreshCache ())
        return NOK;
    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Stats.SignalStrength = [%d]\n", __FUNCTION__, stats.SignalStrength);
    put_int (stMsgData->paramValue, stats.SignalStrength);
    stMsgData->paramtype = hostIf_IntegerType;
    stMsgData->paramLen = sizeof (int);
    return OK;
}

int hostIf_WiFi_EndPoint::get_Device_WiFi_EndPoint_Stats_Retransmissions (HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;
    if (OK != refreshCache ())
        return NOK;
    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Stats.Retransmissions = [%u]\n", __FUNCTION__, stats.Retransmissions);
    put_int (stMsgData->paramValue, stats.Retransmissions);
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen = sizeof (unsigned long);
    return OK;
}

/**
* @brief Refreshes the cache of Device.WiFi.EndPoint. parameters
*/
int hostIf_WiFi_EndPoint::refreshCache()
{
    LOG_ENTRY_EXIT;

    static time_t time_of_last_successful_query = 0;
    static int last_call_status = NOK;
    static std::mutex m;

    std::lock_guard<std::mutex> lg (m);

    // Using a 1-second cache.
    if ((last_call_status == OK ) && (time (0) <= time_of_last_successful_query + 1))
    {
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Cache not stale. last call status is SUCCESS, Refresh not required.\n", __FUNCTION__);
        return OK;
    }

    IARM_BUS_WiFi_DiagsPropParam_t param;
    IARM_Result_t retVal = IARM_Bus_Call (IARM_BUS_NM_SRV_MGR_NAME, IARM_BUS_WIFI_MGR_API_getEndPointProps, (void *) &param, sizeof(param));
    if (IARM_RESULT_SUCCESS != retVal)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Cache refresh failed. IARM_Bus_Call to netsrvmgr returned [%d]\n", __FUNCTION__, retVal);
        last_call_status = NOK;
        return NOK;
    }

    time_of_last_successful_query = time (0);

    Enable = param.data.endPointInfo.enable;
    strncpy (Status, param.data.endPointInfo.status, BUFF_LENGTH_64);
    strncpy (Alias, param.data.endPointInfo.alias, BUFF_LENGTH_64);
    strncpy (ProfileReference, param.data.endPointInfo.ProfileReference, BUFF_LENGTH_256);
    strncpy (SSIDReference, param.data.endPointInfo.SSIDReference, BUFF_LENGTH_256);
    ProfileNumberOfEntries = param.data.endPointInfo.ProfileNumberOfEntries;
    stats.LastDataDownlinkRate = param.data.endPointInfo.stats.lastDataDownlinkRate;
    stats.LastDataUplinkRate = param.data.endPointInfo.stats.lastDataUplinkRate;
    stats.SignalStrength = param.data.endPointInfo.stats.signalStrength;
    stats.Retransmissions = param.data.endPointInfo.stats.retransmissions;

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Cache refreshed.\n", __FUNCTION__);

    if (false == param.data.endPointInfo.enable) // "Disabled" endpoint
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] EndPoint is disabled\n", __FUNCTION__);
        last_call_status = NOK;
        return OK;
    }
    last_call_status = OK;
    return OK;
}

#endif /* #ifdef USE_WIFI_PROFILE */
