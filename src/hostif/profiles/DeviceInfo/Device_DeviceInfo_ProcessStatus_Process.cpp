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
 * @file Device_DeviceInfo_ProcessStatus_Process.c
 *
 * @brief Device_DeviceInfo_ProcessStatus_Process. API Implementation.
 *
 * This is the implementation of the Device_DeviceInfo_ProcessStatus_Process. API.
 *
 * @par Document
 * TBD Relevant design or API documentation.
 *
 */

/** @addtogroup Device_DeviceInfo_ProcessStatus_Process. Implementation
 *  This is the implementation of the Device Public API.
 *  @{
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


#include<proc/readproc.h>
#include<sys/time.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "Device_DeviceInfo_ProcessStatus_Process.h"

GHashTable* hostIf_DeviceProcess::ifHash = NULL;

GMutex* hostIf_DeviceProcess::m_mutex = NULL;

GMutex* hostIf_DeviceProcess::m_libproc_lock = NULL;

hostIf_DeviceProcess::hostIf_DeviceProcess(int _dev_id)
{
    dev_id = _dev_id;
    bCalledProcessPid = false;
    bCalledProcessCommand = false;
    bCalledProcessSize = false;
    bCalledProcessPriority = false;
    bCalledProcessCpuTime = false;
    bCalledProcessState = false;

    backupProcessPid = 0;
    memset(backupProcessCommand,0,_COMMAND_LENGTH);
    backupProcessSize = 0;
    backupProcessPriority = 0;
    backupProcessCpuTime = 0;
    memset(backupProcessState,0,_STATE_LENGTH);
    hostIf_DeviceProcess::initProcpsLock();
}
GMutex* hostIf_DeviceProcess::initProcpsLock()
{
#if GLIB_VERSION_CUR_STABLE <= GLIB_VERSION_2_32
  if(!g_thread_supported())
  {
        g_thread_init(NULL);
  }
#endif
  if( NULL == m_libproc_lock )
      m_libproc_lock = g_mutex_new();
  return m_libproc_lock;
}

hostIf_DeviceProcess::~hostIf_DeviceProcess()
{
    if( NULL == m_libproc_lock )
      g_mutex_free(m_libproc_lock);
}

hostIf_DeviceProcess* hostIf_DeviceProcess::getInstance(int dev_id)
{
    hostIf_DeviceProcess* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_DeviceProcess *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_DeviceProcess(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create MoCA Interface instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_DeviceProcess::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_DeviceProcess::closeInstance(hostIf_DeviceProcess *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_DeviceProcess::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_DeviceProcess* pDev = (hostIf_DeviceProcess *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

void hostIf_DeviceProcess::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void hostIf_DeviceProcess::releaseLock()
{
    g_mutex_unlock(m_mutex);
}


static DeviceProcessStatusProcess processStatus = {0,{'\0'},0,0,0,{'\0'}};

/** Description: Counts the number of Processes in the system.
 *
 * \Return:  Count value or '0' if error
 *
 */
int hostIf_DeviceProcess::getNumOfProcessEntries(HOSTIF_MsgData_t *stMsgData)
{
    hostIf_DeviceProcess::initProcpsLock();
    g_mutex_lock(m_libproc_lock);
    int iTotalNumOfProcess = 0;

    PROCTAB *pProcTab = NULL;
    proc_t *task = NULL;

//#ifndef RDK_DEVICE_CISCO_XI4
//  pProcTab = openproc(PROC_FILLSTAT);
//#else
    pProcTab = openproc(PROC_FILLMEM);
//#endif

    if (pProcTab == NULL)
    {
        g_mutex_unlock(m_libproc_lock);
        return NOK;
    }

    while ((task = readproc(pProcTab, NULL)) != NULL)
    {
        iTotalNumOfProcess++;
        free(task);
    }
    closeproc(pProcTab);

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s: Current Process Count: [%u]\n", __FUNCTION__, iTotalNumOfProcess);

    put_int(stMsgData->paramValue,iTotalNumOfProcess);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    g_mutex_unlock(m_libproc_lock);
    return OK;
}

/**
 *    Description: This is a helper function to fill values for the Process Profile.
 *
 *    @param[in] ProcessInstanceNumber     Takes the instance number in the process profile table.
 *    @param[in] enum value        Takes the Enum value of the member in the process instance to be filled.
 *
 *    @param[out] value    returns OK
 **/
