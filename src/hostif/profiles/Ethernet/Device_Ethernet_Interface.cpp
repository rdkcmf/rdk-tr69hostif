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
 * @file Device_Ethernet_Interface.cpp
 * @brief This source file contains the APIs of device ethernet interface.
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


#include <net/if.h>

#include "Device_Ethernet_Interface.h"

EthernetInterface hostIf_EthernetInterface::stEthInterface = {0,};

GMutex* hostIf_EthernetInterface::m_mutex = NULL;

GHashTable *hostIf_EthernetInterface::ifHash = NULL;

GHashTable* hostIf_EthernetInterface::m_notifyHash = NULL;

/** Description: Counts the number of Ethernet
 *               interfaces present in the device.
 *
 *  Get all the current interfaces in the system and
 *  count "eth" interface from the list.
 *
 * \Return:  Count value or '0' if error
 *
 */

hostIf_EthernetInterface* hostIf_EthernetInterface::getInstance(int dev_id)
{
    hostIf_EthernetInterface* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_EthernetInterface *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_EthernetInterface(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create Ethernet Interface instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_EthernetInterface::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_EthernetInterface::closeInstance(hostIf_EthernetInterface *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_EthernetInterface::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_EthernetInterface* pDev = (hostIf_EthernetInterface *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

void hostIf_EthernetInterface::getLock()
{

    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);

}

void hostIf_EthernetInterface::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

GHashTable* hostIf_EthernetInterface::getNotifyHash()
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

/**
 * @brief Class Constructor of the class hostIf_EthernetInterface.
 *
 * It will initialize the device id.
 *
 * @param[in] devid Device identification Number.
 */
hostIf_EthernetInterface::hostIf_EthernetInterface(int dev_id):
    dev_id(dev_id),
    backupEnable(false),
    backupUpstream(false),
    backupMaxBitRate(0),
    bCalledEnable(false),
    bCalledStatus(false),
    bCalledName(false),
    bCalledUpstream(false),
    bCalledMACAddress(false),
    bCalledMaxBitRate(false),
    bCalledDuplexMode(false)
{
    strcpy(backupStatus,"");
    strcpy(backupName,"");
    strcpy(backupMACAddress,"");
    strcpy(backupDuplexMode,"");

}

hostIf_EthernetInterface::~hostIf_EthernetInterface()
{
    if(m_notifyHash)
    {
        g_hash_table_destroy(m_notifyHash);
    }
}

static int getEthernetInterfaceName (unsigned int ethInterfaceNum, char* name)
{
    int ret = NOK;

    strcpy (name, "");

    //retrieve the current interfaces
    struct if_nameindex* ifname = if_nameindex ();

    if (ifname != NULL)
    {
        unsigned int count = 0;
        for (struct if_nameindex* ifnp = ifname; ifnp->if_index != 0; ifnp++)
        {
            if ((strncmp (ifnp->if_name, "eth", 3) == 0) && (++count == ethInterfaceNum))
            {
                strcpy (name, ifnp->if_name);
                ret = OK;
                break;
            }
        }

        if_freenameindex (ifname); /* free the dynamic memory */

        if (ret != OK)
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s(): Cannot find Ethernet Interface Name for Ethernet Interface Number '%u'\n",
                    __FUNCTION__, ethInterfaceNum);
        }
    }
    else
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s(): if_nameindex Error\n", __FUNCTION__);
    }

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s(): Ethernet Interface (number,name) = (%u,%s)\n", __FUNCTION__, ethInterfaceNum, name);

    return ret;
}

