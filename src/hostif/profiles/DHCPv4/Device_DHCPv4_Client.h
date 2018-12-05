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


#ifndef DEVICE_DHCPv4_CLIENT_H_
#define DEVICE_DHCPv4_CLIENT_H_
#include <net/if.h>
#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "hostIf_updateHandler.h"

#define MAX_IF_LEN 256 //Length of interface. http://www.broadband-forum.org/cwmp/tr-181-2-8-0.html#D.Device:2.Device.DHCPv4.Client.{i}.Interface
#define MAX_DNS_SERVER_LEN 256 //Length of DNS servers. http://www.broadband-forum.org/cwmp/tr-181-2-8-0.html#D.Device:2.Device.DHCPv4.Client.{i}.DNSServers
#define MAX_IP_ROUTER_LEN 256 //Length of IP Routers. http://www.broadband-forum.org/cwmp/tr-181-2-8-0.html#D.Device:2.Device.DHCPv4.Client.{i}.IPRouters 

typedef struct DHCPv4Client {
    char interface[MAX_IF_LEN];
    char dnsservers[MAX_DNS_SERVER_LEN];
    char ipRouters[MAX_IP_ROUTER_LEN];
}DHCPv4Client;

typedef struct DHCPv4ClientParamBackUpFlag {
    unsigned int interface:1;
    unsigned int dnsservers:1;
    unsigned int ipRouters:1;
}DHCPv4ClientParamBackUpFlag;

typedef enum DHCPv4ClientMembers {
    eDHCPv4Interface,
    eDHCPv4Dnsservers,
    eDHCPv4Iprouters
}DHCPv4ClientMembers;


class hostIf_DHCPv4Client {
    int dev_id;
    static  GHashTable  *dhcpv4ClientHash;
    static GMutex *m_mutex;
    static  GHashTable  *m_notifyHash;
    hostIf_DHCPv4Client(int dev_id);
    ~hostIf_DHCPv4Client();
    static DHCPv4Client dhcpClient;
    DHCPv4Client backupDhcpClient;
    DHCPv4ClientParamBackUpFlag bBackUpFlags;
    int get_Device_DHCPv4_Client_Fields(DHCPv4ClientMembers dhclientMember);
    bool isIfnameInroutetoDNSServer(char* dnsServer, char* ifName);
    bool isValidIPAddr(char* addr);
    int getInterfaceName(char* ifname);
    public:
        static hostIf_DHCPv4Client* getInstance(int dev_id);
        static GList* getAllInstances();
        static void closeInstance(hostIf_DHCPv4Client *pDev);
        static void closeAllInstances();
        static int get_Device_DHCPv4_ClientNumberOfEntries(HOSTIF_MsgData_t *stMsgData);
        int get_Device_DHCPv4_Client_InterfaceReference(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
        int get_Device_DHCPv4_Client_DnsServer(HOSTIF_MsgData_t *stMsgData,bool* pChanged=NULL);
        int get_Device_DHCPv4_Client_IPRouters(HOSTIF_MsgData_t *stMsgData,bool* pChanged=NULL);
        static void getLock();
        static void releaseLock();
        static GHashTable* getNotifyHash();
        
};
#endif

