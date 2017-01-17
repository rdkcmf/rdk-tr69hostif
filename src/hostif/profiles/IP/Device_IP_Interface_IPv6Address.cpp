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
 * @file Device_IP_Interface_IPv6Address.cpp
 *
 * @brief Device_IP_Interface_IPv6Address API Implementation.
 *
 * This is the implementation of the Device_IP_Interface_IPv6Address API.
 *
 * @par Document
 * TBD Relevant design or API documentation.
 *
 */

/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/

#include <sys/sysinfo.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#include "Device_IP_Interface_IPv6Address.h"
#include "Device_IP_Interface_Stats.h"
#include "Device_IP_Interface.h"
#include "hostIf_utils.h"
#include "Device_IP.h"

/**
 * @struct in6_ifreq
 * @brief The structure holds the required parameters such as IPv6 address,interface index etc..
 * for the IPv6 interface address.
 */
struct in6_ifreq {
    struct in6_addr ifr6_addr;
    long ifr6_prefixlen;
    unsigned int ifr6_ifindex;
};

GMutex* hostIf_IPv6Address::m_mutex = NULL;
GHashTable *hostIf_IPv6Address::ifHash = NULL;

const char* hostIf_IPv6Address::PREFERRED = "Preferred";
const char* hostIf_IPv6Address::DEPRECATED = "Deprecated";
const char* hostIf_IPv6Address::INVALID = "Invalid";
const char* hostIf_IPv6Address::INACCESSIBLE = "Inaccessible";
const char* hostIf_IPv6Address::UNKNOWN = "Unknown";
const char* hostIf_IPv6Address::TENTATIVE = "Tentative";
const char* hostIf_IPv6Address::DUPLICATE = "Duplicate";
const char* hostIf_IPv6Address::OPTIMISTIC = "Optimistic";

const char* hostIf_IPv6Address::AUTOCONFIGURED = "AutoConfigured";
const char* hostIf_IPv6Address::DHCPv6 = "DHCPv6";
const char* hostIf_IPv6Address::WELLKNOWN = "WellKnown";
const char* hostIf_IPv6Address::STATIC = "Static";
const char* hostIf_IPv6Address::PREFIXDELEGATION = "PrefixDelegation";
const char* hostIf_IPv6Address::ROUTERADVERTISEMENT = "RouterAdvertisement";
const char* hostIf_IPv6Address::CHILD = "Child";
const char* hostIf_IPv6Address::INAPPLICABLE = "Inapplicable";

hostIf_IPv6Address::hostIf_IPv6Address(int dev_id):
    dev_id(dev_id),
    bCalledEnable(0),
    bCalledStatus(0),
    bCalledIPAddress(0),
    bCalledOrigin(0),
    backupIPv6AddressEnable (false),
    backupIPv6PrefixEnable (false)
{
    strcpy (backupIPv6AddressStatus, "Disabled");
    strcpy (backupIPv6AddressIPAddress, "");
    strcpy (backupIPv6AddressOrigin, STATIC);

    strcpy (backupIPv6PrefixStatus, "Disabled");
    strcpy (backupIPv6PrefixPrefix, "");
    strcpy (backupIPv6PrefixOrigin, STATIC);
}

hostIf_IPv6Address::~hostIf_IPv6Address()
{
}

void hostIf_IPv6Address::refreshInterfaceName ()
{
    nameOfInterface[0] = 0;
    if (NULL == hostIf_IP::getInterfaceName (dev_id, nameOfInterface))
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: error getting interface name for Device.IP.Interface.%d\n", __FUNCTION__, dev_id);
}

hostIf_IPv6Address* hostIf_IPv6Address::getInstance(int dev_id)
{
    LOG_ENTRY_EXIT;

    hostIf_IPv6Address* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_IPv6Address *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_IPv6Address(dev_id);
            g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create MoCA Interface instance..\n");
        }
    }

    // make sure returned instance has interface name set
    if (pRet)
        pRet->refreshInterfaceName ();

    return pRet;
}

