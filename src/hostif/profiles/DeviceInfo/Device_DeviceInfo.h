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
 * @file Device_DeviceInfo.h
 * @brief The header file provides TR69 device information APIs.
 */

/**
 * @defgroup TR69_HOSTIF_DEVICEINFO TR-069 Object (Device.DeviceInfo)
 *
 *  TR069 is a technical specification entitled CPE WAN Management Protocol (CWMP). It
 *  defines an application layer protocol for remote management of end-user devices. As
 *  a bidirectional SOAP/HTTP-based protocol, it provides the communication between
 *  customer-premises equipment (CPE) and Auto Configuration Servers (ACS).
 *
 *  The TR-069 Client API lists the TR-069 APIs that will be available for Xi3 client
 *  applications and those that need to be implemented at the stub layer.
 * @ingroup TR69_HOSTIF_PROFILE
 *
 * @defgroup TR69_HOSTIF_PROFILE TR-069 Host Interface Profile
 * @par Trace Route Diagnostics - TR-069 profile
 *
 * Tracing the routing path to a given destination machine is very important diagnostic tool
 * for administrators and service providers.Trace route utility is provided in many operating
 * systems today. Its main purpose is to find out the route IP packets take to reach a specific
 * destination. It gives indication on the routers in between and round trip time of each probe.
 * This utility sends UDP, TCP or ICMP probes with small TTL and listen for ICMP 'time exceeded'
 * reply. It starts with TTL 1 to find out the first hop, TTL 2 to find out the second hop and
 * so on.
 *
 * @par Trace route utility provides options such as:
 * - Destination Host IP address or FQDN.
 * - Use ICMP, UDP or TCP.
 * - Use IPv4 or IPv6
 * - Packet size : Valid in case of ICMP and UDP. Not applicable if TCP is chosen. TCP probe always
 * goes with SYN flag.
 * - Port: Port number to use for destination port of the UDP or TCP probe. In case of ICMP, it is
 * used as sequence number.
 * - TOS: TOS value to use in the IP header of probe packet.
 * - Link interface to use: Utility uses IP address of this link as source IP of the probe packet.
 * if not specified, default is to use link determined by route.
 * - Gateway IP address: This is mainly to select the right link, if there are multiple links on the host.
 * Default is 'based on route'.
 * - Maximum TTL: Indicates the maximum number of hops to discover. Default : 30
 * - Number of Probes: Number of probe messages to each hop. Default 3.
 * - Wait time: wait time to wait for the response to probe. Default : 5 seconds.
 * - Send interval: Interval between probe messages to a hop: Default 0 seconds.
 *
 * @ingroup TR69_HOSTIF
 * @defgroup TR69_HOSTIF_DEVICEINFO_IF TR-069 Object (Device.DeviceInfo.Interface)
 * This object contains general device information such as
 * @ingroup TR69_HOSTIF_DEVICEINFO
 *
 * @defgroup TR69_HOSTIF_DEVICEINFO_API TR-069 Object (Device.DeviceInfo Interface.{i}) Public APIs
 * Describe the details about RDK TR69 DeviceInfo APIs specifications.
 * @ingroup TR69_HOSTIF_DEVICEINFO_IF
 *
 * @defgroup TR69_HOSTIF_DEVICEINFO_CLASSES TR-069 Object (Device.DeviceInfo Interface.{i}) Public Classes
 * Describe the details about classes used in TR69 DeviceInfo.
 * @ingroup TR69_HOSTIF_DEVICEINFO_IF
 */


/** @defgroup TR_069_API TR-069 HAL API
 *  @ingroup COMCAST_RDK_HAL
 *
 *  TR069 is a technical specification entitled CPE WAN Management Protocol (CWMP).  It
 *  defines an application layer protocol for remote management of end-user devices.  As
 *  a bidirectional SOAP/HTTP-based protocol, it provides the communication between
 *  customer-premises equipment (CPE) and Auto Configuration Servers (ACS).
 *
 *  The TR-069 Client API lists the TR-069 APIs that will be available for Xi3 client
 *  applications and those that need to be implemented at the stub layer.
 *
 */



/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#include <string>
#ifndef DEVICE_DEVICEINFO_H_
#define DEVICE_DEVICEINFO_H_

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/
#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"

#ifndef NEW_HTTP_SERVER_DISABLE
#include "XrdkCentralComRFCStore.h"
#include "XrdkCentralComRFC.h"
#else
#include "XrdkCentralComRFC.h"
#endif
#include "XrdkCentralComBSStore.h"
#include "hostIf_updateHandler.h"

