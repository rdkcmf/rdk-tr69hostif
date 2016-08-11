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
 * @file Device_IP_Diagnostics_UDPEchoConfig.h
 * @brief The header file provides TR069 device IP diagnostics UDPEcho config APIs.
 */

/**
 *
 * @defgroup TR69_HOSTIF_DEVICE_IP_UDPECHOCONFIG_API TR-069 Object (Device.IP.Diagnostics.UDPEChoConfig) Public APIs
 * Describe the details about TR-069 Device IP Diagnostics UDPEChoConfig APIs specifications.
 * @ingroup TR69_HOSTIF_DEVICE_IP
 */



/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef DEVICE_IP_DIAGNOSTICS_UDPECHOCONFIG_H_
#define DEVICE_IP_DIAGNOSTICS_UDPECHOCONFIG_H_

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/

#include "paramaccess.h"
#include "parameter.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /** @defgroup TR_069_DEVICE_IP_DIAGNOSTICS_UDPECHOCONFIG_API TR-069 Device.IP.Diagnostics.UDPEchoConfig object API.
     *  @ingroup TR_069_DEVICE_IP_DIAGNOSTICS_API
     *
     *  The Device.IP.Diagnostics.UDPEchoConfig object allows the CPE to be configured to
     *  perform the UDP Echo and Echo Plus Services.
     *
     */

    /** @addtogroup TR_069_DEVICE_IP_DIAGNOSTICS_UDPECHOCONFIG_SETTER_API TR-069 Device.IP.Diagnostics.UDPEchoConfig Setter API.
     *  @ingroup TR_069_DEVICE_IP_DIAGNOSTICS_UDPECHOCONFIG_API
     *
     *  \section dev_ip_diag_udpechoconfig_setter TR-069 Device.IP.Diagnostics.UDPEchoConfig object Setter API
     *
     *  This is the setter group of API for the <b>Device.IP.Diagnostics.UDPEchoConfig</b> object.
     *
     *  The interface for all functions is identical and is described here.
     *
     *  @param[in]   *name     This is the complete path name of the parameter extracted from
     *                         soap message, e.g. ::set_Device_IP_Diagnostics_UDPEchoConfig_Interface.
     *                         In this case, the path is "Device.IP.Diagnostics.UDPEchoConfig.Interface".
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
 * @brief    Set the enable flag for UDP Echo.
 *
 * This function sets the enable flag for UDP Echo.
 *
 * @note MUST be enabled to receive UDP echo. When enabled from a disabled state
 * all related timestamps, statistics and UDP Echo Plus counters are cleared.
 *
 * @param[in] const char   Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the source IP address of the interface.
 * @retval -1 If Not able to set the source IP address of the interface.
 * @retval -2 If Not handle setting the source IP address of the interface.
 * @ingroup TR69_HOSTIF_DEVICE_IP_UDPECHOCONFIG_API
 */

    int set_Device_IP_Diagnostics_UDPEchoConfig_Enable(const char *, ParameterType, ParameterValue *);

/**
 * @brief This function sets the the path name of IP-layer interface over which the CPE
 * MUST listen for and receive UDP echo requests.
 *
 * @note The value of this parameter MUST be either a valid interface or
 * an empty string. An attempt to set this parameter to a different value MUST be rejected as an
 * invalid parameter value.
 * If an empty string is specified, the CPE MUST listen and receive UDP echo requests
 * on all interfaces. Interfaces behind a NAT MAY require port forwarding rules
 * configured in the Gateway to enable receiving the UDP packets.
 *
 * @param[in] const char   Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the path over which the test is to be performed.
 * @retval -1 If Not able to set the path over which the test is to be performed.
 * @retval -2 If Not handle setting the path over which the test is to be performed.
 * @ingroup TR69_HOSTIF_DEVICE_IP_UDPECHOCONFIG_API
 */

    int set_Device_IP_Diagnostics_UDPEchoConfig_Interface(const char *, ParameterType, ParameterValue *);

/**
 * @brief This function sets the UDP port on which the UDP server MUST listen and respond to
 * UDP echo requests.
 *
 * @param[in] const char   Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the UDP Echo port.
 * @retval -1 If Not able to set the UDP Echo port.
 * @retval -2 If Not handle setting the UDP Echo port.
 * @ingroup TR69_HOSTIF_DEVICE_IP_UDPECHOCONFIG_API
 */

    int set_Device_IP_Diagnostics_UDPEchoConfig_UDPPort(const char *, ParameterType, ParameterValue *);

    /* End of TR_069_DEVICE_IP_DIAGNOSTICS_UDPECHOCONFIG_SETTER_API doxygen group */
    /**
     * @}
     */

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_IP_DIAGNOSTICS_UDPECHOCONFIG_H_ */


/** @} */
/** @} */
