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
 * @file XrdkBlueTooth.cpp
 * @brief This source file contains the APIs for getting bluetooth device information.
 */

/**
 * @file XrdkBlueTooth.cpp
 *
 * @brief DeviceInfo X_RDKCENTRAL-COM_xBlueTooth API Implementation.
 *
 * This is the implementation of the DeviceInfo API.
 *
 * @par Document
 * TBD Relevant design or API documentation.
 *
 */


/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/

#ifdef USE_XRDK_BT_PROFILE

#define BTRMGR_QUERY_INTERVAL 5

#include "XrdkBlueTooth.h"
extern "C" {
#include "btmgr.h"
}

static BTRMGR_DiscoveredDevicesList_t disDevList;
static BTRMGR_PairedDevicesList_t pairedDevList;
static BTRMGR_ConnectedDevicesList_t connectedDevList;

static time_t firstExTimeDisList = 0;
static time_t firstExTimePairedList = 0;
static time_t firstExTimeConnList = 0;

static void fetch_Bluetooth_DiscoveredDevicesList();
static void fetch_Bluetooth_PairedDevicesList();
static void fetch_Bluetooth_ConnectedDevicesList();
static BTRMGR_DevicesProperty_t deviceProperty;
static BTRMgrDeviceHandle handle_devInfo = 0;

GMutex* hostIf_DeviceInfoRdk_xBT::m_mutex = NULL;
hostIf_DeviceInfoRdk_xBT* hostIf_DeviceInfoRdk_xBT::m_instance = NULL;

short hostIf_DeviceInfoRdk_xBT::noOfDiscoveredDevice=0;
short hostIf_DeviceInfoRdk_xBT::noOfPairedDevice=0;
short hostIf_DeviceInfoRdk_xBT::noOfConnectedDevices=0;
void BTRMgr_EventCallback(BTRMGR_EventMessage_t );

hostIf_DeviceInfoRdk_xBT* hostIf_DeviceInfoRdk_xBT::getInstance()
{
    if(!m_instance)
    {
        try {
            m_instance = new hostIf_DeviceInfoRdk_xBT();
        } catch(const std::exception& e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s:%d] xBlueTooth: Caught exception \" %s\"\n", __FUNCTION__, __LINE__, e.what());
        }
    }
    return m_instance;
}


void hostIf_DeviceInfoRdk_xBT::closeInstance()
{
    if(m_instance)
    {
        delete m_instance;
    }
}


void hostIf_DeviceInfoRdk_xBT::getLock()
{
    if(!m_mutex) {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void hostIf_DeviceInfoRdk_xBT::releaseLock()
{
    if(m_mutex)  g_mutex_unlock(m_mutex);
}

void hostIf_DeviceInfoRdk_xBT::reset()
{
    noOfDiscoveredDevice = 0;
    noOfPairedDevice = 0;
    noOfConnectedDevices = 0;
}


/**
 * @brief Class default Constructor.
 */
hostIf_DeviceInfoRdk_xBT::hostIf_DeviceInfoRdk_xBT()
{

}

/*

void hostIf_DeviceInfoRdk_xBT::XrdkBlueTooth_init()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering...  \n", __FUNCTION__, __LINE__);

    if (BTRMGR_RESULT_SUCCESS != BTRMGR_Init()) {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Failed in BTRMGR_Init() \n", __FUNCTION__, __LINE__);
    }
    else {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%d] xBlueTooth: Successfully: BTRMGR_Init()  \n", __FUNCTION__, __LINE__);

        if(BTRMGR_RESULT_SUCCESS != BTRMGR_RegisterEventCallback(BTRMgr_EventCallback)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Failed in BTRMGR_RegisterEventCallback() \n", __FUNCTION__, __LINE__);
        }
        else {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Successfully : BTRMGR_RegisterEventCallback() \n", __FUNCTION__, __LINE__);
        }
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]Exiting...  \n", __FUNCTION__, __LINE__);
}
*/

/**
 * @brief This function set the bluetooth profile attributes
 *
 *
 * @param[in] stMsgData  HostIf Message Request param contains the attribute value.
 *
 * @return Returns an Integer value.
 * @ingroup TR69_HOSTIF_X_RDKCENTRAL-COM_xBlueTooth_API
 */
int hostIf_DeviceInfoRdk_xBT::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *path = NULL, *paramName = NULL, *tblAttName = NULL;
    int index = 0;
    getLock();
    try {
        int str_len = strlen(X_BT_ROOT_OBJ);
        path = stMsgData->paramName;

        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering...  \n", __FUNCTION__, __LINE__);

        if(NULL == path) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Failed : Parameter is NULL, %s  \n",  __FUNCTION__, __LINE__, path);
            releaseLock();
            return ret;
        }

        if((strncasecmp(path, X_BT_ROOT_OBJ, str_len) != 0)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Failed due to Mismatch parameter path : %s  \n", __FUNCTION__, __LINE__, path);
            releaseLock();
            return ret;
        }
        const char *tmp_ptr = strchr(path+str_len-1,'.');
        if(tmp_ptr == NULL)  {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Parameter is NULL  \n", __FUNCTION__, __LINE__);
            releaseLock();
            return ret;
        }

        tmp_ptr++;
        paramName = tmp_ptr;
        if (strncasecmp(paramName, BT_DEV_GETDEVICEINFO_STRING, strlen(BT_DEV_GETDEVICEINFO_STRING)) == 0)
        {
            ret = setDeviceInfo(stMsgData);
        }
        else
        {
            ret = NOT_HANDLED;
        }

    }
    catch (const std::exception& e )
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exception caught %s   \n", __FUNCTION__, __LINE__, e.what());
        releaseLock();
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);

    releaseLock();

    return ret;
}

/**
 * @brief This function get the bluetooth attributes such as name, profile, count and others
 * for paired and connected devices..
 *
 * @param[in] stMsgData  HostIf Message Request param contains the external SD Card attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the attribute values.
 * @retval -1 If failed and Not able to get.
 * @retval -2 If Not handle the requested attribute.
 * @ingroup TR69_HOSTIF_X_RDKCENTRAL-COM_xBlueTooth_API
 */
