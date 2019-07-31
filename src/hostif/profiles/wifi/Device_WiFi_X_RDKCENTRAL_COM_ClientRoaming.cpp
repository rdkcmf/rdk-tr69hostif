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
 * @file Device_WiFi_X_RDKCENTRAL_COM_ClientRoaming.cpp
 *
 * @brief Device_WiFi_X_RDKCENTRAL_COM_ClientRoaming implementation
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
#include "Device_WiFi_X_RDKCENTRAL_COM_ClientRoaming.h"

extern "C" {
#include "wifiSrvMgrIarmIf.h"
    /* #include "c_only_header.h"*/
};

#define POST_ASSN_MAX_TIME_FRAME 36000     /* Max time frame in seconds for roaming window - 10 Hours */
#define MAX_POST_ASSN_BACKOFF 3600 /* Max BackOff Factor for PostAssociation BackOff */

static int get_Device_WiFi_Client_Roaming_Configs();

GHashTable* hostIf_WiFi_Xrdk_ClientRoaming::ifHash = NULL;
time_t radioFirstExTime = 0;
bool isParamSet =  false;

hostIf_WiFi_Xrdk_ClientRoaming* hostIf_WiFi_Xrdk_ClientRoaming::getInstance(int dev_id)
{
    hostIf_WiFi_Xrdk_ClientRoaming* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_WiFi_Xrdk_ClientRoaming *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_WiFi_Xrdk_ClientRoaming(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create hostIf_WiFi_Xrdk_ClientRoaming instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}


void hostIf_WiFi_Xrdk_ClientRoaming::closeInstance(hostIf_WiFi_Xrdk_ClientRoaming *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_WiFi_Xrdk_ClientRoaming::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_WiFi_Xrdk_ClientRoaming* pDev = (hostIf_WiFi_Xrdk_ClientRoaming *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}


hostIf_WiFi_Xrdk_ClientRoaming::hostIf_WiFi_Xrdk_ClientRoaming(int dev_id):
    dev_id(dev_id),
    roamingEnable(0),
    preassnBestThreshold(-67),
    preassnBestDelta(2),
    postAssnLevelDeltaConnected(12),
    postAssnLevelDeltaDisconnected(8),
    postAssnSelfSteerThreshold(-75),
    postAssnSelfSteerTimeframe(60),
    //postAssnSelfSteerBeaconsMissedTime(10)
    postAssnAPcontrolThresholdLevel(-75),
    postAssnAPcontrolTimeframe(60),
    postAssnBackOffTime(2),
    postAssnSelfSteerOverrideEnable(false),
    roaming80211kvrEnable(false)
{

}

static int get_Device_WiFi_Client_Roaming_Configs()
{
    WiFi_RoamingCtrl_t param;
    IARM_Result_t retVal = IARM_RESULT_SUCCESS;
    int dev_id = 0;
    int retStatus = 0;

    memset(&param,0,sizeof(param));
    hostIf_WiFi_Xrdk_ClientRoaming *pDev = hostIf_WiFi_Xrdk_ClientRoaming::getInstance(dev_id);
    if(pDev)
    {
        retVal = IARM_Bus_Call(IARM_BUS_NM_SRV_MGR_NAME, IARM_BUS_WIFI_MGR_API_getRoamingCtrls, (void *)(&param), sizeof(param));  // IARM Call to netsrvmgr
        if (IARM_RESULT_SUCCESS != retVal)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] IARM BUS CALL failed with  : %d.\n", __FILE__, __FUNCTION__, retVal);
            retStatus = param.status;
        }
        pDev->roamingEnable = param.roamingEnable;
        pDev->preassnBestThreshold = param.preassnBestThreshold;
        pDev->preassnBestDelta = param.preassnBestDelta;
        pDev->postAssnLevelDeltaConnected = param.postAssnLevelDeltaConnected;
        pDev->postAssnLevelDeltaDisconnected = param.postAssnLevelDeltaDisconnected;
        pDev->postAssnSelfSteerThreshold = param.postAssnSelfSteerThreshold;
        pDev->postAssnSelfSteerTimeframe = param.postAssnSelfSteerTimeframe;
        //pDev->postAssnSelfSteerBeaconsMissedTime = param.postAssnSelfSteerBeaconsMissedTime;
        pDev->postAssnAPcontrolThresholdLevel = param.postAssnAPcontrolThresholdLevel;
        pDev->postAssnAPcontrolTimeframe = param.postAssnAPcontrolTimeframe;
        pDev->postAssnBackOffTime = param.postAssnBackOffTime;
        pDev->postAssnSelfSteerOverrideEnable = param.selfSteerOverride;
        pDev->roaming80211kvrEnable = param.roaming80211kvrEnable;
        return OK;
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s]Error!  Unable to connect to wifi instance\n", __FILE__, __FUNCTION__);
        retStatus = param.status;
    }
    return retStatus;
}

