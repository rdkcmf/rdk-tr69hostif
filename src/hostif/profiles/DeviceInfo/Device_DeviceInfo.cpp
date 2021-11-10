/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016-2019 RDK Management
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
 * @file Device_DeviceInfo.cpp
 * @brief This source file contains the APIs for getting device information.
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


#include <fstream>
#include <cmath>
#include <cstring>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include <sys/inotify.h>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "libIBus.h"
#include "mfrMgr.h"
#include "Device_DeviceInfo.h"
#include "hostIf_utils.h"
#include "pwrMgr.h"
#include <curl/curl.h>

#include "dsTypes.h"
#include "host.hpp"
#include "manager.hpp"
#include "dsError.h"
#include "audioOutputPort.hpp"
#include "sysMgr.h"
#ifdef MEDIA_CLIENT
#include "netsrvmgrIarm.h"
#endif

#ifdef YOCTO_BUILD
#include "secure_wrapper.h"
#endif

#ifdef USE_MoCA_PROFILE
#include "Device_MoCA_Interface.h"
#endif
#ifdef USE_XRESRC
#include "Device_XComcast_Xcalibur_Client_XRE_ConnectionTable.h"
#endif
#if USE_HWSELFTEST_PROFILE
#include "DeviceInfo_hwHealthTest.h"
#endif

#include "hostIf_NotificationHandler.h"
#include "safec_lib.h"

#define VERSION_FILE                       "/version.txt"
#define SOC_ID_FILE                        "/var/log/socprov.log"
#define PREFERRED_GATEWAY_FILE	           "/opt/prefered-gateway"
#define FORWARD_SSH_FILE                   "/opt/secure/.RFC_ForwardSSH"
#define GATEWAY_NAME_SIZE                  4
#define IPREMOTE_SUPPORT_STATUS_FILE       "/opt/.ipremote_status"
#define XRE_CONTAINER_SUPPORT_STATUS_FILE  "/opt/XRE_container_enable"
#define IPREMOTE_INTERFACE_INFO            "/tmp/ipremote_interface_info"
#define MODEL_NAME_FILE                    "/tmp/.model"
#define PREVIOUS_REBOT_REASON_FILE         "/opt/secure/reboot/previousreboot.info"
#define TR069DOSLIMIT_THRESHOLD            "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.Tr069DoSLimit.Threshold"
#define MIN_TR69_DOS_THRESHOLD 0
#define MAX_TR69_DOS_THRESHOLD 30
#define HTTP_OK 200

/* Localhost port range for stunnel client to listen/accept */
#define MIN_PORT_RANGE 3000
#define MAX_PORT_RANGE 3020

GHashTable* hostIf_DeviceInfo::ifHash = NULL;
GHashTable* hostIf_DeviceInfo::m_notifyHash = NULL;
GMutex* hostIf_DeviceInfo::m_mutex = NULL;

void *ResetFunc(void *);


static int get_ParamValue_From_TR69Agent(HOSTIF_MsgData_t *);
static int get_PartnerId_From_Curl(string& );
static char stbMacCache[TR69HOSTIFMGR_MAX_PARAM_LEN] = {'\0'};
static string reverseSSHArgs;
map<string,string> stunnelSSHArgs;
const string sshCommand = "/lib/rdk/startTunnel.sh";
const string stunnelCommand = "/lib/rdk/startStunnel.sh";
const string watchTunnelCommand = "/lib/rdk/watchTunnel.sh";

string hostIf_DeviceInfo::m_xFirmwareDownloadProtocol;
string hostIf_DeviceInfo::m_xFirmwareDownloadURL;
string hostIf_DeviceInfo::m_xFirmwareToDownload;
bool hostIf_DeviceInfo::m_xFirmwareDownloadNow;
bool hostIf_DeviceInfo::m_xFirmwareDownloadUseCodebig;
bool hostIf_DeviceInfo::m_xFirmwareDownloadDeferReboot;

#ifndef NEW_HTTP_SERVER_DISABLE
XRFCStore* hostIf_DeviceInfo::m_rfcStore;
XRFCStorage hostIf_DeviceInfo::m_rfcStorage;
#else
XRFCStorage hostIf_DeviceInfo::m_rfcStorage;
#endif
XBSStore* hostIf_DeviceInfo::m_bsStore;
string hostIf_DeviceInfo::m_xrPollingAction = "0";

/****************************************************************************************************************************************************/
// Device.DeviceInfo Profile. Getters:
/****************************************************************************************************************************************************/

/**
 * @brief Class Constructor of the class hostIf_DeviceInfo.
 *
 * It memset the private members variables of the class such as backupSoftwareVersion, backupSerialNumber,
 * backupManufacturer, backupModelName etc.
 */
hostIf_DeviceInfo::hostIf_DeviceInfo(int dev_id):
    dev_id(dev_id),
    bCalledSoftwareVersion(0),
    bCalledX_COMCAST_COM_STB_IP(0),
    bCalledX_COMCAST_COM_FirmwareFilename(0),
    bCalledSerialNumber(false),
    bCalledProductClass(false),
    bCalledAdditionalSoftwareVersion(false),
    bCalledManufacturer(false),
    bCalledManufacturerOUI(false),
    bCalledModelName(false),
    bCalledHardwareVersion(false),
    bCalledDeviceMAC(false)
{
    memset(backupSoftwareVersion, 0, _BUF_LEN_16);
    memset(backupSerialNumber, 0, _BUF_LEN_16);
    memset(backupProductClass, 0, _BUF_LEN_16);
    memset(backupManufacturer, 0, _BUF_LEN_16);
    memset(backupManufacturerOUI, 0, _BUF_LEN_16);
    memset(backupModelName, 0, _BUF_LEN_16);
    memset(backupHardwareVersion, 0, _BUF_LEN_16);
    memset(backupAdditionalSoftwareVersion, 0, _BUF_LEN_16);
    memset(backupDeviceMAC, 0, _BUF_LEN_32);
    memset(backupX_COMCAST_COM_STB_IP, 0, _BUF_LEN_64);
    memset(backupX_COMCAST_COM_FirmwareFilename, 0, _BUF_LEN_64);
#ifndef NEW_HTTP_SERVER_DISABLE
    if(!legacyRFCEnabled())
        m_rfcStore = XRFCStore::getInstance();
#endif
    m_bsStore = XBSStore::getInstance();
}
hostIf_DeviceInfo::~hostIf_DeviceInfo()
{
    if(m_notifyHash)
    {
        g_hash_table_destroy(m_notifyHash);
    }
}

hostIf_DeviceInfo* hostIf_DeviceInfo::getInstance(int dev_id)
{
    hostIf_DeviceInfo* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_DeviceInfo *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_DeviceInfo(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create MoCA Interface instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_DeviceInfo::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_DeviceInfo::closeInstance(hostIf_DeviceInfo *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_DeviceInfo::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_DeviceInfo* pDev = (hostIf_DeviceInfo *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

void hostIf_DeviceInfo::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void hostIf_DeviceInfo::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

GHashTable*  hostIf_DeviceInfo::getNotifyHash()
{
    if(m_notifyHash)
    {
        return m_notifyHash;
    }
    else
    {
        return m_notifyHash = g_hash_table_new(g_str_hash, g_str_equal);
    }
}

/**
 * @brief This function provides the Identifier of the particular device that is
 * unique for the indicated class of product and manufacturer. This is the Serial Number of the box.
 * This value MUST remain fixed over the lifetime of the device, including
 * across firmware updates. Any change would indicate that it's a new device
 * and would therefore require to inform BOOTSTRAP.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successful.
 * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_SerialNumber(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    int ret=NOT_HANDLED;
    stMsgData->paramtype = hostIf_StringType;

#if !defined (USE_DEV_PROPERTIES_CONF)
    IARM_Bus_MFRLib_GetSerializedData_Param_t param;
    memset(&param, 0, sizeof(param));
    IARM_Result_t iarm_ret = IARM_RESULT_IPCCORE_FAIL;
    param.type = mfrSERIALIZED_TYPE_SERIALNUMBER;
    iarm_ret = IARM_Bus_Call(IARM_BUS_MFRLIB_NAME, IARM_BUS_MFRLIB_API_GetSerializedData, &param, sizeof(param));

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] IARM_BUS_MFRLIB_API_GetSerializedData returns params: %s with paramlen: %d.\r\n",__FUNCTION__, param.buffer, param.bufLen);
    if(iarm_ret == IARM_RESULT_SUCCESS)
    {
        try
        {
            if(param.buffer && param.bufLen)
            {
                strncpy((char *)stMsgData->paramValue, param.buffer, param.bufLen);
                stMsgData->paramValue[param.bufLen+1] = '\0';
                stMsgData->paramLen = param.bufLen;
                if(bCalledSerialNumber && pChanged && strncmp(stMsgData->paramValue,backupSerialNumber,_BUF_LEN_16 ))
                {
                    *pChanged =  true;
                }
                bCalledSerialNumber = true;
                strncpy(backupSerialNumber,stMsgData->paramValue,_BUF_LEN_16 );
                ret = OK;
            }
            else {
                ret = NOK;
                stMsgData->faultCode = fcInvalidParameterValue;
            }
        } catch (const std::exception e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
            stMsgData->faultCode = fcInvalidParameterValue;
            ret = NOK;
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Failed in IARM_Bus_Call() for parameter : %s [param.type:%d with error code:%d]\n",stMsgData->paramName,param.type, ret);
        ret = NOK;
    }
#else
    {
        IARM_Bus_SYSMgr_GetSystemStates_Param_t param;
        if(IARM_RESULT_SUCCESS == IARM_Bus_Call(IARM_BUS_SYSMGR_NAME, IARM_BUS_SYSMGR_API_GetSystemStates, &param, sizeof(param)))
        {
            if (param.stb_serial_no.payload != NULL)
            {
                snprintf(stMsgData->paramValue, _BUF_LEN_16, "%s", param.stb_serial_no.payload);
                RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] SerialNumber : \"%s\".\n",__FUNCTION__, param.stb_serial_no.payload );
                ret = OK;
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Null for SerialNumber from \"%s\".\n",__FUNCTION__,IARM_BUS_SYSMGR_NAME);
                stMsgData->faultCode = fcInvalidParameterValue;
                ret = NOK;
            }
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] The parameter \"%s\" is failed to get from \"%s\".\n",__FUNCTION__, stMsgData->paramName, IARM_BUS_SYSMGR_NAME);
            stMsgData->faultCode = fcInvalidParameterValue;
            ret = NOK;
        }
    }
#endif //!defined (USE_DEV_PROPERTIES_CONF)
    return ret;
}

/**
 * @brief This function identifying the Software/Firmware version of the running
 * image on the box (Vx.y.z). A string identifying the software version currently installed
 * in the CPE (i.e. version of the overall CPE firmware). To allow version comparisons,
 * this element SHOULD be in the form of dot-delimited integers, where each successive
 * integer represents a more minor category of variation.
 * For example, 3.0.21 where the components mean: Major.Minor.Build.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successful.
 * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_SoftwareVersion(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    string line;
    ifstream versionfile (VERSION_FILE);
    bool isTrunkbuild = false;
    bool isJenkinsbuild = false;
    bool versionFlag = false;
    char version[100] = {'\0'};
    errno_t rc = -1;

    try {
        if (versionfile.is_open())
        {
            while ( getline (versionfile,line) )
            {
                if (line.find("trunk") !=string::npos)
                {
                    isTrunkbuild = true;
                }
                if ((line.find("_VERSION") == string::npos) && (line.find("VERSION") != string::npos))
                {
                    char *tmpStr = strstr((char *)line.c_str(), "=");
                    tmpStr++;
                    while(isspace(*tmpStr)) {
                        tmpStr++;
                    }
                    rc=strcpy_s(version,sizeof(version), tmpStr);
		    if(rc!=EOK)
		    {
			ERR_CHK(rc);
		    }
                    versionFlag = true;
                    if(!isTrunkbuild)	break;
                }

                if(isTrunkbuild)
                {
                    if (line.find("JENKINS_BUILD_NUMBER") !=string::npos)
                    {
                        char *tmpStr = strstr((char *)line.c_str(), "=");
                        tmpStr++;
                        while(isspace(*tmpStr)) {
                            tmpStr++;
                        }
                        strcat(version, ".");
                        strcat(version,tmpStr);
                        isJenkinsbuild = true;
                    }
                    if(versionFlag &&isJenkinsbuild)
                    {
                        break;
                    }
                }
            }
            versionfile.close();
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s()] Failed to open file\n", __FUNCTION__);
            return NOK;
        }


        if(bCalledSoftwareVersion && pChanged && strncmp(version,backupSoftwareVersion,_BUF_LEN_16 ))
        {
            *pChanged =  true;
        }
        bCalledSoftwareVersion = true;
        strncpy(backupSoftwareVersion,version,_BUF_LEN_16 );
        stMsgData->paramLen = strlen(version);
        strncpy(stMsgData->paramValue,version, stMsgData->paramLen);
        stMsgData->paramtype = hostIf_StringType;
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s()]Exception caught.\n", __FUNCTION__);
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s()] Exiting..\n", __FUNCTION__ );
    return OK;
}

/**
 * @brief This function retrieves manufacturer specific data from the box using IARM Bus call.
 *  The IARM Manager gets the manufacture information from mfr library.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successful.
 * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_Manufacturer(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    int ret = NOT_HANDLED;
    stMsgData->paramtype = hostIf_StringType;
#if !defined (USE_DEV_PROPERTIES_CONF)
    IARM_Bus_MFRLib_GetSerializedData_Param_t param;
    IARM_Result_t iarm_ret = IARM_RESULT_IPCCORE_FAIL;
    memset(&param, 0, sizeof(param));
    param.type = mfrSERIALIZED_TYPE_MANUFACTURER;
    iarm_ret = IARM_Bus_Call(IARM_BUS_MFRLIB_NAME, IARM_BUS_MFRLIB_API_GetSerializedData, &param, sizeof(param));

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] IARM_BUS_MFRLIB_API_GetSerializedData returns params: %s with paramlen: %d.\r\n",__FUNCTION__, param.buffer, param.bufLen);
    if(iarm_ret == IARM_RESULT_SUCCESS)
    {
        try {
            if(param.buffer && param.bufLen)
            {
                strncpy((char *)stMsgData->paramValue, param.buffer, param.bufLen);
                stMsgData->paramValue[param.bufLen+1] = '\0';
                stMsgData->paramLen = param.bufLen;
                if(bCalledManufacturer && pChanged && strncmp(stMsgData->paramValue,backupManufacturer,_BUF_LEN_16 ))
                {
                    *pChanged =  true;
                }
                bCalledManufacturer = true;
                strncpy(backupManufacturer,stMsgData->paramValue,_BUF_LEN_16 );
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] paramValue: %s param.pBuffer: %s \n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramValue, param.buffer);
                ret = OK;
            }
            else
                ret = NOK;
        } catch (const std::exception e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
            ret = NOK;
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in IARM_Bus_Call() for parameter : %s [param.type:%d with error code:%d]\n",stMsgData->paramName,param.type, ret);
        ret = NOK;
    }
#else
    // Try to get the value from property files
    char* manufact = NULL;
    manufact = getenvOrDefault("MANUFACTURE","");
    if(manufact !=  NULL) {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] manufact = %s.\n",__FUNCTION__, __FILE__, __LINE__,manufact);
        int len = strlen(manufact);
        strncpy((char *)stMsgData->paramValue, manufact, len);
        stMsgData->paramValue[len+1] = '\0';
        stMsgData->paramLen = len;
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] paramValue: %s stMsgData->paramLen: %d \n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramValue,stMsgData->paramLen);
        ret = OK;
    } else {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] Failed to get manufacturer from device properties..!!\n",__FUNCTION__, __FILE__, __LINE__);
        ret = NOK;
    }
#endif //  #if !defined (USE_DEV_PROPERTIES_CONF)
    return ret;
}

/**
 * @brief This function provides the manufactureOUT information. Organizationally unique
 * identifier of the device manufacturer. Represented as a six hexadecimal-digit value using
 * all upper-case letters and including any leading zeros. Possible patterns: [0-9A-F]{6}.
 * This value MUST remain fixed over the lifetime of the device and also across
 * firmware updates. Any change would indicate that, it's a new device and would
 * therefore require a BOOTSTRAP.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if ManufacturerOUI was successfully fetched.
 * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_ManufacturerOUI(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    int ret=NOT_HANDLED;
    stMsgData->paramtype = hostIf_StringType;
#if !defined (USE_DEV_PROPERTIES_CONF)
    IARM_Bus_MFRLib_GetSerializedData_Param_t param;
    IARM_Result_t iarm_ret = IARM_RESULT_IPCCORE_FAIL;
    memset(&param, 0, sizeof(param));
    param.type = mfrSERIALIZED_TYPE_MANUFACTUREROUI;
    param.buffer[MAX_BUF] = {'\0'};
    iarm_ret = IARM_Bus_Call(IARM_BUS_MFRLIB_NAME, IARM_BUS_MFRLIB_API_GetSerializedData, &param, sizeof(param));

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] IARM_BUS_MFRLIB_API_GetSerializedData returns params: %s with paramlen: %d.\r\n",__FUNCTION__, param.buffer, param.bufLen);
    if(iarm_ret == IARM_RESULT_SUCCESS)
    {
        try
        {
            if( param.buffer && param.bufLen) {
                strncpy((char *)stMsgData->paramValue, param.buffer, param.bufLen);
                stMsgData->paramValue[param.bufLen+1] = '\0';
                stMsgData->paramLen = param.bufLen;
                if(bCalledManufacturerOUI && pChanged && strncmp(stMsgData->paramValue,backupManufacturerOUI,_BUF_LEN_16 ))
                {
                    *pChanged =  true;
                }
                bCalledManufacturerOUI = true;
                strncpy(backupManufacturerOUI,stMsgData->paramValue,_BUF_LEN_16 );
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] paramValue: %s param.pBuffer: %s \n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramValue, param.buffer);
                ret = OK;
            }
            else
                ret = NOK;
        } catch (const std::exception e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
            ret = NOK;
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF, "Failed in IARM_Bus_Call() for parameter : %s [param.type:%d with error code:%d]\n",stMsgData->paramName,param.type, ret);
        ret = NOK;
    }
#else

#endif //#if !defined (USE_DEV_PROPERTIES_CONF)
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s()]\n", __FUNCTION__);
    return ret;
}

/**
 * @brief This function provides the Model name of the device.
 * This MUST be based on Comcast_X_HW* specification and of the format TUVVVWXY.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if ManufacturerOUI was successfully fetched.
 * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_ModelName(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    int ret=NOT_HANDLED;
    stMsgData->paramtype = hostIf_StringType;
#if !defined (USE_DEV_PROPERTIES_CONF)
    IARM_Bus_MFRLib_GetSerializedData_Param_t param;
    IARM_Result_t iarm_ret = IARM_RESULT_IPCCORE_FAIL;
    memset(&param, 0, sizeof(param));
    param.type = mfrSERIALIZED_TYPE_MODELNAME;
    iarm_ret = IARM_Bus_Call(IARM_BUS_MFRLIB_NAME, IARM_BUS_MFRLIB_API_GetSerializedData, &param, sizeof(param));

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] IARM_BUS_MFRLIB_API_GetSerializedData returns params: %s with paramlen: %d.\r\n",__FUNCTION__, param.buffer, param.bufLen);

    if(iarm_ret == IARM_RESULT_SUCCESS)
    {
        try
        {
            if(param.buffer && param.bufLen) {
                strncpy((char *)stMsgData->paramValue, param.buffer, param.bufLen);
                stMsgData->paramValue[param.bufLen+1] = '\0';
                stMsgData->paramLen = param.bufLen;
                if(bCalledModelName && pChanged && strncmp(stMsgData->paramValue,backupModelName,_BUF_LEN_16 ))
                {
                    *pChanged =  true;
                }
                bCalledModelName = true;
                strncpy(backupModelName,stMsgData->paramValue,_BUF_LEN_16 );
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] paramValue: %s param.pBuffer: %s \n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramValue, param.buffer);
                ret = OK;
            }
            else
                ret = NOK;
        } catch (const std::exception e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
            ret = NOK;
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Failed in IARM_Bus_Call() for parameter : %s [param.type:%d with error code:%d]\n",stMsgData->paramName,param.type, ret);
        ret = NOK;
    }
#else
    // Try to get MODEL NAME from Device Properties
    FILE *fp = NULL;
    char modelName[64] = {'\0'};

    fp = fopen(MODEL_NAME_FILE,"r");
    if(fp == NULL) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] Failed to open MODEL Name file\n.!",__FUNCTION__, __FILE__, __LINE__);
        return NOK;
    }
    if(fgets(modelName, 64,fp)!=NULL) {
        // Remove new line char if any in model name
        int len = strlen(modelName);
        if(modelName[len-1] == '\n') modelName[len-1] = '\0';
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] modelName = %s.\n",__FUNCTION__, __FILE__, __LINE__,modelName);
        strncpy((char *)stMsgData->paramValue, modelName,sizeof(stMsgData->paramValue)-1);
        stMsgData->paramValue[sizeof(stMsgData->paramValue)-1] = '\0';
        stMsgData->paramLen = strlen(modelName);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] paramValue: %s stMsgData->paramLen: %d \n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramValue,stMsgData->paramLen);
        ret = OK;
    } else {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Failed to read model name.\n", __FUNCTION__);
    }
    fclose(fp);

#endif //#if !defined (USE_DEV_PROPERTIES_CONF)
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s()]\n", __FUNCTION__);
    return ret;

}

/**
 * @brief This function provides the A full description of the device.
 * Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns enum integer '-1' on method completion.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_Description(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    stMsgData->paramtype = hostIf_StringType;
    const char *desc = "TR-181, TR-135 and Comcast specific Datamodel Configuration";
    snprintf((char *)stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN-1, "%s", desc);
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    return OK;
}

/**
 * @brief This function provides the Identifier of the class of product for which
 * the serial number applies. That is, for a given manufacturer,
 * this parameter is used to identify the product or class of product
 * over which the SerialNumber parameter is unique. This value MUST remain fixed
 * over the lifetime of the device and also across firmware updates. Any change
 * would indicate that it's a new device and would therefore require a BOOTSTRAP.
 * Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns enum integer '-1' on method completion.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_ProductClass(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s()]\n", __FUNCTION__);
    stMsgData->paramtype = hostIf_StringType;

#ifndef FETCH_PRODUCTCLASS_FROM_MFRLIB
    /* Fixed DELIA-27160, always returns as OK */
    char *pc = NULL;
    pc = getenv((const char *)"RECEIVER_PLAT_TYPE");

    if(pc) {
        snprintf((char *)stMsgData->paramValue, TR69HOSTIFMGR_MAX_PARAM_LEN-1, "%s", pc);
        stMsgData->paramLen = strlen(stMsgData->paramValue);
    }
    return OK;
#else /* FETCH_PRODUCTCLASS_FROM_MFRLIB */
    int ret;
    IARM_Bus_MFRLib_GetSerializedData_Param_t param;
    IARM_Result_t iarm_ret = IARM_RESULT_IPCCORE_FAIL;
    memset(&param, 0, sizeof(param));
    param.type = mfrSERIALIZED_TYPE_PRODUCTCLASS;
    param.buffer[MAX_BUF] = {'\0'};
    iarm_ret = IARM_Bus_Call(IARM_BUS_MFRLIB_NAME, IARM_BUS_MFRLIB_API_GetSerializedData, &param, sizeof(param));

    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] IARM_BUS_MFRLIB_API_GetSerializedData returns params: %s with paramlen: %d.\n",__FUNCTION__, param.buffer, param.bufLen);
    if(iarm_ret == IARM_RESULT_SUCCESS)
    {
        try
        {
            if( param.buffer && param.bufLen) {
                strncpy((char *)stMsgData->paramValue, param.buffer, param.bufLen);
                stMsgData->paramValue[param.bufLen+1] = '\0';
                stMsgData->paramLen = param.bufLen;
                if(bCalledProductClass && pChanged && strncmp(stMsgData->paramValue,backupProductClass,_BUF_LEN_16 ))
                {
                    *pChanged =  true;
                }
                bCalledProductClass = true;
                strncpy(backupProductClass,stMsgData->paramValue,_BUF_LEN_16 );
                stMsgData->paramtype = hostIf_StringType;
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] paramValue: %s param.pBuffer: %s \n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramValue, param.buffer);
                ret = OK;
            }
            else
                ret = NOK;
        } catch (const std::exception e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\n",__FUNCTION__);
            ret = NOK;
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF, "Failed in IARM_Bus_Call() for parameter : %s [param.type:%d with error code:%d]\n",stMsgData->paramName,param.type, ret);
        ret = NOK;
    }
    return ret;
#endif /* FETCH_PRODUCTCLASS_FROM_MFRLIB */
}

