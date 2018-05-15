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
 * @file Components_DisplayDevice.cpp
 * @brief This source file contains the APIs of TR069 Components Display Device.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#include "Components_DisplayDevice.h"

#define STATUS_STRING "Status"
#define EDID_STRING "EDID"
#define EDID_BYTES_STRING "EDID_BYTES"
#define COMCAST_EDID_STRING "X_COMCAST_COM_EDID"
#define SUPPORTED_RES_STRING "SupportedResolutions"
#define PREF_RES_STRING "PreferredResolution"
#define VIDEO_LATENCY_STRING "VideoLatency"
#define CECSUPPORT_STRING "CECSupport"
#define AUTO_LIP_SYNC_STRING "AutoLipSyncSupport"
#define HDMI3D_STRING "HDMI3DPresent"
#define PRESENT_STRING "Present"
#define ABSENT_STRING "Absent"


#define UPDATE_FORMAT_STRING "%s.%d.%s%s"

/**
 * @brief Class Constructor of the class hostIf_STBServiceDisplayDevice.
 *
 * It will initialize the device id and video output port.
 *
 * @param[in] devid Identification number of the device.
 * @param[in] port Video output port number.
 * @ingroup TR69_HOSTIF_STBSERVICES_DISPLAYDEVICE_API
 */
hostIf_STBServiceDisplayDevice::hostIf_STBServiceDisplayDevice(int devId, device::VideoOutputPort& port) : dev_id(devId), vPort(port)
{
    strcpy(backupDisplayDeviceStatus," ");
    strcpy(backupEDID," ");
    strcpy(backupSupportedResolution," ");
    strcpy(backupPreferredResolution," ");

    bCalledDisplayDeviceStatus = false;
    bCalledEDID = false;
    bCalledEDIDBytes = false;
    bCalledSupportedResolution = false;
    bCalledPreferredResolution = false;
}

/**
 * @brief This function set the display device interface attribute value such as Status, EDID,
 * SupportedResolutions, PreferredResolution etc.. in the connected display device. Currently not
 * implemented.
 *
 * @param[in] paramName  Display device service name string.
 * @param[in] stMsgData  HostIf Message Request param contains the display device attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the hostIf display device interface attribute.
 * @retval -1 If Not able to set the hostIf display device interface attribute.
 * @retval -2 If Not handle the hostIf display device interface attribute.
 * @ingroup TR69_HOSTIF_STBSERVICES_DISPLAYDEVICE_API
 */
int hostIf_STBServiceDisplayDevice::handleSetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    stMsgData->faultCode = fcInvalidParameterName;
    int ret = NOT_HANDLED;
    return ret;
}

/**
 * @brief This function get the display device interface attribute value such as Status, EDID,
 * SupportedResolutions, PreferredResolution etc.. in the connected display device. Currently not handling
 * VideoLatency, AutoLipSyncSupport, CECSupport and HDMI3DPresent.
 *
 * @param[in] paramName  Display device service name string.
 * @param[in] stMsgData  HostIf Message Request param contains the display device attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hostIf display device interface attribute.
 * @retval -1 If Not able to get the hostIf display device interface attribute.
 * @retval -2 If Not handle the hostIf display device interface attribute.
 * @ingroup TR69_HOSTIF_STBSERVICES_DISPLAYDEVICE_API
 */
int hostIf_STBServiceDisplayDevice::handleGetMsg(const char *paramName, HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    if (strcasecmp(paramName, STATUS_STRING) == 0)
    {
        ret = getStatus(stMsgData);
    }
    else if (strcasecmp(paramName, EDID_STRING) == 0)
    {
        ret = getX_COMCAST_COM_EDID(stMsgData);
    }
    else if (strcasecmp(paramName, EDID_BYTES_STRING) == 0)
    {
        ret = getEDID_BYTES(stMsgData);
    }
    else if (strcasecmp(paramName, COMCAST_EDID_STRING) == 0)
    {
        ret = getX_COMCAST_COM_EDID(stMsgData);
    }
    else if (strcasecmp(paramName, SUPPORTED_RES_STRING) == 0)
    {
        ret = getSupportedResolutions(stMsgData);
    }
    else if (strcasecmp(paramName, PREF_RES_STRING) == 0)
    {
        ret = getPreferredResolution(stMsgData);
    }
    else if (strcasecmp(paramName, VIDEO_LATENCY_STRING) == 0)
    {
        //TODO
        //Adding fault code to handle errors, Remove the fault code when implemented
        stMsgData->faultCode = fcInvalidParameterName;
    }
    else if (strcasecmp(paramName, CECSUPPORT_STRING) == 0)
    {
        //TODO
        //Adding fault code to handle errors, Remove the fault code when implemented
        stMsgData->faultCode = fcInvalidParameterName;
    }
    else if (strcasecmp(paramName, AUTO_LIP_SYNC_STRING) == 0)
    {
        //TODO       
        //Adding fault code to handle errors, Remove the fault code when implemented
        stMsgData->faultCode = fcInvalidParameterName;
    }
    else if (strcasecmp(paramName, HDMI3D_STRING) == 0)
    {
        //TODO
        //Adding fault code to handle errors, Remove the fault code when implemented
        stMsgData->faultCode = fcInvalidParameterName;
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
        stMsgData->faultCode = fcInvalidParameterName;
        ret = NOK;
    }
    return ret;
}

