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
 * @file Device_IP_Interface.h
 * @brief The header file provides TR069 device IP interface information APIs.
 */

/**
 * @defgroup TR69_HOSTIF_DEVICE_INTERFACE_IP TR-069 Object (Device.IP.Interface)
 * IP interface table is a stackable interface object . This table models the layer 3 IP interface.
 * Each IP interface can be attached to the IPv4 and/or IPv6 stack. The interfaces IP addresses
 * and prefixes are listed in the IPv4Address, IPv6Address and IPv6Prefix tables.
 *
 * @note Note that support for manipulating 'Loopback' interfaces is OPTIONAL, so the implementation
 * MAY choose not to create or allow the ACS to create the interface instances of type 'Loopback'.
 *
 * - When the ACS administratively disables the interface i.e. sets 'Enable' to false, the interface
 * automatically-assigned IP addresses and prefixes MAY be retained.
 * - When the ACS administratively enables the interface, i.e. sets 'Enable' to true, these IP
 * addresses and prefixes MUST be refreshed.
 * It's up to the implementation to decide exactly what this means: it SHOULD take all reasonable
 * steps to refresh everything but if it is unable, for example, to refresh a prefix that still
 * has a significant lifetime, it might well choose to retain rather than discard it.
 *
 * At most one entry in this table regardless of whether or not it is enabled can exist with a
 * given value for Alias, or with a given value for Name.
 * On creation of a new table entry, the CPE MUST choose initial values for Alias and Name
 * such that the new entry does not conflict with any existing entries.
 * @ingroup TR69_HOSTIF_DEVICE_IP
 *
 * @defgroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API TR-069 Object (Device.IP.Interface.{i}) Public APIs
 * Describe the details about TR-069 Device IP Interface APIs specifications.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP
 *
 * @defgroup TR69_HOSTIF_DEVICE_INTERFACE_IP_CLASSES TR-069 Object (Device.IP.Interface.{i}) Public Classes
 * Describe the details about classes used in TR069 Device IP Interface.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP
 *
 * @defgroup TR69_HOSTIF_DEVICE_INTERFACE_IP_DSSTRUCT TR-069 Object (Device.IP.Interface.{i}) Public DataStructure
 * Describe the details about data structure used in TR069 Device IP Interface.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP
 */

/**
 * @file Device_IP_Interface.h
 *
 * @brief TR-069 Device.IP.Interface object Public API.
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


#ifndef DEVICE_IP_INTERFACE_H_
#define DEVICE_IP_INTERFACE_H_

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/
#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "hostIf_updateHandler.h"
#include <net/if.h>

#define IP_STATUS_LENGTH 24
#define ALIAS_LENGTH 70
#define NAME_LENGTH 70
#define LL_LENGTH 1024
#define ROUTER_LENGTH 256
#define TYPE_LENGTH 24

/**
 * @addtogroup TR69_HOSTIF_DEVICE_INTERFACE_IP_DSSTRUCT
 * @{
 */
/**
 * @brief It contains the members variables of the Device_IP_Interface structure.
 */
typedef struct Device_IP_Interface
{
    bool enable;
    bool iPv4Enable;
    bool iPv6Enable;
    bool uLAEnable;
    char status[IP_STATUS_LENGTH];
    char alias[ALIAS_LENGTH];
    char name[NAME_LENGTH];
    unsigned int lastChange;
    char lowerLayers[LL_LENGTH];
    char router[ROUTER_LENGTH];
    bool reset;
    unsigned int maxMTUSize;
    char type[TYPE_LENGTH];
    bool loopback;
    unsigned int iPv4AddressNumberOfEntries;
#ifdef IPV6_SUPPORT
    unsigned int iPv6AddressNumberOfEntries;
    unsigned int iPv6PrefixNumberOfEntries;
#endif
    bool autoIPEnable;
} IPInterface;

/**
 * @enum EDeviceIPInterfaceMembers
 * @brief It contains the members variables of the EDeviceIPInterfaceMembers enum.
 */
