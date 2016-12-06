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
 * @file Device_IP_Diagnostics_IPPing.h
 *
 * @brief TR-069 Device.IP.Diagnostics.IPPing object Public API.
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


#ifndef DEVICE_IP_DIAGNOSTICS_IPPING_H_
#define DEVICE_IP_DIAGNOSTICS_IPPING_H_

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/
#include "paramaccess.h"
#include "parameter.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /** @defgroup TR_069_DEVICE_IP_DIAGNOSTICS_API TR-069 Device.IP.Diagnostics object API.
     *  @ingroup TR_069_DEVICE_IP_API
     *
     *  The Device.IP.Diagnostics object.
     *
     */

    /** @defgroup TR_069_DEVICE_IP_DIAGNOSTICS_IPPING_API TR-069 Device.IP.Diagnostics.IPPing object API.
     *  @ingroup TR_069_DEVICE_IP_DIAGNOSTICS_API
     *
     *  The Device.IP.Diagnostics.IPPing object provides access to an IP-layer ping test.
     *
     */

    /** @addtogroup TR_069_DEVICE_IP_DIAGNOSTICS_IPPING_GETTER_API TR-069 Device.IP.Diagnostics.IPPing Getter API.
     *  @ingroup TR_069_DEVICE_IP_DIAGNOSTICS_IPPING_API
     *
     *  \section dev_ip_diag_ipping_getter TR-069 Device.IP.Diagnostics.IPPing object Getter API
     *
     *  This is the getter group of API for the <b>Device.IP.Diagnostics.IPPing</b> object.
     *
     *  The interface for all functions is identical and is described here.
     *
     *  @param[in]   *name     This is the complete path name of the parameter extracted from
     *                         soap message, e.g. ::get_Device_IP_Diagnostics_IPPing_DiagnosticsState.
     *                         In this case, the path is "Device.IP.Diagnostics.IPPing.DiagnosticsState".
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
     * @brief    Get the status of diagnostic data.
     *
     * This function indicates availability of diagnostic data. Enumeration of:
     *
     * <tt>
     *     <ul><li>None</li>
     *         <li>Requested</li>
     *         <li>Complete</li>
     *         <li>Error_CannotResolveHostName</li>
     *         <li>Error_Internal</li>
     *         <li>Error_Other</li></ul>
     * </tt>
     *
     * @note     <ul>
     *               <li>If the ACS sets the value of this parameter to <tt>Requested</tt>,
     *               the CPE MUST initiate the corresponding diagnostic test. When writing,
     *               the only allowed value is <tt>Requested</tt>. To ensure the use of the
     *               proper test parameters (the writable parameters in this object), the
     *               test parameters MUST be set either prior to or at the same time as
     *               (in the same SetParameterValues) setting the DiagnosticsState to
     *               Requested.</li>
     *
     *               <li>When requested, the CPE SHOULD wait until after completion of the
     *               communication session with the ACS before starting the diagnostic.</li>
     *
     *               <li>When the test is completed, the value of this parameter MUST be
     *               either <tt>Complete</tt> (if the test completed successfully), or one
     *               of the <tt>Error_xxx</tt> values listed above.</li>
     *
     *               <li>If the value of this parameter is anything other than <tt>Complete</tt>,
     *               the values of the results parameters for this test are indeterminate.</li>
     *
     *               <li>When the diagnostic initiated by the ACS is completed (successfully
     *               or not), the CPE MUST establish a new connection to the ACS to allow the
     *               ACS to view the results, indicating the Event code 8 DIAGNOSTICS COMPLETE
     *               in the Inform message.</li>
     *
     *               <li>After the diagnostic is complete, the value of all result parameters
     *               (all read-only parameters in this object) MUST be retained by the CPE
     *               until either this diagnostic is run again, or the CPE reboots. After a
     *               reboot, if the CPE has not retained the result parameters from the most
     *               recent test, it MUST set the value of this parameter to <tt>None</tt>.</li>
     *
     *               <li>Modifying any of the writable parameters in this object except for this
     *               one MUST result in the value of this parameter being set to <tt>None</tt>.</li>
     *
     *               <li>While the test is in progress, modifying any of the writable parameters
     *               in this object except for this one MUST result in the test being terminated
     *               and the value of this parameter being set to <tt>None</tt>.</li>
     *
     *               <li>While the test is in progress, setting this parameter to <tt>Requested</tt>
     *               (and possibly modifying other writable parameters in this object) MUST
     *               result in the test being terminated and then restarted using the current
     *               values of the test parameters.</li>
     *           </ul>
     *
     * See @ref dev_ip_diag_ipping_getter
     *
     */

    int get_Device_IP_Diagnostics_IPPing_DiagnosticsState(const char *, ParameterType, ParameterValue *);

    /**
     * @brief    Get path over which test is to be performed.
     *
     * This function provides the IP-layer interface over which the test is to be
     * performed. This identifies the source IP address to use when performing the test.
     * Example: Device.IP.Interface.1
     *
     * @note     <ul>
     *               <li>The value MUST be the path name of a row in the IP.Interface table.</li>
     *
     *               <li>If an empty string is specified, the CPE MUST use the interface as directed
     *               by its routing policy (Forwarding table entries) to determine the appropriate
     *               interface.</li>
     *           </ul>
     *
     * See @ref dev_ip_diag_ipping_getter
     *
     */

    int get_Device_IP_Diagnostics_IPPing_Interface(const char *, ParameterType, ParameterValue *);

    /**
     * @brief    Get address of the host to ping.
     *
     * This function provides the host name or address of the host to ping.
     *
     * @note     In the case where Host is specified by name, and the name resolves to
     *           more than one address, it is up to the device implementation to choose
     *           which address to use.
     */

    int get_Device_IP_Diagnostics_IPPing_Host(const char *, ParameterType, ParameterValue *);

    /**
     * @brief    Get the number of times to repeat the test.
     *
     * This function provides the number of repetitions of the ping test to perform before
     * reporting the results.
     *
     * See @ref dev_ip_diag_ipping_getter
     *
     */

    int get_Device_IP_Diagnostics_IPPing_NumberOfRepetitions(const char *, ParameterType, ParameterValue *);

    /**
     * @brief    Get the test timeout.
     *
     * This function provides the timeout in milliseconds for the ping test.
     *
     * See @ref dev_ip_diag_ipping_getter
     *
     */

    int get_Device_IP_Diagnostics_IPPing_Timeout(const char *, ParameterType, ParameterValue *);

    /**
     * @brief    Get the ping message size.
     *
     * This function provides the size in bytes of the data block to be sent for each ping.
     *
     * See @ref dev_ip_diag_ipping_getter
     *
     */

    int get_Device_IP_Diagnostics_IPPing_DataBlockSize(const char *, ParameterType, ParameterValue *);

    /**
     * @brief    Get the DSCP to be used in test.
     *
     * This function provides the Differentiated Services Code Point (DSCP) to be used
     * for the test packets.
     *
     * @note     By default the CPE SHOULD set this value to zero.
     *
     * See @ref dev_ip_diag_ipping_getter
     *
     */

    int get_Device_IP_Diagnostics_IPPing_DSCP(const char *, ParameterType, ParameterValue *);

    /**
     * @brief    Get the number of successful pings.
     *
     * This function provides the result parameter indicating the number of successful
     * pings (those in which a successful response was received prior to the timeout)
     * in the most recent ping test.
     *
     * See @ref dev_ip_diag_ipping_getter
     *
     */

    int get_Device_IP_Diagnostics_IPPing_SuccessCount(const char *, ParameterType, ParameterValue *);

    /**
     * @brief    Get the number of failed pings.
     *
     * This function provides the result parameter indicating the number of failed pings
     * in the most recent ping test.
     *
     * See @ref dev_ip_diag_ipping_getter
     *
     */

    int get_Device_IP_Diagnostics_IPPing_FailureCount(const char *, ParameterType, ParameterValue *);

    /**
     * @brief    Get the average ping response time.
     *
     * This function provides the result parameter indicating the average response time
     * in milliseconds over all repetitions with successful responses of the most recent
     * ping test.
     *
     * @note     If there were no successful responses, this value MUST be zero.
     *
     * See @ref dev_ip_diag_ipping_getter
     *
     */

    int get_Device_IP_Diagnostics_IPPing_AverageResponseTime(const char *, ParameterType, ParameterValue *);

    /**
     * @brief    Get the minimum ping response time.
     *
     * This function provides the result parameter indicating the minimum response time
     * in milliseconds over all repetitions with successful responses of the most recent
     * ping test.
     *
     * @note     If there were no successful responses, this value MUST be zero.
     *
     * See @ref dev_ip_diag_ipping_getter
     *
     */

    int get_Device_IP_Diagnostics_IPPing_MinimumResponseTime(const char *, ParameterType, ParameterValue *);

    /**
     * @brief    Get the maximum ping response time.
     *
     * This function provides the result parameter indicating the maximum response time
     * in milliseconds over all repetitions with successful responses of the most recent
     * ping test.
     *
     * @note     If there were no successful responses, this value MUST be zero.
     *
     * See @ref dev_ip_diag_ipping_getter
     *
     */

    int get_Device_IP_Diagnostics_IPPing_MaximumResponseTime(const char *, ParameterType, ParameterValue *);

    /* End of TR_069_DEVICE_IP_DIAGNOSTICS_IPPING_GETTER_API doxygen group */
    /**
     * @}
     */

    /** @addtogroup TR_069_DEVICE_IP_DIAGNOSTICS_IPPING_SETTER_API TR-069 Device.IP.Diagnostics.IPPing Setter API.
     *  @ingroup TR_069_DEVICE_IP_DIAGNOSTICS_IPPING_API
     *
     *  \section dev_ip_diag_ipping_setter TR-069 Device.IP.Diagnostics.IPPing object Setter API
     *
     *  This is the setter group of API for the <b>Device.IP.Diagnostics.IPPing</b> object.
     *
     *  The interface for all functions is identical and is described here.
     *
     *  @param[in]   *name     This is the complete path name of the parameter extracted from
     *                         soap message, e.g. ::set_Device_IP_Diagnostics_IPPing_DiagnosticsState.
     *                         In this case, the path is "Device.IP.Diagnostics.IPPing.DiagnosticsState".
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
     * @brief    Set the status of diagnostic data.
     *
     * This function sets the <tt>DiagnosticsState</tt> parameter which indicates the
     * availability of diagnostic data. It is an enumeration of:
     *
     * <tt>
     *     <ul><li>None</li>
     *         <li>Requested</li>
     *         <li>Complete</li>
     *         <li>Error_CannotResolveHostName</li>
     *         <li>Error_Internal</li>
     *         <li>Error_Other</li></ul>
     * </tt>
     *
     * @note     <ul>
     *               <li>If the ACS sets the value of this parameter to <tt>Requested</tt>,
     *               the CPE MUST initiate the corresponding diagnostic test. When writing,
     *               the only allowed value is <tt>Requested</tt>. To ensure the use of the
     *               proper test parameters (the writable parameters in this object), the
     *               test parameters MUST be set either prior to or at the same time as
     *               (in the same SetParameterValues) setting the DiagnosticsState to
     *               Requested.</li>
     *
     *               <li>When requested, the CPE SHOULD wait until after completion of the
     *               communication session with the ACS before starting the diagnostic.</li>
     *
     *               <li>When the test is completed, the value of this parameter MUST be
     *               either <tt>Complete</tt> (if the test completed successfully), or one
     *               of the <tt>Error_xxx</tt> values listed above.</li>
     *
     *               <li>If the value of this parameter is anything other than <tt>Complete</tt>,
     *               the values of the results parameters for this test are indeterminate.</li>
     *
     *               <li>When the diagnostic initiated by the ACS is completed (successfully
     *               or not), the CPE MUST establish a new connection to the ACS to allow the
     *               ACS to view the results, indicating the Event code 8 DIAGNOSTICS COMPLETE
     *               in the Inform message.</li>
     *
     *               <li>After the diagnostic is complete, the value of all result parameters
     *               (all read-only parameters in this object) MUST be retained by the CPE
     *               until either this diagnostic is run again, or the CPE reboots. After a
     *               reboot, if the CPE has not retained the result parameters from the most
     *               recent test, it MUST set the value of this parameter to <tt>None</tt>.</li>
     *
     *               <li>Modifying any of the writable parameters in this object except for this
     *               one MUST result in the value of this parameter being set to <tt>None</tt>.</li>
     *
     *               <li>While the test is in progress, modifying any of the writable parameters
     *               in this object except for this one MUST result in the test being terminated
     *               and the value of this parameter being set to <tt>None</tt>.</li>
     *
     *               <li>While the test is in progress, setting this parameter to <tt>Requested</tt>
     *               (and possibly modifying other writable parameters in this object) MUST
     *               result in the test being terminated and then restarted using the current
     *               values of the test parameters.</li>
     *           </ul>
     *
     * See @ref dev_ip_diag_ipping_getter
     *
     */

    int set_Device_IP_Diagnostics_IPPing_DiagnosticsState(const char *, ParameterType, ParameterValue *);

    /**
     * @brief    Set path over which test is to be performed.
     *
     * This function sets the IP-layer interface over which the test is to be
     * performed. This identifies the source IP address to use when performing the test.
     * Example: Device.IP.Interface.1
     *
     * @note     <ul>
     *               <li>The value MUST be the path name of a row in the IP.Interface table.</li>
     *
     *               <li>If an empty string is specified, the CPE MUST use the interface as directed
     *               by its routing policy (Forwarding table entries) to determine the appropriate
     *               interface.</li>
     *           </ul>
     *
     * See @ref dev_ip_diag_ipping_setter
     *
     */

    int set_Device_IP_Diagnostics_IPPing_Interface(const char *, ParameterType, ParameterValue *);

    /**
     * @brief    Set the address of the host to ping.
     *
     * This function sets the host name or address of the host to ping.
     *
     * @note     In the case where Host is specified by name, and the name resolves to
     *           more than one address, it is up to the device implementation to choose
     *           which address to use.
     *
     * See @ref dev_ip_diag_ipping_setter
     *
     */

    int set_Device_IP_Diagnostics_IPPing_Host(const char *, ParameterType, ParameterValue *);

    /**
     * @brief    Set the number of times to repeat the test.
     *
     * This function sets the number of repetitions of the ping test to perform before
     * reporting the results.
     *
     * See @ref dev_ip_diag_ipping_setter
     *
     */

    int set_Device_IP_Diagnostics_IPPing_NumberOfRepetitions(const char *, ParameterType, ParameterValue *);

    /**
     * @brief    Set the test timeout.
     *
     * This function sets the timeout in milliseconds for the ping test.
     *
     * @note     Value is >= 1.
     *
     * See @ref dev_ip_diag_ipping_setter
     *
     */

    int set_Device_IP_Diagnostics_IPPing_Timeout(const char *, ParameterType, ParameterValue *);

    /**
     * @brief    Set the ping message size.
     *
     * This function sets the size in bytes of the data block to be sent for each ping.
     *
     * See @ref dev_ip_diag_ipping_setter
     *
     */

    int set_Device_IP_Diagnostics_IPPing_DataBlockSize(const char *, ParameterType, ParameterValue *);

    /**
     * @brief    Set the DSCP to be used in test.
     *
     * This function sets the Differentiated Services Code Point (DSCP) to be used for
     * the test packets.
     *
     * @note     By default the CPE SHOULD set this value to zero.
     *
     * See @ref dev_ip_diag_ipping_setter
     *
     */

    int set_Device_IP_Diagnostics_IPPing_DSCP(const char *, ParameterType, ParameterValue *);

    /* End of TR_069_DEVICE_IP_DIAGNOSTICS_IPPING_SETTER_API doxygen group */
    /**
     * @}
     */

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_IP_DIAGNOSTICS_IPPING_H_ */


/** @} */
/** @} */
