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
 * @file Device_IP.cpp
 * @brief This source file contains the APIs of device IP.
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


#include "Device_IP.h"
#include <net/if.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include "safec_lib.h"


IP hostIf_IP::stIPInstance = {TRUE,FALSE,{"Disabled"},FALSE,0,0};

char* hostIf_IP::cmd_NumOfActivePorts = "cat /proc/net/tcp | awk '$4 == \"0A\" || $4 == \"01\" {print $2" "$3" "$4}' | wc -l";

GMutex* hostIf_IP::m_mutex = NULL;

GHashTable *hostIf_IP::ifHash = NULL;

GHashTable* hostIf_IP::m_notifyHash = NULL;

/**
 * @brief Class Constructor of the class hostIf_IP.
 *
 * It will initialize the device id ,backup IP4 status to empty string.
 *
 * @param[in] dev_id Device identification number.
 */
hostIf_IP::hostIf_IP(int dev_id):
    dev_id(dev_id),
    bCalledIPv4Capable(false),
    bCalledIPv4Enable(false),
    bCalledIPv4Status(false),
    bCalledInterfaceNumberOfEntries(false),
    bCalledActivePortNumberOfEntries(false),
    backupIPv4Capable(false),
    backupIPv4Enable(false),
    backupInterfaceNumberOfEntries(0),
    backupActivePortNumberOfEntries(0)
{
    backupIPv4Status[0]='\0';
}

hostIf_IP* hostIf_IP::getInstance(int dev_id)
{
    hostIf_IP* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_IP *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_IP(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create MoCA Interface instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_IP::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_IP::closeInstance(hostIf_IP *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_IP::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_IP* pDev = (hostIf_IP *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

void hostIf_IP::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void hostIf_IP::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

GHashTable* hostIf_IP::getNotifyHash()
{
    if(m_notifyHash)
    {
        return m_notifyHash;
    }
    else
    {
        return m_notifyHash = g_hash_table_new(g_str_hash, g_str_equal);
    }
}

hostIf_IP::~hostIf_IP()
{
    if(m_notifyHash)
    {
        g_hash_table_destroy(m_notifyHash);
    }
}

/**
 * This function's interface is just like that of the Linux API "if_indextoname" except that
 * virtual interfaces are also reported.
 *
 * Maps an interface index to its corresponding name. The returned name is placed in the buffer
 * pointed to by if_name, which must be at least IFNAMSIZ bytes in length. If the index was invalid,
 * the function's return value is a null pointer, otherwise it is if_name.
 *
 * Example mapping below. All virtual interface indexes are greater than physical interface indexes.
 * 1 - lo
 * 2 - eth1
 * 3 - sit0
 * 4 - eth1:0
 */
char* hostIf_IP::getInterfaceName (int if_index, char* if_name)
{
    char* ret;
    if ((ret = if_indextoname (if_index, if_name)) == NULL)
    {
        // check for virtual interfaces also
        struct if_nameindex* phy_if_list = if_nameindex ();
        if (phy_if_list == NULL)
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s(): if_nameindex Error\n", __FUNCTION__);
            return NULL;
        }

        int phy_if_count = getPhysicalInterfaceNumberOfEntries (phy_if_list);
        if (if_index > phy_if_count)
        {
            ret = getVirtualInterfaceName (phy_if_list, if_index - phy_if_count, if_name);
        }

        if_freenameindex (phy_if_list);
    }

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: if_index = %d, if_name = %s\n", __FUNCTION__, if_index, if_name);

    return ret;
}

/** Description: Counts the number of IP
 *               interfaces present in the device.
 *
 * \Return:  Count value or '0' if error
 *
 */
unsigned int hostIf_IP::getInterfaceNumberOfEntries(void)
{
    struct if_nameindex* phy_if_list = if_nameindex ();
    if (phy_if_list == NULL)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s(): if_nameindex Error\n", __FUNCTION__);
        return 0;
    }

    unsigned int if_count = 0;
    if_count += getPhysicalInterfaceNumberOfEntries (phy_if_list);
    if_count += getVirtualInterfaceNumberOfEntries (phy_if_list);

    if_freenameindex (phy_if_list);

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: if_count = [%u]\n", __FUNCTION__, if_count);

    return if_count;
}

unsigned int hostIf_IP::getPhysicalInterfaceNumberOfEntries (struct if_nameindex* phy_if_list)
{
    unsigned int phy_if_count = 0;

    for (struct if_nameindex* phy_if = phy_if_list; phy_if->if_index != 0; phy_if++)
    {
        phy_if_count++;
    }

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "%s: phy_if_count = [%u]\n", __FUNCTION__, phy_if_count);

    return phy_if_count;
}

/**
 * Returns the number of virtual interfaces on the system.
 * Requires the list of physical interfaces as input.
 */
unsigned int hostIf_IP::getVirtualInterfaceNumberOfEntries (struct if_nameindex* phy_if_list)
{
    struct ifaddrs *ifa;
    if (getifaddrs (&ifa))
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: getifaddrs returned error\n", __FUNCTION__);
        return 0;
    }

    int virtual_if_count = 0;
    char *p, *v;
    for (struct ifaddrs *ifa_node = ifa; ifa_node; ifa_node = ifa_node->ifa_next)
    {
        if (ifa_node->ifa_addr->sa_family == AF_INET) // virtual interfaces are IPv4-specific, so use IPv4 address family to hunt for them.
        {
            for (struct if_nameindex *phy_if = phy_if_list; phy_if->if_index != 0; phy_if++)
            {
                for (v = ifa_node->ifa_name, p = phy_if->if_name; *v == *p && *p; v++, p++)
                    ;

                if (*v == *p) // ifa_node->ifa_name matches exactly with this physical interface
                    break; // no need to try matching this ifa_node->ifa_name further

                if (*v == ':') // ifa_node->ifa_name could be a virtual interface, so check
                {
                    char *tailPtr;
                    long int value = (int) strtol (++v, &tailPtr, 10);
                    if (*tailPtr == 0 && value >= 0) // string after ':' is an unsigned integer, virtual interface identified!
                    {
                        virtual_if_count++;
                        break; // no need to try matching this ifa_node->ifa_name further
                    }
                }
            }
        }
    }

    freeifaddrs (ifa);

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "%s: virtual_if_count = [%u]\n", __FUNCTION__, virtual_if_count);

    return virtual_if_count;
}

