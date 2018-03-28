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
 * @file hostIf_WiFi_ReqHandler.cpp
 * @brief The header file provides HostIf IP WiFi request handler information APIs.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifdef USE_WIFI_PROFILE
#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_msgHandler.h"

#include "hostIf_WiFi_ReqHandler.h"
#include "Device_WiFi.h"
#include "Device_WiFi_Radio.h"
#include "Device_WiFi_Radio_Stats.h"
#include "Device_WiFi_EndPoint.h"
#include "Device_WiFi_EndPoint_WPS.h"
#include "Device_WiFi_EndPoint_Security.h"
#include "Device_WiFi_EndPoint_Profile.h"
#include "Device_WiFi_EndPoint_Profile_Security.h"
#include "Device_WiFi_AccessPoint.h"
#include "Device_WiFi_AccessPoint_WPS.h"
#include "Device_WiFi_AccessPoint_Security.h"
#include "Device_WiFi_AccessPoint_AssociatedDevice.h"
#include "Device_WiFi_SSID.h"
#include "Device_WiFi_SSID_Stats.h"

WiFiReqHandler* WiFiReqHandler::pInstance = NULL;
updateCallback WiFiReqHandler::mUpdateCallback = NULL;
int WiFiReqHandler::savedSSIDNumberOfEntries = 0;
int WiFiReqHandler::savedRadioNumberOfEntries = 0;
int WiFiReqHandler::savedEndPointNumberOfEntries = 0;
static bool bfirstInstance=false;

msgHandler* WiFiReqHandler::getInstance()
{
    if(!pInstance)
        pInstance = new WiFiReqHandler();
    return pInstance;
}

/**
 * @brief This function is use to initialize. Currently not implemented.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if initialization is successfully .
 * @retval false if initialization is not successful.
 * @ingroup TR-069HOSTIF_WIFI_REQHANDLER_CLASSES
 */
bool WiFiReqHandler::init()
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] WiFi manager Initializing\n", __FUNCTION__);
    return true;
}

void WiFiReqHandler::reset()
{
    bfirstInstance=false;
    savedSSIDNumberOfEntries = 0;
    savedRadioNumberOfEntries = 0;
    savedEndPointNumberOfEntries = 0;
}

/**
 * @brief This function is used to close all the instances of WiFi, WiFi Radio,
 * SSID, AccessPoint, EndPoint etc..
 *
 * @return Returns the status of the operation.
 *
 * @retval true if it is successfully close all the instances.
 * @retval false if not able to close all the instances.
 * @ingroup TR-069HOSTIF_WIFI_REQHANDLER_CLASSES
 */
bool WiFiReqHandler::unInit()
{

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] WiFi manager Deinitializing. \n", __FUNCTION__);

    hostIf_WiFi::closeAllInstances();

    hostIf_WiFi_Radio::closeAllInstances();
    hostIf_WiFi_Radio_Stats::closeAllInstances();

    hostIf_WiFi_SSID::closeAllInstances();
    hostIf_WiFi_SSID_Stats::closeAllInstances();

    hostIf_WiFi_AccessPoint::closeAllInstances();
    hostIf_WiFi_AccessPoint_WPS::closeAllInstances();
    hostIf_WiFi_AccessPoint_Security::closeAllInstances();
    hostIf_WiFi_AccessPoint_AssociatedDevice::closeAllInstances();

    hostIf_WiFi_EndPoint::closeAllInstances();
    hostIf_WiFi_EndPoint_WPS::closeAllInstances();
//    hostIf_WiFi_EndPoint_Security::closeAllInstances();
    hostIf_WiFi_EndPoint_Profile::closeAllInstances();
    hostIf_WiFi_EndPoint_Profile_Security::closeAllInstances();

    WiFiDevice::closeAllInstances();

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

/**
 * @brief This function use to handle the set message request of WiFi.
 * Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully.
 * @retval ERR_INTERNAL_ERROR if not able to set the data to the device.
 * @ingroup TR-069HOSTIF_WIFI_REQHANDLER_CLASSES
 */
