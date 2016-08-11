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
 * @file Device_IP_Interface_IPv4Address.cpp
 * @brief This source file contains the APIs of device IPv4 interface address.
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


#include <sys/sysinfo.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#include "Device_IP_Interface_IPv4Address.h"
#include "Device_IP_Interface_Stats.h"
#include "Device_IP_Interface.h"
#include "hostIf_utils.h"
#include "Device_IP.h"

#ifdef YOCTO_BUILD
extern "C" {
#include "secure_wrapper.h"
}
#endif
GMutex* hostIf_IPv4Address::m_mutex = NULL;
GHashTable *hostIf_IPv4Address::ifHash = NULL;

int hostIf_IPv4Address::getIPv4AddressAndMask (int instance, struct in_addr& in_address, struct in_addr& in_mask)
{
    LOG_ENTRY_EXIT;

    int rc = NOK;
    struct ifaddrs *ifa;
    if (getifaddrs (&ifa))
        return rc;

    int current_instance = 0;
    for (struct ifaddrs *ifa_node = ifa; ifa_node; ifa_node = ifa_node->ifa_next)
    {
        if (ifa_node->ifa_addr->sa_family == AF_INET && !strcmp (ifa_node->ifa_name, nameOfInterface) && (++current_instance == instance))
        {
            in_address = ((struct sockaddr_in *) ifa_node->ifa_addr)->sin_addr;
            in_mask = ((struct sockaddr_in *) ifa_node->ifa_netmask)->sin_addr;
            rc = OK;
            break;
        }
    }

    freeifaddrs (ifa);

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s]: rc=%d, if=%s, instance=%d\n", __FUNCTION__, rc, nameOfInterface, instance);

    return rc;
}

//Function to set the IP OR SubNetMask Address for the Interface
int hostIf_IPv4Address::setIpOrMask(int interfaceNo, char *value, const char* ipOrMask)
{
    LOG_ENTRY_EXIT;

    if (0 != strcmp (value, ""))
    {
        if (0 == strcasecmp (backupAddressingType, "static"))
        {
            char cmd[BUFF_LENGTH] = { 0 };
            sprintf (cmd, "ifconfig %s %s %s", nameOfInterface, ipOrMask, value);
#ifdef YOCTO_BUILD
       v_secure_system(cmd);
#else
       system(cmd);
#endif
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s(),%d] IPv4Address: IPAddress/SubnetMask is set \n", __FUNCTION__, __LINE__);
        }
    }
    return OK;
}

void hostIf_IPv4Address::refreshInterfaceName ()
{
    nameOfInterface[0] = 0;
    if (NULL == hostIf_IP::getInterfaceName (dev_id, nameOfInterface))
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: error getting interface name for Device.IP.Interface.%d\n", __FUNCTION__, dev_id);
}

/**
 * @brief Class Constructor of the class hostIf_IPv4Address.
 *
 * It will initialize the device id. Initialize the status, IP Address, SubnetMask and AddressingType to
 * empty string.
 *
 * @param[in] devid Identification number of the device to communicate.
 */
hostIf_IPv4Address::hostIf_IPv4Address(int dev_id):
    dev_id(dev_id),
    bCalledEnable(0),
    bCalledStatus(0),
    bCalledIPAddress(0),
    bCalledSubnetMask(0),
    bCalledAddressingType(0),
    backupEnable (false)
{
    strcpy (backupStatus, "");
    strcpy (backupIPAddress, "");
    strcpy (backupSubnetMask, "");
    strcpy (backupAddressingType, "Static");
}