GList* hostIf_IPv6Address::getAllInstances()
{
    LOG_ENTRY_EXIT;

    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_IPv6Address::closeInstance(hostIf_IPv6Address *pDev)
{
    LOG_ENTRY_EXIT;

    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_IPv6Address::closeAllInstances()
{
    LOG_ENTRY_EXIT;

    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_IPv6Address* pDev = (hostIf_IPv6Address *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

int hostIf_IPv6Address::handleGetMsg (const char* pSubSetting, int subInstanceNumber, HOSTIF_MsgData_t* stMsgData, bool isAddress)
{
    LOG_ENTRY_EXIT;

    int ret = NOT_HANDLED;

    if (isAddress)
    {
        if (!strcasecmp (pSubSetting, "Enable"))
        {
            ret = get_IPv6Address_Enable (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "Status"))
        {
            ret = get_IPv6Address_Status (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "IPAddressStatus"))
        {
            ret = get_IPv6Address_IPAddressStatus (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "Alias"))
        {
            ret = get_IPv6Address_Alias (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "IPAddress"))
        {
            ret = get_IPv6Address_IPAddress (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "Origin"))
        {
            ret = get_IPv6Address_Origin (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "Prefix"))
        {
            ret = get_IPv6Address_Prefix (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "PreferredLifetime"))
        {
            ret = get_IPv6Address_PreferredLifetime (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "ValidLifetime"))
        {
            ret = get_IPv6Address_ValidLifetime (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "Anycast"))
        {
            ret = get_IPv6Address_Anycast (stMsgData, subInstanceNumber);
        }
    }
    else
    {
        if (!strcasecmp (pSubSetting, "Enable"))
        {
            ret = get_IPv6Prefix_Enable (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "Status"))
        {
            ret = get_IPv6Prefix_Status (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "PrefixStatus"))
        {
            ret = get_IPv6Prefix_PrefixStatus (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "Alias"))
        {
            ret = get_IPv6Prefix_Alias (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "Prefix"))
        {
            ret = get_IPv6Prefix_Prefix (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "Origin"))
        {
            ret = get_IPv6Prefix_Origin (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "StaticType"))
        {
            ret = get_IPv6Prefix_StaticType (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "ParentPrefix"))
        {
            ret = get_IPv6Prefix_ParentPrefix (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "ChildPrefixBits"))
        {
            ret = get_IPv6Prefix_ChildPrefixBits (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "OnLink"))
        {
            ret = get_IPv6Prefix_OnLink (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "Autonomous"))
        {
            ret = get_IPv6Prefix_Autonomous (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "PreferredLifetime"))
        {
            ret = get_IPv6Prefix_PreferredLifetime (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "ValidLifetime"))
        {
            ret = get_IPv6Prefix_ValidLifetime (stMsgData, subInstanceNumber);
        }

    }

    return ret;
}

int hostIf_IPv6Address::handleSetMsg (const char* pSubSetting, int subInstanceNumber, HOSTIF_MsgData_t* stMsgData, bool isAddress)
{
    LOG_ENTRY_EXIT;

    int ret = NOT_HANDLED;

    if (isAddress)
    {
        if (!strcasecmp (pSubSetting, "Enable"))
        {
            ret = set_IPv6Address_Enable (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "Alias"))
        {
            ret = set_IPv6Address_Alias (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "IPAddress"))
        {
            ret = set_IPv6Address_IPAddress (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "Prefix"))
        {
            ret = set_IPv6Address_Prefix (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "PreferredLifetime"))
        {
            ret = set_IPv6Address_PreferredLifetime (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "ValidLifetime"))
        {
            ret = set_IPv6Address_ValidLifetime (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "Anycast"))
        {
            ret = set_IPv6Address_Anycast (stMsgData, subInstanceNumber);
        }
    }
    else
    {
        if (!strcasecmp (pSubSetting, "Enable"))
        {
            ret = set_IPv6Prefix_Enable (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "Alias"))
        {
            ret = set_IPv6Prefix_Alias (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "Prefix"))
        {
            ret = set_IPv6Prefix_Prefix (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "StaticType"))
        {
            ret = set_IPv6Prefix_StaticType (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "ParentPrefix"))
        {
            ret = set_IPv6Prefix_ParentPrefix (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "ChildPrefixBits"))
        {
            ret = set_IPv6Prefix_ChildPrefixBits (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "OnLink"))
        {
            ret = set_IPv6Prefix_OnLink (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "Autonomous"))
        {
            ret = set_IPv6Prefix_Autonomous (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "PreferredLifetime"))
        {
            ret = set_IPv6Prefix_PreferredLifetime (stMsgData, subInstanceNumber);
        }
        else if (!strcasecmp (pSubSetting, "ValidLifetime"))
        {
            ret = set_IPv6Prefix_ValidLifetime (stMsgData, subInstanceNumber);
        }
    }

    return ret;
}

int hostIf_IPv6Address::getIPv6AddressAndMask (int instance, struct in6_addr& in6_address, struct in6_addr& in6_mask)
{
    LOG_ENTRY_EXIT;

    int rc = NOK;
    struct ifaddrs *ifa;
    if (getifaddrs (&ifa))
        return rc;

    int current_instance = 0;
    for (struct ifaddrs *ifa_node = ifa; ifa_node; ifa_node = ifa_node->ifa_next)
    {
        if (ifa_node->ifa_addr->sa_family == AF_INET6 && !strcmp (ifa_node->ifa_name, nameOfInterface) && (++current_instance == instance))
        {
            in6_address = ((struct sockaddr_in6 *) ifa_node->ifa_addr)->sin6_addr;
            in6_mask = ((struct sockaddr_in6 *) ifa_node->ifa_netmask)->sin6_addr;
            rc = OK;
            break;
        }
    }

    freeifaddrs (ifa);

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s]: rc=%d, if=%s, instance=%d\n", __FUNCTION__, rc, nameOfInterface, instance);

    return rc;
}

// TODO: check implementation.
// If an interface can have a prefix that is not tied to an IPv6 address, this logic cannot
// retrieve it. This logic only retrieves prefixes for IPv6 addresses assigned to an interface.
int hostIf_IPv6Address::getIPv6Prefix (int instance, struct in6_addr& in6_prefix_address, unsigned int& prefix_length)
{
    LOG_ENTRY_EXIT;

    struct in6_addr in6_address;
    struct in6_addr in6_mask;
    if (OK != getIPv6AddressAndMask (instance, in6_address, in6_mask))
        return NOK;

    // prefix address = ipv6 address & netmask (bitwise AND)
    for (int i = 3 ; i >= 0 ; i--)
    {
        in6_prefix_address.s6_addr32[i] = in6_address.s6_addr32[i] & in6_mask.s6_addr32[i];
    }

    // prefix length = number of 1 bits in netmask
    prefix_length = 0;
    // start iterating over netmask bytes (16 in all)
    // for each byte with all 1 bits (0xFF) that is encountered, add 8 to prefix_length
    for (uint8_t i = 0, number_of_1bits = 8, bitPattern = 0xFF; i < 16 && number_of_1bits == 8; i++)
    {
        // if byte is not 0xFF, find number of 1-bits in that byte, add that (instead of 8) to prefix_length, and stop iterating
        while (in6_mask.s6_addr[i] != bitPattern && number_of_1bits != 0)
        {
            bitPattern = bitPattern << 1; // shift in 0 into LSB to generate the new bit pattern to match against
            number_of_1bits--;
        }
        prefix_length += number_of_1bits;
    }

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s]: prefix_length =%u\n", __FUNCTION__, prefix_length);

    return OK;
}

/**
 * @brief tests if an IPv6 address is link-local.
 *
 * @param[in] in6_address the IPv6 address to test.
 *
 * @retval true if the IPv6 address is link-local.
 * @retval false otherwise.
 */
bool hostIf_IPv6Address::isLinkLocalAddress (const struct in6_addr& in6_address)
{
    LOG_ENTRY_EXIT;

    // check if the given IPv6 address falls in the IPv6 link-scope address range (fe80/10)

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s]: ntohs(in6_address.s6_addr16[0]) = %x, (ntohs(in6_address.s6_addr16[0]) & 0xffc0) = %x\n",
            __FUNCTION__, ntohs(in6_address.s6_addr16[0]), (ntohs(in6_address.s6_addr16[0]) & 0xffc0));
    return (ntohs(in6_address.s6_addr16[0]) & 0xffc0) == 0xfe80;
}