/**
 * Returns the virtual interface name corresponding to a virtual interface index.
 * Requires the list of physical interfaces and the virtualInterfaceIndex as input.
 *  where virtualInterfaceIndex 1 refers to the first virtual interface returned by
 *  the kernel in the output from 'getifaddrs'.
 */
char* hostIf_IP::getVirtualInterfaceName (struct if_nameindex *phy_if_list, unsigned int virtual_if_index, char* virtual_if_name)
{
    char* ret = NULL;

    struct ifaddrs *ifa;
    if (getifaddrs (&ifa))
        return ret;

    int virtual_if_count = 0;
    char *p, *v;
    for (struct ifaddrs *ifa_node = ifa; ifa_node; ifa_node = ifa_node->ifa_next)
    {
        if (ifa_node->ifa_addr->sa_family == AF_INET) // virtual interfaces are IPv4-specific, so use IPv4 address family to hunt for them.
        {
            for (struct if_nameindex *phy_if = phy_if_list; phy_if->if_index != 0; phy_if++)
            {
                for (v = ifa_node->ifa_name, p = phy_if->if_name; *v == *p && *p; v++, p++)
                    ;

                if (*v == *p) // ifa_node->ifa_name matches exactly with this physical interface
                    break; // no need to try matching ifa_node->ifa_name further

                if (*v == ':') // ifa_node->ifa_name could be a virtual interface, so check
                {
                    char *tailPtr;
                    long int value = (int) strtol (++v, &tailPtr, 10);
                    if (*tailPtr == 0 && value >= 0) // string after ':' is an unsigned integer, virtual interface identified!
                    {
                        if (++virtual_if_count == virtual_if_index)
                        {
			    errno_t rc = -1;
                            rc=strcpy_s (virtual_if_name,IF_NAMESIZE, ifa_node->ifa_name);
			    if(rc!=EOK)
		       	    {
				ERR_CHK(rc);
	 		    }
                            ret = virtual_if_name;
                            goto freeResources; // we have found the virtualInterfaceName no need to try any matching ifa_node->ifa_name further
                        }
                    }
                }
            }
        }
    }

freeResources:
    freeifaddrs (ifa);

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s(),%d] virtual_if_index = %u, virtual_if_name = %s\n",
            __FUNCTION__, __LINE__, virtual_if_index, virtual_if_name);

    return ret;
}