int getProcessFields(int iProcInstanceNum, EProcessMembers eProcessMem)
{
    PROCTAB *pProcTab = NULL;
    proc_t procTask;
    int iProcEntry = 0;

    if(NULL == (pProcTab = openproc(PROC_FILLSTAT | PROC_FILLMEM)))
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d] Failed in openproc(), returned NULL. \n",__FILE__,__FUNCTION__,__LINE__);

        return NOK;
    }

    for (iProcEntry = 0; iProcEntry < iProcInstanceNum; iProcEntry++)
    {
        memset(&procTask, 0, sizeof(procTask));
        //traverse the list and get the pid.
        if(!pProcTab->finder(pProcTab,&procTask))
        {
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"ProcessInstance: %d : No Entry Found In Process Profile Table\n",iProcInstanceNum);
            memset(&processStatus, 0, sizeof(processStatus));
            closeproc(pProcTab);
            return OK;
        }
    }

    // For the given pid get the process info.
    pProcTab->reader(pProcTab,&procTask);

    switch(eProcessMem)
    {
    case eProcessPid:
        processStatus.uiPid = (unsigned int)procTask.tid;
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"ProcessInstance: %d Pid: %d\n",iProcInstanceNum, procTask.tid);
        break;
    case eProcessCommand:
        memset(processStatus.cCommand,'\0',_COMMAND_LENGTH);
        memcpy(processStatus.cCommand,procTask.cmd,sizeof(procTask.cmd));
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"ProcessInstance: %d Command: %s\n",iProcInstanceNum, procTask.cmd);
        break;
    case eProcessState:
        memset(processStatus.cState,'\0',_STATE_LENGTH);
        switch(procTask.state)
        {
        case 'R':
            memcpy(processStatus.cState,PROCESS_STATE_RUNNING,sizeof(PROCESS_STATE_RUNNING));
            break;
        case 'S':
            memcpy(processStatus.cState,PROCESS_STATE_SLEEPING,sizeof(PROCESS_STATE_SLEEPING));
            break;
        case 'T':
            memcpy(processStatus.cState,PROCESS_STATE_STOPPED,sizeof(PROCESS_STATE_STOPPED));
            break;
        case 'Z':
            memcpy(processStatus.cState,PROCESS_STATE_ZOMBIE,sizeof(PROCESS_STATE_ZOMBIE));
            break;
        default:
            break;
        }
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"ProcessInstance: %d State: %c\n",iProcInstanceNum, procTask.state);
        break;
    case eProcessPriority:
        processStatus.uiPriority = (unsigned int)procTask.priority;
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"ProcessInstance: %d Priority: %5ld\n",iProcInstanceNum, procTask.priority);
        break;
    case eProcessSize:
        processStatus.uiSize = (unsigned int)(procTask.size*4);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"ProcessInstance: %d Size: %5ld\n",iProcInstanceNum, procTask.size*4);
        break;
    case eProcessCpuTime:
        processStatus.uiCpuTime = (unsigned int)(procTask.utime+procTask.stime);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"ProcessInstance: %d CPU Time: %5lld\n",iProcInstanceNum, procTask.utime+procTask.stime);
        break;
    default:
        break;
    }

    closeproc(pProcTab);

    return OK;
}

/**
 *     Description: Takes in the request and extracts the instance number of the process requested.
 *    @param[in] name     ACS request path.
 *    @param[in] enum value     Takes the Enum value of the member in the process instance to be filled.
 *    @param[out] value    retuns OK/NOK
 **/
static int
readProcessFields(const char *name,EProcessMembers eProcessMem)
{
    int iProcessInstance = 0;
    const char *pSetting;
    if(!matchComponent(name,"Device.DeviceInfo.ProcessStatus.Process",&pSetting,iProcessInstance))
    {
        return NOK;
    }
    if(OK != getProcessFields(iProcessInstance,eProcessMem))
    {
        return NOK;
    }

    return OK;
}