//Function to remove the IP Address for the Interface
int hostIf_IPv6Address::removeIp(int interfaceNo, char *value)
{
    LOG_ENTRY_EXIT;

    int rc = OK;
    struct ifreq ifr;
    struct sockaddr_in6 sai6;
    struct in6_ifreq ifr6;
    int sockfd;

    if(0 == strcasecmp(backupIPv6AddressOrigin, STATIC))
    {
        /*
         * Creating INET6 socket Here.
         */
        sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
        if( sockfd != -1)
        {
            /*
             * Socket Creation is successful.
             * Format the supplied IP address into INET6 protocol format
             */
            memset(&sai6, 0, sizeof(struct sockaddr));
            sai6.sin6_family = AF_INET6;
            sai6.sin6_port = 0;

            if(inet_pton(AF_INET6, value, (void *)&sai6.sin6_addr) <= 0)
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error Formatting IP Address");
                rc = NOK;
            }
            else
            {
                /*
                 * Got the supplied IP address in INET6 protocol format
                 * Set the interface number, address, prefix into ifr (interface request) variable
                 * Call the IOCTL command for remove IP.
                 */
                ifr6.ifr6_ifindex = interfaceNo;
                memcpy( (char *) &ifr6.ifr6_addr, (char *) &sai6.sin6_addr, sizeof(struct in6_addr));
                ifr6.ifr6_prefixlen = 64;

                if (ioctl(sockfd, SIOCDIFADDR, &ifr6) < 0)
                {
                    perror("SIOCDIFADDR: Error Deleting Existing IPAddress");
                    rc = NOK;
                }
            }
            close(sockfd);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error Creating Socket; Not Removing IP Address");
            rc = NOK;
        }

    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"The interface is not static; Cannot Remove IP Address\n");
        rc = NOK;
    }

    return rc;
}
//Function to set the IP Address for the Interface
int hostIf_IPv6Address::setIp(int interfaceNo, char *value)
{
    LOG_ENTRY_EXIT;

    int rc = OK;
    struct ifreq ifr;
    struct sockaddr_in6 sai6;
    struct in6_ifreq ifr6;
    int sockfd;

    if(0 == strcasecmp(backupIPv6AddressOrigin, STATIC))
    {
        /*
         * Creating INET6 socket Here to assign the IP address in interface.
         */
        sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
        if( sockfd != -1)
        {
            /*
             * Socket Creation is successful.
             * Format the supplied IP address into INET6 protocol format
             */
            memset(&sai6, 0, sizeof(struct sockaddr));
            sai6.sin6_family = AF_INET6;
            sai6.sin6_port = 0;

            if(inet_pton(AF_INET6, value, (void *)&sai6.sin6_addr) <= 0)
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error Formatting IP Address");
                rc = NOK;
            }
            else
            {
                /*
                 * Got the supplied IP address in INET6 protocol format
                 * Set the interface number, address, prefix into ifr (interface request) variable
                 * Call the IOCTL command for adding IP.
                 */
                ifr6.ifr6_ifindex = interfaceNo;
                memcpy( (char *) &ifr6.ifr6_addr, (char *) &sai6.sin6_addr, sizeof(struct in6_addr));
                ifr6.ifr6_prefixlen = 64;

                if (ioctl(sockfd, SIOCSIFADDR, &ifr6) < 0)
                {
                    perror("SIOCDIFADDR: Error Assigning IPAddress");
                    rc = NOK;
                }
            }
            close(sockfd);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error Creating Socket; Not Assigning IP Address");
            rc = NOK;
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"The interface is not static; Cannot Assign IP Address\n");
        rc = NOK;
    }

    return rc;
}

/****************************************************************************************************************************************************/
// Device_IP_Interface_IPv6Address Profile. Getters:
/****************************************************************************************************************************************************/

/**
 * @brief  This function provides the value 'true' or 'false' for Enable status of IPv6 address.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] subInstanceNo SubInstance number.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IPv6_INTERFACE_ADDRESS_API
 */
int hostIf_IPv6Address::get_IPv6Address_Enable(HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    struct in6_addr in6_address;
    struct in6_addr in6_mask;
    bool enable = (OK == getIPv6AddressAndMask (subInstanceNo, in6_address, in6_mask));

    if(bCalledEnable && pChanged && (backupIPv6AddressEnable != enable))
    {
        *pChanged = true;
    }
    bCalledEnable = true;
    backupIPv6AddressEnable = enable;
    put_int(stMsgData->paramValue,enable);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);

    return OK;
}

/**
 * @brief This function provides the status of this IPv6Address table entry. Possible values
 * - Disabled
 * - Enabled
 * - Error_Misconfigured
 * - Error (OPTIONAL)
 * @note  - The "Error_Misconfigured" value indicates that a necessary
 * configuration value is undefined or invalid.
 *        - The "Error" value MAY be used by the CPE to indicate a
 * locally defined error condition.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] subInstanceNo SubInstance number.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_DEVICE_IPv6_INTERFACE_ADDRESS_API
 */
int hostIf_IPv6Address::get_IPv6Address_Status(HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    char status[BUFF_LENGTH_16];
    struct in6_addr in6_address;
    struct in6_addr in6_mask;
    strcpy (status, (OK == getIPv6AddressAndMask (subInstanceNo, in6_address, in6_mask)) ? "Enabled" : "Disabled");

    if (bCalledStatus && pChanged && strncmp (status, backupIPv6AddressStatus, BUFF_LENGTH_16))
    {
        *pChanged = true;
    }
    bCalledStatus = true;
    strncpy (backupIPv6AddressStatus, status, BUFF_LENGTH_16);
    strncpy (stMsgData->paramValue, status, BUFF_LENGTH_16);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (status);

    return OK;
}

int hostIf_IPv6Address::get_IPv6Address_IPAddressStatus (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

/*
    About IPAddressStatus, from the spec:

    The status of IPAddress, indicating whether it can be used for communication. See also PreferredLifetime and
    ValidLifetime. Enumeration of:

        Preferred (Valid address that can appear as the destination or source address of a packet)

        Deprecated (Valid but deprecated address that is not intended to be used as a source address)

        Invalid (Invalid address that is not intended to appear as the destination or source address of a packet)

        Inaccessible (Valid address that is not accessible because the interface to which it is assigned is not operational)

        Unknown (Address status cannot be determined for some reason)

        Tentative (The uniqueness of the address on the link is being verified)

        Duplicate (Invalid address that has been determined to be non-unique on the link)

        Optimistic (Valid address that is available for use, subject to restrictions, while its uniqueness on a link is being verified)

    This parameter is based on ipAddressStatus and ipAddressStatusTC from [RFC4293].
*/

    long int valid_lft;
    if (false == get_ipv6address_valid_lifetime (subInstanceNo, valid_lft))
        return NOK;

    long int preferred_lft;
    if (false == get_ipv6address_preferred_lifetime (subInstanceNo, preferred_lft))
        return NOK;

    char interfaceOperationalState[BUFF_LENGTH_16];
    hostIf_IPInterface::getInstance (dev_id)->getInterfaceOperationalState (interfaceOperationalState);

    char ipaddressStatus[BUFF_LENGTH_16];
    if (valid_lft == 0)
        strcpy (ipaddressStatus, INVALID);
    else if (strcasecmp (interfaceOperationalState, STATE_DOWN) == 0)
        strcpy (ipaddressStatus, INACCESSIBLE);
    else if (preferred_lft == 0)
        strcpy (ipaddressStatus, DEPRECATED);
    else
        strcpy (ipaddressStatus, PREFERRED);
    // TODO: Unknown, Tentative, Duplicate, Optimistic

    // TODO:
    strncpy (stMsgData->paramValue, ipaddressStatus, BUFF_LENGTH_16);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (ipaddressStatus);

    return OK;
}
/**
 * @brief  This function provides a non-volatile handle used to reference this IPv6 address
 * instance of this IP interface. Alias provides a mechanism for an ACS to label this
 * instance for future reference. Currently not implemented.
 *
 * @note If the CPE supports the Alias-based Addressing feature as defined
 * in [Section 3.6.1/TR-069 Amendment 4] and described in [Appendix II/TR-069 Amendment 4],
 * the following mandatory constraints MUST be enforced:
 *  - Its value MUST NOT be empty.
 *  - Its value MUST start with a letter.
 *
 * If its instance object is created by the CPE, the initial value MUST start with a "cpe-" prefix.
 * The CPE MUST NOT change the parameter value. This parameter can only be modified if "Origin"
 * is "Static".
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] subInstanceNo SubInstance number currently not in use.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @retval NOK if not handle.
 * @ingroup TR69_HOSTIF_DEVICE_IPv6_INTERFACE_ADDRESS_API
 */