/** Description: Counts the number of tcp sockets in the device
 *               in listening or established state.
 *
 * \Return:  Count value or '0' if error
 *
 */
unsigned int hostIf_IP::getNumOfActivePorts(void) {

    FILE *fp = NULL;
    char resultBuff[BUFF_LENGTH] = {'\0'};
    int noOfActivePorts = 0;

    fp = popen(cmd_NumOfActivePorts,"r");

    if(fp == NULL)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Error popen\n", __FUNCTION__);
        return 0;
    }

    if(fgets(resultBuff, BUFF_LENGTH, fp)!=NULL)
    {
        sscanf(resultBuff,"%d",&noOfActivePorts);
    }

    pclose(fp);

    //RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s: Current Active Ports Count: [%u]\n", __FUNCTION__, noOfActivePorts);

    return (noOfActivePorts);
}


int hostIf_IP::get_Device_IP_Fields(EIPMembers ipMem)
{
    FILE *fp = NULL;
    struct if_nameindex *ifname, *ifnp;
    unsigned int ipInterfaceCount = 0;
    char resultBuff[BUFF_LENGTH] = {'\0'};
    char command[BUFF_LENGTH] = {'\0'};
    int ipv4AddressAvailable = 0;
    errno_t rc = -1;


    switch(ipMem)
    {
    case eIpIPv4Capable:
        /*the assumption is that we are always IPv4 capable*/
        /*HARD CODING it to 1*/
        stIPInstance.iPv4Capable = TRUE;
        break;
    case eIpIPv4Enable:
    case eIpIPv4Status:
        sprintf(command,"ifconfig | egrep 'inet addr:' | wc -l");

        fp = popen(command,"r");

        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Error in popen eIPv4Enable\n", __FUNCTION__);

            return NOK;
        }

        if(fgets(resultBuff,BUFF_LENGTH,fp)!=NULL)
        {
            sscanf(resultBuff,"%d",&ipv4AddressAvailable);
        }

        /*If there are no IPV4 address available, then assume that IPv4 has been disabled.*/
        if(0 == ipv4AddressAvailable)
        {
            stIPInstance.iPv4Enable = FALSE;
            rc=strcpy_s(stIPInstance.iPv4Status,sizeof(stIPInstance.iPv4Status),"Disabled");
	    if(rc!=EOK)
    	    { 
		    ERR_CHK(rc);
	    }
            //RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s(): stIPInstance.iPv4Enable = %d,stIPInstance.iPv4Status = %s\n",__FUNCTION__,stIPInstance.iPv4Enable,stIPInstance.iPv4Status);
        }
        else
        {
            stIPInstance.iPv4Enable = TRUE;
            rc=strcpy_s(stIPInstance.iPv4Status,sizeof(stIPInstance.iPv4Status),"Enabled");
	    if(rc!=EOK)
	    {
		    ERR_CHK(rc);
	    }
            //RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s(): stIPInstance.iPv4Enable = %d,stIPInstance.iPv4Status = %s\n",__FUNCTION__,stIPInstance.iPv4Enable,stIPInstance.iPv4Status);
        }

        pclose(fp);
        break;
    case eIpULAPrefix:
        break;
    case eIpInterfaceNumberOfEntries:
        //retrieve the current interfaces
        stIPInstance.interfaceNumberOfEntries = getInterfaceNumberOfEntries();

        //RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s(): InterfaceNumberOfEntries %u \n",__FUNCTION__,stIPInstance.interfaceNumberOfEntries);
        break;

    case eIpActivePortNumberOfEntries:
        stIPInstance.activePortNumberOfEntries = getNumOfActivePorts();
        //RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s(): ActivePortsNumberOfEntries %u \n",__FUNCTION__,stIPInstance.activePortNumberOfEntries);
        break;

    default:
        break;
    }

    return 0;
}