static int get_Device_Ethernet_Interface_Fields(unsigned int ethInterfaceNum,EEthInterfaceMembers ethInterfaceMem)
{
    FILE *fp;
    char resultBuff[BUFF_LENGTH] = {'\0'};
    char cmd[BUFF_LENGTH] = {'\0'};
    int temp = 0;
    char ethernetInterfaceName[BUFF_LENGTH_64];

    if(!ethInterfaceNum)
        return 0;

    switch(ethInterfaceMem)
    {
    case eEnable:
        hostIf_EthernetInterface::stEthInterface.enable = FALSE;

        if (OK != getEthernetInterfaceName (ethInterfaceNum, ethernetInterfaceName))
            return 0;

        sprintf(cmd,"cat /sys/class/net/%s/carrier", ethernetInterfaceName);

        fp = popen(cmd,"r");

        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Error in popen eEnable\n", __FUNCTION__);

            return 0;
        }

        if(fgets(resultBuff,1024,fp)!=NULL)
        {
            sscanf(resultBuff,"%d",&hostIf_EthernetInterface::stEthInterface.enable);
        }
        pclose(fp);

        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s(): Interface %u Enable: %d \n",
                __FUNCTION__, ethInterfaceNum, hostIf_EthernetInterface::stEthInterface.enable);

        break;

    case eStatus:
        memset(hostIf_EthernetInterface::stEthInterface.status,'\0',S_LENGTH);

        if (OK != getEthernetInterfaceName (ethInterfaceNum, ethernetInterfaceName))
            return 0;

        sprintf(cmd,"cat /sys/class/net/%s/carrier", ethernetInterfaceName);

        fp = popen(cmd,"r");

        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Error in popen eStatus\n",__FUNCTION__);

            return 0;
        }

        if(fgets(resultBuff,1024,fp)!=NULL)
        {
            sscanf(resultBuff,"%d",&temp);
        }

        strcpy (hostIf_EthernetInterface::stEthInterface.status, (TRUE == temp) ? "Up" : "Down");

        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s(): Interface %u Status: %s \n",
                __FUNCTION__, ethInterfaceNum, hostIf_EthernetInterface::stEthInterface.status);

        pclose(fp);
        break;
    case eName:
    {
        if (OK != getEthernetInterfaceName (ethInterfaceNum, hostIf_EthernetInterface::stEthInterface.name))
            return 0;

        break;
    }
    case eLastChange:
        break;
    case eLowerLayers:
        break;
    case eUpstream:
        hostIf_EthernetInterface::stEthInterface.upStream = FALSE;

        if (OK != getEthernetInterfaceName (ethInterfaceNum, ethernetInterfaceName))
            return 0;

        sprintf(cmd,"cat /sys/class/net/%s/carrier", ethernetInterfaceName);

        fp = popen(cmd,"r");

        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Error in popen eUpStream\n", __FUNCTION__);
            return 0;
        }
        if(fgets(resultBuff,1024,fp)!=NULL)
        {
            sscanf(resultBuff,"%d",&hostIf_EthernetInterface::stEthInterface.upStream );
        }
        pclose(fp);

        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s(): Interface %u UpStream: %d \n",
                __FUNCTION__, ethInterfaceNum, hostIf_EthernetInterface::stEthInterface.upStream);

        break;

    case eMACAddress:
        memset(hostIf_EthernetInterface::stEthInterface.mACAddress, '\0', S_LENGTH);

        if (OK != getEthernetInterfaceName (ethInterfaceNum, ethernetInterfaceName))
            return 0;

        sprintf(cmd,"cat /sys/class/net/%s/address", ethernetInterfaceName);

        fp = popen(cmd,"r");

        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Error in popen eMACAddress\n", __FUNCTION__);

            return 0;
        }

        if(fgets(resultBuff,1024,fp)!=NULL)
        {
            sscanf(resultBuff,"%s",hostIf_EthernetInterface::stEthInterface.mACAddress);
        }

        pclose(fp);

        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s(): Interface %u MACAddress: %s \n",
                __FUNCTION__, ethInterfaceNum, hostIf_EthernetInterface::stEthInterface.mACAddress);

        break;
    case eMaxBitRate:
        hostIf_EthernetInterface::stEthInterface.maxBitRate = 0;

        if (OK != getEthernetInterfaceName (ethInterfaceNum, ethernetInterfaceName))
            return 0;

        sprintf(cmd,"cat /sys/class/net/%s/speed", ethernetInterfaceName);

        fp = popen(cmd,"r");

        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Error in popen eMaxBitRate\n", __FUNCTION__);

            return 0;
        }

        if(fgets(resultBuff,1024,fp)!=NULL)
        {
            sscanf(resultBuff,"%d",&hostIf_EthernetInterface::stEthInterface.maxBitRate);
        }

        pclose(fp);

        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s(): Interface %u MaxBitRate: %d\n",
                __FUNCTION__, ethInterfaceNum, hostIf_EthernetInterface::stEthInterface.maxBitRate);
        break;
    case eDuplexMode:
        memset(hostIf_EthernetInterface::stEthInterface.duplexMode, '\0',S_LENGTH);

        if (OK != getEthernetInterfaceName (ethInterfaceNum, ethernetInterfaceName))
            return 0;

        sprintf(cmd,"cat /sys/class/net/%s/duplex", ethernetInterfaceName);

        fp = popen(cmd,"r");

        if(fp == NULL)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): Error in popen eDuplex\n", __FUNCTION__);

            return 0;
        }

        if(fgets(resultBuff,1024,fp)!=NULL)
        {
            sscanf(resultBuff,"%s",hostIf_EthernetInterface::stEthInterface.duplexMode);
        }

        pclose(fp);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s(): Interface %u DuplexMode: %s\n",
                __FUNCTION__, ethInterfaceNum, hostIf_EthernetInterface::stEthInterface.duplexMode);

        break;
    default:
        break;

    }

    return 0;
}


