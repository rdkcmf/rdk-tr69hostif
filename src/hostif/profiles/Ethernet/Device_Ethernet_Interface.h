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
 * @file Device_Ethernet_Interface.h
 *
 * @brief TR-069 Device.Ethernet.Interface object Public API.
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


#ifndef DEVICE_ETHERNET_INTERFACE_H_
#define DEVICE_ETHERNET_INTERFACE_H_

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/

#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "hostIf_updateHandler.h"


#define S_LENGTH 24
#define A_LENGTH 64
#define LL_LENGTH 1024
#define LENGTH_PARAMETER 64

typedef enum EEthInterfaceMembers
{
    eEnable=0,
    eStatus,
    eName,
    eLastChange,
    eLowerLayers,
    eUpstream,
    eMACAddress,
    eMaxBitRate,
    eDuplexMode
} EEthInterfaceMembers;

typedef struct Device_Ethernet_Interface
{
    bool enable;
    char status[S_LENGTH];
    char alias[A_LENGTH];
    char name[A_LENGTH];
    unsigned int lastChange;
    char lowerLayers[LL_LENGTH];
    bool upStream;
    char mACAddress[S_LENGTH];
    int maxBitRate;
    char duplexMode[S_LENGTH];
} EthernetInterface;


/** @defgroup TR_069_DEVICE_ETHERNET_API TR-069 Device.Ethernet object API.
 *  @ingroup TR_069_API
 *
 *  The Device.Ethernet object models several Ethernet interface objects, each
 *  representing a different stack layer, including: Interface, Link, and
 *  VLANTermination.
 *
 *  Interface is media-specific and models a port, the PHY layer, and the MAC layer.
 *
 *  Link is media-independent and models the Logical Link Control (LLC) layer.
 *
 *  A VLANTermination, when present, is expected to be stacked on top of Link objects
 *  to receive and send frames with a configured VLANID.
 */

/** @defgroup TR_069_DEVICE_ETHERNET_INTERFACE_API TR-069 Device.Ethernet.Interface object API.
 *  @ingroup TR_069_DEVICE_ETHERNET_API
 *
 *  The Device.Ethernet.Interface is the Ethernet interface table (a stackable
 *  interface object). This table models physical Ethernet ports, but in terms of the
 *  interface stack it also models the PHY and MAC level Ethernet interface.
 *
 *  @note    At most one entry in this table can exist with a given value for Alias,
 *           or with a given value for Name.
 */