typedef enum EDeviceIPInterfaceMembers
{
    eIPEnable=0,
    eIPIPv4Enable,
    eIPIPv6Enable,
    eIPULAEnable,
    eIPStatus,
    eIPAlias,
    eIPName,
    eIPLastChange,
    eIPLowerLayers,
    eIPRouter,
    eIPReset,
    eIPMaxMTUSize,
    eIPType,
    eIPLoopback,
    eIPIPv4AddressNumberOfEntries,
#ifdef IPV6_SUPPORT
    eIPIPv6AddressNumberOfEntries,
#endif // IPV6_SUPPORT
    eIPAutoIPEnable
} EDeviceIPInterfaceMembers;

/** @} */ //End of the Doxygen tag TR69_HOSTIF_DEVICE_INTERFACE_IP_DSSTRUCT

/** @defgroup TR_069_DEVICE_IP_INTERFACE_API TR-069 Device.IP.Interface object API.
 *  @ingroup TR_069_DEVICE_IP_API
 *
 *  The Device.IP.Interface table (a stackable interface object) models the layer 3 IP
 *  interface.
 *
 *  Each IP interface can be attached to the IPv4 and/or IPv6 stack. The interface's IP
 *  addresses and prefixes are listed in the IPv4Address, IPv6Address and IPv6Prefix
 *  tables.
 *
 *  @note    <ul>
 *               <li>Support for manipulating Loopback interfaces is OPTIONAL, so the
 *               implementation MAY choose not to create (or allow the ACS to create)
 *               Interface instances of type Loopback.</li>
 *
 *               <li>When the ACS administratively disables the interface, i.e. sets
 *               Enable to false, the interface's automatically-assigned IP addresses
 *               and prefixes MAY be retained. When the ACS administratively enables
 *               the interface, i.e. sets Enable to true, these IP addresses and
 *               prefixes MUST be refreshed. It's up to the implementation to decide
 *               exactly what this means: it SHOULD take all reasonable steps to
 *               refresh everything but if it is unable, for example, to refresh a
 *               prefix that still has a significant lifetime, it might well choose
 *               to retain rather than discard it.</li>
 *
 *               <li>At most one entry in this table (regardless of whether or not it
 *               is enabled) can exist with a given value for Alias, or with a given
 *               value for Name. On creation of a new table entry, the CPE MUST choose
 *               initial values for Alias and Name such that the new entry does not
 *               conflict with any existing entries.</li>
 *           </ul>
 *
 *  @todo    The following parameters are listed as parameters for which "Both Read and
 *           Write support is REQUIRED". However there are no setter or getter functions
 *           declared here for any of these.
 *
 *   <tt>
 *       <ul><li>IPv6Enable</li></ul>
 *   </tt>
 */

/** @addtogroup TR_069_DEVICE_IP_INTERFACE_GETTER_API TR-069 Device.IP.Interface Getter API.
 *  @ingroup TR_069_DEVICE_IP_INTERFACE_API
 *
 *  \section dev_ipinterface_getter TR-069 Device.IP.Interface object Getter API
 *
 *  This is the getter group of API for the <b>Device.IP.Interface.{i}</b> object.
 *
 *  The interface for all functions is identical and is described here.
 *
 *  @param[in]   *name     This is the complete path name of the parameter extracted from
 *                         soap message, e.g. ::get_Device_IP_Interface_Status.
 *                         In this case, the path is "Device.IP.Interface.{i}.Status".
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
 * @brief This class provides the hostIf IP interface for getting IP interface information.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_CLASSES
 */
class hostIf_IPInterface {

    static  GHashTable  *ifHash;

    static GMutex *m_mutex;

    static IPInterface stIPInterfaceInstance;

    int dev_id;

    char nameOfInterface[IF_NAMESIZE];

    hostIf_IPInterface(int dev_id);

    ~hostIf_IPInterface() {};

    void refreshInterfaceName ();

    unsigned int getIPAddressNumberOfEntries (sa_family_t address_family);

    int set_Interface_Reset (unsigned int value);

    int set_Interface_Enable (int value);

    int set_Interface_Mtu (unsigned int value);

    bool bCalledEnable;
    bool bCalledIPv4Enable;
    bool bCalledIPv6Enable;
    bool bCalledStatus;
    bool bCalledName;
    bool bCalledReset;
    bool bCalledMaxMTUSize;
    bool bCalledType;
    bool bCalledLoopback;
    bool bCalledIPv4AddressNumberOfEntries;