int hostIf_IPv6Address::get_IPv6Address_Alias(HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}

/**
 * @brief  This function provides the IPv6 address.
 * @note     This parameter can only be modified if "Origin" is
 *           "Static".
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] subInstanceNo SubInstance number.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @retval NOK if not handle.
 * @ingroup TR69_HOSTIF_DEVICE_IPv6_INTERFACE_ADDRESS_API
 */
int hostIf_IPv6Address::get_IPv6Address_IPAddress(HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    struct in6_addr in6_address;
    struct in6_addr in6_mask;
    if (OK != getIPv6AddressAndMask (subInstanceNo, in6_address, in6_mask))
        return NOK;

    char ipv6Address[BUFF_LENGTH_64];
    inet_ntop (AF_INET6, &in6_address, ipv6Address, BUFF_LENGTH_64);
    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s,%d]: if = %s, instance = %d, ipv6address = %s\n",
            __FUNCTION__, __LINE__, nameOfInterface, subInstanceNo, ipv6Address);

    if (bCalledIPAddress && pChanged && strncmp (ipv6Address, backupIPv6AddressIPAddress, BUFF_LENGTH_64))
    {
        *pChanged = true;
    }
    bCalledIPAddress = true;
    strncpy (backupIPv6AddressIPAddress, ipv6Address, BUFF_LENGTH_64);
    strncpy (stMsgData->paramValue, ipv6Address, BUFF_LENGTH_64);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (ipv6Address);

    return OK;
}

/**
 * @brief  This function provides the addressing method used to assign the IP address.
 * The Possible enum vlaues
 *
 * - DHCP
 * - AutoIP
 * - IPCP
 * - Static
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] subInstanceNo SubInstance number currently not in use.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @retval NOK if it is not handle.
 * @ingroup TR69_HOSTIF_DEVICE_IPv6_INTERFACE_ADDRESS_API
 */
int hostIf_IPv6Address::get_IPv6Address_Origin(HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

/*
    From the spec:

    Mechanism via which the IP address was assigned. Enumeration of:

        AutoConfigured (Automatically generated. For example, a link-local address as specified by SLAAC [Section 5.3/RFC4862],
        a global address as specified by SLAAC [Section 5.5/RFC4862], or generated via CPE logic (e.g. from delegated prefix
        as specified by [RFC3633]), or from ULA /48 prefix as specified by [RFC4193])

        DHCPv6 (Assigned by DHCPv6 [RFC3315])

        IKEv2 (Assigned by IKEv2 [RFC5996])

        MAP (Assigned by MAP [RFC7597], i.e. is this interface's MAP IPv6 address)

        WellKnown (Specified by a standards organization, e.g. the ::1 loopback address, which is defined in [RFC4291])

        Static (For example, present in the factory default configuration (but not WellKnown), created by the ACS, or created
        by some other management entity (e.g. via a GUI))

    This parameter is based on ipOrigin from [RFC4293].
*/

    // verify we have an IPv6 address for the given instance
    struct in6_addr in6_address;
    struct in6_addr in6_mask;
    if (OK != getIPv6AddressAndMask (subInstanceNo, in6_address, in6_mask))
        return NOK;

    char origin[BUFF_LENGTH_32];
    if (hostIf_IPInterface::isLoopback (nameOfInterface))
    {
        strcpy (origin, WELLKNOWN);
    }
    else if (isLinkLocalAddress (in6_address))
    {
        strcpy (origin, AUTOCONFIGURED);
    }
    else
    {
        strcpy (origin, AUTOCONFIGURED); // otherwise assume "AutoConfigured" (even for sit0's IPv4-compatible address "::127.0.0.1"/96 ?)
    }

    if (bCalledOrigin && pChanged && strncmp (origin, backupIPv6AddressOrigin, BUFF_LENGTH_32))
    {
        *pChanged = true;
    }
    bCalledOrigin = true;
    strncpy (backupIPv6AddressOrigin, origin, BUFF_LENGTH_32);
    strncpy (stMsgData->paramValue, origin, BUFF_LENGTH_32);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (origin);

    return OK;
}

int hostIf_IPv6Address::get_IPv6Address_Prefix (HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

/*
    From the spec:

    The value MUST be the path name of a row in the IPv6Prefix table. If the referenced object is deleted,
    the parameter value MUST be set to an empty string. IPv6 address prefix.

    Some addresses, e.g. addresses assigned via the DHCPv6 IA_NA option, are not associated with a prefix,
    and some WellKnown prefixes might not be modeled. In both of these cases Prefix will be an empty string.

    This parameter can only be modified if the Origin is Static.

    This parameter is based on ipAddressPrefix from [RFC4293].
*/

    // if an IPv6 address exists for this instance #, then a prefix exists as well.
    // so proceed only if an IPv6 address exists, else error.
    struct in6_addr in6_address;
    struct in6_addr in6_mask;
    if (OK != getIPv6AddressAndMask (subInstanceNo, in6_address, in6_mask))
        return NOK;

    // path name of corresponding row in IPv6Prefix table can be generated from this TR069 parameter's name:
    // Device.IP.Interface.{i}.IPv6Address.{j}.Prefix (where j is 'subInstanceNo' parameter (2nd parameter) passed to this method
    // as
    // Device.IP.Interface.{i}.IPv6Prefix.{j}.

    char pathnameOfRowInIPv6PrefixTable[TR69HOSTIFMGR_MAX_PARAM_LEN];
    strcpy (pathnameOfRowInIPv6PrefixTable, stMsgData->paramName);
    const char *positionAfterInstanceNumber = 0;
    int instanceNumber = 0;
    matchComponent (pathnameOfRowInIPv6PrefixTable, "Device.IP.Interface", &positionAfterInstanceNumber, instanceNumber);
    sprintf ((char*) positionAfterInstanceNumber, "%s%d.", "IPv6Prefix.", subInstanceNo);

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s,%d] pathnameOfRowInIPv6PrefixTable = %s",
            __FUNCTION__, __LINE__, pathnameOfRowInIPv6PrefixTable);

    strncpy (stMsgData->paramValue, pathnameOfRowInIPv6PrefixTable, TR69HOSTIFMGR_MAX_PARAM_LEN);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (pathnameOfRowInIPv6PrefixTable);

    return OK;
}

