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
 * @file Device_MoCA_Interface.cpp
 *
 * @brief MoCA_Interface API Implementation.
 *
 * This is the implementation of the MoCA_Interface API.
 *
 * @par Document
 * TBD Relevant design or API documentation.
 *
 */

/** @addtogroup MoCA_Interface Implementation
 *  This is the implementation of the Device Public API.
 *  @{
 */
#ifdef USE_WIFI_PROFILE
/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/
#include "Device_WiFi_Radio.h"

extern "C" {
    /* #include "c_only_header.h"*/
#include "wifi_client_hal.h"
#include "wifiSrvMgrIarmIf.h"
};

GHashTable* hostIf_WiFi_Radio::ifHash = NULL;

/*hostIf_WiFi_Radio::hostIf_WiFi_Radio(int dev_id):dev_id(dev_id)
{
}*/

hostIf_WiFi_Radio *hostIf_WiFi_Radio::getInstance(int dev_id)
{
    static hostIf_WiFi_Radio* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_WiFi_Radio *)g_hash_table_lookup(ifHash, (gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_WiFi_Radio(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create hostIf_WiFi_Radio instance..\n");
        }
        g_hash_table_insert(ifHash,(gpointer)dev_id, pRet);
    }

    return pRet;
}

/*void* hostIf_WiFi_Radio::getContext()
{
    return ctxt;
}*/



GList* hostIf_WiFi_Radio::getAllAssociateDevs()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_WiFi_Radio::closeInstance(hostIf_WiFi_Radio *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
//        if(pDev->ctxt)
        delete pDev;
    }
}

void hostIf_WiFi_Radio::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_WiFi_Radio* pDev = (hostIf_WiFi_Radio *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

hostIf_WiFi_Radio::hostIf_WiFi_Radio(int dev_id):
    dev_id(dev_id),
    Enable(false),
    radioFirstExTime(0),
    LastChange(0),
    Upstream (false),
    MaxBitRate(0),
    Channel(0),
    AutoChannelSupported(false),
    AutoChannelEnable(false),
    AutoChannelRefreshPeriod (0),
    mcs (0),
    IEEE80211hSupported (false),
    IEEE80211hEnabled (false),
    TransmitPower (0)
{
    memset(Status, 0, sizeof(Status));
    memset(Alias,0,sizeof(Alias));
    memset(Name,0,sizeof(Name));
    memset(LowerLayers,0,sizeof(LowerLayers));
    memset(SupportedFrequencyBands,0,sizeof(SupportedFrequencyBands));
    memset(OperatingFrequencyBand,0,sizeof(OperatingFrequencyBand));
    memset(SupportedStandards, 0, sizeof(SupportedStandards));
    memset(OperatingStandards, 0, sizeof(OperatingStandards));
    memset(PossibleChannels, 0, sizeof(PossibleChannels));
    memset(ChannelsInUse, 0, sizeof(ChannelsInUse));
    memset(OperatingChannelBandwidth, 0, sizeof(OperatingChannelBandwidth));
    memset(ExtensionChannel, 0, sizeof(ExtensionChannel));
    memset(GuardInterval, 0,sizeof(GuardInterval));
    memset(TransmitPowerSupported, 0, sizeof(TransmitPowerSupported));
    memset(RegulatoryDomain, 0, sizeof(RegulatoryDomain));
}


int hostIf_WiFi_Radio::get_Device_WiFi_Radio_Props_Fields(int radioIndex)
{
    IARM_Result_t retVal = IARM_RESULT_SUCCESS;
    IARM_BUS_WiFi_DiagsPropParam_t param = {0};
    int ret;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    hostIf_WiFi_Radio *pDev = hostIf_WiFi_Radio::getInstance(dev_id);
    if(pDev)
    {
        retVal = IARM_Bus_Call(IARM_BUS_NM_SRV_MGR_NAME, IARM_BUS_WIFI_MGR_API_getRadioProps, (void *)&param, sizeof(param));
        if (IARM_RESULT_SUCCESS != retVal)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] IARM BUS CALL failed with  : %d.\n", __FILE__, __FUNCTION__, retVal);
            return NOK;
        }
        Enable                      =   param.data.radio.params.enable;
        snprintf(Status,BUFF_LENGTH_64,param.data.radio.params.status);
        snprintf(Alias,BUFF_LENGTH_64,param.data.radio.params.alias);
        snprintf(Name,BUFF_LENGTH_64,param.data.radio.params.name);
        LastChange                  =   param.data.radio.params.lastChange;
        snprintf(LowerLayers,BUFF_LENGTH_1024,param.data.radio.params.lowerLayers);
        Upstream                    =   param.data.radio.params.upstream;
        MaxBitRate                  =   param.data.radio.params.maxBitRate;
        snprintf(SupportedFrequencyBands,BUFF_LENGTH_256,param.data.radio.params.supportedFrequencyBands);
        snprintf(OperatingFrequencyBand,BUFF_LENGTH_64,param.data.radio.params.operatingFrequencyBand);
        snprintf(SupportedStandards,BUFF_LENGTH_64,param.data.radio.params.supportedStandards);
        snprintf(OperatingStandards,BUFF_LENGTH_64,param.data.radio.params.operatingStandards);
        snprintf(PossibleChannels,BUFF_LENGTH_64,param.data.radio.params.possibleChannels);
        snprintf(ChannelsInUse,BUFF_LENGTH_1024,param.data.radio.params.channelsInUse);
        Channel                     =   param.data.radio.params.channel;
        AutoChannelSupported        =   param.data.radio.params.autoChannelSupported;
        AutoChannelEnable           =   param.data.radio.params.autoChannelEnable;
        AutoChannelRefreshPeriod    =   param.data.radio.params.autoChannelRefreshPeriod;
        snprintf(OperatingChannelBandwidth,BUFF_LENGTH_1024,param.data.radio.params.operatingChannelBandwidth);
        snprintf(ExtensionChannel,BUFF_LENGTH_64,param.data.radio.params.extensionChannel);
        snprintf(GuardInterval,BUFF_LENGTH_64,param.data.radio.params.guardInterval);
        mcs                         =   param.data.radio.params.mcs;
        snprintf(TransmitPowerSupported,BUFF_LENGTH_64,param.data.radio.params.transmitPowerSupported);
        TransmitPower = param.data.radio.params.transmitPower;
//      IEEE80211hSupported         =   param.data.radio.params.IEEE80211hSupported;
//      IEEE80211hEnabled           =   param.data.radio.params.IEEE80211hEnabled;
        snprintf(RegulatoryDomain,BUFF_MIN_16,param.data.radio.params.regulatoryDomain);
        radioFirstExTime = time (NULL);
        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
        return OK;
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s]Error!  Unable to connect to wifi instance\n", __FILE__, __FUNCTION__);
        return NOK;
    }
}

