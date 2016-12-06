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
 * @file Device_IP_Diagnostics_TraceRoute.h
 * @brief The header file provides TR069 device IP diagnostics trace route APIs.
 */

/**
 * @defgroup TR69_HOSTIF_DEVICE_TRACEROUTE_API TR-069 Object (Device.IP.Diagnostics.TraceRoute) Public APIs
 * Describe the details about TR-069 Device IP Trace route APIs specifications.
 * @ingroup TR69_HOSTIF_DEVICE_IP
 */

/**
 * @file Device_IP_Diagnostics_TraceRoute.h
 *
 * @brief TR-069 Device.IP.Diagnostics.TraceRoute object Public API.
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


#ifndef DEVICE_IP_DIAGNOSTICS_TRACEROUTE_H_
#define DEVICE_IP_DIAGNOSTICS_TRACEROUTE_H_

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/
#include "parameter.h"
#include "paramaccess.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /** @defgroup TR_069_DEVICE_IP_DIAGNOSTICS_TRACEROUTE_API TR-069 Device.IP.Diagnostics.TraceRoute object API.
     *  @ingroup TR_069_DEVICE_IP_DIAGNOSTICS_API
     *
     *  The Device.IP.Diagnostics.TraceRoute object provides access to an IP-layer ping test.
     *
     */

    /** @addtogroup TR_069_DEVICE_IP_DIAGNOSTICS_TRACEROUTE_GETTER_API TR-069 Device.IP.Diagnostics.TraceRoute Getter API.
     *  @ingroup TR_069_DEVICE_IP_DIAGNOSTICS_TRACEROUTE_API
     *
     *  \section dev_ip_diag_traceroute_getter TR-069 Device.IP.Diagnostics.TraceRoute object Getter API
     *
     *  This is the getter group of API for the <b>Device.IP.Diagnostics.TraceRoute</b> object.
     *
     *  The interface for all functions is identical and is described here.
     *
     *  @param[in]   *name     This is the complete path name of the parameter extracted from
     *                         soap message, e.g. ::get_Device_IP_Diagnostics_TraceRoute_DiagnosticsState.
     *                         In this case, the path is "Device.IP.Diagnostics.TraceRoute.DiagnosticsState".
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
 * @brief This function indicates availability of diagnostic data. Enumeration of:
 * - None
 * - Requested
 * - Complete
 * - Error_CannotResolveHostName
 * - Error_MaxHopCountExceeded
 *
 * @note
 * - If the ACS sets the value of this parameter to "Requested",
 *   the CPE MUST initiate the corresponding diagnostic test. When writing,
 *   the only allowed value is "Requested". To ensure the use of the
 *   proper test parameters (the writeable parameters in this object), the
 *   test parameters MUST be set either prior to or at the same time as
 *  (in the same SetParameterValues) setting the DiagnosticsState to
 *  "Requested".
 *
 * - When requested, the CPE SHOULD wait until after completion of the
 *   communication session with the ACS before starting the diagnostic.
 *
 * - When the diagnostic initiated by the ACS is completed
 *  (successfully or not), the CPE MUST establish a new connection to
 *  the ACS to allow the ACS to view the results, indicating the Event
 *  code 8 DIAGNOSTICS COMPLETE in the Inform message.
 *
 *
 * @param[in] const char *  Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the availability of diagnostic data.
 * @retval -1 If Not able to get availability of diagnostic data.
 * @retval -2 If Not handle getting the availability of diagnostic data.
 * @ingroup TR69_HOSTIF_DEVICE_TRACEROUTE_API
 */

    int get_Device_IP_Diagnostics_TraceRoute_DiagnosticsState(const char *name, ParameterType type, ParameterValue *value);

/**
 * @brief  This function provides the IP-layer interface over which the test is to be
 * performed. This identifies the source IP address to use when performing the test.
 * Example: Device.IP.Interface.1
 *
 * @note The value MUST be the path name of a row in the IP.Interface table.
 *
 * @param[in] const char *  Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the source IP address of the interface.
 * @retval -1 If Not able to get the source IP address of the interface.
 * @retval -2 If Not handle getting the source IP address of the interface.
 * @ingroup TR69_HOSTIF_DEVICE_TRACEROUTE_API
 */

    int get_Device_IP_Diagnostics_TraceRoute_Interface(const char *name, ParameterType type, ParameterValue *value);

