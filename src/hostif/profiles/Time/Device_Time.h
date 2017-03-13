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
 * @file Device_Time.h
 *
 * @brief TR-069 Device.Time object Public API.
 *
 * Description of Time module.
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


#ifndef DEVICE_TIME_H_
#define DEVICE_TIME_H_

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/
#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "hostIf_updateHandler.h"

/** @defgroup TR_069_DEVICE_TIME_API TR-069 Device.Time object API.
 *  @ingroup TR_069_API
 *
 *  The Device.Time object contains parameters relating an NTP or SNTP time client in
 *  the CPE.
 *
 */

/** @addtogroup TR_069_DEVICE_TIME_GETTER_API TR-069 Device.Time Getter API.
 *  @ingroup TR_069_DEVICE_TIME_API
 *
 *  \section dev_time_getter TR-069 Device.Time Getter API
 *
 *  This is the getter group of API for the <b>Device.Time</b> object.
 *
 *  The interface for all functions is identical and is described here.
 *
 *  @param[in]   *name     This is the complete path name of the parameter extracted from
 *                         soap message, e.g. ::get_Device_Time_Enable.
 *                         In this case, the path is "Device.Time.Enable".
 *  @param[in]   *type     Data type of parameter defined for TR-069. This is same as the
 *                         data type used in the Xi3 data-model.xml file.
 *                         (see parameter.h)
 *  @param[out]  *value    This is the value of the parameter requested by the ACS.
 *                         (see paramaccess.h)
 *
 *  @return The status of the operation.
 *  @retval OK            If parameter requested was successfully fetched. (Same as <b>NO_ERROR</b>).
 *  @retval NO_ERROR      If parameter requested was successfully fetched. (Same as <b>OK</b>).
 *  @retval DIAG_ERROR    Diagnostic error.
 *  @retval ERR_???       Appropriate error value otherwise (see dimark_globals.h).
 *
 *  @todo Clarify description of DIAG_ERROR.
 *
 *  @{
 */

/**
 * @brief    Get the status of the time client.
 *
 * This function provides the status (enabled or disabled) of the NTP or SNTP time
 * client.
 *
 * See @ref dev_time_getter
 *
 */

class hostIf_Time {

    static  GHashTable  *ifHash;

    static GMutex *m_mutex;

    static  GHashTable  *m_notifyHash;

    int dev_id;

    bool bCalledLocalTimeZone;
    bool bCalledCurrentLocalTime;
    char backupLocalTimeZone[TR69HOSTIFMGR_MAX_PARAM_LEN];
    char backupCurrentLocalTime[TR69HOSTIFMGR_MAX_PARAM_LEN];
    hostIf_Time(int dev_id);

    ~hostIf_Time();
public:

    static hostIf_Time* getInstance(int dev_id);

    static void closeInstance(hostIf_Time*);

    static GList* getAllInstances();

    static void closeAllInstances();

    static void getLock();

    static void releaseLock();

    static GHashTable* getNotifyHash();


    int get_Device_Time_Enable(HOSTIF_MsgData_t *, bool *pChanged = false);

    /**
     * @brief    Get the status of time support on the CPE.
     *
     * This function provides the status of time support on the CPE. This is an
     * enumeration of:
     *
     * <tt>
     *     <ul><li>Disabled</li>
     *         <li>Unsynchronized</li>
     *         <li>Synchronized</li>
     *         <li>Error_FailedToSynchronize</li>
     *         <li>Error (OPTIONAL)</li></ul>
     * </tt>
     *
     * @note     <ul>
     *               <li>The <tt>Unsynchronized</tt> value indicates that the CPE's absolute time
     *               has not yet been set.</li>
     *
     *               <li>The <tt>Synchronized</tt> value indicates that the CPE has acquired
     *               accurate absolute time; its current time is accurate.</li>
     *
     *               <li>The <tt>Error_FailedToSynchronize</tt> value indicates that the CPE
     *               failed to acquire accurate absolute time; its current time is not
     *               accurate.</li>
     *
     *               <li>The Error value MAY be used by the CPE to indicate a locally
     *               defined error condition.</li>
     *           </ul>
     *
     * See @ref dev_time_getter
     *
     */

    int get_Device_Time_Status(HOSTIF_MsgData_t *, bool *pChanged = false);

    /**
     * @brief    Get the first NTP timeserver.
     *
     * This function provides the address of the first NTP timeserver. This can be either
     * a host name or IP address.
     *
     * See @ref dev_time_getter
     *
     */

    int get_Device_Time_NTPServer1(HOSTIF_MsgData_t *, bool *pChanged = false);

    /**
     * @brief    Get the second NTP timeserver.
     *
     * This function provides the address of the second NTP timeserver. This can be either
     * a host name or IP address.
     *
     * See @ref dev_time_getter
     *
     */

    int get_Device_Time_NTPServer2(HOSTIF_MsgData_t *, bool *pChanged = false);

    /**
     * @brief    Get the third NTP timeserver.
     *
     * This function provides the address of the third NTP timeserver. This can be either
     * a host name or IP address.
     *
     * See @ref dev_time_getter
     *
     */

    int get_Device_Time_NTPServer3(HOSTIF_MsgData_t *, bool *pChanged = false);

