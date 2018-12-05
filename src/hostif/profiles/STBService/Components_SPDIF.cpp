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


#include "dsTypes.h"
#include "illegalArgumentException.hpp"
#include "exception.hpp"
#include "Components_SPDIF.h"

#define DEV_NAME "SPDIF"
#define BASE_NAME "Device.Services.STBService.1.Components.SPDIF"
#define UPDATE_FORMAT_STRING "%s.%d.%s"

#define ENABLE_STRING "Enable"
#define STATUS_STRING "Status"
#define ALIAS_STRING "Alias"
#define NAME_STRING "Name"
#define FORCEPCM_STRING "ForcePCM"
#define PASSTHROUGH_STRING "Passthrough"
#define AUDIODELAY_STRING "AudioDelay"

#define ENABLED_STRING "Enabled"
#define DISABLED_STRING "Disabled"

GHashTable * hostIf_STBServiceSPDIF::ifHash = NULL;
GMutex * hostIf_STBServiceSPDIF::m_mutex = NULL;

hostIf_STBServiceSPDIF* hostIf_STBServiceSPDIF::getInstance(int dev_id)
{
    hostIf_STBServiceSPDIF* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_STBServiceSPDIF *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try
        {
            pRet = new hostIf_STBServiceSPDIF(dev_id, device::Host::getInstance().getAudioOutputPort(std::string("SPDIF").append(int_to_string(dev_id-1))));
            g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
        }
        catch (const device::IllegalArgumentException &e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught device::IllegalArgumentException, not able create STB service %s Interface instance %d..\n", DEV_NAME, dev_id);
        }
        catch (const int &e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create STB service %s Interface instance %d..\n", DEV_NAME, dev_id);
        }
        catch (const dsError_t &e)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Caught dsError_t %d, not able create STB service %s Interface instance %d..\n", e, DEV_NAME, dev_id);
        }
        catch (const device::Exception &e) 
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught device::Exception %d \"%s\", not able create STB service %s Interface instance %d..\n", e.getCode(), e.getMessage().c_str(), DEV_NAME, dev_id);
        }
    }
    return pRet;
}

GList* hostIf_STBServiceSPDIF::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_STBServiceSPDIF::closeInstance(hostIf_STBServiceSPDIF *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_STBServiceSPDIF::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_STBServiceSPDIF* pDev = (hostIf_STBServiceSPDIF *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

void hostIf_STBServiceSPDIF::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void hostIf_STBServiceSPDIF::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

hostIf_STBServiceSPDIF::hostIf_STBServiceSPDIF(int devid, device::AudioOutputPort& port) : dev_id(devid), aPort(port)
{
    backupEnable = false;
    strcpy(backupStatus, " ");
    strcpy(backupAlias, " ");
    strcpy(backupName, " ");
    backupForcePCM = false;
    backupPassthrough = false;
    backupAudioDelay = 0;

    bCalledEnable = false;
    bCalledStatus = false;
    bCalledAlias = false;
    bCalledName = false;
    bCalledForcePCM = false;
    bCalledPassthrough = false;
    bCalledAudioDelay = false;

}

int hostIf_STBServiceSPDIF::handleSetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if (strcasecmp(paramName, FORCEPCM_STRING) == 0)
    {
        ret = setForcePCM(stMsgData);
    }
    return ret;
}

int hostIf_STBServiceSPDIF::handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if (strcasecmp(paramName, ENABLE_STRING) == 0)
    {
        ret = getEnable(stMsgData);
    }
    else if (strcasecmp(paramName, STATUS_STRING) == 0)
    {
        ret = getStatus(stMsgData);
    }
    else if (strcasecmp(paramName, ALIAS_STRING) == 0)
    {
        ret = getAlias(stMsgData);
    }
    else if (strcasecmp(paramName, NAME_STRING) == 0)
    {
        ret = getName(stMsgData);
    }
    else if (strcasecmp(paramName, FORCEPCM_STRING) == 0)
    {
        ret = getForcePCM(stMsgData);
    }
    else if (strcasecmp(paramName, PASSTHROUGH_STRING) == 0)
    {
        ret = getPassthrough(stMsgData);
    }
    else if (strcasecmp(paramName, AUDIODELAY_STRING) == 0)
    {
        ret = getAudioDelay(stMsgData);
    }

    return ret;
}

