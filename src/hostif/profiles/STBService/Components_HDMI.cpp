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
 * @file Components_HDMI.cpp
 * @brief This source file contains the APIs of TR069 Components HDMI.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#include "hostIf_main.h"
#include "videoOutputPort.hpp"
#include "illegalArgumentException.hpp"
#include "exception.hpp"
#include "Components_HDMI.h"
#include "Components_DisplayDevice.h"

#define DEV_NAME "HDMI"
#define BASE_NAME "Device.Services.STBService.1.Components.HDMI"
#define UPDATE_FORMAT_STRING "%s.%d.%s"


#define STATUS_STRING "Status"
#define ENABLE_STRING "Enable"
#define RES_MODE_STRING "ResolutionMode"
#define RES_VAL_STRING "ResolutionValue"
#define NAME_STRING "Name"
#define ENABLED_STRING "Enabled"
#define DISABLED_STRING "Disabled"


static EnumStringMapper dsVideoPixelResolutionMapper[] =
{
    { dsVIDEO_PIXELRES_720x480, "720x480"},
    { dsVIDEO_PIXELRES_720x576, "720x576"},
    { dsVIDEO_PIXELRES_1280x720, "1280x720"},
    { dsVIDEO_PIXELRES_1920x1080, "1920x1080"},
    { dsVIDEO_PIXELRES_3840x2160, "3840x2160"}
};

static EnumStringMapper dsVideoFrameRateMapper[] =
{
    {dsVIDEO_FRAMERATE_24, "24"},
    {dsVIDEO_FRAMERATE_25, "25"},
    {dsVIDEO_FRAMERATE_30, "30"},
    {dsVIDEO_FRAMERATE_60, "60"},
    {dsVIDEO_FRAMERATE_23dot98, "23.98"},
    {dsVIDEO_FRAMERATE_29dot97, "29.97"},
    {dsVIDEO_FRAMERATE_50, "50"},
    {dsVIDEO_FRAMERATE_59dot94, "59.94"}
};

char hostIf_STBServiceHDMI::dsHDMIResolutionMode[10] = HDMI_RESOLUTION_MODE_MANUAL;
GHashTable* hostIf_STBServiceHDMI::ifHash = NULL;
GMutex* hostIf_STBServiceHDMI::m_mutex = NULL;

/** Description: Counts the number of HDMI
 *               interfaces present in the device.
 *
 *  Get all the current interfaces in the system and
 *  count "STBService" interface from the list.
 *
 * Return:  Count value or '0' if error
 *
 */