int hostIf_IPv6Address::get_IPv6Address_PreferredLifetime (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    long int lifetime;
    if (false == get_ipv6address_preferred_lifetime (subInstanceNo, lifetime))
        return NOK;

    char preferredLifetime[BUFF_LENGTH_32];
    convert_lifetime_to_string (lifetime, preferredLifetime);

    // TODO:
    strncpy (stMsgData->paramValue, preferredLifetime, BUFF_LENGTH_32);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (preferredLifetime);

    return OK;
}

int hostIf_IPv6Address::get_IPv6Address_ValidLifetime (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    long int lifetime;
    if (false == get_ipv6address_valid_lifetime (subInstanceNo, lifetime))
        return NOK;

    char validLifetime[BUFF_LENGTH_32];
    convert_lifetime_to_string (lifetime, validLifetime);

    // TODO:
    strncpy (stMsgData->paramValue, validLifetime, BUFF_LENGTH_32);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (validLifetime);

    return OK;
}

int hostIf_IPv6Address::get_IPv6Address_Anycast (HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    // from the spec:
    // Indicates whether this is an anycast address [Section 2.6/RFC4291]. Anycast
    // addresses are syntactically identical to unicast addresses and so need to be
    // configured explicitly.

    // Assuming anycast IPv6 addresses are not used on STBs, so returning false
    put_int (stMsgData->paramValue, false);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);

    return OK;
}

/**
 * returns:
 * false if there was an error while trying to determine the specified lifetime
 * true otherwise
 *
 * below are possible values for the output parameter 'lifetime':
 * -1 if lifetime cannot be determined
 * -2 if lifetime is forever
 * a non-negative value otherwise
 */
bool hostIf_IPv6Address::get_ipv6address_lifetime (unsigned int instance, char* search_string, long int& lifetime)
{
    LOG_ENTRY_EXIT;

    // https://www.broadband-forum.org/cwmp/tr-181-2-11-0.html#D.Device:2.Device.IP.Interface.{i}.IPv6Address.{i}.
    //
    // PreferredLifetime:
    // The time at which this address will cease to be preferred (i.e. will become
    // deprecated), or 0001-01-01T00:00:00Z if not known. For an infinite lifetime, the
    // parameter value MUST be 9999-12-31T23:59:59Z.
    //
    // ValidLifetime:
    // The time at which this address will cease to be valid (i.e. will become invalid), or
    // 0001-01-01T00:00:00Z if unknown. For an infinite lifetime, the parameter value
    // MUST be 9999-12-31T23:59:59Z.

    char cmd[BUFF_LENGTH_64] = { 0 };
    char resultBuff[BUFF_LENGTH_128] = { 0 };
    snprintf (cmd, BUFF_LENGTH_64, "ip -6 -o addr show %s | awk '{i++;if(i==%d){print}}'", nameOfInterface, instance);
    if (read_command_output (cmd, resultBuff, BUFF_LENGTH_128) != OK)
        return false;
    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s,%d] resultBuff = %s", __FUNCTION__, __LINE__, resultBuff);

    char *str = strstr (resultBuff, search_string);
    if (str == NULL)
    {
        lifetime = -1;
        return true;
    }

    str += strlen (search_string);
    while (isspace (*str))
        str++;

    char strLifetime[BUFF_LENGTH_64];
    int count = sscanf (str, "%s", strLifetime);

    if (strcmp (strLifetime, "forever") == 0)
    {
        lifetime = -2;
        return true;
    }

    char *time_units;
    long int time_remaining = strtol (strLifetime, &time_units, 10);
    long int seconds_remaining = (strncasecmp (time_units, "s", 1) == 0) ? time_remaining :
                                 (strncasecmp (time_units, "m", 1) == 0) ? time_remaining * 60 : -1;
    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s,%d] time_remaining = %ld, time_units = %s, seconds_remaining = %ld\n",
            __FUNCTION__, __LINE__, time_remaining, time_units, seconds_remaining);

    if (seconds_remaining == -1) // invalid time = unknown
    {
        lifetime = -1;
        return true;
    }

    // lifetime = (current time + seconds_remaining) as spec calls for a time value
    lifetime = time (0) + seconds_remaining;
    return true;
}

/**
 * returns:
 * false if there was an error while trying to determine the preferred lifetime
 * true otherwise
 *
 * below are possible values for the output parameter 'preferred_lifetime':
 * -1 if preferred lifetime cannot be determined
 * -2 if preferred lifetime is forever
 * a non-negative value otherwise
 */
bool hostIf_IPv6Address::get_ipv6address_preferred_lifetime (unsigned int instance, long int& preferred_lifetime)
{
    return hostIf_IPv6Address::get_ipv6address_lifetime (instance, "preferred_lft", preferred_lifetime);
}

/**
 * returns:
 * false if there was an error while trying to determine the valid lifetime
 * true otherwise
 *
 * below are possible values for the output parameter 'valid_lifetime':
 * -1 if valid lifetime cannot be determined
 * -2 if valid lifetime is forever
 * a non-negative value otherwise
 */
bool hostIf_IPv6Address::get_ipv6address_valid_lifetime (unsigned int instance, long int& valid_lifetime)
{
    return hostIf_IPv6Address::get_ipv6address_lifetime (instance, "valid_lft", valid_lifetime);
}

void hostIf_IPv6Address::convert_lifetime_to_string (long int t, char* lifetime)
{
    LOG_ENTRY_EXIT;

    lifetime[0] = 0;

    if (t == -1) // // -1 = unknown
        strcpy (lifetime, TIME_UNKNOWN); // "0001-01-01T00:00:00Z"
    else if  (t == -2) // -2 = forever
        strcpy (lifetime, TIME_INFINITY); // "9999-12-31T23:59:59Z"
    else
    {
        tm tmp;
        gmtime_r (&t, &tmp);
        strftime (lifetime, BUFF_LENGTH_128, "%Y-%m-%dT%H:%M:%SZ", &tmp);
    }
}


int hostIf_IPv6Address::get_IPv6Prefix_Enable(HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    struct in6_addr in6_prefix_address;
    unsigned int prefix_length;
    bool enable = (OK == getIPv6Prefix (subInstanceNo, in6_prefix_address, prefix_length));

    // TODO: use a variable other than bCalledEnable which is for IPv6Address profile
    if (bCalledEnable && pChanged && (backupIPv6PrefixEnable != enable))
    {
        *pChanged = true;
    }
    bCalledEnable = true;
    backupIPv6PrefixEnable = enable;
    put_int (stMsgData->paramValue, enable);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);

    return OK;
}

