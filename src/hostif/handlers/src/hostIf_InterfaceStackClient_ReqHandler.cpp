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
 /*
 * @brief Provides implementation for DHCPv4 Client Request Handler Interface
 *
 * @author vejuturu@cisco.com
 */

/**
 * @file hostIf_InterfaceStackClient_ReqHandler.cpp
 * @brief The header file provides HostIf InterfaceStack client request handler information APIs.
 */
//#define HAVE_VALUE_CHANGE_EVENT 
#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "Device_InterfaceStack.h"
#include "hostIf_InterfaceStackClient_ReqHandler.h"

InterfaceStackClientReqHandler* InterfaceStackClientReqHandler::pInstance = NULL;

updateCallback InterfaceStackClientReqHandler::mUpdateCallback = NULL;
int InterfaceStackClientReqHandler::curNumOfEntriesInInterfaceStack = 0;

msgHandler* InterfaceStackClientReqHandler::getInstance()
{
    hostif_InterfaceStack::getLock();
    if(!pInstance)
        pInstance = new InterfaceStackClientReqHandler();
    hostif_InterfaceStack::releaseLock();
    return pInstance;
}

void InterfaceStackClientReqHandler::reset()
{
    hostif_InterfaceStack::getLock();
    curNumOfEntriesInInterfaceStack = 0;
    hostif_InterfaceStack::releaseLock();
}

/**
 * @brief This function is used to initialize. Currently not implemented.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if initialization is successful.
 * @retval false if initialization is not successful.
 * @ingroup TR-069HOSTIF_INTERFACESTACKCLIENT_REQHANDLER_CLASSES
 */
bool InterfaceStackClientReqHandler::init()
{
    return true;
}

/**
 * @brief This function is used to close all the instances of  interface stack.
 *
 * @return Returns the status of the operation.
 *
 * @retval true if it successfully close all the instances.
 * @retval false if not able to close all the instances.
 * @ingroup TR-069HOSTIF_INTERFACESTACKCLIENT_REQHANDLER_CLASSES
 */
bool InterfaceStackClientReqHandler::unInit()
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s()] Interface Stack manager DeInitializing\n", __FUNCTION__);
    hostif_InterfaceStack::closeAllInstances();

    return true;
}


/**
 * @brief This function is used to handle the set message request of InterfaceStack Client.
 * Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if successful.
 * @retval ERR_INTERNAL_ERROR if not able to set the data to the device.
 * @ingroup TR-069HOSTIF_INTERFACESTACKCLIENT_REQHANDLER_CLASSES
 */
int InterfaceStackClientReqHandler::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Found string as %s. Set command not supported.\n",
             __FUNCTION__, __FILE__, stMsgData->paramName);

    return ret;
}

/**
 * @brief This function is used to handle the get message request of InterfaceStack Client.
 * Gets the total number of active interface stack entries or get the attributes of
 * interface stack such as "HigherLayer" and "LowerLayer".
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if successful.
 * @retval ERR_INTERNAL_ERROR if not able to set the data to the device.
 * @ingroup TR-069HOSTIF_INTERFACESTACKCLIENT_REQHANDLER_CLASSES
 */
int InterfaceStackClientReqHandler::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *pSetting;
    int instanceNumber = 0;
    hostif_InterfaceStack::getLock();

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s:%d] Found string as %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    if(strcasecmp(stMsgData->paramName, "Device.InterfaceStackNumberOfEntries") == 0)
    {
        ret = hostif_InterfaceStack::get_Device_InterfaceStackNumberOfEntries(stMsgData);
    }
    else if(matchComponent(stMsgData->paramName,"Device.InterfaceStack",&pSetting,instanceNumber))
    {
        hostif_InterfaceStack *pInterfaceStack = hostif_InterfaceStack::getInstance(instanceNumber);
        stMsgData->instanceNum = instanceNumber;
        if(pInterfaceStack)
        {
            if(strcasecmp(pSetting,"HigherLayer") == 0)
            {
                ret = pInterfaceStack->get_Device_InterfaceStack_HigherLayer(stMsgData);
            }
            else if(strcasecmp(pSetting,"LowerLayer") == 0)
            {
                ret = pInterfaceStack->get_Device_InterfaceStack_LowerLayer(stMsgData);
            }
        }
        else
        {
            ret = NOK;
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%s:%d] hostif_InterfaceStack::getInstance() returned NULL for instance %d",
                    __FUNCTION__, __FILE__, __LINE__, instanceNumber);
        }
    }

    hostif_InterfaceStack::releaseLock();
    return ret;
}
int InterfaceStackClientReqHandler::handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    int instanceNumber = 0;

    hostif_InterfaceStack::getLock();
    hostif_InterfaceStack *pIface = hostif_InterfaceStack::getInstance(instanceNumber);
    stMsgData->instanceNum = instanceNumber;
    if(!pIface)
    {
        hostif_InterfaceStack::releaseLock();
        return NOK;
    }

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
    }
    hostif_InterfaceStack::releaseLock();
        return ret;
}