int hostIf_DeviceInfoRdk_xBT::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *path = NULL, *paramName = NULL, *tblAttName = NULL;
    int index = 0;
    getLock();
    try {
        int str_len = strlen(X_BT_ROOT_OBJ);
        path = stMsgData->paramName;

        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering...  \n", __FUNCTION__, __LINE__);

        if(NULL == path) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Failed : Parameter is NULL, %s  \n",  __FUNCTION__, __LINE__, path);
            releaseLock();
            return ret;
        }

        if((strncasecmp(path, X_BT_ROOT_OBJ, str_len) != 0)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Failed due to Mismatch parameter path : %s  \n", __FUNCTION__, __LINE__, path);
            releaseLock();
            return ret;
        }
        const char *tmp_ptr = strchr(path+str_len-1,'.');
        if(tmp_ptr == NULL)  {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Parameter is NULL  \n", __FUNCTION__, __LINE__);
            releaseLock();
            return ret;
        }

        tmp_ptr++;
        paramName = tmp_ptr;
        if (strncasecmp(paramName, BT_ENABLE_STRING, strlen(BT_ENABLE_STRING)) == 0)
        {
            ret = isEnabled(stMsgData);
        }
        else if (strncasecmp(paramName, BT_DISCOVERY_ENABLED_STRING, strlen(BT_DISCOVERY_ENABLED_STRING)) == 0)
        {
            ret = isDiscoveryEnabled(stMsgData);
        }
        else if (strncasecmp(paramName, BT_DISCOVERY_DEVICE_CNT_STRING, strlen(BT_DISCOVERY_DEVICE_CNT_STRING)) == 0)
        {
            ret = getDiscoveredDeviceCnt(stMsgData);
        }
        else if (strncasecmp(paramName, BT_PAIRED_DEVICE_CNT_STRING, strlen(BT_PAIRED_DEVICE_CNT_STRING)) == 0)
        {
            ret = getPairedDeviceCnt(stMsgData);
        }
        else if (strncasecmp(paramName, BT_CONNECTED_DEVICE_CNT_STRING, strlen(BT_CONNECTED_DEVICE_CNT_STRING)) == 0)
        {
            ret = getConnectedDeviceCnt(stMsgData);
        }
        /*For profile: Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.*/
        else if (strncasecmp(paramName, BT_DEV_GETDEVICEINFO_STRING, strlen(BT_DEV_GETDEVICEINFO_STRING)) == 0)
        {
            ret = getDeviceInfo(stMsgData);
        }
        else if (strncasecmp(paramName, BT_DEV_INFO_DEVICE_ID_STRING, strlen(BT_DEV_INFO_DEVICE_ID_STRING)) == 0)
        {
            ret = getDeviceInfo_DeviceID(stMsgData);
        }
        else if (strncasecmp(paramName, BT_DEV_INFO_MANUFACTURER_STRING, strlen(BT_DEV_INFO_MANUFACTURER_STRING)) == 0)
        {
            ret = getDeviceInfo_Manufacturer(stMsgData);
        }
        else if (strncasecmp(paramName, BT_DEV_INFO_PROFILE_STRING, strlen(BT_DEV_INFO_PROFILE_STRING)) == 0)
        {
            ret = getDeviceInfo_Profile(stMsgData);
        }
        else if (strncasecmp(paramName, BT_DEV_INFO_MAC_STRING, strlen(BT_DEV_INFO_MAC_STRING)) == 0)
        {
            ret = getDeviceInfo_MAC(stMsgData);
        }
        else if (strncasecmp(paramName, BT_DEV_INFO_SIGNALLEVEL_STRING, strlen(BT_DEV_INFO_SIGNALLEVEL_STRING)) == 0)
        {
            ret = getDeviceInfo_SignalLevel(stMsgData);
        }
        else if (strncasecmp(paramName, BT_DEV_INFO_RSSI_STRING, strlen(BT_DEV_INFO_RSSI_STRING)) == 0)
        {
            ret = getDeviceInfo_RSSI(stMsgData);
        }
        /* For Discovered Devices */
        else if(matchComponent(stMsgData->paramName, X_BT_DISCOVERED_DEV_OBJ, &tblAttName, index))
        {
            stMsgData->instanceNum = index;
            if (strcasecmp(tblAttName, BT_DEV_NAME_STRING) == 0)
            {
                ret = getDiscoveredDevice_Name(stMsgData);
            }
            else if (strcasecmp(tblAttName, BT_DEV_DEVICE_ID_STRING) == 0)
            {
                ret = getDiscoveredDevice_DeviceID(stMsgData);
            }
            else if (strcasecmp(tblAttName, BT_DEV_DEVICE_TYPE_STRING) == 0)
            {
                ret = getDiscoveredDevice_DeviceType(stMsgData);
            }
            else if (strcasecmp(tblAttName, BT_DEV_PAIRED_STRING) == 0)
            {
                ret = getDiscoveredDevice_Paired(stMsgData);
            }
            else if (strcasecmp(tblAttName, BT_DEV_CONNECTED_STRING) == 0)
            {
                ret = getDiscoveredDevice_Connected (stMsgData);
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Parameter \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
                ret = NOK;
            }
        }
        /* For Paired Devices */
        else if(matchComponent(stMsgData->paramName, X_BT_PAIRED_DEV_OBJ, &tblAttName, index))
        {
            stMsgData->instanceNum = index;
            if (strcasecmp(tblAttName, BT_DEV_NAME_STRING) == 0)
            {
                ret = getPairedDevice_Name(stMsgData);
            }
            else if (strcasecmp(tblAttName, BT_DEV_DEVICE_ID_STRING) == 0)
            {
                ret = getPairedDevice_DeviceID(stMsgData);
            }
            else if (strcasecmp(tblAttName, BT_DEV_CONNECTED_STRING) == 0)
            {
                ret = getPairedDevice_Connected (stMsgData);
            }
            else if (strcasecmp(tblAttName, BT_DEV_DEVICE_TYPE_STRING) == 0)
            {
                ret = getPairedDevice_DeviceType(stMsgData);
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Parameter \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
                ret = NOK;
            }
        }
        /* For Connected Devices */
        else if(matchComponent(stMsgData->paramName, X_BT_CONNECTED_DEV_OBJ, &tblAttName, index))
        {
            stMsgData->instanceNum = index;

            if (strcasecmp(tblAttName, BT_DEV_NAME_STRING) == 0)
            {
                ret = getConnectedDevice_Name(stMsgData);
            }
            else if (strcasecmp(tblAttName, BT_DEV_DEVICE_ID_STRING) == 0)
            {
                ret = getConnectedDevice_DeviceID(stMsgData);
            }
            else if (strcasecmp(tblAttName, BT_DEV_DEVICE_TYPE_STRING) == 0)
            {
                ret = getConnectedDevice_DeviceType(stMsgData);
            }
            else if (strcasecmp(tblAttName, BT_DEV_ACTIVE_STRING) == 0)
            {
                ret = getConnectedDevice_Active(stMsgData);
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Parameter \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
                ret = NOK;
            }
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Parameter \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, paramName);
            ret = NOK;
        }
    }
    catch (const std::exception& e )
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exception caught %s   \n", __FUNCTION__, __LINE__, e.what());
        releaseLock();
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    releaseLock();
    return ret;
}



