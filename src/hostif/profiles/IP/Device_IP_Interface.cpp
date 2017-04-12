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
 * @file Device_IP_Interface.cpp
 * @brief This source file contains the APIs of device IP interface.
 */

/**
 * @file Device_IP_Interface.c
 *
 * @brief Device.IP.Interface. API Implementation.
 *
 * This is the implementation of the Device.IP.Interface. API.
 *
 * @par Document
 * TBD Relevant design or API documentation.
 *
 */


/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#include "hostIf_utils.h"
#include "Device_IP_Interface.h"
#include "Device_IP_Interface_Stats.h"
#include <ifaddrs.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <unistd.h>
#include "Device_IP.h"


// TODO: fix potential bug with initialization, as structure definition now has a "#ifdef IPV6_SUPPORT"
IPInterface hostIf_IPInterface::stIPInterfaceInstance = {FALSE,FALSE,FALSE,FALSE,{"Down"},{'\0'},{'\0'},0,{'\0'},{'\0'},FALSE,0,{"Normal"},FALSE,0,
#ifdef IPV6_SUPPORT
        0,
#endif
        FALSE };

GHashTable *hostIf_IPInterface::ifHash = NULL;

int hostIf_IPInterface::set_Interface_Enable (int value)
{
    LOG_ENTRY_EXIT;

    char cmd[BUFF_LENGTH] = { 0 };
    if (FALSE == value)
    {
        sprintf (cmd, "ifconfig %s down", nameOfInterface);
        //RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s(): interfaceName = %s Disabled\n",__FUNCTION__, nameOfInterface);
    }
    else if (TRUE == value)
    {
        sprintf (cmd, "ifconfig %s up", nameOfInterface);
        //RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s(): interfaceName = %s Enabled\n",__FUNCTION__, nameOfInterface);
    }

    return (system (cmd) < 0) ? NOK : OK;
}

int hostIf_IPInterface::set_Interface_Reset (unsigned int value)
{
    LOG_ENTRY_EXIT;

    if (TRUE == value)
    {
        char cmd[BUFF_LENGTH] = { 0 };

        sprintf (cmd, "ifdown %s", nameOfInterface);
        if (system (cmd) < 0)
            return NOK;

        sprintf (cmd, "ifup %s", nameOfInterface);
        if (system (cmd) < 0)
            return NOK;

        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s(): interfaceName = %s Reset Done\n", __FUNCTION__, nameOfInterface);
    }

    return OK;
}

int hostIf_IPInterface::set_Interface_Mtu (unsigned int value)
{
    LOG_ENTRY_EXIT;

    char cmd[BUFF_LENGTH] = { 0 };
    sprintf (cmd, "ifconfig %s mtu %d", nameOfInterface, value);
    if (system (cmd) < 0)
        return NOK;

    //RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s(): interfaceName = %s MaxMTUSize Set with value %u\n",__FUNCTION__, nameOfInterface,value);

    return OK;
}

void hostIf_IPInterface::refreshInterfaceName ()
{
    nameOfInterface[0] = 0;
    if (NULL == hostIf_IP::getInterfaceName (dev_id, nameOfInterface))
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: error getting interface name for Device.IP.Interface.%d\n", __FUNCTION__, dev_id);
}

/**
 * @brief Class Constructor of the class hostIf_IPInterface.
 *
 * It will initialize the device id. Initialize the type and name to empty string.
 *
 * @param[in] dev_id Device identification number.
 */
hostIf_IPInterface::hostIf_IPInterface(int dev_id):
    dev_id(dev_id),
    bCalledEnable(false),
    bCalledIPv4Enable(false),
    bCalledIPv6Enable(false),
    bCalledStatus(false),
    bCalledName(false),
    bCalledReset(false),
    bCalledMaxMTUSize(false),
    bCalledType(false),
    bCalledLoopback(false),
    bCalledIPv4AddressNumberOfEntries(false),
    backupLoopback(false),
    backupReset(false),
    backupIPv4Enable(false),
    backupIPv6Enable(false),
    backupEnable(false),
    backupIPv4AddressNumberOfEntries(0),
    backupMaxMTUSize(0)