/**
 * @brief This function identifying the particular CPE model and version.
 * This MUST be based on Comcast_X_HW* specification and of the format VM.m.R.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch data from device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_HardwareVersion(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    int ret=NOT_HANDLED;
    stMsgData->paramtype = hostIf_StringType;
    IARM_Bus_MFRLib_GetSerializedData_Param_t param;
    IARM_Result_t iarm_ret = IARM_RESULT_IPCCORE_FAIL;
    memset(&param, 0, sizeof(param));
    param.type = mfrSERIALIZED_TYPE_HARDWAREVERSION;
    iarm_ret = IARM_Bus_Call(IARM_BUS_MFRLIB_NAME, IARM_BUS_MFRLIB_API_GetSerializedData, &param, sizeof(param));

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] IARM_BUS_MFRLIB_API_GetSerializedData returns params: %s with paramlen: %d.\r\n",__FUNCTION__, param.buffer, param.bufLen);
    if(iarm_ret == IARM_RESULT_SUCCESS)
    {
        try
        {
            if(param.buffer && param.bufLen) {
                strncpy((char *)stMsgData->paramValue, param.buffer, param.bufLen);
                stMsgData->paramValue[param.bufLen+1] = '\0';
                stMsgData->paramLen = param.bufLen;
                if(bCalledHardwareVersion && pChanged && strncmp(stMsgData->paramValue,backupHardwareVersion,_BUF_LEN_16 ))
                {
                    *pChanged =  true;
                }
                bCalledHardwareVersion = true;
                strncpy(backupHardwareVersion,stMsgData->paramValue,_BUF_LEN_16 );
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] paramValue: %s param.pBuffer: %s \n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramValue, param.buffer);
                ret = OK;
            }
            else
                ret = NOK;
        } catch (const std::exception e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
            ret = NOK;
        }

    }
    else
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Failed in IARM_Bus_Call() for parameter : %s [param.type:%d with error code:%d]\n",stMsgData->paramName,param.type, ret);
        ret = NOK;
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s()]\n", __FUNCTION__);
    return ret;
}

/**
 * @brief This function identifying any additional CPE model and version.
 * Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns enum integer '-1' on method completion.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_AdditionalHardwareVersion(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s()]\n", __FUNCTION__ );
    return NOK;
}

/**
 * @brief This function identifying any additional Software/Firmware version of the running
 * image on the box (Vx.y.z). A string identifying the software version currently installed
 * in the CPE (i.e. version of the overall CPE firmware).
 * To allow version comparisons, the version number SHOULD be in the form of
 * dot-delimited integers, where each successive integer represents a more
 * minor category of variation.
 * For example, 3.0.21 where the components mean: Major.Minor.Build.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns the status of the operation.
 * @retval OK if it is successful fetch data from device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_AdditionalSoftwareVersion(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    int ret = NOT_HANDLED;
    stMsgData->paramtype = hostIf_StringType;
    IARM_Bus_MFRLib_GetSerializedData_Param_t param;
    IARM_Result_t iarm_ret = IARM_RESULT_IPCCORE_FAIL;
    memset(&param, 0, sizeof(param));

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    param.type = mfrSERIALIZED_TYPE_SOFTWAREVERSION;

    iarm_ret = IARM_Bus_Call(IARM_BUS_MFRLIB_NAME, IARM_BUS_MFRLIB_API_GetSerializedData, &param, sizeof(param));

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] IARM_BUS_MFRLIB_API_GetSerializedData returns params: %s with paramlen: %d.\r\n",__FUNCTION__, param.buffer, param.bufLen);
    if(iarm_ret == IARM_RESULT_SUCCESS)
    {
        try
        {
            if(param.buffer && param.bufLen) {
                strncpy((char *)stMsgData->paramValue, param.buffer, param.bufLen);
                stMsgData->paramValue[param.bufLen+1] = '\0';
                if(bCalledAdditionalSoftwareVersion && pChanged && strncmp(stMsgData->paramValue,backupAdditionalSoftwareVersion,_BUF_LEN_16 ))
                {
                    *pChanged =  true;
                }
                bCalledAdditionalSoftwareVersion = true;
                strncpy(backupAdditionalSoftwareVersion,stMsgData->paramValue,_BUF_LEN_16 );

                stMsgData->paramLen = param.bufLen;
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] paramValue: %s param.pBuffer: %s \n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramValue, param.buffer);
                ret = OK;
            }
            else
                ret = NOK;
        } catch (const std::exception e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
            ret = NOK;
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF, "Failed in IARM_Bus_Call() for parameter : %s [param.type:%d with error code:%d]\n",stMsgData->paramName,param.type, ret);
        ret = NOK;
    }
    return ret;
}


/**
 * @brief This is an identifier of the primary service provider and other provisioning
 * information, which MAY be used to determine service provider-specific customization
 * and provisioning parameters.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successful fetch data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch data from the device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_ProvisioningCode(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    int ret=NOT_HANDLED;
    stMsgData->paramtype = hostIf_StringType;
    IARM_Bus_MFRLib_GetSerializedData_Param_t param;
    IARM_Result_t iarm_ret = IARM_RESULT_IPCCORE_FAIL;
    memset(&param, 0, sizeof(param));
    param.type = mfrSERIALIZED_TYPE_PROVISIONINGCODE;
    iarm_ret = IARM_Bus_Call(IARM_BUS_MFRLIB_NAME, IARM_BUS_MFRLIB_API_GetSerializedData, &param, sizeof(param));

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] IARM_BUS_MFRLIB_API_GetSerializedData returns params: %s with paramlen: %d.\r\n",__FUNCTION__, param.buffer, param.bufLen);
    if(iarm_ret == IARM_RESULT_SUCCESS)
    {
        try
        {
            if(param.buffer && param.bufLen)
            {
                strncpy((char *)stMsgData->paramValue, param.buffer, param.bufLen);
                stMsgData->paramValue[param.bufLen+1] = '\0';
                stMsgData->paramLen = param.bufLen;
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] paramValue: %s param.pBuffer: %s \n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramValue, param.buffer);
                ret = OK;
            }
            else
            {
                ret = NOK;
            }
        } catch (const std::exception e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
            ret = NOK;
        }

    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF, "Failed in IARM_Bus_Call() for parameter : %s [param.type:%d with error code:%d]\n",stMsgData->paramName,param.type, ret);
        ret = NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s()]\n", __FUNCTION__);
    return OK; // For any failures return OK for now to get empty results.
}



/**
 * @brief This is an identifier of time in seconds since the CPE was last restarted.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successful fetch data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch data from the device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_UpTime(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    struct sysinfo info;
    sysinfo (&info);
    put_int(stMsgData->paramValue, (int) info.uptime);
    stMsgData->paramtype = hostIf_IntegerType;
    return OK;
}

/**
 * @brief This function use to get the 'Date' and 'Time' in UTC that the CPE first both
 * successfully established an IP-layer network connection and acquired an absolute time
 * reference using NTP or equivalent over that network connection. The CPE MAY reset this
 * date after a factory reset. If NTP or equivalent is not available, this parameter, if
 * present, SHOULD be set to the unknown time value.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successful fetch data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch data from the device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_FirstUseDate(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    struct stat st;
    struct tm *tm;
    char buffer [36] = {'\0'};
    char timeZoneTmp[7] = {'\0'};

    if (0 > stat(NTP_FILE_NAME, &st))
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF, "[%s:%s:%d]Failed:Error on stat() for %s \n",__FILE__,__FUNCTION__,__LINE__, NTP_FILE_NAME);
        return NOK;
    }

    tm = gmtime(&(st.st_mtime));
    strftime(buffer, sizeof(buffer),  "%Y-%m-%dT%H:%M:%S", tm);  //CID:81299 - OVERRUN
    strftime(timeZoneTmp, sizeof(timeZoneTmp),  "%z", tm);
    sprintf(buffer + strlen(buffer), ".%.6d%s", tm->tm_sec, timeZoneTmp);

    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(buffer);
    strncpy(stMsgData->paramValue, buffer, stMsgData->paramLen +1);
    return OK;
}

/**
 * @brief This function use to get the MAC Address of the eth1 interface currently.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch data from the device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_X_COMCAST_COM_STB_MAC(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    errno_t rc = -1;
    int ret = NOT_HANDLED;
#if !defined (USE_DEV_PROPERTIES_CONF)
    IARM_Bus_MFRLib_GetSerializedData_Param_t param;
    memset(&param, 0, sizeof(param));
    IARM_Result_t iarm_ret = IARM_RESULT_IPCCORE_FAIL;
    param.type = mfrSERIALIZED_TYPE_DEVICEMAC;
    int len = strlen(stbMacCache);

    
    try
    {
        if((stbMacCache[0] == '\0') && (len == 0)) {
            iarm_ret = IARM_Bus_Call(IARM_BUS_MFRLIB_NAME, IARM_BUS_MFRLIB_API_GetSerializedData, &param, sizeof(param));
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] IARM_BUS_MFRLIB_API_GetSerializedData returns params: %s with paramlen: %d.\r\n",__FUNCTION__, param.buffer, param.bufLen);
            if(iarm_ret == IARM_RESULT_SUCCESS)
            {
                if(param.buffer && param.bufLen) {
                    strncpy((char *)stMsgData->paramValue, param.buffer, param.bufLen);
                    stMsgData->paramValue[param.bufLen+1] = '\0';
                    stMsgData->paramLen = param.bufLen;
                    if(bCalledDeviceMAC && pChanged && strncmp(stMsgData->paramValue,backupDeviceMAC,_BUF_LEN_32 ))
                    {
                        *pChanged =  true;
                    }
                    bCalledDeviceMAC = true;
                    strncpy(backupDeviceMAC,stMsgData->paramValue,_BUF_LEN_32 );
                    memset(stbMacCache, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN );
                    strncpy(stbMacCache, param.buffer, param.bufLen);
                    stMsgData->paramtype = hostIf_StringType;
                    ret = OK;
                }
                else {
                    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in \'IARM_BUS_MFRLIB_API_GetSerializedData\' for parameter : %s [ Value :%s with size :%d]\n",stMsgData->paramName, param.buffer, param.bufLen);
                    ret = NOK;
                }
            }
            else {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in IARM_Bus_Call() for parameter : %s [param.type:%d with error code:%d]\n",stMsgData->paramName,param.type, ret);
                ret = NOK;
            }
        }
        else
        {
            memset(stMsgData->paramValue, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN );
            stMsgData->paramLen = len;
            strncpy(stMsgData->paramValue, stbMacCache, stMsgData->paramLen);
            stMsgData->paramtype = hostIf_StringType;
            ret = OK;
        }
    } catch (const std::exception& e)
    {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
        ret = NOK;
    }
#else //if defined (USE_DEV_PROPERTIES_CONF)
    memset(stMsgData->paramValue, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN );
    string stb_mac = getStbMacIf_fr_devProperties();
    if(!stb_mac.empty())
    {
        rc=strcpy_s(stMsgData->paramValue,sizeof(stMsgData->paramValue), stb_mac.c_str());
	if(rc!=EOK)
	{
		ERR_CHK(rc);
	}
    }
    else
        stMsgData->faultCode = fcInvalidParameterValue;
    stMsgData->paramLen = stb_mac.length();
    stMsgData->paramtype = hostIf_StringType;
    ret = OK;
#endif
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()]\n", __FUNCTION__);
    return ret;
}

string hostIf_DeviceInfo::getEstbIp()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s()]Entering..\n", __FUNCTION__);
    string retAddr;
#if MEDIA_CLIENT
    IARM_Result_t ret = IARM_RESULT_SUCCESS;
    IARM_BUS_NetSrvMgr_Iface_EventData_t param;
    memset(&param, 0, sizeof(param));
    try
    {
        ret = IARM_Bus_Call(IARM_BUS_NM_SRV_MGR_NAME,IARM_BUS_NETSRVMGR_API_getSTBip, (void*)&param, sizeof(param));
        if (ret != IARM_RESULT_SUCCESS )
        {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s():%d] IARM_BUS_NETSRVMGR_API_getActiveInterface failed \n", __FUNCTION__, __LINE__);
        }
        retAddr=param.activeIfaceIpaddr;
    }
    //Legacy way of getting estb ip.
#else

    struct ifaddrs *ifAddrStr = NULL;
    struct ifaddrs * ifa = NULL;
    void * tmpAddrPtr = NULL;
    char tmp_buff[TR69HOSTIFMGR_MAX_PARAM_LEN] = {'\0'};


    const char *ipv6_fileName = "/tmp/estb_ipv6";
    const char *Wifi_Enable_file = "/tmp/wifi-on";

    try {
        /*check for ipv6 file*/
        bool ipv6Enabled = (!access (ipv6_fileName, F_OK))?true:false;
        bool isWifiEnabled = (!access (Wifi_Enable_file, F_OK))?true:false;
        const char* ip_if = NULL;
//#ifdef MEDIA_CLIENT
        /* Get configured moca interface */
