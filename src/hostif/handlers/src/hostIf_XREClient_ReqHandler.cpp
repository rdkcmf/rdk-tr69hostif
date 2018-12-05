/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
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
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#include "hostIf_main.h"
#include "hostIf_utils.h"
#ifdef USE_XRESRC
#include "hostIf_XREClient_ReqHandler.h"
#include "Device_XComcast_Xcalibur_Client_CoPilot.h"
#include "Device_XComcast_Xcalibur_Client_XRE.h"
#include "Device_XComcast_Xcalibur_Client_XRE_ConnectionTable.h"
#include "Device_XComcast_Xcalibur_TRM.h"
#include "libIBus.h"
static bool resetEnable = false;

XREClientReqHandler* XREClientReqHandler::pInstance = NULL;
updateCallback XREClientReqHandler::mUpdateCallback = NULL;
GMutex* XREClientReqHandler::m_mutex = NULL;
int XREClientReqHandler::numOfEntries = 0;

msgHandler* XREClientReqHandler::getInstance()
{

    if(!pInstance)
        pInstance = new XREClientReqHandler();
    return pInstance;
}

bool XREClientReqHandler::init()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

bool XREClientReqHandler::unInit()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}


void XREClientReqHandler::reset()
{
    getLock();
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    numOfEntries = 0;
    resetEnable = true;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    releaseLock();
}

void XREClientReqHandler::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);

}

void XREClientReqHandler::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

/**
 * @brief This function use to handle the set message request of XREClient interface
 * and set the attributes such as "xreEnable", "xreRefreshXreSession", "xreFlushLocalCache", "xreLogLevel",
 * "xconfCheckNow", "xreReceiverRestart" and "xreRestartXreApp".
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully.
 * @retval ERR_INTERNAL_ERROR if not able to set data to the device.
 * @ingroup TR-069HOSTIF_XRECLIENT_REQHANDLER_CLASSES
 */
