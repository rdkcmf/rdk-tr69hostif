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

/** @addtogroup Device.IP.Interface. Implementation
 *  This is the implementation of the Device Public API.
 *  @{
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
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] IP : Parameter \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
        stMsgData->faultCode = fcInvalidParameterName;
        ret = NOK;
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
    else
    {
        stMsgData->faultCode = fcInvalidParameterName;
        ret = NOT_HANDLED;
    }

    return ret;
}

/****************************************************************************************************************************************************/
// Device.IP.Interface. Profile. Getters:
/****************************************************************************************************************************************************/

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

int hostIf_IPInterface::get_Interface_Alias(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    return NOK;
}

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

int hostIf_IPInterface::get_Interface_LastChange(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Parameter Not Supported \n",__FUNCTION__);

    return NOK;
}

int hostIf_IPInterface::get_Interface_LowerLayers(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Parameter Not Supported \n",__FUNCTION__);

    return NOK;
}

int hostIf_IPInterface::get_Interface_Router(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Parameter Not Supported \n",__FUNCTION__);

    return NOK;
}

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

int hostIf_IPInterface::set_Interface_Enable(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    set_Interface_Enable (get_boolean(stMsgData->paramValue));

    return OK;
}

int hostIf_IPInterface::set_Interface_IPv4Enable(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    set_Interface_Enable (get_boolean (stMsgData->paramValue));

    return OK;
}

int hostIf_IPInterface::set_Interface_ULAEnable(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Parameter Not Supported \n",__FUNCTION__);

    return NOK;
}

int hostIf_IPInterface::set_Interface_Alias(HOSTIF_MsgData_t *stMsgData)
{
    return NOK;
}

int hostIf_IPInterface::set_Interface_LowerLayers(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Parameter Not Supported \n",__FUNCTION__);

    return NOK;
}

int hostIf_IPInterface::set_Interface_Router(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Parameter Not Supported \n",__FUNCTION__);

    return NOK;
}

int hostIf_IPInterface::set_Interface_Reset(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    set_Interface_Reset (get_boolean (stMsgData->paramValue));

    return OK;
}

int hostIf_IPInterface::set_Interface_MaxMTUSize(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    set_Interface_Mtu (get_int (stMsgData->paramValue));

    return OK;
}

int hostIf_IPInterface::set_Interface_Loopback(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Parameter Not Supported \n",__FUNCTION__);

    return NOK;
}

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