/**
 * @brief This function updates the hostIf display device interface attribute value such as Status, EDID,
 * SupportedResolutions, PreferredResolution in the connected display device.
 *
 * @param[in] baseName  Display device service name string.
 * @param[in] mUpdateCallback  Callback function which updates the hostIf video interface.
 * @ingroup TR69_HOSTIF_STBSERVICES_DISPLAYDEVICE_API
 */
void hostIf_STBServiceDisplayDevice::doUpdates(const char *baseName, updateCallback mUpdateCallback)
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
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, baseName, dev_id, DISPLAYDEVICE_OBJECT_NAME, STATUS_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;

    getX_COMCAST_COM_EDID(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, baseName, dev_id, DISPLAYDEVICE_OBJECT_NAME, EDID_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;

    getEDID_BYTES(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, baseName, dev_id, DISPLAYDEVICE_OBJECT_NAME, EDID_BYTES_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;

    getSupportedResolutions(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING, baseName, dev_id, DISPLAYDEVICE_OBJECT_NAME, SUPPORTED_RES_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,PARAM_LEN);
    bChanged =  false;
    msgData.instanceNum=dev_id;

    getPreferredResolution(&msgData,&bChanged);
    if(bChanged)
    {
        snprintf(tmp_buff, PARAM_LEN, UPDATE_FORMAT_STRING,baseName, dev_id, DISPLAYDEVICE_OBJECT_NAME, PREF_RES_STRING);
        if(mUpdateCallback)
        {
            mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,tmp_buff, msgData.paramValue, msgData.paramtype);
        }
    }
}

/************************************************************
 * Description  : Get HDMI Display Device connection status
 * Precondition : None
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> "Present": HDMI Display Connected
                                           "Absent" : HDMI Display Not Connected

************************************************************/

int hostIf_STBServiceDisplayDevice::getStatus(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] vPort.isDisplayConnected(): %d \n",__FUNCTION__, vPort.isDisplayConnected());
        if (true == vPort.isDisplayConnected()) {
            strncpy(stMsgData->paramValue, PRESENT_STRING, PARAM_LEN);
        }
        else {
            strncpy(stMsgData->paramValue, ABSENT_STRING, PARAM_LEN);
        }
        stMsgData->paramValue[PARAM_LEN-1] = '\0';
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        if(bCalledDisplayDeviceStatus && pChanged && strcmp(backupDisplayDeviceStatus, stMsgData->paramValue))
        {
            *pChanged = true;
        }
        bCalledDisplayDeviceStatus = true;
        strncpy(backupDisplayDeviceStatus, stMsgData->paramValue, PARAM_LEN);
        backupDisplayDeviceStatus[PARAM_LEN-1] = '\0';
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] In getHDMIDisplayDeviceStatus(): Value: %s \n",__FUNCTION__, stMsgData->paramValue);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Get Display Device EDID
 * Precondition : HDMI Display should be connected to STB
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> Comcast specific EDID

************************************************************/

int hostIf_STBServiceDisplayDevice::getX_COMCAST_COM_EDID(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    int productCode;        /**< Product Code of display device */
    int serialNumber;       /**< Serial Number of display device */
    int manufactureYear;    /**< Manufacture Year of display device */
    int manufactureWeek;    /**< Manufacture Week of the display device */

    try {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] vPort.isDisplayConnected(): %d \n",__FUNCTION__, vPort.isDisplayConnected());
        if (true == vPort.isDisplayConnected()) {
            productCode = vPort.getDisplay().getProductCode();
            serialNumber = vPort.getDisplay().getSerialNumber();
            manufactureWeek = vPort.getDisplay().getManufacturerWeek();
            manufactureYear = vPort.getDisplay().getManufacturerYear();
            snprintf(stMsgData->paramValue, PARAM_LEN, "pcode=0x%x,pserial=0x%x,year=%d,week=%d", productCode, serialNumber, manufactureYear, manufactureWeek);
        }
        else {
            memset(stMsgData->paramValue, '\0', sizeof (stMsgData->paramValue));
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] HDMI Display is NOT connected\r\n",__FUNCTION__);
        }
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        if(bCalledEDID && pChanged && strcmp(backupEDID, stMsgData->paramValue))
        {
            *pChanged = true;
        }
        bCalledEDID = true;
        strncpy(backupEDID,stMsgData->paramValue,PARAM_LEN);
        backupEDID[PARAM_LEN-1] = '\0';
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] In getHDMIDisplayDeviceX_COMCAST_COM_EDID(): Value: %s \n",__FUNCTION__, stMsgData->paramValue);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : Get Display Device EDID Bytes
 * Precondition : HDMI Display should be connected to STB
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> EDID Bytes