//        ip_if = "MOCA_INTERFACE";
//#else
        ip_if = "DEFAULT_ESTB_INTERFACE";
//#endif
        char *ethIf = getenvOrDefault (ip_if, "");
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s():%d] ipv6Enabled : %d; isWifiEnabled : %d ethIf : %s\n",
                __FUNCTION__, __LINE__, ipv6Enabled, isWifiEnabled, ethIf);

        if(getifaddrs(&ifAddrStr))
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s():%d] Failed in getifaddrs().\n", __FUNCTION__, __LINE__);
            return retAddr;
        }
        bool found = false;

        for (ifa = ifAddrStr; ifa != NULL; ifa = ifa->ifa_next)
        {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s():%d] if name : %s; family : %d %s \n", __FUNCTION__, __LINE__,
                    ifa->ifa_name,
                    ifa ->ifa_addr->sa_family,
                    (ifa ->ifa_addr->sa_family == AF_PACKET) ? " (AF_PACKET)" :
                    (ifa ->ifa_addr->sa_family == AF_INET) ?   " (AF_INET)" :
                    (ifa ->ifa_addr->sa_family == AF_INET6) ?  " (AF_INET6)" : "" );

            if (ifa->ifa_addr == NULL) continue;

            if (ipv6Enabled)
            {
                /* Check for IP6 */
                if ((ifa ->ifa_addr->sa_family == AF_INET6))
                {
                    tmpAddrPtr=&((struct sockaddr_in6  *)ifa->ifa_addr)->sin6_addr;
                    inet_ntop(AF_INET6, tmpAddrPtr, tmp_buff, INET6_ADDRSTRLEN);

                    if(isWifiEnabled && (!(IN6_IS_ADDR_LINKLOCAL(tmpAddrPtr)))) {
                        if(!strcmp(ifa->ifa_name, "wlan0")) {
                            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s():%d] Got ipaddress \'%s\' for (%s), breaking loop.\n", __FUNCTION__, __LINE__, tmp_buff, ifa->ifa_name);
                            found = true;
                            break;
                        }
                    }
                    if(!strcmp(ifa->ifa_name, ethIf) && (!(IN6_IS_ADDR_LINKLOCAL(tmpAddrPtr))))  {
                        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s():%d] Got ipaddress \'%s\' for (%s), breaking loop.\n", __FUNCTION__, __LINE__, tmp_buff, ifa->ifa_name);
                        found = true;
                        break;
                    }
                }
            }
            else {
                /* Check for IP4 */
                if (ifa ->ifa_addr->sa_family == AF_INET) {
                    tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
                    inet_ntop(AF_INET, tmpAddrPtr, tmp_buff, INET_ADDRSTRLEN);

                    if(isWifiEnabled) {
                        if(!strcmp(ifa->ifa_name, "wlan0")) {
                            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s():%d] Got ipaddress \'%s\' for (%s), breaking loop.\n", __FUNCTION__, __LINE__, tmp_buff, ifa->ifa_name);
                            found = true;
                            break;
                        }
                    }
                    else if (strcmp(ifa->ifa_name, ethIf)==0) {
                        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s():%d] Got ipaddress \'%s\' for (%s), breaking loop.\n", __FUNCTION__, __LINE__, tmp_buff, ifa->ifa_name);
                        found = true;
                        break;
                    }
                }
            }
        }

        if (ifAddrStr!=NULL) {
            freeifaddrs(ifAddrStr);
        }

        if (!found) {
            return retAddr;
        }
        else {
            retAddr = tmp_buff;
        }
    }
#endif
    catch (const std::exception &e)
    {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s()]Exception caught %s\n", __FUNCTION__, e.what());
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s()]Exiting..\n", __FUNCTION__);
    return retAddr;
}

bool hostIf_DeviceInfo::isRsshactive()
{
    const string pidfile("/var/tmp/rssh.pid");
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Entering... \n",__FUNCTION__);
    bool retCode = false;

    ifstream pidstrm;
    pidstrm.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        pidstrm.open(pidfile.c_str());
        int sshpid;
        pidstrm>>sshpid;

        if (getpgid(sshpid) >= 0)
        {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] SSH Session Active \n",__FUNCTION__);
            retCode = true;
        }
        else
        {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] SSH Session inactive \n",__FUNCTION__);
        }
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] SSH Session inactive ; Error opening pid file\n",__FUNCTION__);
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Exiting... \n",__FUNCTION__);
    return retCode;
}
/**
 * @brief This function use to get the IPv4 Address of the eth1 interface currently.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch data from the device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_X_COMCAST_COM_STB_IP(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s()]Entering..\n", __FUNCTION__);

    string ipaddr = getEstbIp();

    if (ipaddr.empty())
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s()]Ipaddress is empty..\n", __FUNCTION__);
        return NOK;
    }
    else {
        if(bCalledX_COMCAST_COM_STB_IP && pChanged && strncmp(ipaddr.c_str(),backupX_COMCAST_COM_STB_IP,_BUF_LEN_64 ))
        {
            *pChanged =  true;
        }
        bCalledX_COMCAST_COM_STB_IP = true;
        strncpy(backupX_COMCAST_COM_STB_IP,ipaddr.c_str(),sizeof(backupX_COMCAST_COM_STB_IP) -1);  //CID:136623 - Buffer size warning
        backupX_COMCAST_COM_STB_IP [sizeof(backupX_COMCAST_COM_STB_IP) -1] = '\0';
        memset(stMsgData->paramValue, '\0', _BUF_LEN_64);
        stMsgData->paramLen = ipaddr.length();
        strncpy(stMsgData->paramValue, ipaddr.c_str(), stMsgData->paramLen);
        stMsgData->paramtype = hostIf_StringType;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s()]Exiting..\n", __FUNCTION__);
    return OK;
}

/**
 * @brief The X_COMCAST_COM_PowerStatus as get parameter results in the power status
 * being performed on the device. Power status of the device based on the front panel
 * power LED.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns the status of the operation. When read, this parameter returns an enumeration string.
 * @retval OK if it is successful fetch data from the device.
 * @retval NOK if not able to fetch data from the device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_X_COMCAST_COM_PowerStatus(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s()]Entering..\n", __FUNCTION__);
    IARM_Result_t err;
    int ret = NOK;
    const char *pwrState = "PowerOFF";
    int str_len = 0;
    IARM_Bus_PWRMgr_GetPowerState_Param_t param;
    memset(&param, 0, sizeof(param));
    IARM_Result_t iarm_ret = IARM_RESULT_IPCCORE_FAIL;

    err = IARM_Bus_Call(IARM_BUS_PWRMGR_NAME,
                        IARM_BUS_PWRMGR_API_GetPowerState,
                        (void *)&param,
                        sizeof(param));
    if(err == IARM_RESULT_SUCCESS)
    {
        pwrState = (param.curState==IARM_BUS_PWRMGR_POWERSTATE_OFF)?"PowerOFF":(param.curState==IARM_BUS_PWRMGR_POWERSTATE_ON)?"PowerON":"Standby";

//        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Current state is : (%d)%s\n",param.curState, pwrState);
        str_len = strlen(pwrState);
        try
        {
            strncpy((char *)stMsgData->paramValue, pwrState, str_len);
            stMsgData->paramValue[str_len+1] = '\0';
            stMsgData->paramLen = str_len;
            stMsgData->paramtype = hostIf_StringType;
            ret = OK;
        } catch (const std::exception e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception\r\n",__FUNCTION__);
            ret = NOK;
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in IARM_Bus_Call() for parameter : %s [param.type:%s with error code:%d]\n",stMsgData->paramName, pwrState, ret);
        ret = NOK;
    }

    //RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s()]Exiting..\n", __FUNCTION__);
    return ret;
}

/**
 * @brief Get the filename of the firmware currently running on the device.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns the Filename of the firmware currently running on the device.
 *
 * @retval OK if it is successfully fetch data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch data from the device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareFilename(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    errno_t rc = -1;
    string line;
    bool curFileFlag = true;
    ifstream curFwfile(CURENT_FW_FILE);

    try {
        if(curFwfile.is_open())
        {
            if(getline (curFwfile,line))
            {
                if(!line.empty()) {
                    strncpy(stMsgData->paramValue, line.c_str(), line.length()+1 );
                }
                else {
                    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed: \'%s\' has no content or empty..\n", CURENT_FW_FILE);
                    curFileFlag = false;
                }
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed to read \'%s\' due to Empty Buffer.  \n", CURENT_FW_FILE);
                curFileFlag = false;
            }
            curFwfile.close();
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed \'%s\' due to [\'%s\' (%d)].  \n", CURENT_FW_FILE, strerror(errno), errno);
            curFileFlag = false;
        }

        if(!curFileFlag)
        {
            ifstream myfile ("/version.txt");
            if (myfile.is_open())
            {
                while ( getline (myfile,line) )
                {
                    size_t found = line.find("imagename");
                    if (found!=string::npos)
                    {
                        break;
                    }
                }
                myfile.close();
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]Unable to open file\n", __FUNCTION__);
            }

            if(line.length()) {
                char * cstr = new char [line.length()+1];
                rc=strcpy_s (cstr,(line.length()+1), line.c_str());
		if(rc!=EOK)
		{
			ERR_CHK(rc);
		}
                char * pch = NULL;
                pch = strstr (cstr,":");
                pch++;

                while(isspace(*pch)) {
                    pch++;
                }
                delete[] cstr;

                if(bCalledX_COMCAST_COM_FirmwareFilename && pChanged && strncmp(pch,backupX_COMCAST_COM_FirmwareFilename,_BUF_LEN_64 ))
                {
                    *pChanged =  true;
                }

                bCalledX_COMCAST_COM_FirmwareFilename = true;
                strncpy(backupX_COMCAST_COM_FirmwareFilename,pch,sizeof(backupX_COMCAST_COM_FirmwareFilename) -1);  //CID:136569 - Buffer size
                backupX_COMCAST_COM_FirmwareFilename[sizeof(backupX_COMCAST_COM_FirmwareFilename) -1] = '\0';
                strncpy(stMsgData->paramValue,pch,_BUF_LEN_64 );
                strncpy((char *) stMsgData->paramValue, pch, stMsgData->paramLen +1 );
            }
        }

        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);
    }
    catch (const std::exception e) {
        cout << __FUNCTION__ << endl;
    }

    return OK;
}

/**
 * @brief Get the filename of the firmware that the device was requested to download most recently.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns the filename of the firmware that was recently downloaded.
 *
 * @retval OK if it is successfully fetch data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch data from the device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareToDownload(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    int ret = NOK;
    try
    {
        if(OK == readFirmwareInfo((char *)"DnldFile", stMsgData))
            ret = OK;
        else
            ret = NOK;
    }
    catch (const std::exception & e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s()] Exception occurred due to %s.  \n", __FUNCTION__,  e.what());
    }
    return ret;
}

int hostIf_DeviceInfo::get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadStatus(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    if(OK == readFirmwareInfo((char *)"Status", stMsgData))
        return OK;
    else
        return NOK;
}

int hostIf_DeviceInfo::get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadProtocol(HOSTIF_MsgData_t* stMsgData, bool *pChanged)
{
    if(OK == readFirmwareInfo((char *)"Proto", stMsgData))
        return OK;
    else
        return NOK;
}

int hostIf_DeviceInfo::get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadURL (HOSTIF_MsgData_t* stMsgData, bool *pChanged)
{
    if(OK == readFirmwareInfo((char *)"DnldURL", stMsgData))
        return OK;
    else
        return NOK;
}

// Get the Codebig flag value (from /opt/fwdnldstatus.txt) which is already set using 'Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadUseCodebig' tr-181 parameter
int hostIf_DeviceInfo::get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadUseCodebig(HOSTIF_MsgData_t* stMsgData, bool *pChanged)
{
    if(OK == readFirmwareInfo((char *)"Codebig_Enable", stMsgData))
        return OK;
    else
        return NOK;
}

// Get the Defer Reboot flag value which is already set using 'Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadDeferReboot' tr-181 parameter
int hostIf_DeviceInfo::get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadDeferReboot(HOSTIF_MsgData_t* stMsgData, bool *pChanged)
{
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);

    put_boolean(stMsgData->paramValue, m_xFirmwareDownloadDeferReboot);

    return OK;
}

int hostIf_DeviceInfo::get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadPercent (HOSTIF_MsgData_t* stMsgData, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    int firmwareDownloadPercent = -1;
    char output[8];
    if (OK == read_command_output ((char *)"cat /opt/curl_progress | tr -s '\r' '\n' | tail -n 1 | sed 's/^ *//g' | tr -s ' ' | cut -d ' ' -f3", output, 8))
    {
        output[strcspn(output, "\n")] = 0;
        if (*output)
        {
            firmwareDownloadPercent = strtol (output, NULL, 10);
        }
    }

    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] FirmwareDownloadPercent = [%d]\n", __FUNCTION__, firmwareDownloadPercent);

    put_int (stMsgData->paramValue, firmwareDownloadPercent);
    stMsgData->paramtype = hostIf_IntegerType;
    stMsgData->paramLen = sizeof(int);

    return OK;
}

int hostIf_DeviceInfo::get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareUpdateState(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    if(OK == readFirmwareInfo((char *)"fwUpdateState", stMsgData))
        return OK;
    else
        return NOK;
}

/**
 * @brief This function gets the number of entries in the VendorConfigFile table.
 * Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successful fetch data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch data from the device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_VendorConfigFileNumberOfEntries(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    return NOK;
}

/**
 * @brief This function gets the number of entries in the SupportedDataModel table.
 * Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successful fetch data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch data from the device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_SupportedDataModelNumberOfEntries(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
//    put_int(stMsgData->paramValue, (unsigned int)DEVICE_SUPPORTED_DATA_MODEL_NUMBER_OF_ENTRIES);

    /* Fixed DELIA-27160, always returns as OK */
    return OK;
}

int hostIf_DeviceInfo::get_Device_DeviceInfo_ProcessorNumberOfEntries(HOSTIF_MsgData_t * stMsgData)
{
    FILE *fp = NULL;
    char resultBuff[200] = {'\0'};
    char cmd[200] = "/bin/cat /proc/cpuinfo | /bin/grep processor | /usr/bin/wc -l";
    int noOfProcessorEntries = 0;

    fp = popen(cmd,"r");

    if(fp == NULL) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Error popen\n", __FUNCTION__);
        return 0;
    }

    if(fgets(resultBuff, 200,fp)!=NULL) {
        sscanf(resultBuff,"%d",&noOfProcessorEntries);
    }

    pclose(fp);

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s(): Processors Count: [%d]\n", __FUNCTION__, noOfProcessorEntries);

    put_int(stMsgData->paramValue,noOfProcessorEntries);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen=4;

    return OK;
}

/**
 * @brief The function gets the number of entries in the VendorLogFile table.
 * Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successful fetch data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch data from the device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_VendorLogFileNumberOfEntries(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    return NOK;
}

/**
 * @brief This function get X_COMCAST-COM_Reset returns an empty string.
 * Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns integer '-1' on method completion
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset(HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    return NOK;
}

/*
 * Parameter Name: Device.DeviceInfo.MemoryStatus.Total
 * Status of the device's volatile physical memory.
 * The total physical RAM, in kilobytes, installed on the device.
 */

/**
 * @brief Get the device total memory status.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch data from the device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_MemoryStatus_Total (HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    struct sysinfo sys_info;

    sysinfo (&sys_info);
    put_int(stMsgData->paramValue, (unsigned int) (sys_info.totalram *(unsigned long long)sys_info.mem_unit / 1024));
    stMsgData->paramtype = hostIf_IntegerType;
    return OK;
}

/*
 * Parameter Name: Device.DeviceInfo.MemoryStatus.Free
 * Status of the device's volatile physical memory.
 * The free physical RAM, in kilobytes, currently available on the device.
 */

/**
 * @brief Get the device total free memory status.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successful fetch data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch data from the device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_MemoryStatus_Free (HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{
    struct sysinfo sys_info;

    sysinfo (&sys_info);
    put_int(stMsgData->paramValue,(unsigned int) (sys_info.freeram *(unsigned long long)sys_info.mem_unit / 1024));
    stMsgData->paramtype = hostIf_IntegerType;
    return OK;
}

/**
 * @brief This method is updated with the boot status of the device at the time of query.
 * If the device is currently starting up, this parameter will hold boot status as per the
 * boot sequence of the device.
 * BootStatus string - Boot status for an STB CPE via TR-069 ACS when powered On.
 * Enumeration of :
 *  			Coax connection confirmed, MoCA enabled
 *  			Discovering MoCA Network Coordinator: MoCA MAC: xx:xx:xx:xx:xx:xx
 *  			Joining MoCA Network
 *  			Connection successful
 *  			Acquiring IP Address from Gateway
 *  			Contacting ACS
 *  			Contacting XRE
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch data from the device.
 * @retval NOK if not able to fetch data from the device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_X_RDKCENTRAL_COM_BootStatus (HOSTIF_MsgData_t * stMsgData, bool *pChanged)
{

    int mocaStatus = NOK;
    bool check_AcsConnStatus = false;
    bool check_GatewayConnStatus = false;
    bool check_XreConnStatus = false;
    char statusStr[TR69HOSTIFMGR_MAX_PARAM_LEN] = {'\0'};
    errno_t rc = -1;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Entering \n",__FUNCTION__);

    /**
     * Checking following steps under : checkMoCABootStatus()
     * 	1. Coax connection confirmed, MoCA enabled
     * 	2. Discovering MoCA Network Coordinator: MoCA MAC: xx:xx:xx:xx:xx:xx
     * 	3. Joining MoCA Network
     **/

#ifdef USE_MoCA_PROFILE
    MoCAInterface *mIf = MoCAInterface::getInstance(0);
    mocaStatus = mIf->check_MoCABootStatus(statusStr);
#else
    check_AcsConnStatus = true;