    unsigned int backupIPv4AddressNumberOfEntries;
    unsigned int backupMaxMTUSize;

    bool backupLoopback;
    bool backupReset;
    bool backupIPv4Enable;
    bool backupIPv6Enable;
    bool backupEnable;

    char backupType[TR69HOSTIFMGR_MAX_PARAM_LEN];
    char backupName[TR69HOSTIFMGR_MAX_PARAM_LEN];
    char backupStatus[TR69HOSTIFMGR_MAX_PARAM_LEN];


public:

    static hostIf_IPInterface* getInstance(int dev_id);

    static void closeInstance(hostIf_IPInterface*);

    static GList* getAllInstances();

    static void closeAllInstances();

    static void getLock();

    static void releaseLock();

    int handleGetMsg (const char* pSetting, HOSTIF_MsgData_t* stMsgData);
    int handleSetMsg (const char* pSetting, HOSTIF_MsgData_t* stMsgData);

    static void getActiveFlags (char* nameOfInterface, struct ifreq& ifr);
    static bool isLoopback (char* nameOfInterface);
    static bool isEnabled (char* nameOfInterface);
    static int getMTU (char* nameOfInterface);

    void getInterfaceOperationalState (char* operationalState);
    unsigned int getIPv4AddressNumberOfEntries ();
    unsigned int getIPv6AddressNumberOfEntries ();
    unsigned int getIPv6PrefixNumberOfEntries ();

    int get_Interface_Enable(HOSTIF_MsgData_t *stMsgData, bool *pChanged = false);

    int get_Interface_IPv4Enable(HOSTIF_MsgData_t *stMsgData, bool *pChanged = false);

    int get_Interface_IPv6Enable(HOSTIF_MsgData_t *stMsgData, bool *pChanged = false);

    int get_Interface_ULAEnable(HOSTIF_MsgData_t *stMsgData, bool *pChanged = false);

    int get_Interface_Status(HOSTIF_MsgData_t *stMsgData, bool *pChanged = false);

    int get_Interface_Alias(HOSTIF_MsgData_t *stMsgData, bool *pChanged = false);

    int get_Interface_Name(HOSTIF_MsgData_t *stMsgData, bool *pChanged = false);

    int get_Interface_LastChange(HOSTIF_MsgData_t *stMsgData, bool *pChanged = false);

    /**
     * @brief    Get IP Interface LowerLayers.
     *
     * This function provides a comma-separated list (maximum length 1024) of strings.  Each
     * list item MUST be the path name of an interface object that is stacked immediately
     * below this interface object.
     *
     * @note    If the referenced object is deleted, the corresponding item MUST be removed
     *          from the list.
     *
     * @note    LowerLayers MUST be an empty string and <b>read-only</b> when Type is Loopback,
     *          Tunnel, or Tunneled.
     *
     * See @ref dev_ipinterface_getter
     *
     */
    int get_Interface_LowerLayers(HOSTIF_MsgData_t *stMsgData, bool *pChanged = false);

    int get_Interface_Router(HOSTIF_MsgData_t *stMsgData, bool *pChanged = false);

    int get_Interface_Reset(HOSTIF_MsgData_t *stMsgData, bool *pChanged = false);

    int get_Interface_MaxMTUSize(HOSTIF_MsgData_t *stMsgData, bool *pChanged = false);

    int get_Interface_Type(HOSTIF_MsgData_t *stMsgData, bool *pChanged = false);

    int get_Interface_Loopback(HOSTIF_MsgData_t *stMsgData, bool *pChanged = false);

    int get_Interface_IPv4AddressNumberOfEntries(HOSTIF_MsgData_t *stMsgData);

#ifdef IPV6_SUPPORT
    /**
     * @brief    Get the IP Interface IPv6AddressNumberOfEntries.
     *
     * This function provides the number of entries in the IPv4 Address table.
     *
     * See @ref dev_ipinterface_getter
     *
     */
    int get_Interface_IPv6AddressNumberOfEntries (HOSTIF_MsgData_t *stMsgData);

    int get_Interface_IPv6PrefixNumberOfEntries (HOSTIF_MsgData_t *stMsgData);
#endif