static int set_Device_WiFi_Client_Roaming_Configs(WiFi_RoamingCtrl_t *param)
{
    if (param != NULL)
    {
        IARM_Result_t retVal = IARM_RESULT_SUCCESS;
        int dev_id = 0;
        hostIf_WiFi_Xrdk_ClientRoaming *pDev = hostIf_WiFi_Xrdk_ClientRoaming::getInstance(dev_id);
        if(pDev)
        {
            retVal = IARM_Bus_Call(IARM_BUS_NM_SRV_MGR_NAME, IARM_BUS_WIFI_MGR_API_setRoamingCtrls, (void *)(param), sizeof(WiFi_RoamingCtrl_t));  // IARM Call to netsrvmgr
            if (IARM_RESULT_SUCCESS != retVal)
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] IARM BUS CALL failed with  : %d.\n", __FILE__, __FUNCTION__, retVal);
                return NOK;
            }
            pDev->roamingEnable = param->roamingEnable;
            pDev->preassnBestThreshold = param->preassnBestThreshold;
            pDev->preassnBestDelta = param->preassnBestDelta;
            pDev->postAssnLevelDeltaConnected = param->postAssnLevelDeltaConnected;
            pDev->postAssnLevelDeltaDisconnected = param->postAssnLevelDeltaDisconnected;
            pDev->postAssnSelfSteerThreshold = param->postAssnSelfSteerThreshold;
            pDev->postAssnSelfSteerTimeframe = param->postAssnSelfSteerTimeframe;
            //pDev->postAssnSelfSteerBeaconsMissedTime = param->postAssnSelfSteerBeaconsMissedTime;
            pDev->postAssnAPcontrolThresholdLevel = param->postAssnAPcontrolThresholdLevel;
            pDev->postAssnAPcontrolTimeframe = param->postAssnAPcontrolTimeframe;
            pDev->postAssnSelfSteerOverrideEnable = param->selfSteerOverride;
            pDev->roaming80211kvrEnable = param->roaming80211kvrEnable;
            pDev->postAssnBackOffTime = param->postAssnBackOffTime;
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] Successfully set Params - [roamingEnable=%d,preassnBestThreshold=%d,preassnBestDelta=%d \n",__FILE__, __FUNCTION__,param->roamingEnable,param->preassnBestThreshold,param->preassnBestDelta);
            isParamSet = true;
            return OK;
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s]Error!  Unable to connect to wifi instance\n", __FILE__, __FUNCTION__);
            return NOK;
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s]Error!  Invalid Argument in param \n", __FILE__, __FUNCTION__);
        return NOK;
    }
    return OK;
}

int hostIf_WiFi_Xrdk_ClientRoaming::checkWifiClientRoamingropsFetch()
{
    int ret = OK;
    time_t currExTime = time (NULL);
    if ((currExTime - radioFirstExTime ) > QUERY_INTERVAL || isParamSet == true)
    {
        ret = get_Device_WiFi_Client_Roaming_Configs();
        if( OK != ret)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to fetch   : %d.\n", __FILE__, __FUNCTION__, ret);
        }
        radioFirstExTime = currExTime;
        isParamSet = false;
    }
    return ret;
}