{
    strcpy(backupType,"");
    strcpy(backupName,"");
    strcpy(backupName,"");
}

hostIf_IPInterface* hostIf_IPInterface::getInstance(int dev_id)
{
    LOG_ENTRY_EXIT;

    hostIf_IPInterface* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_IPInterface *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_IPInterface(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create MoCA Interface instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }

    // make sure returned instance has interface name set
    if (pRet)
        pRet->refreshInterfaceName ();

    return pRet;
}

GList* hostIf_IPInterface::getAllInstances()
{
    LOG_ENTRY_EXIT;

    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_IPInterface::closeInstance(hostIf_IPInterface *pDev)
{
    LOG_ENTRY_EXIT;

    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_IPInterface::closeAllInstances()
{
    LOG_ENTRY_EXIT;

    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_IPInterface* pDev = (hostIf_IPInterface *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

int hostIf_IPInterface::handleGetMsg (const char* pSetting, HOSTIF_MsgData_t* stMsgData)
{
    LOG_ENTRY_EXIT;

    int ret = NOT_HANDLED;

    if (!strcasecmp (pSetting, "Enable"))
    {
        ret = get_Interface_Enable (stMsgData);
    }
    else if (!strcasecmp (pSetting, "IPv4Enable"))
    {
        ret = get_Interface_IPv4Enable (stMsgData);
    }
    else if (!strcasecmp (pSetting, "IPv6Enable"))
    {
        ret = get_Interface_IPv6Enable (stMsgData);
    }
    else if (!strcasecmp (pSetting, "ULAEnable"))
    {
        ret = get_Interface_ULAEnable (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Status"))
    {
        ret = get_Interface_Status (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Alias"))
    {
        ret = get_Interface_Alias (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Name"))
    {
        ret = get_Interface_Name (stMsgData);
    }
    else if (!strcasecmp (pSetting, "LastChange"))
    {
        ret = get_Interface_LastChange (stMsgData);
    }
    else if (!strcasecmp (pSetting, "LowerLayers"))
    {
        ret = get_Interface_LowerLayers (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Router"))
    {
        ret = get_Interface_Router (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Reset"))
    {
        ret = get_Interface_Reset (stMsgData);
    }
    else if (!strcasecmp (pSetting, "MaxMTUSize"))
    {
        ret = get_Interface_MaxMTUSize (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Type"))
    {
        ret = get_Interface_Type (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Loopback"))
    {
        ret = get_Interface_Loopback (stMsgData);
    }
    else if (!strcasecmp (pSetting, "IPv4AddressNumberOfEntries"))
    {
        ret = get_Interface_IPv4AddressNumberOfEntries (stMsgData);
    }
#ifdef IPV6_SUPPORT
    else if (!strcasecmp (pSetting, "IPv6AddressNumberOfEntries"))
    {
        ret = get_Interface_IPv6AddressNumberOfEntries (stMsgData);
    }
    else if (!strcasecmp (pSetting, "IPv6PrefixNumberOfEntries"))
    {
        ret = get_Interface_IPv6PrefixNumberOfEntries (stMsgData);
    }
#endif // IPV6_SUPPORT
    else if (!strcasecmp (pSetting, "AutoIPEnable"))
    {
        ret = get_Interface_AutoIPEnable (stMsgData);
    }

    return ret;
}

int hostIf_IPInterface::handleSetMsg (const char* pSetting, HOSTIF_MsgData_t* stMsgData)
{
    LOG_ENTRY_EXIT;

    int ret = NOT_HANDLED;

    if (!strcasecmp (pSetting, "Enable"))
    {
        ret = set_Interface_Enable (stMsgData);
    }
    else if (!strcasecmp (pSetting, "IPv4Enable"))
    {
        ret = set_Interface_IPv4Enable (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Reset"))
    {
        ret = set_Interface_Reset (stMsgData);
    }
    else if (!strcasecmp (pSetting, "MaxMTUSize"))
    {
        ret = set_Interface_MaxMTUSize (stMsgData);
    }

    return ret;
}

/****************************************************************************************************************************************************/
// Device.IP.Interface. Profile. Getters:
/****************************************************************************************************************************************************/

/**
 * @brief This function gets the status to enabled or disabled of an IP Interface.
 * It provides the values such as 'true' or 'false' to Enable the status of IP interface.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::get_Interface_Enable(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    stIPInterfaceInstance.enable = isEnabled (nameOfInterface);

    if(bCalledEnable && pChanged && (backupEnable != stIPInterfaceInstance.enable))
    {
        *pChanged = true;
    }
    bCalledEnable = true;
    backupEnable = stIPInterfaceInstance.enable;
    put_int(stMsgData->paramValue,stIPInterfaceInstance.enable);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = 1;

    return OK;
}

/**
 * @brief This function gets the status of the IPv4 stack attachment for an IP interface.
 * It indicates whether or not this IP interface is attached to the IPv4
 * stack. If set to 'true', then this interface is attached to the IPv4 stack.
 * If set to 'false', then this interface is detached from the IPv4 stack.
 *
 * @note  - Once detached from the IPv4 stack, the interface will now no
 *         longer be able to pass IPv4 packets, and will be operationally down
 *         unless until it attached to an enabled IPv6 stack.
 *
 *        - For an IPv4 capable device, if IPv4Enable is not present this
 *          interface SHOULD be permanently attached to the IPv4 stack.
 *
 *        - IPv4Enable is independent of Enable, and that to administratively
 *          enable an interface for IPv4 it is necessary for both Enable and
 *          IPv4Enable to be 'true'.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::get_Interface_IPv4Enable(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    // Separating this out from Enable/Status as they are independent variables as per
    // the spec: https://www.broadband-forum.org/cwmp/tr-181-2-11-0.html
    // "Note that IPv4Enable is independent of Enable, and that to administratively enable
    // an interface for IPv4 it is necessary for both Enable and IPv4Enable to be true."

    stIPInterfaceInstance.iPv4Enable = (getIPv4AddressNumberOfEntries () >= 1);

    // If "Enable" is false, "Status" can be marked "Down" as per this from the spec:
    // "Enable - "Enables or disables the interface (regardless of IPv4Enable and IPv6Enable)."
    // If both "Enable" and "IPv4Enable" are true, "Status" = Up. But if "IPv4Enable" = false,
    // can't say "Status" = "Down"; need to also check "IPv6Enable" = false because spec says:
    // "Once detached from the IPv4 stack, the interface will now no longer be able to
    // pass IPv4 packets, and will be operationally down (unless also attached to an
    // enabled IPv6 stack)."
    strcpy (stIPInterfaceInstance.status,
            stIPInterfaceInstance.enable && (stIPInterfaceInstance.iPv4Enable || stIPInterfaceInstance.iPv6Enable) ? STATE_UP : STATE_DOWN);
    // TODO: Why write to Status when read requested on IPv4Enable?

    //RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s(): Enable: %d IPv4Enable: %d Status: %s \n",__FUNCTION__,stIPInterfaceInstance.enable,stIPInterfaceInstance.iPv4Enable,stIPInterfaceInstance.status);

    if(bCalledIPv4Enable && pChanged && (backupIPv4Enable != stIPInterfaceInstance.iPv4Enable))
    {
        *pChanged = true;
    }
    bCalledIPv4Enable = true;
    backupIPv4Enable = stIPInterfaceInstance.iPv4Enable;
    put_int(stMsgData->paramValue,stIPInterfaceInstance.iPv4Enable);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = 1;

    return OK;
}

int hostIf_IPInterface::get_Interface_IPv6Enable(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    stIPInterfaceInstance.iPv6Enable = (getIPv6AddressNumberOfEntries () >= 1);

    strcpy (stIPInterfaceInstance.status,
            stIPInterfaceInstance.enable && (stIPInterfaceInstance.iPv4Enable || stIPInterfaceInstance.iPv6Enable) ? STATE_UP : STATE_DOWN);
    // TODO: Why write to Status when read requested on IPv6Enable?

    //RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s(): Enable: %d IPv6Enable: %d Status: %s \n",__FUNCTION__,stIPInterfaceInstance.enable,stIPInterfaceInstance.iPv6Enable,stIPInterfaceInstance.status);

    if(bCalledIPv6Enable && pChanged && (backupIPv6Enable != stIPInterfaceInstance.iPv6Enable))
    {
        *pChanged = true;
    }
    bCalledIPv6Enable = true;
    backupIPv6Enable = stIPInterfaceInstance.iPv6Enable;
    put_int(stMsgData->paramValue,stIPInterfaceInstance.iPv6Enable);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = 1;

    return OK;
}

/**
 * @brief This function gets the status 'enabled' or 'disabled' of ULA(Unique local address)
 * generation for an IP Interface. It indicates the status as 'true' or 'false' of the ULA
 * generation and use this IP interface is enabled. Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::get_Interface_ULAEnable(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Parameter Not Supported \n",__FUNCTION__);

    return NOK;
}

void hostIf_IPInterface::getInterfaceOperationalState (char* operationalState)
{
    LOG_ENTRY_EXIT;

    struct ifreq ifr;
    getActiveFlags (nameOfInterface, ifr);
    strcpy (operationalState, (ifr.ifr_flags & IFF_UP) ? STATE_UP : STATE_DOWN);
}

/**
 * @brief This function gets the IP interface Status. It provides the current operational state
 * of the Interface. Possible state are Up, Down, Unknown, Dormant, NotPresent, LowerLayerDown
 * Error (OPTIONAL).
 * @note    <ul><li> When Enable flag is false then Status SHOULD normally be Down (or NotPresent
 *          or Error if there is a fault condition on the interface).</li>
 *          <li> When Enable is changed to true then ...
 *             <ul><li>Status SHOULD change to Up if and only if the interface is able to
 *                 transmit and receive network traffic.</li>
 *                 <li>Status SHOULD change to Dormant if and only if the interface is operable
 *                 but is waiting for external actions before it can transmit and receive
 *                 network traffic (and subsequently change to Up if still operable when the
 *                 expected actions have completed).</li>
 *                 <li>Status SHOULD change to LowerLayerDown if and only if the interface is
 *                 prevented from entering the Up state because one or more of the
 *                 interfaces beneath it is down.</li>
 *                 <li>Status SHOULD remain in the Error state if there is an error or other
 *                 fault condition detected on the interface.</li>
 *                 <li>Status SHOULD remain in the NotPresent state if the interface has missing
 *                 (typically hardware) components.</li>
 *                 <li>Status SHOULD change to Unknown if the state of the interface can not be
 *                 determined for some reason.</li></ul>
 *           </li></ul>
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::get_Interface_Status(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    getInterfaceOperationalState (stIPInterfaceInstance.status);

    if(bCalledStatus && pChanged && strncmp(stIPInterfaceInstance.status, backupStatus,TR69HOSTIFMGR_MAX_PARAM_LEN ))
    {
        *pChanged = true;
    }
    bCalledStatus = true;
    strncpy(stMsgData->paramValue,stIPInterfaceInstance.status,TR69HOSTIFMGR_MAX_PARAM_LEN );
    strncpy(backupStatus,stIPInterfaceInstance.status,TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stIPInterfaceInstance.status);

    return OK;
}

/**
 * @brief This function gets the instance handle for an IP Interface. It provides a non-volatile handle
 * used to reference this IP interface instance. Alias provides a mechanism for an ACS to label this
 * instance for future reference. Currently not implemented.
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
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::get_Interface_Alias(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    return NOK;
}

/**
 * @brief This function gets the IP Interface Name.It provides the textual name of the interface as
 * assigned by the CPE(Customer Premises Equipment).
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::get_Interface_Name(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    strcpy (stIPInterfaceInstance.name, nameOfInterface);

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s(): Interface name = %s \n", __FUNCTION__, stIPInterfaceInstance.name);

    if(bCalledName && pChanged && strncmp(stIPInterfaceInstance.name, backupName,TR69HOSTIFMGR_MAX_PARAM_LEN ))
    {
        *pChanged = true;
    }
    bCalledName = true;
    strncpy(stMsgData->paramValue,stIPInterfaceInstance.name,TR69HOSTIFMGR_MAX_PARAM_LEN );
    strncpy(backupName,stIPInterfaceInstance.name,TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stIPInterfaceInstance.name);


    return OK;
}

/**
 * @brief This function gets the last change of status time of the interface. It provides the accumulated
 * time in seconds since the interface entered its current operational state. Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::get_Interface_LastChange(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Parameter Not Supported \n",__FUNCTION__);

    return NOK;
}


/**
 * @brief This function gets the IP Interface LowerLayers. It provides a comma-separated
 * list (maximum length 1024) of strings. Each list item MUST be the path name of an
 * interface object that is stacked immediately below this interface object.
 * Currently not implemented.
 *
 * @note  - If the referenced object is deleted, the corresponding item MUST be removed
 *          from the list.
 *        - LowerLayers MUST be an empty string and 'read-only' when Type is Loop back,
 *          Tunnel, or Tunnelled.
 *
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::get_Interface_LowerLayers(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Parameter Not Supported \n",__FUNCTION__);

    return NOK;
}

/**
 * @brief  This function gets the Router instance that is associated with an IP Interface entry.
 * It provides the Router instance that is associated with this IP Interface entry.
 * Currently not implemented.
 *
 * @note     <ul>
 *               <li>The value MUST be the path name of a row in the
 *               <tt>Routing.Router</tt> table.</li>
 *               <li>If the referenced object is deleted, the parameter value MUST be
 *               set to an empty string.</li>
 *           </ul>
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::get_Interface_Router(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Parameter Not Supported \n",__FUNCTION__);

    return NOK;
}

/**
 * @brief This function gets the IP Interface Reset status. It returns 'false', regardless
 * of the actual value.
 *
 * @note  - The value of this parameter is not part of the device configuration and is
 *           always false when read.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::get_Interface_Reset(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    /*According to specification,When read, this parameter returns false, regardless of the actual value.*/
    stIPInterfaceInstance.reset = FALSE;

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s(): Reset: %d\n", __FUNCTION__, stIPInterfaceInstance.reset);

    if(bCalledReset && pChanged && (backupReset != stIPInterfaceInstance.reset))
    {
        *pChanged = true;
    }
    bCalledReset = true;
    backupReset = stIPInterfaceInstance.reset;
    put_int(stMsgData->paramValue,stIPInterfaceInstance.reset);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = 1;

    return OK;
}

/**
 * @brief This function gets the IP Interface MaxMTUSize. It provides the maximum transmission unit (MTU),
 * i.e. the largest allowed size of an IP packet (including IP headers, but excluding lower layer headers such as
 * Ethernet, PPP, or PPPoE headers) that is allowed to be transmitted by or through this device.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::get_Interface_MaxMTUSize(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    stIPInterfaceInstance.maxMTUSize = getMTU (nameOfInterface);

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s(): MaxMTUSize: %u \n", __FUNCTION__, stIPInterfaceInstance.maxMTUSize);

    if(bCalledMaxMTUSize && pChanged && (backupMaxMTUSize != stIPInterfaceInstance.maxMTUSize))
    {
        *pChanged = true;
    }
    bCalledMaxMTUSize = true;
    backupMaxMTUSize = stIPInterfaceInstance.maxMTUSize;
    put_int(stMsgData->paramValue,stIPInterfaceInstance.maxMTUSize);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 * @brief This function gets the IP Interface Type. It provides the IP interface type.
 * Possible values are  Normal, Loopback, Tunnel, Tunnelled.
 * @note     For 'Loopback', 'Tunnel', and 'Tunnelled' IP interface
 *           objects, the LowerLayers parameter MUST be an empty string.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::get_Interface_Type(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    if (isLoopback (nameOfInterface))
        strcpy (stIPInterfaceInstance.type, "Loopback");
    else if (0 == strncmp (nameOfInterface, "eth", 3) || 0 == strncmp (nameOfInterface, "wlan", 4))
        strcpy (stIPInterfaceInstance.type, "Normal");
    else
        strcpy (stIPInterfaceInstance.type, "Tunneled");

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s(): Type: %s \n", __FUNCTION__, stIPInterfaceInstance.type);

    if(bCalledType && pChanged && strncmp(stIPInterfaceInstance.type, backupType,TR69HOSTIFMGR_MAX_PARAM_LEN ))
    {
        *pChanged = true;
    }
    bCalledType = true;
    strncpy(stMsgData->paramValue,stIPInterfaceInstance.type,TR69HOSTIFMGR_MAX_PARAM_LEN );
    strncpy(backupType,stIPInterfaceInstance.type,TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stIPInterfaceInstance.type);

    return OK;
}

void hostIf_IPInterface::getActiveFlags (char* nameOfInterface, struct ifreq& ifr)
{
    memset (&ifr, 0, sizeof(ifr));
    strcpy (ifr.ifr_name, nameOfInterface);
    int fd = socket (PF_INET, SOCK_DGRAM, IPPROTO_IP);
    ioctl (fd, SIOCGIFFLAGS, &ifr);
    close (fd);
}

bool hostIf_IPInterface::isLoopback (char* nameOfInterface)
{
    LOG_ENTRY_EXIT;

    struct ifreq ifr;
    getActiveFlags (nameOfInterface, ifr);
    return ifr.ifr_flags & IFF_LOOPBACK;
}

bool hostIf_IPInterface::isEnabled (char* nameOfInterface)
{
    LOG_ENTRY_EXIT;

    struct ifreq ifr;
    getActiveFlags (nameOfInterface, ifr);
    return ifr.ifr_flags & IFF_UP;
}

int hostIf_IPInterface::getMTU (char* nameOfInterface)
{
    LOG_ENTRY_EXIT;

    struct ifreq ifr;
    memset (&ifr, 0, sizeof(ifr));
    strcpy (ifr.ifr_name, nameOfInterface);
    int fd = socket (PF_INET, SOCK_DGRAM, IPPROTO_IP);
    ioctl (fd, SIOCGIFMTU, &ifr);
    close (fd);
    return ifr.ifr_mtu;
}

/**
 * @brief This function gets the IP interface loopback. It indicates whether or not the
 * IP interface is a loopback interface by 'true' or 'false'.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::get_Interface_Loopback(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    stIPInterfaceInstance.loopback = isLoopback (nameOfInterface);

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s(): Loopback: %d \n", __FUNCTION__, stIPInterfaceInstance.loopback);

    if(bCalledLoopback && pChanged && (backupLoopback != stIPInterfaceInstance.loopback))
    {
        *pChanged = true;
    }
    bCalledLoopback = true;
    backupLoopback = stIPInterfaceInstance.loopback;
    put_int(stMsgData->paramValue,stIPInterfaceInstance.loopback);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = 1;

    return OK;
}

unsigned int hostIf_IPInterface::getIPAddressNumberOfEntries (sa_family_t address_family)
{
    LOG_ENTRY_EXIT;

    struct ifaddrs *ifa;
    if (getifaddrs (&ifa))
        return 0;

    int ipAddressNumberOfEntries = 0;
    for (struct ifaddrs *ifa_node = ifa; ifa_node; ifa_node = ifa_node->ifa_next)
        if (ifa_node->ifa_addr->sa_family == address_family && !strcmp (ifa_node->ifa_name, nameOfInterface))
                ipAddressNumberOfEntries++;

    freeifaddrs (ifa);

    return ipAddressNumberOfEntries;
}

/** Description: Counts the number of IPv4 addresses per
 *               IP interface present in the device.
 *
 * \Return:  Count value or '0' if error
 *
 */
unsigned int hostIf_IPInterface::getIPv4AddressNumberOfEntries ()
{
    return getIPAddressNumberOfEntries (AF_INET);
}

/**
 * @brief This function gets the IP Interface IPv4AddressNumberOfEntries. It provides the number
 * of entries in the IPv4 Address table.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::get_Interface_IPv4AddressNumberOfEntries(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    stIPInterfaceInstance.iPv4AddressNumberOfEntries = getIPv4AddressNumberOfEntries ();

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s(): Interface %d (%s) has [%u] IPv4 Addresses\n", __FUNCTION__, dev_id, nameOfInterface,
            stIPInterfaceInstance.iPv4AddressNumberOfEntries);

    put_int(stMsgData->paramValue,stIPInterfaceInstance.iPv4AddressNumberOfEntries);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

/**
 *  Description: Counts the number of IPv6 addresses per
 *               IP interface present in the device.
 *
 * \Return:  Count value or '0' if error
 *
 */
unsigned int hostIf_IPInterface::getIPv6AddressNumberOfEntries ()
{
    return getIPAddressNumberOfEntries (AF_INET6);
}

unsigned int hostIf_IPInterface::getIPv6PrefixNumberOfEntries ()
{
    return getIPAddressNumberOfEntries (AF_INET6);
}

#ifdef IPV6_SUPPORT
/**
 *
 */
int hostIf_IPInterface::get_Interface_IPv6AddressNumberOfEntries(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    stIPInterfaceInstance.iPv6AddressNumberOfEntries = getIPv6AddressNumberOfEntries ();

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s(): Interface %d (%s) has [%u] IPv6 Addresses\n", __FUNCTION__, dev_id, nameOfInterface,
            stIPInterfaceInstance.iPv6AddressNumberOfEntries);

    put_int(stMsgData->paramValue,stIPInterfaceInstance.iPv6AddressNumberOfEntries);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);

    return OK;
}

int hostIf_IPInterface::get_Interface_IPv6PrefixNumberOfEntries(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    stIPInterfaceInstance.iPv6PrefixNumberOfEntries = getIPv6PrefixNumberOfEntries ();

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s(): Interface %d (%s) has [%u] IPv6 Prefixes\n", __FUNCTION__, dev_id, nameOfInterface,
            stIPInterfaceInstance.iPv6PrefixNumberOfEntries);

    put_int(stMsgData->paramValue,stIPInterfaceInstance.iPv6PrefixNumberOfEntries);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);

    return OK;
}

#endif // IPV6_SUPPORT

/**
 * @brief This function gets the status of Auto-IP on an IP Interface is 'enabled' or
 * 'disabled'. It indicates whether or not auto-IP is enabled for this IP interface.
 * Currently not implemented.
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
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::get_Interface_AutoIPEnable(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Parameter Not Supported \n",__FUNCTION__);

/*
    stIPInterfaceInstance.autoIPEnable = (strcmp (nameOfInterface, "eth1") == 0);

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s(): AutoIPEnable: %d\n", __FUNCTION__, stIPInterfaceInstance.autoIPEnable);

    if(bCalledAutoIPEnable && pChanged && (backupAutoIPEnable != stIPInterfaceInstance.autoIPEnable))
    {
        *pChanged = true;
    }
    bCalledAutoIPEnable = true;
    backupAutoIPEnable = stIPInterfaceInstance.autoIPEnable;
    put_int(stMsgData->paramValue,stIPInterfaceInstance.autoIPEnable);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = 1;
*/

    return OK;
}
/****************************************************************************************************************************************************/
// Device.IP.Interface. Profile. Setters:
/****************************************************************************************************************************************************/

/**
 * @brief This function sets the status of IP Interface 'enabled' or 'disabled'. It
 * enables i.e 'true' or disables i.e 'false' this IP interface regardless of
 * IPv4Enable and IPv6Enable. Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::set_Interface_Enable(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    set_Interface_Enable (get_boolean(stMsgData->paramValue));

    return OK;
}

/**
 * @brief This function attaches or detaches this IP interface to/from the IPv4 stack.
 * If set to 'true', then this interface is attached to the IPv4 stack. If set to 'false',
 * then this interface is detached from the IPv4 stack. Currently not implemented.
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
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::set_Interface_IPv4Enable(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    set_Interface_Enable (get_boolean (stMsgData->paramValue));

    return OK;
}

/**
 * @brief This function sets the status 'enabled' or 'disabled' of ULA(Unique Local Address) generation
 * for an IP Interface. It controls whether or not ULAs are generated and used on this interface.
 * Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::set_Interface_ULAEnable(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Parameter Not Supported \n",__FUNCTION__);

    return NOK;
}

/**
 * @brief This function sets a non-volatile handle used to reference this IP interface instance.
 * Alias provides a mechanism for an ACS to label this instance for future reference.
 * Currently not implemented.
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
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::set_Interface_Alias(HOSTIF_MsgData_t *stMsgData)
{
    return NOK;
}

/**
 * @brief  This function sets the IP Interface LowerLayers. Given a comma-separated
 * list (maximum length 1024) of strings, each list item being the path name of an
 * interface object, this function MUST stack each item in the list immediately below
 * this interface object. Currently not implemented.
 *
 * @note  - If the referenced object is deleted, the corresponding item MUST be removed
 *           from the list.
 *        - LowerLayers MUST be an empty string and 'read-only' when Type is
 *           'Loopback', 'Tunnel', or 'Tunneled'.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::set_Interface_LowerLayers(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Parameter Not Supported \n",__FUNCTION__);

    return NOK;
}

/**
 * @brief This function sets the router instance that is associated with an IP Interface entry.
 * Currently not implemented.
 *
 * @note     <ul>
 *               <li>The value MUST be the path name of a row in the
 *               <tt>Routing.Router</tt> table.</li>
 *               <li>If the referenced object is deleted, the parameter value MUST be
 *               set to an empty string.</li>
 *           </ul>
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::set_Interface_Router(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Parameter Not Supported \n",__FUNCTION__);

    return NOK;
}

/**
 * @brief This function sets the Reset flag to the requested value (normally 'true').
 * Currently not implemented.
 *
 * @note     <ul>
 *               <li>When set to <tt>true</tt>, the device MUST tear down the
 *               existing IP connection represented by this object and establish a
 *               new one.</li>
 *
 *               <li>The device MUST initiate the reset after completion of the
 *               current CWMP session.</li>
 *
 *               <li>The device MAY delay resetting the connection in order to avoid
 *               interruption of a user service such as an ongoing voice call.</li>
 *
 *               <li>Reset on a disabled interface is a no-op (not an error).</li>
 *           </ul>
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::set_Interface_Reset(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    set_Interface_Reset (get_boolean (stMsgData->paramValue));

    return OK;
}

/**
 * @brief This function sets the size of maximum transmission unit (MTU), i.e. the largest
 * allowed size of an IP packet (including IP headers, but excluding lower layer headers
 * such as Ethernet, PPP, or PPPoE headers) that is allowed to be transmitted by or through this device.
 * Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::set_Interface_MaxMTUSize(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    set_Interface_Mtu (get_int (stMsgData->paramValue));

    return OK;
}

/**
 * @brief This function sets the IP interface LoopBack flag to 'true' or 'false'.
 * Currently not implemented.
 *
 * @note     When set to <tt>true</tt>, the IP interface becomes a loopback interface and
 *           the CPE MUST set Type to Loopback. In this case, the CPE MUST also set
 *           the LowerLayers property to an empty string and fail subsequent attempts at
 *           setting LowerLayers until the interface is no longer a loopback.
 *
 * @note     Support for manipulating loopback interfaces is OPTIONAL.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::set_Interface_Loopback(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Parameter Not Supported \n",__FUNCTION__);

    return NOK;
}

/**
 * @brief This function sets the Auto-IP on an IP Interface status 'enable' or 'disable'.
 * It sets 'true' for enable  and 'false' for disables the auto-IP for this IP interface.
 * Currently not implemented.
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
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_INTERFACE_IP_API
 */
int hostIf_IPInterface::set_Interface_AutoIPEnable(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Parameter Not Supported \n",__FUNCTION__);

    return NOK;
}
/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */


/** @} */
/** @} */
