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
 * @file Device_IP_Diagnostics_UploadDiagnostics.h
 * @brief The header file provides TR069 device IP diagnostics upload APIs.
 */

/**
 * @defgroup TR69_HOSTIF_DEVICE_IP_UPLOAD_API TR-069 Object (Device.IP.Diagnostics.UploadDiagnostics) Public APIs
 * Describe the details about TR-069 Device IP UploadDiagnostics APIs specifications.
 * @ingroup TR69_HOSTIF_DEVICE_IP
 */



/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef DEVICE_IP_DIAGNOSTICS_UPLOADDIAGNOSTICS_H_
#define DEVICE_IP_DIAGNOSTICS_UPLOADDIAGNOSTICS_H_

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/

#include "paramaccess.h"
#include "parameter.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /** @defgroup TR_069_DEVICE_IP_DIAGNOSTICS_UPLOADDIAGNOSTICS_API TR-069 Device.IP.Diagnostics.UploadDiagnostics object API.
     *  @ingroup TR_069_DEVICE_IP_DIAGNOSTICS_API
     *
     *  The Device.IP.Diagnostics.UploadDiagnostics object defines the diagnostics
     *  configuration for a HTTP and FTP UploadDiagnostics Test.
     *
     *  Files sent by the UploadDiagnostics do not require file storage on the CPE
     *  device, and MAY be an arbitrary stream of bytes.
     *
     */

    /** @addtogroup TR_069_DEVICE_IP_DIAGNOSTICS_UPLOADDIAGNOSTICS_SETTER_API TR-069 Device.IP.Diagnostics.UploadDiagnostics Setter API.
     *  @ingroup TR_069_DEVICE_IP_DIAGNOSTICS_UPLOADDIAGNOSTICS_API
     *
     *  \section dev_ip_diag_uploaddiagnostics_setter TR-069 Device.IP.Diagnostics.UploadDiagnostics object Setter API
     *
     *  This is the setter group of API for the <b>Device.IP.Diagnostics.UploadDiagnostics</b> object.
     *
     *  The interface for all functions is identical and is described here.
     *
     *  @param[in]   *name     This is the complete path name of the parameter extracted from
     *                         soap message, e.g. ::set_Device_IP_Diagnostics_UploadDiagnostics_DiagnosticsState.
     *                         In this case, the path is "Device.IP.Diagnostics.UploadDiagnostics.DiagnosticsState".
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
 * @brief This function sets the DiagnosticsState parameter which indicates the availability
 * of diagnostic data. This is an enumeration of:
 *  -None
 *  -Requested
 *  -Completed
 *  -Error_InitConnectionFailed
 *  -Error_NoResponse
 *  -Error_TransferFailed
 *  -Error_PasswordRequestFailed
 *  -Error_LoginFailed
 *  -Error_NoTransferMode
 *  -Error_NoPASV
 *  -Error_NoCWD
 *  -Error_NoSTOR
 *  -Error_NoTransferComplete
 *
 * @note - If the ACS sets the value of this parameter to "Requested",
 * the CPE MUST initiate the corresponding diagnostic test. When writing,
 * the only allowed value is "Requested". To ensure the use of the
 * proper test parameters (the writeable parameters in this object), the
 * test parameters MUST be set either prior to or at the same time as
 * (in the same SetParameterValues) setting the DiagnosticsState to
 * "Requested".
 *
 * - When requested, the CPE SHOULD wait until after completion of the
 * communication session with the ACS before starting the diagnostic.
 *
 * - When the test is completed, the value of this parameter MUST be
 * either "Completed" (if the test completed successfully), or
 * one of the "Error_xxx" values listed above.
 *
 * - If the value of this parameter is anything other than
 * "Completed", the values of the results parameters for this
 * test are indeterminate.
 *
 * - When the diagnostic initiated by the ACS is completed
 *(successfully or not), the CPE MUST establish a new connection to
 * the ACS to allow the ACS to view the results, indicating the Event
 * code 8 DIAGNOSTICS COMPLETE in the Inform message.
 *
 * - After the diagnostic is complete, the value of all result
 * parameters (all read-only parameters in this object) MUST be
 * retained by the CPE until either this diagnostic is run again, or
 * the CPE reboots. After a reboot, if the CPE has not retained the
 * result parameters from the most recent test, it MUST set the value
 * of this parameter to "None".
 *
 * - Modifying any of the writeable parameters in this object except
 * for this one MUST result in the value of this parameter being set
 * to "None".
 *
 * - While the test is in progress, modifying any of the writeable
 * parameters in this object except for this one MUST result in the
 * test being terminated and the value of this parameter being set to
 * "None".
 *
 * - While the test is in progress, setting this parameter to
 * "Requested" (and possibly modifying other writeable parameters
 * in this object) MUST result in the test being terminated and then
 * restarted using the current values of the test parameters.
 *
 *
 * @param[in] const char   Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the status of diagnostic data.
 * @retval -1 If Not able to set the status of diagnostic data.
 * @retval -2 If Not handle setting the status of diagnostic data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_UPLOAD_API
 */

    int set_Device_IP_Diagnostics_UploadDiagnostics_DiagnosticsState(const char *, ParameterType, ParameterValue *);

