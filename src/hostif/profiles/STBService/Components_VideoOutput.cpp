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
 * @file Components_VideoOutput.cpp
 * @brief This source file contains the APIs of TR069 Components VideoOutput.
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
#include "Components_VideoOutput.h"

#define DEV_NAME "VideoOutput"
#define BASE_NAME "Device.Services.STBService.1.Components.VideoOutput"
#define UPDATE_FORMAT_STRING "%s.%d.%s"

#define STATUS_STRING "Status"
#define ENABLE_STRING "Enable"
#define DISPLAY_FORMAT_STRING "DisplayFormat"
#define VIDEO_FORMAT_STRING "VideoFormat"
#define AR_BEHAVIOR_STRING "AspectRatioBehaviour"
#define HDCP_STRING "HDCP"
#define ENABLED_STRING "Enabled"
#define DISABLED_STRING "Disabled"

GHashTable * hostIf_STBServiceVideoOutput::ifHash = NULL;
GMutex * hostIf_STBServiceVideoOutput::m_mutex = NULL;

hostIf_STBServiceVideoOutput* hostIf_STBServiceVideoOutput::getInstance(int dev_id)
{
    hostIf_STBServiceVideoOutput* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_STBServiceVideoOutput *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            // TODO: We only create 1 videooutput currently. Fix this to return a count from ds.
            pRet = new hostIf_STBServiceVideoOutput(dev_id, device::Host::getInstance().getVideoOutputPort(std::string("HDMI").append(int_to_string(dev_id-1))));
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

GList* hostIf_STBServiceVideoOutput::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_STBServiceVideoOutput::closeInstance(hostIf_STBServiceVideoOutput *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_STBServiceVideoOutput::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_STBServiceVideoOutput* pDev = (hostIf_STBServiceVideoOutput *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

void hostIf_STBServiceVideoOutput::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void hostIf_STBServiceVideoOutput::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

/**
 * @brief Class Constructor of the class hostIf_STBServiceVideoOutput. 
 *
 * It will initialize the device id and video output port.
 *
 * @param[in] devid Identification number of the device.
 * @param[in] port Video output port number.
 */
hostIf_STBServiceVideoOutput::hostIf_STBServiceVideoOutput(int devid, device::VideoOutputPort& port) : dev_id(devid), vPort(port)
{
    strcpy(backupAspectRatioBehaviour," ");
    strcpy(backupDisplayFormat," ");
    strcpy(backupVideoFormat," ");
    backupHDCP = false;
    strcpy(backupVideoOutputStatus," ");

    bCalledAspectRatioBehaviour = false;
    bCalledDisplayFormat = false;
    bCalledVideoFormat = false;
    bCalledHDCP = false;
    bCalledVideoOutputStatus = false;

}

/**
 * @brief This function set the video interface attribute value such as status, display format,
 * video format, aspect ratio and HDPC(High-Bandwidth Digital Content Protection) in the connected
 * video port etc.. Currently not implemented.
 *
 * @param[in] paramName  Video service name string.
 * @param[in] stMsgData  HostIf Message Request param contains the video attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the hostIf video interface attribute.
 * @retval -1 If Not able to set the hostIf video interface attribute.
 * @retval -2 If Not handle the hostIf video interface attribute.
 * @ingroup TR69_HOSTIF_STBSERVICES_VIDEOOUTPUT_API
 */
int hostIf_STBServiceVideoOutput::handleSetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    return ret;
}

/**
 * @brief This function get the video interface attribute value such as status, display format,
 * video format, aspect ratio and HDPC(High-Bandwidth Digital Content Protection) in the connected
 * video port etc..
 *
 * @param[in] paramName  Video service name.
 * @param[in] stMsgData  HostIf Message Request param which contains the video attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hostIf video interface attribute.
 * @retval -1 If Not able to get the hostIf video interface attribute.
 * @retval -2 If Not handle the hostIf video interface attribute.
 * @ingroup TR69_HOSTIF_STBSERVICES_VIDEOOUTPUT_API
 */
int hostIf_STBServiceVideoOutput::handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if(strcasecmp(paramName, STATUS_STRING) == 0)
    {
        ret = getStatus(stMsgData);
    }
    else if(strcasecmp(paramName, ENABLE_STRING) == 0)
    {
        put_boolean(stMsgData->paramValue,true);
        stMsgData->paramtype = hostIf_BooleanType;
        stMsgData->paramLen = sizeof(bool);
        ret = OK;
    }
    else if(strcasecmp(paramName, DISPLAY_FORMAT_STRING) == 0)
    {
        ret = getDisplayFormat(stMsgData);
    }
    else if(strcasecmp(paramName, VIDEO_FORMAT_STRING) == 0)
    {
        ret = getVideoFormat(stMsgData);
    }
    else if(strcasecmp(paramName, AR_BEHAVIOR_STRING) == 0)
    {
        ret = getAspectRatioBehaviour(stMsgData);
    }
    else if(strcasecmp(paramName, HDCP_STRING) == 0)
    {
        ret = getHDCP(stMsgData);
    }

    return ret;
}

/**
 * @brief This function updates the video interface such as status, display format, video format,
 * aspect ratio and HDPC (High-Bandwidth Digital Content Protection) in a connected video port using
 * callback mechanism.
 *
 * @param[in] mUpdateCallback  Callback function which updates the hostIf video interface.
 * @ingroup TR69_HOSTIF_STBSERVICES_VIDEOOUTPUT_API
 */
void hostIf_STBServiceVideoOutput::doUpdates(updateCallback mUpdateCallback)
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    char tmp_buff[PARAM_LEN];

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
    getDisplayFormat(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, DISPLAY_FORMAT_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getVideoFormat(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, VIDEO_FORMAT_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getAspectRatioBehaviour(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, AR_BEHAVIOR_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getHDCP(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, HDCP_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
}

// Impl of accessors below.

// This is all coded to use the HDMI port, but I think there's incorrect assumptions being made here...

/************************************************************
 * Description  : Get VideoOutputPort Enable / Disable status
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> "Enabled", "Disabled"
************************************************************/
int hostIf_STBServiceVideoOutput::getStatus(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        
        /*
            Following logic 
            - If HDMI not connected - Disabled. 
            - If HDMI connected and Port power is off - Disabled. 
            - If HDMI connected and if Port power is ON and HDCP disabled - Disabled. 
            - If HDMI connected and if Port power is ON and HDCP enabled - Enabled.  
        */
        if (true == vPort.isDisplayConnected())
        {
            //g_printf("[%s] In getHDMIEnable(): vPort.isEnabled(): %d \n",__FUNCTION__, vPort.isEnabled());
            if (vPort.isEnabled())
                strncpy(stMsgData->paramValue, ENABLED_STRING, PARAM_LEN);
            else
                strncpy(stMsgData->paramValue, DISABLED_STRING, PARAM_LEN);

        }
        else
        {
            memset(stMsgData->paramValue, '\0', sizeof (stMsgData->paramValue));
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] HDMI Display is NOT connected\r\n",__FUNCTION__);
            strncpy(stMsgData->paramValue, DISABLED_STRING, PARAM_LEN);
        }
        stMsgData->paramValue[PARAM_LEN-1] = '\0';
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] HDCP Status is [%s]\r\n",__FUNCTION__,stMsgData->paramValue);
        if(bCalledVideoOutputStatus && pChanged && strcmp(backupVideoOutputStatus, stMsgData->paramValue))
        {
            *pChanged = true;
        }
        bCalledVideoOutputStatus = true;
        strncpy(backupVideoOutputStatus,stMsgData->paramValue,_BUF_LEN_16-1);
        backupVideoOutputStatus[_BUF_LEN_16-1] = '\0';
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Get VideoOutput AspectRatioBehaviour value
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                stMsgData->paramValue -> AspectRatioBehaviour

************************************************************/
int hostIf_STBServiceVideoOutput::getAspectRatioBehaviour(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    // TODO: Decoder?  Is this impl correct?
    try {
        device::VideoDevice decoder = device::Host::getInstance().getVideoDevices().at(0);
        strncpy(stMsgData->paramValue, decoder.getDFC().getName().c_str(), PARAM_LEN);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] : Value: %s \n",__FUNCTION__, stMsgData->paramValue);
        stMsgData->paramValue[PARAM_LEN-1] = '\0';
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        if(bCalledAspectRatioBehaviour && pChanged && strncmp(backupAspectRatioBehaviour,stMsgData->paramValue,_BUF_LEN_16-1))
        {
            *pChanged = true;
        }
        bCalledAspectRatioBehaviour = true;
        strncpy(backupAspectRatioBehaviour,stMsgData->paramValue,_BUF_LEN_16-1);
        backupAspectRatioBehaviour[_BUF_LEN_16-1] = '\0';
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Get the currently active video output format.
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> HDMI(XI3 by default, it is HDMI)
               DVI.
TODO:  Need correct implementation.  Here's what TR-135 says:
   Comma-separated list of strings. Each entry is a supported display format and
   MUST be in the form of “x:y”, such as for example “4:3, 16:9, 14:9".
************************************************************/
int hostIf_STBServiceVideoOutput::getVideoFormat(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        /*By default, XI3 only supports HDMI */
        snprintf(stMsgData->paramValue, PARAM_LEN, "%s","HDMI");
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] : Value: %s \n",__FUNCTION__, stMsgData->paramValue);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        if(bCalledVideoFormat && pChanged && strcmp(backupVideoFormat, stMsgData->paramValue))
        {
            *pChanged = true;
        }
        bCalledVideoFormat = true;
        strncpy(backupVideoFormat,stMsgData->paramValue,_BUF_LEN_16-1);
        backupVideoFormat[_BUF_LEN_16-1] = '\0';
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}


/************************************************************
 //TODO
 * Description  : Get VideoOutput Display format
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                stMsgData->paramValue -> Display format string.

************************************************************/
int hostIf_STBServiceVideoOutput::getDisplayFormat(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        sprintf(stMsgData->paramValue,"%s",vPort.getResolution().getName().c_str());
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] : Value: %s \n",__FUNCTION__, stMsgData->paramValue);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        if(bCalledDisplayFormat && pChanged && strcmp(backupDisplayFormat, stMsgData->paramValue))
        {
            *pChanged = true;
        }
        bCalledDisplayFormat = true;
        strncpy(backupDisplayFormat,stMsgData->paramValue,_BUF_LEN_16-1);
        backupDisplayFormat[_BUF_LEN_16-1] = '\0';
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Get if HDCP is being used in this Video Output or not.
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                stMsgData->paramValue -> 1 : HDCP Supported
                                         0 : HDCP Not Supported.
************************************************************/
int hostIf_STBServiceVideoOutput::getHDCP(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    int hdpcStatus = 0;
    bool isHDCPSupported = false;
    try {
        if(vPort.isDisplayConnected())
        {
            hdpcStatus = vPort.getHDCPStatus();
            isHDCPSupported = (hdpcStatus == dsHDCP_STATUS_AUTHENTICATED)?true:false;
        }

        put_boolean(stMsgData->paramValue, isHDCPSupported);

        stMsgData->paramtype = hostIf_BooleanType;
        stMsgData->paramLen = sizeof(bool);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] Value: %s \n",__FUNCTION__, stMsgData->paramValue);
        if(bCalledHDCP && pChanged && (backupHDCP != get_boolean(stMsgData->paramValue)))
        {
            *pChanged = true;
        }
        bCalledHDCP = true;
        backupHDCP = get_boolean(stMsgData->paramValue);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}




/** @} */
/** @} */