int WiFiReqHandler::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%s] Found string as %s. Set command not supported.\n",
             __FUNCTION__, __FILE__, stMsgData->paramName);
    stMsgData->faultCode = fcAttemptToSetaNonWritableParameter;
    return ret;
}

/**
 * @brief This function use to handle the get message request of WiFi interface
 * and get the attributes of "Radio", "SSID", "AccessPoint", "Endpoint" etc.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully.
 * @retval ERR_INTERNAL_ERROR if not able to get data from the device.
 * @ingroup TR-069HOSTIF_WIFI_REQHANDLER_CLASSES
 */
int WiFiReqHandler::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *pSetting;
    const int maxRadioInstances = 1;
    const int maxSSID_Instances = 1;
    int instanceNum = 0;
    int radioIndex = 1;

    if (strcasecmp(stMsgData->paramName,"Device.WiFi.RadioNumberOfEntries") == 0)
    {
        stMsgData->instanceNum = maxRadioInstances;
        hostIf_WiFi *pIface = hostIf_WiFi::getInstance(maxRadioInstances);

        if(!pIface)
        {
            return NOK;
        }

        ret = pIface->get_Device_WiFi_RadioNumberOfEntries(stMsgData);
    }
    else if (strcasecmp(stMsgData->paramName,"Device.WiFi.SSIDNumberOfEntries") == 0)
    {
        stMsgData->instanceNum = maxSSID_Instances;
        hostIf_WiFi *pIface = hostIf_WiFi::getInstance(maxSSID_Instances);

        if(!pIface)
        {
            return NOK;
        }

        ret = pIface->get_Device_WiFi_SSIDNumberOfEntries(stMsgData);
    }
    else if (strcasecmp(stMsgData->paramName,"Device.WiFi.AccessPointNumberOfEntries") == 0)
    {
        stMsgData->instanceNum = 0;
        hostIf_WiFi *pIface = hostIf_WiFi::getInstance(stMsgData->instanceNum);

        if(!pIface)
        {
            return NOK;
        }

        ret = pIface->get_Device_WiFi_AccessPointNumberOfEntries(stMsgData);
    }
    else if (strcasecmp(stMsgData->paramName,"Device.WiFi.EndPointNumberOfEntries") == 0)
    {
        hostIf_WiFi *pIface = hostIf_WiFi::getInstance (1);

        if(!pIface)
        {
            return NOK;
        }
        ret = pIface->get_Device_WiFi_EndPointNumberOfEntries(stMsgData);
    }
    else if (matchComponent(stMsgData->paramName, "Device.WiFi.Radio", &pSetting, instanceNum))
    {
        if ((instanceNum <= 0) || (instanceNum > maxRadioInstances))
        {
            return NOK;
        }

        stMsgData->instanceNum = instanceNum;
        hostIf_WiFi_Radio *pWifiRadio = hostIf_WiFi_Radio::getInstance(stMsgData->instanceNum);
        hostIf_WiFi_Radio_Stats *pWifiRadioStats = hostIf_WiFi_Radio_Stats::getInstance(stMsgData->instanceNum);

        if ((!pWifiRadio) || (!pWifiRadioStats))
        {
            return NOK;
        }

        if (strcasecmp(pSetting,"Enable") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_Enable(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"Status") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_Status(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"Alias") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_Alias(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"Name") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_Name(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"LastChange") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_LastChange(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"LowerLayers") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_LowerLayers(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"Upstream") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_Upstream(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"MaxBitRate") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_MaxBitRate(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"SupportedFrequencyBands") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_SupportedFrequencyBands(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"OperatingFrequencyBand") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_OperatingFrequencyBand(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"SupportedStandards") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_SupportedStandards(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"OperatingStandards") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_OperatingStandards(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"PossibleChannels") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_PossibleChannels(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"ChannelsInUse") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_ChannelsInUse(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"Channel") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_Channel(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"AutoChannelSupported") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_AutoChannelSupported(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"AutoChannelEnable") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_AutoChannelEnable(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"AutoChannelRefreshPeriod") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_AutoChannelRefreshPeriod(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"OperatingChannelBandwidth") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_OperatingChannelBandwidth(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"ExtensionChannel") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_ExtensionChannel(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"GuardInterval") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_GuardInterval(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"MCS") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_MCS(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"MCS") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_MCS(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"TransmitPowerSupported") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_TransmitPowerSupported(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"TransmitPower") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_TransmitPower(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"IEEE80211hSupported") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_IEEE80211hSupported(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"IEEE80211hEnabled") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_IEEE80211hEnabled(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"RegulatoryDomain") == 0)
        {
            ret = pWifiRadio->get_Device_WiFi_Radio_RegulatoryDomain(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"Stats.BytesSent") == 0)
        {
            ret = pWifiRadioStats->get_Device_WiFi_Radio_Stats_BytesSent(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"Stats.BytesReceived") == 0)
        {
            ret = pWifiRadioStats->get_Device_WiFi_Radio_Stats_BytesReceived(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"Stats.PacketsSent") == 0)
        {
            ret = pWifiRadioStats->get_Device_WiFi_Radio_Stats_PacketsSent(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"Stats.PacketsReceived") == 0)
        {
            ret = pWifiRadioStats->get_Device_WiFi_Radio_Stats_PacketsReceived(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"Stats.ErrorsSent") == 0)
        {
            ret = pWifiRadioStats->get_Device_WiFi_Radio_Stats_ErrorsSent(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"Stats.ErrorsReceived") == 0)
        {
            ret = pWifiRadioStats->get_Device_WiFi_Radio_Stats_ErrorsReceived(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"Stats.DiscardPacketsSent") == 0)
        {
            ret = pWifiRadioStats->get_Device_WiFi_Radio_Stats_DiscardPacketsSent(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"Stats.DiscardPacketsReceived") == 0)
        {
            ret = pWifiRadioStats->get_Device_WiFi_Radio_Stats_DiscardPacketsReceived(stMsgData,radioIndex);
        }
        else if (strcasecmp(pSetting,"Stats.Noise") == 0)
        {
            ret = pWifiRadioStats->get_Device_WiFi_Radio_Stats_NoiseFloor(stMsgData,radioIndex);
        }
        else
        {
           RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]  Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
           stMsgData->faultCode = fcInvalidParameterName;
           ret = NOK; 
        }
    }
    else if (matchComponent(stMsgData->paramName, "Device.WiFi.Endpoint", &pSetting, instanceNum))
    {
        stMsgData->instanceNum = instanceNum;
        hostIf_WiFi_EndPoint *pWifiEndPoint = hostIf_WiFi_EndPoint::getInstance(stMsgData->instanceNum);
        hostIf_WiFi_EndPoint_WPS *pWifiEndPointWps  = hostIf_WiFi_EndPoint_WPS::getInstance(stMsgData->instanceNum);

        if ((!pWifiEndPoint) || (!pWifiEndPointWps))
        {
            return NOK;
        }

        if (strcasecmp(pSetting,"Enable") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_Enable(stMsgData);
        }
        else if (strcasecmp(pSetting,"Status") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_Status(stMsgData);
        }
        else if (strcasecmp(pSetting,"Alias") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_Alias(stMsgData);
        }
        else if (strcasecmp(pSetting,"ProfileReference") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_ProfileReference(stMsgData);
        }
        else if (strcasecmp(pSetting,"SSIDReference") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_SSIDReference(stMsgData);
        }
        else if (strcasecmp(pSetting,"ProfileNumberOfEntries") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_ProfileNumberOfEntries(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.LastDataDownlinkRate") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_Stats_LastDataDownlinkRate(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.LastDataUplinkRate") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_Stats_LastDataUplinkRate(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.SignalStrength") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_Stats_SignalStrength(stMsgData);
        }
        else if (strcasecmp(pSetting,"Stats.Retransmissions") == 0)
        {
            ret = pWifiEndPoint->get_Device_WiFi_EndPoint_Stats_Retransmissions(stMsgData);
        }
        else
        {
           RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]  Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
           stMsgData->faultCode = fcInvalidParameterName;
           ret = NOK;
        }

    }
    else if (matchComponent(stMsgData->paramName, "Device.WiFi.AccessPoint", &pSetting, instanceNum))
    {
        stMsgData->instanceNum = instanceNum;
        hostIf_WiFi_AccessPoint *pIfaceAccessPoint = hostIf_WiFi_AccessPoint::getInstance(stMsgData->instanceNum);
        hostIf_WiFi_AccessPoint_WPS *pIfaceAccessPointWps = hostIf_WiFi_AccessPoint_WPS::getInstance(stMsgData->instanceNum);
        hostIf_WiFi_AccessPoint_Security *pIfaceAccessPointSec = hostIf_WiFi_AccessPoint_Security::getInstance(stMsgData->instanceNum);
        hostIf_WiFi_AccessPoint_AssociatedDevice *pIfaceAccessPointAssDev = hostIf_WiFi_AccessPoint_AssociatedDevice::getInstance(stMsgData->instanceNum);

        if ((!pIfaceAccessPoint) || (!pIfaceAccessPointWps) || (!pIfaceAccessPointSec) || (!pIfaceAccessPointAssDev))
        {
            return NOK;
        }
    }
    else if (matchComponent(stMsgData->paramName, "Device.WiFi.SSID", &pSetting, instanceNum))
    {
        if ((instanceNum <= 0) || (instanceNum > maxSSID_Instances))
        {
            return NOK;
        }

        stMsgData->instanceNum = instanceNum;
        hostIf_WiFi_SSID *pIfaceSsid = hostIf_WiFi_SSID::getInstance(stMsgData->instanceNum);
        hostIf_WiFi_SSID_Stats *pIfaceSsidStats = hostIf_WiFi_SSID_Stats::getInstance(stMsgData->instanceNum);

        if ((!pIfaceSsid) || (!pIfaceSsidStats))
        {
            return NOK;
        }

        if (strcasecmp(pSetting, "Enable") == 0)
        {
            ret = pIfaceSsid->get_Device_WiFi_SSID_Enable(stMsgData);
        }
        else if (strcasecmp(pSetting, "Status") == 0)
        {
            ret = pIfaceSsid->get_Device_WiFi_SSID_Status(stMsgData);
        }
        else if (strcasecmp(pSetting, "BSSID") == 0)
        {
            ret = pIfaceSsid->get_Device_WiFi_SSID_BSSID(stMsgData);
        }
        else if (strcasecmp(pSetting, "MACAddress") == 0)
        {
            ret = pIfaceSsid->get_Device_WiFi_SSID_MACAddress(stMsgData);
        }
        else if (strcasecmp(pSetting, "SSID") == 0)
        {
            ret = pIfaceSsid->get_Device_WiFi_SSID_SSID(stMsgData);
        }
        else if (strcasecmp(pSetting, "Name") == 0)
        {
            ret = pIfaceSsid->get_Device_WiFi_SSID_Name(stMsgData);
        }
        else if (strcasecmp(pSetting, "Alias") == 0)
        {
            ret = pIfaceSsid->get_Device_WiFi_SSID_Alias(stMsgData);
        }
        else
        {
           RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]  Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
           stMsgData->faultCode = fcInvalidParameterName;
           ret = NOK;
        }

    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]  Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
        stMsgData->faultCode = fcInvalidParameterName;
        ret = NOK;
    }

    return ret;
}