/**
 * @brief This function sets the IP-layer interface over which the test is to be
 * performed.
 *
 * @note  - The value of this parameter MUST be either a valid interface or
 *          an empty string. An attempt to set this parameter to a different
 *          value MUST be rejected as an invalid parameter value.
 *
 *        - If an empty string is specified, the CPE MUST use the default
 *          routing interface.
 *
 *
 * @param[in] const char   Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the IP-layer interface.
 * @retval -1 If Not able to set the IP-layer interface.
 * @retval -2 If Not handle setting the IP-layer interface.
 * @ingroup TR69_HOSTIF_DEVICE_IP_UPLOAD_API
 */

    int set_Device_IP_Diagnostics_UploadDiagnostics_Interface(const char *, ParameterType, ParameterValue *);

/**
 * @brief This function sets the URL to which CPE is to perform the upload. This
 * parameter MUST be in the form of a valid HTTP or FTP URL.
 *
 * @note
 *       - When using FTP transport, FTP binary transfer MUST be used.
 *
 *       - When using HTTP transport, persistent connections MUST be used
 *         and pipeline MUST NOT be used.
 *
 *       - When using HTTP transport the HTTP Authentication MUST NOT be
 *         used.
 *
 *
 * @param[in] const char   Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the upload URL.
 * @retval -1 If Not able to set the upload URL.
 * @retval -2 If Not handle setting the upload URL.
 * @ingroup TR69_HOSTIF_DEVICE_IP_UPLOAD_API
 */

    int set_Device_IP_Diagnostics_UploadDiagnostics_UploadURL(const char *, ParameterType, ParameterValue *);

/**
 * @brief This function sets the Differentiated Services Code Point (DSCP) to be used for
 * marking packets transmitted in the test.
 *
 * @note The default value SHOULD be zero.
 *
 * @param[in] const char   Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the DSCP to be used in test.
 * @retval -1 If Not able to set the DSCP to be used in test.
 * @retval -2 If Not handle setting the DSCP to be used in test.
 * @ingroup TR69_HOSTIF_DEVICE_IP_UPLOAD_API
 */

    int set_Device_IP_Diagnostics_UploadDiagnostics_DSCP(const char *, ParameterType, ParameterValue *);

/**
 * @brief This function sets the ethernet priority code for marking packets transmitted in the test (if applicable).
 *
 * @note The default value SHOULD be zero.
 *
 * @param[in] const char   Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the ethernet priority code.
 * @retval -1 If Not able to set the ethernet priority code.
 * @retval -2 If Not handle setting the ethernet priority code.
 * @ingroup TR69_HOSTIF_DEVICE_IP_UPLOAD_API
 */

    int set_Device_IP_Diagnostics_UploadDiagnostics_EthernetPriority(const char *, ParameterType, ParameterValue *);

/**
 * @brief This function sets the size (in bytes) of the file to be uploaded to the server.
 *
 * @note The CPE MUST insure the appropriate number of bytes are sent.
 *
 * @param[in] const char   Source IP address.
 * @param[in] ParameterType  Type of the valid interface.
 * @param[in] ParameterValue   Valid interface name or empty string.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the size of the file to be uploaded.
 * @retval -1 If Not able to set the size of the file to be uploaded.
 * @retval -2 If Not handle setting the size of the file to be uploaded.
 * @ingroup TR69_HOSTIF_DEVICE_IP_UPLOAD_API
 */

    int set_Device_IP_Diagnostics_UploadDiagnostics_TestFileLength(const char *, ParameterType, ParameterValue *);

    /* End of TR_069_DEVICE_IP_DIAGNOSTICS_UPLOADDIAGNOSTICS_SETTER_API doxygen group */
    /**
     * @}
     */

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_IP_DIAGNOSTICS_UPLOADDIAGNOSTICS_H_ */


/** @} */
/** @} */