int XREClientReqHandler::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;

    stMsgData->instanceNum = 0;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);

    getLock();

    if (strcasecmp (stMsgData->paramName, "Device.X_COMCAST-COM_Xcalibur.Client.xconfCheckNow") == 0)
    {
        ret = set_Device_X_COMCAST_COM_Xcalibur_Client_xconfCheckNow (stMsgData);
    }
    else if (strcasecmp (stMsgData->paramName, "Device.X_COMCAST-COM_Xcalibur.Client.CoPilot.rssServerArgs") == 0)
    {
        ret = set_Device_X_COMCAST_COM_Xcalibur_Client_CoPilot_rssServerArgs (stMsgData);
    }
    else if (strcasecmp (stMsgData->paramName, "Device.X_COMCAST-COM_Xcalibur.Client.CoPilot.rssServerMode") == 0)
    {
        ret = set_Device_X_COMCAST_COM_Xcalibur_Client_CoPilot_rssServerMode (stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreEnable") == 0)
    {
        ret = set_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreEnable(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreRefreshXreSession") == 0)
    {
        ret = set_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreRefreshXreSession(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreRefreshXreSessionWithRR") == 0)
    {
        ret = set_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreRefreshXreSessionWithRR(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreRestartXreApp") == 0)
    {
        ret = set_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreRestartXreApp(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreFlushLocalCache") == 0)
    {
        ret = set_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreFlushLocalCache(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreLogLevel") == 0)
    {
        ret = set_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreLogLevel(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreReceiverRestart") == 0)
    {
        ret = setXreReceiverRestart(stMsgData);
    }
    else
    {
        stMsgData->faultCode = fcInvalidParameterName;
        ret = NOT_HANDLED;
    }
    releaseLock();

    return ret;
}

int XREClientReqHandler::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *pSetting = NULL;
    int instanceNumber = 0;

    getLock();

    stMsgData->instanceNum = 0;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);

    if (strcasecmp (stMsgData->paramName, "Device.X_COMCAST-COM_Xcalibur.Client.CoPilot.rssServerArgs") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_CoPilot_rssServerArgs (stMsgData);
    }
    else if (strcasecmp (stMsgData->paramName, "Device.X_COMCAST-COM_Xcalibur.Client.CoPilot.rssServerMode") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_CoPilot_rssServerMode (stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreEnable") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreEnable(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreStatus") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreStatus(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreVersion") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreVersion(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreReceiverId") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreReceiverId(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreSessionId") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreSessionId(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreSessionLastModTs") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreSessionLastModTs(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreSessionUptime") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreSessionUptime(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreLastURLAccessed") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreLastURLAccessed(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreMinCmdProcTime") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreMinCmdProcTime(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreAvgCmdProcTime") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreAvgCmdProcTime(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreMaxCmdProcTime") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreMaxCmdProcTime(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"device.x_comcast-com_xcalibur.client.xre.xreCommandCnt") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreCommandCnt(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreErrorCnt") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreErrorCnt(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotViewCnt") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotViewCnt(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotAppCnt") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotAppCnt(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotRectCnt") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotRectCnt(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotTxtCnt") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotTxtCnt(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotHtmlTxtCnt") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotHtmlTxtCnt(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotTxtIpCnt") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotTxtIpCnt(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotFontCnt") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotFontCnt(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotVideoCnt") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotVideoCnt(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotSoundCnt") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotSoundCnt(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotStyleshtCnt") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotStyleshtCnt(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotImgCnt") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotImgCnt(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotNineSliceImgCnt") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotNineSliceImgCnt(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotFlashCnt") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotFlashCnt(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreTotAnimCnt") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotAnimCnt(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreRefreshXreSession") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreRefreshXreSession(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreFlushLocalCache") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreFlushLocalCache(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreLogLevel") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreLogLevel(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreGetTWPDiags") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreGetTWPDiags(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreChannelMapId") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreChannelMapId(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreControllerId") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreControllerId(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xrePlantId") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xrePlantId(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreVodId") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreVodId(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreGatewaySTBMAC") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreGatewaySTBMAC(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.xreLastVideoUrl") == 0)
    {
        ret = get_XreLastVideoUrl(stMsgData);
    }

#if 0
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.numberOfConnections") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_numberOfConnections(stMsgData);
    }
    else if(matchComponent(stMsgData->paramName, "Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable", &pSetting, instanceNumber))
    {
        if ((pSetting != NULL) && (instanceNumber != 0))
        {
            if(strcasecmp(pSetting, "xreAppId") == 0)
            {
                ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_xreAppId(stMsgData, instanceNumber);
            }
            else if(strcasecmp(pSetting, "xreConnURL") == 0)
            {
                ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_xreConnURL(stMsgData, instanceNumber);
            }
            else if(strcasecmp(pSetting, "xreConnStatus") == 0)
            {
                ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_xreConnStatus(stMsgData, instanceNumber);
            }
            else if(strcasecmp(pSetting, "xreConnIfName") == 0)
            {
                ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_xreConnIfName(stMsgData, instanceNumber);
            }
            else if(strcasecmp(pSetting, "xreConnEstTs") == 0)
            {
                ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_xreConnEstTs(stMsgData, instanceNumber);
            }
            else if(strcasecmp(pSetting, "xreConnRetryAttempts") == 0)
            {
                ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_xreConnRetryAttempts(stMsgData, instanceNumber);
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Parameter \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
                stMsgData->faultCode = fcInvalidParameterName;
                ret = NOK;
            }
        }
    }
#endif
	
	else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.xreAppId") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_xreAppId(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.xreConnURL") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_xreConnURL(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.xreConnStatus") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_xreConnStatus(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.xreConnIfName") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_xreConnIfName(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.xreConnEstTs") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_xreConnEstTs(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.xreConnRetryAttempts") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_xreConnRetryAttempts(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.TRM.trmGatewaySTBMAC") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_TRM_trmGatewaySTBMAC(stMsgData);
        ret = OK;                // always returning OK to fix Delia-28157

    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.TRM.trmGatewayMoCAMAC") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_TRM_trmGatewayMoCAMAC(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.TRM.trmGatewayMoCAIP") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_TRM_trmGatewayMoCAIP(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,"Device.X_COMCAST-COM_Xcalibur.TRM.trmGatewayDeviceFriendlyName") == 0)
    {
        ret = get_Device_X_COMCAST_COM_Xcalibur_TRM_trmGatewayDeviceFriendlyName(stMsgData);
    }
    else 
    {
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
       stMsgData->faultCode = fcInvalidParameterName;
       ret = NOK;
    }

    releaseLock();

    return ret;
}

int XREClientReqHandler::handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    getLock();
    // TODO: Retrieve notification value from DeviceInfo structure for given parameter
    releaseLock();
    return ret; 
}       
     
int XREClientReqHandler::handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{           
    int ret = NOT_HANDLED;
    getLock();
    // TODO: Set notification value from DeviceInfo structure for given parameter
    releaseLock();
    return ret;
}
void XREClientReqHandler::registerUpdateCallback(updateCallback cb)
{
    mUpdateCallback = cb;
}

void XREClientReqHandler::checkForUpdates()
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    GList *elem;
    int index = 1;
    char tmp_buff[TR69HOSTIFMGR_MAX_PARAM_LEN];
    getLock();
#if 0
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d] Entering..\n", __FUNCTION__, __LINE__);

    if(resetEnable && get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_numberOfConnections(&msgData) == OK)
    {
        short tmpNoDev = get_int(msgData.paramValue);
        char tmp[TR69HOSTIFMGR_MAX_PARAM_LEN] = "";
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable");

        while(numOfEntries > tmpNoDev)
        {
            sprintf(tmp,"%s.%d.",tmp_buff,tmpNoDev);
            if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE,tmp, NULL, hostIf_IntegerType);
	    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s:%d] IARM_Bus_BroadcastEvent : 'IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE for \'%s\' object. \n", __FUNCTION__, __LINE__, tmp);
            tmpNoDev++;
        }
        while(numOfEntries < tmpNoDev)
        {
            sprintf(tmp,"%s.",tmp_buff);
            if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_ADD,tmp, NULL, hostIf_IntegerType);
	    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s:%d] IARM_Bus_BroadcastEvent : Sending (%d) 'IARM_BUS_TR69HOSTIFMGR_EVENT_ADD' for \'%s\' object. \n", __FUNCTION__, __LINE__, tmpNoDev, tmp);
            tmpNoDev--;
        }
        numOfEntries = get_int(msgData.paramValue);
    }
#endif
#ifdef HAVE_VALUE_CHANGE_EVENT

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreEnable(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreEnable");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }
    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreStatus(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreStatus");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreVersion(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreVersion");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreReceiverId(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreReceiverId");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreSessionId(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreSessionId");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreSessionLastModTs(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreSessionLastModTs");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreSessionUptime(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreSessionUptime");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreLastURLAccessed(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreLastURLAccessed");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreMinCmdProcTime(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreMinCmdProcTime");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreAvgCmdProcTime(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreAvgCmdProcTime");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreMaxCmdProcTime(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreMaxCmdProcTime");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreCommandCnt(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreCommandCnt");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreErrorCnt(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreErrorCnt");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotViewCnt(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreTotViewCnt");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotAppCnt(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreTotAppCnt");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotRectCnt(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreTotRectCnt");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotTxtCnt(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreTotTxtCnt");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotHtmlTxtCnt(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreTotHtmlTxtCnt");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotTxtIpCnt(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreTotTxtIpCnt");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotFontCnt(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreTotFontCnt");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotVideoCnt(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreTotVideoCnt");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotSoundCnt(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreTotSoundCnt");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotStyleshtCnt(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreTotStyleshtCnt");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotImgCnt(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreTotImgCnt");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotNineSliceImgCnt(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreTotNineSliceImgCnt");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotFlashCnt(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreTotFlashCnt");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreTotAnimCnt(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreTotAnimCnt");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreRefreshXreSession(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreRefreshXreSession");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreFlushLocalCache(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreFlushLocalCache");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreLogLevel(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreLogLevel");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreGetTWPDiags(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreGetTWPDiags");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreChannelMapId(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreChannelMapId");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreControllerId(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreControllerId");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xrePlantId(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xrePlantId");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }


    memset(&msgData,0,sizeof(&msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    bChanged =  false;
    get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_xreVodId(&msgData,&bChanged);
    if(bChanged)
    {
        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.%s","xreVodId");
        if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
    }
#if 0
    if(mUpdateCallback)
    {
        memset(&msgData, 0, sizeof(&msgData));
        memset(tmp_buff, 0, TR69HOSTIFMGR_MAX_PARAM_LEN);
        unsigned int connections = 0;
        unsigned int loopCnt = 0;

        /* Get Number of Connections */
        get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_numberOfConnections(&msgData);

        sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.numberOfConnections");
        connections = atoi (msgData.paramValue);
        mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED, tmp_buff, msgData.paramValue, msgData.paramtype);

        for (loopCnt = 1; loopCnt <= connections; loopCnt++)
        {
            memset(&msgData, 0, sizeof(&msgData));
            get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_xreAppId(&msgData, loopCnt);
            sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.%d.xreAppId", loopCnt);
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED, tmp_buff, msgData.paramValue, msgData.paramtype);
        }

        for (loopCnt = 1; loopCnt <= connections; loopCnt++)
        {
            memset(&msgData, 0, sizeof(&msgData));
            get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_xreConnURL(&msgData, loopCnt);
            sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.%d.xreConnURL", loopCnt);
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED, tmp_buff, msgData.paramValue, msgData.paramtype);
        }

        for (loopCnt = 1; loopCnt <= connections; loopCnt++)
        {
            memset(&msgData, 0, sizeof(&msgData));
            get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_xreConnStatus(&msgData, loopCnt);
            sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.%d.xreConnStatus", loopCnt);
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED, tmp_buff, msgData.paramValue, msgData.paramtype);
        }
        for (loopCnt = 1; loopCnt <= connections; loopCnt++)
        {
            memset(&msgData, 0, sizeof(&msgData));
            get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_xreConnIfName(&msgData, loopCnt);
            sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.%d.xreConnIfName", loopCnt);
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED, tmp_buff, msgData.paramValue, msgData.paramtype);
        }
        for (loopCnt = 1; loopCnt <= connections; loopCnt++)
        {
            memset(&msgData, 0, sizeof(&msgData));
            get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_xreConnEstTs(&msgData, loopCnt);
            sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.%d.xreConnEstTs", loopCnt);
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED, tmp_buff, msgData.paramValue, msgData.paramtype);
        }
        for (loopCnt = 1; loopCnt <= connections; loopCnt++)
        {
            memset(&msgData, 0, sizeof(&msgData));
            get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_xreConnRetryAttempts(&msgData, loopCnt);
            sprintf(tmp_buff,"Device.X_COMCAST-COM_Xcalibur.Client.XRE.ConnectionTable.%d.xreConnRetryAttempts", loopCnt);
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED, tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
#endif
#endif /* HAVE_VALUE_CHANGE_EVENT */
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d] Exiting..\n", __FUNCTION__, __LINE__);
    releaseLock();
}
#endif /*USE_XRESRC*/


/** @} */
/** @} */