    /**
     * @brief    Get the status (enabled/disabled) of Auto-IP on an IP Interface.
     *
     * This function indicates whether or not (<tt>true</tt> or <tt>false</tt>) auto-IP
     * is enabled for this IP interface.
     *
     * @note     <ul>
     *               <li>This mechanism is only used with IPv4.</li>
     *               <li>When auto-IP is enabled on an interface, an IPv4Address object
     *               will dynamically be created and configured with auto-IP parameter
     *               values.</li>
     *               <li>The exact conditions under which an auto-IP address is created
     *               (e.g. always when enabled or only in absence of dynamic IP
     *               addressing) is implementation specific.</li>
     *           </ul>
     *
     * @par
     *
     * See @ref dev_ipinterface_getter
     *
     */
    int get_Interface_AutoIPEnable(HOSTIF_MsgData_t *stMsgData, bool *pChanged = false);

    /* End of TR_069_DEVICE_IP_INTERFACE_GETTER_API doxygen group */
    /**
     * @}
     */

    /** @addtogroup TR_069_DEVICE_IP_INTERFACE_SETTER_API TR-069 Device.IP.Interface Setter API.
     *  @ingroup TR_069_DEVICE_IP_INTERFACE_API
     *
     *  \section dev_ipinterface_setter TR-069 Device.IP.Interface object Setter API
     *
     *  This is the setter group of API for the <b>Device.IP.Interface.{i}</b> object.
     *
     *  The interface for all functions is identical and is described here.
     *
     *  @param[in]   *name     This is the complete path name of the parameter extracted from
     *                         soap message, e.g. ::set_Device_IP_Interface_LowerLayers.
     *                         In this case, the path is "Device.IP.Interface.{i}.LowerLayers".
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
     * @brief    Set the status (enabled/disabled) of IP Interface.
     *
     * This function enables (<tt>true</tt>) or disables (<tt>false</tt>) this IP
     * interface (regardless of IPv4Enable and IPv6Enable).
     *
     * See @ref dev_ipinterface_setter
     *
     */
    int set_Interface_Enable(HOSTIF_MsgData_t *stMsgData);

    /**
     * @brief    Attach/Detach an IP interface to/from the IPv4 stack.
     *
     * This function attaches or detaches this IP interface to/from the IPv4 stack. If
     * set to <tt>true</tt>, then this interface is attached to the IPv4 stack. If set
     * to false, then this interface is detached from the IPv4 stack.
     *
     * @note     <ul>
     *               <li>Once detached from the IPv4 stack, the interface will now no
     *               longer be able to pass IPv4 packets, and will be operationally down
     *               (unless also attached to an enabled IPv6 stack).</li>
     *
     *               <li>For an IPv4 capable device, if IPv4Enable is not present this
     *               interface SHOULD be permanently attached to the IPv4 stack.</li>
     *
     *               <li>IPv4Enable is independent of Enable, and that to administratively
     *               enable an interface for IPv4 it is necessary for both Enable and
     *               IPv4Enable to be true.</li>
     *           </ul>
     *
     * See @ref dev_ipinterface_setter
     *
     */
    int set_Interface_IPv4Enable(HOSTIF_MsgData_t *stMsgData);

    int set_Interface_ULAEnable(HOSTIF_MsgData_t *stMsgData);

    int set_Interface_Alias(HOSTIF_MsgData_t *stMsgData);

    int set_Interface_LowerLayers(HOSTIF_MsgData_t *stMsgData);

    int set_Interface_Router(HOSTIF_MsgData_t *stMsgData);

    int set_Interface_Reset(HOSTIF_MsgData_t *stMsgData);

    int set_Interface_MaxMTUSize(HOSTIF_MsgData_t *stMsgData);

    int set_Interface_Loopback(HOSTIF_MsgData_t *stMsgData);

    int set_Interface_AutoIPEnable(HOSTIF_MsgData_t *stMsgData);

    /* End of TR_069_DEVICE_IP_INTERFACE_SETTER_API doxygen group */
    /**
     * @}
     */

};

#endif /* DEVICE_IP_INTERFACE_H_ */


/** @} */
/** @} */
