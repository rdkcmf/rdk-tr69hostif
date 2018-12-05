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
#include <net/if.h>
#include <string.h>
#include "Device_DHCPv4_Client.h"
#include "Device_IP_Interface.h"
#include "Device_IP.h"

#define MAX_CMD_LEN 128
#define MAX_BUF_LEN 256
#define MAX_IP_LEN 16

DHCPv4Client hostIf_DHCPv4Client::dhcpClient = {{'\0'}, {'\0'}, {'\0'}};
GHashTable *hostIf_DHCPv4Client::dhcpv4ClientHash = NULL;
GMutex *hostIf_DHCPv4Client::m_mutex = NULL;
GHashTable* hostIf_DHCPv4Client::m_notifyHash = NULL;
/* Constructor for hostIf_DHCPv4Client*/
hostIf_DHCPv4Client::hostIf_DHCPv4Client(int dev_id):dev_id(dev_id)
{
    FILE* cmdOP;
    int len;
    char buffer[MAX_BUF_LEN]={'\0'};
    bBackUpFlags.interface=0;
    bBackUpFlags.dnsservers=0;
    bBackUpFlags.ipRouters=0;
    memset(&backupDhcpClient, '\0',  sizeof(backupDhcpClient));
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Inside constructor for dev_id:%d\n", dev_id);
}

void hostIf_DHCPv4Client::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void hostIf_DHCPv4Client::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

GHashTable* hostIf_DHCPv4Client::getNotifyHash()
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