#endif

    if(mocaStatus == OK)
    {
        check_AcsConnStatus = true;
    }

    /**
     * Check for Gateway Connection State
     *  4. Connection successful
     *  5. Acquiring IP Address from Gateway(Not valid)
     */

    if(check_GatewayConnStatus)
    {

        if(get_GatewayConnStatus() == true)
        {
            memset(statusStr, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN);
            rc=strcpy_s(statusStr,sizeof(statusStr), "Connection successful");
	    if(rc!=EOK)
	    {
		ERR_CHK(rc);
	    }
            check_AcsConnStatus = true;
        }
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] BootStatus: %s GatewayConnStatuss: %d\n", __FILE__, __FUNCTION__, statusStr, get_GatewayConnStatus());
    }


    /*
     *
     * */
    HOSTIF_MsgData_t stTR069SupportData = {0};
    snprintf(stTR069SupportData.paramName, TR69HOSTIFMGR_MAX_PARAM_LEN-1, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.TR069support.Enable");
    stTR069SupportData.paramtype = hostIf_BooleanType;
    get_xRDKCentralComRFC(&stTR069SupportData);

    bool isTr69RfcEnabled = get_boolean(stTR069SupportData.paramValue);

    check_AcsConnStatus = (isTr69RfcEnabled)?true:false;

    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] %s: %d\n",  __FUNCTION__, __LINE__, stTR069SupportData.paramName, isTr69RfcEnabled);

    /**
     * Check for ACS Connection State
     *   6. Contacting ACS
     */

    if(check_AcsConnStatus)
    {
        if(get_ACSStatus() == true)
        {
            memset(statusStr, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN);
            rc=strcpy_s(statusStr,sizeof(statusStr), "Contacting ACS");
	    if(rc!=EOK)
	    {
		ERR_CHK(rc);
	    }
            check_XreConnStatus = true;
        }
//	RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] BootStatus: %s gAcsConnStatus: %d\n", __FILE__, __FUNCTION__, statusBuf, get_ACSStatus());
    }
    else {
        check_XreConnStatus = true;
    }

    /**
     * Check for Xre Connection State
     *   6. XRE connection established - Successful
     */

#ifdef USE_XRESRC
    if(get_Device_X_COMCAST_COM_Xcalibur_Client_XRE_ConnectionTable_xreConnStatus(stMsgData) == OK)
    {
        const char* xreConn = "XRE connection established - Successful";
        const char* xreConnAcs = "XRE connection established, ACS connection in progresss";

        if(strcasecmp(stMsgData->paramValue, "Connected" ) == 0) {
            memset(stMsgData->paramValue, '\0', TR69HOSTIFMGR_MAX_PARAM_LEN);
            snprintf(statusStr, TR69HOSTIFMGR_MAX_PARAM_LEN -1, ((check_XreConnStatus)?xreConn:((check_AcsConnStatus)?xreConnAcs:xreConn)));
        }
    }
#endif /*USE_XRESRC*/

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] BootStatus string: %s \n", __FILE__, __FUNCTION__, statusStr);
    strncpy(stMsgData->paramValue, statusStr, TR69HOSTIFMGR_MAX_PARAM_LEN);
    stMsgData->paramLen = strlen(statusStr);
    stMsgData->paramtype = hostIf_StringType;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Exiting \n",__FUNCTION__);
    return OK;
}

/**
 * @brief This method gets the CPU temperature for doing general health check up
 * of the box.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch data from the device.
 * @retval NOK if not able to fetch data from the device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::get_Device_DeviceInfo_X_RDKCENTRAL_COM_CPUTemp(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int cpuTemp = 0;
    float dsCpuTemp = device::Host::getInstance().getCPUTemperature();
    cpuTemp = (int)round(dsCpuTemp);

    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%s] Current CPU temperature  is: %+7.2fC and roundoff CPUTemp : [%d] \n",
            __FILE__, __FUNCTION__, dsCpuTemp, cpuTemp);

    put_int(stMsgData->paramValue, cpuTemp);
    stMsgData->paramtype = hostIf_IntegerType;
    return OK;
}

int hostIf_DeviceInfo::get_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType(HOSTIF_MsgData_t * stMsgData,bool *pChanged)
{
    errno_t rc = -1;
    int ret = OK;
    char prefGatewayValue[GATEWAY_NAME_SIZE];
    FILE *fp;
    if((fp=fopen(PREFERRED_GATEWAY_FILE,"r"))==NULL)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]Unable to open file\n", __FUNCTION__);
        prefGatewayValue[0]='\0';

    }
    else
    {
        if(fgets(prefGatewayValue, GATEWAY_NAME_SIZE, fp) == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]not able to read string from file \n", __FUNCTION__);
	    prefGatewayValue[0]='\0';
        }
        fclose(fp);
    }

    strncpy(stMsgData->paramValue,prefGatewayValue,TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);
    return ret;
}

int hostIf_DeviceInfo::get_xOpsDMUploadLogsNow (HOSTIF_MsgData_t *stMsgData)
{
    // @TODO
    put_boolean(stMsgData->paramValue, false);
    return OK;
}

int hostIf_DeviceInfo::get_xOpsDMLogsUploadStatus(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s]Entering...\n", __FUNCTION__);

    FILE *logUpfile = NULL;
    size_t n = 1024;
    char* curLogUploadStatus = (char*) malloc (n);

    if(curLogUploadStatus  == NULL)   //CID:88476 - Reverse_inull
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] curLogUploadStatus is Null\n", __FUNCTION__);   //CID:84731 - Forward Null
    }
    else if ((logUpfile = fopen (CURRENT_LOG_UPLOAD_STATUS, "r")) == NULL)
    {
        sprintf (curLogUploadStatus, "Unknown: Failed to open file '%s' due to \'%s\'.", CURRENT_LOG_UPLOAD_STATUS, strerror (errno));
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] curLogUploadStatus = %s\n", __FUNCTION__, curLogUploadStatus);
    }
    else if (-1 == getline (&curLogUploadStatus, &n, logUpfile))
    {
        sprintf (curLogUploadStatus, "Unknown: Failed to read file '%s'.", CURRENT_LOG_UPLOAD_STATUS);
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] curLogUploadStatus = %s\n", __FUNCTION__, curLogUploadStatus);
        fclose (logUpfile);
    }
    else
    {
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] Successfully read from %s. The value is \'%s\'. \n", __FUNCTION__, CURRENT_LOG_UPLOAD_STATUS,
                  curLogUploadStatus);
        fclose (logUpfile);
    }

    stMsgData->paramValue[0] = 0;
    strncat (stMsgData->paramValue, curLogUploadStatus, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);

    if (curLogUploadStatus)
        free (curLogUploadStatus);

    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (stMsgData->paramValue);

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s]Exiting...\n", __FUNCTION__);

    return OK;
}

int hostIf_DeviceInfo::get_X_RDKCENTRAL_COM_BootTime(HOSTIF_MsgData_t * stMsgData,bool *pChanged)
{
    int ret = OK;
    static unsigned int bTime = 0;
    struct sysinfo sys_info;
    struct timeval currTime;

    if(!bTime)
    {
        if(sysinfo(&sys_info)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in sysinfo due to \'%s\' (%d).  \n",  strerror(errno), errno);
            return NOK;
        }

        unsigned long upTime = sys_info.uptime;

        if(gettimeofday(&currTime, NULL)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in sysinfo due to \'%s\' (%d).  \n",  strerror(errno), errno);
            return NOK;
        }
        bTime = (unsigned int)(currTime.tv_sec - upTime);
    }

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s]The Bootup Time is [%u].\n", __FUNCTION__, bTime);
    put_ulong(stMsgData->paramValue, bTime);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(hostIf_UnsignedIntType);
    return ret;
}

int hostIf_DeviceInfo::set_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOK;
    FILE *fp;
    if((fp=fopen(PREFERRED_GATEWAY_FILE,"w"))==NULL)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]Unable to open file\n", __FUNCTION__);

    }
    else
    {
        if(fputs(stMsgData->paramValue, fp) == EOF)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]not able to read string from file \n", __FUNCTION__);
        }
        else
        {
            ret=OK;
        }
        fclose(fp);

    }

    return ret;
}

/**
 * @brief This is set X_COMCAST-COM_Reset as setting this parameter results in
 * the reset being performed on the device. The level of reset performed
 * is defined by the value that is written into this parameter.
 * Enumeration of:
 *         Cold
 *        Factory
 *        Warehouse
 *        Customer
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data from device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::set_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOK;
    char scriptbuff[100] = {'\0'};
    const char *val = stMsgData->paramValue;
    int rc;
    pthread_t reset_Thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    if(val[0] == '\0')
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s():] Set as NULL. Fail to execute \'X_COMCAST_COM_Reset\'Please give the correct input value as \
                       a \'Cold\', \'Factory\', \'Warehouse\' or \'Customer\' String. \n",__FUNCTION__);
        return ret;
    }
    if (0 == strcasecmp(val,"Cold"))
    {
        setResetState(ColdReset);
    }
    else if (0 == strcasecmp(val,"Factory"))
    {
        setResetState(FactoryReset);
    }
    else if (0 == strcasecmp(val,"Warehouse"))
    {
        setResetState(WarehouseReset);
    }
    else if (0 == strcasecmp(val,"Customer"))
    {
        setResetState(CustomerReset);
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Invalid input: %s. Please give the correct input value as a \'Cold\', \'Factory\', \'Warehouse\' or \'Customer\' String. \n",__FUNCTION__,val);
        return NOT_HANDLED;
    }

    rc = pthread_create(&reset_Thread, &attr, ResetFunc, NULL);
    if (rc) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"ERROR; return code from pthread_create() is %d\n", rc);
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"ERROR; failed to do \'%s\'  \n", val);
    }

    return OK;
}

/**
 * @brief This method resets according to the STB reset state such as NoReset, ColdReset
 * FactoryReset, WarehouseReset and CustomerReset.
 */
void *ResetFunc( void *)
{
    sleep(2);
    triggerResetScript();
    return NULL;

}

// Set 'm_xFirmwareDownloadUseCodebig' based on 'Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadUseCodebig' request
int hostIf_DeviceInfo::set_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadUseCodebig(HOSTIF_MsgData_t *stMsgData)
{
    m_xFirmwareDownloadUseCodebig = get_boolean(stMsgData->paramValue);
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s]Successfully set \"%s\" to \"%d\". \n", __FUNCTION__, stMsgData->paramName, m_xFirmwareDownloadUseCodebig);
    return OK;
}

// Set 'FW download Defer Reboot' based on 'Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadDeferReboot' request
int hostIf_DeviceInfo::set_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadDeferReboot(HOSTIF_MsgData_t *stMsgData)
{
    m_xFirmwareDownloadDeferReboot = get_boolean(stMsgData->paramValue);
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s]Successfully set \"%s\" to \"%d\". \n", __FUNCTION__, stMsgData->paramName, m_xFirmwareDownloadDeferReboot);
    return OK;
}

/**
 * @brief This method set the firmware download file path which is present in
 * "/opt/fwdnldstatus.txt"
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from device.
 * @retval NOK if not able to fetch the data from device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::set_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareToDownload(HOSTIF_MsgData_t *stMsgData)
{
//    int ret = NOK;
    m_xFirmwareToDownload = stMsgData->paramValue;
//    ret = writeFirmwareInfo((char *)"DnldFile",stMsgData);
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s]Successfully set \"%s\" to \"%s\". \n", __FUNCTION__, stMsgData->paramName, m_xFirmwareToDownload.c_str() );
    return OK;
}

/**
 * @brief This method set the status of the firmware download which is present in
 * "/opt/fwdnldstatus.txt"
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Return the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from device.
 * @retval NOK if not able to fetch the data from device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::set_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadStatus(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOK;
    writeFirmwareInfo((char *)"Status",stMsgData);
    return OK;
}

int hostIf_DeviceInfo::set_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadProtocol (HOSTIF_MsgData_t *stMsgData)
{
//    int ret = NOK;
    m_xFirmwareDownloadProtocol = stMsgData->paramValue;
//    ret = writeFirmwareInfo((char *)"Proto",stMsgData);
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s]Successfully set \"%s\" to \"%s\". \n", __FUNCTION__, stMsgData->paramName, m_xFirmwareDownloadProtocol.c_str() );
    return OK;
}

int hostIf_DeviceInfo::set_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadURL (HOSTIF_MsgData_t *stMsgData)
{
//    int ret = NOK;
    m_xFirmwareDownloadURL = stMsgData->paramValue;
//    ret = writeFirmwareInfo((char *)"DnldURL",stMsgData);
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s]Successfully set \"%s\" to \"%s\". \n", __FUNCTION__, stMsgData->paramName, m_xFirmwareDownloadURL.c_str() );
    return OK;
}

int hostIf_DeviceInfo::set_xOpsDMUploadLogsNow (HOSTIF_MsgData_t *stMsgData)
{
    bool triggerUploadLog  = false;
    char syscommand[256] = {'\0'};

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Entering... \n",__FUNCTION__);
    triggerUploadLog = get_boolean(stMsgData->paramValue);

    if(triggerUploadLog)
    {
        /*@ TODO: Execute the script;*/
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] Start executing script to upload logs... \n",__FUNCTION__);
        sprintf(syscommand,"%s &", LOG_UPLOAD_SCR);
        system(syscommand);
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"Successfully executed %s. \n", syscommand);
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]Failed, the set value is %d, so failed to execute . Please set as true(1)\
                to execute trigger upload now. \n",__FUNCTION__, triggerUploadLog );
        return OK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Exiting... \n",__FUNCTION__);
    return OK;
}

int hostIf_DeviceInfo::set_xOpsDMMoCALogEnabled (HOSTIF_MsgData_t *stMsgData)
{
    bool mocaLogEnabled  = false;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Entering... \n",__FUNCTION__);
    mocaLogEnabled = get_boolean(stMsgData->paramValue);
#ifdef USE_MoCA_PROFILE
    if (IARM_Bus_BroadcastEvent(IARM_BUS_NM_SRV_MGR_NAME, (IARM_EventId_t) IARM_BUS_NETWORK_MANAGER_MOCA_TELEMETRY_LOG, (void *)&mocaLogEnabled, sizeof(mocaLogEnabled)) == IARM_RESULT_SUCCESS)
    {
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] MoCA Telemetry Logging is %d \n",__FUNCTION__, mocaLogEnabled);
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] MoCA Telemetry Logging IARM FAILURE \n",__FUNCTION__);
    }
#endif
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Exiting... \n",__FUNCTION__);
    return OK;
}

int hostIf_DeviceInfo::set_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportEnable(HOSTIF_MsgData_t *stMsgData)
{
    const char *status = getStringValue(stMsgData).c_str();

    if(0 == strcasecmp(status,"false"))
    {

        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s] IPRemoteSupport disable request\n", __FUNCTION__);
        ifstream ifp(IPREMOTE_SUPPORT_STATUS_FILE);
        if(ifp.is_open())
        {
            if(remove(IPREMOTE_SUPPORT_STATUS_FILE) == 0)
            {
                RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Removed File %s, IPRemoteSupport is disabled\n", __FUNCTION__, IPREMOTE_SUPPORT_STATUS_FILE);
            }
        }

    }
    else if(0 == strcasecmp(status,"true"))
    {
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s] IPRemoteSupport enable request\n", __FUNCTION__);
        ofstream ofp(IPREMOTE_SUPPORT_STATUS_FILE);
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Created File %s, IPRemoteSupport is enabled\n", __FUNCTION__, IPREMOTE_SUPPORT_STATUS_FILE);
    }
    else
    {
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s] Value not passed for the param, IPRemoteSupport Status unchanged \n", __FUNCTION__);
    }

    return OK;
}

int hostIf_DeviceInfo::set_Device_DeviceInfo_X_RDKCENTRAL_COM_XRPollingAction(HOSTIF_MsgData_t *stMsgData)
{
    m_xrPollingAction = getStringValue(stMsgData);
    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s] Setting XRPollingAction to %s\n", __FUNCTION__, m_xrPollingAction.c_str());
    return OK;
}

size_t static writeCurlResponse(void *ptr, size_t size, size_t nmemb, string stream)
{
    size_t realsize = size * nmemb;
    string temp(static_cast<const char*>(ptr), realsize);
    stream.append(temp);
    return realsize;
}

int hostIf_DeviceInfo::set_Device_DeviceInfo_X_RDKCENTRAL_COM_Syndication_PartnerId(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Entering... \n",__FUNCTION__);
    string n_PartnerId="";
    string current_PartnerId="";
    int ret = NOK;

    n_PartnerId=getStringValue(stMsgData); /* this is the value to set from RFC */

    if(n_PartnerId.empty())
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] We received empty Buffer \n", __FUNCTION__);
        return NOK;
    }

    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Calling get_PartnerId_From_Curl \n",__FUNCTION__);

    /* get the value from the curl and see if they are different */
    ret = get_PartnerId_From_Curl (current_PartnerId);

    //we get a valid partnerID after the curl.
    if( !current_PartnerId.empty() && ret == OK )
    {
        if( n_PartnerId.compare(current_PartnerId) )
        {
            CURL *curl = curl_easy_init();
            bool upload_flag = false;
            if(curl)
            {
                /* We have different partner IDs
                 * set the partnerId using setpartnerid() */
                long http_code = 0;
                RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] call curl to set partner ID.. with New PartnerId = %s \n", __FUNCTION__, n_PartnerId);

                curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:50050/authService/setPartnerId");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)n_PartnerId.length());
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS,n_PartnerId.c_str());

                CURLcode res = curl_easy_perform(curl);

                if ( res == CURLE_OK )
                {
                    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
                    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] curl response : %d http response code: %ld\n", __FUNCTION__, res, http_code);
                }

                curl_easy_cleanup(curl);

                if( res == CURLE_OK && http_code == HTTP_OK )
                {
                    upload_flag = true;
                    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] PartnerID uploaded using Curl Success \n",__FUNCTION__);
                }
                else
                {
                    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] curl returned with error : %d http response code: %ld\n",\
                             __FUNCTION__, res, http_code);
                    return NOK;
                }
            }
            else {
                RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] curl init failed\n", __FUNCTION__);
                return NOK;
            }

            /* Reload the bootstrap config if CURLE_OK */
            int ret=NOK;

            if (upload_flag)
            {
                ret = m_bsStore->overrideValue(stMsgData);
            }
            if (ret == OK)
            {
                if ( !m_bsStore->call_loadJson() )
                {
                    RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] loadFromJson() failed for new partner id %s,\
                            continuing with older config from partner id %s \n", __FUNCTION__, n_PartnerId.c_str(), current_PartnerId.c_str());
                    return NOK;
                }
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Bootstrap update failed for new partner id %s,\
                        continuing with older config from partner id %s\n",__FUNCTION__,n_PartnerId.c_str(), current_PartnerId.c_str());
                return NOK;
            }
        }
        else
        {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] PartnerIDs are same. \n",__FUNCTION__);
            return OK;
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] We received empty Buffer \n", __FUNCTION__);
        return NOK;
    }

    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] PartnerID uploaded using Curl Success =>\
            Updated new partnerid:%s to authservice \n",__FUNCTION__, n_PartnerId.c_str());
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Exiting... \n",__FUNCTION__);
    return OK;
}

