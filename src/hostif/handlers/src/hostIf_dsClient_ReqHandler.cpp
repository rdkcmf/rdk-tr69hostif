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
#include "hostIf_dsClient_ReqHandler.h"

#ifdef USE_XRDK_SDCARD_PROFILE
#include "Components_XrdkSDCard.h"
#endif

#include "Capabilities.h"
#include "Components_AudioOutput.h"
#include "Components_SPDIF.h"
#include "Components_HDMI.h"
#include "Components_VideoOutput.h"
#include "Components_VideoDecoder.h"
#include "hostIf_utils.h"
#include "host.hpp"
#include "manager.hpp"
#include "dsError.h"
#include "libIBus.h"

#ifdef USE_XRDK_RF4CE_PROFILE
#include "Components_XrdkRf4ce.h"
#endif
#define CAPABILTIES_OBJ "Device.Services.STBService.1.Capabilities."

DSClientReqHandler* DSClientReqHandler::pInstance = NULL;

updateCallback DSClientReqHandler::mUpdateCallback = NULL;

msgHandler* DSClientReqHandler::getInstance()
{

    if(!pInstance)
        pInstance = new DSClientReqHandler();
    return pInstance;
}

bool DSClientReqHandler::init()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] Device manager Initializing\n", __FUNCTION__);
    try
    {
       device::Manager::Initialize();
    }
    catch(const std::exception e)
    {
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Exception thrown while initializing device manager %s\n", e.what());
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

bool DSClientReqHandler::unInit()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] Device manager DeInitializing\n", __FUNCTION__);
    hostIf_STBServiceHDMI::closeAllInstances();
	hostIf_STBServiceVideoOutput::closeAllInstances();
	hostIf_STBServiceVideoDecoder::closeAllInstances();
    hostIf_STBServiceAudioInterface::closeAllInstances();
	hostIf_STBServiceSPDIF::closeAllInstances();
    device::Manager::DeInitialize();
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

int DSClientReqHandler::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;

    const char *pSetting;
    int instanceNumber;
    hostIf_STBServiceHDMI::getLock();
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    if(matchComponent(stMsgData->paramName,"Device.Services.STBService.1.Components.HDMI",&pSetting,instanceNumber))
    {
        hostIf_STBServiceHDMI *pIface = hostIf_STBServiceHDMI::getInstance(instanceNumber);
        if(!pIface)
        {
            hostIf_STBServiceHDMI::releaseLock();
            return NOK;
        }
        stMsgData->instanceNum = instanceNumber;
		ret = pIface->handleSetMsg(pSetting, stMsgData);
		
    }
    else if(matchComponent(stMsgData->paramName,
                           "Device.Services.STBService.1.Components.VideoDecoder",&pSetting,instanceNumber))
    {
        hostIf_STBServiceVideoDecoder *pIface = hostIf_STBServiceVideoDecoder::getInstance(instanceNumber);
        if(!pIface)
        {
            hostIf_STBServiceHDMI::releaseLock();
            return NOK;
        }
        stMsgData->instanceNum = instanceNumber;
		ret = pIface->handleSetMsg(pSetting, stMsgData);
    }
    else if(matchComponent(stMsgData->paramName,
                           "Device.Services.STBService.1.Components.AudioOutput",&pSetting,instanceNumber))
    {
        hostIf_STBServiceAudioInterface *pIfaceAudio = hostIf_STBServiceAudioInterface::getInstance(instanceNumber);
        if(!pIfaceAudio)
        {
            hostIf_STBServiceHDMI::releaseLock();
            return NOK;
        }
        stMsgData->instanceNum = instanceNumber;
		ret = pIfaceAudio->handleSetMsg(pSetting, stMsgData);
		
    }
    else if(matchComponent(stMsgData->paramName,
                           "Device.Services.STBService.1.Components.SPDIF",&pSetting,instanceNumber))
    {
        hostIf_STBServiceSPDIF *pIfaceSpdif = hostIf_STBServiceSPDIF::getInstance(instanceNumber);
        if(!pIfaceSpdif)
        {
            hostIf_STBServiceHDMI::releaseLock();
            return NOK;
        }
        stMsgData->instanceNum = instanceNumber;
		ret = pIfaceSpdif->handleSetMsg(pSetting, stMsgData);
    }
    else
    {
        ret = NOK;
        stMsgData->faultCode = fcAttemptToSetaNonWritableParameter;
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed, since Attempt To Set a NonWritable Parameter \"%s\"\n", __FUNCTION__, stMsgData->paramName);
    }
    hostIf_STBServiceHDMI::releaseLock();
    return ret;
}