int hostIf_IPv6Address::get_IPv6Prefix_Status(HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    char status[BUFF_LENGTH_16];
    struct in6_addr in6_prefix_address;
    unsigned int prefix_length;
    strcpy (status, (OK == getIPv6Prefix (subInstanceNo, in6_prefix_address, prefix_length)) ? "Enabled" : "Disabled");

    // TODO: use a variable other than bCalledStatus which is for IPv6Address profile
    if (bCalledStatus && pChanged && strncmp (status, backupIPv6PrefixStatus, BUFF_LENGTH_16))
    {
        *pChanged = true;
    }
    bCalledStatus = true;
    strncpy (backupIPv6PrefixStatus, status, BUFF_LENGTH_16);
    strncpy (stMsgData->paramValue, status, BUFF_LENGTH_16);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (status);

    return OK;
}

int hostIf_IPv6Address::get_IPv6Prefix_PrefixStatus (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

/*
    About PrefixStatus, from the spec:

    The status of Prefix, indicating whether it can be used for communication. See also PreferredLifetime and
    ValidLifetime. Enumeration of:

        Preferred (Valid prefix)

        Deprecated (Valid but deprecated prefix)

        Invalid (Invalid prefix)

        Inaccessible (Valid prefix that is not accessible because the interface to which it is assigned is not
        operational)

        Unknown (Prefix status cannot be determined for some reason)

    This parameter is based on ipAddressStatus and ipAddressStatusTC from [RFC4293].
*/

    // TODO: Where do we look to find information about the valid and preferred lifetimes of a prefix ?
    // For now, valid/preferred lifetimes of the IPv6 address associated with this prefix are used
    // as the basis for deriving prefix status

    long int valid_lft;
    if (false == get_ipv6address_valid_lifetime (subInstanceNo, valid_lft))
        return NOK;

    long int preferred_lft;
    if (false == get_ipv6address_preferred_lifetime (subInstanceNo, preferred_lft))
        return NOK;

    char interfaceOperationalState[BUFF_LENGTH_16];
    hostIf_IPInterface::getInstance (dev_id)->getInterfaceOperationalState (interfaceOperationalState);

    char prefixStatus[BUFF_LENGTH_16];
    if (valid_lft == 0)
        strcpy (prefixStatus, INVALID);
    else if (strcasecmp (interfaceOperationalState, STATE_DOWN) == 0)
        strcpy (prefixStatus, INACCESSIBLE);
    else if (preferred_lft == 0)
        strcpy (prefixStatus, DEPRECATED);
    else
        strcpy (prefixStatus, PREFERRED);
    // TODO: Unknown

    // TODO:
    strncpy (stMsgData->paramValue, prefixStatus, BUFF_LENGTH_16);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (prefixStatus);

    return OK;
}

int hostIf_IPv6Address::get_IPv6Prefix_Alias(HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}

int hostIf_IPv6Address::get_IPv6Prefix_Prefix(HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    struct in6_addr in6_prefix_address;
    unsigned int prefix_length;
    if (OK != getIPv6Prefix (subInstanceNo, in6_prefix_address, prefix_length))
        return NOK;

    char prefix_address[BUFF_LENGTH_64];
    inet_ntop (AF_INET6, &in6_prefix_address, prefix_address, 64);

    char prefix[BUFF_LENGTH_64];
    sprintf (prefix, "%s/%u", prefix_address, prefix_length);
    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s]: if=%s, instance=%d, prefix=%s\n",
            __FUNCTION__, nameOfInterface, subInstanceNo, prefix);

    // TODO: use a variable other than bCalledIPAddress which is for IPv6Address profile
    if (bCalledIPAddress && pChanged && strncmp (prefix, backupIPv6PrefixPrefix, BUFF_LENGTH_64))
    {
        *pChanged = true;
    }
    bCalledIPAddress = true;
    strncpy (backupIPv6PrefixPrefix, prefix, BUFF_LENGTH_64);
    strncpy (stMsgData->paramValue, prefix, BUFF_LENGTH_64);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (prefix);

    return OK;
}

int hostIf_IPv6Address::get_IPv6Prefix_Origin (int instance, char* origin)
{
    LOG_ENTRY_EXIT;

/*
    Mechanism via which the prefix was assigned or most recently updated. Enumeration of:
        AutoConfigured (Generated via internal CPE logic (e.g. the ULA /48 prefix) or derived from
        an internal prefix that is not modeled in any IPv6Prefix table)

        PrefixDelegation (Delegated via DHCPv6 [RFC3633] or some other protocol, e.g. IPv6rd
        [RFC5969]. Also see StaticType)

        RouterAdvertisement (Discovered via router advertisement [RFC4861] Prefix Information
        Option)

        WellKnown (Specified by a standards organization, e.g. fe80::/10 for link-local addresses,
        or ::1/128 for the loopback address, both of which are defined in [RFC4291])

        Static (Created by the ACS, by some other management entity (e.g. via a GUI), or present in
        the factory default configuration (but not WellKnown). Unrelated to any shorter length
        prefix that might exist on the CPE. Also see StaticType. Can be used for RA (Prefix
        Information), DHCPv6 address assignment (IA_NA) or DHCPv6 prefix delegation (IA_PD))

        Child (Derived from an associated AutoConfigured or PrefixDelegation parent prefix. Also
        see StaticType, ParentPrefix and ChildPrefixBits. Can be used for RA (Prefix Information),
        DHCPv6 address assignment (IA_NA) or DHCPv6 prefix delegation (IA_PD))

    Note that:
        PrefixDelegation and RouterAdvertisement prefixes can exist only on upstream interfaces
        (i.e. interfaces for which the physical layer interface object has Upstream = true),
        AutoConfigured and WellKnown prefixes can exist on any interface, and
        Static and Child prefixes can exist only on downstream interfaces (i.e. interfaces for
        which the physical layer interface object has Upstream = false).

    Also note that a Child prefix's ParentPrefix will always be an AutoConfigured, PrefixDelegation,
    or RouterAdvertisement prefix.

    This parameter is based on ipAddressOrigin from [RFC4293].
*/

    // verify we have an IPv6 address for the given instance
    struct in6_addr in6_address;
    struct in6_addr in6_mask;
    if (OK != getIPv6AddressAndMask (instance, in6_address, in6_mask))
        return NOK;

    if (hostIf_IPInterface::isLoopback (nameOfInterface) || isLinkLocalAddress (in6_address))
    {
        strcpy (origin, WELLKNOWN); // "WellKnown" for loopback and link-local addresses
    }
    else
    {
        strcpy (origin, AUTOCONFIGURED); // "AutoConfigured" - otherwise (assume)
        // TODO: for a gateway, this should be "PrefixDelegation" (Delegated via DHCPv6)
    }
    return OK;
}