hostIf_IPv4Address* hostIf_IPv4Address::getInstance(int dev_id)
{
    LOG_ENTRY_EXIT;

    hostIf_IPv4Address* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_IPv4Address *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_IPv4Address(dev_id);
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

GList* hostIf_IPv4Address::getAllInstances()
{
    LOG_ENTRY_EXIT;

    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_IPv4Address::closeInstance(hostIf_IPv4Address *pDev)
{
    LOG_ENTRY_EXIT;

    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_IPv4Address::closeAllInstances()
{
    LOG_ENTRY_EXIT;

    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_IPv4Address* pDev = (hostIf_IPv4Address *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

void hostIf_IPv4Address::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void hostIf_IPv4Address::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

int hostIf_IPv4Address::handleGetMsg (const char* pSubSetting, int subInstanceNumber, HOSTIF_MsgData_t* stMsgData)
{
    LOG_ENTRY_EXIT;

    int ret = NOT_HANDLED;

    if (!strcasecmp (pSubSetting, "Enable"))
    {
        ret = get_IPv4Address_Enable (stMsgData, subInstanceNumber);
    }
    else if (!strcasecmp (pSubSetting, "Status"))
    {
        ret = get_IPv4Address_Status (stMsgData, subInstanceNumber);
    }
    else if (!strcasecmp (pSubSetting, "Alias"))
    {
        ret = get_IPv4Address_Alias (stMsgData, subInstanceNumber);
    }
    else if (!strcasecmp (pSubSetting, "SubnetMask"))
    {
        ret = get_IPv4Address_SubnetMask (stMsgData, subInstanceNumber);
    }
    else if (!strcasecmp (pSubSetting, "AddressingType"))
    {
        ret = get_IPv4Address_AddressingType (stMsgData, subInstanceNumber);
    }
    else if (!strcasecmp (pSubSetting, "IPAddress"))
    {
        ret = get_IPv4Address_IPAddress (stMsgData, subInstanceNumber);
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Device.IPv4: Parameter \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
        stMsgData->faultCode = fcInvalidParameterName;
        ret = NOK;
    }
    return ret;
}

int hostIf_IPv4Address::handleSetMsg (const char* pSubSetting, int subInstanceNumber, HOSTIF_MsgData_t* stMsgData)
{
    LOG_ENTRY_EXIT;

    int ret = NOT_HANDLED;

    if (!strcasecmp (pSubSetting, "Enable"))
    {
        ret = set_IPv4Address_Enable (stMsgData, subInstanceNumber);
    }
    else if (!strcasecmp (pSubSetting, "SubnetMask"))
    {
        ret = set_IPv4Address_SubnetMask (stMsgData, subInstanceNumber);
    }
    else if (!strcasecmp (pSubSetting, "IPAddress"))
    {
        ret = set_IPv4Address_IPAddress (stMsgData, subInstanceNumber);
    }
    else
    {
        stMsgData->faultCode = fcInvalidParameterName;
        ret = NOT_HANDLED;
    }

    return ret;
}

/****************************************************************************************************************************************************/
// Device_IP_Interface_IPv4Address Profile. Getters:
/****************************************************************************************************************************************************/


/**
 * @brief This function gets the status of the IPv4 interface 'enabled' or 'disabled'.
 * It provides 'true' for enable and 'false' for disable IPv4 address.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] subInstanceNo SubInstance number currently not in use.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_IPv4_INTERFACE_ADDRESS_API
 */
int hostIf_IPv4Address::get_IPv4Address_Enable(HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    struct in_addr in_address;
    struct in_addr in_mask;
    bool enable = (OK == getIPv4AddressAndMask (subInstanceNo, in_address, in_mask));

    if (bCalledEnable && pChanged && (backupEnable != enable))
    {
        *pChanged = true;
    }
    bCalledEnable = true;
    backupEnable = enable;
    put_int (stMsgData->paramValue, enable);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = 1;

    return OK;
}

/**
 * @brief This function gets the status of an IPv4 Address table entry. It provides the status of
 * this IPv4Address table entry. Possible enum values are Disabled, Enabled, Error_Misconfigured
 * and Error (OPTIONAL).
 *
 * @note  - The 'Error_Misconfigured' value indicates that a necessary
 *          configuration value is undefined or invalid.
 *        - The 'Error' value MAY be used by the CPE to indicate a
 *              locally defined error condition.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] subInstanceNo SubInstance number currently not in use.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_IPv4_INTERFACE_ADDRESS_API
 */
int hostIf_IPv4Address::get_IPv4Address_Status(HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    char status[BUFF_LENGTH_16];
    struct in_addr in_address;
    struct in_addr in_mask;
    strcpy (status, (OK == getIPv4AddressAndMask (subInstanceNo, in_address, in_mask)) ? "Enabled" : "Disabled");

    if (bCalledStatus && pChanged && strncmp (status, backupStatus, BUFF_LENGTH_16))
    {
        *pChanged = true;
    }
    bCalledStatus = true;
    strncpy (backupStatus, status, BUFF_LENGTH_16);
    strncpy (stMsgData->paramValue, status, BUFF_LENGTH_16);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (status);

    return OK;
}

/**
 * @brief This function gets the instance handle for an IPv4 Address of an IP Interface.
 * It provides a non-volatile handle used to reference this IPv4 address
 * instance of this IP interface. Alias provides a mechanism for an ACS to label this
 * instance for future reference. Currently not implemented.
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
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] subInstanceNo SubInstance number currently not in use.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_IPv4_INTERFACE_ADDRESS_API
 */
int hostIf_IPv4Address::get_IPv4Address_Alias(HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;
    // TODO: example Device.IP.Interface.1.IPv4Address.1.Alias = cpe-IPv4Address_1
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}

/**
 * @brief This function used to get an IP Interface IPv4 IP Address.
 *
 * @note This parameter can only be modified if 'AddressingType' is static.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] subInstanceNo SubInstance number currently not in use.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_IPv4_INTERFACE_ADDRESS_API
 */
int hostIf_IPv4Address::get_IPv4Address_IPAddress(HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    struct in_addr in_address;
    struct in_addr in_mask;
    if (OK != getIPv4AddressAndMask (subInstanceNo, in_address, in_mask))
        return NOK;

    char ipv4Address[BUFF_LENGTH_64] = {0};
    inet_ntop (AF_INET, &in_address, ipv4Address, BUFF_LENGTH_64);
    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s]: ipv4Address = %s\n", __FUNCTION__, ipv4Address);

    if (bCalledIPAddress && pChanged && strncmp (ipv4Address, backupIPAddress, BUFF_LENGTH_64))
    {
        *pChanged = true;
    }
    bCalledIPAddress = true;
    strncpy (backupIPAddress, ipv4Address, BUFF_LENGTH_64);
    strncpy (stMsgData->paramValue, ipv4Address, BUFF_LENGTH_64);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (ipv4Address);

    return OK;
}

/**
 * @brief This function gets an IP Interface IPv4 Address SubnetMask.
 *
 * @note This parameter can only be modified if 'AddressingType' is static.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] subInstanceNo SubInstance number currently not in use.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_IPv4_INTERFACE_ADDRESS_API
 */
int hostIf_IPv4Address::get_IPv4Address_SubnetMask(HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    struct in_addr in_address;
    struct in_addr in_mask;
    if (OK != getIPv4AddressAndMask (subInstanceNo, in_address, in_mask))
        return NOK;

    char subnetMask[BUFF_LENGTH_64] = {0};
    inet_ntop (AF_INET, &in_mask, subnetMask, BUFF_LENGTH_64);
    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s]: subnetMask = %s\n", __FUNCTION__, subnetMask);

    if (bCalledSubnetMask && pChanged && strncmp (subnetMask, backupSubnetMask, BUFF_LENGTH_64))
    {
        *pChanged = true;
    }
    bCalledSubnetMask = true;
    strncpy (backupSubnetMask, subnetMask, BUFF_LENGTH_64);
    strncpy (stMsgData->paramValue, subnetMask, BUFF_LENGTH_64);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (subnetMask);

    return OK;
}

/**
 * @brief tests if an IPv4 address is link-local.
 *
 * @param[in] in_address the IPv4 address to test.
 *
 * @retval true if the IPv4 address is link-local.
 * @retval false otherwise.
 */
bool hostIf_IPv4Address::isLinkLocalAddress (const struct in_addr& in_address)
{
    LOG_ENTRY_EXIT;

    // check if the given IPv4 address falls in the IPv4 link-local address range (169.254.x.y)

    char ipv4Address[BUFF_LENGTH_64];
    inet_ntop (AF_INET, &in_address, ipv4Address, BUFF_LENGTH_64);

    bool isAutoIp = (strncmp (ipv4Address, "169.254.", 8) == 0);

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s]: ipv4Address = %s, isAutoIp = %d\n", __FUNCTION__, ipv4Address, isAutoIp);

    return isAutoIp;
}

/** 
 * @brief This function gets an IP Interface IPv4 Address AddressingType assign the IP address.
 * Possible enum values are DHCP, AutoIP, IPCP and Static.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] subInstanceNo SubInstance number currently not in use.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_IPv4_INTERFACE_ADDRESS_API
 */
int hostIf_IPv4Address::get_IPv4Address_AddressingType(HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

/*
    From the spec:

    Addressing method used to assign the IP address. Enumeration of:

        DHCP

        IKEv2 (Assigned by IKEv2 [RFC5996])

        AutoIP

        IPCP

        Static
*/

    // first verify that we have an IPv4 address for the given instance
    struct in_addr in_address;
    struct in_addr in_mask;
    if (OK != getIPv4AddressAndMask (subInstanceNo, in_address, in_mask))
        return NOK;

    char ipv4Address[BUFF_LENGTH_64];
    inet_ntop (AF_INET, &in_address, ipv4Address, BUFF_LENGTH_64);

    char addressingType[BUFF_LENGTH_16];
    if (hostIf_IPInterface::isLoopback (nameOfInterface))
    {
        strcpy (addressingType, "Static");
    }
    else if (isLinkLocalAddress (in_address))
    {
        strcpy (addressingType, "AutoIP");
    }
    /*
     * If a MoCA / WiFi interface (specified by MOCA_INTERFACE / WIFI_INTERFACE in /etc/device.properties)
     * still has its default IPv4 address (specified by DEFAULT_MOCA_IFACE_IP / DEFAULT_WIFI_IFACE_IP),
     * then IPv4 AddressingType for that interface is reported as Static. Otherwise, it is reported as DHCP.
     */
    else if (((hasPhysicalInterfaceAs (getenvOrDefault ("MOCA_INTERFACE", ""))) &&
              (0 == strcmp (ipv4Address, getenvOrDefault ("DEFAULT_MOCA_IFACE_IP", "")))) ||
             ((hasPhysicalInterfaceAs (getenvOrDefault ("WIFI_INTERFACE", ""))) &&
              (0 == strcmp (ipv4Address, getenvOrDefault ("DEFAULT_WIFI_IFACE_IP", "")))))
    {
        strcpy (addressingType, "Static");
    }
    else
    {
        strcpy (addressingType, "DHCP"); // DHCP - otherwise (assume)
    }

    if (bCalledAddressingType && pChanged && strncmp (addressingType, backupAddressingType, BUFF_LENGTH_16))
    {
        *pChanged = true;
    }
    bCalledAddressingType = true;
    strncpy (backupAddressingType, addressingType, BUFF_LENGTH_16);
    strncpy (stMsgData->paramValue, addressingType, BUFF_LENGTH_16);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (addressingType);

    return OK;
}

bool hostIf_IPv4Address::hasPhysicalInterfaceAs (char* phy_if_name)
{
    int l = strlen (phy_if_name);
    return strncmp (nameOfInterface, phy_if_name, l) == 0 && (nameOfInterface[l] == 0 || nameOfInterface[l] == ':');
}

/****************************************************************************************************************************************************/
// Device_IP_Interface_IPv4Address Profile. Setters:
/****************************************************************************************************************************************************/

/**
 * @brief This function sets the device IP Interface IPv4 Address to 'Enable' or 'Disable'.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] subInstanceNo  SubInstance number currently not in use.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_IPv4_INTERFACE_ADDRESS_API
 */
int hostIf_IPv4Address::set_IPv4Address_Enable(HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;

    char cmd[BUFF_LENGTH] = { 0 };
    if(FALSE == get_boolean(stMsgData->paramValue))
    {
        sprintf(cmd,"ifconfig %s 0.0.0.0",nameOfInterface);
#ifdef YOCTO_BUILD
       v_secure_system(cmd);
#else
       system(cmd);
#endif
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s(),%d] IPv4Address Disabled   \n",__FUNCTION__,__LINE__);

    }
    if(TRUE == get_boolean(stMsgData->paramValue))
    {
        sprintf(cmd,"ifdown %s && ifup %s",nameOfInterface,nameOfInterface);
#ifdef YOCTO_BUILD
       v_secure_system(cmd);
#else
       system(cmd);
#endif
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s(),%d] IPv4Address Enabled \n",__FUNCTION__,__LINE__);
    }

    return OK;
}