/****************************************************************************************************************************************************/
// Device.Ethernet.Interface. Profile. Getters:
/****************************************************************************************************************************************************/
int hostIf_EthernetInterface::get_Device_Ethernet_InterfaceNumberOfEntries(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    //retrieve the current interfaces
    struct if_nameindex* ifname = if_nameindex ();

    if (ifname == NULL)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s(): if_nameindex Error\n", __FUNCTION__);
        return NOK;
    }

    int noOfEthInterfaces = 0;
    for (struct if_nameindex* ifnp = ifname; ifnp->if_index != 0; ifnp++)
    {
        if (strncmp (ifnp->if_name, "eth", 3) == 0)
        {
            noOfEthInterfaces++;
        }
    }

    if_freenameindex (ifname); /* free the dynamic memory */

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s(): Current Ethernet Interfaces Count: [%d] \n", __FUNCTION__, noOfEthInterfaces);

    stMsgData->paramtype=hostIf_IntegerType;
    stMsgData->paramLen=4;
    put_int(stMsgData->paramValue,noOfEthInterfaces);

    return OK;
}

/**
 * @brief Get the status of an Ethernet interface is 'Enabled' or 'Disabled'.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_API
 */
int hostIf_EthernetInterface::get_Device_Ethernet_Interface_Enable(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    get_Device_Ethernet_Interface_Fields(dev_id, eEnable);

    if(bCalledEnable && pChanged && (backupEnable != stEthInterface.enable))
    {
        *pChanged = true;
    }
    bCalledEnable = true;
    backupEnable = stEthInterface.enable;
    put_boolean(stMsgData->paramValue,stEthInterface.enable);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=1;

    return OK;
}


/**
 * @brief This function gets the current operational state of the 'Ethernet' interface.
 * Such as Up, Down, Unknown, Dormant, NotPresent, LowerLayerDown and Error (OPTIONAL).
 *
 * @note
 *  - When Enable is "false" then status SHOULD normally be 'Down' or 'NotPresent' or
 * 'Error' if there is a fault condition on the interface.
 *
 * Code             | Description
 * -----------------|------------
 * 'Error'          | if there is an error or other fault condition detected on the interface.
 *                  |
 * 'NotPresent'     | if the interface is missing i.e the hardware component is not present.
 *                  |
 * 'Unknown'        | if the state of the interface can not be determined for some reason.
 *
 *  - When enable is changed to "true" then status should change to
 * Code             | Description
 * -----------------|------------
 * 'Up'             | if and only if the interface is able to transmit and receive network traffic
 *                  |
 * 'Dormant'        | if and only if the interface is operable but is waiting for external actions before
 *                  | it can transmit and receive network traffic and subsequently change to 'Up' if still operable
 *                  | when the expected actions have completed.
 *                  |
 * 'LowerLayerDown' | if and only if the interface is prevented from entering the 'Up' state because one or more
 *                  | of the interfaces beneath it is down.
 *
 *  These parameter is based on ifOperStatus from RFC 2863, the Interfaces Group MIB, IETF, 2000.
 *
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_API
 */
int hostIf_EthernetInterface::get_Device_Ethernet_Interface_Status(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    get_Device_Ethernet_Interface_Fields(dev_id, eStatus);

    if(bCalledStatus && pChanged && strncmp(stEthInterface.status,backupStatus,_BUF_LEN_16-1))
    {
        *pChanged = true;
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] Ethernet Interface Status Changed..", __FUNCTION__, __FILE__, __LINE__);
    }
    bCalledStatus = true;
    strcpy(backupStatus, stEthInterface.status);
    strncpy(stMsgData->paramValue,stEthInterface.status,_BUF_LEN_16-1 );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stEthInterface.status);
    return OK;
}

/**
 * @brief Get the textual name of the 'Ethernet' interface.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_API
 */
int hostIf_EthernetInterface::get_Device_Ethernet_Interface_Name(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    get_Device_Ethernet_Interface_Fields(dev_id, eName);

    if(bCalledName && pChanged && strncmp(stEthInterface.name,backupName,_BUF_LEN_16-1))
    {
        *pChanged = true;
    }
    bCalledName = true;
    strcpy(backupName, stEthInterface.name);
    strncpy(stMsgData->paramValue,stEthInterface.name,_BUF_LEN_16-1 );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stEthInterface.name);

    return OK;
}