#define CDL_FLASH_FILE "/opt/cdl_flashed_file_name"
#define CURENT_FW_FILE "/tmp/currently_running_image_name"
#define LOG_UPLOAD_SCR	"/lib/rdk/UploadLogsNow.sh"
#define CURRENT_LOG_UPLOAD_STATUS "/opt/loguploadstatus.txt"

#define xOpsDMUploadLogsNow_STR 	"Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMUploadLogsNow"
#define xOpsDMLogsUploadStatus_STR 	"Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMLogsUploadStatus"



#define xOpsDMMoCALogEnabled 	"Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMMoCALogEnabled"
#define xOpsDMMoCALogPeriod 	"Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMMoCALogPeriod"

#define xOpsReverseSshTrigger_STR "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshTrigger"
#define xOpsReverseSshArgs_STR "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshArgs"
#define xOpsReverseSshStatus_STR "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshStatus"

#define xOpsForwardSshEnable_STR "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ForwardSSH.Enable"

#define XRDK_BOOT_TIME "Device.DeviceInfo.X_RDKCENTRAL-COM_BootTime"

#define xOpsRPCRebootNow "Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootNow"

#define TR181_RFC_PREFIX	"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC"
#define TR181_RFC_RESET_DATA_START     "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ClearDB"
#define TR181_RFC_RESET_DATA_END		"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ClearDBEnd"
#define HDR_DV_RFC_ENABLE	"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.DolbyVision.Enable"


#define RFC_WL_ROAM_TRIGGER_RF	"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RoamTrigger"
#define MS12_DAPV2_RFC_ENABLE	"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MS12.DAPv2_Enable"
#define MS12_DE_RFC_ENABLE	"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.MS12.DE_Enable"
#define LE_RFC_ENABLE   "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LoudnessEquivalence.Enable"
#define HDR_RFC_ENABLE	"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.HdrEnable.Enable"
#define UHD_RFC_ENABLE	"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.UhdEnable.Enable"
#define XRE_CONTAINER_RFC_ENABLE "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.LXC.XRE.Enable"

#define RFC_CTL_RETRIEVE_NOW	"Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.RetrieveNow"

#define xFirmwareDownloadNow_STR	"Device.DeviceInfo.X_RDKCENTRAL-COM_FirmwareDownloadNow" /* boolean [w] Trigger download on set true. */

#define IPREMOTE_SUPPORT  "Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.Enable"
#define IPREMOTE_IPADDRESS "Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.IPAddr"
#define IPREMOTE_MACADDRESS "Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.MACAddr"
#define XRPOLLING_ACTION  "Device.DeviceInfo.X_RDKCENTRAL-COM_XRPolling.Action"

/**
 * @brief This class provides the interface for getting device information.
 * @ingroup TR69_HOSTIF_DEVICEINFO_CLASSES
 */
class hostIf_DeviceInfo {

    static  GHashTable  *ifHash;

    static  GHashTable  *m_notifyHash;

    static GMutex *m_mutex;

    int dev_id;

    hostIf_DeviceInfo(int dev_id);

    ~hostIf_DeviceInfo();

    bool bCalledSoftwareVersion;
    bool bCalledX_COMCAST_COM_STB_IP;
    bool bCalledX_COMCAST_COM_FirmwareFilename;
    bool bCalledManufacturer;
    bool bCalledManufacturerOUI;
    bool bCalledModelName;
    bool bCalledHardwareVersion;
    bool bCalledDeviceMAC;
    bool bCalledSerialNumber;
    bool bCalledProductClass;
    bool bCalledAdditionalSoftwareVersion;

    char backupSoftwareVersion[_BUF_LEN_16];
    char backupX_COMCAST_COM_STB_IP[_BUF_LEN_64];
    char backupX_COMCAST_COM_FirmwareFilename[_BUF_LEN_64];
    char backupSerialNumber[_BUF_LEN_16];
    char backupProductClass[_BUF_LEN_16];
    char backupManufacturer[_BUF_LEN_16];
    char backupManufacturerOUI[_BUF_LEN_16];
    char backupModelName[_BUF_LEN_16];
    char backupHardwareVersion[_BUF_LEN_16];
    char backupDeviceMAC[_BUF_LEN_32];
    char backupAdditionalSoftwareVersion[_BUF_LEN_16];

    static string m_xFirmwareDownloadProtocol;
    static string m_xFirmwareDownloadURL;
    static string m_xFirmwareToDownload;
    static bool m_xFirmwareDownloadNow;
    static bool m_xFirmwareDownloadUseCodebig;

#ifndef NEW_HTTP_SERVER_DISABLE
    static XRFCStore *m_rfcStore;
    static XRFCStorage m_rfcStorage;
#else
    static XRFCStorage m_rfcStorage;
#endif
    static XBSStore *m_bsStore;
    static string m_xrPollingAction;

