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
 * @file Device_IP_Interface_IPv4Address.h
 *
 * @brief TR-069 Device.IP.Interface.IPv4Address object Public API.
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


#ifndef DEVICE_IP_INTERFACE_IPV4ADDRESS_H_
#define DEVICE_IP_INTERFACE_IPV4ADDRESS_H_

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/
#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "hostIf_updateHandler.h"
#include <net/if.h>


/** @defgroup TR_069_DEVICE_IP_INTERFACE_IPV4ADDRESS_API TR-069 Device.IP.Interface.IPv4Address object API.
 *  @ingroup TR_069_DEVICE_IP_INTERFACE_API
 *
 *  The Device.IP.Interface.IPv4Address is the table of IPv4 Addresses.  Entries are
 *  auto-created and auto-deleted as IP addresses are added and deleted via DHCP,
 *  auto-IP, or IPCP. Static entries are created and configured by the ACS.
 *
 *  @note    <ul>
 *               <li>At most one entry in this table (regardless of whether or not it
 *               is enabled) can exist with a given value for Alias. On creation of a
 *               new table entry, the CPE MUST choose an initial value for Alias such
 *               that the new entry does not conflict with any existing entries.</li>
 *
 *               <li>At most one enabled entry in this table can exist with the same
 *               values for IPAddress and SubnetMask.</li>
 *           </ul>
 *
 */