hostIf_STBServiceHDMI* hostIf_STBServiceHDMI::getInstance(int dev_id)
{
    hostIf_STBServiceHDMI* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_STBServiceHDMI *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try 
        {
            pRet = new hostIf_STBServiceHDMI(dev_id, device::Host::getInstance().getVideoOutputPort(std::string("HDMI").append(int_to_string(dev_id-1))));
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

GList* hostIf_STBServiceHDMI::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_STBServiceHDMI::closeInstance(hostIf_STBServiceHDMI *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_STBServiceHDMI::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_STBServiceHDMI* pDev = (hostIf_STBServiceHDMI *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

void hostIf_STBServiceHDMI::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void hostIf_STBServiceHDMI::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

/**
 * @brief Class Constructor of the class hostIf_STBServiceHDMI.
 *
 * It will initialize the device id and video output port.
 *
 * @param[in] devid Identification number of the device.
 * @param[in] port Video output port number.
 */
hostIf_STBServiceHDMI::hostIf_STBServiceHDMI(int devid, device::VideoOutputPort& port) : dev_id(devid), vPort(port)
{
    backupEnable = false;
    strcpy(backupStatus, " ");
    strcpy(backupResolutionValue," ");
    strcpy(backupName," ");

    bCalledEnable = false;
    bCalledStatus = false;
    bCalledResolutionValue = false;
    bCalledName = false;

    displayDevice = new hostIf_STBServiceDisplayDevice(devid, vPort);

}

/**
 * @brief This function set the HDMI interface updates such as Status, Enable,
 * ResolutionMode, ResolutionValue etc in a connected HDMI port.
 *
 * @param[in] paramName  HDMI service name string.
 * @param[in] stMsgData  HostIf Message Request param contains the HDMI attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the hostIf HDMI interface attribute.
 * @retval -1 If Not able to set the hostIf HDMI interface attribute.
 * @retval -2 If Not handle the hostIf HDMI interface attribute.
 * @ingroup TR69_HOSTIF_STBSERVICES_HDMI_API
 */
int hostIf_STBServiceHDMI::handleSetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if(strcasecmp(paramName, RES_MODE_STRING) == 0)
    {
        ret = setHDMIResolutionMode(stMsgData->paramValue);
    }
    else if(strcasecmp(paramName, RES_VAL_STRING) == 0)
    {
        if(0 == strcasecmp(getHDMIResolutionMode(), HDMI_RESOLUTION_MODE_MANUAL))
        {
            ret = setResolution(stMsgData);
        }
        else
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Setting the ResolutionValue has no effect because the ResolutionMode is \"Auto\".\n");
            ret = NOK;
        }
    }
    else if(strcasecmp(paramName, ENABLE_STRING) == 0)
    {
        ret = setEnableVideoPort(stMsgData);
    }

    return ret;
}

/**
 * @brief This function get the HDMI interface updates such as Status, Enable,
 * ResolutionMode, ResolutionValue etc in a connected HDMI port.
 *
 * @param[in] paramName  HDMI service name string.
 * @param[in] stMsgData  HostIf Message Request param contains the HDMI attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the host IF HDMI interface attribute.
 * @retval -1 If Not able to get the host IF HDMI interface attribute.
 * @retval -2 If Not handle the host IF HDMI interface attribute.
 * @ingroup TR69_HOSTIF_STBSERVICES_HDMI_API
 */
int hostIf_STBServiceHDMI::handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if(strcasecmp(paramName, ENABLE_STRING) == 0)
    {
        ret = getEnable(stMsgData);
    }
    else if(strcasecmp(paramName, STATUS_STRING) == 0)
    {
        ret = getStatus(stMsgData);
    }
    else if(strcasecmp(paramName, NAME_STRING) == 0)
    {
        ret = getName(stMsgData);
    }
    else if(strcasecmp(paramName, RES_MODE_STRING) == 0)
    {
        strncpy(stMsgData->paramValue,getHDMIResolutionMode(), PARAM_LEN);
        stMsgData->paramtype=hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        ret = OK;
    }
    else if(strcasecmp(paramName, RES_VAL_STRING) == 0)
    {
        ret = getResolutionValue(stMsgData);
    }
    else if(strncasecmp(paramName, DISPLAYDEVICE_OBJECT_NAME, strlen(DISPLAYDEVICE_OBJECT_NAME)) == 0)
    {
        ret = displayDevice->handleGetMsg(paramName+strlen(DISPLAYDEVICE_OBJECT_NAME), stMsgData);
    }
    return ret;
}

/**
 * @brief This function updates the HDMI interface updates such as Status, Enable,
 * ResolutionMode, ResolutionValue etc in a connected HDMI port.
 *
 * @param[in] mUpdateCallback  Callback function pointer to updated the host IF HDMI interface.
 * @ingroup TR69_HOSTIF_STBSERVICES_HDMI_API
 */