/************************************************************
 * Description  : Get the 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.enable' status.
 * 				 'true(1)' if enabled or 'false(0)'
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> 0: false
                        -> 1: true
************************************************************/
int hostIf_DeviceInfoRdk_xBT::isEnabled(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        /* BLUETOOTH_DISABLED, BLUETOOTH_ENABLED, BLUETOOTH_INPUT_ENABLED */
        unsigned char power_status = 0;
        if(BTRMGR_RESULT_SUCCESS == BTRMGR_GetAdapterPowerStatus (0, &power_status) && 0 != power_status) {
            strncpy(stMsgData->paramValue, "BLUETOOTH_ENABLED", TR69HOSTIFMGR_MAX_PARAM_LEN);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s]xBlueTooth: Successfully get BTRMGR_GetAdapterPowerStatus (%u), so BLUETOOTH_ENABLED. \n",
                    __FUNCTION__, power_status);
        }
        else {
            strncpy(stMsgData->paramValue, "BLUETOOTH_DISABLED", TR69HOSTIFMGR_MAX_PARAM_LEN);
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Failed in get BTRMGR_GetAdapterPowerStatus, so BLUETOOTH_DISABLED. \n", __FUNCTION__, __LINE__);
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exception : %s\r\n",__FUNCTION__, __LINE__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_StringType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}


/************************************************************
 * Description  : Get the 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveryEnabled' status.
 * 				 'true(1)' if enabled or 'false(0)'
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> 0: false
                        -> 1: true
************************************************************/
int hostIf_DeviceInfoRdk_xBT::isDiscoveryEnabled(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);
    bool isDisEnable = false;

    try {
        unsigned char isDiscoverable = 0;
        isDisEnable = (BTRMGR_RESULT_SUCCESS == BTRMGR_IsAdapterDiscoverable(0, &isDiscoverable) && isDiscoverable == 1)? true:false;
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: isDiscoveryEnabled : %d \n",
                __FUNCTION__, isDisEnable);
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exception : %s\r\n",__FUNCTION__, __LINE__, e.what());
        return NOK;
    }
    put_boolean(stMsgData->paramValue, isDisEnable);
    stMsgData->paramtype=hostIf_BooleanType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}


/************************************************************
 * Description  : Number of discovered devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDeviceCnt' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> 1..N
                        -> 0: Not discovered
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getDiscoveredDeviceCnt(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);
    int disDevCount = 0;

    try {
        fetch_Bluetooth_DiscoveredDevicesList();
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]Successful, Total Discovered Devices are (%d).\n", __FUNCTION__, disDevList.m_numOfDevices);
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    put_int(stMsgData->paramValue, disDevList.m_numOfDevices);
    stMsgData->paramtype=hostIf_UnsignedIntType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}


/************************************************************
 * Description  : Number of Paired devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDeviceCnt' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> 1..N
                        -> 0: Not paired
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getPairedDeviceCnt(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        fetch_Bluetooth_PairedDevicesList();
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"Successful, Total Paired Devices are (%d).\n", __FUNCTION__, pairedDevList.m_numOfDevices);
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    put_int(stMsgData->paramValue, pairedDevList.m_numOfDevices);
    stMsgData->paramtype=hostIf_UnsignedIntType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}


/************************************************************
 * Description  : Number of Connected devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDeviceCnt' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> 1..N
                        -> 0: Not Connected devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getConnectedDeviceCnt(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        fetch_Bluetooth_ConnectedDevicesList();
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"Successful, Total Connected Devices are (%d).\n", __FUNCTION__, connectedDevList.m_numOfDevices);
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    put_int(stMsgData->paramValue, connectedDevList.m_numOfDevices);
    stMsgData->paramtype=hostIf_UnsignedIntType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}


/************************************************************
 * Description  : Name of discovered devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.{i}.Name' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Name of the discovered devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getDiscoveredDevice_Name(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    stMsgData->paramtype=hostIf_StringType;

    try {
        int devIndex = stMsgData->instanceNum;
        if((devIndex && disDevList.m_numOfDevices) && (devIndex <= disDevList.m_numOfDevices)) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Discovered Devices (index : %d) Name is \'%s\'.\n", __FUNCTION__, devIndex-1, disDevList.m_deviceProperty[devIndex-1].m_name);
            strncpy(stMsgData->paramValue, disDevList.m_deviceProperty[devIndex-1].m_name, TR69HOSTIFMGR_MAX_PARAM_LEN);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: Failed due to wrong device index %d\r\n",__FUNCTION__, devIndex);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}

/************************************************************
 * Description  : MAC of Discovered devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.{i}.MAC' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Mac of the DiscoveredDevice devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getDiscoveredDevice_MAC(HOSTIF_MsgData_t *stMsgData) {
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        int devIndex = stMsgData->instanceNum;
        if((devIndex && disDevList.m_numOfDevices) && (devIndex <= disDevList.m_numOfDevices)) {

            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Discovered Devices (index : %d) MAC is \'%s\'.\n", __FUNCTION__, devIndex-1, disDevList.m_deviceProperty[devIndex-1].m_deviceAddress);
            strncpy(stMsgData->paramValue, disDevList.m_deviceProperty[devIndex-1].m_deviceAddress, TR69HOSTIFMGR_MAX_PARAM_LEN);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: Failed due to wrong device index %d\r\n",__FUNCTION__, devIndex);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_StringType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}

/************************************************************
 * Description  : DeviceType of Discovered devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.{i}.DeviceType' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Device Type of the DiscoveredDevice devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getDiscoveredDevice_DeviceType(HOSTIF_MsgData_t *stMsgData) {
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        int devIndex = stMsgData->instanceNum;
        if((devIndex && disDevList.m_numOfDevices) && (devIndex <= disDevList.m_numOfDevices)) {
            const char *pType = BTRMGR_GetDeviceTypeAsString (disDevList.m_deviceProperty[devIndex-1].m_deviceType);
            if (pType)
            {
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Discovered Devices (index : %d) device type is \'%s\'.\n", __FUNCTION__, devIndex-1, pType);
                strncpy(stMsgData->paramValue, pType, TR69HOSTIFMGR_MAX_PARAM_LEN);
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth:Failed; Null Name\r\n",__FUNCTION__);
                return NOK;
            }
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: Failed due to wrong device index %d\r\n",__FUNCTION__, devIndex);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_StringType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}

/************************************************************
 * Description  : Paired status of Discovered devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.{i}.Paired' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Pair status of the DiscoveredDevice devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getDiscoveredDevice_Paired(HOSTIF_MsgData_t *stMsgData) {
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        int devIndex = stMsgData->instanceNum;
        if((devIndex && disDevList.m_numOfDevices) && (devIndex <= disDevList.m_numOfDevices)) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Discovered Devices (index : %d) Pairing status is \'%d\'.\n", __FUNCTION__, devIndex-1, disDevList.m_deviceProperty[devIndex-1].m_isPairedDevice);
            put_boolean(stMsgData->paramValue, disDevList.m_deviceProperty[devIndex-1].m_isPairedDevice);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: Failed due to wrong device index %d\r\n",__FUNCTION__, devIndex);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_BooleanType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}

/************************************************************
 * Description  : Manufacturer of Discovered devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.{i}.DeviceID' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Manufacturer of the Discovered devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getDiscoveredDevice_DeviceID(HOSTIF_MsgData_t *stMsgData) {
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        int devIndex = stMsgData->instanceNum;
        stMsgData->paramtype = hostIf_StringType;

        if((devIndex) && (devIndex <= disDevList.m_numOfDevices)) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Discovered Devices (index : %d) DeviceID is \'%llu\'.\n", __FUNCTION__, devIndex-1, disDevList.m_deviceProperty[devIndex-1].m_deviceHandle);
            memset(stMsgData->paramValue, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN);
            sprintf(stMsgData->paramValue, "%llu", disDevList.m_deviceProperty[devIndex-1].m_deviceHandle);
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Discovered Devices (index : %d) Convert DeviceID is \'%s\'.\n", __FUNCTION__, devIndex-1, stMsgData->paramValue);

        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: Failed due to wrong device index %d\r\n",__FUNCTION__, devIndex);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}


/************************************************************
 * Description  : Manufacturer of Discovered devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.{i}.Manufacturer' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Manufacturer of the Discovered devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getDiscoveredDevice_Manufacturer(HOSTIF_MsgData_t *stMsgData) {
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        int devIndex = stMsgData->instanceNum;
        if((devIndex && disDevList.m_numOfDevices) && (devIndex <= disDevList.m_numOfDevices)) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Discovered Devices (index : %d) Manufacturer is \'%d\'.\n", __FUNCTION__, devIndex-1, disDevList.m_deviceProperty[devIndex-1].m_vendorID);
            put_int(stMsgData->paramValue, disDevList.m_deviceProperty[devIndex-1].m_vendorID);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: Failed due to wrong device index %d\r\n",__FUNCTION__, devIndex);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_UnsignedIntType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}

/************************************************************
 * Description  : Connected Status of Discovered devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice.{i}.Connected' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Connected Status of the Discovered devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getDiscoveredDevice_Connected(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);
    BTRMGR_DevicesProperty_t deviceProperty;
    memset (&deviceProperty, 0, sizeof (deviceProperty));

    try {
        int devIndex = stMsgData->instanceNum;
        if((devIndex && disDevList.m_numOfDevices) && (devIndex <= disDevList.m_numOfDevices)) {

            /* Get the signal level */
            BTRMGR_GetDeviceProperties(0, disDevList.m_deviceProperty[devIndex-1].m_deviceHandle, &deviceProperty);
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Discovered Devices (index : %d) Connection Status is \'%d\'.\n", __FUNCTION__, devIndex-1, deviceProperty.m_isConnected);

            put_boolean(stMsgData->paramValue, deviceProperty.m_isConnected);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: Failed due to wrong device index %d\r\n",__FUNCTION__, devIndex);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_BooleanType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}