hostIf_DHCPv4Client* hostIf_DHCPv4Client::getInstance(int dev_id)
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Entering [%s]\n", __FUNCTION__);
    hostIf_DHCPv4Client* pRet = NULL;
    if(dhcpv4ClientHash)
    {
        pRet = (hostIf_DHCPv4Client *)g_hash_table_lookup(dhcpv4ClientHash, (gpointer) dev_id);
    }
    else
    {

        dhcpv4ClientHash = g_hash_table_new(NULL, NULL);
    }
    if(!pRet)
    {
        try {
            pRet = new hostIf_DHCPv4Client(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create DHCPv4 client instance..\n");
        }
        g_hash_table_insert(dhcpv4ClientHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}


GList* hostIf_DHCPv4Client::getAllInstances()
{
    if(dhcpv4ClientHash)
        return g_hash_table_get_keys(dhcpv4ClientHash);
    return NULL;
}


void hostIf_DHCPv4Client::closeInstance(hostIf_DHCPv4Client *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(dhcpv4ClientHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

hostIf_DHCPv4Client::~hostIf_DHCPv4Client()
{
    if(m_notifyHash)
    {
        g_hash_table_destroy(m_notifyHash);
    }
}

void hostIf_DHCPv4Client::closeAllInstances()
{
    if(dhcpv4ClientHash)
    {
        GList* tmp_list = g_hash_table_get_values (dhcpv4ClientHash);

        while(tmp_list)
        {
           hostIf_DHCPv4Client* pDev = (hostIf_DHCPv4Client *)tmp_list->data;
           tmp_list = tmp_list->next;
           closeInstance(pDev);
        }
    }
}

int hostIf_DHCPv4Client::get_Device_DHCPv4_Client_InterfaceReference(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int ret = get_Device_DHCPv4_Client_Fields(eDHCPv4Interface);
    if(ret== OK)
    {
        if(bBackUpFlags.interface && pChanged && (strncmp(backupDhcpClient.interface, dhcpClient.interface, sizeof(backupDhcpClient.interface))))
        {
            *pChanged= true;
        }
        bBackUpFlags.interface = 1;
        strncpy(backupDhcpClient.interface, dhcpClient.interface, MAX_IF_LEN -1);
        backupDhcpClient.interface[MAX_IF_LEN-1]='\0';
        strncpy(stMsgData->paramValue, dhcpClient.interface, TR69HOSTIFMGR_MAX_PARAM_LEN -1);
        stMsgData->paramValue[TR69HOSTIFMGR_MAX_PARAM_LEN -1]='\0';
        stMsgData->paramLen=strlen(dhcpClient.interface);
        stMsgData->paramtype = hostIf_StringType;
    }
    return ret;
}

int hostIf_DHCPv4Client::get_Device_DHCPv4_Client_DnsServer(HOSTIF_MsgData_t *stMsgData, bool* pChanged)
{
    int ret= get_Device_DHCPv4_Client_Fields(eDHCPv4Dnsservers);
    if(ret== OK)
    {
        if(bBackUpFlags.dnsservers && pChanged && (strncmp(backupDhcpClient.dnsservers, dhcpClient.dnsservers, sizeof(backupDhcpClient.dnsservers))))
        {
            *pChanged= true;
        }
        bBackUpFlags.dnsservers = 1;
        strncpy(backupDhcpClient.dnsservers, dhcpClient.dnsservers,  MAX_DNS_SERVER_LEN -1);
        backupDhcpClient.dnsservers[MAX_DNS_SERVER_LEN-1]='\0';
        strncpy(stMsgData->paramValue, dhcpClient.dnsservers, TR69HOSTIFMGR_MAX_PARAM_LEN -1);
        stMsgData->paramValue[TR69HOSTIFMGR_MAX_PARAM_LEN -1]='\0';
        stMsgData->paramLen=strlen(dhcpClient.dnsservers);
        stMsgData->paramtype = hostIf_StringType;
    }
    return ret;
}

int hostIf_DHCPv4Client::get_Device_DHCPv4_Client_IPRouters(HOSTIF_MsgData_t *stMsgData, bool* pChanged)
{
    int ret = get_Device_DHCPv4_Client_Fields(eDHCPv4Iprouters);

    if ( ret == OK)
    {
        if(bBackUpFlags.ipRouters && pChanged && (strncmp(backupDhcpClient.ipRouters, dhcpClient.ipRouters, sizeof(backupDhcpClient.ipRouters))))
        {
            *pChanged= true;
        }
        bBackUpFlags.ipRouters = 1;
        strncpy(backupDhcpClient.ipRouters, dhcpClient.ipRouters, MAX_IP_ROUTER_LEN - 1);
        backupDhcpClient.ipRouters[MAX_IP_ROUTER_LEN -1 ] ='\0';
        strncpy(stMsgData->paramValue, dhcpClient.ipRouters, TR69HOSTIFMGR_MAX_PARAM_LEN -1);
        stMsgData->paramValue[TR69HOSTIFMGR_MAX_PARAM_LEN -1]='\0';
        stMsgData->paramLen=strlen(dhcpClient.ipRouters);
        stMsgData->paramtype = hostIf_StringType;
    }
    return ret;
}

int hostIf_DHCPv4Client::get_Device_DHCPv4_ClientNumberOfEntries(HOSTIF_MsgData_t *stMsgData)
{
    int num=1;
    char cmd[MAX_CMD_LEN]={'\0'};
    FILE* cmdOP=NULL;
    char buffer[MAX_BUF_LEN]={'\0'};
    int ret=NOK;
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Entering [%s]\n", __FUNCTION__);
    snprintf(cmd,MAX_CMD_LEN -1,"ip r | grep default|wc -l");
    cmdOP=popen(cmd,"r");
    if (cmdOP)
    {
        fgets(buffer,MAX_BUF_LEN,cmdOP);
        num = strtol(buffer,NULL,10);
        put_int(stMsgData->paramValue, num);
        stMsgData->paramtype = hostIf_UnsignedIntType;
        stMsgData->paramLen = sizeof(unsigned int);
        pclose(cmdOP);
        ret = OK;
    }
    return ret;
}

bool hostIf_DHCPv4Client::isIfnameInroutetoDNSServer(char* dnsServer, char* ifName)
{
    char cmd[MAX_CMD_LEN]={'\0'};
    char opIfName[8]={'\0'};
    FILE* cmdOP;

    snprintf(cmd,MAX_CMD_LEN -1, "ip route get %s | grep %s | awk 'BEGIN {FR=\" \"} {printf $5}'", dnsServer, dnsServer);
    cmdOP=popen(cmd, "r");
    if (cmdOP)
    {
        fgets(opIfName, 8, cmdOP);
        pclose(cmdOP);
    }
    if(strncmp(opIfName, ifName, sizeof(opIfName))==0)
    {
        return 1;
    }
    return 0;
}

int hostIf_DHCPv4Client::get_Device_DHCPv4_Client_Fields(DHCPv4ClientMembers dhclientMember)
{
    FILE* cmdOP;
    int ret=NOK;
    int len;
    char ifname[IFNAMSIZ]={'\0'};
    char buffer[MAX_BUF_LEN]={'\0'};
    char dupbuf[MAX_BUF_LEN]={'\0'};
    char cmd[MAX_CMD_LEN]={'\0'};
    char *token=NULL;
    char *savePtr=NULL;
    int ipNumOfEntries=0;
    int index=0;
    GList *devList =NULL;
    GList *elem=NULL;
    HOSTIF_MsgData_t msgData;

    memset(&msgData, 0, sizeof(msgData));
    ret=getInterfaceName(ifname);
    if(ret!=OK)
        return ret;


    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Entering [%s] asking for member %d\n", __FUNCTION__, dhclientMember);
    switch(dhclientMember)
    {
        case eDHCPv4Interface:
            /* Get the number of IP interfaces, loop through the objects match the dev_id which matches the ifname and name the interface */
            hostIf_IP::get_Device_IP_InterfaceNumberOfEntries(&msgData);
            ipNumOfEntries=get_int(msgData.paramValue);
            for(index = 1; index<=ipNumOfEntries; index++)
            {
                hostIf_IPInterface *pIface = hostIf_IPInterface::getInstance(index);
                pIface->get_Interface_Name(&msgData);
                if(strncmp(msgData.paramValue, ifname, strlen(ifname))==0)
                {
                    snprintf(dhcpClient.interface, MAX_IF_LEN - 1, "Device.IP.Interface.%d", index);
                    ret=OK;
                    break;
                }

            }
            break;
        case eDHCPv4Dnsservers:
            memset(dhcpClient.dnsservers, '\0', sizeof dhcpClient.dnsservers);
            /* Get the name server from resolv.conf and check if they belong to the interface address*/
            snprintf(cmd, MAX_CMD_LEN - 1, "cat /etc/resolv.conf  |  grep nameserver | cut -f2 -d' '|awk 'BEGIN { RS=\"\"; FS=\"\\n\"}{ for (i=1; i<=NF; i++)  printf $i\",\" }'");
            cmdOP=popen(cmd, "r");
            if (cmdOP)
            {
                fgets(buffer, MAX_BUF_LEN, cmdOP);
                strncpy(dupbuf, buffer, MAX_BUF_LEN - 1);
                dupbuf[MAX_BUF_LEN-1]='\0';
                memset(dhcpClient.dnsservers, 0, sizeof(dhcpClient.dnsservers));
                token=(char *)strtok_r(dupbuf, ",", &savePtr);
                len=0;
                /*check if we have a valid token and if the token is a valid IP address*/
                if(token!=NULL && isValidIPAddr(token))
                {
                    if((isIfnameInroutetoDNSServer(token, ifname)))
                    {
                        snprintf(dhcpClient.dnsservers, sizeof(dhcpClient.dnsservers), "%s,", token);
                        len=strlen(token)+1;
                    }
                    while(token=strtok_r(NULL, ",", &savePtr))
                    {
                        if((isIfnameInroutetoDNSServer(token, ifname)))
                        {
                            snprintf(dhcpClient.dnsservers+len, sizeof(dhcpClient.dnsservers)-len, "%s,", token);
                            len+=strlen(token)+1;
                        }
                    }
                    len=strlen(dhcpClient.dnsservers);
                    if(len > 0)
                    {
                        dhcpClient.dnsservers[len-1]='\0';
                    }
                    ret=OK;
                }
                pclose(cmdOP);
            }
            break;
        case eDHCPv4Iprouters:
            memset(dhcpClient.ipRouters, '\0', sizeof(dhcpClient.ipRouters));
            memset(cmd, 0, sizeof cmd);
            /*Get the default interface name and its gateway. If the interface name matches with the class interface, then fill iprouters */
            snprintf(cmd, MAX_CMD_LEN - 1 ,"ip r|grep default| grep %s |awk '{printf $3}'",ifname);
            cmdOP=popen(cmd, "r");
            if (cmdOP)
            {
                fgets(buffer, MAX_BUF_LEN, cmdOP);
                if(buffer[0]!='\0')
                {
                    snprintf(dhcpClient.ipRouters, sizeof(dhcpClient.ipRouters), "%s",  buffer);
                }
                ret=OK;
                pclose(cmdOP);
            }
            break;
        default:
            ret=NOT_HANDLED;
    }
    return ret;
}

bool hostIf_DHCPv4Client::isValidIPAddr(char* addr)
{
    int i, count=0;
    int retVal=1;
    char temp[MAX_IP_LEN]={0};
    char *tptr, *tptr1, *savePtr;
    int val=0;
    int alpha=0;
    int valid=1;

    /*ipv4 address cannot be greater than 15 chars*/
    if(strlen(addr)>15)
        return 0;
    strncpy(temp, addr, MAX_IP_LEN -1);
    temp[MAX_IP_LEN-1]='\0';
    /*Check if the address contains only numbers and has 3 dots.*/
    for (i=0; temp[i]!='\0';  ((temp[i]=='.')?count++:(isdigit(temp[i])?1:alpha++)), i++);
    do
    {
        if(count!=3)
        {
            retVal= 0;
            break;
        }
        if(alpha)
        {
            retVal= 0;
            break;
        }
        /*each segment in the IP address cannot be greater than 255 */
        tptr=strtok_r(temp, ".", &savePtr);
        if(tptr)
        {
            val=atoi(tptr);
            if(val>255 || val < 0)
            {
                retVal= 0;
                break;
            }
            while(count)
            {
                tptr=strtok_r(NULL, ".", &savePtr);
                if(tptr)
                {
                    val=atoi(tptr);
                    if(val>255 || val < 0)
                    {
                        retVal= 0;
                        break;
                    }
                }
                else
                {
                    retVal= 0;
                    break;
                }
                count --;
            }
        }
        else
        {
            retVal= 0;
            break;
        }
    } while(0);
    return retVal;
}
int hostIf_DHCPv4Client::getInterfaceName(char* ifname){
    char cmd[MAX_CMD_LEN]={'\0'};
    char opIfName[IFNAMSIZ+1]={'\0'};
    FILE* cmdOP=NULL;
    int retVal=NOK;

    snprintf(cmd,MAX_CMD_LEN - 1, "ip r | grep default | sed -n '%dp' | awk '{printf $5}'",this->dev_id);
    cmdOP=popen(cmd,"r");
    if (cmdOP)
    {
        fgets(opIfName,IFNAMSIZ,cmdOP);
        if(opIfName[0]!='\0')
        {
            strncpy(ifname,opIfName,IFNAMSIZ);
            retVal=OK;
        }
        else
        {
            retVal= NOK;
        }
        pclose(cmdOP);
    }
    return retVal;
}
