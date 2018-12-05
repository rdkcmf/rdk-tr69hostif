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
#ifndef DEVICE_IP_INTERFACE_IPV6ADDRESS_H_
#define DEVICE_IP_INTERFACE_IPV6ADDRESS_H_

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/
#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "hostIf_updateHandler.h"
#include <net/if.h>


class hostIf_IPv6Address {

    static  GHashTable  *ifHash;
    static GMutex *m_mutex;

    int dev_id;
    char nameOfInterface[IF_NAMESIZE];

    bool bCalledEnable;
    bool bCalledStatus;
    bool bCalledIPAddress;
    bool bCalledOrigin;

    bool backupIPv6AddressEnable;
    char backupIPv6AddressStatus[BUFF_LENGTH_16];
    char backupIPv6AddressIPAddress[BUFF_LENGTH_64];
    char backupIPv6AddressOrigin[BUFF_LENGTH_32];

    bool backupIPv6PrefixEnable;
    char backupIPv6PrefixStatus[BUFF_LENGTH_16];
    char backupIPv6PrefixPrefix[BUFF_LENGTH_64];
    char backupIPv6PrefixOrigin[BUFF_LENGTH_32];

    static const char* PREFERRED;
    static const char* DEPRECATED;
    static const char* INVALID;
    static const char* INACCESSIBLE;
    static const char* UNKNOWN;
    static const char* TENTATIVE;
    static const char* DUPLICATE;
    static const char* OPTIMISTIC;

    static const char* AUTOCONFIGURED;
    static const char* DHCPv6;
    static const char* WELLKNOWN;
    static const char* STATIC;
    static const char* PREFIXDELEGATION;
    static const char* ROUTERADVERTISEMENT;
    static const char* CHILD;
    static const char* INAPPLICABLE;

    hostIf_IPv6Address(int dev_id);
    ~hostIf_IPv6Address();

    int getIPv6AddressAndMask (int instance, struct in6_addr& in6_address, struct in6_addr& in6_mask);
    int getIPv6Prefix (int instance, struct in6_addr& in6_prefix_address, unsigned int& prefix_length);
    static bool isLinkLocalAddress (const struct in6_addr& in6_address);

    bool get_ipv6address_lifetime (unsigned int instance, char* search_string, long int& lifetime);
    bool get_ipv6address_preferred_lifetime (unsigned int instance, long int& preferred_lifetime);
    bool get_ipv6address_valid_lifetime (unsigned int instance, long int& valid_lifetime);
    void convert_lifetime_to_string (long int t, char* lifetime);

    int setIp(int interfaceNo, char *value);
    int removeIp(int interfaceNo, char *value);
    void refreshInterfaceName ();
    int get_IPv6Prefix_Origin (int instance, char* origin);
    int get_IPv6Prefix_StaticType (int instance, char* staticType);
    int get_IPv6Prefix_ParentPrefix (int instance, char* parentPrefix);
    int get_IPv6Prefix_ChildPrefixBits (int instance, char* childPrefixBits);

public:

    static hostIf_IPv6Address* getInstance(int dev_id);
    static GList* getAllInstances();
    static void closeInstance(hostIf_IPv6Address*);
    static void closeAllInstances();

    int handleGetMsg (const char* pSubSetting, int subInstanceNumber, HOSTIF_MsgData_t* stMsgData, bool isAddress);
    int handleSetMsg (const char* pSubSetting, int subInstanceNumber, HOSTIF_MsgData_t* stMsgData, bool isAddress);

    /**
     * @brief    Get status (enabled/disabled) of IPv6 address.
     *
     * This function provides the value (<tt>true/false</tt>) of the Enable status of this
     * IPv6 address.
     *
     * See @ref dev_ip_ipv6address_getter
     *
     */
    int get_IPv6Address_Enable(HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged = NULL);

    /**
     * @brief    Get the status of an IPv6 Address table entry.
     *
     * This function provides the status of this IPv6Address table entry.   Enumeration of:
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
     * See @ref dev_ip_ipv6address_getter
     *
     */
    int get_IPv6Address_Status(HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);

    int get_IPv6Address_IPAddressStatus (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);

    /**
     * @brief    Get the instance handle for an IPv6 Address of an IP Interface.
     *
     * This function provides a non-volatile handle used to reference this IPv6 address
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
     *               <li>This parameter can only be modified if <tt>Origin</tt>
     *               is <tt>Static</tt>.</li>
     *           </ul>
     * @par
     *
     * See @ref dev_ip_ipv6address_getter
     *
     */
    int get_IPv6Address_Alias(HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);

    /**
     * @brief    Get an IP Interface IPv6 IP Address
     *
     * This function provides the IPv6 address.
     *
     * @note     This parameter can only be modified if <tt>Origin</tt> is
     *           <tt>Static</tt>.
     *
     * See @ref dev_ip_ipv6address_getter
     *
     */
    int get_IPv6Address_IPAddress(HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);

    /**
     * @brief    Get an IP Interface IPv6 Address Origin.
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
     * See @ref dev_ip_ipv6address_getter
     *
     */
    int get_IPv6Address_Origin(HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);