void hostIf_STBServiceHDMI::doUpdates(updateCallback mUpdateCallback)
{
    displayDevice->doUpdates(BASE_NAME, mUpdateCallback);
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    char tmp_buff[PARAM_LEN];

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;
    getResolutionValue(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, RES_VAL_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
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
    getName(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, BASE_NAME, dev_id, NAME_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
}

/************************************************************
 * Description  : Set HDMI Resolution value to Device Setting
 * Precondition : HDMI Display should be connected to STB
 * Input        : stMsgData->paramValue -> resolution string [ example: "1920x1080p/24Hz" ]

 * Return       : OK -> Success
                  NOK -> Failure
************************************************************/

int hostIf_STBServiceHDMI::setResolution(const HOSTIF_MsgData_t *stMsgData)
{
    try {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] (): Value: %s \n",__FUNCTION__, stMsgData->paramValue);

        char inResolution[MAX_RESOLUTION_LENGTH] = {'\0'};
        char *bufPtr = NULL, *hzPtr = NULL, *interlacedPtr = NULL;
        char *pixelName = NULL, *frameRateName = NULL;
        bool isInterlaced = true;

        strncpy(inResolution, stMsgData->paramValue, strlen(stMsgData->paramValue));
        inResolution[sizeof(inResolution)-1] = '\0';
        hzPtr = strcasestr(inResolution, (char*)"Hz");

        if (NULL == hzPtr)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] (): Missing Hz value in Resolution\n",__FUNCTION__);
            return NOK;
        }
        else
        {
            strncpy(hzPtr, "Hz", 2);
        }

        interlacedPtr = strcasestr(inResolution, (char*)"p");
        if(interlacedPtr)
        {
            isInterlaced = false;
            *interlacedPtr = 'p';
        }
        else
        {
            interlacedPtr = strcasestr(inResolution, (char*)"i");
            if(interlacedPtr)
            {
                isInterlaced = true;
                *interlacedPtr = 'i';
            }
        }

        bufPtr = inResolution;

        if(interlacedPtr)
        {
            if(isInterlaced)
            {
                pixelName = strsep (&bufPtr, "i");
            }
            else
            {
                pixelName = strsep (&bufPtr, "p");
            }
            bufPtr++; // For "/"
        }
        else
        {
            pixelName = strsep (&bufPtr, "/");
        }

        frameRateName = strsep (&bufPtr, "Hz");

        if (!pixelName || !frameRateName)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Null Pointers caught \n",__FUNCTION__);
            return NOK;
        }

        static int pixelResolArrSize = sizeof( dsVideoPixelResolutionMapper ) / sizeof( EnumStringMapper );
        int pixelId = getEnumFromString(dsVideoPixelResolutionMapper, pixelResolArrSize, pixelName);
        if ((pixelId<dsVIDEO_PIXELRES_720x480) || (pixelId>dsVIDEO_PIXELRES_1920x1080))
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Error in PixelResolution value: %s\n",__FUNCTION__, pixelName);
            return NOK;
        }

        static int frameRateArrSize = sizeof( dsVideoFrameRateMapper ) / sizeof( EnumStringMapper );
        int frameRateId = getEnumFromString(dsVideoFrameRateMapper, frameRateArrSize, frameRateName);
        if ((frameRateId<dsVIDEO_FRAMERATE_24) || (frameRateId>dsVIDEO_FRAMERATE_59dot94))
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Error in FrameRate value: %s\n",__FUNCTION__, frameRateName);
            return NOK;
        }

        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] pixelId = %d frameRateId = %d interlaced = %s\n", __FUNCTION__, pixelId, frameRateId, isInterlaced?"true":"false");
        size_t numResolutions = dsUTL_DIM(kResolutions);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] (): numResolutions = %d\n",__FUNCTION__, numResolutions);
        for (size_t i = 0; i < numResolutions; i++) {
            dsVideoPortResolution_t *resolution = &kResolutions[i];
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] pixelId = %d frameRateId = %d interlaced = %s\n", __FUNCTION__, resolution->pixelResolution, resolution->frameRate, resolution->interlaced?"true":"false");
            if ( (resolution->pixelResolution == pixelId) &&
                 (resolution->frameRate == frameRateId) &&
                 (interlacedPtr?(resolution->interlaced == isInterlaced):true) )
            {
                vPort.setResolution(kResolutions[i].name);
                return OK;
            }
        }

        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] (): Out of range values entered\n",__FUNCTION__);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
    }

    return NOK;
}

/************************************************************
 * Description  : Get HDMI Resolution value from Device Setting
 * Precondition : HDMI Display should be connected to STB
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> resolution string [ example: "1920x1080p/24Hz" ]
************************************************************/

int hostIf_STBServiceHDMI::getResolutionValue(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        if(vPort.getResolution().getPixelResolution().getName().c_str() && vPort.getResolution().getFrameRate().getName().c_str())
        {
            snprintf(stMsgData->paramValue, PARAM_LEN, "%s%s/%sHz",
                    vPort.getResolution().getPixelResolution().getName().c_str(),
                    vPort.getResolution().isInterlaced()?"i":"p",
                    vPort.getResolution().getFrameRate().getName().c_str());

            if(bCalledResolutionValue && pChanged && strcmp(backupResolutionValue, stMsgData->paramValue))
            {
                *pChanged = true;
            }
            bCalledResolutionValue = true;
            strncpy(backupResolutionValue, stMsgData->paramValue, PARAM_LEN);
            backupResolutionValue[PARAM_LEN-1] = '\0';
        }
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] getHDMIResolution(): Value: %s\n",__FUNCTION__, stMsgData->paramValue);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Set HDMI to Enable / Disable in Device Setting
 * Precondition : HDMI Display should be connected to STB
 * Input        : stMsgData->paramValue -> 1 : Enable
                                        -> 0 : Disable

 * Return       : OK -> Success
                  NOK -> Failure
************************************************************/