int static get_PartnerId_From_Curl( string& current_PartnerId )
{
    string response="";
    CURL *curl = curl_easy_init();

    if(curl)
    {
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] call curl to get partner ID..\n", __FUNCTION__);

        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:50050/authService/getDeviceId");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCurlResponse);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);

        long http_code = 0;

        if(res == CURLE_OK)
        {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] curl response : %d http response code: %ld\n", __FUNCTION__, res, http_code);
        }

        curl_easy_cleanup(curl);

        if(res == CURLE_OK && http_code == HTTP_OK )
        {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] curl response string = %s\n", __FUNCTION__, response.c_str());
            cJSON* root = cJSON_Parse(response.c_str());
            if(root)
            {
                cJSON* partnerID    = cJSON_GetObjectItem(root, "partnerId");
                if(partnerID->type == cJSON_String && partnerID->valuestring && strlen(partnerID->valuestring) > 0)
                {
                    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "Found partnerID value = %s\n", partnerID->valuestring);
                    current_PartnerId = partnerID->valuestring;
                }
                cJSON_Delete(root);
            }
            else
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] json parse error\n", __FUNCTION__);
                return NOK;
            }
        }
        else
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] curl returned error with : %d http response code: %ld\n", __FUNCTION__, res, http_code);
            return NOK;
        }
    }
    else
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] curl init failed\n", __FUNCTION__);
        return NOK;
    }

    return OK;
}


int hostIf_DeviceInfo::get_Device_DeviceInfo_X_RDKCENTRAL_COM_Syndication_PartnerId(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Entering... \n",__FUNCTION__);
    string current_PartnerId = "";
    int ret = NOK;

    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Calling get_PartnerId_From_Curl \n",__FUNCTION__);
    ret = get_PartnerId_From_Curl( current_PartnerId );
    /* we get a valid partnerID after the curl */
    if( !current_PartnerId.empty() && ret == OK )
    {
        stMsgData->paramLen = current_PartnerId.length();
        strncpy(stMsgData->paramValue, current_PartnerId.c_str(), stMsgData->paramLen);
        stMsgData->paramtype = hostIf_StringType;
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] We received empty Buffer after curl \n", __FUNCTION__);
        return NOK;
    }
    return OK;
}

int hostIf_DeviceInfo::set_xOpsDMMoCALogPeriod (HOSTIF_MsgData_t *stMsgData)
{
    unsigned int mocaLogDuration  = 3600;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Entering... \n",__FUNCTION__);
    mocaLogDuration = get_int(stMsgData->paramValue);

#ifdef USE_MoCA_PROFILE
    if (IARM_Bus_BroadcastEvent(IARM_BUS_NM_SRV_MGR_NAME, (IARM_EventId_t) IARM_BUS_NETWORK_MANAGER_MOCA_TELEMETRY_LOG_DURATION, (void *)&mocaLogDuration, sizeof(mocaLogDuration)) == IARM_RESULT_SUCCESS)
    {
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] MoCA Telemetry Logging Duration is %d \n",__FUNCTION__, mocaLogDuration);
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] MoCA Telemetry Logging Duration IARM FAILURE \n",__FUNCTION__);
    }
#endif
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Exiting... \n",__FUNCTION__);
    return OK;
}
int hostIf_DeviceInfo::get_xOpsDMMoCALogEnabled (HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Entering... \n",__FUNCTION__);
    IARM_Result_t retVal = IARM_RESULT_SUCCESS;
    bool param;
    memset(stMsgData->paramValue, 0, TR69HOSTIFMGR_MAX_PARAM_LEN);
#ifdef USE_MoCA_PROFILE
    retVal = IARM_Bus_Call(IARM_BUS_NM_SRV_MGR_NAME,IARM_BUS_NETWORK_MANAGER_MOCA_getTelemetryLogStatus, (void *)&param, sizeof(param));

    if(retVal == IARM_RESULT_SUCCESS)
    {
        put_boolean(stMsgData->paramValue,param);
        stMsgData->paramtype = hostIf_BooleanType;
        stMsgData->paramLen=1;
    }
    else
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] IARM Failed \n", __FUNCTION__, __FILE__, __LINE__);
#endif
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Exiting... \n",__FUNCTION__);
    return OK;
}

int hostIf_DeviceInfo::get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportEnable(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    const char *status = "false";
    ifstream ifp(IPREMOTE_SUPPORT_STATUS_FILE);
    if(ifp.is_open())
    {
        status="true";
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] %s exists, IpRemoteSupport Status is enabled\n", __FUNCTION__, IPREMOTE_SUPPORT_STATUS_FILE);

    }
    else
    {
        status="false";
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] File not exists, IpRemoteSupport Status is Disabled\n", __FUNCTION__);
    }

    snprintf((char *)stMsgData->paramValue, strlen(stMsgData->paramValue)-1, "%s", status);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);

    return OK;
}

int hostIf_DeviceInfo::get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportIpaddress(HOSTIF_MsgData_t *stMsgData)
{
    string line;
    errno_t rc = -1;
    char ipAddress[100] = {'\0'};
    ifstream remoteInterface_file(IPREMOTE_INTERFACE_INFO);
    try
    {
        if (remoteInterface_file.is_open())
        {
            while ( getline (remoteInterface_file,line) )
            {
                if (line.find("Ipv4_Address") !=string::npos)
                {
                    char *tmpStr = strstr((char *)line.c_str(), "=")  ;
                    tmpStr++;
                    while(isspace(*tmpStr)) {
                        tmpStr++;
                    }
                    rc=strcpy_s(ipAddress,sizeof(ipAddress), tmpStr);
		    if(rc!=EOK)
		    {
			ERR_CHK(rc);
		    }
                }
            }
            remoteInterface_file.close();
            snprintf((char *)stMsgData->paramValue, strlen(stMsgData->paramValue)-1, "%s",ipAddress);
        }
        else
        {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] File not exists, IpRemoteInterface file open failed \n", __FUNCTION__);

            //values not populated so unknown.
            rc=strcpy_s(ipAddress,sizeof(ipAddress),"unknown");
	    if(rc!=EOK)
            {
                   ERR_CHK(rc);
            }
            snprintf((char *)stMsgData->paramValue, strlen(stMsgData->paramValue)-1, "%s",ipAddress);
        }

        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);

        return OK;
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s()]Exception caught.  \n", __FUNCTION__);
        return NOK;
    }
}

int hostIf_DeviceInfo::get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportMACaddress(HOSTIF_MsgData_t *stMsgData)
{
    string line;
    errno_t rc = -1;
    char macAddress[100] = {'\0'};
    ifstream remoteInterface_file(IPREMOTE_INTERFACE_INFO);
    try
    {
        if (remoteInterface_file.is_open())
        {
            while ( getline (remoteInterface_file,line) )
            {
                if (line.find("MAC_Address") !=string::npos)
                {
                    char *tmpStr = strstr((char *)line.c_str(), "=")  ;
                    tmpStr++;
                    while(isspace(*tmpStr)) {
                        tmpStr++;
                    }
                    rc=strcpy_s(macAddress,sizeof(macAddress), tmpStr);
		    if(rc!=EOK)
		    {
			ERR_CHK(rc);
		    }
                }
            }
            remoteInterface_file.close();
            snprintf((char *)stMsgData->paramValue, strlen(stMsgData->paramValue)-1, "%s",macAddress);
        }
        else
        {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] File not exists, IpRemoteInterface file open failed \n", __FUNCTION__);

            //values not populated so unknown.
            rc=strcpy_s(macAddress,sizeof(macAddress),"unknown");
	    if(rc!=EOK)
            {
                  ERR_CHK(rc);
            }
            snprintf((char *)stMsgData->paramValue, strlen(stMsgData->paramValue)-1, "%s",macAddress);
        }

        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(stMsgData->paramValue);

        return OK;
    }
    catch (const std::exception e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s()]Exception caught.  \n", __FUNCTION__);
        return NOK;
    }
}

int hostIf_DeviceInfo::get_Device_DeviceInfo_X_RDKCENTRAL_COM_XRPollingAction(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] XRPollingAction = %s\n", __FUNCTION__, m_xrPollingAction.c_str());
    snprintf((char *)stMsgData->paramValue, strlen(stMsgData->paramValue)-1, "%s", m_xrPollingAction.c_str());
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);

    if(pChanged && m_xrPollingAction != "0") {
        *pChanged = true;
    }
    return OK;
}

int hostIf_DeviceInfo::get_xOpsDMMoCALogPeriod (HOSTIF_MsgData_t *stMsgData)
{

    IARM_Result_t retVal = IARM_RESULT_SUCCESS;
    unsigned int param;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Entering... \n",__FUNCTION__);
    memset(stMsgData->paramValue, 0, TR69HOSTIFMGR_MAX_PARAM_LEN);
#ifdef USE_MoCA_PROFILE
    retVal = IARM_Bus_Call(IARM_BUS_NM_SRV_MGR_NAME,IARM_BUS_NETWORK_MANAGER_MOCA_getTelemetryLogDuration, (void *)&param, sizeof(param));

    if(retVal == IARM_RESULT_SUCCESS)
    {
        put_int(stMsgData->paramValue,param);
        stMsgData->paramtype = hostIf_UnsignedIntType;
        stMsgData->paramLen=4;
    }
    else
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] IARM Failed \n", __FUNCTION__, __FILE__, __LINE__);
#endif
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Exiting... \n",__FUNCTION__);
    return OK;
}

// Check SHORTS RFC
bool hostIf_DeviceInfo::isShortsEnabled()
{
    HOSTIF_MsgData_t stRfcData = {0};
    strcpy(stRfcData.paramName, SHORTS_RFC_ENABLE);

    if((get_xRDKCentralComRFC(&stRfcData) == OK) && (strncmp(stRfcData.paramValue, "true", sizeof("true")) == 0))
    {
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] SHORTS enabled: %s \n",  __FUNCTION__, "True");
        return true;
    }
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] SHORTS enabled: %s \n",  __FUNCTION__, "False");
    return false;
}

// Find localhost port available in specified range for stunnel-client to listen
int hostIf_DeviceInfo::findLocalPortAvailable()
{
    struct sockaddr_in address;
    int sockfd = -1, status;
    int port = MIN_PORT_RANGE;

    while (port <= MAX_PORT_RANGE) {
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr("127.0.0.1");
        address.sin_port = htons(port);

        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        status = connect(sockfd, (struct sockaddr *)&address, sizeof(address));

        if (status<0)
        {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Port %d is available.\n", __FUNCTION__, port);
            close(sockfd);
            return port;
        }

        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] Port %d is in use.\n", __FUNCTION__, port);
        close(sockfd);
        port++;
    }
    return -1;
}

int hostIf_DeviceInfo::set_xOpsReverseSshTrigger(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Entering... \n",__FUNCTION__);
    string inputStr(stMsgData->paramValue);
    const string startShorts = "start shorts";
    bool trigger = strncmp(inputStr.c_str(),"start",strlen("start")) == 0;
    bool trigger_shorts = strncmp(inputStr.c_str(), startShorts.c_str(), startShorts.length()) == 0;

    if (trigger)
    {
#ifdef __SINGLE_SESSION_ONLY__
        if (!isRsshactive())
        {
#endif
            // Run stunnel client to establish stunnel.
            if (isShortsEnabled() && trigger_shorts) {
                RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Starting Stunnel \n",__FUNCTION__);
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] StunnelSSH Command  = /bin/sh %s %s %s %s \n",
                                                   __FUNCTION__,
                                                   stunnelCommand.c_str(),
                                                   stunnelSSHArgs.at("localport").c_str(),
                                                   stunnelSSHArgs.at("host").c_str(),
                                                   stunnelSSHArgs.at("stunnelport").c_str());
                v_secure_system("/bin/sh %s %s %s %s &", stunnelCommand.c_str(),
                                                   stunnelSSHArgs.at("localport").c_str(),
                                                   stunnelSSHArgs.at("host").c_str(),
                                                   stunnelSSHArgs.at("stunnelport").c_str());
            }

            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Starting SSH Tunnel \n",__FUNCTION__);
            string command = sshCommand + " start " + reverseSSHArgs;
            system(command.c_str());

            // Keep watching the pid of SSH tunnel. Once SSH session is closed or timed-out,
            // terminate the associated stunnel-client to close the stunnel.
            if (isShortsEnabled() && trigger_shorts) {
                RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Watching Tunnel for termination \n",__FUNCTION__);
                v_secure_system("/bin/sh %s &", watchTunnelCommand.c_str());
            }
#ifdef __SINGLE_SESSION_ONLY__
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF, "[%s] SSH Session is already active. Not starting again! \n",__FUNCTION__);
            return NOK;
        }
#endif
    }
    else
    {
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Stop SSH Tunnel \n",__FUNCTION__);
        string command = sshCommand + " stop ";
        system(command.c_str());
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Exiting... \n",__FUNCTION__);
    return OK;
}

int hostIf_DeviceInfo::get_xOpsReverseSshArgs(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Entering... \n",__FUNCTION__);

    memset(stMsgData->paramValue, 0, TR69HOSTIFMGR_MAX_PARAM_LEN);
    stMsgData->paramtype = hostIf_StringType;

    if (reverseSSHArgs.empty())
    {
        strncpy(stMsgData->paramValue, "Not Set", TR69HOSTIFMGR_MAX_PARAM_LEN );
    }
    else
    {
        strncpy(stMsgData->paramValue, reverseSSHArgs.c_str(), sizeof(stMsgData->paramValue) -1);  //CID:136563 - Buffer size warning
        stMsgData->paramValue[sizeof(stMsgData->paramValue) -1] = '\0';
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Exiting ... \n",__FUNCTION__);
    return OK;
}

int hostIf_DeviceInfo::set_xOpsReverseSshArgs(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Entering... \n",__FUNCTION__);
    try
    {
        map<string,string> parsedMap;
        string inputStr(stMsgData->paramValue);
        std::size_t start = inputStr.find_first_not_of(";"), end = start;
        while (start != string::npos)
        {
            end = inputStr.find(";",start);
            string chunk = inputStr.substr(start,end - start);
            std::size_t keyEnd = chunk.find("=");
            parsedMap[chunk.substr(0,keyEnd)] = chunk.substr(keyEnd +1);
            start = inputStr.find_first_not_of(";",end);
        }
        string parsedValues;
        for (auto &it : parsedMap)
        {
            parsedValues += "key = " + it.first + " value = " + it.second + ";";
        }
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] parsed Values are : %s\n",__FUNCTION__,parsedValues.c_str());

        reverseSSHArgs = " -I " + parsedMap["idletimeout"] + " -f -N -y -T -R " + parsedMap["revsshport"] + ":";
        string estbip = getEstbIp();
        unsigned char buf[sizeof(struct in6_addr)];
        //Check if estbip is ipv6 address
        if (inet_pton(AF_INET6, estbip.c_str(), buf))
        {
            reverseSSHArgs += "[" + estbip + "]";
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] SSH Tunnel estb ipv6 address is : %s\n",__FUNCTION__,estbip.c_str());
        }
        else
        {
            reverseSSHArgs += estbip;
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] SSH Tunnel estb ipv4 address is : %s\n",__FUNCTION__,estbip.c_str());
        }

        reverseSSHArgs +=  ":22 " + parsedMap["user"] + "@";
        if (isShortsEnabled() && parsedMap.count("stunnelport")) {
            string localPort = to_string(findLocalPortAvailable());
            if (localPort == "-1") {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Reserved ports are not availale... \n",__FUNCTION__);
                return NOK;
            }
            reverseSSHArgs += string("localhost") + " -p " + localPort;

            // Arguments for stunnel script in the form " Local port + Remote FQDN + Stunnel port "
            stunnelSSHArgs["localport"] = localPort;
            stunnelSSHArgs["host"] = parsedMap.at("host");
            stunnelSSHArgs["stunnelport"] = parsedMap.at("stunnelport");
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] Stunnel Args = %s %s %s \n",
                                                       __FUNCTION__, localPort,
                                                       parsedMap.at("host"),
                                                       parsedMap.at("stunnelport"));
        } else {
            reverseSSHArgs += parsedMap["host"] + " -p " + parsedMap["sshport"];
        }

        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] String is  : %s\n",__FUNCTION__,reverseSSHArgs.c_str());

        string::const_iterator it = std::find_if(reverseSSHArgs.begin(), reverseSSHArgs.end(), [](char c) {
            return !(isalnum(c) || (c == ' ') || (c == ':') || (c == '-') || (c == '.') || (c == '@') || (c == '_') || (c == '[') || (c == ']'));
        });

        if (it  != reverseSSHArgs.end())
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Exception Accured... \n",__FUNCTION__);
            reverseSSHArgs = "";
            return NOK;
        }

        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] ReverseSSH Args = %s \n",__FUNCTION__,reverseSSHArgs.c_str());

    } catch (const std::exception e) {
        std::cout << __FUNCTION__ << "An exception occurred. " << e.what() << endl;

        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Exception Accured... \n",__FUNCTION__);
        reverseSSHArgs = "";
        return NOK;
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Exiting... \n",__FUNCTION__);
    return OK;
}

int hostIf_DeviceInfo::get_xOpsReverseSshStatus(HOSTIF_MsgData_t *stMsgData)
{
    const string activeStr("ACTIVE");
    const string inActiveStr("INACTIVE");
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Entering... \n",__FUNCTION__);
    memset(stMsgData->paramValue, 0, TR69HOSTIFMGR_MAX_PARAM_LEN);
    stMsgData->paramtype = hostIf_StringType;

    if (isRsshactive())
    {
        strncpy(stMsgData->paramValue, activeStr.c_str(), sizeof(stMsgData->paramValue) -1);  //CID:136402 - Buffer size warning
        stMsgData->paramValue[sizeof(stMsgData->paramValue) -1] = '\0';
    }
    else
    {
        strncpy(stMsgData->paramValue, inActiveStr.c_str(), TR69HOSTIFMGR_MAX_PARAM_LEN );
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] Exiting... \n",__FUNCTION__);
    return OK;
}




int hostIf_DeviceInfo::get_xOpsDeviceMgmtForwardSSHEnable(HOSTIF_MsgData_t * stMsgData)
{
    LOG_ENTRY_EXIT;
    char ForwardSSH[10]="true";
    char *p = NULL;
    char dataToBeRead[50];
    FILE *fp;

    if((fp=fopen(FORWARD_SSH_FILE,"r"))==NULL)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]Unable to open file\n", __FUNCTION__);
        put_boolean(stMsgData->paramValue, true);  //Default value
        return NOK;
    }
    else
    {
        while( fgets ( dataToBeRead, 50, fp ) != NULL )
        {
            p = strchr(dataToBeRead,'=');
            sprintf( ForwardSSH,"%s", ++p ) ;
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Enable value:%s --> Parameter[%s]\n", __FUNCTION__, ForwardSSH, stMsgData->paramName);
            if( strcmp( ForwardSSH,"true"))
            {
                put_boolean(stMsgData->paramValue, false);
            }
            else
            {
                put_boolean(stMsgData->paramValue, true);
            }
        }
    }

    fclose(fp);
    return OK;
}

