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
* @file
*
* @brief TR69 Bus Agent Public API.
*
*/

/**
 * @defgroup TR69 TR-069
 *
 * @par Terminology
 * Code        | Description
 * ------------|------------
 * ACS         | Auto-Configuration Server
 * CPE         | Customer Premises Equipment
 * CWMP        | CPE WAN Management Protocol
 * NAT         | Network Address Translation
 * STUN        | Simple Traversal of UDP through NATs
 * B-NT        | Broadband-Network Termination
 * DT Schema   | Device Type Schema
 * RPC         | Remote Procedure Call
 * DT Instance | Device Type Schema instance document
 * ACL         | Access control list
 * ADSL        | Asymmetric Digital Subscriber Line
 * AES         | Advanced Encryption Standard
 * ASCII       | American Standard Code for Information Interchange
 * ATA         | Analog terminal adapter
 * ATM         | Asynchronous Transfer Mode
 * BOOTP       | Boot Strap Protocol
 * CGI         | Common Gateway Interface
 * CN          | Common Name
 * CSRF        | Cross-site request forgery
 * DHCP        | Dynamic Host Configuration Protocol
 * DNS         | Domain Name System
 * DSL         | Digital Subscriber Line
 *
 * @par Introduction to TR-069
 * TR-069 is a technical specification that defines an application layer protocol for remote management of end-user devices.
 * It was published by the Broadband Forum and was entitled CPE WAN Management Protocol(CWMP).
 * The CWMP, published by The Broadband Forum as TR-069, specifies a standard
 * communication mechanism for the remote management of end-user devices. It defines a protocol for the secure
 * auto-configuration of a TR-069 device and incorporates other management functions into a common framework.
 * This protocol simplifies device management by specifying the use of an auto configuration server (ACS)
 * to perform remote, centralized management of customer premises equipment (CPE).
 * @n
 * TR-069 supports a variety of functionalities to manage CPEs and has the following primary capabilities:
 * - Auto-configuration and dynamic service provisioning
 * - Software/firmware management
 * - Status and performance monitoring
 * - Diagnostics
 *
 * @par RDK TR-069 Feature Summary
 * - Service Activation
 * - Firmware upgrade management
 * - Register device to Auto-Configuration Server (ACS) using Inform notification
 * - Periodically send device information to ACS using Inform notification
 * - Forced Inform for required parameters
 * - Allow ACS to configure periodic Inform interval.
 * - Allow ACS to reach devices that are connected behind NAT using STUN + UDP NAT traversal
 * - (future) Allow ACS to reach devices that are connected behind NAT using XMPP
 * - Retrieve device diagnostics/parameters using GetParameterValues() method
 * - Set device parameters using SetParameterValues() method
 * - Factory reset using FactoryReset method
 * - Device reboot using Reboot method
 * - SSL/TLS 1.2 (Transport Layer Security) per RFC5246 to encrypt communications with ACS
 *
 * @par What is an ACS and why is it necessary?
 * - TR-069 specifies communication between customer-premises equipment (CPE) and an auto configuration server.
 * - The auto configuration server acts as a management server for TR-069-enabled CPE.
 * - It is essentially the link between the subscriber's devices in the home and the service provider's customer service
 * representative (CSR), support staff, operational support systems and business support systems (OSS/BSS).
 * - An auto configuration server enables you to automate provisioning and many management tasks for TR-069 devices,
 * facilitating remote management.
 *
 * @par How does TR-069 work?
 * TR-069 is a SOAP(Simple Object Access Protocol)/HTTP-based protocol. Orders are sent between the device (CPE)
 * and an auto configuration server over HTTP or HTTPS in the form of remote procedure calls (RPCs) and responses,
 * with SOAP acting as the encoding syntax to transport RPCs. The CPE acts as the HTTP client and the ACS acts as
 * the HTTP server.
 * @n
 * The basic network elements required include:
 * @n
 * - An auto configuration server (ACS): The management server on the network.
 * - Customer premises equipment (CPE): The device that is managed on the network
 * - DNS server: Used to resolve the URL that is required for the ACS and CPE to interact
 * - DHCP server:  Can be used to assign an IP address to a device on the network. Well-known DHCP options
 * can configure important parameters on the CPE, such as the ACS URL.
 * - TR-069 provides the communication interface between CPE and Auto Configuration Server (ACS)
 * - An ACS can manage a device, trouble shoot, configure and upgrade. the information.
 * @n
 * @image html tr69_overview.png
 * TR-069 closely monitor and control the Customer Premises Equipment (CPE). In RDK it would be Settop box or Gateway.
 * - The Auto Configuration Server (ACS) is located in the cloud which control the Customer Premises Equipment.
 * - ACS server is controlled by Operation/Support team.
 * - There are different kinds of data model used in customer premises, such as TR-135 data model is used in STB,
 *  TR-104 data model is used in VoIP, TR-140 data module is used in storage, and so on.
 *
 *  @par What is a remote procedure call (RPC)?
 * A remote procedure call (RPC) is an operation between an ACS and the CPE. It is used for bidirectional communication between
 * CPE and an ACS. Some common RPCs include:
 * - @b GetParameterValue: The ACS uses this RPC to get the value of one or more parameters of a CPE
 * - @b SetParameterValue: The ACS sets the value of one or more parameters of a CPE
 * - @b Inform: A CPE sends this message to an ACS to initiate a session and to periodically send local information
 * - @b Download: When the ACS requires a CPE to download a specified file to upgrade hardware and download a configuration file
 * - @b Upload: When the ACS requires a CPE to upload a specified file to a specified location
 * - @b Reboot: An ACS reboots a CPE remotely when the CPE encounters a failure or needs a software upgrade
 * - <b> Add Object</b>: Allows the ACS to create instances of objects available on the CPE, for example, port mapping entries.
 * The ACS also creates the associated parameters and sub-objects.
 * - <b>Delete Object</b>: Enables the ACS to delete existing instances of objects available on the CPE.
 * It also deletes the associated parameters and sub-objects.
 *
 * @par Why RDK use TR-069?
 * There are many benefits associated with using TR-069 to provision and manage end-user devices.
 * TR-069 and its extensions allow you to:
 * - Enable remote provisioning of CPE devices such as Xi3, XG1v3, etc.
 * - Better manage broadband networks with increased visibility and control of CPE.
 * - Collect data for analytics on network usage and activity, home network characteristics, and service utilization.
 * - Deliver new managed data services such as WiFi, content filtering and other parental controls, online backup,
 * and home surveillance.
 * - Offer subscribers a degree of self-service through web portals.
 * - Expand service offerings and manage the connected home.
 * - Improve your customer service with improved diagnostics, monitoring, and firmware management.
 * - Reduce support calls and remove the burden of CPE configuration from subscribers and roll-out services
 * with an automated process.
 *
 * @par TR-069 Setup
 * @image html tr69_intro.png
 *
 * @par TR-069 Message Flow:
 * @image html tr69_setup.png
 *
 * @par Features implemented in RDK IP Clients,
 * These are the following features implemented in RDK IP Clients.
 * @n
 * @li @ref TR69_FIRMWARE_DOWNLOAD
 * @li @ref TR69_PERIODIC_INFORMATION
 * @li @ref TR69_DEVICE_MANAGEMENT
 * @li @ref TR69_DIAGNOSTICS
 *
 * @defgroup TR69_FIRMWARE_DOWNLOAD TR-069 Firmware Download
 * @ingroup TR69
 * Firmware Download in NATed Clients
 * @n Currently Docsis based RDK STBs support the following CDL methods
 * - SNMP and bootfile triggered downloads
 * - TFTP based downloads
 * - However, IP client devices do not have a public IP address, and can only be reached through their Gateway STBs
 * - IP Client STBs hence rely on TR-069 and ACS directive for Code Download
 * - It can be triggered from CDL server just as in the case of gateways,
 * - The IP client device is connected to Gateway device and it is behind the XG1.
 * - Download RPC is used to trigger download to the IP Clients
 * @li The CDL server triggers the request to ACS based on the Serial and MOUI of IP Client device.
 * - The ACS selects the device based on the request from CDL server.
 * - Then ACS uses the download RPC to trigger the firmware download.
 *
 * @par Code Download Terminologies
 * @n
 * Term  | Definition
 * -----| -----------
 * ACS  | Auto-Configuration Server
 * Bootloader  | Refers to the first stage bootloader responsible for lunching the firmware image.
 * B-DRI       | Used in the event the disaster recovery image is corrupt. The capabilities are limited to ensuring that a secure download of the firmware image can take place.
 * P-DRI       | Disaster Recovery Image. Refers to a disaster recovery image that is launched if the firmware image is invalid due to corruption of other reasons. When the device is running an DRI image, then it is DRI mode. If a normal firmware image is running, then it is in normal mode.
 * OTP         | One time programmable memory.
 *
 * @n
 * @par Code Download - Example Decision Flow
 *
 * @image html tr69_cdl.png
 *
 * When firmware updates are required, the device needs to know where to go to get the firmware update file.
 * ACS calls "Download" method to download Firmware image to Device with the following arguments
 * - Command Key
 * - FileType
 * - URL
 * - Username
 * - Password
 * - FileSize
 * - TargetFileName
 * - DelaySeconds
 * - SuccessURL
 * - FailureURL
 * CPE uses the above information to trigger for firmware download.
 *
 * @defgroup TR69_PERIODIC_INFORMATION TR-069 Gathering Periodic Information
 * @ingroup TR69
 * Intended to check the heart beat of deployed IP client STB.
 * A mechanism to establish periodic communication with the ACS on an ongoing basis, or when events occurs that
 * must be reported to the ACS such as when the broadband IP address of the CPE changes.
 * The ACS must be aware of this event in order to establish incoming connections to the CPE.
 * @n - CPE periodically sends CPE information to the ACS using the inform method call.
 * @n - CPE attempts to connect with ACS and call the inform method if, the following parameter is set to True.
 * @li "Device.ManagementServer.PeriodicInformEnable"
 * @n - The periodic interval duration can be configured by the ACS using the parameter,
 * @li "Device.ManagementServer.PeriodicInformInterval"
 *
 * @defgroup TR69_DEVICE_MANAGEMENT TR-069 Device Management
 * @ingroup TR69
 * Following are the device management features implemented in RDK,
 * - Fetching of parameter values from IP client STB to ACS using @b GetParameterValues" RPC.
 * - Setting of parameter values to IP client STB from ACS using @b SetParameterValues RPC.
 * - Factory Reset to IP client STB from ACS using @b FactoryReset RPC.
 * - Reboot to IP client STB from ACS using @b Reboot RPC.
 *
 * @par Broadband Data Model Definition,
 * RDK supports the following data models,
 * @n
 * - IP client STB device related: tr-181-2-2-0 (http://www.broadband-forum.org/cwmp/tr-181-2-2-0.html)
 * @n Example:
 * Name | Description
 * -----| -----------
 * Device.DeviceInfo.SoftwareVersion | Software version currently installed in the CPE.
 * Device.DeviceInfo.HardwareVersion | Identifying the particular CPE model and version.
 * Device.DeviceInfo.UpTime          | Time in seconds since the CPE was last restarted.
 * Device.DeviceInfo.MemoryStatus.Total | Status of the device's volatile physical memory.
 * Device.DeviceInfo.MemoryStatus.Free | Status of the free physical RAM, in kilobytes, currently available on the device.
 * Device.DeviceInfo.TemperatureStatus | Status of the temperature of the device.
 * Device.ManagementServer. | This object contains parameters relating to the CPE's association with an ACS
 * Device.ManagementServer.EnableCWMP | Enables and disables the CPE's support for CWMP.
 * Device.ManagementServer.URL | For the CPE to connect to the ACS using the CPE WAN Management Protocol.
 * @n @n
 * - IP client STB service related: tr-135-1-2-0 (http://www.broadband-forum.org/cwmp/tr-135-1-0-0.html)
 * @n Example:
 * Name | Description
 * -----| -----------
 * STBService.{i}.Components.AudioOutput.{i}. | Audio output instance table.
 * STBService.{i}.Components.AudioOutput.{i}.AudioFormat | Currently active audio output format.
 * STBService.{i}.Components.AudioOutput.{i}.AudioLevel | The audio level on this audio output.
 * STBService.{i}.Components.VideoOutput.{i}. | Video output instance table.
 * STBService.{i}.Components.VideoOutput.{i}.VideoFormat | Currently active video output format
 * @n
 * - RDK IP client custom parameters
 * @li Example1: STBService.{i}.Components.X_RDKCENTRAL-COM_SDCard.Capacity
 * @li Example2: STBService.{i}.Components.X_RDKCENTRAL-COM_SDCard.Model
 *
 *
 * @defgroup TR69_DIAGNOSTICS TR-069 Diagnostics
 * When the diagnostic initiated by the ACS is completed successfully or not,
 * the CPE MUST establish a new connection to the ACS to allow the ACS to view the results, indicating the
 * Event code "8 DIAGNOSTICS COMPLETE" in the Inform message.
 * @n @n
 * After the diagnostic is complete, the value of all result parameters MUST be retained by the CPE until
 * either this diagnostic runs again, or the CPE reboots. After a reboot, if the CPE has not retained
 * the resultant parameters from the most recent test, it MUST set the value of this parameter to None.
 * @n @n
 * Modifying any of the writable parameters in this object except for this one MUST result in the value
 * of this parameter being set to None.
 * @n @n
 * While the test is in progress, modifying any of the writable parameters in this object except for this
 * one MUST result in the test being terminated and the value of this parameter being set to None.
 * @n @n
 * While the test is in progress, setting this parameter to Requested MUST result in the test being terminated
 * and then restarted using the current values of the test parameters.
 *
 * @par Diagnostics states,
 * If the ACS sets the value of this parameter to Requested, the CPE MUST initiate the
 * corresponding diagnostic test. When writing, the only allowed value is Requested.
 * To ensure the use of the proper test parameters i.e the writable parameters in this object,
 * the test parameters MUST be set either prior to or at the same time as in the same SetParameterValues
 * setting the DiagnosticsState to Requested.
 * @n @n
 * When requested, the CPE SHOULD wait until after completion of the communication session with the
 * ACS before starting the diagnostic. When the test is completed, the value of this parameter MUST be
 * either Complete i.e if the test completed successfully, or one of the Error values listed above.
 * If the value of this parameter is anything other than Complete, the values of the results parameters for
 * this test are indeterminate. Following are the diagnostics states,
 * - None
 * - Requested
 * - Complete
 * - Error_CannotResolveHostName
 * - Error_Internal
 * - Error_Other
 *
 * @ingroup TR69
 *
 *
 * @defgroup TR69_HOSTIF TR-069 Host Interface
 * @par TR-069 Host Interface Manager
 * TR-069 Host Interface Manager is responsible to cater the service for two interfaces
 * - TR-069 client interface
 * - JSON Interface
 * TR-069 route the request based on the request parameter path and map to respective manager components,
 * like DeviceSetting, MoCA and other managers.
 *
 * @par TR-069 Host Interface Manager Vs TR-069 client interface
 * Following methods are supported by Host-if to tr-069 agent
 * - Get Parameter/Profile values | TR-069 Agent -> TR069 Host-If
 * - Set Parameter values | TR-069 Host-If -> TR-069 Agent
 *
 * @par TR-069 Notification
 * - Add instance of table.
 * - Delete instance of table.
 * - Subscribe | This will subscribe the parameter for notification of type Active, Passive and no notification
 *
 * @par TR-069 Host Interface Manager / Json interface
 * - It can only be used to get values from Host-If
 * - It can provide the values to html diagnostic pages.
 *
 * @par TR-069 Host Interface Manager / Json interface
 * <b> Json message request format: </b>
 * @code
 * {
 *       "paramList":[
 *          {
 *                "name:: "<Parameter-Path",
 *          }
 *       ]
 * }
 * @endcode
 * <b> Json message response format: </b>
 * @code
 * {
 *     "paramList": [
 *         {
 *                "name":"<Parameter-Path>",
 *                "value":"<Param-Value>"
 *         }
 *     ]
 * }
 * @endcode
 *
 * @ingroup TR69
 *
 *
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup tr69BusAgent
* @{
**/


