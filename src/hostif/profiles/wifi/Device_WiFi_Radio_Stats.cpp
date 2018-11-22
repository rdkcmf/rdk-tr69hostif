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
 * @file Device_WiFi_Radio_Stats.cpp
 *
 * @brief Device.WiFi.Radio.Stats API Implementation.
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
#include "Device_WiFi_Radio_Stats.h"

extern "C" {
#include "wifiSrvMgrIarmIf.h"
    /* #include "c_only_header.h"*/
};

GHashTable* hostIf_WiFi_Radio_Stats::ifHash = NULL;
static time_t radioFirstExTime = 0;

hostIf_WiFi_Radio_Stats *hostIf_WiFi_Radio_Stats::getInstance(int dev_id)
{
    static hostIf_WiFi_Radio_Stats* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_WiFi_Radio_Stats *)g_hash_table_lookup(ifHash, (gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_WiFi_Radio_Stats(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create hostIf_WiFi_Radio_Stats instance..\n");
        }
        g_hash_table_insert(ifHash,(gpointer)dev_id, pRet);
    }
    return pRet;
}

void hostIf_WiFi_Radio_Stats::closeInstance(hostIf_WiFi_Radio_Stats *pDev)
{
    if(pDev)
    {
//        g_hash_table_remove(devHash, (gconstpointer)pDev->dev_id);
//        if(pDev->ctxt)
        delete pDev;
    }
}

void hostIf_WiFi_Radio_Stats::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_WiFi_Radio_Stats* pDev = (hostIf_WiFi_Radio_Stats *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}


hostIf_WiFi_Radio_Stats::hostIf_WiFi_Radio_Stats(int dev_id):
    dev_id(dev_id),
    BytesSent(0),
    BytesReceived(0),
    PacketsSent(0),
    PacketsReceived(0),
    ErrorsSent(0),
    ErrorsReceived(0),
    DiscardPacketsSent(0),
    DiscardPacketsReceived(0),
    NoiseFloor(0)
{

}

int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_Props_Fields(int radioIndex)
{
    IARM_Result_t retVal = IARM_RESULT_SUCCESS;
    IARM_BUS_WiFi_DiagsPropParam_t param = {0};
    int ret;
    radioIndex=1;

    hostIf_WiFi_Radio_Stats *pDev = hostIf_WiFi_Radio_Stats::getInstance(dev_id);
    if(pDev)
    {
        retVal = IARM_Bus_Call(IARM_BUS_NM_SRV_MGR_NAME, IARM_BUS_WIFI_MGR_API_getRadioStatsProps, (void *)&param, sizeof(param));
        if (IARM_RESULT_SUCCESS != retVal)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] IARM BUS CALL failed with  : %d.\n", __FILE__, __FUNCTION__, retVal);
            return NOK;
        }
        BytesSent = param.data.radio_stats.params.bytesSent;
        BytesReceived = param.data.radio_stats.params.bytesReceived;
        PacketsSent = param.data.radio_stats.params.packetsSent;
        PacketsReceived = param.data.radio_stats.params.packetsReceived;
        ErrorsSent = param.data.radio_stats.params.errorsSent;
        ErrorsReceived = param.data.radio_stats.params.errorsReceived;
        DiscardPacketsSent = param.data.radio_stats.params.discardPacketsSent;
        DiscardPacketsReceived = param.data.radio_stats.params.discardPacketsReceived;
        NoiseFloor = param.data.radio_stats.params.noiseFloor;
        radioFirstExTime = time (NULL);
        return OK;
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s]Error!  Unable to connect to wifi instance\n", __FILE__, __FUNCTION__);
        return NOK;
    }

}

void hostIf_WiFi_Radio_Stats::checkWifiRadioPropsFetch(int radioIndex)
{
    int ret = NOK;
    time_t currExTime = time (NULL);
    if ((currExTime - radioFirstExTime ) > QUERY_INTERVAL)
    {
        ret = get_Device_WiFi_Radio_Stats_Props_Fields(radioIndex);
        if( OK != ret)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to fetch   : %d.\n", __FILE__, __FUNCTION__, ret);
        }
    }
}

int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_BytesSent(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioPropsFetch(radioIndex);
    put_int(stMsgData->paramValue, BytesSent);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;

}


int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_BytesReceived(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioPropsFetch(radioIndex);
    put_int(stMsgData->paramValue, BytesReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_PacketsSent(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioPropsFetch(radioIndex);
    put_int(stMsgData->paramValue, PacketsSent);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_PacketsReceived(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioPropsFetch(radioIndex);
    put_int(stMsgData->paramValue, PacketsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_ErrorsSent(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioPropsFetch(radioIndex);
    put_int(stMsgData->paramValue, ErrorsSent);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_ErrorsReceived(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioPropsFetch(radioIndex);
    put_int(stMsgData->paramValue, ErrorsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_DiscardPacketsSent(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioPropsFetch(radioIndex);
    put_int(stMsgData->paramValue, DiscardPacketsSent);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_DiscardPacketsReceived(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioPropsFetch(radioIndex);
    put_int(stMsgData->paramValue, DiscardPacketsReceived);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}
int hostIf_WiFi_Radio_Stats::get_Device_WiFi_Radio_Stats_NoiseFloor(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioPropsFetch(radioIndex);
    put_int(stMsgData->paramValue, NoiseFloor);
    stMsgData->paramtype = hostIf_IntegerType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}


#endif /* #ifdef USE_WIFI_PROFILE */