int update_from_local_config(WiFi_RoamingCtrl_t* param, hostIf_WiFi_Xrdk_ClientRoaming* roamInst)
{
    param->roamingEnable = roamInst->roamingEnable;
    param->preassnBestThreshold = roamInst->preassnBestThreshold;
    param->preassnBestDelta = roamInst->preassnBestDelta;
    param->selfSteerOverride = roamInst->postAssnSelfSteerOverrideEnable;
    param->postAssnLevelDeltaConnected = roamInst->postAssnLevelDeltaConnected;
    param->postAssnLevelDeltaDisconnected = roamInst->postAssnLevelDeltaDisconnected;
    param->postAssnSelfSteerThreshold = roamInst->postAssnSelfSteerThreshold;
    param->postAssnSelfSteerTimeframe = roamInst->postAssnSelfSteerTimeframe;
    //param->postAssnSelfSteerBeaconsMissedTime = roamInst->postAssnSelfSteerBeaconsMissedTime;
    param->postAssnAPcontrolThresholdLevel = roamInst->postAssnAPcontrolThresholdLevel;
    param->postAssnAPcontrolTimeframe = roamInst->postAssnAPcontrolTimeframe;
    param->roaming80211kvrEnable = roamInst->roaming80211kvrEnable;
    param->postAssnBackOffTime = roamInst->postAssnBackOffTime;

}
int hostIf_WiFi_Xrdk_ClientRoaming::get_Device_WiFi_X_Rdkcentral_clientRoaming_Enable(HOSTIF_MsgData_t *stMsgData )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    int retStatus = OK;
    retStatus = checkWifiClientRoamingropsFetch();
    if(retStatus != OK) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to get clientRoaming_Enable \n", __FUNCTION__, __FILE__);
        stMsgData->faultCode = fcInternalError;
    }
    else {
        put_boolean(stMsgData->paramValue, this->roamingEnable);
    }
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=1;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retStatus;
}

int hostIf_WiFi_Xrdk_ClientRoaming::set_Device_WiFi_X_Rdkcentral_clientRoaming_Enable(HOSTIF_MsgData_t *stMsgData )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    int retStatus = NOK;
    if( radioFirstExTime == 0)
        checkWifiClientRoamingropsFetch();

    WiFi_RoamingCtrl_t param;
    memset(&param,0,sizeof(param));
    update_from_local_config(&param,this);
    param.roamingEnable = get_int(stMsgData->paramValue);
    retStatus = set_Device_WiFi_Client_Roaming_Configs(&param);
    if(retStatus == -2)
        stMsgData->faultCode = fcAttemptToSetaNonWritableParameter; //RFC not enabled 
    else if(retStatus != OK)
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set clientRoaming_Enable..\n", __FUNCTION__, __FILE__);

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retStatus;
}


int hostIf_WiFi_Xrdk_ClientRoaming::get_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_BestThresholdLevel(HOSTIF_MsgData_t *stMsgData )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    int retStatus = OK;
    retStatus = checkWifiClientRoamingropsFetch();
    if(retStatus != OK) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to get BestThresholdLevel \n", __FUNCTION__, __FILE__);
        stMsgData->faultCode = fcInternalError;
    }
    else {
        put_int(stMsgData->paramValue, this->preassnBestThreshold);
    }
    stMsgData->paramtype = hostIf_IntegerType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retStatus;
}

int hostIf_WiFi_Xrdk_ClientRoaming::set_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_BestThresholdLevel(HOSTIF_MsgData_t *stMsgData )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    int retStatus = NOK;
    int threshold = 0;
    if( radioFirstExTime == 0)
        checkWifiClientRoamingropsFetch();
    threshold = get_int(stMsgData->paramValue);
    if(threshold > 0 || threshold < -200) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set BestThresholdLevel - Invalid Value\n", __FUNCTION__, __FILE__);
        stMsgData->faultCode = fcInvalidParameterValue;
        return retStatus;
    }    
    WiFi_RoamingCtrl_t param;
    memset(&param,0,sizeof(param));
    update_from_local_config(&param,this);
    param.preassnBestThreshold = threshold; 
    retStatus = set_Device_WiFi_Client_Roaming_Configs(&param);
    if(retStatus != OK)
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set BestThresholdLevel..\n", __FUNCTION__, __FILE__);

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retStatus;
}

int hostIf_WiFi_Xrdk_ClientRoaming::get_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_BestDeltaLevel(HOSTIF_MsgData_t *stMsgData )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    int retStatus = OK;
    retStatus = checkWifiClientRoamingropsFetch();
    if(retStatus != OK) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to get getDeltaLevel  \n", __FUNCTION__, __FILE__);
        stMsgData->faultCode = fcInternalError;
    }
    else {
        put_int(stMsgData->paramValue, this->preassnBestDelta);
    }
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retStatus;
}