/**
 * @brief This function sets a non-volatile handle used to reference this IPv4 address
 * instance of this IP interface. Alias provides a mechanism for an ACS to label this
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
 * @param[in] subInstanceNo  SubInstance number currently not in use.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_IPv4_INTERFACE_ADDRESS_API
 */
int hostIf_IPv4Address::set_IPv4Address_Alias(HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}

/**
 * @brief This function sets an IP Interface IPv4 Address.
 *
 * @note  This parameter can only be modified if the 'AddressingType' is Static.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] subInstanceNo  SubInstance number currently not in use.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_IPv4_INTERFACE_ADDRESS_API
 */
int hostIf_IPv4Address::set_IPv4Address_IPAddress(HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;

    setIpOrMask(dev_id, stMsgData->paramValue ,"inet");

    return OK;
}

/**
 * @brief This function sets an IP Interface IPv4Address Subnet Mask of an IPv4 address.
 *
 * @note This parameter can only be modified if the 'AddressingType' is static.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] subInstanceNo  SubInstance number currently not in use.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IP_IPv4_INTERFACE_ADDRESS_API
 */
int hostIf_IPv4Address::set_IPv4Address_SubnetMask(HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;

    setIpOrMask(dev_id, stMsgData->paramValue ,"netmask");

    return OK;
}
/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */


/** @} */
/** @} */