int hostIf_IPv6Address::get_IPv6Prefix_Origin(HOSTIF_MsgData_t *stMsgData,int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    char origin[BUFF_LENGTH_32];
    if (OK != get_IPv6Prefix_Origin (subInstanceNo, origin))
        return NOK;

    // TODO: use a variable other than bCalledOrigin which is for IPv6Address profile
    if (bCalledOrigin && pChanged && strncmp (origin, backupIPv6PrefixOrigin, BUFF_LENGTH_32))
    {
        *pChanged = true;
    }
    bCalledOrigin = true;
    strncpy (backupIPv6PrefixOrigin, origin, BUFF_LENGTH_32);
    strncpy (stMsgData->paramValue, origin, BUFF_LENGTH_32);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (origin);

    return OK;
}

int hostIf_IPv6Address::get_IPv6Prefix_StaticType (int instance, char* staticType)
{
    LOG_ENTRY_EXIT;

/*
    Static prefix sub-type. For a Static prefix, this can be set to PrefixDelegation or Child, thereby creating an
    unconfigured prefix of the specified type that will be populated in preference to creating a new instance. This
    allows the ACS to pre-create "prefix slots" with known path names that can be referenced from elsewhere in the data
    model before they have been populated. Enumeration of:

        Static (Prefix is a "normal" Static prefix)

        Inapplicable (Prefix is not Static, so this parameter does not apply, READONLY)

        PrefixDelegation (Prefix will be populated when a PrefixDelegation prefix needs to be created)

        Child (Prefix will be populated when a Child prefix needs to be created. In this case, the ACS needs also to set
        ParentPrefix and might want to set ChildPrefixBits (if parent prefix is not set, or goes away, then the child
        prefix will become operationally disabled))

    This mechanism works as follows:

        When this parameter is set to PrefixDelegation or Child, the instance becomes a "prefix slot" of the specified
        type.
        Such an instance can be administratively enabled (Enable = true) but will remain operationally disabled (Status
        = Disabled) until it has been populated.
        When a new prefix of of type T is needed, the CPE will look for a matching unpopulated instance, i.e. an
        instance with (Origin,StaticType,Prefix) = (Static,T,""). If the CPE finds at least one such instance it will
        choose one and populate it. If already administratively enabled it will immediately become operationally
        enabled. If the CPE finds no such instances, it will create and populate a new instance with (Origin,StaticType)
        = (T,T). If the CPE finds more than one such instance, the algorithm via which it chooses which instance to
        populate is implementation-specific.
        When a prefix that was populated via this mechanism becomes invalid, the CPE will reset Prefix to an empty
        string. This does not affect the value of the Enable parameter.

    The prefix StaticType can only be modified if Origin is Static.
*/

    // first check if the StaticType parameter is relevant

    char origin[BUFF_LENGTH_32];
    if (OK != get_IPv6Prefix_Origin (instance, origin))
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Error retrieving IPv6Prefix Origin\n", __FUNCTION__);
        return NOK;
    }

    // if Prefix is not Static, this parameter does not apply and StaticType should be set to "Inapplicable"
    if (strcasecmp (origin, STATIC) != 0)
    {
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: StaticType = Inapplicable (IPv6Prefix origin not Static)\n", __FUNCTION__);
        strcpy (staticType, INAPPLICABLE);
        return OK;
    }

    // TODO: Implement StaticType
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    strcpy (staticType, NOT_IMPLEMENTED);

    return OK;
}

int hostIf_IPv6Address::get_IPv6Prefix_StaticType (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    char staticType[BUFF_LENGTH_32];
    if (OK != get_IPv6Prefix_StaticType (subInstanceNo, staticType))
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Error retrieving IPv6Prefix StaticType\n", __FUNCTION__);
        return NOK;
    }

    strcpy (stMsgData->paramValue, staticType);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (staticType);

    return OK;
}

int hostIf_IPv6Address::get_IPv6Prefix_ParentPrefix (int instance, char* parentPrefix)
{
    LOG_ENTRY_EXIT;

/*
    The value MUST be the path name of a row in the IP.Interface.{i}.IPv6Prefix table. If the referenced object is
    deleted, the parameter value MUST be set to an empty string. Indicates the parent prefix from which this prefix was
    derived. The parent prefix is relevant only for Child prefixes and for Static Child prefixes (both of which will
    always be on downstream interfaces), i.e. for Origin=Child and for (Origin,StaticType) = (Static,Child) prefixes.

    This parameter can only be modified if Origin is Static (which makes sense only for a prefix whose StaticType is
    already or will be changed to Child).
*/

    // first check if the ParentPrefix parameter is relevant

    char origin[BUFF_LENGTH_32];
    if (OK != get_IPv6Prefix_Origin (instance, origin))
        return NOK;

    bool relevant = false;
    if (strcasecmp (origin, CHILD) == 0)
    {
        relevant = true;
    }
    else
    {
        char staticType[BUFF_LENGTH_32];
        if (OK != get_IPv6Prefix_StaticType (instance, staticType))
            return NOK;
        if (strcasecmp (staticType, CHILD) == 0)
            relevant = true;
    }

    if (!relevant)
    {
        strcpy (parentPrefix, "");
        return OK;
    }

    // TODO: Implement ParentPrefix
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    strcpy (parentPrefix, NOT_IMPLEMENTED);

    return OK;
}

int hostIf_IPv6Address::get_IPv6Prefix_ParentPrefix (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    char parentPrefix[BUFF_LENGTH_64];
    if (OK != get_IPv6Prefix_ParentPrefix (subInstanceNo, parentPrefix))
        return NOK;

    strcpy (stMsgData->paramValue, parentPrefix);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (parentPrefix);

    return OK;
}

int hostIf_IPv6Address::get_IPv6Prefix_ChildPrefixBits (int instance, char* childPrefixBits)
{
    LOG_ENTRY_EXIT;

/*
    [IPv6Prefix] A prefix that specifies the length of Static Child prefixes and how they are derived from their
    ParentPrefix. It will be used if and only if it is not an empty string and is longer than the parent prefix (if it
    is not used, derivation of such prefixes is implementation-specific). Any bits to the right of the parent prefix
    are set to the bits in this prefix.

    For example, for a parent prefix of fedc::/56, if this parameter had the value 123:4567:89ab:cdef::/64, the child
    /64 would be fedc:0:0:ef::/64. For a parent prefix of fedc::/60, the child /64 would be fedc:0:0:f::/64.

    This parameter can only be modified if Origin is Static.
*/

    char staticType[BUFF_LENGTH_32];
    if (OK != get_IPv6Prefix_StaticType (instance, staticType))
        return NOK;

    if (strcasecmp (staticType, CHILD) != 0)
    {
        strcpy (childPrefixBits, "");
        return OK;
    }

    // TODO: Implement ChildPrefixBits
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    strcpy (childPrefixBits, NOT_IMPLEMENTED);

    return OK;
}