int hostIf_WiFi_Xrdk_ClientRoaming::set_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_BestDeltaLevel(HOSTIF_MsgData_t *stMsgData )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    int retStatus = NOK;
    if( radioFirstExTime == 0)
        checkWifiClientRoamingropsFetch();
    int delta = get_int(stMsgData->paramValue);
    if(delta < 0 || delta > 200) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set PreAssn_BestDeltaLevel - Invalid Value\n", __FUNCTION__, __FILE__);
        stMsgData->faultCode = fcInvalidParameterValue;
        return retStatus;
    } 
    WiFi_RoamingCtrl_t param;
    memset(&param,0,sizeof(param));
    update_from_local_config(&param,this);
    param.preassnBestDelta = delta;
    retStatus = set_Device_WiFi_Client_Roaming_Configs(&param);
    if(retStatus != OK)
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set PreAssn_BestDeltaLevel..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retStatus;
}
int hostIf_WiFi_Xrdk_ClientRoaming::get_Device_WiFi_X_Rdkcentral_clientRoaming_SelfSteerOverride(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] get get_Device_WiFi_X_Rdkcentral_clientRoaming_SelfSteerOverride \n", __FUNCTION__, __FILE__);
    int retStatus = OK;
    retStatus = checkWifiClientRoamingropsFetch();
    if(retStatus != OK) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to get  SelfSteerOverride value \n", __FUNCTION__, __FILE__);
        stMsgData->faultCode = fcInternalError;
    }
    else {
        put_int(stMsgData->paramValue, this->postAssnSelfSteerOverrideEnable);
    }
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retStatus;
}

