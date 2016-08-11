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
 * @file Device_DeviceInfo_Processor.cpp
 * @brief This source file contains the APIs for getting device processor information.
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


#include "Device_DeviceInfo_Processor.h"

GHashTable* hostIf_DeviceProcessorInterface::ifHash = NULL;
GMutex* hostIf_DeviceProcessorInterface::m_mutex = NULL;

hostIf_DeviceProcessorInterface* hostIf_DeviceProcessorInterface::getInstance(int dev_id)
{
    hostIf_DeviceProcessorInterface* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_DeviceProcessorInterface *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_DeviceProcessorInterface(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create Ethernet Interface instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_DeviceProcessorInterface::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_DeviceProcessorInterface::closeInstance(hostIf_DeviceProcessorInterface *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_DeviceProcessorInterface::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_DeviceProcessorInterface* pDev = (hostIf_DeviceProcessorInterface *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

void hostIf_DeviceProcessorInterface::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void hostIf_DeviceProcessorInterface::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

/**
 * @brief Class Constructor of the class hostIf_DeviceProcessorInterface.
 *
 * It will initialize the device id.
 *
 * @param[in] devid Device identification Number.
 */
hostIf_DeviceProcessorInterface::hostIf_DeviceProcessorInterface(int devid)
{
    dev_id = devid;
    bCalledArchitecture = false;
    strcpy(backupArchitecture,"");
}



unsigned int hostIf_DeviceProcessorInterface::getNumOfProcessorEntries(void) {

    FILE *fp = NULL;
    char resultBuff[BUF_LEN] = {'\0'};
    char cmd[BUF_LEN] = "/bin/cat /proc/cpuinfo | /bin/grep processor | /usr/bin/wc -l";
    int noOfProcessorEntries = 0;

    fp = popen(cmd,"r");

    if(fp == NULL) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Error popen\n", __FUNCTION__);
        return 0;
    }

    if(fgets(resultBuff, BUF_LEN,fp)!=NULL) {
        sscanf(resultBuff,"%d",&noOfProcessorEntries);
    }

    pclose(fp);

    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"%s(): Processors Count: [%d]\n", __FUNCTION__, noOfProcessorEntries);

    return noOfProcessorEntries;
}

/**
 * @brief This function use to get the architecture of the processor on the underlying hardware.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from device.
 * @retval NOK if not able to fetch the data from device.
 * @ingroup TR69_HOSTIF_DEVICE_PROCESSOR_API
 */
int hostIf_DeviceProcessorInterface::get_Device_DeviceInfo_Processor_Architecture(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    struct utsname  utsName;
    uname(&utsName);

    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"Get Architecture value: '%s'\n", utsName.machine);
    strncpy(stMsgData->paramValue, utsName.machine, strlen(utsName.machine));
    if(pChanged && bCalledArchitecture && strncpy(stMsgData->paramValue,backupArchitecture,strlen(stMsgData->paramValue)))
    {
        *pChanged = true;
    }

    bCalledArchitecture = true;
    strncpy(backupArchitecture,stMsgData->paramValue,strlen(stMsgData->paramValue));

    return OK;
}
/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */


/** @} */
/** @} */