void hostIf_STBServiceSPDIF::doUpdates(updateCallback mUpdateCallback)
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    char tmp_buff[PARAM_LEN];

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getEnable(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, ENABLE_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getStatus(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, STATUS_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getAlias(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, ALIAS_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getName(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, NAME_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getForcePCM(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, FORCEPCM_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getPassthrough(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, PASSTHROUGH_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getAudioDelay(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, AUDIODELAY_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
}

int hostIf_STBServiceSPDIF::getNumberOfInstances(HOSTIF_MsgData_t *stMsgData)
{
    int portCount = 0;
    device::List<device::AudioOutputPort> aPorts = device::Host::getInstance().getAudioOutputPorts();
    for (int i; i < aPorts.size() ; i++)
    {
        if (strcasestr(aPorts.at(i).getName().c_str(), "spdif"))
            portCount++;
    }
    put_int(stMsgData->paramValue, portCount);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);
    return OK;
}

// Private Impl below here.

/************************************************************
 * Description  : Set device enable
 * Precondition : None
 * Input        : stMsgData->paramValue -> 1 : Device is enabled.
                                           0 : Device is disabled.

 * Return       : OK -> Success
                  NOK -> Failure
************************************************************/
int hostIf_STBServiceSPDIF::setEnable(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    return ret;
}

/************************************************************
 * Description  : Set Alias for device
 * Precondition : None
 * Input        : stMsgData->paramValue -> non volatile handle for this device.

 * Return       : OK -> Success
                  NOK -> Failure
************************************************************/
int hostIf_STBServiceSPDIF::setAlias(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    return ret;
}

/************************************************************
 * Description  : Set ForcePCM
 * Precondition : None
 * Input        : stMsgData->paramValue -> 1 : Force stream to be uncompressed.
                                           0 : Allow compressed data to be sent.

 * Return       : OK -> Success
                  NOK -> Failure
************************************************************/
int hostIf_STBServiceSPDIF::setForcePCM(HOSTIF_MsgData_t *stMsgData)
{
    int ret = OK;
    try
    {
        dsAudioEncoding_t newEncoding;
        if (*((bool *)stMsgData->paramValue))
            newEncoding = dsAUDIO_ENC_PCM;
        else
            newEncoding = dsAUDIO_ENC_DISPLAY;
        aPort.setEncoding(newEncoding);
    }
    catch (const std::exception e)
    {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception: %s\r\n",__FUNCTION__, e.what());
        ret = NOK;
    }
    return ret;
}

int hostIf_STBServiceSPDIF::getEnable(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int ret = NOT_HANDLED;
    return ret;
}

/************************************************************
 * Description  : Get status
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> "Enabled", "Disabled" or "Error"
************************************************************/
int hostIf_STBServiceSPDIF::getStatus(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int ret = OK; // Since it's hard-coded.
    strncpy(stMsgData->paramValue, ENABLED_STRING, PARAM_LEN);
    stMsgData->paramValue[PARAM_LEN-1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(ENABLED_STRING);
    if(bCalledStatus && pChanged && strcmp(backupStatus, stMsgData->paramValue))
    {
        *pChanged = true;
    }
    bCalledStatus = true;
    strncpy(backupStatus, stMsgData->paramValue, PARAM_LEN);
    backupStatus[PARAM_LEN-1] = '\0';
    return ret;
}

/************************************************************
 * Description  : Get non volatile handle for device
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> the alias value
************************************************************/
int hostIf_STBServiceSPDIF::getAlias(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int ret = NOT_HANDLED;
    return ret;
}

/************************************************************
 * Description  : Get human readable device name
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> humaan readable name string.
************************************************************/
int hostIf_STBServiceSPDIF::getName(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int ret = NOT_HANDLED;
    return ret;
}

/************************************************************
 * Description  : Get ForcePCM setting
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> 1 : Audio format is forced to be pcm
                                           0 : Compressed audio may be sent through (passthrough)
************************************************************/
int hostIf_STBServiceSPDIF::getForcePCM(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int ret = OK;
    try
    {
        dsAudioEncoding_t newEncoding;
        if (aPort.getEncoding().getId() == dsAUDIO_ENC_PCM)
        {
            put_boolean(stMsgData->paramValue, true);
        }
        else
        {
            put_boolean(stMsgData->paramValue, false);
        }
        stMsgData->paramtype = hostIf_BooleanType;
        stMsgData->paramLen = sizeof(bool);
        if(bCalledForcePCM && pChanged && (backupForcePCM != get_boolean(stMsgData->paramValue)))
        {
            *pChanged = true;
        }
        bCalledForcePCM = true;
        backupForcePCM = get_boolean(stMsgData->paramValue);
    }
    catch (const std::exception e)
    {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception: %s\r\n",__FUNCTION__, e.what());
        ret = NOK;
    }
    return ret;
}

/************************************************************
 * Description  : Get Passthrough
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> 1 : Audio datastream is passed without decoding
                                           0 : Audio stream is decoded
************************************************************/
int hostIf_STBServiceSPDIF::getPassthrough(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int ret = OK;
    try
    {
        dsAudioEncoding_t currentEncoding;
        currentEncoding = (dsAudioEncoding_t )aPort.getEncoding().getId();
        if ((currentEncoding == dsAUDIO_ENC_DISPLAY) || (currentEncoding == dsAUDIO_ENC_AC3))
        {
            put_boolean(stMsgData->paramValue, true);
        }
        else
        {
            put_boolean(stMsgData->paramValue, false);
        }
        stMsgData->paramtype = hostIf_BooleanType;
        stMsgData->paramLen = sizeof(bool);
        if(bCalledPassthrough && pChanged && (backupPassthrough != get_boolean(stMsgData->paramValue)))
        {
            *pChanged = true;
        }
        bCalledPassthrough = true;
        backupPassthrough = get_boolean(stMsgData->paramValue);
    }
    catch (const std::exception e)
    {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception: %s\r\n",__FUNCTION__, e.what());
        ret = NOK;
    }
    return ret;
}

/************************************************************
 * Description  : Get Audio Delay
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> Audio Delay
************************************************************/
int hostIf_STBServiceSPDIF::getAudioDelay(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int ret = NOT_HANDLED;
    return ret;
}



/** @} */
/** @} */
