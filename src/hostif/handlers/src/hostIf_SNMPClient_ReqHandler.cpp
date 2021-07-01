/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2017 RDK Management
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
 * @file hostIf_SNMPClient_ReqHandler.cpp
 * @brief The header file provides HostIf SNMP client request handler information APIs.
 */

#include "hostIf_SNMPClient_ReqHandler.h"
#include "snmpAdapter.h"
#include "safec_lib.h"

SNMPClientReqHandler* SNMPClientReqHandler::pInstance = NULL;
updateCallback SNMPClientReqHandler::mUpdateCallback = NULL;

msgHandler* SNMPClientReqHandler::getInstance()
{
    if(!pInstance)
        pInstance = new SNMPClientReqHandler();

    return pInstance;
}


/**
 * @brief This function is used to initialize all the current process and processor
 * to '0' using memset.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if initialization is successful.
 * @retval false if initialization is not successful.
 * @ingroup TR-181 HOSTIF_SNMPCLIENT_REQHANDLER_CLASSES
 */
bool SNMPClientReqHandler::init()
{
    hostIf_snmpAdapter::init();
    return true;
}

/**
 * @brief This function is used to close all the instances of SNMP.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if it successfully close all the instances.
 * @retval false if not able to close all the instances.
 * @ingroup TR-181 HOSTIF_SNMPCLIENT_REQHANDLER_CLASSES
 */
bool SNMPClientReqHandler::unInit()
{
    hostIf_snmpAdapter::unInit();
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] SNMP manager DeInitializing\n", __FUNCTION__);
    return true;
}

/**
 * @brief This function is used to set the SNMP set.
 *
 * @param[out] stMsgData TR-181 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successful.
 * @retval ERR_INTERNAL_ERROR if not able to set the SNMP command.
 */
int SNMPClientReqHandler::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *pSetting;
    int instanceNumber;
    hostIf_snmpAdapter::getLock();

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);

    if(strncasecmp(stMsgData->paramName,"Device.X_RDKCENTRAL-COM_DocsIf",strlen("Device.X_RDKCENTRAL-COM_DocsIf"))==0)
    {
        hostIf_snmpAdapter *pIfaceStatus = hostIf_snmpAdapter::getInstance(instanceNumber);
        stMsgData->instanceNum = instanceNumber;
        if(pIfaceStatus)
        {
            if(strcasecmp(stMsgData->paramName,"Device.X_RDKCENTRAL-COM_DocsIf.docsIfCmStatusTxPower")==0)
                ret = pIfaceStatus->set_ValueToSNMPAdapter(stMsgData);
        }
        else
        {
            ret = NOK;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] hostIf_snmpAdapter::getInstance is NULL for %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
        }
    }
    else
    {
        ret = NOK;
        stMsgData->faultCode = fcAttemptToSetaNonWritableParameter;
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed, since Attempt To Set a NonWritable Parameter \"%s\"\n", __FUNCTION__, stMsgData->paramName);
    }
    hostIf_snmpAdapter::releaseLock();
    return ret;
}

/**
 * @brief This function is used to set the SNMP get.
 *
 * @param[out] stMsgData TR-181 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is gets the data successfully.
 * @retval ERR_INTERNAL_ERROR if not able to get the data from the device.
 * @ingroup TR-181 HOSTIF_DEVICECLIENT_REQHANDLER_CLASSES
 */
int SNMPClientReqHandler::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *pSetting;
    int instanceNumber = 0;
    hostIf_snmpAdapter::getLock();

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);

    if(strncasecmp(stMsgData->paramName,"Device.X_RDKCENTRAL-COM_DocsIf",strlen("Device.X_RDKCENTRAL-COM_DocsIf"))==0)
    {
        hostIf_snmpAdapter *pIfaceStatus = hostIf_snmpAdapter::getInstance(instanceNumber);
        stMsgData->instanceNum = instanceNumber;

        if(pIfaceStatus)
            ret = pIfaceStatus->get_ValueFromSNMPAdapter(stMsgData);
        else
        {
            ret = NOK;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] hostIf_snmpAdapter::getInstance is NULL for %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
        }
    }
    else
    {
        stMsgData->faultCode = fcInvalidParameterName;
    }
    hostIf_snmpAdapter::releaseLock();
    return ret;
}

int SNMPClientReqHandler::handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    int instanceNumber = 0;

    hostIf_snmpAdapter::getLock();
    stMsgData->instanceNum = instanceNumber;
    hostIf_snmpAdapter *pIface = hostIf_snmpAdapter::getInstance(instanceNumber);
    if(pIface)
    {
        GHashTable* notifyhash = pIface->getNotifyHash();
        if(notifyhash != NULL)
        {
            int* notifyvalue = (int*) g_hash_table_lookup(notifyhash,stMsgData->paramName);
            put_int(stMsgData->paramValue, *notifyvalue);
            stMsgData->paramtype = hostIf_IntegerType;
            ret = OK;
        }
        else
        {
            ret = NOK;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] Not able to get notifyhash  %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
        }
    }
    else
    {
        ret = NOK;
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] hostIf_snmpAdapter::getInstance is NULL for %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    }

    hostIf_snmpAdapter::releaseLock();
    return ret;
}

int SNMPClientReqHandler::handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    int instanceNumber = 0;
    const char *pSetting;
    hostIf_snmpAdapter::getLock();
    hostIf_snmpAdapter *pIface = hostIf_snmpAdapter::getInstance(instanceNumber);

    stMsgData->instanceNum = instanceNumber;
    if(!pIface)
    {
        hostIf_snmpAdapter::releaseLock();
        return NOK;
    }
    GHashTable* notifyhash = pIface->getNotifyHash();
    if(notifyhash != NULL)
    {
        int *notifyValuePtr;
        notifyValuePtr = (int*) malloc(1 * sizeof(int));

        // Inserting Notification parameter to Notify Hash Table,
        // Note that neither keys nor values are copied when inserted into the GHashTable, so they must exist for the lifetime of the GHashTable
        // There for allocating a memory for both Param name and param value. This should be freed whenever we disable Notification.
        char *notifyKey;
        notifyKey = (char*) malloc(sizeof(char)*strlen(stMsgData->paramName)+1);
        if(NULL != notifyValuePtr)
        {
            *notifyValuePtr = 1;
	    errno_t rc = -1;
            rc=strcpy_s(notifyKey,strlen(stMsgData->paramName)+1,stMsgData->paramName);
	    if(rc!=EOK)
    	    {
	   	ERR_CHK(rc);
    	    }
            g_hash_table_insert(notifyhash,notifyKey,notifyValuePtr);
            ret = OK;
        }
        else
        {
            ret = NOK;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] Not able to allocate Notify pointer %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
        }
    }
    else
    {
        ret = NOK;
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] Not able to get notifyhash  %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    }
    hostIf_snmpAdapter::releaseLock();
    return ret;
}

void SNMPClientReqHandler::registerUpdateCallback(updateCallback cb)
{
    mUpdateCallback = cb;
}

/** @} */
/** @} */