#ifndef TR69BUSAGENT_H
#define TR69BUSAGENT_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "hostIf_tr69ReqHandler.h"
#define TR69_BUF_MIN 64
#define TR69_BUF_MAX 1024
#define IARM_BUS_TR69_COMMON_API_AgentParameterHandler      "agentParameterHandler"

    faultCode_t TR69Bus_ProcessSharedMalloc(size_t , void **);

    /**
     * @brief TR69 Incoming Request function pointer.
     *
     * This function passes the request info structure, parameter type and value .
     *
     * @return bool true/false.
     */

    typedef bool (*fpIncomingTR69Request) (HOSTIF_MsgData_t *);
    /**
     * @brief Starts the TR69 Register Callback.
     *
     * This function registers and connects TR69 Bus Agent.
     *
     * @return bool true/false.
     */

    bool tr69Register( const char*,  fpIncomingTR69Request);

    /**
     * @brief Function Wrapper around fpIncomingTR69Request.
     *
     * @return bool true/false.
     */

    IARM_Result_t _TR69AgentCallback_FuncWrapper(void *);

    /**
     * @brief TR69 UnRegister function.
     *
     * This function unregisters and disconnects TR69 Bus Agent.
     *
     * @return bool true/false.
     */

    bool tr69UnRegister( const char *);

    /**
     * @brief TR69 Request Complete function.
     *
     * This function is responsible to map the heap memory to process
     *  of requestinfo pointer from TR69 Bus Agents.
     *
     * @return bool true/false.
     */

    bool tr69RequestComplete(HOSTIF_MsgData_t *);

#ifdef __cplusplus
}
#endif

#endif // TR69BUSAGENT_H


/** @} */
/** @} */