int hostIf_IP::handleGetMsg (HOSTIF_MsgData_t* stMsgData)
{
    int ret = NOT_HANDLED;

    if (!strcasecmp (stMsgData->paramName, "Device.IP.IPv4Capable"))
    {
        ret = get_Device_IP_IPv4Capable (stMsgData);
    }
    else if (!strcasecmp (stMsgData->paramName, "Device.IP.IPv4Enable"))
    {
        ret = get_Device_IP_IPv4Enable (stMsgData);
    }
    else if (!strcasecmp (stMsgData->paramName, "Device.IP.IPv4Status"))
    {
        ret = get_Device_IP_IPv4Status (stMsgData);
    }
    else if (!strcasecmp (stMsgData->paramName, "Device.IP.ULAPrefix"))
    {
        ret = get_Device_IP_ULAPrefix (stMsgData);
    }
    else if (!strcasecmp (stMsgData->paramName, "Device.IP.InterfaceNumberOfEntries"))
    {
        ret = hostIf_IP::get_Device_IP_InterfaceNumberOfEntries (stMsgData);
    }
    else if (!strcasecmp (stMsgData->paramName, "Device.IP.ActivePortNumberOfEntries"))
    {
        ret = hostIf_IP::get_Device_IP_ActivePortNumberOfEntries (stMsgData);
    }
    else
    {
         RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]Device.IP: Parameter \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, stMsgData->paramName);
         stMsgData->faultCode = fcInvalidParameterName;
         ret = NOK;
    }

    return ret;
}

int hostIf_IP::handleSetMsg (HOSTIF_MsgData_t* stMsgData)
{
    int ret = NOT_HANDLED;

    if (!strcasecmp (stMsgData->paramName, "Device.IP.IPv4Enable"))
    {
        ret = set_Device_IP_IPv4Enable (stMsgData);
    }
    else
    {
        stMsgData->faultCode = fcInvalidParameterName;
        ret = NOT_HANDLED;
    }
    return ret;
}

/****************************************************************************************************************************************************/
// Device.IP. Profile. Getters:
/****************************************************************************************************************************************************/

/**
 * @brief This function gets the IPv4 capability of a device. It indicates
 * whether or not the device is IPv4 capable. 'true' if it is capable, 'false' if it
 * is not capable.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully get the capability of IPv4 interface else
 * returns '-1'.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_API
 */
int hostIf_IP::get_Device_IP_IPv4Capable(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    get_Device_IP_Fields(eIpIPv4Capable);
    if(bCalledIPv4Capable && pChanged && (backupIPv4Capable != stIPInstance.iPv4Capable))
    {
        *pChanged = true;
    }
    bCalledIPv4Capable = true;
    backupIPv4Capable = stIPInstance.iPv4Capable;
    put_int(stMsgData->paramValue,stIPInstance.iPv4Capable);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = 1;

    return OK;
}

/**
 * @brief Get the status of the IPv4 stack on a device. This function provides the status
 * 'enabled' or 'disabled' of the IPv4 stack, and the use of IPv4 on the device. This affects
 * only layer 3 and above.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully get the device IPv4 enable else returns '-1'.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_API
 */
int hostIf_IP::get_Device_IP_IPv4Enable(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    get_Device_IP_Fields(eIpIPv4Enable);
    if(bCalledIPv4Enable && pChanged && (backupIPv4Enable != stIPInstance.iPv4Enable))
    {
        *pChanged = true;
    }
    bCalledIPv4Enable = true;
    backupIPv4Enable = stIPInstance.iPv4Enable;
    put_int(stMsgData->paramValue,stIPInstance.iPv4Enable);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen = 1;

    return OK;
}

/**
 * @brief This function gets the status of the IPv4 stack on a device. It indicates the status of the IPv4 stack.
 * It is an enumeration of: Disabled, Enabled, Error (OPTIONAL).
 *
 * @note The Error value MAY be used by the CPE to indicate a locally defined  error condition.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully get the device IPv4 status else returns '-1'.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_API
 */