int hostIf_IPv6Address::get_IPv6Prefix_ChildPrefixBits (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged)
{
    LOG_ENTRY_EXIT;

    char childPrefixBits[BUFF_LENGTH_64];
    if (OK != get_IPv6Prefix_ChildPrefixBits (subInstanceNo, childPrefixBits))
        return NOK;

    strcpy (stMsgData->paramValue, childPrefixBits);
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen (childPrefixBits);

    return OK;
}

int hostIf_IPv6Address::get_IPv6Prefix_OnLink (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged)
{
/*
    On-link flag [Section 4.6.2/RFC4861] as received (in the RA) for RouterAdvertisement. Indicates whether this prefix
    can be used for on-link determination.

    This parameter can only be modified if Origin is Static.

    This parameter is based on ipAddressPrefixOnLinkFlag from [RFC4293].
*/

    LOG_ENTRY_EXIT;

    put_int(stMsgData->paramValue, false);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);

    return OK;
}

int hostIf_IPv6Address::get_IPv6Prefix_Autonomous (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged)
{
/*
    Autonomous address configuration flag [Section 4.6.2/RFC4861] as received (in the RA) for RouterAdvertisement.
    Indicates whether this prefix can be used for generating global addresses as specified by SLAAC [RFC4862].

    This parameter can only be modified if Origin is Static.

    This parameter is based on ipAddressPrefixAutonomousFlag from [RFC4293].
*/

    LOG_ENTRY_EXIT;

    put_int(stMsgData->paramValue, false);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = sizeof(bool);

    return OK;
}

int hostIf_IPv6Address::get_IPv6Prefix_PreferredLifetime (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged)
{
/*
    This parameter is based on ipAddressPrefixAdvPreferredLifetime from [RFC4293]. The time at which this prefix will
    cease to be preferred (i.e. will become deprecated), or 0001-01-01T00:00:00Z if not known. For an infinite lifetime,
    the parameter value MUST be 9999-12-31T23:59:59Z.

    This parameter can only be modified if Origin is Static.
*/

    LOG_ENTRY_EXIT;

    return get_IPv6Address_PreferredLifetime (stMsgData, subInstanceNo, pChanged);
}

int hostIf_IPv6Address::get_IPv6Prefix_ValidLifetime (HOSTIF_MsgData_t *stMsgData, int subInstanceNo, bool *pChanged)
{
/*
    This parameter is based on ipAddressPrefixAdvValidLifetime from [RFC4293]. The time at which this prefix will cease
    to be valid (i.e. will become invalid), or 0001-01-01T00:00:00Z if not known. For an infinite lifetime, the
    parameter value MUST be 9999-12-31T23:59:59Z.

    This parameter can only be modified if Origin is Static.
*/
    LOG_ENTRY_EXIT;

    return get_IPv6Address_ValidLifetime (stMsgData, subInstanceNo, pChanged);
}


/****************************************************************************************************************************************************/
// Device_IP_Interface_IPv6Address Profile. Setters:
/****************************************************************************************************************************************************/
/**
 * @brief This function enables or disables this IPv6 address. Currently not
 * implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] subInstanceNo SubInstance number currently not in use.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @retval NOK if it is not handle.
 * @ingroup TR69_HOSTIF_DEVICE_IPv6_INTERFACE_ADDRESS_API
 */
int hostIf_IPv6Address::set_IPv6Address_Enable(HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}

/**
 * @brief  This function sets a non-volatile handle used to reference this IPv6 address
 * instance of this IP interface. Alias provides a mechanism for an ACS to label this
 * instance for future reference.
 *
 * @note     If the CPE supports the Alias-based Addressing feature as defined in
 *           [Section 3.6.1/TR-069 Amendment 4] and described in [Appendix II/TR-069
 *           Amendment 4], the following mandatory constraints MUST be enforced:
 *               - Its value MUST NOT be empty.
 *               - Its value MUST start with a letter.
 *               - If its instance object is created by the CPE, the initial
 *                   value MUST start with a "cpe-" prefix.
 *               - The CPE MUST NOT change the parameter value.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] subInstanceNo SubInstance number currently not in use.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @retval NOK if it is not handle.
 * @ingroup TR69_HOSTIF_DEVICE_IPv6_INTERFACE_ADDRESS_API
 */
int hostIf_IPv6Address::set_IPv6Address_Alias(HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}

/**
 * @brief  This function sets the IPv6 address.
 *
 * @note  This parameter can only be modified if the Origin is <tt>Static</tt>.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] subInstanceNo SubInstance number currently not in use.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @retval NOK if it is not handle.
 * @ingroup TR69_HOSTIF_DEVICE_IPv6_INTERFACE_ADDRESS_API
 */
int hostIf_IPv6Address::set_IPv6Address_IPAddress(HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;

    /*
     * Getting IPV6 info for the supplied instance
     */
    struct in6_addr in6_address;
    struct in6_addr in6_mask;
    int rc = getIPv6AddressAndMask (subInstanceNo, in6_address, in6_mask);

    char ipAddress[BUFF_LENGTH_64];
    inet_ntop (AF_INET6, &in6_address, ipAddress, BUFF_LENGTH_64);

    /*
     * If IPV6 address has already been available, remove it.
     */
    if (OK == rc && (strcmp (ipAddress, "") != 0))
    {
        rc = removeIp (dev_id, ipAddress);
    }

    /* 
     * Assign a new IP address
     */
    if (OK == rc)
    {
        rc = setIp (dev_id, stMsgData->paramValue);
    }

    return rc;
}

/* End of doxygen group */
/**
 * @}
 */

int hostIf_IPv6Address::set_IPv6Address_Prefix (HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}

int hostIf_IPv6Address::set_IPv6Address_PreferredLifetime (HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}

int hostIf_IPv6Address::set_IPv6Address_ValidLifetime (HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}

int hostIf_IPv6Address::set_IPv6Address_Anycast (HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}

int hostIf_IPv6Address::set_IPv6Prefix_Enable (HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}

int hostIf_IPv6Address::set_IPv6Prefix_Alias (HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}

int hostIf_IPv6Address::set_IPv6Prefix_Prefix (HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}

int hostIf_IPv6Address::set_IPv6Prefix_StaticType (HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}

int hostIf_IPv6Address::set_IPv6Prefix_ParentPrefix (HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}

int hostIf_IPv6Address::set_IPv6Prefix_ChildPrefixBits (HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}

int hostIf_IPv6Address::set_IPv6Prefix_OnLink (HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}

int hostIf_IPv6Address::set_IPv6Prefix_Autonomous (HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}

int hostIf_IPv6Address::set_IPv6Prefix_PreferredLifetime (HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}

int hostIf_IPv6Address::set_IPv6Prefix_ValidLifetime (HOSTIF_MsgData_t *stMsgData, int subInstanceNo)
{
    LOG_ENTRY_EXIT;
    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Not Implemented\n", __FUNCTION__);
    return NOK;
}