int hostIf_DeviceInfo::set_xOpsDeviceMgmtForwardSSHEnable(HOSTIF_MsgData_t * stMsgData)
{
    errno_t rc = -1;
    LOG_ENTRY_EXIT;
    char ForwardSSH[10]="true";   //Default value
    FILE *fp;

    if((fp=fopen(FORWARD_SSH_FILE,"w"))==NULL)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]Unable to open file\n", __FUNCTION__);
        return NOK;
    }
    else
    {
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Default Enable value:%s --> Parameter[%s]\n", __FUNCTION__, ForwardSSH, stMsgData->paramName);
        /*rc = strcpy_s(ForwardSSH, sizeof(ForwardSSH), get_boolean(stMsgData->paramValue) ? "true" : "false");
        if (rc != EOK) 
	{
        	ERR_CHK(rc);
    	}*/
	if (get_boolean(stMsgData->paramValue))
        {
            rc=strcpy_s(ForwardSSH,sizeof(ForwardSSH),"true");
	    if(rc!=EOK)
	    {
		ERR_CHK(rc);
	
    	    }
	}
        else
        {
            rc=strcpy_s(ForwardSSH,sizeof(ForwardSSH),"false");
	    if(rc!=EOK)
            {
                ERR_CHK(rc);
            }
        }
	fprintf(fp,"ForwardSSH=%s", ForwardSSH);
    }
    fclose(fp);
    return OK;
}


#ifdef USE_HWSELFTEST_PROFILE
int hostIf_DeviceInfo::set_xOpsDeviceMgmt_hwHealthTest_Enable(HOSTIF_MsgData_t *stMsgData)
{
    return hwselftest::set_Device_DeviceInfo_xOpsDeviceMgmt_hwHealthTest_Enable(LOG_TR69HOSTIF, stMsgData)? OK : NOK;
}

int hostIf_DeviceInfo::set_xOpsDeviceMgmt_hwHealthTest_ExecuteTest(HOSTIF_MsgData_t *stMsgData)
{
    return hwselftest::set_Device_DeviceInfo_xOpsDeviceMgmt_hwHealthTest_ExecuteTest(LOG_TR69HOSTIF, stMsgData)? OK : NOK;
}

int hostIf_DeviceInfo::get_xOpsDeviceMgmt_hwHealthTest_Results(HOSTIF_MsgData_t *stMsgData)
{
    return hwselftest::get_Device_DeviceInfo_xOpsDeviceMgmt_hwHealthTest_Results(LOG_TR69HOSTIF, stMsgData)? OK : NOK;
}

int hostIf_DeviceInfo::set_xOpsDeviceMgmt_hwHealthTest_SetTuneType(HOSTIF_MsgData_t *stMsgData)
{
    return hwselftest::set_Device_DeviceInfo_xOpsDeviceMgmt_hwHealthTest_SetTuneType(LOG_TR69HOSTIF, stMsgData)? OK : NOK;
}

int hostIf_DeviceInfo::set_xOpsDeviceMgmt_hwHealthTest_ExecuteTuneTest(HOSTIF_MsgData_t *stMsgData)
{
    return hwselftest::set_Device_DeviceInfo_xOpsDeviceMgmt_hwHealthTest_ExecuteTuneTest(LOG_TR69HOSTIF, stMsgData)? OK : NOK;
}

int hostIf_DeviceInfo::get_xOpsDeviceMgmt_hwHealthTestTune_TuneResults(HOSTIF_MsgData_t *stMsgData)
{
    return hwselftest::get_Device_DeviceInfo_xOpsDeviceMgmt_hwHealthTestTune_TuneResults(LOG_TR69HOSTIF, stMsgData)? OK : NOK;
}

int hostIf_DeviceInfo::set_xOpsDeviceMgmt_hwHealthTest_EnablePeriodicRun(HOSTIF_MsgData_t *stMsgData)
{
    return hwselftest::set_Device_DeviceInfo_xOpsDeviceMgmt_hwHealthTest_EnablePeriodicRun(LOG_TR69HOSTIF, stMsgData)? OK : NOK;
}

int hostIf_DeviceInfo::set_xOpsDeviceMgmt_hwHealthTest_PeriodicRunFrequency(HOSTIF_MsgData_t *stMsgData)
{
    return hwselftest::set_Device_DeviceInfo_xOpsDeviceMgmt_hwHealthTest_PeriodicRunFrequency(LOG_TR69HOSTIF, stMsgData)? OK : NOK;
}

int hostIf_DeviceInfo::set_xOpsDeviceMgmt_hwHealthTest_CpuThreshold(HOSTIF_MsgData_t *stMsgData)
{
    return hwselftest::set_Device_DeviceInfo_xOpsDeviceMgmt_hwHealthTest_CpuThreshold(LOG_TR69HOSTIF, stMsgData)? OK : NOK;
}

int hostIf_DeviceInfo::set_xOpsDeviceMgmt_hwHealthTest_DramThreshold(HOSTIF_MsgData_t *stMsgData)
{
    return hwselftest::set_Device_DeviceInfo_xOpsDeviceMgmt_hwHealthTest_DramThreshold(LOG_TR69HOSTIF, stMsgData)? OK : NOK;
}

int hostIf_DeviceInfo::set_RFC_hwHealthTestWAN_WANEndPointURL(HOSTIF_MsgData_t *stMsgData)
{
    return hwselftest::set_Device_DeviceInfo_RFC_hwHealthTestWAN_WANEndPointURL(LOG_TR69HOSTIF, stMsgData)? OK : NOK;
}

int hostIf_DeviceInfo::set_xRDKCentralComRFC_hwHealthTest_ResultFilter_Enable(HOSTIF_MsgData_t *stMsgData)
{
    return hwselftest::set_Device_DeviceInfo_xRDKCentralComRFC_hwHealthTest_ResultFilter_Enable(LOG_TR69HOSTIF, stMsgData)? OK : NOK;
}

int hostIf_DeviceInfo::set_xRDKCentralComRFC_hwHealthTest_ResultFilter_QueueDepth(HOSTIF_MsgData_t *stMsgData)
{
    return hwselftest::set_Device_DeviceInfo_xRDKCentralComRFC_hwHealthTest_ResultFilter_QueueDepth(LOG_TR69HOSTIF, stMsgData)? OK : NOK;
}

int hostIf_DeviceInfo::set_xRDKCentralComRFC_hwHealthTest_ResultFilter_FilterParams(HOSTIF_MsgData_t *stMsgData)
{
    return hwselftest::set_Device_DeviceInfo_xRDKCentralComRFC_hwHealthTest_ResultFilter_FilterParams(LOG_TR69HOSTIF, stMsgData)? OK : NOK;
}

int hostIf_DeviceInfo::set_xRDKCentralComRFC_hwHealthTest_ResultFilter_ResultsFiltered(HOSTIF_MsgData_t *stMsgData)
{
    return hwselftest::set_Device_DeviceInfo_xRDKCentralComRFC_hwHealthTest_ResultFilter_ResultsFiltered(LOG_TR69HOSTIF, stMsgData)? OK : NOK;
}
#endif /* USE_HWSELFTEST_PROFILE */
/*
 * * int hostIf_DeviceInfo::validate_ParamValue(HOSTIF_MsgData * sMsgData)
 * * in : stMsgData pointer
 * * out : int OK/NOK
 * * this method is used to validate the RFC param Values limits for SET
 * */
int hostIf_DeviceInfo::validate_ParamValue(HOSTIF_MsgData_t * stMsgData)
{
    int ret = OK;
    if (strcasecmp(stMsgData->paramName,TR069DOSLIMIT_THRESHOLD) == 0)
    {
        long int tmpVal;
        if(stMsgData->paramtype == hostIf_StringType )
        {
            tmpVal = strtol(stMsgData->paramValue,NULL,10);
        }
        else if(stMsgData->paramtype == hostIf_UnsignedIntType )
        {
            tmpVal = get_uint(stMsgData->paramValue);
        }
        else
        {
            ret = NOK;
            stMsgData->faultCode = fcInvalidParameterType;
            return ret;
        }
        if (tmpVal < MIN_TR69_DOS_THRESHOLD || tmpVal > MAX_TR69_DOS_THRESHOLD)
        {
            ret = NOK;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Failed due to wrong Value,Value should be[0-30] for %s to set.\n", __FUNCTION__, __LINE__, stMsgData->paramName);
            stMsgData->faultCode = fcInvalidParameterValue;
        }
    }
    return ret;
}

int hostIf_DeviceInfo::set_xRDKCentralComBootstrap(HOSTIF_MsgData_t * stMsgData)
{
    int ret = NOK;
    ret = m_bsStore->overrideValue(stMsgData);
    return ret;
}

int hostIf_DeviceInfo::set_xRDKCentralComRFC(HOSTIF_MsgData_t * stMsgData)
{
    int ret = NOK;
    int validate_paramVal;

    // any additional immediate handling
    if (strcasecmp(stMsgData->paramName,TR181_RFC_RESET_DATA_START) == 0) // used to clear out all data from storage
    {
#ifndef NEW_HTTP_SERVER_DISABLE
        if(!legacyRFCEnabled())
        {
            string stringValue = getStringValue(stMsgData);
            if(!strcasecmp(stringValue.c_str(), "true") || !strcmp(stringValue.c_str(), "1"))
            {
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] Calling clearAll in New RFC Store I/O\n",__FUNCTION__);
                m_rfcStore->clearAll();
                ret = OK;
                stMsgData->faultCode = fcNoFault;
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Invalid Value : Only true is allowed\n",__FUNCTION__);
                stMsgData->faultCode = fcInternalError;
            }
        }
        else
        {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] Calling clearAll in Old RFC Storage I/O\n",__FUNCTION__);
            m_rfcStorage.clearAll();
        }
#else
        m_rfcStorage.clearAll();
#endif
    }
    else if(strcasecmp(stMsgData->paramName, TR181_RFC_RESET_DATA_END) == 0)
    {
#ifndef NEW_HTTP_SERVER_DISABLE
        if(!legacyRFCEnabled())
        {
            string stringValue = getStringValue(stMsgData);
            if(!strcasecmp(stringValue.c_str(), "true") || !strcmp(stringValue.c_str(), "1"))
            {
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] Calling reloadCache in New RFC Store I/O\n",__FUNCTION__);
                m_rfcStore->reloadCache();
                ret = OK;
                stMsgData->faultCode = fcNoFault;
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Invalid Value : Only true is allowed\n",__FUNCTION__);
                stMsgData->faultCode = fcInternalError;
            }
        }
#endif
    }
    else
    {
        validate_paramVal = validate_ParamValue(stMsgData);
        if(validate_paramVal == OK)
        {
#ifndef NEW_HTTP_SERVER_DISABLE
            if(!legacyRFCEnabled())
            {
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] Calling setValue in New RFC Store I/O\n",__FUNCTION__);
                ret = m_rfcStore->setValue(stMsgData);
            }
            else
            {
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] Calling setValue in Old RFC Storage I/O\n",__FUNCTION__);
                ret = m_rfcStorage.setValue(stMsgData);
            }
#else
            ret = m_rfcStorage.setValue(stMsgData);
#endif
        }
        else
        {
            ret = NOK;
            stMsgData->faultCode = fcInvalidParameterValue;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Invalid ParamValue/Type to SET for param [%s] \n", __FUNCTION__, __LINE__, stMsgData->paramName);
        }
    }

    if (strcasecmp(stMsgData->paramName,RFC_WL_ROAM_TRIGGER_RF) == 0)
    {
        ret = set_xRDKCentralComRFCRoamTrigger(stMsgData);
    }
    else if (strcasecmp(stMsgData->paramName,MS12_DAPV2_RFC_ENABLE) == 0)
    {
        bool enable = get_boolean(stMsgData->paramValue);
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] MS12->DAPV2 RFC status:%d\n",__FUNCTION__, enable);
        if(enable)
        {
            device::Host::getInstance().getAudioOutputPort("HDMI0").enableMS12Config(dsMS12FEATURE_DAPV2,1);
        }
        else
        {
            device::Host::getInstance().getAudioOutputPort("HDMI0").enableMS12Config(dsMS12FEATURE_DAPV2,0);
        }
    }
    else if (strcasecmp(stMsgData->paramName,MS12_DE_RFC_ENABLE) == 0)
    {
        bool enable = get_boolean(stMsgData->paramValue);
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] MS12->DE RFC status:%d\n",__FUNCTION__, enable);
        if(enable)
        {
            device::Host::getInstance().getAudioOutputPort("HDMI0").enableMS12Config(dsMS12FEATURE_DE,1);
        }
        else
        {
            device::Host::getInstance().getAudioOutputPort("HDMI0").enableMS12Config(dsMS12FEATURE_DE,0);
        }
    }
    else if (strcasecmp(stMsgData->paramName,LE_RFC_ENABLE) == 0)
    {
       ret = set_xRDKCentralComRFCLoudnessEquivalenceEnable(stMsgData);
    }
    else if (strcasecmp(stMsgData->paramName,HDR_RFC_ENABLE) == 0)
    {
        ret = set_xRDKCentralComHdrRFCEnable(stMsgData);
    }
    else if (strcasecmp(stMsgData->paramName,UHD_RFC_ENABLE) == 0)
    {
        ret = set_xRDKCentralComUhdRFCEnable(stMsgData);
    }
    else if (strcasecmp(stMsgData->paramName,XRE_CONTAINER_RFC_ENABLE) == 0)
    {
        ret = set_xRDKCentralComXREContainerRFCEnable(stMsgData);
    }
    else if (strcasecmp(stMsgData->paramName,RFC_CTL_RETRIEVE_NOW) == 0)
    {
        ret = set_xRDKCentralComRFCRetrieveNow(stMsgData);
    }
#ifdef ENABLE_LLAMA_PLATCO
    else if (!strcasecmp(stMsgData->paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.VideoTelemetry.FrequncyMinutes"))
    {
        ret = set_xRDKCentralComRFCVideoTelFreq(stMsgData);
    }
#endif
    else if (!strcasecmp(stMsgData->paramName,TR181_AUTOREBOOT_ENABLE) )
    {
        ret = set_xRDKCentralComRFCAutoRebootEnable(stMsgData);
    }
#ifdef USE_HWSELFTEST_PROFILE
    else if (!strcasecmp(stMsgData->paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.hwHealthTest.Enable"))
    {
        ret = set_xOpsDeviceMgmt_hwHealthTest_Enable(stMsgData);
    }
    else if (!strcasecmp(stMsgData->paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.hwHealthTest.EnablePeriodicRun"))
    {
        ret = set_xOpsDeviceMgmt_hwHealthTest_EnablePeriodicRun(stMsgData);
    }
    else if (!strcasecmp(stMsgData->paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.hwHealthTest.PeriodicRunFrequency"))
    {
        ret = set_xOpsDeviceMgmt_hwHealthTest_PeriodicRunFrequency(stMsgData);
    }
    else if (!strcasecmp(stMsgData->paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.hwHealthTest.cpuThreshold"))
    {
        ret = set_xOpsDeviceMgmt_hwHealthTest_CpuThreshold(stMsgData);
    }
    else if (!strcasecmp(stMsgData->paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.hwHealthTest.dramThreshold"))
    {
        ret = set_xOpsDeviceMgmt_hwHealthTest_DramThreshold(stMsgData);
    }
    else if (!strcasecmp(stMsgData->paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.hwHealthTestWAN.WANTestEndPointURL"))
    {
        ret = set_RFC_hwHealthTestWAN_WANEndPointURL(stMsgData);
    }
    else if (!strcasecmp(stMsgData->paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.hwHealthTest.ResultFilter.Enable"))
    {
        ret = set_xRDKCentralComRFC_hwHealthTest_ResultFilter_Enable(stMsgData);
    }
    else if (!strcasecmp(stMsgData->paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.hwHealthTest.ResultFilter.QueueDepth"))
    {
        ret = set_xRDKCentralComRFC_hwHealthTest_ResultFilter_QueueDepth(stMsgData);
    }
    else if (!strcasecmp(stMsgData->paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.hwHealthTest.ResultFilter.FilterParams"))
    {
        ret = set_xRDKCentralComRFC_hwHealthTest_ResultFilter_FilterParams(stMsgData);
    }
    else if (!strcasecmp(stMsgData->paramName, "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.hwHealthTest.ResultFilter.ResultsFiltered"))
    {
        ret = set_xRDKCentralComRFC_hwHealthTest_ResultFilter_ResultsFiltered(stMsgData);
    }
#endif /* USE_HWSELFTEST_PROFILE */
    return ret;
}

int hostIf_DeviceInfo::get_xRDKCentralComBootstrap(HOSTIF_MsgData_t *stMsgData)
{
    return m_bsStore->getValue(stMsgData);
}

int hostIf_DeviceInfo::get_xRDKCentralComRFC(HOSTIF_MsgData_t *stMsgData)
{
    int ret=NOK;
#ifndef NEW_HTTP_SERVER_DISABLE
    if(!legacyRFCEnabled())
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] Calling getValue in New RFC Store I/O\n",__FUNCTION__);
        ret=m_rfcStore->getValue(stMsgData);
    }
    else
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] Calling getValue in Old RFC Storage I/O\n",__FUNCTION__);
        ret=m_rfcStorage.getValue(stMsgData);
    }
#else
    ret=m_rfcStorage.getValue(stMsgData);
#endif

    if (strcasecmp(stMsgData->paramName,"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.AccountInfo.AccountID") == 0 && (strlen(stMsgData->paramValue) == 0 || strcasecmp(stMsgData->paramValue,"unknown") == 0))
    {
        if ((ret = get_xRDKCentralComRFCAccountId(stMsgData)) == OK)
        {
           // Store the value from authservice into the db so we don't get here again
           m_rfcStorage.setValue(stMsgData);
        }
    }

    return ret;
}

int hostIf_DeviceInfo::get_xRDKCentralComRFCAccountId(HOSTIF_MsgData_t *stMsgData)
{
    int ret=NOK;
    string response;
    CURL *curl = curl_easy_init();
    if(curl)
    {
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: call curl to get Account ID..\n", __FUNCTION__);
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:50050/authService/getServiceAccountId");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCurlResponse);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        CURLcode res = curl_easy_perform(curl);
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: curl response : %d http response code: %ld\n", __FUNCTION__, res, http_code);
        curl_easy_cleanup(curl);

        if(res == CURLE_OK)
        {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: curl response string = %s\n", __FUNCTION__, response.c_str());
            cJSON* root = cJSON_Parse(response.c_str());
            if(root)
            {
                cJSON* accountIdObj    = cJSON_GetObjectItem(root, "serviceAccountId");
                if(accountIdObj->type == cJSON_String && accountIdObj->valuestring && strlen(accountIdObj->valuestring) > 0)
                {
                    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "Found accountId value = %s\n", accountIdObj->valuestring);
                    putValue(stMsgData, accountIdObj->valuestring);
                    stMsgData->faultCode = fcNoFault;
                    ret = OK;
                }
                cJSON_Delete(root);
            }
            else
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: json parse error\n", __FUNCTION__);
            }
        }
    }
    else
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: curl init failed\n", __FUNCTION__);
    }
    return ret;
}

int hostIf_DeviceInfo::set_xRDKCentralComRFCAutoRebootEnable(HOSTIF_MsgData_t *stMsgData)
{
    bool enableStatus = false;
    int ret = OK;
    int l_uptime = 0;
    bool bValue = false;

    if(stMsgData->paramtype == hostIf_BooleanType)
    {
        enableStatus=get_boolean(stMsgData->paramValue);

        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] %s: %s\n",  __FUNCTION__, __LINE__ \
                ,stMsgData->paramName, (enableStatus)? "true":"false");

        /* Call Schedule Auto Reboot */
        ret = ScheduleAutoReboot(enableStatus);
        if ( NOK == ret )
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s] ScheduleAutoReboot Failed \n", __FUNCTION__);
        }
    }
    else
    {
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s] Value Incorrect, Auto Reboot Enable status unchanged \n" \
                , __FUNCTION__);
        ret=NOK;
    }

    return ret;
}

int hostIf_DeviceInfo::ScheduleAutoReboot(bool bValue)
{
    int ret = OK;
    char cmd[100] = {'\0'};


    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d]  bValue = %s\n" \
            ,  __FUNCTION__, __LINE__, bValue? "true" : "false" );

    /* Call the script for scheduling
     * cron args with Reboot day and bValue */
    snprintf(cmd,sizeof(cmd),"sh /lib/rdk/ScheduleAutoReboot.sh %d &", bValue);
#ifdef YOCTO_BUILD
    v_secure_system("sh /lib/rdk/ScheduleAutoReboot.sh %d &", bValue);
#else
    system(cmd);
#endif
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] Successfully executed \"%s\". \n", __FUNCTION__, __LINE__, cmd);
    ret = OK;
    return ret;
}

int hostIf_DeviceInfo::set_xRDKCentralComRFCRoamTrigger(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOK;
    LOG_ENTRY_EXIT;
    //if(stMsgData->paramtype)
    {
        char execBuf[100] = {'\0'};
        sprintf(execBuf, "wl roam_trigger %s &", stMsgData->paramValue);
#ifdef YOCTO_BUILD
        v_secure_system("wl roam_trigger %s &", stMsgData->paramValue);
#else
        system(execBuf);
#endif
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] Successfully executed \"%s\" with \"%s\". \n", __FUNCTION__, __LINE__, stMsgData->paramName, execBuf);
        ret = OK;
    }
    system("wl roam_trigger &");
    return ret;
}

int hostIf_DeviceInfo::set_xRDKCentralComRFCLoudnessEquivalenceEnable(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOK;
    bool enable = false;
    dsError_t status = dsERR_GENERAL;

    if(stMsgData->paramtype == hostIf_BooleanType)
    {
        enable = get_boolean(stMsgData->paramValue);

        //set the value TRUE/FALSE.
        status = device::Host::getInstance().getAudioOutputPort("HDMI0").enableLEConfig((enable)?1:0);
        if(status != dsERR_NONE)
        {
            /* We return NOK and log for all failed/unsupported calls. */
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] enableLEConfig failed/unsupported \n",__FUNCTION__);
            ret = NOK;
        }
        else
        {
            ret = OK;
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Failed due to wrong data type for %s, please use boolean(0/1) to set.\n"
                , __FUNCTION__, __LINE__, stMsgData->paramName);
        stMsgData->faultCode = fcInvalidParameterType;
    }
    return ret;
}

int hostIf_DeviceInfo::set_xRDKCentralComHdrRFCEnable(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOK;
    bool enable = false;
    LOG_ENTRY_EXIT;
    if(stMsgData->paramtype == hostIf_BooleanType)
    {
        enable = get_boolean(stMsgData->paramValue);
        device::VideoDevice decoder = device::Host::getInstance().getVideoDevices().at(0);
        decoder.forceDisableHDRSupport(!enable);
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] Successfully set \"%s\" to \"%d\". \n", __FUNCTION__, __LINE__, stMsgData->paramName, enable);
        ret = OK;
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Failed due to wrong data type for %s, please use boolean(0/1) to set.\n", __FUNCTION__, __LINE__, stMsgData->paramName);
        stMsgData->faultCode = fcInvalidParameterType;
    }

    return ret;
}

