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
 * @file Device_IP_ActivePort.c
 *
 * @brief Device.IP.ActivePort. API Implementation.
 *
 * This is the implementation of the Device.IP.ActivePort. API.
 *
 * @par Document
 * TBD Relevant design or API documentation.
 *
 */

/** @addtogroup Device.IP.ActivePort. Implementation
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
#include <arpa/inet.h>
#include "Device_IP_ActivePort.h"

#define         _LENGTH_PARAMETER               64
#define         _LENGTH_IPADDR                   16
#define         _LENGTH_STATUS                  12
#define         _HEX_STATE_LEN                  3
#define         _BUF_LEN            1024

GHashTable *hostIf_IPActivePort::ifHash = NULL;

GMutex *hostIf_IPActivePort::m_mutex = NULL;

struct Device_IP_ActivePort hostIf_IPActivePort::activePort = {0,};

int hostIf_IPActivePort::getActivePortsFields(unsigned int activePortNo)
{
    FILE *fp = NULL;
    char resultBuff[_BUF_LEN]= {'\0'};
    const char *cmd = "cat /proc/net/tcp | awk '$4 == \"0A\" || $4 == \"01\" {print $2\" \"$3\" \"$4}'";
    char local_address[_LENGTH_IPADDR]= {'\0'}, rem_address[_LENGTH_IPADDR]= {'\0'}, state[_HEX_STATE_LEN] = {'\0'};
    int activePortCount = 1, ret = -1;
    unsigned long ipAddr = 0;
    char *addrPtr = NULL;

    fp = popen(cmd,"r");
    if(fp == NULL) {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error popen");
        return 0;
    }

    // local_ip:port remote_ip:port st
    while(fgets(resultBuff, _BUF_LEN, fp) != NULL)
    {
        if(activePortNo == activePortCount) {
            sscanf(resultBuff,"%s %s %s", local_address, rem_address, state);
            ret = OK;
            break;
        }
        activePortCount++;
    }

    pclose(fp);

    memset(activePort.localIpAddress, '\0', _LENGTH_IPADDR);
    memset(activePort.remoteIpAddress, '\0', _LENGTH_IPADDR);
    memset(activePort.status, '\0', _LENGTH_STATUS);
    activePort.localPort = 0;
    activePort.remotePort = 0;

    if(ret != OK)
        return ret;

    // STATUS
    /* st = 0A = LISTEN
       st = 01 = ESTABLISHED */
    if (strtoul(state, NULL, 16) == 10)
        strncpy(activePort.status, "LISTEN", strlen("LISTEN"));
    else if (strtoul(state, NULL, 16) == 1)
        strncpy(activePort.status, "ESTABLISHED", strlen("ESTABLISHED"));

    // LOCAL IP ADDRESS and LOCAL PORT
    addrPtr = local_address;

    ipAddr = strtoul(strsep(&addrPtr, ":"), NULL, 16);
    inet_ntop(AF_INET, &ipAddr, activePort.localIpAddress, INET_ADDRSTRLEN);
    activePort.localPort = strtoul(addrPtr, NULL, 16);

    // REMOTE IP ADDRESS and REMOTE PORT
    addrPtr = rem_address;

    ipAddr = strtoul(strsep(&addrPtr, ":"), NULL, 16);
    inet_ntop(AF_INET, &ipAddr, activePort.remoteIpAddress, INET_ADDRSTRLEN);
    activePort.remotePort = strtoul(addrPtr, NULL, 16);

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"ActivePort: %d local addr: %s:%d remote addr: %s:%d Status: %s\n", activePortNo, activePort.localIpAddress,
             activePort.localPort, activePort.remoteIpAddress, activePort.remotePort, activePort.status);

    return ret;
}

hostIf_IPActivePort::hostIf_IPActivePort(int dev_id):
    dev_id(dev_id),
    bCalledLocalIPAddress(false),
    bCalledLocalPort(false),
    bCalledRemoteIPAddress(false),
    bCalledRemotePort(false),
    bCalledStatus(false),
    backupLocalPort(0),
    backupRemotePort(0)
{
    strcpy(backupLocalIPAddress,"");
    strcpy(backupRemoteIPAddress,"");
    strcpy(backupStatus,"");

}