void hostIf_WiFi_Radio::checkWifiRadioFetch(int radioIndex)
{
    int retVal=NOK;
    time_t currExTime = time (NULL);
    if((currExTime - radioFirstExTime ) > QUERY_INTERVAL)
    {
        retVal = get_Device_WiFi_Radio_Props_Fields(radioIndex);
        if( OK != retVal)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to fetch   : %d.\n", __FILE__, __FUNCTION__, retVal);
        }
    }
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_Enable(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    put_boolean(stMsgData->paramValue, Enable);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=1;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_Upstream(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    put_boolean(stMsgData->paramValue, Upstream);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=1;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_AutoChannelSupported(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{


    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    put_boolean(stMsgData->paramValue, AutoChannelSupported);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=1;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_AutoChannelEnable(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{


    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    put_boolean(stMsgData->paramValue, AutoChannelEnable);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=1;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_IEEE80211hSupported(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{


    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    put_boolean(stMsgData->paramValue, IEEE80211hSupported);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=1;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_IEEE80211hEnabled(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{


    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    put_boolean(stMsgData->paramValue, IEEE80211hEnabled);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=1;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_LastChange(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    put_int(stMsgData->paramValue, LastChange);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_MaxBitRate(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    put_int(stMsgData->paramValue, MaxBitRate);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_Channel(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    put_int(stMsgData->paramValue, Channel);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_AutoChannelRefreshPeriod(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    put_int(stMsgData->paramValue, AutoChannelRefreshPeriod);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_MCS(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    put_int(stMsgData->paramValue, mcs);
    stMsgData->paramtype = hostIf_IntegerType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_TransmitPower(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    put_int(stMsgData->paramValue, TransmitPower);
    stMsgData->paramtype = hostIf_IntegerType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_Status(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    strncpy(stMsgData->paramValue, Status,TR69HOSTIFMGR_MAX_PARAM_LEN);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(Status);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_Alias(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    strncpy(stMsgData->paramValue, Alias,TR69HOSTIFMGR_MAX_PARAM_LEN);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(Alias);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_Name(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    strncpy(stMsgData->paramValue, Name,TR69HOSTIFMGR_MAX_PARAM_LEN);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(Name);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_SupportedFrequencyBands(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    strncpy(stMsgData->paramValue, SupportedFrequencyBands,TR69HOSTIFMGR_MAX_PARAM_LEN);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(SupportedFrequencyBands);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_OperatingFrequencyBand(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    strncpy(stMsgData->paramValue, OperatingFrequencyBand,TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(OperatingFrequencyBand);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_SupportedStandards(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    strncpy(stMsgData->paramValue, SupportedStandards,TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(SupportedStandards);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_OperatingStandards(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    strncpy(stMsgData->paramValue, OperatingStandards,TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(OperatingStandards);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_PossibleChannels(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    strncpy(stMsgData->paramValue, PossibleChannels,TR69HOSTIFMGR_MAX_PARAM_LEN);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(PossibleChannels);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_ChannelsInUse(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    strncpy(stMsgData->paramValue, ChannelsInUse,TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(ChannelsInUse);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_OperatingChannelBandwidth(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    strncpy(stMsgData->paramValue, OperatingChannelBandwidth,TR69HOSTIFMGR_MAX_PARAM_LEN);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(OperatingChannelBandwidth);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_ExtensionChannel(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    strncpy(stMsgData->paramValue, ExtensionChannel,TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(ExtensionChannel);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_GuardInterval(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    strncpy(stMsgData->paramValue, GuardInterval,TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(GuardInterval);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_LowerLayers(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    strncpy(stMsgData->paramValue, LowerLayers,TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(LowerLayers);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_TransmitPowerSupported(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    strncpy(stMsgData->paramValue, TransmitPowerSupported,TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(TransmitPowerSupported);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

int hostIf_WiFi_Radio::get_Device_WiFi_Radio_RegulatoryDomain(HOSTIF_MsgData_t *stMsgData,int radioIndex )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    checkWifiRadioFetch(radioIndex);
    strncpy(stMsgData->paramValue, RegulatoryDomain,TR69HOSTIFMGR_MAX_PARAM_LEN);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(RegulatoryDomain);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}


#endif /* #ifdef USE_WIFI_PROFILE */
