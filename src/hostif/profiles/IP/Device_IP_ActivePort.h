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
 * @file Device_IP_ActivePort.h
 * @brief The header file provides TR069 device IP active port information APIs.
 */

/**
 * @defgroup TR69_HOSTIF_DEVICE_IP_ACTIVEPORT TR-069 Object (Device.IP.ActivePort.Interface)
 * This table lists the ports on which TCP connections are listening or established.
 * At most one entry in this table can exist with all the same values for
 * - LocalIPAddress
 * - LocalPort
 * - RemoteIPAddress
 * - RemotePort.
 * @ingroup TR69_HOSTIF_DEVICE_IP
 *
 * @defgroup TR69_HOSTIF_DEVICE_IP_ACTIVEPORT_API TR-069 Object (Device.IP.ActivePort.Interface.{i}) Public APIs
 * Describe the details about TR-069 Device IP active port APIs specifications.
 * @ingroup TR69_HOSTIF_DEVICE_IP_ACTIVEPORT
 *
 * @defgroup TR69_HOSTIF_DEVICE_IP_ACTIVEPORT_CLASSES TR-069 Object (Device.IP.ActivePort.Interface.{i}) Public Classes
 * Describe the details about classes used in TR-069 Device IP active port.
 * @ingroup TR69_HOSTIF_DEVICE_IP_ACTIVEPORT
 *
 * @defgroup TR69_HOSTIF_DEVICE_IP_ACTIVEPORT_DSSTRUCT TR-069 Object (Device.IP.ActivePort.Interface.{i}) Public DataStructure
 * Describe the details about data structure used in TR-069 Device IP active port.
 * @ingroup TR69_HOSTIF_DEVICE_IP_ACTIVEPORT
 */

/**
 * @file Device_IP_ActivePort.h
 *
 * @brief TR-069 Device.IP.ActivePort object Public API.
 *
 * Description of Device IP Active module.
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


#ifndef DEVICE_IP_ACTIVEPORT_H_
#define DEVICE_IP_ACTIVEPORT_H_

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/
#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "hostIf_updateHandler.h"

#define         _LENGTH_PARAMETER               64
#define         _LENGTH_IPADDR                   16
#define         _LENGTH_STATUS                  12
#define         _HEX_STATE_LEN                  3
#define         _BUF_LEN            1024


/** @defgroup TR_069_DEVICE_IP_ACTIVEPORT_API TR-069 Device.IP.ActivePort object API.
 *  @ingroup TR_069_DEVICE_IP_API
 *
 *  The Device.IP.ActivePort table lists the ports on which TCP connections are listening
 *  or established.
 *
 *  @note    At most one entry in this table can exist with all the same values for
 *           LocalIPAddress, LocalPort, RemoteIPAddress and RemotePort.
 *
 */

/** @addtogroup TR_069_DEVICE_IP_ACTIVEPORT_GETTER_API TR-069 Device.IP.ActivePort Getter API.
 *  @ingroup TR_069_DEVICE_IP_ACTIVEPORT_API
 *
 *  \section dev_ipactiveport_getter TR-069 Device.IP.ActivePort object Getter API
 *
 *  This is the getter group of API for the <b>Device.IP.ActivePort.{i}</b> object.
 *
 *  The interface for all functions is identical and is described here.
 *
 *  @param[in]   *name     This is the complete path name of the parameter extracted from
 *                         soap message, e.g. ::get_Device_IP_ActivePort_LocalIPAddress.
 *                         In this case, the path is "Device.IP.ActivePort.{i}.LocalIPAddress".
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
 * @addtogroup TR69_HOSTIF_DEVICE_IP_ACTIVEPORT_DSSTRUCT
 * @{
 */

/**
 * @brief The structure holds the required parameters such as local ipaddress, localport, remoteIpAddress
 * etc.. for the device ip of the active port.
 */
struct Device_IP_ActivePort {

    char localIpAddress[_LENGTH_IPADDR];
    unsigned int localPort;
    char remoteIpAddress[_LENGTH_IPADDR];
    unsigned int remotePort;
    char  status[_LENGTH_STATUS];
};
/** @} */ //End of the Doxygen tag TR69_HOSTIF_DEVICE_IP_ACTIVEPORT_DSSTRUCT

/**
 * @brief This class provides the hostIf IP active port for getting IP active port information.
 * @ingroup TR69_HOSTIF_DEVICE_IP_ACTIVEPORT_CLASSES
 */
class hostIf_IPActivePort {

    static  GHashTable  *ifHash;

    static GMutex *m_mutex;

    static struct Device_IP_ActivePort activePort;

    int dev_id;

    bool bCalledLocalIPAddress;
    bool bCalledLocalPort;
    bool bCalledRemoteIPAddress;
    bool bCalledRemotePort;
    bool bCalledStatus;

    char backupLocalIPAddress[TR69HOSTIFMGR_MAX_PARAM_LEN];
    char backupRemoteIPAddress[TR69HOSTIFMGR_MAX_PARAM_LEN];
    char backupStatus[TR69HOSTIFMGR_MAX_PARAM_LEN];
    unsigned int backupLocalPort;
    unsigned int backupRemotePort;

    hostIf_IPActivePort(int dev_id);

    ~hostIf_IPActivePort() {};

    int getActivePortsFields(unsigned int activePortNo);

public:

    static hostIf_IPActivePort *getInstance(int dev_id);

    static void closeInstance(hostIf_IPActivePort *);

    static GList* getAllInstances();

    static void closeAllInstances();

    static void getLock();

    static void releaseLock();



    int handleGetMsg (const char* pSetting, HOSTIF_MsgData_t* stMsgData);

    int get_Device_IP_ActivePort_LocalIPAddress(HOSTIF_MsgData_t *, bool *pChanged = false);



    int get_Device_IP_ActivePort_LocalPort(HOSTIF_MsgData_t *, bool *pChanged = false);



    int get_Device_IP_ActivePort_RemoteIPAddress(HOSTIF_MsgData_t *, bool *pChanged = false);



    int get_Device_IP_ActivePort_RemotePort(HOSTIF_MsgData_t *, bool *pChanged = false);



    int get_Device_IP_ActivePort_Status(HOSTIF_MsgData_t *, bool *pChanged = false);

    /* End of TR_069_DEVICE_IP_ACTIVEPORT_GETTER_API doxygen group */
    /**
     * @}
     */
};

#endif /* DEVICE_IP_ACTIVEPORT_H_ */


/** @} */
/** @} */