int DSClientReqHandler::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *paramName;
    int index = -1;
    hostIf_STBServiceHDMI::getLock();


    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    if(strncasecmp(stMsgData->paramName,
                  CAPABILTIES_OBJ, strlen(CAPABILTIES_OBJ)) == 0)
    {
        hostIf_STBServiceCapabilities *pIface = hostIf_STBServiceCapabilities::getInstance();
        if(!pIface)
        {
            hostIf_STBServiceHDMI::releaseLock();
            stMsgData->faultCode = fcInternalError;
            return NOK;
        }
		ret = pIface->handleGetMsg(stMsgData);
        if(OK == ret)
        {
            stMsgData->faultCode = fcNoFault;
        }
    }
    else if(strcasecmp(stMsgData->paramName,
                                  "Device.Services.STBService.1.Enable") == 0)
    {
        put_int(stMsgData->paramValue,1);
        stMsgData->paramtype = hostIf_UnsignedIntType;
        ret = OK;
    }
    else if(strcasecmp(stMsgData->paramName,
                  "Device.Services.STBService.1.Components.VideoDecoderNumberOfEntries") == 0)
    {
        put_int(stMsgData->paramValue,1);
        stMsgData->paramtype = hostIf_UnsignedIntType;
        ret = OK;
    }
    else if(strcasecmp(stMsgData->paramName,
                  "Device.Services.STBServiceNumberOfEntries") == 0)
    {
        put_int(stMsgData->paramValue,1);
        stMsgData->paramtype = hostIf_UnsignedIntType;
        ret = OK;
    }
    else if(strcasecmp(stMsgData->paramName,
                       "Device.Services.STBService.1.Components.AudioOutputNumberOfEntries") == 0)
    {
		ret = hostIf_STBServiceAudioInterface::getNumberOfInstances(stMsgData);
    }
    else if(strcasecmp(stMsgData->paramName,
                       "Device.Services.STBService.1.Components.VideoOutputNumberOfEntries") == 0)
    {
        put_int(stMsgData->paramValue,1);
        stMsgData->paramtype = hostIf_UnsignedIntType;
        ret = OK;
    }
    else if(strcasecmp(stMsgData->paramName,
                       "Device.Services.STBService.1.Components.HDMINumberOfEntries") == 0)
    {
        put_int(stMsgData->paramValue,1);
        stMsgData->paramtype = hostIf_UnsignedIntType;
        ret = OK;
    }
    else if(strcasecmp(stMsgData->paramName,
                       "Device.Services.STBService.1.Components.SPDIFNumberOfEntries") == 0)
    {
        ret = hostIf_STBServiceSPDIF::getNumberOfInstances(stMsgData);
    }
    else if(matchComponent(stMsgData->paramName,
                           "Device.Services.STBService.1.Components.HDMI",&paramName,index))
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] pSetting :[%s] instanceNumber: [%d]\n", __FILE__, __FUNCTION__, __LINE__, paramName,index);
        hostIf_STBServiceHDMI *pIface = hostIf_STBServiceHDMI::getInstance(index);
        if(!pIface)
        {
            hostIf_STBServiceHDMI::releaseLock();
            return NOK;
        }
        stMsgData->instanceNum = index;
		ret = pIface->handleGetMsg(paramName, stMsgData);

    }
    else if(matchComponent(stMsgData->paramName,
                           "Device.Services.STBService.1.Components.VideoDecoder",
                           &paramName,index))
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s%s:%d] pSetting :[%s] instanceNumber: [%d]\n", __FILE__, __FUNCTION__, __LINE__, paramName,index);
        hostIf_STBServiceVideoDecoder *pIface = hostIf_STBServiceVideoDecoder::getInstance(index);
        if(!pIface)
        {
            hostIf_STBServiceHDMI::releaseLock();
            return NOK;
        }
        stMsgData->instanceNum = index;
		ret = pIface->handleGetMsg(paramName, stMsgData);
    }
    else if (matchComponent(stMsgData->paramName,
                            "Device.Services.STBService.1.Components.AudioOutput",
                            &paramName,index))
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s%s:%d] pSetting :[%s] instanceNumber: [%d]\n", __FILE__, __FUNCTION__, __LINE__, paramName,index);
        hostIf_STBServiceAudioInterface *pIfaceAudio = hostIf_STBServiceAudioInterface::getInstance(index);
        if(!pIfaceAudio)
        {
            hostIf_STBServiceHDMI::releaseLock();
            return NOK;
        }
        stMsgData->instanceNum = index;
		ret = pIfaceAudio->handleGetMsg(paramName, stMsgData);

    }
    else if (matchComponent(stMsgData->paramName,"Device.Services.STBService.1.Components.SPDIF",
                            &paramName,index))
    {
        hostIf_STBServiceSPDIF *pIfaceSpdif = hostIf_STBServiceSPDIF::getInstance(index);
        if(!pIfaceSpdif)
        {
            hostIf_STBServiceHDMI::releaseLock();
            return NOK;
        }
        stMsgData->instanceNum = index;
		ret = pIfaceSpdif->handleGetMsg(paramName, stMsgData);
    }
    else if (matchComponent(stMsgData->paramName,
                            "Device.Services.STBService.1.Components.VideoOutput",
                            &paramName,index))
    {
        hostIf_STBServiceVideoOutput *pIface = hostIf_STBServiceVideoOutput::getInstance(index);
        if(!pIface)
        {
            hostIf_STBServiceHDMI::releaseLock();
            return NOK;
        }
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s%s:%d] pSetting :[%s] instanceNumber: [%d]\n", __FILE__, __FUNCTION__, __LINE__, paramName,index);
        stMsgData->instanceNum = index;
		ret = pIface->handleGetMsg(paramName, stMsgData);
    }