/**
 * @brief This function provides the accumulated time in seconds since this Ethernet
 * interface entered its current operational state. Currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns enum integer '-1' on method completion.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_API
 */
int hostIf_EthernetInterface::get_Device_Ethernet_Interface_LastChange(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    /*Retrieving value */
    // Implementation for SOC vendor
    // value->out_cval =
    return NOK;
}

/**
 * @brief  This function provides a comma-separated list (maximum length 1024) of strings.  Each
 * list item MUST be the path name of an interface object that is stacked immediately
 * below this Ethernet interface object. Currently not implemented.
 *
 * @note  If the referenced object is deleted, the corresponding item MUST be removed from the list.
 * Since Interface is a layer 1 interface, it is expected that 'LowerLayers' will not be used.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns enum integer '-1' on method completion.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_API
 */
int hostIf_EthernetInterface::get_Device_Ethernet_Interface_LowerLayers(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    /*Retrieving value */
    // Implementation for SOC vendor
    // value->out_cval =
    return NOK;
}

/**
 * @brief This function indicates whether the interface points towards the Internet represent by 'true'
 * or towards End Devices represent by false
 * For example:
 * - For an Internet Gateway Device, Upstream will be "true" for all WAN interfaces and "false"
 * for all LAN interfaces.
 * - For a standalone WiFi Access Point that is connected via Ethernet to an Internet Gateway
 * Device, Upstream will be "true" for the Ethernet interface and "false" for the WiFi Radio interface.
 * - For an End Device, Upstream will be "true" for all interfaces.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if is successfully fetch the data from the device.
 * @retval ERR_INTERNAL_ERROR if not able to fetch the data.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_API
 *
 */
int hostIf_EthernetInterface::get_Device_Ethernet_Interface_Upstream(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    get_Device_Ethernet_Interface_Fields(dev_id, eUpstream);

    if(bCalledUpstream && pChanged && (backupUpstream != stEthInterface.upStream))
    {
        *pChanged = true;
    }
    bCalledUpstream = true;
    backupUpstream = stEthInterface.upStream;
    put_boolean(stMsgData->paramValue,stEthInterface.upStream);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=1;

    return OK;
}

/**
 * @brief This function provides the MAC Address of this Ethernet interface.
 *
 * @note    This is not necessarily the same as the Ethernet header source or
 *          destination MAC address, which is associated with the IP interface and is
 *          modelled via the Ethernet.Link.{i}.MACAddress parameter.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if is successfully get the MAC address.
 * @retval ERR_INTERNAL_ERROR if not able to get the MAC address.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_API
 */
int hostIf_EthernetInterface::get_Device_Ethernet_Interface_MACAddress(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    get_Device_Ethernet_Interface_Fields(dev_id, eMACAddress);

    if(bCalledMACAddress && pChanged && strncmp(stEthInterface.mACAddress,backupMACAddress,S_LENGTH-1))
    {
        *pChanged = true;
    }
    bCalledMACAddress = true;
    strcpy(backupMACAddress, stEthInterface.mACAddress);
    strncpy(stMsgData->paramValue,stEthInterface.mACAddress,S_LENGTH );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stEthInterface.mACAddress);

    return OK;
}

/**
 * @brief This function provides the maximum upstream and downstream PHY bit rate supported
 * by this Ethernet interface it expressed in Mbps.
 *
 * @note  A value of -1 indicates automatic selection of the maximum bit rate.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if is successfully get the Max BitRate.
 * @retval ERR_INTERNAL_ERROR if not able to get the Max BitRate.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_API
 */
int hostIf_EthernetInterface::get_Device_Ethernet_Interface_MaxBitRate(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    get_Device_Ethernet_Interface_Fields(dev_id, eMaxBitRate);

    stMsgData->paramtype=hostIf_IntegerType;
    stMsgData->paramLen=4;
    if(bCalledMaxBitRate && pChanged && (backupMaxBitRate != stEthInterface.maxBitRate))
    {
        *pChanged = true;
    }
    bCalledMaxBitRate = true;
    backupMaxBitRate = stEthInterface.maxBitRate;
    put_int(stMsgData->paramValue,stEthInterface.maxBitRate);

    return OK;
}

/**
 * @brief  This function provides a string indicating the duplex mode available to this
 * Ethernet connection. This contains the enumeration such as Half, Full, Auto.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if is successfully get the Duplex mode status of ethernet interface.
 * @retval ERR_INTERNAL_ERROR if not able to get the Duplex mode status of ethernet interface.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_API
 */
