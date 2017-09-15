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
 * @file Components_VideoDecoder.cpp
 * @brief This source file contains the APIs of TR069 Components Video decoder.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#include "dsTypes.h"
#include "Components_VideoDecoder.h"

#define BASE_NAME "Device.Services.STBService.1.Components.VideoDecoder"
#define UPDATE_FORMAT_STRING "%s.%d.%s"

#define STATUS_STRING "Status"
#define CONTENT_AR_STRING "ContentAspectRatio"
#define COMCAST_STANDBY_STRING "X_COMCAST-COM_Standby"
#define HEVC_STRING "X_RDKCENTRAL-COM_MPEGHPart2"
#define HEVC_PROFILE_PATH ".Capabilities.VideoDecoder.X_RDKCENTRAL-COM_MPEGHPart2.ProfileLevel.1"
#define NAME_STRING "Name"

#define ENABLE_STRING "Enable"
#define ENABLED_STRING "Enabled"
#define DISABLED_STRING "Disabled"
#define ERROR_STRING "Error"

GHashTable * hostIf_STBServiceVideoDecoder::ifHash = NULL;
GMutex * hostIf_STBServiceVideoDecoder::m_mutex = NULL;

hostIf_STBServiceVideoDecoder* hostIf_STBServiceVideoDecoder::getInstance(int dev_id)
{
    hostIf_STBServiceVideoDecoder* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_STBServiceVideoDecoder *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_STBServiceVideoDecoder(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create STB service Video Interface instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_STBServiceVideoDecoder::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_STBServiceVideoDecoder::closeInstance(hostIf_STBServiceVideoDecoder *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_STBServiceVideoDecoder::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_STBServiceVideoDecoder* pDev = (hostIf_STBServiceVideoDecoder *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

void hostIf_STBServiceVideoDecoder::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void hostIf_STBServiceVideoDecoder::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

/**
 * @brief Class Constructor of the class hostIf_STBServiceVideoDecoder.
 *
 * It will initialize the device id.
 *
 * @param[in] devid Identification number of the device.
 */
hostIf_STBServiceVideoDecoder::hostIf_STBServiceVideoDecoder(int devid)
{
    dev_id = devid;

    strcpy(backupContentAspectRatio," ");
    backupStandby = false;
    strcpy(backupVideoDecoderStatus," ");

    bCalledContentAspectRatio = false;
    bCalledStandby = false;
    bCalledVideoDecoderStatus = false;
}

/**
 * @brief This function sets the video decoder interface updates such as Status,
 * ContentAspectRatio, X_COMCAST-COM_Standby, Name in a connected video decoder.
 * Currently X_COMCAST-COM_Standby is handled.
 *
 * @param[in] paramName  Video decoder service name string.
 * @param[in] stMsgData  HostIf Message Request param contains the video decoder attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the hostIf video decoder interface attribute.
 * @retval -1 If Not able to set the hostIf video decoder interface attribute.
 * @retval -2 If Not handle the hostIf video decoder interface attribute.
 * @ingroup TR69_HOSTIF_STBSERVICES_VIDEODECODER_API
 */
int hostIf_STBServiceVideoDecoder::handleSetMsg(const char *pSetting, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if (strcasecmp(pSetting, COMCAST_STANDBY_STRING) == 0)
    {
        ret = setX_COMCAST_COM_Standby(stMsgData);
    }
    return ret;
}

/**
 * @brief This function get the video decoder interface updates such as Status,
 * ContentAspectRatio, X_COMCAST-COM_Standby, Name in a connected video decoder.
 *
 * @param[in] paramName  Video decoder service name string.
 * @param[in] stMsgData  HostIf Message Request param contains the video decoder attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hostIf video decoder interface attribute.
 * @retval -1 If Not able to get the hostIf video decoder interface attribute.
 * @retval -2 If Not handle the hostIf video decoder interface attribute.
 * @ingroup TR69_HOSTIF_STBSERVICES_VIDEODECODER_API
 */
int hostIf_STBServiceVideoDecoder::handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if(strcasecmp(paramName, COMCAST_STANDBY_STRING) == 0)
    {
        ret = getX_COMCAST_COM_Standby(stMsgData);
    }
    else if(strcasecmp(paramName, ENABLE_STRING) == 0)
    {
        put_boolean(stMsgData->paramValue,true);
        stMsgData->paramtype = hostIf_BooleanType;
        stMsgData->paramLen = sizeof(bool);
        ret = OK;
    }
    else if(strcasecmp(paramName, STATUS_STRING) == 0)
    {
        ret = getStatus(stMsgData);
    }
    else if(strcasecmp(paramName, NAME_STRING) == 0)
    {
        strncpy(stMsgData->paramValue,"VideoDecoderHDMI0", strlen("VideoDecoderHDMI0")+1);  // Questionable relationship.
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        ret = OK;
    }
    else if(strcasecmp(paramName, CONTENT_AR_STRING) == 0)
    {
        ret = getContentAspectRatio(stMsgData);
    }
    else if(strcasecmp(paramName, HEVC_STRING) == 0)
    {
        strncpy(stMsgData->paramValue, HEVC_PROFILE_PATH, strlen(HEVC_PROFILE_PATH)+1);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        ret = OK;
    }
    return ret;
}

/**
 * @brief This function updates the video decoder interface updates such as
 * Status, ContentAspectRatio, X_COMCAST-COM_Standby, Name in a connected video decoder.
 *
 * @param[in] mUpdateCallback  Callback function which updates the hostIf video decoder interface.
 * @ingroup TR69_HOSTIF_STBSERVICES_VIDEODECODER_API
 */
void hostIf_STBServiceVideoDecoder::doUpdates(updateCallback mUpdateCallback)
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    char tmp_buff[PARAM_LEN];

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getX_COMCAST_COM_Standby(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, COMCAST_STANDBY_STRING);
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
    getContentAspectRatio(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, CONTENT_AR_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
}

// Impl of accessors below.

/************************************************************
 * Description  : Get VideoDecoder ContentAspectRatio
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> "4x3"
                   "16x9"

// TODO:  This is implemented incorrectly, in that it returns the AR of the attached display, and not the Content.

************************************************************/

int hostIf_STBServiceVideoDecoder::getContentAspectRatio(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        device::VideoOutputPort vPort = device::Host::getInstance().getVideoOutputPort(std::string("HDMI").append(int_to_string(stMsgData->instanceNum-1)));
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] vPort.isDisplayConnected(): %d \n",__FUNCTION__, vPort.isDisplayConnected());

        if (true == vPort.isDisplayConnected()) {
            const device::VideoOutputPort::Display disp = vPort.getDisplay();
            const device::AspectRatio aspect_ratio = disp.getAspectRatio();
            const std::string str = aspect_ratio.getName();
            strncpy(stMsgData->paramValue, str.c_str(),str.length());
        }
        else {
            memset(stMsgData->paramValue, '\0', sizeof (stMsgData->paramValue));
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] %s Display is NOT connected, vPort.isDisplayConnected() %d \r\n",__FUNCTION__, "HDMI", vPort.isDisplayConnected());
        }
        stMsgData->paramValue[PARAM_LEN-1] = '\0';
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        if(bCalledContentAspectRatio && pChanged && strcmp(backupContentAspectRatio,stMsgData->paramValue))
        {
            *pChanged = true;
        }
        bCalledContentAspectRatio = true;
        strncpy(backupContentAspectRatio,stMsgData->paramValue,PARAM_LEN);
        backupContentAspectRatio[PARAM_LEN-1] = '\0';
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] In getVideoDecoderContentAspectRatio(): Value: %s \n",__FUNCTION__, stMsgData->paramValue);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Get if Video decoder is in Standby or not.
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                stMsgData->paramValue -> 1 : Decoder in Standby
                                         2 : Decoder not in Standby
************************************************************/

int hostIf_STBServiceVideoDecoder::getX_COMCAST_COM_Standby(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        int iPowerMode =  device::Host::getInstance().getPowerMode();

        /*If the decoder in Standby mode, display true.*/
        /* If the decoder in PowerON mode, display false */
        if(iPowerMode == dsPOWER_STANDBY)
            put_boolean(stMsgData->paramValue, true);
        else
            put_boolean(stMsgData->paramValue, false);

        stMsgData->paramtype=hostIf_BooleanType;
        stMsgData->paramLen = sizeof(bool);
        if(bCalledStandby && pChanged && (backupStandby != get_boolean(stMsgData->paramValue)))
        {
            *pChanged = true;
        }
        bCalledStandby = true;
        backupStandby = get_boolean(stMsgData->paramValue);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] PowerMode: [%d , %s] \n", __FUNCTION__, iPowerMode, stMsgData->paramValue);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}
/************************************************************
 * Description  : Get if Video decoder is in Standby or not.
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                stMsgData->paramValue -> "Enabled", "Disabled", "Error", "X_COMCAST-COM_Standby"
************************************************************/

int hostIf_STBServiceVideoDecoder::getStatus(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        int iPowerMode =  device::Host::getInstance().getPowerMode();
        switch(iPowerMode)
        {
            case dsPOWER_ON:
                strncpy(stMsgData->paramValue, ENABLED_STRING, strlen(ENABLED_STRING)+1);
                break;
            case dsPOWER_STANDBY:
                strncpy(stMsgData->paramValue, COMCAST_STANDBY_STRING, strlen(COMCAST_STANDBY_STRING)+1);
                break;
            case dsPOWER_OFF:
                strncpy(stMsgData->paramValue, DISABLED_STRING, strlen(DISABLED_STRING)+1);
                break;
            default:
                strncpy(stMsgData->paramValue, ERROR_STRING, strlen(ERROR_STRING)+1);
        }
        stMsgData->paramValue[PARAM_LEN-1] = '\0';
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        if(bCalledVideoDecoderStatus && pChanged && strcmp(backupVideoDecoderStatus, stMsgData->paramValue))
        {
            *pChanged = true;
        }
        bCalledVideoDecoderStatus = true;
        strncpy(backupVideoDecoderStatus,stMsgData->paramValue,PARAM_LEN);
        backupVideoDecoderStatus[PARAM_LEN-1] = '\0';
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] PowerMode: [%d , %s] \n", __FUNCTION__, iPowerMode, stMsgData->paramValue);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}
/************************************************************
 * Description  : Set Video decoder in Standby.
 * Precondition : None
 * Input        : stMsgData->paramValue -> 1: POWER_ON
                                           2: POWER_STANDBY
                                           3: POWER_OFF

 * Return       : OK -> Success
                  NOK -> Failure

************************************************************/
int hostIf_STBServiceVideoDecoder::setX_COMCAST_COM_Standby(const HOSTIF_MsgData_t *stMsgData)
{
    try
    {
        int iPowerMode;
        bool val = get_boolean(stMsgData->paramValue);

        if(val) {
            iPowerMode = dsPOWER_STANDBY;
        } else {
            iPowerMode = dsPOWER_ON;
        }

        if((iPowerMode < dsPOWER_ON) || (iPowerMode > dsPOWER_OFF))
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed to set [%s] due to Invalid input value [%d]\r\n",__FUNCTION__, stMsgData->paramName, val);
            return NOK;
        }
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] Get value as \'%d\' and Set as: \'%d\' \n", __FILE__, __FUNCTION__, get_boolean(stMsgData->paramValue), iPowerMode);
        device::Host::getInstance().setPowerMode(iPowerMode);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}



/** @} */
/** @} */