    /**
     * @brief    Get the fourth NTP timeserver.
     *
     * This function provides the address of the fourth NTP timeserver. This can be either
     * a host name or IP address.
     *
     * See @ref dev_time_getter
     *
     */

    int get_Device_Time_NTPServer4(HOSTIF_MsgData_t *, bool *pChanged = false);

    /**
     * @brief    Get the fifth NTP timeserver.
     *
     * This function provides the address of the fifth NTP timeserver. This can be either
     * a host name or IP address.
     *
     * See @ref dev_time_getter
     *
     */

    int get_Device_Time_NTPServer5(HOSTIF_MsgData_t *, bool *pChanged = false);

    /**
     * @brief    Get the current dates & time.
     *
     * This function provides the current date and time in the CPE's local time zone.
     *
     * See @ref dev_time_getter
     *
     */

    int get_Device_Time_CurrentLocalTime(HOSTIF_MsgData_t *, bool *pChanged = false);

    /**
     * @brief    Get the local time zone definition.
     *
     * This function provides the local time zone definition, encoded according to IEEE
     * 1003.1 (POSIX). The following is an example value:
     *
     *     <tt>EST+5 EDT,M4.1.0/2,M10.5.0/2</tt>
     *
     * See @ref dev_time_getter
     *
     */

    int get_Device_Time_LocalTimeZone(HOSTIF_MsgData_t *, bool *pChanged = false);

    /* End of TR_069_DEVICE_TIME_GETTER_API doxygen group. */
    /**
     * @}
     */

    /** @addtogroup TR_069_DEVICE_TIME_SETTER_API TR-069 Device.Time Setter API.
     *  @ingroup TR_069_DEVICE_TIME_API
     *
     *  \section dev_time_setter TR-069 Device.Time Setter API
     *
     *  This is the setter group of API for the <b>Device.Time</b> object.
     *
     *  The interface for all functions is identical and is described here.
     *
     *  @param[in]   *name     This is the complete path name of the parameter extracted from
     *                         soap message, e.g. ::set_Device_Time_Enable.
     *                         In this case, the path is "Device.Time.Enable".
     *  @param[in]   *type     Data type of parameter defined for TR-069. This is same as the
     *                         data type used in the Xi3 data-model.xml file.
     *                         (see parameter.h)
     *  @param[in]   *value    This is the value to which the parameter requested by the ACS
     *                         must be set.
     *                         (see paramaccess.h)
     *
     *  @return The status of the operation.
     *  @retval OK            If parameter requested was successfully fetched. (Same as <b>NO_ERROR</b>).
     *  @retval NO_ERROR      If parameter requested was successfully fetched. (Same as <b>OK</b>).
     *  @retval DIAG_ERROR    Diagnostic error.
     *  @retval ERR_???       Appropriate error value otherwise (see dimark_globals.h).
     *
     *  @todo Clarify description of DIAG_ERROR.
     *
     *  @{
     */

    /**
     * @brief    Set the status of the time client.
     *
     * This function sets enables or disables the NTP or SNTP time client.
     *
     * See @ref dev_time_setter
     *
     */

    int set_Device_Time_Enable(HOSTIF_MsgData_t*);

    /**
     * @brief    Set the first NTP timeserver.
     *
     * This function sets the address of the first NTP timeserver. This can be either
     * a host name or IP address.
     *
     * See @ref dev_time_setter
     *
     */

    int set_Device_Time_NTPServer1(HOSTIF_MsgData_t*);

    /**
     * @brief    Set the second NTP timeserver.
     *
     * This function sets the address of the second NTP timeserver. This can be either
     * a host name or IP address.
     *
     * See @ref dev_time_setter
     *
     */

    int set_Device_Time_NTPServer2(HOSTIF_MsgData_t*);

    /**
     * @brief    Set the third NTP timeserver.
     *
     * This function sets the address of the third NTP timeserver. This can be either
     * a host name or IP address.
     *
     * See @ref dev_time_setter
     *
     */

    int set_Device_Time_NTPServer3(HOSTIF_MsgData_t*);

    /**
     * @brief    Set the fourth NTP timeserver.
     *
     * This function sets the address of the fourth NTP timeserver. This can be either
     * a host name or IP address.
     *
     * See @ref dev_time_setter
     *
     */

    int set_Device_Time_NTPServer4(HOSTIF_MsgData_t*);

    /**
     * @brief    Set the fifth NTP timeserver.
     *
     * This function sets the address of the fifth NTP timeserver. This can be either
     * a host name or IP address.
     *
     * See @ref dev_time_setter
     *
     */

    int set_Device_Time_NTPServer5(HOSTIF_MsgData_t*);

    /**
     * @brief    Set the local time zone definition.
     *
     * This function sets the local time zone definition, encoded according to IEEE
     * 1003.1 (POSIX). The following is an example value:
     *
     *     <tt>EST+5 EDT,M4.1.0/2,M10.5.0/2</tt>
     *
     * See @ref dev_time_setter
     *
     */

    int set_Device_Time_LocalTimeZone(HOSTIF_MsgData_t*);

};
/* End of TR_069_DEVICE_TIME_SETTER_API doxygen group. */
/**
 * @}
 */

#endif /* DEVICE_TIME_H_ */


/** @} */
/** @} */