/**
 * @brief This function provides the host name or address of the host to which to find a route.
 *
 * @note In the case where 'Host' is specified by name, and the name
 * resolves to more than one address, it is up to the device implementation
 * to choose which address to use.
 *
 * @param[in] const char *  Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the host name or address of the host interface.
 * @retval -1 If Not able to get the host name or address of the host interface.
 * @retval -2 If Not handle getting the host name or address of the host interface.
 * @ingroup TR69_HOSTIF_DEVICE_TRACEROUTE_API
 */

    int get_Device_IP_Diagnostics_TraceRoute_Host(const char *name, ParameterType type, ParameterValue *value);

/**
 * @brief This function provides the number of tries per hop. This is set prior to running
 * Diagnostic.
 *
 * @note By default, the CPE SHOULD set this value to 3.
 *
 * @param[in] const char *  Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the number of tries per hop.
 * @retval -1 If Not able to get the number of tries per hop.
 * @retval -2 If Not handle getting the number of tries per hop.
 * @ingroup TR69_HOSTIF_DEVICE_TRACEROUTE_API
 */

    int get_Device_IP_Diagnostics_TraceRoute_NumberOfTries(const char *name, ParameterType type, ParameterValue *value);

/**
 * @brief This function provides the timeout in milliseconds for each hop of the trace route
 * test.
 *
 * @note By default the CPE SHOULD set this value to 5000.
 *
 * @param[in] const char *  Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the timeout of each hop.
 * @retval -1 If Not able to get the timeout of each hop.
 * @retval -2 If Not handle getting the timeout of each hop.
 * @ingroup TR69_HOSTIF_DEVICE_TRACEROUTE_API
 */

    int get_Device_IP_Diagnostics_TraceRoute_Timeout(const char *name, ParameterType type, ParameterValue *value);

/**
 * @brief  This function provides the size in bytes of the data block to be sent for each trace route.
 *
 * @note By default, the CPE SHOULD set this value to 38.
 *
 * @param[in] const char *  Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the trace route message size.
 * @retval -1 If Not able to get the trace route message size.
 * @retval -2 If Not handle getting the trace route message size.
 * @ingroup TR69_HOSTIF_DEVICE_TRACEROUTE_API
 */

    int get_Device_IP_Diagnostics_TraceRoute_DataBlockSize(const char *name, ParameterType type, ParameterValue *value);

/**
 * @brief This function provides the Differentiated Services Code Point (DSCP) to be used
 * for the test packets.
 *
 * @note By default the CPE SHOULD set this value to zero.
 *
 * @param[in] const char *  Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the Differentiated Services Code Point (DSCP).
 * @retval -1 If Not able to get the Differentiated Services Code Point (DSCP).
 * @retval -2 If Not handle getting the Differentiated Services Code Point (DSCP).
 * @ingroup TR69_HOSTIF_DEVICE_TRACEROUTE_API
 */

    int get_Device_IP_Diagnostics_TraceRoute_DSCP(const char *name, ParameterType type, ParameterValue *value);

/**
 * @brief This function provides the maximum number of hop used in outgoing probe packets
 * (max TTL).
 *
 * @note By default the CPE SHOULD set this value to 30.
 *
 * @param[in] const char *  Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the maximum number of hops count.
 * @retval -1 If Not able to get the maximum number of hops count.
 * @retval -2 If Not handle getting the maximum number of hops count.
 * @ingroup TR69_HOSTIF_DEVICE_TRACEROUTE_API
 */

    int get_Device_IP_Diagnostics_TraceRoute_MaxHopCount(const char *name, ParameterType type, ParameterValue *value);