int hostIf_WiFi_Xrdk_ClientRoaming::set_Device_WiFi_X_Rdkcentral_clientRoaming_SelfSteerOverride(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    int retStatus = NOK;
    if( radioFirstExTime == 0)
       checkWifiClientRoamingropsFetch();
    int self_steerOverride = get_int(stMsgData->paramValue);
    if(self_steerOverride != 0 && self_steerOverride != 1) {
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set  SelfSteerOverride Invalid Value\n", __FUNCTION__, __FILE__);
       stMsgData->faultCode = fcInvalidParameterValue;
       return retStatus;
    }
    WiFi_RoamingCtrl_t param;
    memset(&param,0,sizeof(param));
    update_from_local_config(&param,this);
    param.selfSteerOverride = self_steerOverride;
    retStatus = set_Device_WiFi_Client_Roaming_Configs(&param);
    if(retStatus != OK)
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set SelfSteerOverride..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retStatus;
}
int hostIf_WiFi_Xrdk_ClientRoaming::get_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_BestDeltaLevelConnected(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering...\n", __FUNCTION__, __FILE__);
    int retStatus = OK;
    retStatus = checkWifiClientRoamingropsFetch();
    if(retStatus != OK) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to get  PostAssn_LevelDeltaConnected \n", __FUNCTION__, __FILE__);
        stMsgData->faultCode = fcInternalError;
    }
    else {
        put_int(stMsgData->paramValue, this->postAssnLevelDeltaConnected);
    }
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting...\n", __FUNCTION__, __FILE__);
    return retStatus;
}
int hostIf_WiFi_Xrdk_ClientRoaming::set_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_BestDeltaLevelConnected(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    int retStatus = NOK;
    if( radioFirstExTime == 0)
       checkWifiClientRoamingropsFetch();
    int post_level_Delta_Connected = get_int(stMsgData->paramValue);
    if(post_level_Delta_Connected < 0 || post_level_Delta_Connected > 200) {
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set PostAssn_LevelDeltaConnected Invalid Value\n", __FUNCTION__, __FILE__);
       stMsgData->faultCode = fcInvalidParameterValue;
       return retStatus;
    }
    WiFi_RoamingCtrl_t param;
    memset(&param,0,sizeof(param));
    update_from_local_config(&param,this);
    param.postAssnLevelDeltaConnected = post_level_Delta_Connected;
    retStatus = set_Device_WiFi_Client_Roaming_Configs(&param);
    if(retStatus != OK)
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set postAssnLevelDeltaConnected..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retStatus;
}
int hostIf_WiFi_Xrdk_ClientRoaming::get_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_BestDeltaLevelDisconnected(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering...\n", __FUNCTION__, __FILE__);
    int retStatus = OK;
    retStatus = checkWifiClientRoamingropsFetch();
    if(retStatus != OK) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to get  PostAssn_LevelDeltaDisconnected \n", __FUNCTION__, __FILE__);
        stMsgData->faultCode = fcInternalError;
    }
    else {
        put_int(stMsgData->paramValue, this->postAssnLevelDeltaDisconnected);
    }
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting...\n", __FUNCTION__, __FILE__);
    return retStatus;
}
int hostIf_WiFi_Xrdk_ClientRoaming::set_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_BestDeltaLevelDisconnected(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    int retStatus = NOK;
    if( radioFirstExTime == 0)
       checkWifiClientRoamingropsFetch();
    int post_level_Delta_Disconnected = get_int(stMsgData->paramValue);
    if(post_level_Delta_Disconnected < 0 || post_level_Delta_Disconnected > 200) {
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set PostAssn_LevelDeltaConnected Invalid Value\n", __FUNCTION__, __FILE__);
       stMsgData->faultCode = fcInvalidParameterValue;
       return retStatus;
    }
    WiFi_RoamingCtrl_t param;
    memset(&param,0,sizeof(param));
    update_from_local_config(&param,this);
    param.postAssnLevelDeltaDisconnected = post_level_Delta_Disconnected;
    retStatus = set_Device_WiFi_Client_Roaming_Configs(&param);
    if(retStatus != OK)
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set postAssnLevelDeltaDisconnected..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retStatus;
}
int hostIf_WiFi_Xrdk_ClientRoaming::get_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_SelfSteerThreshold(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering...\n", __FUNCTION__, __FILE__);
    int retStatus = OK;
    retStatus = checkWifiClientRoamingropsFetch();
    if(retStatus != OK) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to get  PostAssn_SelfSteerThreshold \n", __FUNCTION__, __FILE__);
        stMsgData->faultCode = fcInternalError;
    }
    else {
        put_int(stMsgData->paramValue, this->postAssnSelfSteerThreshold);
    }
    stMsgData->paramtype = hostIf_IntegerType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting...\n", __FUNCTION__, __FILE__);
    return retStatus;
}
int hostIf_WiFi_Xrdk_ClientRoaming::set_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_SelfSteerThreshold(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    int retStatus = NOK;
    if( radioFirstExTime == 0)
       checkWifiClientRoamingropsFetch();
    int post_Self_SteerThreshold = get_int(stMsgData->paramValue);
    if(post_Self_SteerThreshold > 0 || post_Self_SteerThreshold < -200) {
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set PostAssn_SelfSteerThreshold Invalid Value\n", __FUNCTION__, __FILE__);
       stMsgData->faultCode = fcInvalidParameterValue;
       return retStatus;
    }
    WiFi_RoamingCtrl_t param;
    memset(&param,0,sizeof(param));
    update_from_local_config(&param,this);
    param.postAssnSelfSteerThreshold = post_Self_SteerThreshold;
    retStatus = set_Device_WiFi_Client_Roaming_Configs(&param);
    if(retStatus != OK)
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set PostAssn_SelfSteerThreshold..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retStatus;
}
int hostIf_WiFi_Xrdk_ClientRoaming::get_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_SelfSteerTimeframe(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering...\n", __FUNCTION__, __FILE__);
    int retStatus = OK;
    retStatus = checkWifiClientRoamingropsFetch();
    if(retStatus != OK) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to get  PostAssn_SelfSteerTimeframe \n", __FUNCTION__, __FILE__);
        stMsgData->faultCode = fcInternalError;
    }
    else {
        put_int(stMsgData->paramValue, this->postAssnSelfSteerTimeframe);
    }
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting...\n", __FUNCTION__, __FILE__);
    return retStatus;
}

