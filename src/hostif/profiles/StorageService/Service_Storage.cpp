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
 * @file Service_Storage.cpp
 * @brief This source file contains the APIs of TR069 storage service.
 */
#include <string.h>
#include "Service_Storage_PhyMedium.h"
#include "Service_Storage.h"

#define MAX_CMD_LEN 128
#define MAX_BUF_LEN 256
#define MAX_IP_LEN 16
#define MAX_DEV_STR 6

GHashTable *hostIf_StorageSrvc::storageSrvHash = NULL;
GMutex *hostIf_StorageSrvc::m_mutex = NULL;

/* Constructor for hostIf_StorageSrvc*/

/**
 * @brief Class Constructor of the class hostIf_StorageSrvc.
 *
 * It will initialize the device id.
 *
 * @param[in] dev_id Identification number of the device.
 */
hostIf_StorageSrvc::hostIf_StorageSrvc(int dev_id):dev_id(dev_id) {
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Inside constructor for dev_id:%d\n", dev_id);
}

void hostIf_StorageSrvc::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void hostIf_StorageSrvc::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

hostIf_StorageSrvc* hostIf_StorageSrvc::getInstance(int dev_id) 
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"Entering [%s]\n", __FUNCTION__);
    hostIf_StorageSrvc* pRet = NULL;
    int maxInstance =  0;
    HOSTIF_MsgData_t stMsgData;
    if(NULL==storageSrvHash)
    {
        get_Device_StorageSrvc_ClientNumberOfEntries(&stMsgData);
    }
    pRet = (hostIf_StorageSrvc *)g_hash_table_lookup(storageSrvHash, (gpointer) dev_id);
    return pRet;
}


GList* hostIf_StorageSrvc::getAllInstances()
{
    if(storageSrvHash)
        return g_hash_table_get_keys(storageSrvHash);
    return NULL;
}


void hostIf_StorageSrvc::closeInstance(hostIf_StorageSrvc *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(storageSrvHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}


void hostIf_StorageSrvc::closeAllInstances()
{
    if(storageSrvHash)
    {
        GList* val_list = g_hash_table_get_values (storageSrvHash);
        GList* tmp_list = val_list;
        while(tmp_list)
        {
           hostIf_StorageSrvc* pDev = (hostIf_StorageSrvc *)tmp_list->data;
           tmp_list = tmp_list->next;
           closeInstance(pDev);
        }
        g_list_free(val_list);
    }
}


unsigned int hostIf_StorageSrvc :: get_Device_StorageSrvc_ClientNumberOfEntries(HOSTIF_MsgData_t* stMsgData)
{
    //Currently we have only one type of Storage service. The one which is within the box. So Number of entries is one.
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"Entring :%s\n",__FUNCTION__);
    int numberOfEntries = 1;
    put_int(stMsgData->paramValue, numberOfEntries);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);
    hostIf_StorageSrvc* pRet = NULL;
    if(NULL != storageSrvHash)
    {
        closeAllInstances();
    }
    else
    {
        storageSrvHash = g_hash_table_new(NULL, NULL);
    }
    for(int index=1;index<=numberOfEntries; index++)
    {
        try 
        {
            pRet = new hostIf_StorageSrvc(index);
            g_hash_table_insert(storageSrvHash, (gpointer)(index), pRet);
        } 
        catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create physical medium instance..\n");
        }
    }
    return OK;
}

/**
 * @brief This function get the hostIf storage service interface such as total number
 * physical medium entries, attributes of physical medium "Name", "SmartCapable" and
 * "Health".
 *
 * @param[in] stMsgData  HostIf Message Request param contains the storage service attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hostIf storage service interface attribute.
 * @retval -1 If Not able to get the hostIf storage service interface attribute.
 * @retval -2 If Not handle the hostIf storage service interface attribute.
 * @ingroup TR69_HOSTIF_STORAGE_SERVICE_API
 */
int hostIf_StorageSrvc :: handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"Entring:%s\n",__FUNCTION__);
    int retVal = NOK;
    string param;
    const char* psettings;
    const char* pSubSettings;
    int instanceNumber = 0;
    char dev_str[MAX_DEV_STR]={'\0'};
    /* Incoming message will be of Device.Service.StorageService.1.PhysicalMedium.1.X
     * We would have parsed till Device.Service.StorageService.1, Now we have to find 
     * the handler for the remaining part and call the functon accordingly
     */
    hostIf_PhysicalMedium *pPhyMed ;
    param.assign("Device.Services.StorageService");
    snprintf(dev_str, MAX_DEV_STR, ".%d",dev_id);
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Param Name:%s param:%s\n",stMsgData->paramName,param.c_str());
    if(matchComponent(stMsgData->paramName, param.c_str(), &psettings, instanceNumber))
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"psettings:%s\n",psettings);
        if(strcasecmp(psettings,"PhysicalMediumNumberOfEntries")==0)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Getting the number of physical medium number of entries\n");
            retVal = hostIf_PhysicalMedium::get_Device_Service_StorageMedium_ClientNumberOfEntries(stMsgData, dev_id);
        }
        else if(strncasecmp(psettings,"PhysicalMedium.",strlen("PhysicalMedium."))==0)
        {
            param.append(dev_str);
            param.append(".PhysicalMedium");
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Getting the param value for Physical Medium:%s from param name:%s\n",param.c_str(), stMsgData->paramName);
            if(matchComponent(stMsgData->paramName, param.c_str(), &pSubSettings, instanceNumber))
            {
                F("Sub param:%s\n",pSubSettings);
                pPhyMed = hostIf_PhysicalMedium :: getInstance(dev_id,instanceNumber);
                if(NULL!= pPhyMed )
                {
                    if(0 == strcasecmp(pSubSettings, "Name"))
                    {
                        retVal = pPhyMed->get_Device_Service_StorageMedium_Name(stMsgData);
                    }
                    else if(0 == strcasecmp(pSubSettings, "SmartCapable"))
                    {
                        retVal = pPhyMed->get_Device_Service_StorageMedium_SMARTCapable(stMsgData);
                    }
                    else if(0 == strcasecmp(pSubSettings, "Health"))
                    {
                        retVal = pPhyMed->get_Device_Service_StorageMedium_Health(stMsgData);
                    }
                    else
                    {
                        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
                        stMsgData->faultCode = fcInvalidParameterName;
                        ret = NOK;
                    }
                }
                
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed to match component\n");
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
                stMsgData->faultCode = fcInvalidParameterName;
                ret = NOK;
            }
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s not implemented\n",stMsgData->paramName);
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Parameter : \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
            stMsgData->faultCode = fcInvalidParameterName;
            ret = NOK;
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Not able to get component:%s %s %d",stMsgData->paramName, __FUNCTION__, __LINE__);
        stMsgData->faultCode = fcInvalidParameterName;
        ret = NOK;
   }
    return retVal;
}