************************************************************/

int hostIf_STBServiceDisplayDevice::getEDID_BYTES(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    try {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] vPort.isDisplayConnected(): %d \n",__FUNCTION__, vPort.isDisplayConnected());
        if (true == vPort.isDisplayConnected()) {
            std::vector<unsigned char> bytes;
            vPort.getDisplay().getEDIDBytes(bytes);
            if (bytes.size() > 256){
                memset(stMsgData->paramValue, '\0', sizeof (stMsgData->paramValue));
                RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] HDMI Display has EDID of %d bytes. We only support 256 bytes!\r\n",__FUNCTION__, bytes.size());
            } else {
                for(int j = 0; j < bytes.size(); j++)
                    sprintf(&stMsgData->paramValue[2*j], "%02X", bytes[j]);
                stMsgData->paramValue[2*bytes.size()]='\0';
            }
        }
        else {
            memset(stMsgData->paramValue, '\0', sizeof (stMsgData->paramValue));
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] HDMI Display is NOT connected\r\n",__FUNCTION__);
        }
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        if(bCalledEDIDBytes && pChanged && strcmp(backupEDIDBytes, stMsgData->paramValue))
        {
            *pChanged = true;
        }
        bCalledEDIDBytes = true;
        strncpy(backupEDIDBytes,stMsgData->paramValue,PARAM_LEN);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] In getHDMIDisplayDeviceEDIDBytes(): Value: %s \n",__FUNCTION__, stMsgData->paramValue);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}


/************************************************************
 * Description  : List of Supported resolution by display device connected.
 * Precondition : HDMI Display should be connected to STB
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> List of Supported resolution.
************************************************************/
int hostIf_STBServiceDisplayDevice::getSupportedResolutions(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    size_t iElementInList = 0;
    size_t iResolutionsListSize = 0;
    char aiResolution[MAX_RESOLUTION_LENGTH] = {'\0'};
    try
    {
        device::List<device::VideoResolution> vResolutions = vPort.getType().getSupportedResolutions();
        iResolutionsListSize = vResolutions.size();

        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] : List Size: %d \n",__FUNCTION__, iResolutionsListSize);
        memset(stMsgData->paramValue, 0, TR69HOSTIFMGR_MAX_PARAM_LEN);

        for(iElementInList = 0; iElementInList < iResolutionsListSize; iElementInList++)
        {
            device::VideoResolution vResIdex = vPort.getType().getSupportedResolutions().at(iElementInList);
            snprintf(aiResolution, MAX_RESOLUTION_LENGTH, "%s%s/%sHz",
                    vResIdex.getPixelResolution().getName().c_str(),
                    vResIdex.isInterlaced()?"i":"p",
                    vResIdex.getFrameRate().getName().c_str());
            strncat(stMsgData->paramValue, aiResolution, TR69HOSTIFMGR_MAX_PARAM_LEN-strlen(stMsgData->paramValue)-1);
            if(iElementInList < (iResolutionsListSize-1))
                    strncat(stMsgData->paramValue,",", TR69HOSTIFMGR_MAX_PARAM_LEN-strlen(stMsgData->paramValue)-1);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] : resolution: %s\n",__FUNCTION__, vPort.getType().getSupportedResolutions().at(iElementInList).getName().c_str());
        }
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        if(bCalledSupportedResolution && pChanged && strcmp(backupSupportedResolution, stMsgData->paramValue))
        {
            *pChanged = true;
        }
        bCalledSupportedResolution = true;
        strncpy(backupSupportedResolution,stMsgData->paramValue,PARAM_LEN);
        backupSupportedResolution[PARAM_LEN-1] = '\0';
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s]  : Value: %s \n",__FUNCTION__, stMsgData->paramValue);
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}

/************************************************************
 * Description  : preferred resolution of display device connected.
 * Precondition : HDMI Display should be connected to STB
 * Input        : stMsgData for result return.
                  pChanged

 * Return       : OK -> Success
                  NOK -> Failure
                  stMsgData->paramValue -> preferred resolution [ example : "1920x1080p/24Hz" ]
************************************************************/
int hostIf_STBServiceDisplayDevice::getPreferredResolution(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{

    try
    {
        snprintf(stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN, "%s%s/%sHz",
                vPort.getDefaultResolution().getPixelResolution().getName().c_str(),
                vPort.getDefaultResolution().isInterlaced()?"i":"p",
                vPort.getDefaultResolution().getFrameRate().getName().c_str());
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] : Value: %s \n",__FUNCTION__, stMsgData->paramValue);
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
        if(bCalledPreferredResolution && pChanged && strcmp(stMsgData->paramValue, backupPreferredResolution))
        {
            *pChanged = true;
        }
        bCalledPreferredResolution = true;
        strncpy(backupPreferredResolution, stMsgData->paramValue, PARAM_LEN);
        backupPreferredResolution[PARAM_LEN-1] = '\0';
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        return NOK;
    }

    return OK;
}



/** @} */
/** @} */