/************************************************************
 * Description  : Name of paired devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice.{i}.Name' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Name of the paired devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getPairedDevice_Name(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        int devIndex = stMsgData->instanceNum;
        if((devIndex && pairedDevList.m_numOfDevices) && (devIndex <= pairedDevList.m_numOfDevices)) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Paired Devices (index : %d) Name is \'%s\'.\n", __FUNCTION__, devIndex-1, pairedDevList.m_deviceProperty[devIndex-1].m_name);
            memset(stMsgData->paramValue, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN);
            strncpy(stMsgData->paramValue, pairedDevList.m_deviceProperty[devIndex-1].m_name, TR69HOSTIFMGR_MAX_PARAM_LEN);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: Failed due to wrong device index %d\r\n",__FUNCTION__, devIndex);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_StringType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}

/************************************************************
 * Description  : Profile of Paired devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice.{i}.Profile' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Name of the paired devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getPairedDevice_Profile(HOSTIF_MsgData_t *stMsgData) {
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        int devIndex = stMsgData->instanceNum;
        char serviceInfo[TR69HOSTIFMGR_MAX_PARAM_LEN];
        memset (&serviceInfo, '\0', sizeof(serviceInfo));
        if((devIndex && pairedDevList.m_numOfDevices) && (devIndex <= pairedDevList.m_numOfDevices)) {
            for (int i = 0; i < pairedDevList.m_deviceProperty[devIndex-1].m_serviceInfo.m_numOfService; i++)
            {
                strcat(serviceInfo, pairedDevList.m_deviceProperty[devIndex-1].m_serviceInfo.m_profileInfo[i].m_profile);
                if ( (i + 1) < pairedDevList.m_deviceProperty[devIndex-1].m_serviceInfo.m_numOfService)
                    strcat(serviceInfo, ",");
            }

            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Paired Devices (index : %d) profile is \'%s\'.\n", __FUNCTION__, devIndex-1, serviceInfo);
            memset(stMsgData->paramValue, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN);
            strncpy(stMsgData->paramValue, serviceInfo, TR69HOSTIFMGR_MAX_PARAM_LEN);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: Failed due to wrong device index %d\r\n",__FUNCTION__, devIndex);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_StringType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}

/************************************************************
 * Description  : MAC of Paired devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice.{i}.MAC' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Mac of the paired devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getPairedDevice_MAC(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        int devIndex = stMsgData->instanceNum;
        if((devIndex && pairedDevList.m_numOfDevices) && (devIndex <= pairedDevList.m_numOfDevices)) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Paired Devices (index : %d) MAC is \'%s\'.\n", __FUNCTION__, devIndex-1, pairedDevList.m_deviceProperty[devIndex-1].m_deviceAddress);
            memset(stMsgData->paramValue, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN);
            strncpy(stMsgData->paramValue, pairedDevList.m_deviceProperty[devIndex-1].m_deviceAddress, TR69HOSTIFMGR_MAX_PARAM_LEN);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: Failed due to wrong device index %d\r\n",__FUNCTION__, devIndex);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_StringType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}

/************************************************************
 * Description  : Manufacturer of Paired devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice.{i}.Manufacturer' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Manufacturer of the paired devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getPairedDevice_Manufacturer(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        int devIndex = stMsgData->instanceNum;
        if((devIndex && pairedDevList.m_numOfDevices) && (devIndex <= pairedDevList.m_numOfDevices)) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Paired Devices (index : %d) Manufacturer is \'%d\'.\n", __FUNCTION__, devIndex-1, pairedDevList.m_deviceProperty[devIndex-1].m_vendorID);
            put_int(stMsgData->paramValue, pairedDevList.m_deviceProperty[devIndex-1].m_vendorID);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: Failed due to wrong device index %d\r\n",__FUNCTION__, devIndex);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_UnsignedIntType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}


/************************************************************
 * Description  : Manufacturer of Paired devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice.{i}.DeviceID' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> DeviceID   of the paired devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getPairedDevice_DeviceID(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        int devIndex = stMsgData->instanceNum;
        stMsgData->paramtype = hostIf_StringType;

        if((devIndex && pairedDevList.m_numOfDevices) && (devIndex <= pairedDevList.m_numOfDevices)) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Paired Devices (index : %d) DeviceID is \'%llu\'.\n", __FUNCTION__, devIndex-1, pairedDevList.m_deviceProperty[devIndex-1].m_deviceHandle);
            memset(stMsgData->paramValue, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN);
            sprintf(stMsgData->paramValue, "%llu", pairedDevList.m_deviceProperty[devIndex-1].m_deviceHandle);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: Failed due to wrong device index %d\r\n",__FUNCTION__, devIndex);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}


/************************************************************
 * Description  : Connected Status of Paired devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice.{i}.Connected' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Connected Status of the paired devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getPairedDevice_Connected(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        int devIndex = stMsgData->instanceNum;
        if((devIndex && pairedDevList.m_numOfDevices) && (devIndex <= pairedDevList.m_numOfDevices)) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Paired Devices (index : %d) Connection status is \'%d\'.\n", __FUNCTION__, devIndex-1, pairedDevList.m_deviceProperty[devIndex-1].m_isConnected);
            put_boolean(stMsgData->paramValue, pairedDevList.m_deviceProperty[devIndex-1].m_isConnected);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: Failed due to wrong device index %d\r\n",__FUNCTION__, devIndex);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_BooleanType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}

/************************************************************
 * Description  : DeviceType of Paired devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice.{i}.DeviceType' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Device Type of the Paired devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getPairedDevice_DeviceType(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        int devIndex = stMsgData->instanceNum;
        if((devIndex && pairedDevList.m_numOfDevices) && (devIndex <= pairedDevList.m_numOfDevices)) {
            const char *pType = BTRMGR_GetDeviceTypeAsString (pairedDevList.m_deviceProperty[devIndex-1].m_deviceType);
            if (pType)
            {
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Paired Devices (index : %d) device type is \'%s\'.\n", __FUNCTION__, devIndex-1, pType);
                strncpy(stMsgData->paramValue, pType, TR69HOSTIFMGR_MAX_PARAM_LEN);
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: Failed; Null Name\r\n",__FUNCTION__);
                return NOK;
            }
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: Failed due to wrong device index %d\r\n",__FUNCTION__, devIndex);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_StringType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}

/************************************************************
 * Description  : Name of Connected devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.{i}.Name' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Name of the paired devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getConnectedDevice_Name(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        int devIndex = stMsgData->instanceNum;
        if((devIndex && connectedDevList.m_numOfDevices) && (devIndex <= connectedDevList.m_numOfDevices)) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Connected Devices (index : %d) profile is \'%s\'.\n", __FUNCTION__, devIndex-1, connectedDevList.m_deviceProperty[devIndex-1].m_name);
            strncpy(stMsgData->paramValue, connectedDevList.m_deviceProperty[devIndex-1].m_name, TR69HOSTIFMGR_MAX_PARAM_LEN);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: Failed due to wrong device index %d\r\n",__FUNCTION__, devIndex);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_StringType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}



/************************************************************
 * Description  : LowEnergyEnabled of Connected devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.{i}.LowEnergyEnabled' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> LowEnergyEnabled of the connected devices
************************************************************/
#if 0 // Not required
int hostIf_DeviceInfoRdk_xBT::getConnectedDevice_LowEnergyEnabled(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        int devIndex = stMsgData->instanceNum;
        if((devIndex && connectedDevList.m_numOfDevices) && (devIndex <= connectedDevList.m_numOfDevices)) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Connected Device (index : %d) IsLowEnergyEnabled \'%d\'.\n", __FUNCTION__, devIndex-1, connectedDevList.m_deviceProperty[devIndex-1].m_isLowEnergyDevice);
            put_int(stMsgData->paramValue, connectedDevList.m_deviceProperty[devIndex-1].m_isLowEnergyDevice);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: Failed due to wrong device index %d\r\n",__FUNCTION__, devIndex);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_UnsignedIntType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}
#endif

/************************************************************
 * Description  : Active state of Connected devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.{i}.Active' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Active state of the connected devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getConnectedDevice_Active(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        int devIndex = stMsgData->instanceNum;
        if((devIndex && connectedDevList.m_numOfDevices) && (devIndex <= connectedDevList.m_numOfDevices)) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Connected Device (index : %d) active status \'%d\'.\n", __FUNCTION__, devIndex-1, connectedDevList.m_deviceProperty[devIndex-1].m_powerStatus);
            if (BTRMGR_DEVICE_POWER_ACTIVE == connectedDevList.m_deviceProperty[devIndex-1].m_powerStatus)
                strncpy(stMsgData->paramValue, "ACTIVE", TR69HOSTIFMGR_MAX_PARAM_LEN);
            else if (BTRMGR_DEVICE_POWER_LOW == connectedDevList.m_deviceProperty[devIndex-1].m_powerStatus)
                strncpy(stMsgData->paramValue, "LOW_POWER", TR69HOSTIFMGR_MAX_PARAM_LEN);
            else if (BTRMGR_DEVICE_POWER_STANDBY == connectedDevList.m_deviceProperty[devIndex-1].m_powerStatus)
                strncpy(stMsgData->paramValue, "STANDBY", TR69HOSTIFMGR_MAX_PARAM_LEN);
            else
                strncpy(stMsgData->paramValue, "UNKNOWN", TR69HOSTIFMGR_MAX_PARAM_LEN);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: Failed due to wrong device index %d\r\n",__FUNCTION__, devIndex);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_StringType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}


/************************************************************
* Description  : Manufacturer of Connected devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.Manufacturer' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> DeviceID of the DeviceInfo
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getDeviceInfo_DeviceID(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        stMsgData->paramtype = hostIf_StringType;;
        if(handle_devInfo) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: The Device ID is \'%llu\'.\n", __FUNCTION__, __LINE__, deviceProperty.m_deviceHandle);
            memset(stMsgData->paramValue, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN);
            sprintf(stMsgData->paramValue,"%llu" ,deviceProperty.m_deviceHandle);
        }
        else
        {
            RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Failed to get, since not Set for '%s.%s'.\r\n", __FUNCTION__,__LINE__, X_BT_ROOT_OBJ, BT_DEV_GETDEVICEINFO_STRING);
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}


/************************************************************
* Description  : Manufacturer of Connected devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.Manufacturer' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Manufacturer of the paired devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getDeviceInfo_Manufacturer(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        if(handle_devInfo) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: The Device Manufacturer/vendorID :  \'%d\'.\n", __FUNCTION__, __LINE__, deviceProperty.m_vendorID);
            put_int(stMsgData->paramValue, deviceProperty.m_vendorID);
        }
        else
        {
            RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Failed to get, since not Set for '%s.%s'.\r\n", __FUNCTION__,__LINE__, X_BT_ROOT_OBJ, BT_DEV_GETDEVICEINFO_STRING);
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_IntegerType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}


/************************************************************
* Description  : Profile of Connected devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo.Profile' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Profile of the DeviceInfo devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getDeviceInfo_Profile(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    stMsgData->paramtype=hostIf_StringType;

    try {

        int numOfService = deviceProperty.m_serviceInfo.m_numOfService;

        if(handle_devInfo && numOfService) {
            char serviceInfo[TR69HOSTIFMGR_MAX_PARAM_LEN];
            memset (&serviceInfo, '\0', sizeof(serviceInfo));
            for (int i = 0; i < deviceProperty.m_serviceInfo.m_numOfService; i++)
            {
                strcat(serviceInfo, deviceProperty.m_serviceInfo.m_profileInfo[i].m_profile);
                if ( (i + 1) < deviceProperty.m_serviceInfo.m_numOfService)
                    strcat(serviceInfo, ",");
            }

            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Profile is \'%s\'.\n", __FUNCTION__, __LINE__, serviceInfo);
            strncpy(stMsgData->paramValue, serviceInfo, TR69HOSTIFMGR_MAX_PARAM_LEN);
        }
        else
        {
//            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: DeviceProperty.m_serviceInfo.m_numOfService is %d.\r\n",__FUNCTION__, numOfService);
            RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Failed to get, since not Set for '%s.%s'.\r\n", __FUNCTION__,__LINE__, X_BT_ROOT_OBJ, BT_DEV_GETDEVICEINFO_STRING);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}


/************************************************************
 * Description  : MAC of Connected devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.{i}.MAC' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Mac of the connected devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getDeviceInfo_MAC(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        if( handle_devInfo) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: The Devices MAC is \'%s\'.\n", __FUNCTION__, __LINE__, deviceProperty.m_deviceAddress);
            memset(stMsgData->paramValue, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN);
            strncpy(stMsgData->paramValue, deviceProperty.m_deviceAddress, TR69HOSTIFMGR_MAX_PARAM_LEN);
        }
        else
        {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Failed since not Set for '%s.%s'.\r\n", __FUNCTION__,__LINE__, X_BT_ROOT_OBJ, BT_DEV_GETDEVICEINFO_STRING);
//            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: The Devices MAC is Empty. \n",__FUNCTION__, __LINE__);
        }

    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_StringType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}



/************************************************************
 * Description  : Signal Level of Connected devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.{i}.SignalLevel' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> SignalLevel of the connected devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getDeviceInfo_SignalLevel(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        if(handle_devInfo)
        {
            memset(stMsgData->paramValue, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN);
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: The Device signal level \'%d\'.\n", __FUNCTION__, __LINE__, deviceProperty.m_signalLevel);
            sprintf(stMsgData->paramValue, "%d", deviceProperty.m_signalLevel);
        }
        else
        {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Failed since not Set for '%s.%s'.\r\n", __FUNCTION__,__LINE__, X_BT_ROOT_OBJ, BT_DEV_GETDEVICEINFO_STRING);
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    stMsgData->paramtype = hostIf_StringType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}

/************************************************************
 * Description  : RSSI of Connected devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.{i}.RSSI' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> RSSI of the connected devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getDeviceInfo_RSSI (HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        if(handle_devInfo) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: The DeviceInfo RSSI \'%d\'.\n", __FUNCTION__, __LINE__, deviceProperty.m_rssi);
            put_int(stMsgData->paramValue, deviceProperty.m_rssi);
        }
        else {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Failed since not Set for '%s.%s'.\r\n", __FUNCTION__,__LINE__, X_BT_ROOT_OBJ, BT_DEV_GETDEVICEINFO_STRING);
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exception : %s\r\n",__FUNCTION__,__LINE__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_IntegerType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}

/************************************************************
 * Description  : DeviceType of Connected devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.{i}.DeviceType' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Device Type of the connected devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getConnectedDevice_DeviceType(HOSTIF_MsgData_t *stMsgData) {
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        int devIndex = stMsgData->instanceNum;
        if((devIndex && connectedDevList.m_numOfDevices) && (devIndex <= connectedDevList.m_numOfDevices)) {
            const char *pType = BTRMGR_GetDeviceTypeAsString (connectedDevList.m_deviceProperty[devIndex-1].m_deviceType);
            if (pType)
            {
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Connected Devices (index : %d) device type is \'%s\'.\n", __FUNCTION__, devIndex-1, pType);
                strncpy(stMsgData->paramValue, pType, TR69HOSTIFMGR_MAX_PARAM_LEN);
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Failed; Null Name\r\n",__FUNCTION__, __LINE__);
                return NOK;
            }
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Failed due to wrong device index %d\r\n",__FUNCTION__, __LINE__, devIndex);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exception : %s\r\n",__FUNCTION__, __LINE__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_StringType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}


/************************************************************
 * Description  : Name of Connected devices and get the
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice.{i}.DeviceID' value.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> DeviceID of the paired devices
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getConnectedDevice_DeviceID(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {
        int devIndex = stMsgData->instanceNum;
        stMsgData->paramtype = hostIf_StringType;;
        if((devIndex && connectedDevList.m_numOfDevices) && (devIndex <= connectedDevList.m_numOfDevices)) {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]xBlueTooth: The Connected Devices (index : %d) DeviceID is \'%llu\'.\n", __FUNCTION__, devIndex-1, connectedDevList.m_deviceProperty[devIndex-1].m_deviceHandle);
            memset(stMsgData->paramValue, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN);
            sprintf(stMsgData->paramValue, "%llu", connectedDevList.m_deviceProperty[devIndex-1].m_deviceHandle);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]xBlueTooth: Failed due to wrong device index %d\r\n",__FUNCTION__, devIndex);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s]xBlueTooth: Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}

/************************************************************
 * Description  : Get api for
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.GetDeviceInfo' parameter.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> OK on successful set or NOK
************************************************************/
int hostIf_DeviceInfoRdk_xBT::getDeviceInfo (HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {

        if(handle_devInfo) {
            memset(stMsgData->paramValue, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN);
            sprintf(stMsgData->paramValue, "%llu", deviceProperty.m_deviceHandle);
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: GetDeviceInfo value %llu, converted string is (%s). \r\n",  __FUNCTION__,__LINE__,
                    deviceProperty.m_deviceHandle, stMsgData->paramValue);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Failed to return any value, since this is not Set for \"%s.%s\". \r\n",__FUNCTION__, __LINE__, X_BT_ROOT_OBJ, BT_DEV_GETDEVICEINFO_STRING);

            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exception : %s\r\n",__FUNCTION__,__LINE__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_StringType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}


/************************************************************
 * Description  : Set api for
 * 				 'Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.GetDeviceInfo' parameter.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> OK on successful set or NOK
	              R/W deviceID input parameter. When written
	              set should call getDeviceInfo and populate
************************************************************/
int hostIf_DeviceInfoRdk_xBT::setDeviceInfo (HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    try {

        if(stMsgData->paramValue[0] != '\0') {
            BTRMgrDeviceHandle handle = 0;
            int base = 16;
            char *stopstring = NULL;

            handle = strtoll(stMsgData->paramValue, NULL, 0);

            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Received Set GetDeviceInfo value : %s and converted value as : %llu\r\n",
                    __FUNCTION__,__LINE__, stMsgData->paramValue, handle);

            memset (&deviceProperty, 0, sizeof (deviceProperty));

            if (BTRMGR_RESULT_SUCCESS != BTRMGR_GetDeviceProperties(0, handle, &deviceProperty))
            {
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Failed in  BTRMGR_GetDeviceProperties for \'%s\'.\n", __FUNCTION__, __LINE__, stMsgData->paramValue);
                return NOK;
            }
            else
            {
                handle_devInfo = handle;
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Successfully BTRMGR_GetDeviceProperties.\n", __FUNCTION__, __LINE__);
            }
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Failed due to empty GetDeviceInfo value %s\r\n",__FUNCTION__, __LINE__,stMsgData->paramValue);
            return NOK;
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exception : %s\r\n",__FUNCTION__,__LINE__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_UnsignedLongType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}

/************************************************************
 * Description  : This method is to fetch Bluetooth Discovered Devices details.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Device Type of the connected devices
************************************************************/
void fetch_Bluetooth_DiscoveredDevicesList()
{
    time_t currExTime = time (NULL);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    if ((currExTime - firstExTimeDisList ) > BTRMGR_QUERY_INTERVAL)
    {
        memset (&disDevList, '\0', sizeof(disDevList));
        if (BTRMGR_RESULT_SUCCESS != BTRMGR_GetDiscoveredDevices(0, &disDevList)) {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s]xBlueTooth: Failed in BTRMGR_GetDiscoveredDevices. \n", __FUNCTION__);
        } else
        {

            if(disDevList.m_numOfDevices >= MAX_DISCOVERY_DEVICE_NUM) {
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s]xBlueTooth: Successfully in BTRMGR_GetDiscoveredDevices, Failed due to Invalid  device count (%d). \n", __FUNCTION__, disDevList.m_numOfDevices);
            }
            else
            {
                int j = 0;
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s]xBlueTooth: Successfully gets the Discovered Devices details. Total count is (%d). \n", __FUNCTION__, disDevList.m_numOfDevices);
                for (; j< disDevList.m_numOfDevices; j++)
                {
                    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%d \t %llu \t %s \t %s\n", j,
                            disDevList.m_deviceProperty[j].m_deviceHandle,
                            disDevList.m_deviceProperty[j].m_name,
                            disDevList.m_deviceProperty[j].m_deviceAddress);
                }
            }
        }
        firstExTimeDisList = time (NULL);
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
}

/************************************************************
 * Description  : This method is to fetch Bluetooth Paired Devices details.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Device Type of the connected devices
************************************************************/
void fetch_Bluetooth_PairedDevicesList()
{
    time_t currExTime = time (NULL);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);

    if ((currExTime - firstExTimePairedList ) > BTRMGR_QUERY_INTERVAL)
    {
        memset (&pairedDevList, 0, sizeof(pairedDevList));

        if (BTRMGR_RESULT_SUCCESS != BTRMGR_GetPairedDevices(0, &pairedDevList)) {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s]xBlueTooth: Failed in BTRMGR_GetPairedDevices. \n", __FUNCTION__);
        }
        else
        {
            if(disDevList.m_numOfDevices >= MAX_PAIRED_DEVICE_NUM) {
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s]xBlueTooth: Successfully in BTRMGR_GetPairedDevices, Failed due to Invalid  device count (%d). \n", __FUNCTION__, pairedDevList.m_numOfDevices);
            }
            else
            {
                int j = 0;
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s]xBlueTooth: Successfully get Paired Devices details. Total count is (%d). \n", __FUNCTION__, pairedDevList.m_numOfDevices);
                for (; j< pairedDevList.m_numOfDevices; j++)
                {
                    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%d \t %llu \t %s \t %s\n", j,
                            pairedDevList.m_deviceProperty[j].m_deviceHandle,
                            pairedDevList.m_deviceProperty[j].m_name,
                            pairedDevList.m_deviceProperty[j].m_deviceAddress);
                }
            }
        }
        firstExTimePairedList = time (NULL);
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
}