int hostIf_DeviceInfo::set_xRDKCentralComXREContainerRFCEnable(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOK;
    bool enable;
    LOG_ENTRY_EXIT;
    if(stMsgData->paramtype == hostIf_BooleanType)
    {
        enable = get_boolean(stMsgData->paramValue);
        if( enable )
        {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s] XRE_ContainerSupport enable request\n", __FUNCTION__);
            ofstream ofp(XRE_CONTAINER_SUPPORT_STATUS_FILE);
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Created File %s, XRE_ContainerSupport is enabled\n", __FUNCTION__, XRE_CONTAINER_SUPPORT_STATUS_FILE);
            ret = OK;
        }
        else
        {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s] XRE_ContainerSupport disable request\n", __FUNCTION__);
            ifstream ifp(XRE_CONTAINER_SUPPORT_STATUS_FILE);
            if(ifp.is_open())
            {
                if(remove(XRE_CONTAINER_SUPPORT_STATUS_FILE) == 0)
                {
                    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Removed File %s, XRE_ContainerSupport is disabled\n", __FUNCTION__, XRE_CONTAINER_SUPPORT_STATUS_FILE);
                    ret = OK;
                }
                else
                {
                    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Unable to remove File %s, XRE_ContainerSupport Status unchanged\n", __FUNCTION__, XRE_CONTAINER_SUPPORT_STATUS_FILE);
                }
            }
            else
            {
                RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] File %s is already removed, XRE_ContainerSupport is disabled already\n", __FUNCTION__, XRE_CONTAINER_SUPPORT_STATUS_FILE);
                ret = OK;
            }
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Failed due to wrong data type for %s, please use boolean(0/1) to set.\n", __FUNCTION__, __LINE__, stMsgData->paramName);
    }
    return ret;
}

int hostIf_DeviceInfo::set_xRDKCentralComUhdRFCEnable(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOK;
    bool enable = false;
    LOG_ENTRY_EXIT;
    if(stMsgData->paramtype == hostIf_BooleanType)
    {
        enable = get_boolean(stMsgData->paramValue);
        std::string strVideoPort = device::Host::getInstance().getDefaultVideoPortName();
        device::Host::getInstance().getVideoOutputPort(strVideoPort.c_str()).forceDisable4KSupport(!enable);
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] Successfully set \"%s\" to \"%d\". \n", __FUNCTION__, __LINE__, stMsgData->paramName, enable);
        ret = OK;
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Failed due to wrong data type for %s, please use boolean(0/1) to set.\n", __FUNCTION__, __LINE__, stMsgData->paramName);
        stMsgData->faultCode = fcInvalidParameterType;
    }

    return ret;
}

// Handle RFC RetrieveNow
int hostIf_DeviceInfo::set_xRDKCentralComRFCRetrieveNow(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOK;
    struct timeval currTime;
    char cmd[100] = {'\0'};

    LOG_ENTRY_EXIT;

    sprintf(cmd, "sh /lib/rdk/RFCbase.sh &");
    system (cmd);
    ret = OK;
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] Successfully executed \"%s\" with \"%s\". \n", __FUNCTION__, __LINE__, stMsgData->paramName, cmd);
    // log the last RFC request
    if(gettimeofday(&currTime, NULL))
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in sysinfo due to \'%s\' (%d).  \n",  strerror(errno), errno);
    }
    else
    {
        put_ulong(stMsgData->paramValue, (unsigned int)currTime.tv_sec);
        stMsgData->paramtype = hostIf_UnsignedIntType;
        stMsgData->paramLen = sizeof(hostIf_UnsignedIntType);
        // set the time in Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.RetrieveNow parameter
#ifndef NEW_HTTP_SERVER_DISABLE
        ret = m_rfcStore->setValue(stMsgData);
#else
        ret = m_rfcStorage.setValue(stMsgData);
#endif
    }

    return ret;
}

#ifdef ENABLE_LLAMA_PLATCO
int hostIf_DeviceInfo::set_xRDKCentralComRFCVideoTelFreq(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOK;
    unsigned int tmpVal = 0;
    char execBuf[100] = {'\0'};
    if (stMsgData->paramtype == hostIf_UnsignedIntType)
    {
        tmpVal = get_uint(stMsgData->paramValue);
        if (tmpVal > 0 && tmpVal <=60)
        {
            sprintf(execBuf, "sh /lib/rdk/vdec-statistics.sh %d &", tmpVal);
            v_secure_system("sh /lib/rdk/vdec-statistics.sh %d &", tmpVal);
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] Successfully executed \"%s\" with \"%s\". \n", __FUNCTION__, __LINE__, stMsgData->paramName, execBuf);
            ret = OK;
        }
        else
        {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] FrequncyMinutes value should be in the range between 1 to 60 (Minutes) \n", __FUNCTION__, __LINE__); 
        }
    }
    return ret;
}
#endif
int get_ParamValue_From_TR69Agent(HOSTIF_MsgData_t * stMsgData)
{
    int ret = OK;
    FILE *fp;
    char cmd[400] = {'\0'};
    sprintf(cmd, "%s %s %s", "/usr/local/tr69agent/host-if -H 127.0.0.1 -p 56981 -g", stMsgData->paramName, "| grep -i Value | cut -d \":\" -f2");

    char result[50] = {'\0'};
    char buf[50] = {'\0'};
    static bool bCalled = false;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] CMD : [ %s ] \n",__FUNCTION__, cmd);

#ifdef YOCTO_BUILD
    fp = v_secure_popen("r", "/usr/local/tr69agent/host-if -H 127.0.0.1 -p 56981 -g %s | grep -i Value | cut -d \":\" -f2", stMsgData->paramName);
#else
    fp = popen(cmd,"r");
#endif

    if(fp == NULL)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF, "Error popen : Not able to read through host-if from TR69 agent.\n");
        return -1;
    }

    if(fgets(buf,50,fp) != NULL)
    {
        sscanf(buf,"%s", result);
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] result %s buf %s Reset. \n",__FUNCTION__, result, buf);

#ifdef YOCTO_BUILD
    v_secure_pclose(fp);
#else
    pclose(fp);
#endif
    memset(stMsgData->paramValue, 0, TR69HOSTIFMGR_MAX_PARAM_LEN);
    strncpy(stMsgData->paramValue, result, TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(result);

    return ret;
}

/**
 * @brief This method read the firmware information which is present in
 * "/opt/fwdnldstatus.txt"
 *
 * @param[in] param Firmware name string.
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from device.
 * @retval NOK if not able to fetch the data from device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::readFirmwareInfo(char *param, HOSTIF_MsgData_t * stMsgData)
{
    int ret = NOK;
    errno_t rc = -1;
    string line;
    ifstream fwDwnfile(FW_DWN_FILE_PATH);

    try {
        if (fwDwnfile.is_open()) {
            while ( getline (fwDwnfile,line) ) {
                if (string::npos != findIgnoreCase (line, param))
                    break;
            }
            fwDwnfile.close();
        } else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]Unable to open file\n", __FUNCTION__);
            return NOK;
        }

        char *cstr = new char [line.length()+1];
        if (NULL == cstr) {
            return NOK;
        }

        if (0 == line.length()) {
            delete[] cstr;
            return NOK;
        }

        rc=strcpy_s (cstr,(line.length()+1), line.c_str());
	if(rc!=EOK)
	{
		ERR_CHK(rc);
	}
        char * pch = NULL;
        pch = strstr (cstr,"|");
        if (pch == NULL) {
            delete[] cstr;
            return NOK;
        }
        pch++;
        while(isspace(*pch)) {
            pch++;
        }
        memset(stMsgData->paramValue, 0, TR69HOSTIFMGR_MAX_PARAM_LEN);
        strncpy(stMsgData->paramValue, pch, sizeof(stMsgData->paramValue) -1);  //CID:136525 - Buffer size warning
        stMsgData->paramValue[sizeof(stMsgData->paramValue) -1] = '\0';
        stMsgData->paramtype = hostIf_StringType;
        stMsgData->paramLen = strlen(pch);
        delete[] cstr;

    } catch (const std::exception e) {
        std::cout << __FUNCTION__ << "An exception occurred. " << e.what() << endl;
    }
    return (ret = OK);
}

/**
 * @brief This method writes the firmware information which is present in
 * "/opt/fwdnldstatus.txt"
 *
 * @param[in] param  Firmware name string.
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from device.
 * @retval NOK if not able to fetch the data from device.
 * @ingroup TR69_HOSTIF_DEVICEINFO_API
 */
int hostIf_DeviceInfo::writeFirmwareInfo(char *param, HOSTIF_MsgData_t * stMsgData)
{
    int ret = NOK;
    string line;
    ifstream infwDwnfile(FW_DWN_FILE_PATH);
    try {
        if (!infwDwnfile.is_open()) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s]Unable to open file %s\n", __FUNCTION__, FW_DWN_FILE_PATH);
            return NOK;
        }
        else
        {
            ofstream tmpfwDwnfile(TEMP_FW_DWN_FILE_PATH);
            while ( getline (infwDwnfile,line) )
            {
                if(line.find("Method") != string::npos) {
                    string tmpline = "Method|tr69";
                    tmpfwDwnfile << tmpline << "\n";
                }
                else if (line.find(param)!=string::npos) {
                    string tmpline = string(param);
                    tmpline = tmpline + "|" + stMsgData->paramValue;
                    tmpfwDwnfile << tmpline << "\n";;
                }
                else {
                    tmpfwDwnfile << line << "\n";;
                }
            }
            infwDwnfile.close();
            tmpfwDwnfile.close();
        }
           //CID:90007 , 82197-Checked return
        if((remove(FW_DWN_FILE_PATH) == 0) && (rename(TEMP_FW_DWN_FILE_PATH, FW_DWN_FILE_PATH) == 0))
        {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s] File removed from the path ,  Temp file is set \n", __FUNCTION__);
        }

    } catch (const std::exception e) {
        std::cout << __FUNCTION__ << "An exception occurred. " << e.what() << endl;
    }
    return (ret = OK);
}


int hostIf_DeviceInfo::set_xFirmwareDownloadNow(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOK;
    const char *userTriggerDwScr = "/lib/rdk/userInitiatedFWDnld.sh";

    LOG_ENTRY_EXIT;

    if(stMsgData->paramtype == hostIf_BooleanType)
    {
        bool xDwnldTrigger = get_boolean(stMsgData->paramValue);


        if(m_xFirmwareDownloadNow) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Failed to trigger Download, since download already in progress ....\n", __FUNCTION__, __LINE__);
            return ret;
        }
        else if (hostIf_DeviceInfo::m_xFirmwareDownloadNow == false && xDwnldTrigger) {
            if(xDwnldTrigger && (!m_xFirmwareDownloadProtocol.empty()) &&
                    (!m_xFirmwareDownloadURL.empty()) && (!m_xFirmwareToDownload.empty())) {

                char cmd[200] = {'\0'};
                sprintf(cmd, "%s %s %s %s %d %d &",userTriggerDwScr, m_xFirmwareDownloadProtocol.c_str(), m_xFirmwareDownloadURL.c_str(), m_xFirmwareToDownload.c_str(), m_xFirmwareDownloadUseCodebig, m_xFirmwareDownloadDeferReboot);

#ifdef YOCTO_BUILD
                ret = v_secure_system("%s %s %s %s %d %d &",userTriggerDwScr, m_xFirmwareDownloadProtocol.c_str(), m_xFirmwareDownloadURL.c_str(), m_xFirmwareToDownload.c_str(), m_xFirmwareDownloadUseCodebig, m_xFirmwareDownloadDeferReboot);
#else
                ret = system (cmd);
#endif

                if (ret != 0) {
                    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Failed to trigger Download, \'system (\"%s\")\' returned error code '%d'\n", __FUNCTION__, cmd, ret);
                    return NOK;
                }
                RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF, "[%s:%d] Yw.. Successfully executed (\'%s\')Triggered Download.\n",__FUNCTION__,__LINE__, cmd);

                /*Reset all cache parameter values for download trigger on successfully executed*/
                m_xFirmwareDownloadProtocol.clear();
                m_xFirmwareToDownload.clear();
                m_xFirmwareDownloadURL.clear();
                m_xFirmwareDownloadNow = false;
                m_xFirmwareDownloadUseCodebig = false;
                m_xFirmwareDownloadDeferReboot = false;
                ret = OK;
            }
            else {
                if(m_xFirmwareDownloadProtocol.empty())
                    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Failed to trigger Download, since \"FirmwareDownloadProtocols\" is not set/configured.\n", __FUNCTION__, __LINE__);
                if(m_xFirmwareDownloadURL.empty())
                    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Failed to trigger Download, since \"m_xFirmwareDownloadURL\" is not set/configured.\n", __FUNCTION__, __LINE__);
                if(m_xFirmwareToDownload.empty())
                    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Failed to trigger Download, since \"FirmwareToDownload\" is not set/configured. \n", __FUNCTION__, __LINE__);

                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Please configure other parameters, then trigger download. \n", __FUNCTION__, __LINE__);
                return ret;
            }
        }
        else {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Failed to trigger Download, since \"FirmwareToDownload\" is not set. \n", __FUNCTION__, __LINE__);
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Failed due to wrong data type for %s, please use boolean(0/1) to set.\n", __FUNCTION__, __LINE__, stMsgData->paramName);
    }

    return ret;
}

/**
 * @brief Finds if one string occurs within another string. The search is case-insensitive.
 *
 * @param[in] haystack the string within which the search is to be performed.
 * @param[in] needle   the string to be searched for.
 * @param[in] pos      the index within 'haystack' from where the search for 'needle' is to be started.
 *
 * @return the index of where the 'needle' is found in the 'haystack', or npos if 'needle' is not found.
 */
size_t hostIf_DeviceInfo::findIgnoreCase (std::string haystack, std::string needle, size_t pos)
{
    std::transform (haystack.begin(), haystack.end(), haystack.begin(), ::tolower); // convert haystack to lower case
    std::transform (needle.begin(), needle.end(), needle.begin(), ::tolower); // convert needle to lower case
    return haystack.find (needle, pos); // find and return the location of the needle hidden in the haystack
}

string hostIf_DeviceInfo::getStbMacIf_fr_devProperties()
{
    string stb_mac;
    try {
        char mac_buf[20] = {0};

        struct ifreq ifr;

        /* Get configured Estb Mac interface */
        char *stbMacIf = getenvOrDefault ((const char*)"DEFAULT_ESTB_INTERFACE", (char*)"");

        int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
        if(fd > 0) {
            strncpy(ifr.ifr_name, stbMacIf,IFNAMSIZ-1);
            if (0 == ioctl(fd, SIOCGIFHWADDR, &ifr)) {
                unsigned char *mac = NULL;
                mac = (unsigned char *) ifr.ifr_hwaddr.sa_data;
                if(mac) {
                    sprintf (mac_buf, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                    stb_mac = mac_buf;
                    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] STB MAC : \'%s\'..\n", __FUNCTION__, __LINE__, mac_buf );
                }
                else {
                    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Failed to get ifr_hwaddr.sa_data\'%s\'..\n", __FUNCTION__, __LINE__, strerror (errno) );
                }
            }
            else {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Failed in ioctl() with  \'%s\'..\n", __FUNCTION__, __LINE__, strerror (errno) );
            }
            close (fd);  //CID:89478 - NEGATIVE RETURNS
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Socket failed with  \'%s\'..\n", __FUNCTION__, __LINE__, strerror (errno) );
            close (fd);
        }
    } catch (const std::exception &e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s()]Exception caught %s\n", __FUNCTION__, e.what());
    }
    return stb_mac;
}

