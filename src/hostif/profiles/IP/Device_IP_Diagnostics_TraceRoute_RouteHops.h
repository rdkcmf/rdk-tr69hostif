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
 * @file Device_IP_Diagnostics_TraceRoute_RouteHops.h
 * @brief The header file provides TR069 device IP diagnostics trace route APIs.
 */

/**
 *
 * @defgroup TR69_HOSTIF_DEVICE_ROUTEHOPS_API TR-069 Object (Device.IP.Diagnostics.TraceRoute.RouteHops.{i}) Public APIs
 * Describe the details about TR-069 Device IP Interface APIs specifications.
 * @ingroup TR69_HOSTIF_DEVICE_IP
 */

/**
 * @file Device_IP_Diagnostics_TraceRoute_RouteHops.h
 *
 * @brief TR-069 Device.IP.Diagnostics.TraceRoute.RouteHops object Public API.
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


#ifndef DEVICE_IP_DIAGNOSTICS_TRACEROUTE_ROUTEHOPS_H_
#define DEVICE_IP_DIAGNOSTICS_TRACEROUTE_ROUTEHOPS_H_

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/
#include "parameter.h"
#include "paramaccess.h"


#ifdef __cplusplus
extern "C"
{
#endif

    /** @defgroup TR_069_DEVICE_IP_DIAGNOSTICS_TRACEROUTE_ROUTEHOPS_API TR-069 Device.IP.Diagnostics.TraceRoute.RouteHops object API.
     *  @ingroup TR_069_DEVICE_IP_DIAGNOSTICS_TRACEROUTE_API
     *
     *  The Device.IP.Diagnostics.TraceRoute.RouteHops object contains the array of hop results returned.
     *
     *  @note    If a route could not be determined, this array will be empty.
     *
     */

    /** @addtogroup TR_069_DEVICE_IP_DIAGNOSTICS_TRACEROUTE_ROUTEHOPS_GETTER_API TR-069 Device.IP.Diagnostics.TraceRoute.RouteHops Getter API.
     *  @ingroup TR_069_DEVICE_IP_DIAGNOSTICS_TRACEROUTE_ROUTEHOPS_API
     *
     *  \section dev_ip_diag_traceroute_hops_getter TR-069 Device.IP.Diagnostics.TraceRoute.RouteHops object Getter API
     *
     *  This is the getter group of API for the <b>Device.IP.Diagnostics.TraceRoute.RouteHops.{i}</b> object.
     *
     *  The interface for all functions is identical and is described here.
     *
     *  @param[in]   *name     This is the complete path name of the parameter extracted from
     *                         soap message, e.g. ::get_Device_IP_Diagnostics_TraceRoute_RouteHops_Host.
     *                         In this case, the path is "Device.IP.Diagnostics.TraceRoute.RouteHops.{i}.Host".
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
 * @brief This function provides the result parameter indicating the Host Name, if DNS is
 * able to resolve it, or the IP Address of a hop along the discovered route.
 *
 * @param[in] name Source IP address.
 * @param[in] type  Type of the valid interface.
 * @param[in] value   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the address of a hop.
 * @retval -1 If Not able to get the address of a hop.
 * @retval -2 If Not handle getting the address of a hop.
 * @ingroup TR69_HOSTIF_DEVICE_ROUTEHOPS_API
 */

    int get_Device_IP_Diagnostics_TraceRoute_RouteHops_Host(const char *name, ParameterType type, ParameterValue *value);

/**
 * @brief This function provides the last IP address of the host returned for this hop.
 *
 * @note This may be an empty string. If this parameter is not an empty string, the "Host" will
 * contain the Host Name returned from the reverse DNS query.
 *
 * @param[in] name  Source IP address.
 * @param[in] type  Type of the valid interface.
 * @param[in] value Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the address of the host returned for a hop.
 * @retval -1 If Not able to get the address of the host returned for a hop.
 * @retval -2 If Not handle getting the address of the host returned for a hop.
 * @ingroup TR69_HOSTIF_DEVICE_ROUTEHOPS_API
 */

    int get_Device_IP_Diagnostics_TraceRoute_RouteHops_HostAddress(const char *name, ParameterType type, ParameterValue *value);

/**
 * @brief This function provides the error code returned for this hop. This code is directly
 * from the ICMP CODE field.
 *
 * @param[in] name  Source IP address.
 * @param[in] type  Type of the valid interface.
 * @param[in] value   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the error code returned for this hop.
 * @retval -1 If Not able to get the error code returned for this hop.
 * @retval -2 If Not handle getting the error code returned for this hop.
 * @ingroup TR69_HOSTIF_DEVICE_ROUTEHOPS_API
 */

    int get_Device_IP_Diagnostics_TraceRoute_RouteHops_ErrorCode(const char *name, ParameterType type, ParameterValue *value);

/**
 * @brief This function provides a comma-separated list (maximum length 16) of unsigned
 * integers. Each list item contains one or more round trip times in milliseconds
 * (one for each repetition) for this hop.
 *
 * @param[in] name  Source IP address.
 * @param[in] type  Type of the valid interface.
 * @param[in] value   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the round trip times for a hop.
 * @retval -1 If Not able to get the round trip times for a hop.
 * @retval -2 If Not handle getting the round trip times for a hop.
 * @ingroup TR69_HOSTIF_DEVICE_ROUTEHOPS_API
 */

    int get_Device_IP_Diagnostics_TraceRoute_RouteHops_RTTimes(const char *name, ParameterType type, ParameterValue *value);

    /* End of TR_069_DEVICE_IP_DIAGNOSTICS_TRACEROUTE_GETTER_API doxygen group */
    /**
     * @}
     */

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_IP_DIAGNOSTICS_TRACEROUTE_ROUTEHOPS_H_ */


/** @} */
/** @} */
