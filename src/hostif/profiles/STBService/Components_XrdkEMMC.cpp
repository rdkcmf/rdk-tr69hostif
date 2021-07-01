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
 * @file Components_XrdkEMMC.cpp
 * @brief This source file contains API implementation to handle data model parameters for the eMMC component.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/

#ifdef USE_XRDK_EMMC_PROFILE

#include "rdk_debug.h"
#include "hostIf_main.h"
#include "Components_XrdkEMMC.h"

#include <exception>
#include <map>

#ifdef USE_RDK_STORAGE_MANAGER_V2
#include "rdkStorageMgr.h"
#else
#include "storageMgr.h"
#endif // ifdef USE_RDK_STORAGE_MANAGER_V2
#include "safec_lib.h"

hostIf_STBServiceXeMMC *hostIf_STBServiceXeMMC::instance = NULL;

hostIf_STBServiceXeMMC* hostIf_STBServiceXeMMC::getInstance()
{
    errno_t safec_rc = -1;
    if (!instance)
    {
        char emmcDeviceID[RDK_STMGR_MAX_STRING_LENGTH] = "";
        char emmcPartitionID[RDK_STMGR_MAX_STRING_LENGTH] = "";

        eSTMGRDeviceInfoList deviceInfoList;
        eSTMGRReturns stRet = rdkStorage_getDeviceInfoList (&deviceInfoList);
        for (int i = 0; i < deviceInfoList.m_numOfDevices; i++)
        {
            if (RDK_STMGR_DEVICE_TYPE_EMMCCARD == deviceInfoList.m_devices[i].m_type)
            {
                safec_rc=memcpy_s (emmcDeviceID, RDK_STMGR_MAX_STRING_LENGTH , &deviceInfoList.m_devices[i].m_deviceID,RDK_STMGR_MAX_STRING_LENGTH);
                if(safec_rc!=EOK)
	        {
		    ERR_CHK(safec_rc);
	        }
		safec_rc=memcpy_s (emmcPartitionID, RDK_STMGR_MAX_STRING_LENGTH , &deviceInfoList.m_devices[i].m_partitions,RDK_STMGR_MAX_STRING_LENGTH);
                if(safec_rc!=EOK)
                {
                    ERR_CHK(safec_rc);
                }
		break;
            }
        }

        try
        {
            if (*emmcDeviceID)
            {
                instance = new hostIf_STBServiceXeMMC(emmcDeviceID, emmcPartitionID);
            }
            else
            {
                RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Failed: eMMC not found\n",__FUNCTION__);
            }
        }
        catch(const std::exception& e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Caught exception \" %s\"\n", __FUNCTION__, e.what());
        }
    }
    return instance;
}

void hostIf_STBServiceXeMMC::closeInstance(hostIf_STBServiceXeMMC *pDev)
{
    if(pDev)
    {
        delete pDev;
    }
}

hostIf_STBServiceXeMMC::hostIf_STBServiceXeMMC(char* emmcDeviceID, char* emmcPartitionID)
{
    snprintf (this->emmcDeviceID, sizeof(this->emmcDeviceID), "%s", emmcDeviceID);
    snprintf (this->emmcPartitionID, sizeof(this->emmcPartitionID), "%s", emmcPartitionID);
}

int hostIf_STBServiceXeMMC::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    return ret;
}

int hostIf_STBServiceXeMMC::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    typedef int (hostIf_STBServiceXeMMC::*getter_function) (HOSTIF_MsgData_t *);
    typedef std::pair<const char*, getter_function> getter_function_entry;
    #define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

    static getter_function_entry getter_function_map[] = {
            {"Capacity", &hostIf_STBServiceXeMMC::getCapacity},
            {"LifeElapsedA", &hostIf_STBServiceXeMMC::getLifeElapsedA},
            {"LifeElapsedB", &hostIf_STBServiceXeMMC::getLifeElapsedB},
            {"LotID", &hostIf_STBServiceXeMMC::getLotID},
            {"Manufacturer", &hostIf_STBServiceXeMMC::getManufacturer},
            {"Model", &hostIf_STBServiceXeMMC::getModel},
            {"ReadOnly", &hostIf_STBServiceXeMMC::getReadOnly},
            {"SerialNumber", &hostIf_STBServiceXeMMC::getSerialNumber},
            {"TSBQualified", &hostIf_STBServiceXeMMC::getTSBQualified},
            {"PreEOLStateSystem", &hostIf_STBServiceXeMMC::getPreEOLStateSystem},
            {"PreEOLStateEUDA", &hostIf_STBServiceXeMMC::getPreEOLStateEUDA},
            {"PreEOLStateMLC", &hostIf_STBServiceXeMMC::getPreEOLStateMLC},
            {"FirmwareVersion", &hostIf_STBServiceXeMMC::getFirmwareVersion},
            {"DeviceReport", &hostIf_STBServiceXeMMC::getDeviceReport} };

    int str_len = strlen(X_EMMC_OBJ);
    if ((strncasecmp(stMsgData->paramName, X_EMMC_OBJ, str_len) != 0))
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s]Failed : Mismatch parameter path : %s\n", __FILE__, __FUNCTION__, stMsgData->paramName);
        return NOK;
    }

    const char *paramName = strchr(stMsgData->paramName + str_len - 1, '.');
    if (paramName == NULL)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Parameter is NULL  \n", __FILE__, __FUNCTION__);
        return NOK;
    }
    paramName++;

    try
    {
        for (const auto& entry : getter_function_map)
            if (strcasecmp (paramName, entry.first) == 0)
                return CALL_MEMBER_FN(*this, entry.second)(stMsgData);
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s:%s] Parameter \'%s\' is Not Supported  \n", __FILE__, __FUNCTION__, paramName);
    }
    catch (const std::exception& e)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Exception caught %s   \n", __FILE__, __FUNCTION__, e.what());
    }
    return NOK;
}