    string getEstbIp();
    bool isRsshactive();
    size_t findIgnoreCase (std::string haystack, std::string needle, size_t pos = 0);
    string getStbMacIf_fr_devProperties();

    int set_xRDKCentralComTelemetryRFCEnable(HOSTIF_MsgData_t *);
    int set_xRDKCentralComHdrRFCEnable(HOSTIF_MsgData_t *);
    int set_xRDKCentralComUhdRFCEnable(HOSTIF_MsgData_t *);
    int set_xRDKCentralComXREContainerRFCEnable(HOSTIF_MsgData_t *);

    // This is to set wl roam_trigger
    int set_xRDKCentralComRFCRoamTrigger(HOSTIF_MsgData_t *);

    int set_xRDKCentralComRFCRetrieveNow(HOSTIF_MsgData_t *);

public:

    static hostIf_DeviceInfo *getInstance(int dev_id);

    static void closeInstance(hostIf_DeviceInfo *);

    static GList* getAllInstances();

    static void closeAllInstances();

    static void getLock();

    static void releaseLock();

    GHashTable* getNotifyHash();

    /**
    * Description. This is the getter api for DeviceInfo for
    *  Device.DeviceInfo Profile.
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
    * calls. It should probably just a device inventory message from the platform.
    *
    * @see XXX_SomeOtherFunction.
    */