/** @addtogroup TR_069_DEVICE_ETHERNET_GETTER_API TR-069 Device.Ethernet Getter API.
 *  @ingroup TR_069_DEVICE_ETHERNET_API
 *
 *  \section dev_ethernet_getter TR-069 Device.Ethernet object Getter API
 *
 *  This is the getter group of API for the <b>Device.Ethernet</b> object.
 *
 *  The interface for all functions is identical and is described here.
 *
 *  @param[in]   *name     This is the complete path name of the parameter extracted from
 *                         soap message, e.g. ::get_Device_Ethernet_InterfaceNumberOfEntries.
 *                         In this case, the path is "Device.Ethernet.InterfaceNumberOfEntries".
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

class hostIf_EthernetInterface {


    hostIf_EthernetInterface(int dev_id);

    ~hostIf_EthernetInterface();

    static  GHashTable  *ifHash;
    static  GHashTable  *m_notifyHash;
    static GMutex *m_mutex;

    int dev_id;

    bool backupEnable;
    bool backupUpstream;
    int backupMaxBitRate;
    char backupStatus[TR69HOSTIFMGR_MAX_PARAM_LEN+1];
    char backupName[TR69HOSTIFMGR_MAX_PARAM_LEN+1];
    char backupMACAddress[TR69HOSTIFMGR_MAX_PARAM_LEN+1];
    char backupDuplexMode[TR69HOSTIFMGR_MAX_PARAM_LEN+1];

    bool bCalledEnable;
    bool bCalledStatus;
    bool bCalledName;
    bool bCalledUpstream;
    bool bCalledMACAddress;
    bool bCalledMaxBitRate;
    bool bCalledDuplexMode;

public:

    static EthernetInterface stEthInterface;

    static hostIf_EthernetInterface *getInstance(int dev_id);

    static void closeInstance(hostIf_EthernetInterface *);

    static GList* getAllInstances();

    static void closeAllInstances();

    static void getLock();

    static void releaseLock();

    static GHashTable* getNotifyHash();

    /**
     * @brief    Get the number of entries in the Interface table.
     *
     * This function provides the number of entries in the Device.Ethernet.Interface.{i} table.
     *
     * See @ref dev_ethernet_getter
     *
     */


    static int get_Device_Ethernet_InterfaceNumberOfEntries(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /* End of TR_069_DEVICE_ETHERNET_GETTER_API doxygen group */
    /**
     * @}
     */


    /** @addtogroup TR_069_DEVICE_ETHERNET_INTERFACE_GETTER_API TR-069 Device.Ethernet.Interface Getter API.
     *  @ingroup TR_069_DEVICE_ETHERNET_INTERFACE_API
     *
     *  \section dev_ethernet_interface_getter TR-069 Device.Ethernet.Interface object Getter API
     *
     *  This is the getter group of API for the <b>Device.Ethernet.Interface.{i}</b> object.
     *
     *  The interface for all functions is identical and is described here.
     *
     *  @param[in]   *name     This is the complete path name of the parameter extracted from
     *                         soap message, e.g. ::get_Device_Ethernet_Interface_Status.
     *                         In this case, the path is "Device.Ethernet.Interface.{i}.Status".
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
     * @brief    Get the status (enabled/disabled) of an Ethernet interface.
     *
     * This function provides the value (<tt>true/false</tt>) of the Enable status of this
     * Ethernet interface.
     *
     * See @ref dev_ethernet_interface_getter
     *
     */
    int get_Device_Ethernet_Interface_Enable(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the status of an Ethernet Interface.
     *
     * This function provides the current operational state of this Ethernet interface.
     * This is an enumeration of:
     *  <tt>
     *      <ul><li>Up</li>
     *          <li>Down</li>
     *          <li>Unknown</li>
     *          <li>Dormant</li>
     *          <li>NotPresent</li>
     *          <li>LowerLayerDown</li>
     *          <li>Error (OPTIONAL)</li></ul>
     *  </tt>
     *
     * @note    <ul><li>When Enable is false then Status SHOULD normally be <tt>Down</tt> (or
     *              <tt>NotPresent</tt> or <tt>Error</tt> if there is a fault condition
     *              on the interface).</li>
     *
     *              <li>When Enable is changed to <tt>true</tt> then ...
     *                  <ul><li>Status SHOULD change to <tt>Up</tt> if and only if the
     *                  interface is able to transmit and receive network traffic.</li>
     *                  <li>Status SHOULD change to <tt>Dormant</tt> if and only if the
     *                  interface is operable but is waiting for external actions before
     *                  it can transmit and receive network traffic (and subsequently
     *                  change to <tt>Up</tt> if still operable when the expected actions
     *                  have completed).</li>
     *                  <li>Status SHOULD change to <tt>LowerLayerDown</tt> if and only if
     *                  the interface is prevented from entering the <tt>Up</tt> state
     *                  because one or more of the interfaces beneath it is down.</li>
     *                  <li>Status SHOULD remain in the <tt>Error</tt> state if there is
     *                  an error or other fault condition detected on the interface.</li>
     *                  <li>Status SHOULD remain in the <tt>NotPresent</tt> state if the
     *                  interface has missing (typically hardware) components.</li>
     *                  <li>Status SHOULD change to <tt>Unknown</tt> if the state of the
     *                  interface can not be determined for some reason.</li></ul>
     *              </li>
     *              <li>This parameter is based on ifOperStatus from [RFC 2863, The
     *              Interfaces Group MIB, IETF, 2000.].</li>
     *          </ul>
     *
     * @par
     *
     * See @ref dev_ethernet_interface_getter
     *
     */
    int get_Device_Ethernet_Interface_Status(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the instance handle for an Ethernet Interface.
     *
     * This function provides a non-volatile handle used to reference this Ethernet
     * interface instance. Alias provides a mechanism for an ACS to label this instance
     * for future reference.
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
     * See @ref dev_ethernet_interface_getter
     *
     */

    int get_Device_Ethernet_Interface_Alias(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get an Ethernet Interface Name.
     *
     * This function provides the textual name of this Ethernet interface as assigned
     * by the CPE.
     *
     * See @ref dev_ethernet_interface_getter
     *
     */

    int get_Device_Ethernet_Interface_Name(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the time since an Ethernet interface's last change of status.
     *
     * This function provides the accumulated time in seconds since this Ethernet
     * interface entered its current operational state.
     *
     * See @ref dev_ethernet_interface_getter
     *
     */

    int get_Device_Ethernet_Interface_LastChange(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get Ethernet Interface LowerLayers.
     *
     * This function provides a comma-separated list (maximum length 1024) of strings.  Each
     * list item MUST be the path name of an interface object that is stacked immediately
     * below this Ethernet interface object.
     *
     * @note    <ul>
     *              <li>If the referenced object is deleted, the corresponding item MUST
     *              be removed from the list.</li>
     *
     *              <li>Since Interface is a layer 1 interface, it is expected that
     *              <tt>LowerLayers</tt> will not be used.</li>
     *          </ul>
     *
     * See @ref dev_ethernet_interface_getter
     *
     */

    int get_Device_Ethernet_Interface_LowerLayers(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the Upstream device type for an Ethernet Interface.
     *
     * This function indicates whether the interface points towards the Internet (<tt>true</tt>)
     * or towards End Devices (false).
     *
     * For example:
     *     <ul>
     *         <li>For an Internet Gateway Device, Upstream will be <tt>true</tt> for all
     *         WAN interfaces and <tt>false</tt> for all LAN interfaces.</li>
     *         <li>For a standalone WiFi Access Point that is connected via Ethernet to
     *         an Internet Gateway Device, Upstream will be <tt>true</tt> for the Ethernet
     *         interface and <tt>false</tt> for the WiFi Radio interface.</li>
     *         <li>For an End Device, Upstream will be <tt>true</tt> for all interfaces.</li>
     *     </ul>
     *
     * See @ref dev_ethernet_interface_getter
     *
     */

    int get_Device_Ethernet_Interface_Upstream(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the MAC Address of at Ethernet Interface.
     *
     * This function provides the MAC Address of this Ethernet interface.
     *
     * @note    This is not necessarily the same as the Ethernet header source or
     *          destination MAC address, which is associated with the IP interface and is
     *          modelled via the Ethernet.Link.{i}.MACAddress parameter.
     *
     * See @ref dev_ethernet_interface_getter
     *
     */

    int get_Device_Ethernet_Interface_MACAddress(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the maximum bit rate attainable on an Ethernet Interface.
     *
     * This function provides the maximum upstream and downstream PHY bit rate supported
     * by this Ethernet interface (expressed in Mbps).
     *
     * @note     A value of -1 indicates automatic selection of the maximum bit rate.
     *
     * See @ref dev_ethernet_interface_getter
     *
     */

    int get_Device_Ethernet_Interface_MaxBitRate(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /**
     * @brief    Get the Duplex Mode available on an Ethernet Interface.
     *
     * This function provides a string indicating the duplex mode available to this
     * Ethernet connection. This is an enumeration of:
     *
     * <tt>
     *     <ul><li>Half</li>
     *         <li>Full</li>
     *         <li>Auto</li></ul>
     * </tt>
     *
     * See @ref dev_ethernet_interface_getter
     *
     */

    int get_Device_Ethernet_Interface_DuplexMode(HOSTIF_MsgData_t *,bool *pChanged = NULL);

    /* End of TR_069_DEVICE_ETHERNET_INTERFACE_GETTER_API doxygen group */
    /**
     * @}
     */

    /** @addtogroup TR_069_DEVICE_ETHERNET_INTERFACE_SETTER_API TR-069 Device.Ethernet.Interface Setter API.
     *  @ingroup TR_069_DEVICE_ETHERNET_INTERFACE_API
     *
     *  \section dev_ethernet_interface_setter TR-069 Device.Ethernet.Interface object Setter API
     *
     *  This is the setter group of API for the <b>Device.Ethernet.Interface.{i}</b> object.
     *
     *  The interface for all functions is identical and is described here.
     *
     *  @param[in]   *name     This is the complete path name of the parameter extracted from
     *                         soap message, e.g. ::set_Device_Ethernet_Interface_LowerLayers.
     *                         In this case, the path is "Device.Ethernet.Interface.{i}.LowerLayers".
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
     * @brief    Set the status (enabled/disabled) of Ethernet Interface.
     *
     * This function enables (<tt>true</tt>) or disables (<tt>false</tt>) this Ethernet
     * interface.
     *
     * See @ref dev_ethernet_interface_setter
     *
     */

    int set_Device_Ethernet_Interface_Enable(HOSTIF_MsgData_t* );

    /**
     * @brief    Set the instance handle for an Ethernet Interface.
     *
     * This function sets a non-volatile handle used to reference this Ethernet
     * interface instance. Alias provides a mechanism for an ACS to label this instance
     * for future reference.
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
     * See @ref dev_ethernet_interface_setter
     *
     */

    int set_Device_Ethernet_Interface_Alias(HOSTIF_MsgData_t* );

    /**
     * @brief    Set Ethernet Interface LowerLayers.
     *
     * Given a comma-separated list (maximum length 1024) of strings, each list item being
     * the path name of an Ethernet interface object, this function MUST stack each item
     * in the list immediately below this interface object.
     *
     * @note    <ul>
     *              <li>If the referenced object is deleted, the corresponding item MUST
     *              be removed from the list.</li>
     *
     *              <li>Since Interface is a layer 1 interface, it is expected that
     *              <tt>LowerLayers</tt> will not be used.</li>
     *          </ul>
     *
     * See @ref dev_ethernet_interface_setter
     *
     */

    int set_Device_Ethernet_Interface_LowerLayers(HOSTIF_MsgData_t* );

    /**
     * @brief    Set the maximum bit rate attainable on an Ethernet Interface.
     *
     * This function sets the maximum upstream and downstream PHY bit rate supported
     * by this Ethernet interface (expressed in Mbps).
     *
     * @note     A value of -1 indicates automatic selection of the maximum bit rate.
     *
     * See @ref dev_ethernet_interface_setter
     *
     */

    int set_Device_Ethernet_Interface_MaxBitRate(HOSTIF_MsgData_t* );

    /**
     * @brief    Set the Duplex Mode available on an Ethernet Interface.
     *
     * This function sets the string indicating the duplex mode available to this
     * Ethernet connection. This is an enumeration of:
     *
     * <tt>
     *     <ul><li>Half</li>
     *         <li>Full</li>
     *         <li>Auto</li></ul>
     * </tt>
     *
     * See @ref dev_ethernet_interface_setter
     *
     */

    int set_Device_Ethernet_Interface_DuplexMode(HOSTIF_MsgData_t* );

    /* End of TR_069_DEVICE_ETHERNET_INTERFACE_SETTER_API doxygen group */
    /**
     * @}
     */
};

#endif /* DEVICE_ETHERNET_INTERFACE_H_ */


/** @} */
/** @} */