int hostIf_IP::get_Device_IP_IPv4Status(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    get_Device_IP_Fields(eIpIPv4Status);
    if(bCalledIPv4Status && pChanged && strncmp(stIPInstance.iPv4Status, backupIPv4Status,_BUF_LEN_16 ))
    {
        *pChanged = true;
    }
    bCalledIPv4Status = true;
    strncpy(stMsgData->paramValue,stIPInstance.iPv4Status,TR69HOSTIFMGR_MAX_PARAM_LEN );
    strncpy(backupIPv4Status,stIPInstance.iPv4Status,sizeof(backupIPv4Status) -1);  //CID:136557 - Buffer size warning
    backupIPv4Status[sizeof(backupIPv4Status) -1] = '\0';
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stIPInstance.iPv4Status);

    return OK;
}

/**
 * @brief Get the ULA(Unique Local Address)/48 prefix for a device. This function provides the
 * ULA /48 prefix of the device. This is the IPv6 address prefix and can be any IPv6 prefix that
 * is permitted by the IPPrefix data type. Currently not implemented.
 *
 * @note This is specified as an IP address followed by an appended "/n" suffix, where n
 * (the prefix size) is an integer in the range 0-32 (for IPv4) or 0-128 (for IPv6) that
 * indicates the number of (leftmost) '1' bits of the routing prefix.
 * - IPv4 example: 192.168.1.0/24
 * - IPv6 example: 2001:edff:fe6a:f76::/64
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully get the device IP ULA Prefix else returns '-1'.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_API
 */
int hostIf_IP::get_Device_IP_ULAPrefix(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s()] Parameter Not Supported \n",__FUNCTION__);

    return NOK;
}

int hostIf_IP::get_Device_IP_InterfaceNumberOfEntries(HOSTIF_MsgData_t *stMsgData)
{

    hostIf_IP::get_Device_IP_Fields(eIpInterfaceNumberOfEntries);
    put_int(stMsgData->paramValue,stIPInstance.interfaceNumberOfEntries);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}

int hostIf_IP::get_Device_IP_ActivePortNumberOfEntries(HOSTIF_MsgData_t *stMsgData)
{

    get_Device_IP_Fields(eIpActivePortNumberOfEntries);
    put_int(stMsgData->paramValue,stIPInstance.activePortNumberOfEntries);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}




/****************************************************************************************************************************************************/
// Device.IP. Profile. Setters:
/****************************************************************************************************************************************************/


/**
 * @brief This function sets the status to 'enabled' or 'disabled' of the IPv4 stack
 * on a device. This affects only layer 3 and above.
 *
 * @note     When 'false', IP interfaces that had been operationally up and
 *           passing IPv4 packets will now no longer be able to do so, and will be
 *           operationally down (unless also attached to an enabled IPv6 stack).
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully set the device IPv4 enable else returns '-1'.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_API
 */
int hostIf_IP::set_Device_IP_IPv4Enable(HOSTIF_MsgData_t *stMsgData)
{
    char command[BUFF_LENGTH]= {'\0'};
    errno_t rc = -1;

    if(get_int(stMsgData->paramValue) == 1)
    {
        rc=strcpy_s(command,sizeof(command),"ifup -a");
	if(rc!=EOK)
	{
		ERR_CHK(rc);
	}
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s()] Enabled \n",__FUNCTION__);

    }
    else if(get_int(stMsgData->paramValue) == 0)
    {
        rc=strcpy_s(command,sizeof(command),"ifdown -a");
	if(rc!=EOK)
        {
                ERR_CHK(rc);
        }
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s()] Disabled \n",__FUNCTION__);
    }

    if(system(command) < 0)
        return NOK;

    return OK;
}

/**
 * @brief This function sets the ULA /48 prefix of the device. This is the IPv6 address
 * prefix and can be any IPv6 prefix that is permitted by the IPPrefix data type.
 * Currently not implemented.
 *
 * @note This is specified as an IP address followed by an appended "/n" suffix, where n
 * (the prefix size) is an integer in the range 0-32 (for IPv4) or 0-128 (for IPv6) that
 * indicates the number of (leftmost) '1' bits of the routing prefix.
 * - IPv4 example: 192.168.1.0/24
 * - IPv6 example: 2001:edff:fe6a:f76::/64
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully set the device IP ULA prefix else returns '-1'.
 * @ingroup TR69_HOSTIF_DEVICE_IP_INTERFACE_API
 */
int hostIf_IP::set_Device_IP_ULAPrefix(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s()] Parameter Not Supported \n",__FUNCTION__);

    return NOK;
}


/** @} */
/** @} */