int hostIf_WiFi_Xrdk_ClientRoaming::set_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_SelfSteerTimeframe(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    int retStatus = NOK;
    if( radioFirstExTime == 0)
       get_Device_WiFi_Client_Roaming_Configs();
    int post_Self_SteerTimeframe = get_int(stMsgData->paramValue);
    if(post_Self_SteerTimeframe < 0 || post_Self_SteerTimeframe > POST_ASSN_MAX_TIME_FRAME) {
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set PostAssn_SelfSteerTimeframe Invalid Value\n", __FUNCTION__, __FILE__);
       stMsgData->faultCode = fcInvalidParameterValue;
       return retStatus;
    }
    WiFi_RoamingCtrl_t param;
    memset(&param,0,sizeof(param));
    update_from_local_config(&param,this);
    param.postAssnSelfSteerTimeframe = post_Self_SteerTimeframe;
    retStatus = set_Device_WiFi_Client_Roaming_Configs(&param);
    if(retStatus != OK)
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set PostAssn_SelfSteerTimeframe..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retStatus;
}
/*
int hostIf_WiFi_Xrdk_ClientRoaming::get_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_SelfSteerBeaconsMissedTime(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering...\n", __FUNCTION__, __FILE__);
    int retStatus = OK;
    retStatus = checkWifiClientRoamingropsFetch();
    if(retStatus != OK) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to get  PostAssn_SelfSteerBeaconsMissedTime \n", __FUNCTION__, __FILE__);
        stMsgData->faultCode = fcInternalError;
    }
    else {
        put_int(stMsgData->paramValue, this->postAssnSelfSteerBeaconsMissedTime);
    }
    stMsgData->paramtype = hostIf_IntegerType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting...\n", __FUNCTION__, __FILE__);
    return retStatus;
}
int hostIf_WiFi_Xrdk_ClientRoaming::set_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_SelfSteerBeaconsMissedTime(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    int retStatus = NOK;
    if( radioFirstExTime == 0)
       checkWifiClientRoamingropsFetch();
    int post_Self_SteerBeacons_MissedTime = get_int(stMsgData->paramValue);
    if(post_Self_SteerBeacons_MissedTime < 0) {
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set PostAssn_SelfSteerBeaconsMissedTime  Invalid Value\n", __FUNCTION__, __FILE__);
       stMsgData->faultCode = fcInvalidParameterValue;
       return retStatus;
    }
    WiFi_RoamingCtrl_t param;
    memset(&param,0,sizeof(param));
    update_from_local_config(&param,this);
    param.postAssnSelfSteerBeaconsMissedTime = post_Self_SteerBeacons_MissedTime;
    retStatus = set_Device_WiFi_Client_Roaming_Configs(&param);
    if(retStatus != OK)
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set PostAssn_SelfSteerBeaconsMissedTime ..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retStatus;
}*/

int hostIf_WiFi_Xrdk_ClientRoaming::get_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_APcontrolThresholdLevel(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering...\n", __FUNCTION__, __FILE__);
    int retStatus = OK;
    retStatus = checkWifiClientRoamingropsFetch();
    if(retStatus != OK) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to get PostAssn_APcontrolThresholdLevel \n", __FUNCTION__, __FILE__);
        stMsgData->faultCode = fcInternalError;
    }
    else {
        put_int(stMsgData->paramValue, this->postAssnAPcontrolThresholdLevel);
    }
    stMsgData->paramtype = hostIf_IntegerType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting...\n", __FUNCTION__, __FILE__);
    return retStatus;
}

int hostIf_WiFi_Xrdk_ClientRoaming::set_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_APcontrolThresholdLevel(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    int retStatus = NOK;
    if( radioFirstExTime == 0)
       get_Device_WiFi_Client_Roaming_Configs();
    int APcontrolThresholdLevel = get_int(stMsgData->paramValue);
    if(APcontrolThresholdLevel > 0 || APcontrolThresholdLevel < -200 ) {
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set PostAssn_APcontrolThresholdLevel Invalid Value\n", __FUNCTION__, __FILE__);
       stMsgData->faultCode = fcInvalidParameterValue;
       return retStatus;
    }
    WiFi_RoamingCtrl_t param;
    memset(&param,0,sizeof(param));
    update_from_local_config(&param,this);
    param.postAssnAPcontrolThresholdLevel = APcontrolThresholdLevel;
    retStatus = set_Device_WiFi_Client_Roaming_Configs(&param);
    if(retStatus != OK)
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set PostAssn_APcontrolThresholdLevel..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retStatus;
}

int hostIf_WiFi_Xrdk_ClientRoaming::get_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_APcontrolTimeframe(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering...\n", __FUNCTION__, __FILE__);
    int retStatus = OK;
    retStatus = checkWifiClientRoamingropsFetch();
    if(retStatus != OK) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to get PostAssn_APcontrolTimeframe \n", __FUNCTION__, __FILE__);
        stMsgData->faultCode = fcInternalError;
    }
    else {
        put_int(stMsgData->paramValue, this->postAssnAPcontrolTimeframe);
    }
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting...\n", __FUNCTION__, __FILE__);
    return retStatus;
}