int hostIf_STBServiceHDMI::setEnableVideoPort(const HOSTIF_MsgData_t *stMsgData)
{
    try {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] In setEnableVideoPort(): Value: %s \n",__FUNCTION__, stMsgData->paramValue);
        if (true == vPort.isDisplayConnected())
        {
            if(get_boolean(stMsgData->paramValue))
                vPort.enable();
            else
                vPort.disable();
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] HDMI Display is NOT connected\r\n",__FUNCTION__);
            return NOK;
        }
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Get HDMI Enable / Disable status
 * Precondition : HDMI Display should be connected to STB
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->palue -> 1 : Enabled
                                      0 : Disabled

************************************************************/

int hostIf_STBServiceHDMI::getEnable(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        if (true == vPort.isDisplayConnected())
        {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] In getHDMIEnable(): vPort.isEnabled(): %d \n",__FUNCTION__, vPort.isEnabled());
            if (vPort.isEnabled())
                put_boolean(stMsgData->paramValue,true);
            else
                put_boolean(stMsgData->paramValue,false);

            stMsgData->paramtype = hostIf_BooleanType;
            stMsgData->paramLen = sizeof(bool);
        }
        else {
            memset(stMsgData->paramValue, '\0', sizeof (stMsgData->paramValue));
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] HDMI Display is NOT connected\r\n",__FUNCTION__);
        }
        if(bCalledEnable && pChanged && (backupEnable != get_boolean(stMsgData->paramValue)))
        {
            *pChanged = true;
        }
        bCalledEnable = true;
        backupEnable = get_boolean(stMsgData->paramValue);

        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] In getHDMIEnable(): Value: %s \n",__FUNCTION__, stMsgData->paramValue);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : get HDMI port status
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> "Enabled" | "Disabled"
************************************************************/

int hostIf_STBServiceHDMI::getStatus(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int ret = getEnable(stMsgData);
    if(ret == OK)
    {
        if(get_boolean(stMsgData->paramValue))
        {
            strncpy(stMsgData->paramValue, ENABLED_STRING, PARAM_LEN);
        }
        else
        {
            strncpy(stMsgData->paramValue, DISABLED_STRING, PARAM_LEN);
        }
        stMsgData->paramValue[PARAM_LEN-1] = '\0';
        stMsgData->paramtype=hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        if(bCalledStatus && pChanged && strcmp(backupStatus, stMsgData->paramValue))
        {
            *pChanged = true;
        }
        bCalledStatus = true;
        strncpy(backupStatus, stMsgData->paramValue, PARAM_LEN);
        backupStatus[PARAM_LEN-1] = '\0';
    }
    return ret;
}


/************************************************************
 * Description  : get HDMI port human readable name
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> name string
************************************************************/

int hostIf_STBServiceHDMI::getName(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    snprintf(stMsgData->paramValue, PARAM_LEN, "HDMI%d",stMsgData->instanceNum-1);
    if(bCalledName && pChanged && strcmp(backupName, stMsgData->paramValue))
    {
        *pChanged = true;
    }
    bCalledName = true;
    strncpy(backupName, stMsgData->paramValue, PARAM_LEN);
    backupName[PARAM_LEN-1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    return OK;
}

/************************************************************
 * Description  : Set HDMI resolution mode
 * Precondition : None
 * Input        : value ["Auto" or "Manual"]

 * Return       : OK -> Success
                  NOK -> Failure
************************************************************/

int hostIf_STBServiceHDMI::setHDMIResolutionMode(const char* value)
{
    if((0 != strcasecmp(value, HDMI_RESOLUTION_MODE_AUTO)) &&
       (0 != strcasecmp(value, HDMI_RESOLUTION_MODE_MANUAL)))
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]Failed to set HDMI ResolutionMode [%s]. Expected input values: [Auto|Manual] \n", __FUNCTION__, value);
        return NOK;
    }

    strncpy(dsHDMIResolutionMode,value, sizeof(dsHDMIResolutionMode));
    dsHDMIResolutionMode[sizeof(dsHDMIResolutionMode)-1] = '\0';
    if(0 == strcasecmp(value, HDMI_RESOLUTION_MODE_AUTO))
    {
        HOSTIF_MsgData_t msgData;
        strncpy(msgData.paramValue,HDMI_RESOLUTION_VALUE_DEFAULT, PARAM_LEN);
        msgData.paramValue[PARAM_LEN-1] = '\0';
        msgData.instanceNum = dev_id;  //Use tr69 protocol 1 based indexing
        setResolution(&msgData);
    }
    return OK;
}

const char* hostIf_STBServiceHDMI::getHDMIResolutionMode()
{
    return dsHDMIResolutionMode;
}



/** @} */
/** @} */
