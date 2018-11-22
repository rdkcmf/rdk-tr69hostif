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
    preassnBestDelta(2)
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
        get_Device_WiFi_Client_Roaming_Configs();

    WiFi_RoamingCtrl_t param;
    memset(&param,0,sizeof(param));
    param.roamingEnable = get_int(stMsgData->paramValue);
    param.preassnBestThreshold = this->preassnBestThreshold;
    param.preassnBestDelta = this->preassnBestDelta;
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
        get_Device_WiFi_Client_Roaming_Configs();
    threshold = get_int(stMsgData->paramValue);
    if(threshold > 0 || threshold < -200) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set BestThresholdLevel - Invalid Value\n", __FUNCTION__, __FILE__);
        stMsgData->faultCode = fcInvalidParameterValue;
        return retStatus;
    }    
    WiFi_RoamingCtrl_t param;
    memset(&param,0,sizeof(param));
    param.roamingEnable = this->roamingEnable;
    param.preassnBestThreshold = threshold; 
    param.preassnBestDelta = this->preassnBestDelta;
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
    stMsgData->paramtype = hostIf_IntegerType;
    stMsgData->paramLen=4;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retStatus;
}

int hostIf_WiFi_Xrdk_ClientRoaming::set_Device_WiFi_X_Rdkcentral_clientRoaming_PreAssn_BestDeltaLevel(HOSTIF_MsgData_t *stMsgData )
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    int retStatus = NOK;
    if( radioFirstExTime == 0)
        get_Device_WiFi_Client_Roaming_Configs();
    int delta = get_int(stMsgData->paramValue);
    if(delta < 0 || delta > 200) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set PreAssn_BestDeltaLevel - Invalid Value\n", __FUNCTION__, __FILE__);
        stMsgData->faultCode = fcInvalidParameterValue;
        return retStatus;
    } 
    WiFi_RoamingCtrl_t param;
    memset(&param,0,sizeof(param));
    param.roamingEnable = this->roamingEnable;
    param.preassnBestThreshold = this->preassnBestThreshold;
    param.preassnBestDelta = get_int(stMsgData->paramValue);
    retStatus = set_Device_WiFi_Client_Roaming_Configs(&param);
    if(retStatus != OK)
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to set PreAssn_BestDeltaLevel..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return retStatus;
}

#endif /* #ifdef USE_WIFI_PROFILE */