/** @addtogroup TR_069_DEVICE_IP_INTERFACE_IPV4ADDRESS_GETTER_API TR-069 Device.IP.Interface.IPv4Address Getter API.
 *  @ingroup TR_069_DEVICE_IP_INTERFACE_IPV4ADDRESS_API
 *
 *  \section dev_ip_ipv4address_getter TR-069 Device.IP.Interface.IPv4Address object Getter API
 *
 *  This is the getter group of API for the <b>Device.IP.Interface.{i}.IPv4Address.{j}</b> object.
 *
 *  The interface for all functions is identical and is described here.
 *
 *  @param[in]   *name     This is the complete path name of the parameter extracted from
 *                         soap message, e.g. ::get_Device_IP_Interface_IPv4Address_Enable.
 *                         In this case, the path is "Device.IP.Interface.{i}.IPv4Address.{j}.Enable".
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


class hostIf_IPv4Address {

    static  GHashTable  *ifHash;
    static GMutex *m_mutex;

    int dev_id;
    char nameOfInterface[IF_NAMESIZE];

    bool bCalledEnable;
    bool bCalledStatus;
    bool bCalledIPAddress;
    bool bCalledSubnetMask;
    bool bCalledAddressingType;

    bool backupEnable;
    char backupStatus[BUFF_LENGTH_16];
//    char backupAlias[TR69HOSTIFMGR_MAX_PARAM_LEN];
    char backupIPAddress[BUFF_LENGTH_64];
    char backupSubnetMask[BUFF_LENGTH_64];
    char backupAddressingType[BUFF_LENGTH_16];

    hostIf_IPv4Address(int dev_id);
    ~hostIf_IPv4Address() {};

    void refreshInterfaceName ();
    int getIPv4AddressAndMask (int instance, struct in_addr& in_address, struct in_addr& in_mask);
    static bool isLinkLocalAddress (const struct in_addr& in_address);
    bool hasPhysicalInterfaceAs (char* phy_if_name);
    int setIpOrMask(int interfaceNo, char *value, const char* ipOrMask);

public:

    static hostIf_IPv4Address* getInstance(int dev_id);
    static GList* getAllInstances();
    static void closeInstance(hostIf_IPv4Address*);
    static void closeAllInstances();

    static void getLock();
    static void releaseLock();

    int handleGetMsg (const char* pSubSetting, int subInstanceNumber, HOSTIF_MsgData_t* stMsgData);
    int handleSetMsg (const char* pSubSetting, int subInstanceNumber, HOSTIF_MsgData_t* stMsgData);

    /**
     * @brief    Get status (enabled/disabled) of IPv4 address.
     *
     * This function provides the value (<tt>true/false</tt>) of the Enable status of this
     * IPv4 address.
     *
     * See @ref dev_ip_ipv4address_getter
     *
     */
    int get_IPv4Address_Enable(HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged = NULL);

    /**
     * @brief    Get the status of an IPv4 Address table entry.
     *
     * This function provides the status of this IPv4Address table entry.   Enumeration of:
     * <tt>
     *     <ul><li>Disabled</li>
     *         <li>Enabled</li>
     *         <li>Error_Misconfigured</li>
     *         <li>Error (OPTIONAL)</li></ul>
     * </tt>
     *
     * @note    <ul>
     *              <li>The <tt>Error_Misconfigured</tt> value indicates that a necessary
     *              configuration value is undefined or invalid.</li>
     *
     *              <li>The <tt>Error</tt> value MAY be used by the CPE to indicate a
     *              locally defined error condition.</li>
     *          </ul>
     *
     * See @ref dev_ip_ipv4address_getter
     *
     */
    int get_IPv4Address_Status(HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);

    /**
     * @brief    Get the instance handle for an IPv4 Address of an IP Interface.
     *
     * This function provides a non-volatile handle used to reference this IPv4 address
     * instance of this IP interface. Alias provides a mechanism for an ACS to label this
     * instance for future reference.
     *
     * @note     <ul>
     *               <li>If the CPE supports the Alias-based Addressing feature as defined
     *               in [Section 3.6.1/TR-069 Amendment 4] and described in [Appendix
     *               II/TR-069 Amendment 4], the following mandatory constraints MUST be
     *               enforced:
     *               <ul><li>Its value MUST NOT be empty.</li>
     *                   <li>Its value MUST start with a letter.</li>
     *                   <li>If its instance object is created by the CPE, the initial
     *                   value MUST start with a "cpe-" prefix.</li>
     *                   <li>The CPE MUST NOT change the parameter value.</li>
     *               </ul></li>
     *               <li>This parameter can only be modified if <tt>AddressingType</tt>
     *               is <tt>Static</tt>.</li>
     *           </ul>
     * @par
     *
     * See @ref dev_ip_ipv4address_getter
     *
     */
    int get_IPv4Address_Alias(HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);

    /**
     * @brief    Get an IP Interface IPv4 IP Address
     *
     * This function provides the IPv4 address.
     *
     * @note     This parameter can only be modified if <tt>AddressingType</tt> is
     *           <tt>Static</tt>.
     *
     * See @ref dev_ip_ipv4address_getter
     *
     */
    int get_IPv4Address_IPAddress(HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);

    /**
     * @brief    Get an IP Interface IPv4 Address SubnetMask.
     *
     * This function provides the IPv4 Address Subnet mask.
     *
     * @note     This parameter can only be modified if <tt>AddressingType</tt> is
     *           <tt>Static</tt>.
     *
     * See @ref dev_ip_ipv4address_getter
     *
     */
    int get_IPv4Address_SubnetMask(HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);

    /**
     * @brief    Get an IP Interface IPv4 Address AddressingType.
     *
     * This function provides the addressing method used to assign the IP address.
     * Enumeration of:
     * <tt>
     *     <ul><li>DHCP</li>
     *         <li>AutoIP</li>
     *         <li>IPCP</li>
     *         <li>Static</li></ul>
     * </tt>
     *
     * See @ref dev_ip_ipv4address_getter
     *
     */
    int get_IPv4Address_AddressingType(HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);

    /* End of TR_069_DEVICE_IP_IPV4ADDRESS_GETTER_API doxygen group */
    /**
     * @}
     */

    /** @addtogroup TR_069_DEVICE_IP_IPV4ADDRESS_SETTER_API TR-069 Device.IP.Interface.IPv4Address Setter API.
     *  @ingroup TR_069_DEVICE_IP_INTERFACE_IPV4ADDRESS_API
     *
     *  \section dev_ip_ipv4address_setter TR-069 Device.IP.Interface.IPv4Address object Setter API
     *
     *  This is the setter group of API for the <b>Device.IP.Interface.{i}.IPv4Address.{j}</b> object.
     *
     *  The interface for all functions is identical and is described here.
     *
     *  @param[in]   *name     This is the complete path name of the parameter extracted from
     *                         soap message, e.g. ::set_Device_IP_Interface_IPv4Address_Enable.
     *                         In this case, the path is "Device.IP.Interface.{i}.IPv4Address.{j}.Enable".
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
     * @brief Enable/Disable an IP Interface IPv4 Address.
     *
     * This function enables or disables this IPv4 address.
     *
     * See @ref dev_ip_ipv4address_setter
     *
     */
    int set_IPv4Address_Enable(HOSTIF_MsgData_t *stMsgData, int subInstanceNo);

    /**
     * @brief    Set the instance handle for an IPv4 Address of an IP Interface.
     *
     * This function sets a non-volatile handle used to reference this IPv4 address
     * instance of this IP interface. Alias provides a mechanism for an ACS to label this
     * instance for future reference.
     *
     * @note     If the CPE supports the Alias-based Addressing feature as defined in
     *           [Section 3.6.1/TR-069 Amendment 4] and described in [Appendix II/TR-069
     *           Amendment 4], the following mandatory constraints MUST be enforced:
     *               <ul><li>Its value MUST NOT be empty.</li>
     *                   <li>Its value MUST start with a letter.</li>
     *                   <li>If its instance object is created by the CPE, the initial
     *                   value MUST start with a "cpe-" prefix.</li>
     *                   <li>The CPE MUST NOT change the parameter value.</li>
     *               </ul>
     * @par
     *
     * See @ref dev_ip_ipv4address_setter
     *
     */
    int set_IPv4Address_Alias(HOSTIF_MsgData_t *stMsgData, int subInstanceNo);

    /**
     * @brief    Set an IP Interface IPv4 Address.
     *
     * This function sets the IPv4 address.
     *
     * @note     This parameter can only be modified if the AddressingType is <tt>Static</tt>.
     *
     * See @ref dev_ip_ipv4address_setter
     *
     */
    int set_IPv4Address_IPAddress(HOSTIF_MsgData_t *stMsgData, int subInstanceNo);

    /**
     * @brief    Set an IP Interface IPv4Address Subnet Mask.
     *
     * This function sets the Subnet Mask of an IPv4 Address.
     *
     * @note     This parameter can only be modified if the AddressingType is <tt>Static</tt>.
     *
     * See @ref dev_ip_ipv4address_setter
     *
     */
    int set_IPv4Address_SubnetMask(HOSTIF_MsgData_t *stMsgData, int subInstanceNo);

    /* End of TR_069_DEVICE_IP_IPV4ADDRESS_SETTER_API doxygen group */
    /**
     * @}
     */
};


#endif /* DEVICE_IP_INTERFACE_IPV4ADDRESS_H_ */


/** @} */
/** @} */