#ifdef USE_XRDK_SDCARD_PROFILE
    else if(strncasecmp(stMsgData->paramName, X_SDCARD_OBJ, strlen(X_SDCARD_OBJ)) == 0)
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] Parameter Name : [\'%s\'] \n", __FILE__, __FUNCTION__, __LINE__, stMsgData->paramName);
    	hostIf_STBServiceXSDCard *pIfaceSdcardif = hostIf_STBServiceXSDCard::getInstance();
    	if(!pIfaceSdcardif)
    	{
    		hostIf_STBServiceHDMI::releaseLock();
    		return NOK;
    	}
    	stMsgData->instanceNum = 0;
    	ret = pIfaceSdcardif->handleGetMsg(stMsgData);
    }
#endif

#ifdef USE_XRDK_RF4CE_PROFILE
    else if(strncasecmp(stMsgData->paramName, X_RF4CE_REMOTE_OBJ, strlen(X_RF4CE_REMOTE_OBJ)) == 0)
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] Parameter Name : [\'%s\'] \n", __FILE__, __FUNCTION__, __LINE__, stMsgData->paramName);
        hostIf_STBServiceXrdkRf4ce *pIface = hostIf_STBServiceXrdkRf4ce::getInstance();
        if(!pIface)
        {
            hostIf_STBServiceHDMI::releaseLock();
            return NOK;
        }
        stMsgData->instanceNum = 0;
        ret = pIface->handleGetMsg(stMsgData);
    }
#endif /* USE_XRDK_RF4CE_PROFILE */
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s%s] Failed to match STBSevice Object. Not supported \'%s\' object. \n", __FILE__, __FUNCTION__, stMsgData->paramName);
        stMsgData->faultCode = fcInvalidParameterName;
        ret = NOK;
    }


    hostIf_STBServiceHDMI::releaseLock();
    return ret;
}

void DSClientReqHandler::registerUpdateCallback(updateCallback cb)
{
    mUpdateCallback = cb;
}

int DSClientReqHandler::handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
        int ret = NOT_HANDLED;
        hostIf_STBServiceHDMI::getLock();
        // TODO: Retrieve notification value from DeviceInfo structure for given parameter
    hostIf_STBServiceHDMI::releaseLock();
        return ret;
}

int DSClientReqHandler::handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    hostIf_STBServiceHDMI::getLock();
    // TODO: Set notification value from DeviceInfo structure for given parameter
    hostIf_STBServiceHDMI::releaseLock();
    return ret;
}



void DSClientReqHandler::checkForUpdates()
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    GList *elem;
    int index = 1;
    char tmp_buff[TR69HOSTIFMGR_MAX_PARAM_LEN];
    hostIf_STBServiceHDMI::getLock();

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);

    GList *devList = hostIf_STBServiceHDMI::getAllInstances();
    for(elem = devList; elem; elem = elem->next,index++)
    {
        hostIf_STBServiceHDMI *pIface = hostIf_STBServiceHDMI::getInstance((int)elem->data);
        if(pIface)
        {
			pIface->doUpdates(mUpdateCallback);
		}
    }

	g_list_free(devList);

	devList = hostIf_STBServiceVideoOutput::getAllInstances();
    for(elem = devList; elem; elem = elem->next,index++)
    {
        hostIf_STBServiceVideoOutput *pIface = hostIf_STBServiceVideoOutput::getInstance((int)elem->data);
        if(pIface)
        {
			pIface->doUpdates(mUpdateCallback);
		}
    }

	g_list_free(devList);

	devList = hostIf_STBServiceVideoDecoder::getAllInstances();
    for(elem = devList; elem; elem = elem->next,index++)
    {
        hostIf_STBServiceVideoDecoder *pIface = hostIf_STBServiceVideoDecoder::getInstance((int)elem->data);
        if(pIface)
        {
			pIface->doUpdates(mUpdateCallback);
		}
    }
	
    g_list_free(devList);

    devList = hostIf_STBServiceAudioInterface::getAllInstances();

    for(elem = devList; elem; elem = elem->next,index++)
    {
        hostIf_STBServiceAudioInterface *pIfaceAudio = hostIf_STBServiceAudioInterface::getInstance((int)elem->data);

        if(pIfaceAudio)
        {
			pIfaceAudio->doUpdates(mUpdateCallback);
        }
    }

    g_list_free(devList);
    
    devList = hostIf_STBServiceSPDIF::getAllInstances();
    
    for (elem = devList; elem; elem = elem->next, index++)
    {
        hostIf_STBServiceSPDIF *pIfaceSPDIF = hostIf_STBServiceSPDIF::getInstance((int)elem->data);
        if (pIfaceSPDIF)
        {
            pIfaceSPDIF->doUpdates(mUpdateCallback);
        }
    }
        
    hostIf_STBServiceHDMI::releaseLock();
}







/** @} */
/** @} */