/************************************************************
 * Description  : This method is to fetch Bluetooth Connected Devices details..
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Device Type of the connected devices
************************************************************/
void fetch_Bluetooth_ConnectedDevicesList()
{
    time_t currExTime = time (NULL);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);
    if ((currExTime - firstExTimeConnList ) > BTRMGR_QUERY_INTERVAL)
    {
        memset (&connectedDevList, 0, sizeof(connectedDevList));

        if (BTRMGR_RESULT_SUCCESS != BTRMGR_GetConnectedDevices(0, &connectedDevList)) {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s:%d]xBlueTooth: Failed in BTRMGR_GetConnectedDevices. \n", __FUNCTION__, __LINE__);
        }
        else
        {
            if(disDevList.m_numOfDevices >= MAX_CONNECTED_DEVICE_NUM) {
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s]xBlueTooth: Successfully in BTRMGR_GetConnectedDevices, Failed due to Invalid  device count (%d). \n", __FUNCTION__, connectedDevList.m_numOfDevices);
            }
            else {
                int j = 0;
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s]xBlueTooth: Successfully gets the Connected Devices details. Total count is (%d). \n", __FUNCTION__, connectedDevList.m_numOfDevices);
                for (; j< connectedDevList.m_numOfDevices; j++)
                {
                    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%d \t %llu \t %s \t %s\n", j,
                            connectedDevList.m_deviceProperty[j].m_deviceHandle,
                            connectedDevList.m_deviceProperty[j].m_name,
                            connectedDevList.m_deviceProperty[j].m_deviceAddress);
                }
            }
        }
        firstExTimeConnList = time (NULL);
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
}