/**
 * @brief This function provides the result parameter indicating the response time in
 * milliseconds of the most recent trace route test.
 *
 * @note If a route could not be determined, this value MUST be zero.
 *
 * @param[in] const char *  Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hop response time.
 * @retval -1 If Not able to get the hop response time.
 * @retval -2 If Not handle getting the hop response time.
 * @ingroup TR69_HOSTIF_DEVICE_TRACEROUTE_API
 */

    int get_Device_IP_Diagnostics_TraceRoute_ResponseTime(const char *name, ParameterType type, ParameterValue *value);

/**
 * @brief This function provides the number of entries in the RouteHops table.
 *
 * @param[in] const char *  Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the number of entries in the RouteHops table.
 * @retval -1 If Not able to get the number of entries in the RouteHops table.
 * @retval -2 If Not handle getting the number of entries in the RouteHops table.
 * @ingroup TR69_HOSTIF_DEVICE_TRACEROUTE_API
 */

    int get_Device_IP_Diagnostics_TraceRoute_RouteHopsNumberOfEntries(const char *name, ParameterType type, ParameterValue *value);

    /* End of TR_069_DEVICE_IP_DIAGNOSTICS_TRACEROUTE_GETTER_API doxygen group */
    /**
     * @}
     */

    /** @addtogroup TR_069_DEVICE_IP_DIAGNOSTICS_TRACEROUTE_SETTER_API TR-069 Device.IP.Diagnostics.TraceRoute Setter API.
     *  @ingroup TR_069_DEVICE_IP_DIAGNOSTICS_TRACEROUTE_API
     *
     *  \section dev_ip_diag_traceroute_setter TR-069 Device.IP.Diagnostics.TraceRoute object Setter API
     *
     *  This is the setter group of API for the <b>Device.IP.Diagnostics.TraceRoute</b> object.
     *
     *  The interface for all functions is identical and is described here.
     *
     *  @param[in]   *name     This is the complete path name of the parameter extracted from
     *                         soap message, e.g. ::set_Device_IP_Diagnostics_TraceRoute_DiagnosticsState.
     *                         In this case, the path is "Device.IP.Diagnostics.TraceRoute.DiagnosticsState".
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
 * @brief This function sets the 'DiagnosticsState' parameter which indicates
 * the availability of diagnostic data. This is an enumeration of:
 * - None
 * - Requested
 * - Complete
 * - Error_CannotResolveHostName
 * - Error_MaxHopCountExceeded
 *
 *
 * @note
 *             - If the ACS sets the value of this parameter to "Requested",
 *               the CPE MUST initiate the corresponding diagnostic test. When writing,
 *               the only allowed value is "Requested". To ensure the use of the
 *               proper test parameters (the writeable parameters in this object), the
 *               test parameters MUST be set either prior to or at the same time as
 *               (in the same SetParameterValues) setting the DiagnosticsState to
 *               "Requested".
 *
 *             - When requested, the CPE SHOULD wait until after completion of the
 *               communication session with the ACS before starting the diagnostic.
 *
 *             - When the diagnostic initiated by the ACS is completed
 *               (successfully or not), the CPE MUST establish a new connection to
 *               the ACS to allow the ACS to view the results, indicating the Event
 *               code 8 DIAGNOSTICS COMPLETE in the Inform message.
 *
 * @param[in] const char *  Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the status of diagnostic data.
 * @retval -1 If Not able to set the status of diagnostic data.
 * @retval -2 If Not handle setting the status of diagnostic data.
 * @ingroup TR69_HOSTIF_DEVICE_TRACEROUTE_API
 */

    int set_Device_IP_Diagnostics_TraceRoute_DiagnosticsState(const char *name, ParameterType type, ParameterValue *value);

/**
 * @brief This function sets the IP-layer interface over which the test is to be
 * performed. This identifies the source IP address to use when performing the test.
 * Example: Device.IP.Interface.1
 *
 * @note The value MUST be the path name of a row in the IP.Interface table.
 *
 * @param[in] const char *  Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the IP-layer interface.
 * @retval -1 If Not able to set the IP-layer interface.
 * @retval -2 If Not handle setting the IP-layer interface.
 * @ingroup TR69_HOSTIF_DEVICE_TRACEROUTE_API
 */

    int set_Device_IP_Diagnostics_TraceRoute_Interface(const char *name, ParameterType type, ParameterValue *value);

