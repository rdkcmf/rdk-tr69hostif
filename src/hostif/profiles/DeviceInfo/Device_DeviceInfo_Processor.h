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
 * @file Device_DeviceInfo_Processor.h
 *
 * @brief Device_DeviceInfo_Processor API.
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


#ifndef DEVICE_DEVICEINFO_PROCESSSOR_H_
#define DEVICE_DEVICEINFO_PROCESSSOR_H_

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/

#include <sys/utsname.h>
#include <string.h>
#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"

#define SYS_STRING_LEN  256
#define BUF_LEN         1024

class hostIf_DeviceProcessorInterface
{
    hostIf_DeviceProcessorInterface(int dev_id);
    ~hostIf_DeviceProcessorInterface() {};
    static GHashTable *ifHash;
    static GMutex *m_mutex;
    int dev_id;

    bool bCalledArchitecture;

    char backupArchitecture[SYS_STRING_LEN];

public:

    static hostIf_DeviceProcessorInterface *getInstance(int dev_id);

    static void closeInstance(hostIf_DeviceProcessorInterface *);

    static GList* getAllInstances();

    static void closeAllInstances();

    static void getLock();

    static void releaseLock();


    static unsigned int getNumOfProcessorEntries(void);

    /**
     * Description. This is the getter api for Device_DeviceInfo_Processor. for
     *  Device_DeviceInfo_Processor. object Profile.
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
     * @brief get_Device_DeviceInfo_Processor_Architecture.
     *
     * This function provides the architecture of the processor on the underlying hardware
     *
     * @return Processor hardware architecture.
     *
     * @retval OK if is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_Processor_Architecture(HOSTIF_MsgData_t *,bool *pChanged = NULL);

};
/* End of doxygen group */
/**
 * @}
 */

#endif /* DEVICE_DEVICEINFO_PROCESSSOR_H_ */


/** @} */
/** @} */