int hostIf_STBServiceXeMMC::getCapacity(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    stMsgData->paramtype = hostIf_UnsignedIntType;

    eSTMGRDeviceInfo deviceInfo;
    if (RDK_STMGR_RETURN_SUCCESS == rdkStorage_getDeviceInfo (emmcDeviceID, &deviceInfo))
    {
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s:%s] deviceInfo.m_capacity = %llu\n",
                __FILE__, __FUNCTION__, deviceInfo.m_capacity);

        put_int(stMsgData->paramValue, (int) (deviceInfo.m_capacity / 1024 / 1024)); // returned capacity is in bytes, convert to MB
        return OK;
    }
    else
    {
        put_int(stMsgData->paramValue, 0);
        return NOK;
    }
}

int hostIf_STBServiceXeMMC::getLifeElapsedA(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    return getLifeElapsed(stMsgData, "LifeElapsedA");
}

int hostIf_STBServiceXeMMC::getLifeElapsedB(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    return getLifeElapsed(stMsgData, "LifeElapsedB");
}

int hostIf_STBServiceXeMMC::getLifeElapsed(HOSTIF_MsgData_t *stMsgData, const char* life_elapsed_type)
{
    LOG_ENTRY_EXIT;

    stMsgData->paramtype = hostIf_IntegerType;

    eSTMGRHealthInfo healthInfo;
    if (RDK_STMGR_RETURN_SUCCESS == rdkStorage_getHealth (emmcDeviceID, &healthInfo))
    {
        for (int i = 0; i < healthInfo.m_lifetimesList.m_numOfAttributes; i++)
        {
            if (0 == strcmp (healthInfo.m_lifetimesList.m_diagnostics[i].m_name, life_elapsed_type))
            {
                put_int(stMsgData->paramValue, atoi (healthInfo.m_lifetimesList.m_diagnostics[i].m_value));
                return OK;
            }
        }
    }

    put_int(stMsgData->paramValue, -1); // Error
    return NOK;
}

int hostIf_STBServiceXeMMC::getLotID(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    stMsgData->paramtype = hostIf_StringType;

    errno_t rc = -1;
    eSTMGRDeviceInfo deviceInfo;
    if (RDK_STMGR_RETURN_SUCCESS == rdkStorage_getDeviceInfo (emmcDeviceID, &deviceInfo))
    {
        rc=strcpy_s((char*) stMsgData->paramValue, sizeof(stMsgData->paramValue),deviceInfo.m_hwVersion);
	if(rc!=EOK)
    	{
	    ERR_CHK(rc);
    	}
        return OK;
    }
    else
    {
        stMsgData->paramValue[0]='\0';
        return NOK;
    }
}

int hostIf_STBServiceXeMMC::getManufacturer(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    stMsgData->paramtype = hostIf_StringType;

    errno_t rc = -1;
    eSTMGRDeviceInfo deviceInfo;
    if (RDK_STMGR_RETURN_SUCCESS == rdkStorage_getDeviceInfo (emmcDeviceID, &deviceInfo))
    {
        rc=strcpy_s((char*) stMsgData->paramValue,sizeof(stMsgData->paramValue), deviceInfo.m_manufacturer);
	if(rc!=EOK)
        {
            ERR_CHK(rc);
        }
        return OK;
    }
    else
    {
        stMsgData->paramValue[0]='\0';
        return NOK;
    }
}

int hostIf_STBServiceXeMMC::getModel(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    stMsgData->paramtype = hostIf_StringType;

    errno_t rc = -1;
    eSTMGRDeviceInfo deviceInfo;
    if (RDK_STMGR_RETURN_SUCCESS == rdkStorage_getDeviceInfo (emmcDeviceID, &deviceInfo))
    {
        rc=strcpy_s((char*) stMsgData->paramValue,sizeof(stMsgData->paramValue), deviceInfo.m_model);
	if(rc!=EOK)
        {
            ERR_CHK(rc);
        }
        return OK;
    }
    else
    {
        stMsgData->paramValue[0]='\0';
        return NOK;
    }
}