int InterfaceStackClientReqHandler::handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    int instanceNumber = 0;

    hostif_InterfaceStack::getLock();
    hostif_InterfaceStack *pIface = hostif_InterfaceStack::getInstance(instanceNumber);
    stMsgData->instanceNum = instanceNumber;
    if(!pIface)
    {
        hostif_InterfaceStack::releaseLock();
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
            strcpy(notifyKey,stMsgData->paramName);
            g_hash_table_insert(notifyhash,notifyKey,notifyValuePtr);
            ret = OK;
        }
        else
        {
            ret = NOK;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d]  Not able to allocate Notify pointer %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
        }
    }
    else
    {
        ret = NOK;
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d]  Not able to get notifyhash  %s\n", __FUNCTION__, __FILE__, __LINE__, stMsgData->paramName);
    }
    hostif_InterfaceStack::releaseLock();
    return ret;
}

void InterfaceStackClientReqHandler::registerUpdateCallback(updateCallback cb)
{
    mUpdateCallback = cb;
}

void InterfaceStackClientReqHandler::checkForUpdates()
{
    HOSTIF_MsgData_t msgData;
    bool bChanged;
    GList *elem;
    int index = 1;
    int instanceNumber;
    GHashTable* notifyhash = NULL;
    const char *pSetting;

    char tmp_buff[TR69HOSTIFMGR_MAX_PARAM_LEN];
    hostif_InterfaceStack::getLock();

    memset(&msgData,0,sizeof(msgData));
    memset(tmp_buff,0,TR69HOSTIFMGR_MAX_PARAM_LEN);
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FILE__, __FUNCTION__);


    if(hostif_InterfaceStack::get_Device_InterfaceStackNumberOfEntries(&msgData) == OK)
    {
        int tmpNoDev = get_int(msgData.paramValue);
        char tmp[TR69HOSTIFMGR_MAX_PARAM_LEN] = "";
        snprintf(tmp_buff, sizeof(tmp_buff), "Device.InterfaceStack");
        while(curNumOfEntriesInInterfaceStack > tmpNoDev)
        {
            snprintf(tmp, sizeof(tmp), "%s.%d.", tmp_buff, tmpNoDev);
            if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_REMOVE, tmp, NULL, hostIf_IntegerType);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] InterfaceStack Removed Event..\n", __FILE__, __FUNCTION__);

            tmpNoDev++;
        }
        while(curNumOfEntriesInInterfaceStack < tmpNoDev)
        {
            snprintf(tmp, sizeof(tmp), "%s.", tmp_buff);
            if(mUpdateCallback) mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_ADD,tmp, NULL, hostIf_IntegerType);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] InterfaceStack Added Event..\n", __FILE__, __FUNCTION__);

            tmpNoDev--;
        }
        curNumOfEntriesInInterfaceStack = get_int(msgData.paramValue);
    }

#ifdef HAVE_VALUE_CHANGE_EVENT

    instanceNumber = 0;
    hostif_InterfaceStack *pInterfaceStackClient = hostif_InterfaceStack::getInstance(instanceNumber);
    if(NULL != pInterfaceStackClient)
    {
        notifyhash = pInterfaceStackClient->getNotifyHash();
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Unable to get hostif_InterfaceStack Instance\n", __FUNCTION__, __FILE__);
    }
    // Iterate through Ghash Table
    if(NULL != notifyhash)
    { 
        GHashTableIter notifyHashIterator;
        gpointer paramName;
        gpointer notifyEnable;
        bool  bChanged;

        g_hash_table_iter_init (&notifyHashIterator, notifyhash);
        while (g_hash_table_iter_next (&notifyHashIterator, &paramName, &notifyEnable))
        {
            int* isNotifyEnabled = (int *)notifyEnable;
            instanceNumber = 0;
            if(matchComponent((const char*)paramName,"Device.InterfaceStack.",&pSetting,instanceNumber))
            {
                hostif_InterfaceStack *pIface = hostif_InterfaceStack::getInstance(instanceNumber);
                if(pIface)
                {
                    if (strcasecmp(pSetting,"HigherLayer") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_InterfaceStack_HigherLayer(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                    if (strcasecmp(pSetting,"LowerLayer") == 0)
                    {
                        memset(&msgData,0,sizeof(msgData));
                        bChanged =  false;
                        pIface->get_Device_InterfaceStack_LowerLayer(&msgData,&bChanged);
                        if(bChanged)
                        {
                            if(mUpdateCallback && (*isNotifyEnabled == 1))
                            {
                                mUpdateCallback(IARM_BUS_TR69HOSTIFMGR_EVENT_VALUECHANGED,(const char*)paramName, msgData.paramValue, msgData.paramtype);
                            }
                        }
                    }
                }
            }
        }
    }
#endif
    hostif_InterfaceStack::releaseLock();
}