int WiFiReqHandler::handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] TODO Entering  Parameter value = %s \n", __FILE__, __FUNCTION__,stMsgData->paramName);
    // TODO: Retrieve notification value from DeviceInfo structure for given parameter
    return ret;
}

int WiFiReqHandler::handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{

	int ret = NOT_HANDLED;
/*	int instanceNumber = 0;
	hostIf_WiFi::getLock();
	// TODO: Set notification value from DeviceInfo structure for given parameter
	RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] WiFiReqHandler get para as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
	hostIf_WiFi *pIface = hostIf_WiFi::getInstance(instanceNumber);
	stMsgData->instanceNum = instanceNumber;
	if(!pIface)
	{
		RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] pIface is failed. For param as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
		hostIf_WiFi::releaseLock();
			return NOK;
	}

	GHashTable* notifyhash = pIface->getNotifyHash();
	if(notifyhash != NULL)
	{
		RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] notifyhash is not Null %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
	int notifyvalue = get_int(stMsgData->paramValue);
	g_hash_table_insert(notifyhash,stMsgData->paramName,(gpointer) notifyvalue);
	ret = OK;
	}
	else
	{
		RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] notifyhash is Null %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
	   ret = NOK;
	}
	hostIf_WiFi::releaseLock();*/
	return ret;
}
void WiFiReqHandler::registerUpdateCallback(updateCallback cb)
{
    mUpdateCallback = cb;
}