/************************************************************
 * Description  : This method is register to BTRMGR_RegisterEventCallback method.
 * 				  It gives the all the events and device count details with additional details.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Event data for bluetooth connected devices
************************************************************/
#if 0
void BTRMgr_EventCallback(BTRMGR_EventMessage_t eventData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);
    BTRMGR_Events_t eventType = eventData.m_eventType;
    unsigned short numOfDevices = eventData.m_numOfDevices;

    switch (eventType) {
    case BTRMGR_EVENT_DEVICE_DISCOVERY_COMPLETE :
        hostIf_DeviceInfoRdk_xBT::notifyAddDelEvent(numOfDevices, (char *)X_BT_DISCOVERED_DEV_OBJ);
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Received \"BTRMGR_EVENT_DEVICE_DISCOVERY_COMPLETE\" (%d).\n", __FUNCTION__, __LINE__, eventType);
        break;
    case BTRMGR_EVENT_DEVICE_PAIRING_COMPLETE :
    case BTRMGR_EVENT_DEVICE_UNPAIRING_COMPLETE :
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Received \"%s\" (%d).\n", __FUNCTION__, __LINE__,
                BTRMGR_EVENT_DEVICE_PAIRING_COMPLETE?"BTRMGR_EVENT_DEVICE_PAIRING_COMPLETE":"BTRMGR_EVENT_DEVICE_UPAIRING_COMPLETE",
                eventType);
        hostIf_DeviceInfoRdk_xBT::notifyAddDelEvent(numOfDevices, X_BT_PAIRED_DEV_OBJ);
        break;
    case BTRMGR_EVENT_DEVICE_CONNECTION_COMPLETE :
    case BTRMGR_EVENT_DEVICE_DISCONNECT_COMPLETE :
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Received \"%s\" (%d).\n", __FUNCTION__, __LINE__,
                BTRMGR_EVENT_DEVICE_CONNECTION_COMPLETE?"BTRMGR_EVENT_DEVICE_CONNECTION_COMPLETE":"BTRMGR_EVENT_DEVICE_DISCONNECT_COMPLETE",
                eventType);
        hostIf_DeviceInfoRdk_xBT::notifyAddDelEvent(numOfDevices, X_BT_CONNECTED_DEV_OBJ);
        break;
    default:
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Invalid BTRMGR_Event (%d) received.\n",	__FUNCTION__, __LINE__, eventType);
        break;
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
}
#endif