int hostIf_EthernetInterface::get_Device_Ethernet_Interface_DuplexMode(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    get_Device_Ethernet_Interface_Fields(dev_id, eDuplexMode);

    if(bCalledDuplexMode && pChanged && strncmp(stEthInterface.duplexMode,backupDuplexMode,_BUF_LEN_16-1))
    {
        *pChanged = true;
    }
    bCalledDuplexMode = true;
    strcpy(backupDuplexMode, stEthInterface.duplexMode);
    strncpy(stMsgData->paramValue,stEthInterface.duplexMode,_BUF_LEN_16-1 );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stEthInterface.duplexMode);

    return OK;
}
/****************************************************************************************************************************************************/
// Device.Ethernet.Interface. Profile. Setters:
/****************************************************************************************************************************************************/

/**
 * @brief This function sets the status of 'Ethernet' interface as enabled or disabled.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns 'true' if the Ethernet interface is enabled successfully else 'false'.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_API
 */
int hostIf_EthernetInterface::set_Device_Ethernet_Interface_Enable(HOSTIF_MsgData_t* stMsgData)
{
    unsigned int ethInterfNo = 0;
    char cmd[64]= {'\0'};
    bool value =  get_boolean(stMsgData->paramValue);

    if(FALSE == value)
    {
        sprintf(cmd,"ifconfig eth%d down",ethInterfNo-1);
    }
    else
    {
        sprintf(cmd,"ifconfig eth%d up",ethInterfNo-1);
    }

    if(system(cmd)< 0)
        return NOK;
    hostIf_EthernetInterface::stEthInterface.enable = value;

    return OK;
}

/**
 * @brief  This function sets a non-volatile handle used to reference this Ethernet
 * interface instance. Alias provides a mechanism for an ACS to label this instance
 * for future reference. This function is currently not implemented.
 * @note  If the CPE supports the Alias-based Addressing feature as defined in
 * [Section 3.6.1/TR-069 Amendment 4] and described in [Appendix II/TR-069
 * Amendment 4], the following mandatory constraints MUST be enforced:
 * - Its value MUST NOT be empty.
 * - Its value MUST start with a letter.
 * - If its instance object is created by the CPE, the initial
 *   value MUST start with a "cpe-" prefix.
 * - The CPE MUST NOT change the parameter value.
 *
 * @param[out] TR-069 Host interface message request.
 *
 * @return Returns enum integer '-1' on method completion.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_API
 */
int hostIf_EthernetInterface::set_Device_Ethernet_Interface_Alias(HOSTIF_MsgData_t* stMsgData)
{
    return NOK;
}

/**
 * @brief This function sets the 'Ethernet' interface 'LowerLayers'. Given a comma-separated list
 * (maximum length 1024) of strings, each list item being the path name of an 'Ethernet' interface
 * object, this function MUST stack each item in the list immediately below this interface object.
 * Currently not implemented.
 *
 * @note If the referenced object is deleted, the corresponding item MUST be removed from the list.
 * Since Interface is a layer 1 interface, it is expected that 'LowerLayers' will not be used.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns enum integer '-1' on method completion.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_API
 */
int hostIf_EthernetInterface::set_Device_Ethernet_Interface_LowerLayers(HOSTIF_MsgData_t* stMsgData)
{
    /*Retrieving value */
    // Implementation for SOC vendor
    // value->out_cval =

    return NOK;
}


/**
 * @brief This function sets the maximum bit rate attainable on an 'Ethernet' Interface.
 * This function sets the maximum upstream and downstream PHY bit rate supported
 * by this 'Ethernet' interface (expressed in Mbps). This function is currently not implemented.
 *
 * @note  A value of -1 indicates automatic selection of the maximum bit rate.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns enum integer '-1' on method completion.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_API
 */
int hostIf_EthernetInterface::set_Device_Ethernet_Interface_MaxBitRate(HOSTIF_MsgData_t* stMsgData)
{

    /*Retrieving value */
    // Implementation for SOC vendor
    // value->out_cval =
    return NOK;
}

/**
 * @brief This function sets the 'Duplex Mode' available on an ethernet Interface.
 * This function sets the string indicating the duplex mode available to this
 * Ethernet connection. This is an enumeration having values Half, Full, Auto.
 * This function is currently not implemented.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns enum integer '-1' on method completion.
 * @ingroup TR69_HOSTIF_ETHERNET_INTERFACE_DEVICE_API
 */
int hostIf_EthernetInterface::set_Device_Ethernet_Interface_DuplexMode(HOSTIF_MsgData_t* stMsgData)
{
    /*Retrieving value */
    // Implementation for SOC vendor
    // value->out_cval =

    return NOK;
}


/** @} */
/** @} */
