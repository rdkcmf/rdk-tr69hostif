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
 * @file Device_DeviceInfo_ProcessStatus.h
 *
 * @brief Device.DeviceInfo.ProcessStatus API.
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


#ifndef DEVICE_DEVICEINFO_PROCESSSTATUS_H_
#define DEVICE_DEVICEINFO_PROCESSSTATUS_H_

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/
#include "hostIf_main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "hostIf_updateHandler.h"

#define PARAM_LEN 256

class hostIf_DeviceProcessStatusInterface
{
    hostIf_DeviceProcessStatusInterface(int dev_id);
    ~hostIf_DeviceProcessStatusInterface() {};
    static GHashTable *ifHash;
    static GMutex *m_mutex;

    int dev_id;

    bool bCalledCPUUsage;
    bool bCalledProcessNumberOfEntries;

    int backupCPUUsage;
    int backupProcessNumberOfEntries;

    unsigned int getNumOfProcessEntries();
    int getProcessStatusCPUUsage();

public:

    static hostIf_DeviceProcessStatusInterface *getInstance(int dev_id);

    static void closeInstance(hostIf_DeviceProcessStatusInterface *);

    static GList *getAllInstances();

    static void closeAllInstances();

    static void getLock();

    static void releaseLock();

    /**
     * Description. This is the getter api for Device_DeviceInfo_ProcessStatus. for
     *  Device_DeviceInfo_ProcessStatus. object Profile.
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
     * @brief get_Device_DeviceInfo_ProcessStatus_CPUUsage.
     *
     * This function provides the total amount of cpu usage, in percentage.
     *
     * @return The total amount of cpu usage in percentage.
     *
     * @retval OK if is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */

    int get_Device_DeviceInfo_ProcessStatus_CPUUsage(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
    /**
     * @brief get_Device_DeviceInfo_ProcessStatus_ProcessNumberOfEntries.
     *
     * This function provides The number of entries in the process table.
     *
     * @return The number of entries in the process table.
     *
     * @retval OK if is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */

};
/* End of doxygen group */
/**
 * @}
 */

#endif /* DEVICE_DEVICEINFO_PROCESSSTATUS_H_ */


/** @} */
/** @} */