/************************************************************
 * Description  : This method use to send IARM notification to add or delete events
 * 				  for discovery, paired and connection table entries.
 * 				  The tr-069 agent will listen and add or delete the table entries.
 *
 * Precondition : Bluetooth stack should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> Device Type of the connected devices
************************************************************/
void hostIf_DeviceInfoRdk_xBT::notifyAddDelEvent(unsigned short numOfDevEntry, char *tblObj)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Entering..\n", __FUNCTION__, __LINE__);
    updateCallback mUpdateCallback;
    HOSTIF_MsgData_t msgData;
    memset(&msgData,0,sizeof(msgData));
    char tmp_buff[TR69HOSTIFMGR_MAX_PARAM_LEN] = {'\0'};
    msgData.paramtype = hostIf_IntegerType;

    IARM_Bus_tr69HostIfMgr_EventData_t eventData;

    try {
        if(tblObj && (0 == strcasecmp(X_BT_DISCOVERED_DEV_OBJ, tblObj)))
        {
            unsigned short disDevCnt = 0;
            fetch_Bluetooth_DiscoveredDevicesList();
            disDevCnt = disDevList.m_numOfDevices;
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Total Bluetooth Discovered Device Count : %d.\n", __FUNCTION__, __LINE__, disDevCnt);
            char tmp[TR69HOSTIFMGR_MAX_PARAM_LEN] = "";
            sprintf(tmp_buff, tblObj);

            while(noOfDiscoveredDevice > disDevCnt)
            {
                sprintf(tmp,"%s.%d.",tmp_buff,disDevCnt);
                if(tmp[0] != '\0') {
                    memset(&eventData, '\0', sizeof(eventData));
                    strncpy(eventData.paramName, tmp, TR69HOSTIFMGR_MAX_PARAM_LEN);
                    eventData.paramtype = hostIf_IntegerType;
                    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Sending IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE for [\"%s\"].\n", __FUNCTION__, __LINE__, tmp);
                    IARM_Bus_BroadcastEvent(IARM_BUS_TR69HOSTIFMGR_NAME, (IARM_EventId_t) IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE, (void *)&eventData, sizeof(eventData));
                }
                disDevCnt++;
            }
            while(noOfDiscoveredDevice < disDevCnt)
            {
                sprintf(tmp,"%s.",tmp_buff);
                if(tmp[0] != '\0') {
                    memset(&eventData, '\0', sizeof(eventData));
                    strncpy(eventData.paramName, tmp, TR69HOSTIFMGR_MAX_PARAM_LEN);
                    eventData.paramtype = hostIf_IntegerType;
                    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Sending IARM_BUS_TR69HOSTIFMGR_EVENT_ADD for [\"%s\"].\n", __FUNCTION__, __LINE__, tmp);
                    IARM_Bus_BroadcastEvent(IARM_BUS_TR69HOSTIFMGR_NAME, (IARM_EventId_t) IARM_BUS_TR69HOSTIFMGR_EVENT_ADD, (void *)&eventData, sizeof(eventData));
                }
                disDevCnt--;
            }
            noOfDiscoveredDevice = disDevList.m_numOfDevices;
        }
        else if((NULL != tblObj) && (0 == strcasecmp(X_BT_PAIRED_DEV_OBJ, tblObj)))
        {
            fetch_Bluetooth_PairedDevicesList();
            unsigned short pairedDevCnt = pairedDevList.m_numOfDevices;
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: The bluetooth PairedDeviceCnt : %d.\n", __FUNCTION__, __LINE__, pairedDevCnt);
            char tmp[TR69HOSTIFMGR_MAX_PARAM_LEN] = "";
            sprintf(tmp_buff, X_BT_PAIRED_DEV_OBJ);

            while(noOfPairedDevice > pairedDevCnt)
            {
                sprintf(tmp,"%s.%d.",tmp_buff,pairedDevCnt);
                if(tmp[0] != '\0') {
                    memset(&eventData, '\0', sizeof(eventData));
                    strncpy(eventData.paramName, tmp, TR69HOSTIFMGR_MAX_PARAM_LEN);
                    eventData.paramtype = hostIf_IntegerType;
                    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Sending IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE for [\"%s\"].\n", __FUNCTION__, __LINE__, tmp);
                    IARM_Bus_BroadcastEvent(IARM_BUS_TR69HOSTIFMGR_NAME, (IARM_EventId_t) IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE, (void *)&eventData, sizeof(eventData));
                }
                pairedDevCnt++;
            }
            while(noOfPairedDevice < pairedDevCnt)
            {
                sprintf(tmp,"%s.",tmp_buff);
                if(tmp[0] != '\0') {
                    memset(&eventData, '\0', sizeof(eventData));
                    strncpy(eventData.paramName, tmp, TR69HOSTIFMGR_MAX_PARAM_LEN);
                    eventData.paramtype = hostIf_IntegerType;
                    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Sending IARM_BUS_TR69HOSTIFMGR_EVENT_ADD for [\"%s\"].\n", __FUNCTION__, __LINE__, tmp);
                    IARM_Bus_BroadcastEvent(IARM_BUS_TR69HOSTIFMGR_NAME, (IARM_EventId_t) IARM_BUS_TR69HOSTIFMGR_EVENT_ADD, (void *)&eventData, sizeof(eventData));
                }

                pairedDevCnt--;
            }
            noOfPairedDevice = pairedDevList.m_numOfDevices;
        }
        else if((NULL != tblObj) && (0 == strcasecmp(X_BT_CONNECTED_DEV_OBJ, tblObj)))
        {
            fetch_Bluetooth_ConnectedDevicesList();
            unsigned short connDevCnt = connectedDevList.m_numOfDevices;
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: ConnectedDeviceCnt : %d.\n", __FUNCTION__, __LINE__, connDevCnt);
            char tmp[TR69HOSTIFMGR_MAX_PARAM_LEN] = "";
            sprintf(tmp_buff, X_BT_CONNECTED_DEV_OBJ);

            while(noOfConnectedDevices > connDevCnt)
            {
                sprintf(tmp,"%s.%d.",tmp_buff,connDevCnt);
                if(tmp[0] != '\0') {
                    memset(&eventData, '\0', sizeof(eventData));
                    strncpy(eventData.paramName, tmp, TR69HOSTIFMGR_MAX_PARAM_LEN);
                    eventData.paramtype = hostIf_IntegerType;
                    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Sending IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE for [\"%s\"].\n", __FUNCTION__, __LINE__, tmp);
                    IARM_Bus_BroadcastEvent(IARM_BUS_TR69HOSTIFMGR_NAME, (IARM_EventId_t) IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE, (void *)&eventData, sizeof(eventData));
                }
                connDevCnt++;
            }
            while(noOfConnectedDevices < connDevCnt)
            {
                sprintf(tmp,"%s.",tmp_buff);
                if(tmp[0] != '\0') {
                    memset(&eventData, '\0', sizeof(eventData));
                    strncpy(eventData.paramName, tmp, TR69HOSTIFMGR_MAX_PARAM_LEN);
                    eventData.paramtype = hostIf_IntegerType;
                    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Sending IARM_BUS_TR69HOSTIFMGR_EVENT_ADD for [\"%s\"].\n", __FUNCTION__, __LINE__, tmp);
                    IARM_Bus_BroadcastEvent(IARM_BUS_TR69HOSTIFMGR_NAME, (IARM_EventId_t) IARM_BUS_TR69HOSTIFMGR_EVENT_ADD, (void *)&eventData, sizeof(eventData));
                }
                connDevCnt--;
            }
            noOfConnectedDevices = connectedDevList.m_numOfDevices;
        }

    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exception : %s\r\n",__FUNCTION__, __LINE__, e.what());
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xBlueTooth: Exiting..\n", __FUNCTION__, __LINE__);
}


#endif

/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */


/** @} */
/** @} */
