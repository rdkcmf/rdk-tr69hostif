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
 * @file Service_Storage.h
 * @brief The header file provides storage service information APIs.
 */

/**
 * @defgroup TR69_HOSTIF_STORAGE TR-069 Object (Device.StorageService)
 *
 * TR-069 Data Model for Storage Service Devices, permits the remote
 * management of Storage Service devices via CWMP . It covers the data model for describing
 * a Storage Service device as well as rules regarding notifications on parameter value change.
 * General use cases are also described including standard data model profiles that would
 * typically be seen while remotely managing a device of this nature.
 *
 * @par Basic Managed Storage Service
 * A basic managed storage service offers a Service Provider the option to assist the
 * customer as soon as the customers StorageService-enabled device is activated and being
 * managed by the ACS.
 *
 * The following is a sample list of support capabilities an ACS can
 * provide using CWMP (NOTE: Not all of these capabilities are handled with this data
 * model; some are handled from a protocol perspective and some are handled via other data
 * models):
 * - Basic configuration and setup during device activation [addressed by this document
 * (configuration parameters) & TR-106 (configuration parameters)]
 * - User credentials setup and file privilege access [addressed by this document (Folder
 * Access)]
 * - Firmware upgrade [addressed by TR-069 (Download command)]
 * - Retrieval of device status [addressed by this document (parameters) and TR-106]
 * - Wireless setup (e.g., WEP security) for a Storage Service device with Wi-Fi access
 * [addressed by a future version of TR-106]
 * - Configuration and log file retrieval for root cause analysis of problems [addressed by
 * a future version of TR-106]
 * - Monitoring active/passive notification events, e.g., volume capacity reached, and
 * potential physical media failures [addressed by this document (parameters) and TR-
 * 069 (notification mechanism)]
 * - Network diagnostics and troubleshooting, e.g., network connectivity to the Internet
 * gateway device, and to the Internet [addressed by TR-106 (connection parameters)]
 * With such support capabilities from the ACS, a technical support agent will be better
 * equipped to help a customer with Storage Service issues during trouble calls. The goal
 * is to provide a true plug-and-play solution for the customers.
 * @ingroup TR69_HOSTIF_PROFILE
 *
 * @defgroup TR69_HOSTIF_STORAGE_SERVICE TR-069 Object (Device.StorageService.Interface)
 * The Service Object for a Storage Service device.
 * At most one entry in this table can exist with a given value for Alias.
 * @note Alias
 * is a non-volatile handle used to reference this instance. Alias provides a mechanism for an ACS to
 * label this instance for future reference.
 *
 * If the CPE supports the Alias-based Addressing feature the following mandatory constraints MUST be enforced:
 * - Its value MUST NOT be empty.
 * - Its value MUST start with a letter.
 * If its value is not assigned by the ACS, it MUST start with a 'cpe-' prefix.
 * The CPE MUST NOT change the parameter value.
 *
 * @par The overall capabilities of a Storage Service device.
 * This is a constant read-only object, meaning that only a firmware upgrade will cause these
 * values to be altered.
 *
 * @ingroup TR69_HOSTIF_STORAGE
 *
 * @defgroup TR69_HOSTIF_STORAGE_SERVICE_API TR-069 Object (Device.StorageService.Interface.{i}) Public APIs
 * Describe the details about RDK TR-069 storage service APIs specifications.
 * @ingroup TR69_HOSTIF_STORAGE_SERVICE
 *
 * @defgroup TR69_HOSTIF_STORAGE_SERVICE_CLASSES TR-069 Object (Device.StorageService.Interface.{i}) Public Classes
 * Describe the details about classes used in TR069 storage service.
 * @ingroup TR69_HOSTIF_STORAGE_SERVICE
 */
#ifndef _SERVICE_STORAGE_H_
#define _SERVICE_STORAGE_H_
#include "hostIf_msgHandler.h"
#include "hostIf_updateHandler.h"

/**
 * @brief This class provides the TR-069 storage service information.
 * @ingroup TR69_HOSTIF_STORAGE_SERVICE_CLASSES
 */
class hostIf_StorageSrvc
{
    
    static  GHashTable  *storageSrvHash;

    static GMutex *m_mutex;

    int dev_id;
    
    hostIf_StorageSrvc(int dev_id);

    public:
        
        static hostIf_StorageSrvc* getInstance(int dev_id);

        static void closeInstance(hostIf_StorageSrvc*);

        static GList* getAllInstances();

        static void closeAllInstances();
        
        static unsigned int get_Device_StorageSrvc_ClientNumberOfEntries(HOSTIF_MsgData_t *stMsgData);

        int handleGetMsg(HOSTIF_MsgData_t *stMsgData);

/**
 * @brief This function set the hostIf storage service interface such as total number
 * physical medium entries, attributes of physical medium "Name", "SmartCapable" and
 * "Health". Currently not implemented.
 *
 * @param[in] stMsgData  HostIf Message Request data contains the storage service attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the hostIf storage service interface attribute.
 * @retval -1 If Not able to set the hostIf storage service interface attribute.
 * @retval -2 If Not handle the hostIf storage service interface attribute.
 * @ingroup TR69_HOSTIF_STORAGE_SERVICE_API
 */
        int handleSetMsg(HOSTIF_MsgData_t *stMsgData);

        static void getLock();

        static void releaseLock();

};
#endif