/****************************************************************************************************************************************************/
// Device_DeviceInfo_ProcessStatus_Process. Profile. Getters:
/****************************************************************************************************************************************************/
int hostIf_DeviceProcess::get_Device_DeviceInfo_ProcessStatus_Process_PID(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    g_mutex_lock(m_libproc_lock);
    if(OK != readProcessFields(stMsgData->paramName,eProcessPid))
    {
        g_mutex_unlock(m_libproc_lock);
        return NOK;
    }
    if(bCalledProcessPid && pChanged && (backupProcessPid != processStatus.uiPid))
    {
        *pChanged =  true;
    }
    bCalledProcessPid = true;
    backupProcessPid = processStatus.uiPid;

    put_int(stMsgData->paramValue,processStatus.uiPid);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    g_mutex_unlock(m_libproc_lock);
    return OK;
}

int hostIf_DeviceProcess::get_Device_DeviceInfo_ProcessStatus_Process_Command(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    g_mutex_lock(m_libproc_lock);
    if(OK != readProcessFields(stMsgData->paramName,eProcessCommand))
    {
        g_mutex_unlock(m_libproc_lock);
        return NOK;
    }
    if(bCalledProcessCommand && pChanged && strcpy(backupProcessCommand,processStatus.cCommand))
    {
        *pChanged =  true;
    }
    bCalledProcessCommand = true;
    strcpy(backupProcessCommand,processStatus.cCommand);

    strcpy(stMsgData->paramValue,processStatus.cCommand);
    g_mutex_unlock(m_libproc_lock);
    return OK;
}
int hostIf_DeviceProcess::get_Device_DeviceInfo_ProcessStatus_Process_Size(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    g_mutex_lock(m_libproc_lock);
    if(OK != readProcessFields(stMsgData->paramName,eProcessSize))
    {
        g_mutex_unlock(m_libproc_lock);
        return NOK;
    }
    if(bCalledProcessSize && pChanged && (backupProcessSize != processStatus.uiSize))
    {
        *pChanged =  true;
    }
    bCalledProcessSize = true;
    backupProcessSize = processStatus.uiSize;

    put_int(stMsgData->paramValue,processStatus.uiSize);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    g_mutex_unlock(m_libproc_lock);
    return OK;
}
int hostIf_DeviceProcess::get_Device_DeviceInfo_ProcessStatus_Process_Priority(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    g_mutex_lock(m_libproc_lock);
    if(OK != readProcessFields(stMsgData->paramName,eProcessPriority))
    {
        g_mutex_unlock(m_libproc_lock);
        return NOK;
    }
    if(bCalledProcessPriority && pChanged && (backupProcessPriority != processStatus.uiPriority))
    {
        *pChanged =  true;
    }
    bCalledProcessPriority = true;
    backupProcessPriority = processStatus.uiPriority;

    put_int(stMsgData->paramValue,processStatus.uiPriority);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    g_mutex_unlock(m_libproc_lock);
    return OK;
}
int hostIf_DeviceProcess::get_Device_DeviceInfo_ProcessStatus_Process_CPUTime(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    g_mutex_lock(m_libproc_lock);
    if(OK != readProcessFields(stMsgData->paramName,eProcessCpuTime))
    {
       g_mutex_unlock(m_libproc_lock);
       return NOK;
    }
    if(bCalledProcessCpuTime && pChanged && (backupProcessCpuTime != processStatus.uiCpuTime))
    {
        *pChanged =  true;
    }
    bCalledProcessCpuTime = true;
    backupProcessCpuTime = processStatus.uiCpuTime;

    put_int(stMsgData->paramValue,processStatus.uiCpuTime);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    g_mutex_unlock(m_libproc_lock);
    return OK;
}
int hostIf_DeviceProcess::get_Device_DeviceInfo_ProcessStatus_Process_State(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    g_mutex_lock(m_libproc_lock);
    if(OK != readProcessFields(stMsgData->paramName,eProcessState))
    {
        g_mutex_unlock(m_libproc_lock);
        return NOK;
    }
    if(bCalledProcessState && pChanged && strcpy(backupProcessState,processStatus.cState))
    {
        *pChanged =  true;
    }
    bCalledProcessState = true;
    strcpy(backupProcessState,processStatus.cState);

    strcpy(stMsgData->paramValue,processStatus.cState);
    g_mutex_unlock(m_libproc_lock);
    return OK;
}
/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */


/** @} */
/** @} */
