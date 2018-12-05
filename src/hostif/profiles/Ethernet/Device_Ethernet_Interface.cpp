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
 * @file Device_Ethernet_Interface.c
 *
 * @brief Device.Ethernet.Interface. API Implementation.
 *
 * This is the implementation of the Device.Ethernet.Interface. API.
 *
 * @par Document
 * TBD Relevant design or API documentation.
 *
 */

/** @addtogroup Device.Ethernet.Interface. Implementation
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

int hostIf_EthernetInterface::get_Device_Ethernet_Interface_Status(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    get_Device_Ethernet_Interface_Fields(dev_id, eStatus);

    if(bCalledStatus && pChanged && strncmp(stEthInterface.status,backupStatus,TR69HOSTIFMGR_MAX_PARAM_LEN))
    {
        *pChanged = true;
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s:%d] Ethernet Interface Status Changed..", __FUNCTION__, __FILE__, __LINE__);
    }
    bCalledStatus = true;
    strcpy(backupStatus, stEthInterface.status);
    strncpy(stMsgData->paramValue,stEthInterface.status,TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stEthInterface.status);
    return OK;
}

int hostIf_EthernetInterface::get_Device_Ethernet_Interface_Name(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    get_Device_Ethernet_Interface_Fields(dev_id, eName);

    if(bCalledName && pChanged && strncmp(stEthInterface.name,backupName,TR69HOSTIFMGR_MAX_PARAM_LEN))
    {
        *pChanged = true;
    }
    bCalledName = true;
    strcpy(backupName, stEthInterface.name);
    strncpy(stMsgData->paramValue,stEthInterface.name,TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stEthInterface.name);

    return OK;
}

int hostIf_EthernetInterface::get_Device_Ethernet_Interface_LastChange(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    /*Retrieving value */
    // Implementation for SOC vendor
    // value->out_cval =
    return NOK;
}

int hostIf_EthernetInterface::get_Device_Ethernet_Interface_LowerLayers(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    /*Retrieving value */
    // Implementation for SOC vendor
    // value->out_cval =
    return NOK;
}

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

int hostIf_EthernetInterface::get_Device_Ethernet_Interface_MACAddress(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    get_Device_Ethernet_Interface_Fields(dev_id, eMACAddress);

    if(bCalledMACAddress && pChanged && strncmp(stEthInterface.mACAddress,backupMACAddress,TR69HOSTIFMGR_MAX_PARAM_LEN))
    {
        *pChanged = true;
    }
    bCalledMACAddress = true;
    strcpy(backupMACAddress, stEthInterface.mACAddress);
    strncpy(stMsgData->paramValue,stEthInterface.mACAddress,TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stEthInterface.mACAddress);

    return OK;
}

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

int hostIf_EthernetInterface::get_Device_Ethernet_Interface_DuplexMode(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    get_Device_Ethernet_Interface_Fields(dev_id, eDuplexMode);

    if(bCalledDuplexMode && pChanged && strncmp(stEthInterface.duplexMode,backupDuplexMode,TR69HOSTIFMGR_MAX_PARAM_LEN))
    {
        *pChanged = true;
    }
    bCalledDuplexMode = true;
    strcpy(backupDuplexMode, stEthInterface.duplexMode);
    strncpy(stMsgData->paramValue,stEthInterface.duplexMode,TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stEthInterface.duplexMode);

    return OK;
}
/****************************************************************************************************************************************************/
// Device.Ethernet.Interface. Profile. Setters:
/****************************************************************************************************************************************************/
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
int hostIf_EthernetInterface::set_Device_Ethernet_Interface_Alias(HOSTIF_MsgData_t* stMsgData)
{
    return NOK;
}

int hostIf_EthernetInterface::set_Device_Ethernet_Interface_LowerLayers(HOSTIF_MsgData_t* stMsgData)
{
    /*Retrieving value */
    // Implementation for SOC vendor
    // value->out_cval =

    return NOK;
}
int hostIf_EthernetInterface::set_Device_Ethernet_Interface_MaxBitRate(HOSTIF_MsgData_t* stMsgData)
{

    /*Retrieving value */
    // Implementation for SOC vendor
    // value->out_cval =
    return NOK;
}
int hostIf_EthernetInterface::set_Device_Ethernet_Interface_DuplexMode(HOSTIF_MsgData_t* stMsgData)
{
    /*Retrieving value */
    // Implementation for SOC vendor
    // value->out_cval =

    return NOK;
}


/** @} */
/** @} */