int hostIf_STBServiceXeMMC::getReadOnly(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    stMsgData->paramtype = hostIf_BooleanType;

    eSTMGRDeviceInfo deviceInfo;
    if (RDK_STMGR_RETURN_SUCCESS == rdkStorage_getDeviceInfo (emmcDeviceID, &deviceInfo))
    {
        put_boolean(stMsgData->paramValue, (deviceInfo.m_status == RDK_STMGR_DEVICE_STATUS_READ_ONLY));
        return OK;
    }
    else
    {
        put_boolean(stMsgData->paramValue, true);
        return NOK;
    }
}

int hostIf_STBServiceXeMMC::getSerialNumber(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    stMsgData->paramtype = hostIf_StringType;

    errno_t rc = -1;
    eSTMGRDeviceInfo deviceInfo;
    if (RDK_STMGR_RETURN_SUCCESS == rdkStorage_getDeviceInfo (emmcDeviceID, &deviceInfo))
    {
        rc=strcpy_s((char*) stMsgData->paramValue,sizeof(stMsgData->paramValue), deviceInfo.m_serialNumber);
	if(rc!=EOK)
    	{
	    ERR_CHK(rc);
    	}
        return OK;
    }
    else
    {
        stMsgData->paramValue[0]='\0';
        return NOK;
    }
}

// TODO: is a "TSBQualified" data model parameter even needed for an eMMC (which cannot be pulled out of the device) ?
int hostIf_STBServiceXeMMC::getTSBQualified(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    stMsgData->paramtype = hostIf_BooleanType;

    eSTMGRDeviceInfo deviceInfo;
    if (RDK_STMGR_RETURN_SUCCESS == rdkStorage_getDeviceInfo (emmcDeviceID, &deviceInfo))
    {
        put_boolean(stMsgData->paramValue, (deviceInfo.m_status != RDK_STMGR_DEVICE_STATUS_NOT_QUALIFIED));
        return OK;
    }
    else
    {
        put_boolean(stMsgData->paramValue, true); // assume eMMC (a non-user-removable part) is Comcast-approved and thus TSB-qualified
        return OK;
    }
}

int hostIf_STBServiceXeMMC::getPreEOLStateSystem(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    return getPreEOLState(stMsgData, "PreEOLStateSystem");
}

int hostIf_STBServiceXeMMC::getPreEOLStateEUDA(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    return getPreEOLState(stMsgData, "PreEOLStateEUDA");
}

int hostIf_STBServiceXeMMC::getPreEOLStateMLC(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    return getPreEOLState(stMsgData, "PreEOLStateMLC");
}

int hostIf_STBServiceXeMMC::getPreEOLState(HOSTIF_MsgData_t *stMsgData, const char* pre_eol_state_type)
{
    LOG_ENTRY_EXIT;

    stMsgData->paramtype = hostIf_StringType;

    eSTMGRHealthInfo healthInfo;
    if (RDK_STMGR_RETURN_SUCCESS == rdkStorage_getHealth (emmcDeviceID, &healthInfo))
    {
        for (int i = 0; i < healthInfo.m_healthStatesList.m_numOfAttributes; i++)
        {
            if (0 == strcmp (healthInfo.m_healthStatesList.m_diagnostics[i].m_name, pre_eol_state_type))
            {
                snprintf (stMsgData->paramValue, sizeof (stMsgData->paramValue), healthInfo.m_healthStatesList.m_diagnostics[i].m_value);
                return OK;
            }
        }
    }

    snprintf (stMsgData->paramValue, sizeof (stMsgData->paramValue), "Error");
    return NOK;
}

int hostIf_STBServiceXeMMC::getFirmwareVersion(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    stMsgData->paramtype = hostIf_StringType;

    errno_t rc = -1;
    eSTMGRDeviceInfo deviceInfo;
    if (RDK_STMGR_RETURN_SUCCESS == rdkStorage_getDeviceInfo (emmcDeviceID, &deviceInfo))
    {
        rc=strcpy_s((char*) stMsgData->paramValue,sizeof(stMsgData->paramValue), deviceInfo.m_firmwareVersion);
	if(rc!=EOK)
    	{
	    ERR_CHK(rc);
    	}
        return OK;
    }
    else
    {
        stMsgData->paramValue[0]='\0';
        return NOK;
    }
}

int hostIf_STBServiceXeMMC::getDeviceReport(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    stMsgData->paramtype = hostIf_StringType;

    eSTMGRHealthInfo healthInfo;
    if (RDK_STMGR_RETURN_SUCCESS == rdkStorage_getHealth (emmcDeviceID, &healthInfo))
    {
        snprintf(stMsgData->paramValue, sizeof(stMsgData->paramValue), healthInfo.m_diagnostics.m_blob);
//        memcpy(stMsgData->paramValue, healthInfo.m_diagnostics.m_blob, sizeof (healthInfo.m_diagnostics.m_blob));
//        stMsgData->paramValue[sizeof (healthInfo.m_diagnostics.m_blob)] = '\0';
//        stMsgData->paramLen = sizeof (healthInfo.m_diagnostics.m_blob);
        return OK;
    }

    stMsgData->paramValue[0] = '\0';
//    stMsgData->paramLen = 0;
    return NOK;
}

#endif // ifdef USE_XRDK_EMMC_PROFILE

/** @} */
/** @} */