    /**
     * @brief get_Device_DeviceInfo_Manufacturer.
     *
     * This function provides the manufacture information.
     * The manufacturer of the CPE (human readable string).
     *
     * @return The status of the operation.
     *
     * @retval OK if DeviceInfo_Manufacturer was successfully fetched.
     :1
    * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     * @see get_Device_DeviceInfo_ManufacturerOUI.
     */
    int get_Device_DeviceInfo_Manufacturer(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_Manufacturer.
     *
     * This function provides the manufactureOUT information.
     *
     * Organizationally unique identifier of the device manufacturer. Represented
     * as a six hexadecimal-digit value using all upper-case letters and
     * including any leading zeros. Possible patterns:
     *
     *     [0-9A-F]{6}
     *
     *     The value MUST be a valid OUI as defined in [OUI].
     *
     * This value MUST remain fixed over the lifetime of the device, including across
     * firmware updates. Any change would indicate that it's a new device and would
     * therefore require a BOOTSTRAP Inform.
     *
     * @return The status of the operation.
     *
     * @retval OK if ManufacturerOUI was successfully fetched.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     * @see get_Device_DeviceInfo_Manufacturer.
     */
    int get_Device_DeviceInfo_ManufacturerOUI(HOSTIF_MsgData_t *, bool *pChanged = NULL);


    /**
     * @brief get_Device_DeviceInfo_ModelName.
     *
     * This function provides the Model name of the CPE (human readable string)
     * Device.
     * This MUST be based on Comcast_X_HW* specification and of the format TUVVVWXY.
     *
     * @return The status of the operation.
     *
     * @retval OK if ManufacturerOUI was successfully fetched.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     * @see get_Device_DeviceInfo_Manufacturer.
     */
    int get_Device_DeviceInfo_ModelName(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_Description.
     *
     * This function provides the A full description of the CPE device
     * (human readable string).
     * Device. *
     *
     * @return The status of the operation.
     *
     * @retval OK if Description is successfully fetched.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     * @see get_Device_DeviceInfo_Description.
     */
    int get_Device_DeviceInfo_Description(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_ProductClass.
     *
     * This function provides the Identifier of the class of product for which
     * the serial number applies. That is, for a given manufacturer,
     * this parameter is used to identify the product or class of product
     * over which the SerialNumber parameter is unique.
     *
     * This value MUST remain fixed over the lifetime of the device, including
     * across firmware updates. Any change would indicate that it's a new device
     * and would therefore require a BOOTSTRAP Inform.
     *
     * @return The status of the operation.
     *
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     * @see get_Device_DeviceInfo_Description.
     */
    int get_Device_DeviceInfo_ProductClass(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_SerialNumber.
     *
     * This function provides the Identifier of the particular device that is
     * unique for the indicated class of product and manufacturer.
     * This is the Serial Number of the box.
     *
     * This value MUST remain fixed over the lifetime of the device, including
     * across firmware updates. Any change would indicate that it's a new device
     *  and would therefore require a BOOTSTRAP Inform.
     *
     * @return The status of the operation.
     *
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     * @see get_Device_DeviceInfo_ProductClass.
     */
    int get_Device_DeviceInfo_SerialNumber(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_HardwareVersion.
     *
     * This function identifying the particular CPE model and version.
     * This MUST be based on Comcast_X_HW* specification and of the format VM.m.R.
     *
     * @return The status of the operation.
     *
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     * @see get_Device_DeviceInfo_ProductClass.
     */
    int get_Device_DeviceInfo_HardwareVersion(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_SoftwareVersion.
     *
     * This function identifying the Software/Firmware version of the running
     * image on the box (Vx.y.z)
     *
     * A string identifying the software version currently installed in the CPE
     * (i.e. version of the overall CPE firmware).
     *
     * To allow version comparisons, this element SHOULD be in the form of
     * dot-delimited integers, where each successive integer represents a more
     * minor category of variation.
     * For example, 3.0.21 where the components mean: Major.Minor.Build.
     *
     * @return The status of the operation.
     *
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     * @see get_Device_DeviceInfo_ProductClass.
     */
    int get_Device_DeviceInfo_SoftwareVersion(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_AdditionalHardwareVersion.
     *
     * This function identifying any additional CPE model and version.
     * This MUST be based on Comcast_X_HW* specification and of the format VM.m.R.
     *
     * @return The status of the operation.
     *
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     * @see get_Device_DeviceInfo_ProductClass.
     */
    int get_Device_DeviceInfo_AdditionalHardwareVersion(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_AdditionalSoftwareVersion.
     *
     * This function identifying any additional Software/Firmware version of the running
     * image on the box (Vx.y.z)
     *
     * A string identifying the software version currently installed in the CPE
     * (i.e. version of the overall CPE firmware).
     *
     * To allow version comparisons, this element SHOULD be in the form of
     * dot-delimited integers, where each successive integer represents a more
     * minor category of variation.
     * For example, 3.0.21 where the components mean: Major.Minor.Build.
     *
     * @return The status of the operation.
     *
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     * @see get_Device_DeviceInfo_ProductClass.
     */
    int get_Device_DeviceInfo_AdditionalSoftwareVersion(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_ProvisioningCode.
     *
     * This is an Identifier of the primary service provider and other provisioning
     *  information, which MAY be used by the ACS to determine service
     *  provider-specific customization and provisioning parameters.
     *
     * @return The status of the operation.
     *
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     * @see get_Device_DeviceInfo_ProductClass.
     */
    int get_Device_DeviceInfo_ProvisioningCode(HOSTIF_MsgData_t *, bool *pChanged = NULL);


    /**
     * @brief get_Device_DeviceInfo_UpTime.
     *
     * This is an Identifier of time in seconds since the CPE was last restarted.
     *
     * @return The status of the operation.
     *
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_UpTime(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_FirstUseDate.
     *
     * This is Date and time in UTC that the CPE first both successfully established
     *  an IP-layer network connection and acquired an absolute time reference
     *  using NTP or equivalent over that network connection.
     * The CPE MAY reset this date after a factory reset.
     *
     * If NTP or equivalent is not available, this parameter, if present,
     * SHOULD be set to the Unknown Time value.
     *
     * @return The status of the operation.
     *
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_FirstUseDate(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_X_COMCAST_COM_STB_MAC.
     *
     * This is the MAC Address of the interface.
     * This is currently the mac address of the eth1 interface.
     *
     * @return The status of the operation.
     *
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_X_COMCAST_COM_STB_MAC(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_X_COMCAST_COM_STB_IP.
     *
     * This is the IPv4 address of the interface.
     * This is currently the ip address of the eth1 interface.
     *
     * @return The status of the operation.
     *
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_X_COMCAST_COM_STB_IP(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareFilename.
     *
     * Filename of the firmware currently running on the device.
     *
     * @return The Filename of the firmware currently running on the device.
     *
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareFilename(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareToDownload.
     *
     * Filename of the firmware that the device was requested to download most recently..
     *
     * @return The Filename of the firmware that was recently downloaded.
     *
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareToDownload(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadStatus.
     *
     * Download status of the firmware file when the download is initiated via XCONF or TR-069 ACS.
     *
     * @return The Download status of the firmware file.
     *                            - Idle(default).
     *                             - Initiated.
     *                            - InProgress.
     *                            - Completed.
     *                            - Successful.
     *                            - Error.
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadStatus(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadProtocol.
     *
     * Download protocal used for the firmware file when the download is initiated via XCONF or TR-069 ACS.
     *
     * @return The Download protocol of the firmware file.
     *                            - http
     *                            - https
     *                            - ftp
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadProtocol(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadURL.
     *
     * Download url used for the download when initiated via XCONF or TR-069 ACS.
     *
     * @return The Download url of the firmware file.
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadURL (HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadUseCodebig.
     *
     * Get the Codebig flag which is already set via TR-069 ACS.
     *
     * @return The Codebig flag value either '0' or '1' based on the value already set
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadUseCodebig(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    int get_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadPercent (HOSTIF_MsgData_t *, bool *pChanged = NULL);

    int get_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareUpdateState (HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset.
     *
     * Returns an empty string.
     *
     * @return The status of the operation.
     *
     * @retval OK if it is successful.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    int get_Device_DeviceInfo_MemoryStatus_Total(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    int get_Device_DeviceInfo_MemoryStatus_Free(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    int get_Device_DeviceInfo_X_RDKCENTRAL_COM_CPUTemp(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief set_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset.
     *
     * Setting this parameter results in a reset being performed on the device.
     * The level of reset performed is defined by the value written into this parameter.
     * Enumeration of:
     *        Cold
     *        Factory
     *        Warehouse
     *        Customer
     *
     * @return The status of the operation. When read, this parameter returns an empty string.
     *
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int set_Device_DeviceInfo_X_RDKCENTRAL_COM_Reset(HOSTIF_MsgData_t*);

    /**
     * @brief get_Device_DeviceInfo_VendorConfigFileNumberOfEntries.
     *
     * This is the number of entries in the VendorConfigFile table.
     *
     * @return The status of the operation.
     *
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */

    int get_Device_DeviceInfo_VendorConfigFileNumberOfEntries(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_SupportedDataModelNumberOfEntries.
     *
     * This is the number of entries in the SupportedDataModel table.
     *
     * @return The status of the operation.
     *
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_SupportedDataModelNumberOfEntries(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief get_Device_DeviceInfo_ProcessorNumberOfEntries.
     *
     * The number of entries in the Processor table.
     *
     * @return The status of the operation.
     *
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    static int get_Device_DeviceInfo_ProcessorNumberOfEntries(HOSTIF_MsgData_t *);

    /**
     * @brief get_Device_DeviceInfo_VendorLogFileNumberOfEntries.
     *
     * The number of entries in the VendorLogFile table.
     *
     * @return The status of the operation.
     *
     * @retval OK if it is successful.
     * @retval ERR_INTERNAL_ERROR if not able to fetch from device.
     *
     * @sideeffect All necessary structures and buffers are deallocated.
     * @execution Synchronous.
     *
     */
    int get_Device_DeviceInfo_VendorLogFileNumberOfEntries(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
    * @brief get_Device_DeviceInfo_X_COMCAST_COM_PowerStatus.
    *
    * The X_COMCAST_COM_PowerStatus as get parameter results in the power status
    * being performed on the device. Power status of the device based on the front panel power LED.
    * Enumeration of:
    *        PowerOn
    *        Standby
    *        PowerOff
    *
    * @return The status of the operation. When read, this parameter returns an enumeration string.
    *
    * @retval OK if it is successful.
    * @retval NOK if not able to fetch from device.
    *
    * @sideeffect All necessary structures and buffers are deallocated.
    * @execution Synchronous.
    *
    */
    int get_Device_DeviceInfo_X_COMCAST_COM_PowerStatus(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
    * @brief get_Device_DeviceInfo_X_RDKCENTRAL_COM_BootStatus.
    *
    * This method is updated with the boot status of the device at the time of query.
    * If the device is currently starting up, this parameter will hold boot status as per the boot sequence of the device.
    *
    * BootStatus string - Boot status for an STB CPE via TR-069 ACS when powered On.
    * Enumeration of :
    *  			Coax connection confirmed, MoCA enabled
    *  			Discovering MoCA Network Coordinator: MoCA MAC: xx:xx:xx:xx:xx:xx
    *  			Joining MoCA Network
    *  			Connection successful
    *  			Acquiring IP Address from Gateway
    *  			Contacting ACS
    *  			Contacting XRE
    *
    * @return The status of the operation. When read, this parameter returns an enumeration string.
    *
    * @retval OK if it is successful.
    * @retval NOK if not able to fetch from device.
    */
    int get_Device_DeviceInfo_X_RDKCENTRAL_COM_BootStatus(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
        * @brief get_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType.
        *
        * This method is uget the preferred gateway of the client
        *
        * @retval OK if it is successful.
        * @retval NOK if not able to fetch from device.
        */

    int get_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    int get_X_RDKCENTRAL_COM_BootTime(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
    * @brief get_xOpsDMLogsUploadStatus.
    *
    * This method is to Initiate Log upload to cloud server
    * with following TR-069 definition:
    *	Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMUploadLogsNow
    *	Data type: boolean - Setting this parameter to true will initiate Log upload.
    *
    * @retval OK if it is successful.
    * @retval NOK if not able to fetch from device.
    *
    */
    int get_xOpsDMUploadLogsNow(HOSTIF_MsgData_t *);

    /**
    * @brief get_xOpsDMLogsUploadStatus.
    *
    * This method is to Read back upload status:
    * Parameter Name : Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMLogsUploadStatus
    * @retval OK if it is successful.
    * @retval NOK if not able to fetch from device.
    * 	Result is reported in form of string: <Result> <TimesStamp>
    * 	Valid values for <Result> are:
    *           Not triggered
    *			Triggered
    *			In progress
    *			Failed
    *			Complete
    *  <TimeStamp> is expressed in format of Linux date output
    */
    int get_xOpsDMLogsUploadStatus(HOSTIF_MsgData_t *);
    /**
    * @brief get_xOpsDMMoCALogEnabled.
    *
    * This method is to get  moca telemetry log status
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMMoCALogEnabled
    *   Data type: boolean - getting this parameter and if the value is true then  moca telemetry logging is enabled
    *
    * @retval OK if it is successful.
    * @retval NOK if not able to fetch from device.
    */
    int get_xOpsDMMoCALogEnabled(HOSTIF_MsgData_t *);

    /**
    * @brief get_xOpsDMMoCALogPeriod.
    *
    * This method is to get the MoCA logging period
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMMoCALogPeriod
    *   Data type: integer - getting the value for moca telemetry logging period
    *
    * @retval OK if it is successful.
    * @retval NOK if not able to fetch from device.
    */
    int get_xOpsDMMoCALogPeriod(HOSTIF_MsgData_t *);
    /**
     * Helper methods
     */
    int set_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareToDownload(HOSTIF_MsgData_t *);
    int set_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadStatus(HOSTIF_MsgData_t *);
    int set_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadProtocol(HOSTIF_MsgData_t *);
    int set_Device_DeviceInfo_X_COMCAST_COM_FirmwareDownloadURL (HOSTIF_MsgData_t *);
    int set_Device_DeviceInfo_X_RDKCENTRAL_COM_PreferredGatewayType(HOSTIF_MsgData_t *);
    int set_Device_DeviceInfo_X_RDKCENTRAL_COM_FirmwareDownloadUseCodebig(HOSTIF_MsgData_t *);

    /**
    * @brief set_xOpsDMUploadLogsNow.
    *
    * This method is to Initiate Log upload to cloud server
    * with following TR-069 definition:
    *	Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMUploadLogsNow
    *	Data type: boolean - Setting this parameter to true will initiate Log upload.
    *
    * @retval OK if it is successful.
    * @retval NOK if not able to fetch from device.
    */
    int set_xOpsDMUploadLogsNow(HOSTIF_MsgData_t *);
    /**
    * @brief set_xOpsDMMoCALogEnabled.
    *
    * This method is to enable or disable moca telemetry log
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMMoCALogEnabled
    *   Data type: boolean - Setting this parameter to true will initiate moca telemetry logging
    *
    * @retval OK if it is successful.
    * @retval NOK if not able to fetch from device.
    */
    int set_xOpsDMMoCALogEnabled(HOSTIF_MsgData_t *);

    /**
    * @brief set_xOpsDMMoCALogPeriod.
    *
    * This method is to get the MoCA logging period
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.Logging.xOpsDMMoCALogPeriod
    *   Data type: integer - Setting the value for moca telemetry logging period
    *
    * @retval OK if it is successful.
    * @retval NOK if not able to fetch from device.
    */
    int set_xOpsDMMoCALogPeriod(HOSTIF_MsgData_t *);
    /**
    * @brief set_xOpsReverseSshTrigger
    *
    * This method is to enable or disable reverse ssh tunnel. This uses the arguments set using set_xOpsReverseSshArgs.
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshTrigger
    *   Data type: boolean - Enable or disable the tunnel.
    *
    * @retval OK if it is successful.
    * @retval NOK if operation fails.
    */
    int set_xOpsReverseSshTrigger(HOSTIF_MsgData_t *);
    /**
    * @brief set_xOpsReverseSshArgs
    *
    * This method is to set arguments for reverse ssh tunnel.
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshArgs
    *   Data type: string - Arguments for ssh tunnel.
    *                       example format: user=test;host=192.168.111.11;port=33321;idletime=30;
    *
    * @retval OK if it is successful.
    * @retval NOK if operation fails.
    */
    int set_xOpsReverseSshArgs(HOSTIF_MsgData_t *);
    /**
    * @brief get_xOpsReverseSshArgs
    *
    * This method is to get arguments for reverse ssh tunnel.
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshArgs
    *   Data type: string - Arguments for ssh tunnel.
    *                       example format: user=test;host=192.168.111.11;port=33321;idletime=30;
    *
    * @retval OK if it is successful.
    * @retval NOK if operation fails.
    */
    int get_xOpsReverseSshArgs(HOSTIF_MsgData_t *);
    /**
    * @brief get_xOpsReverseSshStatus
    *
    * This method is to get status of ssh session
    * Result is reported in string(ACTIVE,INACTIVE).
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.ReverseSSH.xOpsReverseSshStatus
    *   Data type: string - Arguments for ssh tunnel.
    *                       example format: user=test;host=192.168.111.11;port=33321;idletime=30;
    *
    * @retval OK if it is successful.
    * @retval NOK if operation fails.
    */
    int get_xOpsReverseSshStatus(HOSTIF_MsgData_t *);

    /**
    * @brief set_xOpsDeviceMgmtRPCRebootNow
    *
    * This method is to trigger an immediate reboot of the box.
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.RPC.RebootNow
    *   Data type: boolean - Setting this parameter to true will immediately reboot the box.
    *
    * @retval OK if it is successful.
    * @retval NOK if operation fails.
    */
    int set_xOpsDeviceMgmtRPCRebootNow (HOSTIF_MsgData_t *);

    int get_xOpsDeviceMgmtForwardSSHEnable (HOSTIF_MsgData_t *);

    int set_xOpsDeviceMgmtForwardSSHEnable (HOSTIF_MsgData_t *);

    int get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportEnable(HOSTIF_MsgData_t *, bool *pChanged = NULL);
    int set_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportEnable(HOSTIF_MsgData_t *);

   /*
    * @brief get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportIpaddress
    *
    * This method is to get the ipremote interface ipaddress.
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.IPAddr
    *   Data type: String - Arguments Ipaddress
    *
    * @retval OK if it is successful.
    * @retval NOK if operation fails.
    */

    int get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportIpaddress(HOSTIF_MsgData_t *);

   /*
    * @brief get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportMACaddress
    *
    * This method is to get the ipremote interface MAC address.
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_IPRemoteSupport.MACAddr
    *   Data type: String - Arguments MACaddress
    *
    * @retval OK if it is successful.
    * @retval NOK if operation fails.
    */

    int get_Device_DeviceInfo_X_RDKCENTRAL_COM_IPRemoteSupportMACaddress(HOSTIF_MsgData_t *);

    int get_Device_DeviceInfo_X_RDKCENTRAL_COM_XRPollingAction(HOSTIF_MsgData_t *, bool *pChanged = NULL);
    int set_Device_DeviceInfo_X_RDKCENTRAL_COM_XRPollingAction(HOSTIF_MsgData_t *);

#ifdef USE_HWSELFTEST_PROFILE
    /**
    * @brief set_xOpsDeviceMgmt_hwHealthTest_Enable
    *
    * This method is used to enable/disable the hardware health test functionality.
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.hwHealthTest.Enable
    *   Data type: boolean - Enable (True)/disable (False) health test functionality.
    *
    * @retval OK if it is successful.
    * @retval NOK if operation fails.
    */
    int set_xOpsDeviceMgmt_hwHealthTest_Enable(HOSTIF_MsgData_t *);

    /**
    * @brief set_xOpsDeviceMgmt_hwHealthTest_ExecuteTest
    *
    * This method is used to trigger hardware health test on the STB.
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.hwHealthTest.ExecuteTest
    *   Data type: integer - Unused.
    *
    * @retval OK if it is successful.
    * @retval NOK if operation fails.
    */
    int set_xOpsDeviceMgmt_hwHealthTest_ExecuteTest(HOSTIF_MsgData_t *);

    /**
    * @brief set_xOpsDeviceMgmt_hwHealthTest_Results
    *
    * This method is used to retrieve the most recent hardware health test results.
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.hwHealthTest.Results
    *
    * @retval OK if it is successful.
    * @retval NOK if operation fails.
    */
    int get_xOpsDeviceMgmt_hwHealthTest_Results(HOSTIF_MsgData_t *);

    /**
    * @brief set_xOpsDeviceMgmt_hwHealthTest_SetTuneType
    *
    * This method is used to start tune tests by using a particular tune type.
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.hwHealthTest.SetTuneType
    *   Data type: integer - Type of tune data to set.
    *
    * @retval OK if it is successful.
    * @retval NOK if operation fails.
    */
    int set_xOpsDeviceMgmt_hwHealthTest_SetTuneType(HOSTIF_MsgData_t *);

    /**
    * @brief set_xOpsDeviceMgmt_hwHealthTest_ExecuteTuneTest
    *
    * This method is used to perform the tune testing based on the tune type.
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.hwHealthTest.ExecuteTuneTest
    *   Data type: string - json format of string with tune related data.
    *
    * @retval OK if it is successful.
    * @retval NOK if operation fails.
    */    
    int set_xOpsDeviceMgmt_hwHealthTest_ExecuteTuneTest(HOSTIF_MsgData_t *);

    /**
    * @brief set_xOpsDeviceMgmt_hwHealthTest_TuneResults
    *
    * This method is used to retrieve the most recent hardware health Tune test results.
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.hwHealthTestTune.TuneResults
    *
    * @retval OK if it is successful.
    * @retval NOK if operation fails.
    */
    int get_xOpsDeviceMgmt_hwHealthTestTune_TuneResults(HOSTIF_MsgData_t *);

    /**
    * @brief set_xOpsDeviceMgmt_hwHealthTest_EnablePeriodicRun
    *
    * This method is used to enable/disable the hardware health test periodic run functionality.
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.hwHealthTest.EnablePeriodicRun
    *   Data type: boolean - Enable (True)/disable (False) health test periodic run functionality.
    *
    * @retval OK if it is successful.
    * @retval NOK if operation fails.
    */
    int set_xOpsDeviceMgmt_hwHealthTest_EnablePeriodicRun(HOSTIF_MsgData_t *);

    /**
    * @brief set_xOpsDeviceMgmt_hwHealthTest_PeriodicRunFrequency
    *
    * This method is used to set the hardware health test periodic run frequency.
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.hwHealthTest.PeriodicRunFrequency
    *   Data type: unsigned int - Periodic run frequency to set (in minutes), 0 = default frequency.
    *
    * @retval OK if it is successful.
    * @retval NOK if operation fails.
    */
    int set_xOpsDeviceMgmt_hwHealthTest_PeriodicRunFrequency(HOSTIF_MsgData_t *);

    /**
    * @brief set_xOpsDeviceMgmt_hwHealthTest_cpuThreshold
    *
    * This method is used to set the hardware health test periodic run CPU usage threshold.
    * If CPU usage is higher than the threshold set, periodic health test will not execute.
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.hwHealthTest.cpuThreshold
    *   Data type: unsigned int - CPU threshold to set (in percent).
    *
    * @retval OK if it is successful.
    * @retval NOK if operation fails.
    */
    int set_xOpsDeviceMgmt_hwHealthTest_CpuThreshold(HOSTIF_MsgData_t *);

    /**
    * @brief set_xOpsDeviceMgmt_hwHealthTest_dramThreshold
    *
    * This method is used to set the hardware health test periodic run DRAM usage threshold.
    * If free DRAM memory is less than the threshold set, periodic health test will not execute.
    * with following TR-069 definition:
    *   Parameter Name: Device.DeviceInfo.X_RDKCENTRAL-COM_xOpsDeviceMgmt.hwHealthTest.dramThreshold
    *   Data type: unsigned int - DRAM threshold to set (in MB).
    *
    * @retval OK if it is successful.
    * @retval NOK if operation fails.
    */
    int set_xOpsDeviceMgmt_hwHealthTest_DramThreshold(HOSTIF_MsgData_t *);
#endif /* USE_HWSELFTEST_PROFILE */

    int validate_ParamValue(HOSTIF_MsgData_t *);

    int set_xRDKCentralComRFC(HOSTIF_MsgData_t *);
    int get_xRDKCentralComRFC(HOSTIF_MsgData_t *);

    int set_xRDKCentralComBootstrap(HOSTIF_MsgData_t *);
    int get_xRDKCentralComBootstrap(HOSTIF_MsgData_t *);

    int readFirmwareInfo(char *, HOSTIF_MsgData_t *);
    int writeFirmwareInfo(char *, HOSTIF_MsgData_t *);

    int set_xFirmwareDownloadNow(HOSTIF_MsgData_t *);

};
/* End of doxygen group */
/**
 * @}
 */

#endif /* DEVICE_DEVICEINFO_H_ */


/** @} */
/** @} */