    int get_IPv6Address_Prefix (HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged = NULL);
    int get_IPv6Address_PreferredLifetime (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);
    int get_IPv6Address_ValidLifetime (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);
    int get_IPv6Address_Anycast (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);

    int get_IPv6Prefix_Enable (HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged = NULL);
    int get_IPv6Prefix_Status (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);
    int get_IPv6Prefix_PrefixStatus (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);
    int get_IPv6Prefix_Alias (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);
    int get_IPv6Prefix_Prefix (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);
    int get_IPv6Prefix_Origin (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);
    int get_IPv6Prefix_StaticType (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);
    int get_IPv6Prefix_ParentPrefix (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);
    int get_IPv6Prefix_ChildPrefixBits (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);
    int get_IPv6Prefix_OnLink (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);
    int get_IPv6Prefix_Autonomous (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);
    int get_IPv6Prefix_PreferredLifetime (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);
    int get_IPv6Prefix_ValidLifetime (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged = NULL);

    /* End of TR_069_DEVICE_IP_IPV6ADDRESS_GETTER_API doxygen group */
    /**
     * @}
     */

    /** @addtogroup TR_069_DEVICE_IP_IPV6ADDRESS_SETTER_API TR-069 Device.IP.Interface.IPv6Address Setter API.
     *  @ingroup TR_069_DEVICE_IP_INTERFACE_IPV6ADDRESS_API
     *
     *  \section dev_ip_ipv6address_setter TR-069 Device.IP.Interface.IPv6Address object Setter API
     *
     *  This is the setter group of API for the <b>Device.IP.Interface.{i}.IPv6Address.{j}</b> object.
     *
     *  The interface for all functions is identical and is described here.
     *
     *  @param[in]   *name     This is the complete path name of the parameter extracted from
     *                         soap message, e.g. ::set_Device_IP_Interface_IPv6Address_Enable.
     *                         In this case, the path is "Device.IP.Interface.{i}.IPv6Address.{j}.Enable".
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
     * @brief Enable/Disable an IP Interface IPv6 Address.
     *
     * This function enables or disables this IPv6 address.
     *
     * See @ref dev_ip_ipv6address_setter
     *
     */
    int set_IPv6Address_Enable(HOSTIF_MsgData_t *stMsgData, int subInstanceNo);

    /**
     * @brief    Set the instance handle for an IPv6 Address of an IP Interface.
     *
     * This function sets a non-volatile handle used to reference this IPv6 address
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
     * See @ref dev_ip_ipv6address_setter
     *
     */

    int set_IPv6Address_Alias(HOSTIF_MsgData_t *stMsgData, int subInstanceNo);

    /**
     * @brief    Set an IP Interface IPv6 Address.
     *
     * This function sets the IPv6 address.
     *
     * @note     This parameter can only be modified if the Origin is <tt>Static</tt>.
     *
     * See @ref dev_ip_ipv6address_setter
     *
     */

    int set_IPv6Address_IPAddress(HOSTIF_MsgData_t *stMsgData, int subInstanceNo);

    /* End of TR_069_DEVICE_IP_IPV6ADDRESS_SETTER_API doxygen group */
    /**
     * @}
     */

    int set_IPv6Address_Prefix (HOSTIF_MsgData_t *stMsgData, int subInstanceNo);
    int set_IPv6Address_PreferredLifetime (HOSTIF_MsgData_t *stMsgData, int subInstanceNo);
    int set_IPv6Address_ValidLifetime (HOSTIF_MsgData_t *stMsgData, int subInstanceNo);
    int set_IPv6Address_Anycast (HOSTIF_MsgData_t *stMsgData, int subInstanceNo);

    /* IPv6Prefix profile set methods */
    int set_IPv6Prefix_Enable (HOSTIF_MsgData_t *stMsgData, int subInstanceNo);
    int set_IPv6Prefix_Alias (HOSTIF_MsgData_t *stMsgData, int subInstanceNo);
    int set_IPv6Prefix_Prefix (HOSTIF_MsgData_t *stMsgData, int subInstanceNo);
    int set_IPv6Prefix_StaticType (HOSTIF_MsgData_t *stMsgData, int subInstanceNo);
    int set_IPv6Prefix_ParentPrefix (HOSTIF_MsgData_t *stMsgData, int subInstanceNo);
    int set_IPv6Prefix_ChildPrefixBits (HOSTIF_MsgData_t *stMsgData, int subInstanceNo);
    int set_IPv6Prefix_OnLink (HOSTIF_MsgData_t *stMsgData, int subInstanceNo);
    int set_IPv6Prefix_Autonomous (HOSTIF_MsgData_t *stMsgData, int subInstanceNo);
    int set_IPv6Prefix_PreferredLifetime (HOSTIF_MsgData_t *stMsgData, int subInstanceNo);
    int set_IPv6Prefix_ValidLifetime (HOSTIF_MsgData_t *stMsgData, int subInstanceNo);
};


#endif /* DEVICE_IP_INTERFACE_IPV6ADDRESS_H_ */