int hostIf_DeviceInfo::get_X_RDKCENTRAL_COM_LastRebootReason(HOSTIF_MsgData_t *stMsgData)
{

    int ret = NOK;
    FILE *fp = NULL;
    char buffer[1024] = {0};
    char *reboot_reason = NULL;

    LOG_ENTRY_EXIT;

    fp = fopen(PREVIOUS_REBOT_REASON_FILE,"r");
    if(!fp)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF," '%s' file not Present.\n", PREVIOUS_REBOT_REASON_FILE);
        return ret;
    }
    fread(buffer, 1024, 1, fp);
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Previous Reboot Info buffer: %s\n", buffer);
    fclose(fp);

    bool reason_str_found = false;
    char search[] = "reason";
    char *ptr = strstr(buffer, search);

    if (ptr != NULL)
    {
        reason_str_found = true;
    }
    else /* Substring not found */
    {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF," '%s' file doesn't contain '%s' string.\n", PREVIOUS_REBOT_REASON_FILE, search);
    }

    if(reason_str_found)
    {
        cJSON *root = cJSON_Parse(buffer);

        if(root) {
            reboot_reason = cJSON_GetObjectItem(root, "reason")->valuestring;

            if(reboot_reason) {
                ret = OK;
                memset(stMsgData->paramValue, 0, TR69HOSTIFMGR_MAX_PARAM_LEN);
                strncpy(stMsgData->paramValue, reboot_reason, sizeof(stMsgData->paramValue) -1);
                stMsgData->paramValue[sizeof(stMsgData->paramValue) -1] = '\0';
                stMsgData->paramtype = hostIf_StringType;
                stMsgData->paramLen = strlen(reboot_reason);
            }

            cJSON_Delete(root);
        }
        else {

        }

    }
    return ret;
}

int hostIf_DeviceInfo::get_xOpsRPC_Profile(HOSTIF_MsgData_t * stMsgData)
{
    int ret = NOK;

    if(strcmp(stMsgData->paramName, X_OPS_RPC_DEV_MANAGEABLE_NOTIFICATION) == 0)
    {
        ret = get_xOpsRPCDevManageableNotification(stMsgData);
    }
    else if(strcmp(stMsgData->paramName, X_OPS_RPC_FW_DNLD_STARTED_NOTIFICATION) == 0)
    {
        ret = get_xOpsRPCFwDwldStartedNotification(stMsgData);
    }
    else if(strcmp(stMsgData->paramName, X_OPS_RPC_FW_DNLD_COMPLETED_NOTIFICATION) == 0)
    {
        ret = get_xOpsRPCFwDwldCompletedNotification(stMsgData);
    }
    else if(strcmp(stMsgData->paramName, X_OPS_RPC_REBOOT_PENDING_NOTIFICATION) == 0)
    {
        ret = get_xOpsRPCRebootPendingNotification(stMsgData);
    }
    else
    {
        stMsgData->faultCode = fcInvalidParameterName;
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Invalid Parameter Name\n", __FUNCTION__, stMsgData->paramName);
    }
    return ret;
}

int hostIf_DeviceInfo::get_xOpsRPCDevManageableNotification(HOSTIF_MsgData_t* stMsgData)
{
    char *chVal = (char *)(!m_strXOpsDevManageableNotification.empty() ? m_strXOpsDevManageableNotification.c_str() : "0");
    errno_t rc = -1;
    rc=strcpy_s(stMsgData->paramValue,sizeof(stMsgData->paramValue), chVal);
    if(rc!=EOK)
    {
	ERR_CHK(rc);
    }
    return OK;
}

int hostIf_DeviceInfo::get_xOpsRPCFwDwldStartedNotification(HOSTIF_MsgData_t* stMsgData)
{
    char *chVal = (char *)((!m_strXOpsRPCFwDwldStartedNotification.empty()) ? m_strXOpsRPCFwDwldStartedNotification.c_str() : "0");
    errno_t rc = -1;
    rc=strcpy_s(stMsgData->paramValue,sizeof(stMsgData->paramValue), chVal);
    if(rc!=EOK)
    {
        ERR_CHK(rc);
    }
    return OK;
}

int hostIf_DeviceInfo::get_xOpsRPCFwDwldCompletedNotification(HOSTIF_MsgData_t* stMsgData)
{
    put_boolean(stMsgData->paramValue, m_bXOpsRPCFwDwldCompletedNotification);
    return OK;
}

int hostIf_DeviceInfo::get_xOpsRPCRebootPendingNotification(HOSTIF_MsgData_t* stMsgData)
{
    put_uint(stMsgData->paramValue, 0);
    return OK;
}

int hostIf_DeviceInfo::set_xOpsRPC_Profile(HOSTIF_MsgData_t * stMsgData)
{
    if(strcmp(stMsgData->paramName, X_OPS_RPC_REBOOTNOW) == 0)
    {
        set_xOpsDeviceMgmtRPCRebootNow (stMsgData);
    }
    else if(strcmp(stMsgData->paramName, X_OPS_RPC_DEV_MANAGEABLE_NOTIFICATION) == 0)
    {
        set_xOpsRPCDevManageableNotification(stMsgData);
    }
    else if(strcmp(stMsgData->paramName, X_OPS_RPC_FW_DNLD_STARTED_NOTIFICATION) == 0)
    {
        set_xOpsRPCFwDwldStartedNotification(stMsgData);
    }
    else if(strcmp(stMsgData->paramName, X_OPS_RPC_FW_DNLD_COMPLETED_NOTIFICATION) == 0)
    {
        set_xOpsRPCFwDwldCompletedNotification(stMsgData);
    }
    else if(strcmp(stMsgData->paramName, X_OPS_RPC_REBOOT_PENDING_NOTIFICATION) == 0)
    {
        set_xOpsRPCRebootPendingNotification(stMsgData);
    }
    else
    {
        stMsgData->faultCode = fcInvalidParameterName;
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Invalid Parameter Name\n", __FUNCTION__, stMsgData->paramName);
    }
    return OK;
}

int hostIf_DeviceInfo::set_xOpsDeviceMgmtRPCRebootNow (HOSTIF_MsgData_t * stMsgData)
{
    LOG_ENTRY_EXIT;

    if (get_boolean (stMsgData->paramValue))
    {
        char* command = (char *)"(sleep 1; /lib/rdk/rebootNow.sh -s hostifDeviceInfo) &";
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Invoking 'system (\"%s\")'. %s = true\n", __FUNCTION__, command, stMsgData->paramName);
        int ret = system (command);
        if (ret != 0)
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] 'system (\"%s\")' returned error code '%d'\n", __FUNCTION__, command, ret);
            return NOK;
        }
    }
    else
    {
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Not rebooting. %s = false\n", __FUNCTION__, stMsgData->paramName);
    }

    return OK;
}

int hostIf_DeviceInfo::set_xOpsRPCDevManageableNotification(HOSTIF_MsgData_t *stMsgData) {
    LOG_ENTRY_EXIT;

    errno_t rc = -1;
    m_strXOpsDevManageableNotification.clear();

    HOSTIF_MsgData_t stRfcData = {0};
    rc=strcpy_s(stRfcData.paramName,sizeof(stRfcData.paramName), X_RDK_RFC_MANGEBLENOTIFICATION_ENABLE);
    if(rc!=EOK)
    {
 	ERR_CHK(rc);
    }
    if((get_xRDKCentralComRFC(&stRfcData) == OK) && (strncmp(stRfcData.paramValue, "true", sizeof("true")) == 0))
    {
        m_strXOpsDevManageableNotification = stMsgData->paramValue;
        NotificationHandler::getInstance()->push_device_mgmt_notifications(NULL, NULL, NULL, (char* )"fully-manageable", stMsgData->paramValue);

        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Successfully set [\"%s\"] and Send notification as \"status\":\"fully-manageable\" \
    \"system-ready-time\":\"%s\" \n", __FUNCTION__, stMsgData->paramName, stMsgData->paramValue);
    }
    else {
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] RFC Parameter (%s) is disabled, so not sending notification for [%s]. \n",
                 __FUNCTION__, X_RDK_RFC_MANGEBLENOTIFICATION_ENABLE, X_OPS_RPC_DEV_MANAGEABLE_NOTIFICATION );
    }
    return OK;
}

int hostIf_DeviceInfo::set_xOpsRPCFwDwldStartedNotification(HOSTIF_MsgData_t *stMsgData) {
    LOG_ENTRY_EXIT;

    errno_t rc = -1;
    m_strXOpsRPCFwDwldStartedNotification.clear();
    /* Check for RFC */
    HOSTIF_MsgData_t stRfcData = {0};
    rc=strcpy_s(stRfcData.paramName,sizeof(stRfcData.paramName), X_RDK_RFC_MANGEBLENOTIFICATION_ENABLE);
    if(rc!=EOK)
    {
        ERR_CHK(rc);
    }
    if((get_xRDKCentralComRFC(&stRfcData) == OK) && (strncmp(stRfcData.paramValue, "true", sizeof("true")) == 0))
    {
        m_strXOpsRPCFwDwldStartedNotification = stMsgData->paramValue;
        NotificationHandler::getInstance()->push_device_mgmt_notifications(NULL, (char *)m_strXOpsRPCFwDwldStartedNotification.c_str(), NULL, (char* )"firmware-download-started", NULL);
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Successfully Set [\"%s\"] and Send notification as \"status\":\"firmware-download-started\" with \"start-time\":\"%s\" \n",
                 __FUNCTION__, stMsgData->paramName, stMsgData->paramValue);
    }
    else {
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] RFC Parameter (%s) is disabled, so not sending notification for [%s]. \n",
                 __FUNCTION__, X_RDK_RFC_MANGEBLENOTIFICATION_ENABLE, X_OPS_RPC_FW_DNLD_STARTED_NOTIFICATION );
    }
    return OK;
}

int hostIf_DeviceInfo::set_xOpsRPCFwDwldCompletedNotification(HOSTIF_MsgData_t *stMsgData) {
    LOG_ENTRY_EXIT;
    errno_t rc = -1;
    /* Check for RFC */
    HOSTIF_MsgData_t stRfcData = {0};
    rc=strcpy_s(stRfcData.paramName,sizeof(stRfcData.paramName), X_RDK_RFC_MANGEBLENOTIFICATION_ENABLE);
    if(rc!=EOK)
    {
        ERR_CHK(rc);
    }
    if((get_xRDKCentralComRFC(&stRfcData) == OK) && (strncmp(stRfcData.paramValue, "true", sizeof("true")) == 0))
    {
        m_bXOpsRPCFwDwldCompletedNotification = get_boolean(stMsgData->paramValue);
        const char *status = (m_bXOpsRPCFwDwldCompletedNotification)?"true":"false";
        char* start_time=  (char*) m_strXOpsRPCFwDwldStartedNotification.c_str();

        NotificationHandler::getInstance()->push_device_mgmt_notifications(NULL, start_time, (char*)status, (char* )"firmware-download-completed", NULL);
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Successfully Set [\"%s\"] and Send notification as \"status\":\"firmware-download-completed\" \
    		with \"start-time\":\"%s\" and \"download-status\":\"%s\". \n", __FUNCTION__, stMsgData->paramName, m_strXOpsRPCFwDwldStartedNotification.c_str(), status);
    }
    else {
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] RFC Parameter (%s) is disabled, so not sending notification for [%s]. \n",
                 __FUNCTION__, X_RDK_RFC_MANGEBLENOTIFICATION_ENABLE, X_OPS_RPC_FW_DNLD_COMPLETED_NOTIFICATION );
    }
    return OK;
}

int hostIf_DeviceInfo::set_xOpsRPCRebootPendingNotification(HOSTIF_MsgData_t *stMsgData) {
    LOG_ENTRY_EXIT;
    /* Check for RFC */
    errno_t rc = -1;
    HOSTIF_MsgData_t stRfcData = {0};
    rc=strcpy_s(stRfcData.paramName,sizeof(stRfcData.paramName), X_RDK_RFC_MANGEBLENOTIFICATION_ENABLE);
    if(rc!=EOK)
    {
        ERR_CHK(rc);
    }
    if((get_xRDKCentralComRFC(&stRfcData) == OK) && (strncmp(stRfcData.paramValue, "true", sizeof("true")) == 0))
    {
        unsigned int uinVal = get_uint(stMsgData->paramValue);
        char temp_buff[64] = {0};
        sprintf(temp_buff,"%d",uinVal);
        NotificationHandler::getInstance()->push_device_mgmt_notifications(temp_buff, NULL, NULL, (char* )"reboot-pending", NULL);

        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Successfully Set [\"%s\"] and Send \"reboot-pending\" notification  \
    		 \n", __FUNCTION__, stMsgData->paramName );
    }
    else {
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s] RFC Parameter (%s) is disabled, so not sending notification for [%s]. \n",
                 __FUNCTION__, X_RDK_RFC_MANGEBLENOTIFICATION_ENABLE, X_OPS_RPC_REBOOT_PENDING_NOTIFICATION );
    }
    return OK;
}


int hostIf_DeviceInfo::set_X_RDKCENTRAL_COM_LastRebootReason(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;
    RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"This '%s' file contains the LastRebootReason.\n", PREVIOUS_REBOT_REASON_FILE);
    return OK;
}

void hostIf_DeviceInfo::send_DeviceManageableNotification()
{
    LOG_ENTRY_EXIT;
    std::thread systemMgmtTimeMonitorThrd(hostIf_DeviceInfo::systemMgmtTimePathMonitorThr);
    systemMgmtTimeMonitorThrd.detach();
}

void hostIf_DeviceInfo::systemMgmtTimePathMonitorThr()
{
    unsigned long system_manageable_time = 0;

    sleep(10);
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s]Entering...\n", __FUNCTION__);
    /*This thread is to send the `DeviceManageableNotification` based on
     * Check for /tmp/webpa/start_time
     * 1. File exist for system manage up time, then send  */

    LOG_ENTRY_EXIT;

    int count = 0;
    bool is_webpa_ready = 0;
    do {
        bool is_webpa_ready = (access("/tmp/webpa/start_time", F_OK) == 0)?true:false;
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s] is_webpa_ready : %d\n", __FUNCTION__, is_webpa_ready);
        if(is_webpa_ready) {
            //system_manageable_time = get_system_manageble_time();
            system_manageable_time = get_device_manageble_time();
            HOSTIF_MsgData_t stMsgData = {0};
            sprintf(stMsgData.paramName, "%s", X_OPS_RPC_DEV_MANAGEABLE_NOTIFICATION);
            sprintf(stMsgData.paramValue, "%ld", system_manageable_time);

            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%d] Set [%s] to send the notification. \n",__FUNCTION__,__LINE__, X_OPS_RPC_DEV_MANAGEABLE_NOTIFICATION);
            hostIf_SetMsgHandler(&stMsgData);
            break;
        }
        else
        {
            sleep(60);
            count++;
        }

        if(count >= 5)
            break;
    } while(is_webpa_ready == false);

    if(is_webpa_ready == false)
    {
        int inotifyFd = 0, wd = 0;
        std::string m_path = "/tmp/webpa";
        inotifyFd = inotify_init();

        if (inotifyFd == -1)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]: Failed in inotify_init. Exiting thread \n",__FUNCTION__,__LINE__);
            return;
        }
        wd = inotify_add_watch(inotifyFd, m_path.c_str(), IN_CREATE | IN_ATTRIB);

        if (wd == -1)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]Unable to create Watch inotify file descriptor. Exiting thread \n",__FUNCTION__,__LINE__);
            return;
        }
        char buffer[sizeof(struct inotify_event) + NAME_MAX + 1] = {0};

        while (1)
        {
            ssize_t count = read(inotifyFd, buffer, sizeof(buffer));

            if (count < 0)
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Failed to read. Exiting \n",__FUNCTION__,__LINE__);
                return;
            }

            struct inotify_event * event = reinterpret_cast<struct inotify_event *>(buffer);
            if (event->mask & IN_ATTRIB)
            {
                if(!is_webpa_ready) {
                    if(0 == (strcmp(event->name, "start_time")))
                        is_webpa_ready = true;
                }


                if(is_webpa_ready) {
                    system_manageable_time = get_device_manageble_time();
                    HOSTIF_MsgData_t stMsgData = {0};
                    sprintf(stMsgData.paramName, "%s", X_OPS_RPC_DEV_MANAGEABLE_NOTIFICATION);
                    sprintf(stMsgData.paramValue, "%ld", system_manageable_time);
                    hostIf_SetMsgHandler(&stMsgData);
                    break;
                }
            }
        }
        inotify_rm_watch( inotifyFd, wd );
        close( inotifyFd );
    }
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s]Exiting...\n", __FUNCTION__);
}

int hostIf_DeviceInfo::get_X_RDKCENTRAL_COM_experience( HOSTIF_MsgData_t *stMsgData)
{
    string resp="";
    string experience = "";

    CURL *curl = curl_easy_init();

    if(NULL == curl)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s]Failed at curl_easy_init.\n", __FUNCTION__);
        return NOK;
    }

    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:50050/authService/getExperience");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCurlResponse);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);

        CURLcode res = curl_easy_perform(curl);

        long http_code = 0;

        if(res == CURLE_OK)
        {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] curl response : %d http response code: %ld\n", __FUNCTION__, res, http_code);
        }

        curl_easy_cleanup(curl);

        if(res == CURLE_OK && http_code == HTTP_OK )
        {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] curl response string = %s\n", __FUNCTION__, resp.c_str());

            cJSON* root = cJSON_Parse(resp.c_str());

            if(root)
            {
                cJSON* jsonObj    = cJSON_GetObjectItem(root, "experience");

                if(jsonObj->type == cJSON_String && jsonObj->valuestring && (strlen(jsonObj->valuestring) > 0))
                {
                    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s]The parameter [%s] value is [%s].\n", __FUNCTION__, stMsgData->paramName, jsonObj->valuestring);
                    experience = jsonObj->valuestring;
                }

                cJSON_Delete(root);
            }
            else
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] json parse error\n", __FUNCTION__);
                return NOK;
            }
        }
    }

    if(!experience.empty()) {
        strncpy(stMsgData->paramValue, experience.c_str(), TR69HOSTIFMGR_MAX_PARAM_LEN-1 );
    }
    else {
        return NOK;
    }

    return OK;
}

/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */


/** @} */
/** @} */