/**
 * @brief This function sets the host name or address of the host to which to find a route.
 *
 * @note In the case where "Host" is specified by name, and the name
 * resolves to more than one address, it is up to the device implementation
 * to choose which address to use.
 *
 * @param[in] const char *  Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the host name or address of the host.
 * @retval -1 If Not able to set the host name or address of the host.
 * @retval -2 If Not handle setting the host name or address of the host.
 * @ingroup TR69_HOSTIF_DEVICE_TRACEROUTE_API
 */

    int set_Device_IP_Diagnostics_TraceRoute_Host(const char *name, ParameterType type, ParameterValue *value);

/**
 * @brief This function sets the number of tries per hop. This is set prior to running
 * Diagnostic.
 *
 * @note By default, the CPE SHOULD set this value to 3.
 *
 * @param[in] const char *  Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the number of tries per hop.
 * @retval -1 If Not able to set the number of tries per hop.
 * @retval -2 If Not handle setting the number of tries per hop.
 * @ingroup TR69_HOSTIF_DEVICE_TRACEROUTE_API
 */

    int set_Device_IP_Diagnostics_TraceRoute_NumberOfTries(const char *name, ParameterType type, ParameterValue *value);

/**
 * @brief This function sets the timeout in milliseconds for each hop of the trace route
 * test.
 *
 * @note  By default the CPE SHOULD set this value to 5000.
 *
 * @param[in] const char *  Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the hop timeout.
 * @retval -1 If Not able to set the hop timeout.
 * @retval -2 If Not handle setting the hop timeout.
 * @ingroup TR69_HOSTIF_DEVICE_TRACEROUTE_API
 */

    int set_Device_IP_Diagnostics_TraceRoute_Timeout(const char *name, ParameterType type, ParameterValue *value);

/**
 * @brief  This function sets the size in bytes of the data block to be sent for each trace route.
 *
 * @note By default, the CPE SHOULD set this value to 38.
 *
 * @param[in] const char *  Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the trace route message size.
 * @retval -1 If Not able to set the trace route message size.
 * @retval -2 If Not handle setting the trace route message size.
 * @ingroup TR69_HOSTIF_DEVICE_TRACEROUTE_API
 */

    int set_Device_IP_Diagnostics_TraceRoute_DataBlockSize(const char *name, ParameterType type, ParameterValue *value);

/**
 * @brief This function sets the Differentiated Services Code Point (DSCP) to be used
 * for the test packets.
 *
 * @note  By default the CPE SHOULD set this value to zero.
 *
 * @param[in] const char *  Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the DSCP to be used in test.
 * @retval -1 If Not able to set the DSCP to be used in test.
 * @retval -2 If Not handle setting the DSCP to be used in test.
 * @ingroup TR69_HOSTIF_DEVICE_TRACEROUTE_API
 */

    int set_Device_IP_Diagnostics_TraceRoute_DSCP(const char *name, ParameterType type, ParameterValue *value);



/**
 * @brief This function sets the maximum number of hop used in outgoing probe packets (max
 * TTL).
 *
 * @note  By default the CPE SHOULD set this value to 30.
 *
 * @param[in] const char *  Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the maximum number of hops.
 * @retval -1 If Not able to set the maximum number of hops.
 * @retval -2 If Not handle setting the maximum number of hops.
 * @ingroup TR69_HOSTIF_DEVICE_TRACEROUTE_API
 */

    int set_Device_IP_Diagnostics_TraceRoute_MaxHopCount(const char *name, ParameterType type, ParameterValue *value);

    /* End of TR_069_DEVICE_IP_DIAGNOSTICS_TRACEROUTE_SETTER_API doxygen group */
    /**
     * @}
     */

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_IP_DIAGNOSTICS_TRACEROUTE_H_ */


/** @} */
/** @} */
