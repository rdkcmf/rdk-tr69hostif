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
 * @file Components_AudioOutput.cpp
 * @brief This source file contains the APIs of TR069 Components AudioOutput.
 */


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#include "hostIf_main.h"
#include "dsTypes.h"
#include "illegalArgumentException.hpp"
#include "exception.hpp"
#include "Components_AudioOutput.h"
#include "safec_lib.h"

#define DEV_NAME "AudioOutput"
#define BASE_NAME "Device.Services.STBService.1.Components.AudioOutput"
#define UPDATE_FORMAT_STRING "%s.%d.%s"

#define STATUS_STRING "Status"
#define ENABLED_STRING "Enabled"
#define ENABLE_STRING "Enable"
#define CANCELMUTE_STRING "CancelMute"
#define AUDIOFORMAT_STRING "AudioFormat"
#define NAME_STRING "Name"
#define AUDIOLEVEL_STRING "AudioLevel"
#define COMCAST_AUDIOOPTIMALLEVEL_STRING "X_COMCAST-COM_AudioOptimalLevel"
#define COMCAST_MINAUDIODB_STRING "X_COMCAST-COM_MinAudioDB"
#define COMCAST_MAXAUDIODB_STRING "X_COMCAST-COM_MaxAudioDB"
#define COMCAST_AUDIODB_STRING "X_COMCAST-COM_AudioDB"
#define COMCAST_AUDIOSTEREOMODE_STRING "X_COMCAST-COM_AudioStereoMode"
#define COMCAST_AUDIOLOOPTHRU_STRING "X_COMCAST-COM_AudioLoopThru"
#define COMCAST_AUDIOENCODING_STRING "X_COMCAST-COM_AudioEncoding"
#define COMCAST_AUDIOCOMPRESSION_STRING "X_COMCAST-COM_AudioCompression"
#define COMCAST_AUDIOGAIN_STRING "X_COMCAST-COM_AudioGain"
#define COMCAST_DIALOGENHANCEMENT_STRING "X_COMCAST-COM_DialogEnhancement"


GHashTable * hostIf_STBServiceAudioInterface::ifHash = NULL;
GMutex * hostIf_STBServiceAudioInterface::m_mutex = NULL;

hostIf_STBServiceAudioInterface* hostIf_STBServiceAudioInterface::getInstance(int dev_id)
{
    hostIf_STBServiceAudioInterface* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_STBServiceAudioInterface *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d]: dev_id=[%d]; pRet=0x%08X\n", __FILE__, __FUNCTION__, __LINE__, dev_id, pRet);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try 
        {
            device::List<device::AudioOutputPort> aPorts = device::Host::getInstance().getAudioOutputPorts();
            pRet = new hostIf_STBServiceAudioInterface(dev_id, aPorts.at(dev_id-1));
            g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d]: AudioLevel[%d]=%f\n", __FILE__, __FUNCTION__, __LINE__, dev_id, pRet->aPort.getLevel());
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
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught dsError_t %d, not able create STB service %s Interface instance %d..\n", e, DEV_NAME, dev_id);
        }
        catch (const device::Exception &e) 
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Caught device::Exception %d \"%s\", not able create STB service %s Interface instance %d..\n", e.getCode(), e.getMessage().c_str(), DEV_NAME, dev_id);
        }
    }
    return pRet;
}

