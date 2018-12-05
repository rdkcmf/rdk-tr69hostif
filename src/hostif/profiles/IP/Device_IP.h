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
 * @file Device_IP.h
 *
 * @brief TR-069 Device.IP object Public API.
 *
 * Description of Device IP module.
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


#ifndef DEVICE_IP_H_
#define DEVICE_IP_H_

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/

#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "hostIf_updateHandler.h"



#define LENGTH 16

typedef enum EIPMembers
{
    eIpIPv4Capable=0,
    eIpIPv4Enable,
    eIpIPv4Status,
    eIpULAPrefix,
    eIpInterfaceNumberOfEntries,
    eIpActivePortNumberOfEntries
}
EIPMembers;

typedef struct Device_IP
{
    bool iPv4Capable;
    bool iPv4Enable;
    char iPv4Status[LENGTH];
    char uLAPrefix[LENGTH];
    unsigned int interfaceNumberOfEntries;
    unsigned int activePortNumberOfEntries;
} IP;

/** @defgroup TR_069_DEVICE_IP_API TR-069 Device.IP object API.
 *  @ingroup TR_069_API
 *
 *  The Device.IP object is the IP object that contains the Interface, ActivePort,
 *  and Diagnostics objects.
 */

/** @addtogroup TR_069_DEVICE_IP_GETTER_API TR-069 Device.IP Getter API.
 *  @ingroup TR_069_DEVICE_IP_API
 *
 *  \section dev_ip_getter TR-069 Device.IP object Getter API
 *
 *  This is the getter group of API for the <b>Device.IP</b> object.
 *
 *  The interface for all functions is identical and is described here.
 *
 *  @param[in]   *name     This is the complete path name of the parameter extracted from
 *                         soap message, e.g. ::get_Device_IP_IPv4Capable.
 *                         In this case, the path is "Device.IP.IPv4Capable".
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
class hostIf_IP {

    static  GHashTable  *ifHash;

    static GMutex *m_mutex;

    static IP stIPInstance;

    static char* cmd_NumOfActivePorts;

    static  GHashTable  *m_notifyHash;

    int dev_id;

    hostIf_IP(int dev_id);

    ~hostIf_IP();

    bool bCalledIPv4Capable;
    bool bCalledIPv4Enable;
    bool bCalledIPv4Status;
    bool bCalledInterfaceNumberOfEntries;
    bool bCalledActivePortNumberOfEntries;

    bool backupIPv4Capable;
    bool backupIPv4Enable;
    int backupInterfaceNumberOfEntries;
    int backupActivePortNumberOfEntries;
    char backupIPv4Status[TR69HOSTIFMGR_MAX_PARAM_LEN];

    static unsigned int getInterfaceNumberOfEntries(void);
    static unsigned int getPhysicalInterfaceNumberOfEntries (struct if_nameindex* phy_if_list);
    static unsigned int getVirtualInterfaceNumberOfEntries (struct if_nameindex* phy_if_list);
    static char* getVirtualInterfaceName (struct if_nameindex *phy_if_list, unsigned int virtual_if_index, char* virtual_if_name);

public:

    static hostIf_IP* getInstance(int dev_id);

    static void closeInstance(hostIf_IP*);

    static GList* getAllInstances();

    static void closeAllInstances();

    static void getLock();

    static void releaseLock();

    static char* getInterfaceName (int if_index, char* if_name);

    static unsigned int getNumOfActivePorts(void);

    static int get_Device_IP_Fields(EIPMembers ipMem);

    static GHashTable* getNotifyHash();


    int handleGetMsg (HOSTIF_MsgData_t* stMsgData);
    int handleSetMsg (HOSTIF_MsgData_t* stMsgData);

    /**
     * @brief    Get the IPv4 capability of a device.
     *
     * This function indicates whether or not the device is IPv4 capable (<tt>true</tt>)
     * or not (<tt>false</tt>).
     *
     * See @ref dev_ip_getter
     *
     */
    int get_Device_IP_IPv4Capable(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief    Get the status (enabled/disabled) of the IPv4 stack on a device.
     *
     * This function provides the status (enabled/disabled) of the IPv4 stack, and so the
     * use of IPv4 on the device. This affects only layer 3 and above.
     *
     * @note     When <tt>false</tt>, IP interfaces that had been operationally up and
     *           passing IPv4 packets will now no longer be able to do so, and will be
     *           operationally down (unless also attached to an enabled IPv6 stack).
     *
     * See @ref dev_ip_getter
     *
     */
    int get_Device_IP_IPv4Enable(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief    Get the status of the IPv4 stack on a device.
     *
     * This function indicates the status of the IPv4 stack. It is an enumeration of:
     *
     * <tt>
     *     <ul><li>Disabled</li>
     *         <li>Enabled</li>
     *         <li>Error (OPTIONAL)</li></ul>
     * </tt>
     *
     * @note     The Error value MAY be used by the CPE to indicate a locally defined
     *           error condition.
     *
     * See @ref dev_ip_getter
     *
     */

    int get_Device_IP_IPv4Status(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief    Get the ULA /48 prefix for a device.
     *
     * This function provides the ULA /48 prefix of the device. This is the IPv6 address
     * prefix and can be any IPv6 prefix that is permitted by the IPPrefix data type.
     *
     * @note     <ul>
     *               <li>This is specified as an IP address followed by an appended "/n"
     *               suffix, where n (the prefix size) is an integer in the range 0-32
     *               (for IPv4) or 0-128 (for IPv6) that indicates the number of
     *               (leftmost) '1' bits of the routing prefix.
     *               <ul>
     *                   <li>IPv4 example: 192.168.1.0/24</li>
     *                   <li>IPv6 example: 2001:edff:fe6a:f76::/64</li>
     *               </ul></li>
     *
     *               <li>If the IP address part is unspecified or inapplicable, it MUST
     *               be an empty string unless otherwise specified by the parameter
     *               definition. In this case the IP prefix will be of the form "/n".</li>
     *
     *               <li>If the entire IP prefix is unspecified or inapplicable, it MUST
     *               be an empty string unless otherwise specified by the parameter
     *               definition.</li>
     *           </ul>
     *
     * See @ref dev_ip_getter
     *
     */

    int get_Device_IP_ULAPrefix(HOSTIF_MsgData_t *, bool *pChanged = NULL);

    /**
     * @brief    Get the number of entries in the Interface table.
     *
     * This function provides the number of entries in the Interface table of the device.
     *
     * See @ref dev_ip_getter
     *
     */

    static int get_Device_IP_InterfaceNumberOfEntries(HOSTIF_MsgData_t *);

    /**
     * @brief    Get the number of entries in the ActivePort table.
     *
     * This function provides the number of entries in the ActivePort table of the device.
     *
     * See @ref dev_ip_getter
     *
     */

    static int get_Device_IP_ActivePortNumberOfEntries(HOSTIF_MsgData_t *);

    /* End of TR_069_DEVICE_IP_GETTER_API doxygen group */
    /**
     * @}
     */

    /** @addtogroup TR_069_DEVICE_IP_SETTER_API TR-069 Device.IP Setter API.
     *  @ingroup TR_069_DEVICE_IP_API
     *
     *  \section dev_ip_setter TR-069 Device.IP object Setter API
     *
     *  This is the setter group of API for the <b>Device.IP</b> object.
     *
     *  The interface for all functions is identical and is described here.
     *
     *  @param[in]   *name     This is the complete path name of the parameter extracted from
     *                         soap message, e.g. ::set_Device_IP_IPv4Enable.
     *                         In this case, the path is "Device.IP.IPv4Enable".
     *  @param[in]   *type     Data type of parameter defined for TR-069. This is same as the
     *                         data type used in the Xi3 data-model.xml file.
     *                         (see parameter.h)
     *  @param[in]   *value    This is the value to which the parameter requested by the ACS
     *                         must be set.
     *                         (see paramaccess.h)
     *
     *  @return The status of the operation.
     *  @retval OK            If parameter requested was successfully updated. (Same as <b>NO_ERROR</b>).
     *  @retval NO_ERROR      If parameter requested was successfully updated. (Same as <b>OK</b>).
     *  @retval DIAG_ERROR    Diagnostic error.
     *  @retval ERR_???       Appropriate error value otherwise (see dimark_globals.h).
     *
     *  @todo Clarify description of DIAG_ERROR.
     *
     *  @{
     */


    /**
     * @brief    Set the status (enabled/disabled) of the IPv4 stack on a device.
     *
     * This function sets the status (enabled/disabled) of the IPv4 stack, and so the
     * use of IPv4 on the device. This affects only layer 3 and above.
     *
     * @note     When <tt>false</tt>, IP interfaces that had been operationally up and
     *           passing IPv4 packets will now no longer be able to do so, and will be
     *           operationally down (unless also attached to an enabled IPv6 stack).
     *
     * See @ref dev_ip_setter
     *
     */

    int set_Device_IP_IPv4Enable(HOSTIF_MsgData_t *);

    /**
     * @brief    Set the ULA /48 prefix for a device.
     *
     * This function sets the ULA /48 prefix of the device. This is the IPv6 address
     * prefix and can be any IPv6 prefix that is permitted by the IPPrefix data type.
     *
     * @note     <ul>
     *               <li>This is specified as an IP address followed by an appended "/n"
     *               suffix, where n (the prefix size) is an integer in the range 0-32
     *               (for IPv4) or 0-128 (for IPv6) that indicates the number of
     *               (leftmost) '1' bits of the routing prefix.
     *               <ul>
     *                   <li>IPv4 example: 192.168.1.0/24</li>
     *                   <li>IPv6 example: 2001:edff:fe6a:f76::/64</li>
     *               </ul></li>
     *
     *               <li>If the IP address part is unspecified or inapplicable, it MUST
     *               be an empty string unless otherwise specified by the parameter
     *               definition. In this case the IP prefix will be of the form "/n".</li>
     *
     *               <li>If the entire IP prefix is unspecified or inapplicable, it MUST
     *               be an empty string unless otherwise specified by the parameter
     *               definition.</li>
     *           </ul>
     *
     * See @ref dev_ip_setter
     *
     */

    int set_Device_IP_ULAPrefix(HOSTIF_MsgData_t *);

    /* End of TR_069_DEVICE_IP_SETTER_API doxygen group */
    /**
     * @}
     */
};

#endif /* DEVICE_IP_H_ */


/** @} */
/** @} */
