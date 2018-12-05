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
 * @file Device_DeviceInfo_ProcessStatus_Process.h
 *
 * @brief Device.DeviceInfo.ProcessStatus.Process API.
 *
 * Description of XXX module.
 *
 *
 * @par Document
 * Document reference.
 *
 *
 * @par Open Issues (in no particular order)
 * -# Issue 1
 * -# Issue 2
 *
 *
 * @par Assumptions
 * -# Assumption
 * -# Assumption
 *
 *
 * @par Abbreviations
 * - ACK:     Acknowledge.
 * - BE:      Big-Endian.
 * - cb:      Callback function (suffix).
 * - config:  Configuration.
 * - desc:    Descriptor.
 * - dword:   Double word quantity, i.e., four bytes or 32 bits in size.
 * - intfc:   Interface.
 * - LE:      Little-Endian.
 * - LS:      Least Significant.
 * - MBZ:     Must be zero.
 * - MS:      Most Significant.
 * - _t:      Type (suffix).
 * - word:    Two byte quantity, i.e. 16 bits in size.
 * - xfer:    Transfer.
 *
 *
 * @par Implementation Notes
 * -# Note
 * -# Note
 *
 */




/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef DEVICE_DEVICEINFO_PROCESSSTATUS_PROCESS_H_
#define DEVICE_DEVICEINFO_PROCESSSTATUS_PROCESS_H_

#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"

#define _COMMAND_LENGTH 256
#define _STATE_LENGTH 64
#define _LENGTH_PARAMETER 64
#define PROCESS_STATE_RUNNING "Running"
#define PROCESS_STATE_SLEEPING "Sleeping"
#define PROCESS_STATE_STOPPED "Stopped"
#define PROCESS_STATE_ZOMBIE "Zombie"

    typedef struct Device_DeviceInfo_ProcessStatus_Process
    {
        unsigned int uiPid;                    /*Process Identifier.*/
        char cCommand[_COMMAND_LENGTH];        /*The name of the command that has caused the process to exist.*/
        unsigned int uiSize;                     /* The size in kilobytes of the memory occupied by the process.*/
        unsigned int uiPriority;             /*The priority of the process. */
        unsigned int uiCpuTime;                /*The amount of time in milliseconds that the process has spent taking up CPU time since the process was started. */
        char cState[_STATE_LENGTH];            /* The current state that the process is in.*/
    } DeviceProcessStatusProcess;

    typedef enum EProcessMembers
    {
        eProcessPid = 0,
        eProcessCommand,
        eProcessSize,
        eProcessPriority,
        eProcessCpuTime,
        eProcessState
    } EProcessMembers;

/**
 *   Description. This returns the total number of processes running.
 *
 *   @param[in] void Takes no parameter.
 *   @param[out] value Total number of processes running.
 **/

class hostIf_DeviceProcess
{
    
    static  GHashTable  *ifHash;

    static GMutex *m_mutex;

    static GMutex *m_libproc_lock;
    
    int dev_id;

    hostIf_DeviceProcess(int _dev_id);

    ~hostIf_DeviceProcess(); 
        
    bool bCalledProcessPid;
    bool bCalledProcessCommand;
    bool bCalledProcessSize;
    bool bCalledProcessPriority;
    bool bCalledProcessCpuTime;
    bool bCalledProcessState;

        
    unsigned int backupProcessPid;              
    char backupProcessCommand[_COMMAND_LENGTH];
    unsigned int backupProcessSize;               
    unsigned int backupProcessPriority;         
    unsigned int backupProcessCpuTime;             
    char backupProcessState[_STATE_LENGTH];        
    
public:
    
    static hostIf_DeviceProcess *getInstance(int dev_id);

    static void closeInstance(hostIf_DeviceProcess *);

    static GList* getAllInstances();

    static void closeAllInstances();

    static void getLock();

    static void releaseLock();

    static GMutex* initProcpsLock();

    static int getNumOfProcessEntries(HOSTIF_MsgData_t *); 

    /**
     * Description. This is the getter api for Device_DeviceInfo_ProcessStatus_Process. for
     *  Device_DeviceInfo_ProcessStatus_Process. object Profile.
     *
     * @param[in]  name  Complete path name of the parameter.
     * @param[in]  type  It is a user data type of ParameterType.
     * @param[out] value It is the value of the parameter.
     *
     * @retval OK if successful.
     * @retval XXX_ERR_BADPARAM if a bad parameter was supplied.
     *
     * @execution Synchronous.
     * @sideeffect None.
     *
     * @note This function must not suspend and must not invoke any blocking system
     * calls. It should probably just send a message to a driver event handler task.
     *
     * @see XXX_SomeOtherFunction.
     */


    /**
     * @brief get_Device_DeviceInfo_ProcessStatus_Process_PID.
     *
     * This function provides the Process Identifier.
     *
     * @return The Process Identifier.
     *
     * @retval OK if is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_ProcessStatus_Process_PID(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_ProcessStatus_Process_Command.
     *
     * This function provides The name of the command that has caused the process to exist.
     *
     * @return The name of the command.
     *
     * @retval OK if is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_ProcessStatus_Process_Command(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_ProcessStatus_Process_Size.
     *
     * This function provides The Size in Killo bytes of the memory occupied by process.
     *
     * @return The size of memory occupied by process .
     *
     * @retval OK if is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_ProcessStatus_Process_Size(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_ProcessStatus_Process_Priority.
     *
     * This function provides The priority of the process.
     *
     * @return The priority of the process.
     *
     * @retval OK if is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_ProcessStatus_Process_Priority(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_ProcessStatus_Process_CPUTime.
     *
     * This function provides The amount of time spent by the process taking the cpu.
     *
     * @return The amount of time spent by the process taking the cpu.
     *
     * @retval OK if is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_ProcessStatus_Process_CPUTime(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_ProcessStatus_Process_State.
     *
     * This function provides The current state of the process.
     *
     * @return The current state of the process.
     *
     * @retval OK if is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_ProcessStatus_Process_State(HOSTIF_MsgData_t *,bool *pChanged = NULL);

};
/* End of doxygen group */
/**
 * @}
 */

#endif /* DEVICE_DEVICEINFO_PROCESSSTATUS_H_ */


/** @} */
/** @} */