GList* hostIf_STBServiceAudioInterface::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_STBServiceAudioInterface::closeInstance(hostIf_STBServiceAudioInterface *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_STBServiceAudioInterface::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_STBServiceAudioInterface* pDev = (hostIf_STBServiceAudioInterface *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

void hostIf_STBServiceAudioInterface::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void hostIf_STBServiceAudioInterface::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

/**
 * @brief Class Constructor of the class hostIf_STBServiceAudioInterface.
 *
 * It will initialize the device id and audio output port.
 *
 * @param[in] devid Identification number of the device.
 * @param[in] port Audio output port number.
 */
hostIf_STBServiceAudioInterface::hostIf_STBServiceAudioInterface(int devid, device::AudioOutputPort& port) : dev_id(devid), aPort(port)
{
    errno_t rc = -1;
    rc=strcpy_s(backupStatus,sizeof(backupStatus), " ");
    if(rc!=EOK)
    {
	    ERR_CHK(rc);
    }
    backupCancelMute = false;
    rc=strcpy_s(backupAudioStereoMode,sizeof(backupAudioStereoMode)," ");
    if(rc!=EOK)
    {
	    ERR_CHK(rc);
    }
    backupAudioLevel=0;
    rc=strcpy_s(backupAudioDB,sizeof(backupAudioDB)," ");
    if(rc!=EOK)
    {
	    ERR_CHK(rc);
    }
    rc=strcpy_s(backupAudioLoopThru,sizeof(backupAudioLoopThru)," ");
    if(rc!=EOK)
    {
	    ERR_CHK(rc);
    }
    backupAudioCompression=0;
    rc=strcpy_s(backupAudioEncoding,sizeof(backupAudioEncoding)," ");
    if(rc!=EOK)
    {
	    ERR_CHK(rc);
    }
    rc=strcpy_s(backupAudioGain,sizeof(backupAudioGain)," ");
    if(rc!=EOK)
    {
	    ERR_CHK(rc);
    }
    rc=strcpy_s(backupMinAudioDB,sizeof(backupMinAudioDB)," ");
    if(rc!=EOK)
    {
	    ERR_CHK(rc);
    }
    rc=strcpy_s(backupMaxAudioDB,sizeof(backupMaxAudioDB)," ");
    if(rc!=EOK)
    {
	    ERR_CHK(rc);
    }
    rc=strcpy_s(backupAudioOptimalLevel,sizeof(backupAudioOptimalLevel)," ");
    if(rc!=EOK)
    {
	    ERR_CHK(rc);
    }
    backupDialogEnhancement=0;
    bCalledStatus = false;
    bCalledCancelMute = false;
    bCalledAudioStereoMode = false;
    bCalledAudioLevel = false;
    bCalledAudioDB = false;
    bCalledAudioLoopThru = false;
    bCalledAudioCompression = false;
    bCalledAudioEncoding = false;
    bCalledAudioGain = false;
    bCalledMinAudioDB = false;
    bCalledMaxAudioDB = false;
    bCalledAudioOptimalLevel = false;
    bCalledDialogEnhancement = false;
}

/**
 * @brief This function set the audio interface such as mute status, audio encoding, audio level,
 * Minimum dB(decibel) and Maximum dB(decibel) that can be supported on audio port, Stereo mode used
 * in a given audio port etc..
 *
 * @param[in] pSetting  Audio service name string.
 * @param[in] stMsgData  HostIf Message Request param contains the audio attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the hostIf audio interface attribute.
 * @retval -1 If Not able to set the hostIf audio interface attribute.
 * @ingroup TR69_HOSTIF_STBSERVICES_AUDIOOUTPUT_API
 */
int hostIf_STBServiceAudioInterface::handleSetMsg(const char *pSetting, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if (strcasecmp(pSetting, CANCELMUTE_STRING) == 0)
    {
        ret = setCancelMute(stMsgData);
    }
    else if (strcasecmp(pSetting, AUDIOLEVEL_STRING) == 0)
    {
        ret = setAudioLevel(stMsgData);
    }
    else if (strcasecmp(pSetting, COMCAST_AUDIODB_STRING) == 0)
    {
        ret = setX_COMCAST_COM_AudioDB(stMsgData);
    }
    else if (strcasecmp(pSetting, COMCAST_AUDIOSTEREOMODE_STRING) == 0)
    {
        ret = setX_COMCAST_COM_AudioStereoMode(stMsgData);
    }
    else if (strcasecmp(pSetting, COMCAST_AUDIOLOOPTHRU_STRING) == 0)
    {
        ret = setX_COMCAST_COM_AudioLoopThru(stMsgData);
    }
    else if (strcasecmp(pSetting, COMCAST_AUDIOENCODING_STRING) == 0)
    {
        ret = setAudioEncoding(stMsgData);
    }
    else if (strcasecmp(pSetting, COMCAST_AUDIOCOMPRESSION_STRING) == 0)
    {
        ret = setX_COMCAST_COM_AudioCompression(stMsgData);
    }
    else if (strcasecmp(pSetting, COMCAST_DIALOGENHANCEMENT_STRING) == 0)
    {
        ret = setX_COMCAST_COM_DialogEnhancement(stMsgData);
    }
    return ret;
}

/**
 * @brief This function get the audio interface attribute values such as mute status, audio encoding,
 * audio level, Minimum dB(decibel) and Maximum dB(decibel) that can be supported on audio port, Stereo mode used
 * in a given audio port etc..
 *
 * @param[in] paramName  Audio service name string.
 * @param[in] stMsgData  Host IF Message Request param contains the audio attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hostIf audio interface attribute.
 * @retval -1 If Not able to get the hostIf audio interface attribute.
 * @retval -2 If Not handle the hostIf audio interface attribute.
 * @ingroup TR69_HOSTIF_STBSERVICES_AUDIOOUTPUT_API
 */
int hostIf_STBServiceAudioInterface::handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if(strcasecmp(paramName, STATUS_STRING) == 0)
    {
        ret = getStatus(stMsgData);
    }
    else if(strcasecmp(paramName, ENABLE_STRING) == 0)
    {
        ret = getEnable(stMsgData);
    }
    else if(strcasecmp(paramName, CANCELMUTE_STRING) == 0)
    {
        ret = getCancelMute(stMsgData);
    }
    else if(strcasecmp(paramName, AUDIOFORMAT_STRING) == 0)
    {
        ret = getX_COMCAST_COM_AudioFormat(stMsgData);
    }
    else if(strcasecmp(paramName, NAME_STRING) == 0)
    {
        ret = getName(stMsgData);
    }
    else if(strcasecmp(paramName, AUDIOLEVEL_STRING) == 0)
    {
        ret = getAudioLevel(stMsgData);
    }
    else if(strcasecmp(paramName, COMCAST_AUDIOOPTIMALLEVEL_STRING) == 0)
    {
        ret = getX_COMCAST_COM_AudioOptimalLevel(stMsgData);
    }
    else if(strcasecmp(paramName, COMCAST_MINAUDIODB_STRING) == 0)
    {
        ret = getX_COMCAST_COM_MinAudioDB(stMsgData);
    }
    else if(strcasecmp(paramName, COMCAST_MAXAUDIODB_STRING) == 0)
    {
        ret = getX_COMCAST_COM_MaxAudioDB(stMsgData);
    }
    else if(strcasecmp(paramName, COMCAST_AUDIODB_STRING) == 0)
    {
        ret = getX_COMCAST_COM_AudioDB(stMsgData);
    }
    else if(strcasecmp(paramName,COMCAST_AUDIOSTEREOMODE_STRING) == 0)
    {
        ret = getX_COMCAST_COM_AudioStereoMode(stMsgData);
    }
    else if(strcasecmp(paramName, COMCAST_AUDIOLOOPTHRU_STRING) == 0)
    {
        ret = getX_COMCAST_COM_AudioLoopThru(stMsgData);
    }
    else if(strcasecmp(paramName, COMCAST_AUDIOENCODING_STRING) == 0)
    {
        ret = getX_COMCAST_COM_AudioEncoding(stMsgData);
    }
    else if(strcasecmp(paramName, COMCAST_AUDIOCOMPRESSION_STRING) == 0)
    {
        ret = getX_COMCAST_COM_AudioCompression(stMsgData);
    }
    else if(strcasecmp(paramName, COMCAST_AUDIOGAIN_STRING) == 0)
    {
        ret = getX_COMCAST_COM_AudioGain(stMsgData);
    }
    else if(strcasecmp(paramName, COMCAST_DIALOGENHANCEMENT_STRING) == 0)
    {
        ret = getX_COMCAST_COM_DialogEnhancement(stMsgData);
    }
    return ret;
}

/**
 * @brief This function updates the audio interface such as mute status, audio encoding, audio level,
 * Minimum dB(decibel) and Maximum dB(decibel) that can be supported on audio port, Stereo mode used
 * in a given audio port etc.. using callback mechanism.
 *
 * @param[in] mUpdateCallback  Callback function which updates the hostIf audio interface.
 * @ingroup TR69_HOSTIF_STBSERVICES_AUDIOOUTPUT_API
 */
void hostIf_STBServiceAudioInterface::doUpdates(updateCallback mUpdateCallback)
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    char tmp_buff[PARAM_LEN];

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getCancelMute(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, CANCELMUTE_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getX_COMCAST_COM_AudioEncoding(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_AUDIOENCODING_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getAudioLevel(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, AUDIOLEVEL_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getX_COMCAST_COM_AudioOptimalLevel(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_AUDIOOPTIMALLEVEL_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getX_COMCAST_COM_MinAudioDB(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_MINAUDIODB_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getX_COMCAST_COM_MaxAudioDB(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_MAXAUDIODB_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getX_COMCAST_COM_AudioDB(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_AUDIODB_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getX_COMCAST_COM_AudioStereoMode(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_AUDIOSTEREOMODE_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getX_COMCAST_COM_AudioLoopThru(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_AUDIOLOOPTHRU_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getX_COMCAST_COM_AudioCompression(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_AUDIOCOMPRESSION_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getX_COMCAST_COM_AudioGain(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_AUDIOGAIN_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getX_COMCAST_COM_DialogEnhancement(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_DIALOGENHANCEMENT_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }

}

int hostIf_STBServiceAudioInterface::getNumberOfInstances(HOSTIF_MsgData_t *stMsgData)
{
    device::List<device::AudioOutputPort> aPorts = device::Host::getInstance().getAudioOutputPorts();
    put_int(stMsgData->paramValue, aPorts.size());
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);
    return OK;
}

/************************************************************
 * Description  : Get Audio Status
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> Disabled: Audio port diabled
                                        -> Enabled: Audio port enabled
                                        -> Muted: Audio port enabled and muted
                                        -> Error: Error
************************************************************/

int hostIf_STBServiceAudioInterface::getStatus(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    //RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    try {
        const char* audioPortStatus = NULL;
        bool isEnabled = aPort.isEnabled();
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] dev_id = %d, isEnabled = %s\n", __FUNCTION__, dev_id, isEnabled?"true":"false");
        audioPortStatus = isEnabled? "Enabled" : "Disabled";
        if(isEnabled)
        {
            bool isMute = aPort.isMuted();
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] dev_id = %d, isMute = %s\n", __FUNCTION__, dev_id, isMute?"true":"false");
            audioPortStatus = isMute? "Muted" : "Enabled";
        }

        strncpy(stMsgData->paramValue, audioPortStatus, PARAM_LEN);
        stMsgData->paramValue[PARAM_LEN-1] = '\0';
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);

        if(bCalledStatus && pChanged && strcmp(backupStatus, stMsgData->paramValue))
        {
            *pChanged = true;
        }
        bCalledStatus = true;
        strncpy(backupStatus, stMsgData->paramValue, sizeof(backupStatus) -1);
        backupStatus [sizeof(backupStatus) -1] = '\0';
        //CID:44082 - OVERRUN
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] [Value: %s] \n", __FUNCTION__, stMsgData->paramValue);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }
    //RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

/************************************************************
 * Description  : Cancel Audio Mute
 * Precondition : None
 * Input        : stMsgData->paramValue -> 1: cancels audio output mute status
                                        -> 0: Setting it to false has no effect
 * Return       : OK -> Success
                  NOK -> Failure
************************************************************/

int hostIf_STBServiceAudioInterface::setCancelMute(const HOSTIF_MsgData_t *stMsgData)
{
    bool toMute = get_boolean(stMsgData->paramValue);

    try {
        aPort.setMuted(toMute);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Get Audio Mute Status
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> 1: Audio output set to mute
                                        -> 0: Audio output not set to mute
************************************************************/

int hostIf_STBServiceAudioInterface::getCancelMute(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    //RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    try {
        bool getMute = 0;
        getMute = aPort.isMuted();

        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] getMute = %d\n", __FUNCTION__, getMute);
        put_boolean(stMsgData->paramValue,getMute);
        stMsgData->paramtype=hostIf_BooleanType;
        stMsgData->paramLen = sizeof(bool);
        if(bCalledCancelMute && pChanged && (backupCancelMute != get_boolean(stMsgData->paramValue)))
        {
            *pChanged = true;
        }
        bCalledCancelMute = true;
        backupCancelMute = get_boolean(stMsgData->paramValue);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] [Value: %s] \n", __FUNCTION__, stMsgData->paramValue);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }
    //RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

/************************************************************
 * Description  : Get the enabled status for the audio port.
 * Precondition : None
 * Input        : stMsgData for result return.

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> true
************************************************************/

int hostIf_STBServiceAudioInterface::getEnable(HOSTIF_MsgData_t *stMsgData)
{
    put_boolean(stMsgData->paramValue,true);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);
    return OK;
}

/************************************************************
 * Description  : Get the human readable name for the audio device.
 * Precondition : None
 * Input        : stMsgData for result return.

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> name string
************************************************************/

int hostIf_STBServiceAudioInterface::getName(HOSTIF_MsgData_t *stMsgData)
{
    try {
        snprintf(stMsgData->paramValue, PARAM_LEN, "AudioOutputPort%s%d", aPort.getName().c_str(), dev_id);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }
    return OK;
}

/************************************************************
 * Description  : Set Stereo mode used in a given audio port
 * Precondition : None
 * Input        : stMsgData->paramValue -> 0-3
                                        -> 0: Unknown
                                        -> 1: Mono
                                        -> 2: Stereo
                                        -> 3: Surround

 * Return       : OK -> Success
                  NOK -> Failure
************************************************************/

int hostIf_STBServiceAudioInterface::setX_COMCAST_COM_AudioStereoMode(const HOSTIF_MsgData_t *stMsgData)
{
    int mode = get_int(stMsgData->paramValue);

    try {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] Value: %s \n",__FUNCTION__, stMsgData->paramValue);
        aPort.setStereoMode(mode);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Get Stereo mode used in a given audio port
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> 0: Unknown
                                        -> 1: Mono
                                        -> 2: Stereo
                                        -> 3: Surround
************************************************************/

int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioStereoMode(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        int stereoMode = 0;

        stereoMode =  aPort.getStereoMode().getId();
       
        switch(stereoMode)
        {
        case dsAUDIO_STEREO_UNKNOWN:
            strncpy(stMsgData->paramValue, "Unknown", PARAM_LEN);
            break;
        case dsAUDIO_STEREO_MONO:
            strncpy(stMsgData->paramValue, "Mono", PARAM_LEN);
            break;
        case dsAUDIO_STEREO_STEREO:
            strncpy(stMsgData->paramValue, "Stereo", PARAM_LEN);
            break;
        case dsAUDIO_STEREO_SURROUND:
            strncpy(stMsgData->paramValue, "Surround", PARAM_LEN);
            break;
        case dsAUDIO_STEREO_PASSTHRU:
            strncpy(stMsgData->paramValue, "Expertmode", PARAM_LEN);
            break;
        default:
            return NOK;
        }
        stMsgData->paramValue[PARAM_LEN-1] = '\0';
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] Value:%s\n",__FUNCTION__, stMsgData->paramValue);
        if(bCalledAudioStereoMode && pChanged && strcmp(backupAudioStereoMode, stMsgData->paramValue))
        {
            *pChanged = true;
        }
        bCalledAudioStereoMode = true;
        strncpy(backupAudioStereoMode, stMsgData->paramValue, sizeof(backupAudioStereoMode) -1);
        backupAudioStereoMode[sizeof(backupAudioStereoMode) -1] = '\0';
        //CID:44081 - OVERRUN
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Set Audio Encoding
 * Precondition : None
 * Input        : stMsgData->paramValue -> 0-3
                                           0: None
                                           1: Display
                                           2: PCM
                                           3: AC3
 * Return       : OK -> Success
                  NOK -> Failure
************************************************************/

int hostIf_STBServiceAudioInterface::setAudioEncoding(const HOSTIF_MsgData_t *stMsgData)
{
    int newEncoding = atoi((const char *)stMsgData->paramValue);
    if ((newEncoding < device::AudioEncoding::kNone) || (newEncoding >= device::AudioEncoding::kMax))
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Invalid value to setAudioEncoding: %d\n",__FUNCTION__, newEncoding);
        return NOK;
    }

    try {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] In setAudioEncoding(): Value: %s \n",__FUNCTION__, stMsgData->paramValue);
        aPort.setEncoding(newEncoding);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Get Audio Encoding
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> the audio encoding
                                           0: None
                                           1: Display
                                           2: PCM
                                           3: AC3
                                           4: Max
************************************************************/

int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioEncoding(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        switch(aPort.getEncoding().getId())
        {
        case dsAUDIO_ENC_NONE:
            strncpy(stMsgData->paramValue, "None", _BUF_LEN_16);
            break;
        case dsAUDIO_ENC_DISPLAY:
            strncpy(stMsgData->paramValue, "Display", _BUF_LEN_16);
            break;
        case dsAUDIO_ENC_PCM:
            strncpy(stMsgData->paramValue, "PCM", _BUF_LEN_16);
            break;
        case dsAUDIO_ENC_AC3:
            strncpy(stMsgData->paramValue, "AC3", _BUF_LEN_16);
            break;
        default:
            return NOK;
        }
        stMsgData->paramValue[_BUF_LEN_16-1] = '\0';
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        if(bCalledAudioEncoding && pChanged && strcmp(backupAudioEncoding, stMsgData->paramValue))
        {
            *pChanged = true;
        }
        bCalledAudioEncoding = true;
        strncpy(backupAudioEncoding,stMsgData->paramValue, _BUF_LEN_16);
        backupAudioEncoding[_BUF_LEN_16-1] = '\0';
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] stMsgData->paramValue [%s]\n",__FUNCTION__, stMsgData->paramValue);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception: %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Get Audio Format
 * Precondition : None
 * Input        : stMsgData for result return.
                  

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> The Audio Format
                                           0: None
                                           1: PCM
                                           2: AC3
                                           3: Max
************************************************************/
int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioFormat(HOSTIF_MsgData_t *stMsgData)
{
    try {
        switch(aPort.getEncoding().getId())
        {
            case dsAUDIO_ENC_NONE:
                strncpy(stMsgData->paramValue, "None", PARAM_LEN);
                break;

            case dsAUDIO_ENC_DISPLAY:
                /* Platform-selected digital audio encoding format. */
                strncpy(stMsgData->paramValue, "Other", PARAM_LEN);
                break;

            case dsAUDIO_ENC_PCM:
                strncpy(stMsgData->paramValue, "PCM", PARAM_LEN);
                break;

            case dsAUDIO_ENC_AC3:
                strncpy(stMsgData->paramValue, "AC3", PARAM_LEN);
                break;

            case dsAUDIO_ENC_EAC3:
                strncpy(stMsgData->paramValue, "EAC3", PARAM_LEN);
                break;

            default:
                return NOK;
        }

        stMsgData->paramValue[PARAM_LEN-1] = '\0';
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
                
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] stMsgData->paramValue [%s]\n",__FUNCTION__, stMsgData->paramValue);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception: %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }

    return OK;
}


/************************************************************
 * Description  : Set Audio Compression
 * Precondition : None
 * Precondition : None
 * Input        : stMsgData->paramValue -> 0-10

 * Return       : OK -> Success
                  NOK -> Failure
************************************************************/

int hostIf_STBServiceAudioInterface::setX_COMCAST_COM_AudioCompression(const HOSTIF_MsgData_t *stMsgData)
{
    int newComp = get_int(stMsgData->paramValue);
    if (newComp < 0 || newComp > 10)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed due to Invalid value: %d\n",__FUNCTION__, newComp);
        return NOK;
    }

    try {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] Value: %s \n",__FUNCTION__, stMsgData->paramValue);
        aPort.setCompression(newComp);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Get Audio Compression
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> the audio compression
************************************************************/

int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioCompression(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        int compression = aPort.getCompression();

        put_int(stMsgData->paramValue, compression);
        stMsgData->paramtype = hostIf_UnsignedIntType;
        stMsgData->paramLen = sizeof(unsigned int);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] \n",__FUNCTION__, stMsgData->paramValue);
        if(bCalledAudioCompression && pChanged && (backupAudioCompression != get_int(stMsgData->paramValue)))
        {
            *pChanged = true;
        }
        bCalledAudioCompression = true;
        backupAudioCompression = get_int(stMsgData->paramValue);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception: %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Set Audio Level
 * Precondition : None
 * Input        : stMsgData->paramValue -> 0-100

 * Return       : OK -> Success
                  NOK -> Failure
************************************************************/
int hostIf_STBServiceAudioInterface::setAudioLevel(const HOSTIF_MsgData_t *stMsgData)
{

    float newLevel = (float)get_int(stMsgData->paramValue);
    if (newLevel < 0.0 || newLevel > 100.0)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed due to invalid value : %f\n",__FUNCTION__, newLevel);
        return NOK;
    }

    try {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d]: Value=%f \n",__FUNCTION__, __FILE__, __LINE__, newLevel);
        aPort.setLevel(newLevel);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s:%s:%d]: Exception caught\r\n",__FUNCTION__, __FILE__, __LINE__);
        return NOK;
    }

    return OK;
}


/************************************************************
 * Description  : Get Audio Level
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> the audio level
************************************************************/

int hostIf_STBServiceAudioInterface::getAudioLevel(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        float level = aPort.getLevel();
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d]: getAudioLevel=%f, muted=%s, getOptimalLevel=%f, DB=[%f, %f]:%f, getGain=%f\n", __FUNCTION__, __FILE__, __LINE__, level, aPort.isMuted()?"Muted":"Not Muted", aPort.getOptimalLevel(), aPort.getMinDB(), aPort.getMaxDB(), aPort.getDB(), aPort.getGain());
        put_int(stMsgData->paramValue, (int)(level + 0.5));
        stMsgData->paramtype=hostIf_UnsignedIntType;
        stMsgData->paramLen = sizeof(unsigned int);
        if(bCalledAudioLevel && pChanged && (backupAudioLevel != get_int(stMsgData->paramValue)))
        {
            *pChanged = true;
        }
        bCalledAudioLevel = true;
        backupAudioLevel = get_int(stMsgData->paramValue);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception: %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Set Audio DB
 * Precondition : None
 * Input        : stMsgData->paramValue -> ???

 * Return       : OK -> Success
                  NOK -> Failure
************************************************************/
int hostIf_STBServiceAudioInterface::setX_COMCAST_COM_AudioDB(const HOSTIF_MsgData_t *stMsgData)
{

    float newDb = (float)get_int(stMsgData->paramValue);

    try {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] : Value: %s \n",__FUNCTION__, stMsgData->paramValue);
        aPort.setDB(newDb);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Get Audio DB
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> the audio decibel (DB) level
************************************************************/

int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioDB(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        float dbValue = 0.0;
        dbValue = aPort.getDB();

        snprintf(stMsgData->paramValue, PARAM_LEN, "%f", dbValue);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] \n",__FUNCTION__, stMsgData->paramValue);
        if(bCalledAudioDB && pChanged && strcmp(backupAudioDB, stMsgData->paramValue))
        {
            *pChanged = true;
        }
        bCalledAudioDB = true;
        strncpy(backupAudioDB, stMsgData->paramValue, sizeof(backupAudioDB) -1);
        backupAudioDB[sizeof(backupAudioDB) -1] = '\0';
         //CID:44080 - OVERRUN
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Get Minimum DB that can be supported on audio port
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> the audio decibel (DB) level
************************************************************/

int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_MinAudioDB(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        float minDbValue = 0.0;
        minDbValue = aPort.getMinDB();

        snprintf(stMsgData->paramValue, PARAM_LEN, "%f", minDbValue);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] \n",__FUNCTION__, stMsgData->paramValue);
        if(bCalledMinAudioDB && pChanged && strcmp(backupMinAudioDB, stMsgData->paramValue))
        {
            *pChanged = true;
        }
        bCalledMinAudioDB = true;
        strncpy(backupMinAudioDB, stMsgData->paramValue, _BUF_LEN_16-1);
        backupMinAudioDB[_BUF_LEN_16-1] = '\0';
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Get Maximum DB that can be supported on audio port
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> the audio decibel (DB) level
************************************************************/

int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_MaxAudioDB(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        float maxDbValue = 0.0;
        maxDbValue = aPort.getMaxDB();

        snprintf(stMsgData->paramValue, PARAM_LEN, "%f", maxDbValue);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] \n",__FUNCTION__, stMsgData->paramValue);
        if(bCalledMaxAudioDB && pChanged && strcmp(backupMaxAudioDB, stMsgData->paramValue))
        {
            *pChanged = true;
        }
        bCalledMaxAudioDB = true;
        strncpy(backupMaxAudioDB, stMsgData->paramValue, _BUF_LEN_16-1);
        backupMaxAudioDB[_BUF_LEN_16-1] = '\0';
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Get Audio Gain
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> the audio gain
************************************************************/

int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioGain(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        float gainValue = 0.0;
        gainValue = aPort.getGain();

        snprintf(stMsgData->paramValue, PARAM_LEN, "%f", gainValue);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] \n",__FUNCTION__, stMsgData->paramValue);
        if(bCalledAudioGain && pChanged && strcmp(backupAudioGain, stMsgData->paramValue))
        {
            *pChanged = true;
        }
        bCalledAudioGain = true;
        strncpy(backupAudioGain,stMsgData->paramValue, _BUF_LEN_16-1);
        backupAudioGain[_BUF_LEN_16-1] = '\0';
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Sets a given audio port for loopthru
 * Precondition : None
 * Input        : stMsgData->paramValue -> 1: Set audio port for loopthru
                                        -> 0: Unset audio port for loopthru
 * Return       : OK -> Success
                  NOK -> Failure
************************************************************/

int hostIf_STBServiceAudioInterface::setX_COMCAST_COM_AudioLoopThru(const HOSTIF_MsgData_t *stMsgData)
{
    bool loopThru = get_boolean(stMsgData->paramValue);

    try {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] Value: %s \n",__FUNCTION__, stMsgData->paramValue);
            aPort.setLoopThru(loopThru);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Informs if a given audio port is set for loopthru
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> 1 : audio port set for loopthru
                                           0 : audio port not set for loopthru

************************************************************/

int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioLoopThru(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        bool isLoopThru = 0;
        isLoopThru = aPort.isLoopThru();

        snprintf(stMsgData->paramValue, PARAM_LEN, "%d", isLoopThru);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        if(bCalledAudioLoopThru && pChanged && strcmp(backupAudioLoopThru, stMsgData->paramValue))
        {
            *pChanged = true;
        }
        bCalledAudioLoopThru = true;
        strncpy(backupAudioLoopThru, stMsgData->paramValue, sizeof(backupAudioLoopThru) -1);
        backupAudioLoopThru[sizeof(backupAudioLoopThru) -1] = '\0';
         //CID:44083 - OVERRUN
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Get Audio Optimal audio level
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> the audio level
************************************************************/

int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_AudioOptimalLevel(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        float optimalLevel = 0.0;
        optimalLevel = aPort.getOptimalLevel();

        snprintf(stMsgData->paramValue, PARAM_LEN, "%f", optimalLevel);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        if(bCalledAudioOptimalLevel && pChanged && strcmp(backupAudioOptimalLevel, stMsgData->paramValue))
        {
            *pChanged = true;
        }
        bCalledAudioOptimalLevel = true;
        strncpy(backupAudioOptimalLevel, stMsgData->paramValue, _BUF_LEN_16-1);
        backupAudioOptimalLevel[_BUF_LEN_16-1] = '\0';
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Set Dialog Enhancement Level
 * Precondition : None
 * Precondition : None
 * Input        : stMsgData->paramValue -> 0-16

 * Return       : OK -> Success
                  NOK -> Failure
************************************************************/

int hostIf_STBServiceAudioInterface::setX_COMCAST_COM_DialogEnhancement(const HOSTIF_MsgData_t *stMsgData)
{
    int newLevel = get_int(stMsgData->paramValue);

    if (newLevel < 0 || newLevel > 16)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed due to invalid value : %d\n",__FUNCTION__, newLevel);
        return NOK;
    }

    try {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] Value: %s \n",__FUNCTION__, stMsgData->paramValue);
        aPort.setDialogEnhancement(newLevel);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Get Dialog Enhancement Level
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> the dialog enhancement level
************************************************************/

int hostIf_STBServiceAudioInterface::getX_COMCAST_COM_DialogEnhancement(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        int enhancementLevel = 0;
        enhancementLevel = aPort.getDialogEnhancement();

        put_int(stMsgData->paramValue, enhancementLevel);
        stMsgData->paramtype = hostIf_UnsignedIntType;
        stMsgData->paramLen = sizeof(unsigned int);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] \n",__FUNCTION__, stMsgData->paramValue);
        if(bCalledDialogEnhancement && pChanged && (backupDialogEnhancement != get_int(stMsgData->paramValue)))
        {
            *pChanged = true;
        }
        bCalledDialogEnhancement = true;
        backupDialogEnhancement = get_int(stMsgData->paramValue);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/** @} */
/** @} */