void WiFiReqHandler::checkForUpdates()
{
    LOG_ENTRY_EXIT;

    if (NULL == mUpdateCallback)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s:%s] mUpdateCallback is NULL\n", __FILE__, __FUNCTION__);
        return;
    }

    hostIf_WiFi *pIface = hostIf_WiFi::getInstance(1);
    if (NULL == pIface)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s:%s] hostIf_WiFi::getInstance(1) returned NULL\n", __FILE__, __FUNCTION__);
        return;
    }

    HOSTIF_MsgData_t stMsgData;

    if (OK == pIface->get_Device_WiFi_SSIDNumberOfEntries(&stMsgData))
    {
        int currentSSIDNumberOfEntries = get_int (stMsgData.paramValue);
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s:%s] currentSSIDNumberOfEntries = %d, savedSSIDNumberOfEntries = %d\n",
                __FILE__, __FUNCTION__, currentSSIDNumberOfEntries, savedSSIDNumberOfEntries);
        sendAddRemoveEvents (mUpdateCallback, currentSSIDNumberOfEntries, savedSSIDNumberOfEntries, (char *)DEVICE_WIFI_SSID_PROFILE);
    }

    if (!bfirstInstance && (OK == pIface->get_Device_WiFi_RadioNumberOfEntries (&stMsgData)))
    {
        bfirstInstance = true;
        int currentRadioNumberOfEntries = get_int (stMsgData.paramValue);
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s:%s] currentRadioNumberOfEntries = %d, savedRadioNumberOfEntries = %d\n",
                __FILE__, __FUNCTION__, currentRadioNumberOfEntries, savedRadioNumberOfEntries);
        sendAddRemoveEvents (mUpdateCallback, currentRadioNumberOfEntries, savedRadioNumberOfEntries, (char *)DEVICE_WIFI_RADIO_PROFILE);
    }

    if (OK == pIface->get_Device_WiFi_EndPointNumberOfEntries (&stMsgData))
    {
        int currentEndPointNumberOfEntries = get_int (stMsgData.paramValue);
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s:%s] currentEndPointNumberOfEntries = %d, savedEndPointNumberOfEntries = %d\n",
                __FILE__, __FUNCTION__, currentEndPointNumberOfEntries, savedEndPointNumberOfEntries);
        sendAddRemoveEvents (mUpdateCallback, currentEndPointNumberOfEntries, savedEndPointNumberOfEntries, (char *)DEVICE_WIFI_ENDPOINT_PROFILE);
    }
}

#endif /* #ifdef USE_WIFI_PROFILE */


/** @} */
/** @} */