hostIf_IPActivePort* hostIf_IPActivePort::getInstance(int dev_id)
{
    hostIf_IPActivePort* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_IPActivePort *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_IPActivePort(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create MoCA Interface instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_IPActivePort::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_IPActivePort::closeInstance(hostIf_IPActivePort *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_IPActivePort::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_IPActivePort* pDev = (hostIf_IPActivePort *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

void hostIf_IPActivePort::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void hostIf_IPActivePort::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

int hostIf_IPActivePort::handleGetMsg (const char* pSetting, HOSTIF_MsgData_t* stMsgData)
{
    int ret = NOT_HANDLED;

    if (!strcasecmp (pSetting, "LocalIPAddress"))
    {
        ret = get_Device_IP_ActivePort_LocalIPAddress (stMsgData);
    }
    else if (!strcasecmp (pSetting, "LocalPort"))
    {
        ret = get_Device_IP_ActivePort_LocalPort (stMsgData);
    }
    else if (!strcasecmp (pSetting, "RemoteIPAddress"))
    {
        ret = get_Device_IP_ActivePort_RemoteIPAddress (stMsgData);
    }
    else if (!strcasecmp (pSetting, "RemotePort"))
    {
        ret = get_Device_IP_ActivePort_RemotePort (stMsgData);
    }
    else if (!strcasecmp (pSetting, "Status"))
    {
        ret = get_Device_IP_ActivePort_Status (stMsgData);
    }

    return ret;
}

/****************************************************************************************************************************************************/
// Device.IP.ActivePort. Profile. Getters:
/****************************************************************************************************************************************************/

/*
 * Parameter Name: Device.IP.ActivePort.
*/
int hostIf_IPActivePort::get_Device_IP_ActivePort_LocalIPAddress(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    /*Retrieving value */
    getActivePortsFields(dev_id);

    if(bCalledLocalIPAddress && pChanged && strncmp(activePort.localIpAddress,backupLocalIPAddress,TR69HOSTIFMGR_MAX_PARAM_LEN ))
    {
        *pChanged =  true;
    }
    bCalledLocalIPAddress = true;
    strncpy(backupLocalIPAddress,activePort.localIpAddress,TR69HOSTIFMGR_MAX_PARAM_LEN );
    strncpy(stMsgData->paramValue,activePort.localIpAddress,TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen( activePort.localIpAddress);

    return OK;
}

int hostIf_IPActivePort::get_Device_IP_ActivePort_LocalPort(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    /*Retrieving value */
    getActivePortsFields(dev_id);
    if(bCalledLocalPort && pChanged && (backupLocalPort != activePort.localPort))
    {
        *pChanged = true;
    }
    bCalledLocalPort = true;
    backupLocalPort = activePort.localPort;
    put_int(stMsgData->paramValue,activePort.localPort);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    return OK;
}
int hostIf_IPActivePort::get_Device_IP_ActivePort_RemoteIPAddress(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    /*Retrieving value */
    getActivePortsFields(dev_id);
    if(bCalledRemoteIPAddress && pChanged && strncmp(activePort.remoteIpAddress,backupRemoteIPAddress,TR69HOSTIFMGR_MAX_PARAM_LEN ))
    {
        *pChanged =  true;
    }
    bCalledRemoteIPAddress = true;
    strncpy(backupRemoteIPAddress,activePort.remoteIpAddress,TR69HOSTIFMGR_MAX_PARAM_LEN );
    strncpy(stMsgData->paramValue,activePort.remoteIpAddress,TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen( activePort.remoteIpAddress);
    return OK;
}
int hostIf_IPActivePort::get_Device_IP_ActivePort_RemotePort(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{

    /*Retrieving value */
    getActivePortsFields(dev_id);
    if(bCalledRemotePort && pChanged && (backupRemotePort != activePort.remotePort))
    {
        *pChanged = true;
    }
    bCalledRemotePort = true;
    backupRemotePort = activePort.remotePort;
    put_int(stMsgData->paramValue,activePort.remotePort);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;
    return OK;
}
int hostIf_IPActivePort::get_Device_IP_ActivePort_Status(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    /*Retrieving value */
    getActivePortsFields(dev_id);
    if(bCalledStatus && pChanged && strncmp(activePort.status,backupStatus,TR69HOSTIFMGR_MAX_PARAM_LEN ))
    {
        *pChanged =  true;
    }
    bCalledStatus = true;
    strncpy(backupStatus,activePort.status,TR69HOSTIFMGR_MAX_PARAM_LEN );
    strncpy(stMsgData->paramValue,activePort.status,TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen( activePort.status);
    return OK;
}
/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */


/** @} */
/** @} */