int hostIf_WiFi_Xrdk_ClientRoaming::set_Device_WiFi_X_Rdkcentral_clientRoaming_PostAssn_APcontrolTimeframe(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    int retStatus = NOK;
    if( radioFirstExTime == 0)
       get_Device_WiFi_Client_Roaming_Configs();
    int APcontrolTimeframe = get_int(stMsgData->paramValue);
    if(APcontrolTimeframe < 0 || APcontrolTimeframe > POST_ASSN_MAX_TIME_FRAME) {
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set PostAssn_APcontrolTimeframe Invalid Value\n", __FUNCTION__, __FILE__);
       stMsgData->faultCode = fcInvalidParameterValue;
       return retStatus;
    }
    WiFi_RoamingCtrl_t param;
    memset(&param,0,sizeof(param));
    update_from_local_config(&param,this);
    param.postAssnAPcontrolTimeframe = APcontrolTimeframe;
    retStatus = set_Device_WiFi_Client_Roaming_Configs(&param);
    if(retStatus != OK)
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set PostAssn_APcontrolTimeframe..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retStatus;
}

int hostIf_WiFi_Xrdk_ClientRoaming::get_Device_WiFi_X_Rdkcentral_clientRoaming_postAssnBackOffTime(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering...\n", __FUNCTION__, __FILE__);
    int retStatus = OK;
    retStatus = checkWifiClientRoamingropsFetch();
    if(retStatus != OK) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to get postAssnBackOffTime \n", __FUNCTION__, __FILE__);
        stMsgData->faultCode = fcInternalError;
    }
    else {
        put_int(stMsgData->paramValue, this->postAssnBackOffTime);
    }
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting...\n", __FUNCTION__, __FILE__);
    return retStatus;
}

int hostIf_WiFi_Xrdk_ClientRoaming::set_Device_WiFi_X_Rdkcentral_clientRoaming_postAssnBackOffTime(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    int retStatus = NOK;
    if( radioFirstExTime == 0)
       get_Device_WiFi_Client_Roaming_Configs();
    int postAssnBackOffTime = get_int(stMsgData->paramValue);
    if(postAssnBackOffTime < 0 || postAssnBackOffTime > MAX_POST_ASSN_BACKOFF) {
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set postAssnBackOffTime Invalid Value\n", __FUNCTION__, __FILE__);
       stMsgData->faultCode = fcInvalidParameterValue;
       return retStatus;
    }
    WiFi_RoamingCtrl_t param;
    memset(&param,0,sizeof(param));
    update_from_local_config(&param,this);
    param.postAssnBackOffTime = postAssnBackOffTime;
    retStatus = set_Device_WiFi_Client_Roaming_Configs(&param);
    if(retStatus != OK)
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set postAssnBackOffTime..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retStatus;
}

int hostIf_WiFi_Xrdk_ClientRoaming::get_Device_WiFi_X_Rdkcentral_clientRoaming_80211kvrEnable(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering...\n", __FUNCTION__, __FILE__);
    int retStatus = OK;
    retStatus = checkWifiClientRoamingropsFetch();
    if(retStatus != OK) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to get 80211kvrEnable \n", __FUNCTION__, __FILE__);
        stMsgData->faultCode = fcInternalError;
    }
    else {
        put_int(stMsgData->paramValue, this->roaming80211kvrEnable);
    }
    stMsgData->paramtype = hostIf_BooleanType;;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting...\n", __FUNCTION__, __FILE__);
    return retStatus;
}

int hostIf_WiFi_Xrdk_ClientRoaming::set_Device_WiFi_X_Rdkcentral_clientRoaming_80211kvrEnable(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    int retStatus = NOK;
    if( radioFirstExTime == 0)
       get_Device_WiFi_Client_Roaming_Configs();
    int roaming80211kvrEnable = get_int(stMsgData->paramValue);
    if(roaming80211kvrEnable < 0) {
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set 80211kvrEnable Invalid Value\n", __FUNCTION__, __FILE__);
       stMsgData->faultCode = fcInvalidParameterValue;
       return retStatus;
    }
    WiFi_RoamingCtrl_t param;
    memset(&param,0,sizeof(param));
    update_from_local_config(&param,this);
    param.roaming80211kvrEnable = roaming80211kvrEnable;
    retStatus = set_Device_WiFi_Client_Roaming_Configs(&param);
    if(retStatus != OK)
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set 80211kvrEnable..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retStatus;
}

#endif /* #ifdef USE_WIFI_PROFILE */
