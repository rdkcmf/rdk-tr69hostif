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
 * FileName  : Device_DHCPv4_Client.h
 * Purpose   : This class gives details that are required for filling Device.DHCPv4.Client
 */

/**
 * @file Device_DHCPv4_Client.h
 * @brief The header file provides TR069 device DHCPv4 Client information APIs.
 */

/**
 * @defgroup TR69_HOSTIF_DHCPv4_CLIENT TR-069 Object (Device.DHCPv4)
 * @par Overview
 * Dynamic Host Configuration Protocol version 4 (DHCPv4) allows for the automatic configuration of
 * client systems on an Internet Protocol version 4 (IPv4) network. DHCPv4 eliminates the need to
 * individually configure devices, allowing them to set network properties by connecting to the DHCPv4 server instead.
 * @par Function
 * Prerequisites :
 * - DHCPv4 server system
 * - DHCPv4 client system
 * - Navigable network connection between DHCPv4 server and DHCPv4 client
 * When a new system is brought online (generally at boot) and needs to configure itself for
 * communicating on the network it makes a broadcast looking for a DHCPv4 server. When the DHCPv4
 * server observes this broadcast it responds to the request with an offer that contains the
 * information necessary to join the network. At its most basic level, this includes an IP address,
 * netmask, the DHCP server's IP address, the default route/gateway, DNS server address(es), and
 * the lease time (how long the offer will be good for). The DHCPv4 client then responds requesting
 * the configuration offered. Finally, the DHCPv4 server responds acknowledging and granting the
 * request. In shorter form, this transaction is as follows:
 * - Client requests information (the DHCPDISCOVER)
 * - One or more servers respond (the DHCPOFFER)
 * - Client selects one response and requests that configuration from the offering server (the DHCPREQUEST)
 * - The server which made the offer responds acknowledging the configuration (the DHCPACKNOWLEDGEMENT)
 *
 * Example container
 * DHCPDISCOVER:
 * Bit Range |	Bit Use	Nominal                            | Value
 * ----------|---------------------------------------------|--------
 * 0 - 15	 | Source Port Number                          | 01000011
 * 16 - 31	 | Destination Port Number        			   | 01000100
 * 32 - 47	 | Packet Length	              			   | 0000000101001000
 * 48 - 63	 | Checksum	                      			   | varies
 * 64 - 71	 | Operation Code (OP)	          			   | 00000001
 * 72 - 79	 | Hardware Address Type (HTYPE)  			   | 00000001
 * 80 - 87	 | Hardware Address Length (HLEN) 			   | 00000110
 * 88 - 95	 | Hops (HOPS)	                               | 00000000
 * 96 - 127	 | Transaction ID (XID)	                       | varies
 * 128 - 143 | Seconds Since Client Started Request (SECS) | 0000000000000000
 * 144 - 159 | Flags (FLAGS)							   | 0000000000000000
 * 160 - 181 | Client IP Address (CIADDR)	               | 00000000000000000000000000000000
 * 182 - 213 | Your IP Address (YIADDR)	                   | 00000000000000000000000000000000
 * 214 - 245 | Server IP Address (SIADDR)	               | 00000000000000000000000000000000
 * 246 - 277 | Relay IP Address (GIADDR)	               | 00000000000000000000000000000000
 *
 * DHCPOFFER
 * Bit Range  |	Bit Use
 * -----------|---------
 *  a - b	  |  Use
 *
 * DHCPREQUEST
 * Bit Range  | Bit Use
 * -----------|--------
 *  a - b	  |   Use
 *
 * DHCPACKNOWLEDGEMENT
 * Bit Range           | Bit Use
 * --------------------|---------
 *  a - b	           |  Use
 *
 * @ingroup TR69_HOSTIF_PROFILE
 *
 * @defgroup TR69_HOSTIF_DHCPv4_CLIENT_API TR-069 Object (Device.DHCPv4.Interface.{i}) Public APIs
 * Describe the details about RDK TR069 Device DHCPv4 Client APIs specifications.
 * @ingroup TR69_HOSTIF_DHCPv4_CLIENT
 *
 * @defgroup TR69_HOSTIF_DHCPv4_CLIENT_CLASSES TR-069 Object (Device.DHCPv4.Interface.{i}) Public Classes
 * Describe the details about classes used in TR069 Device DHCPv4 Client.
 * @ingroup TR69_HOSTIF_DHCPv4_CLIENT
 *
 * @defgroup TR69_HOSTIF_DHCPv4_CLIENT_DSSTRUCT TR-069 Object (Device.DHCPv4.Interface.{i}) Public DataStructure
 * Describe the details about structure used in TR069 Device DHCPv4 Client.
 * @ingroup TR69_HOSTIF_DHCPv4_CLIENT
 *
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

/**
 * @addtogroup TR69_HOSTIF_DHCPv4_CLIENT_DSSTRUCT
 * @{
 */
/**
 * @brief These values are the members variables of the DHCPv4Client structure.
 */
typedef struct DHCPv4Client {
    char interface[MAX_IF_LEN];
    char dnsservers[MAX_DNS_SERVER_LEN];
    char ipRouters[MAX_IP_ROUTER_LEN];
}DHCPv4Client;

/**
 * @brief These values are the members variables of the DHCPv4ClientParamBackUpFlag structure.
 */
typedef struct DHCPv4ClientParamBackUpFlag {
    unsigned int interface:1;
    unsigned int dnsservers:1;
    unsigned int ipRouters:1;
}DHCPv4ClientParamBackUpFlag;

/**
 * @enum DHCPv4ClientMembers
 * @brief These values are the members of the DHCPv4ClientMembers.
 */
typedef enum DHCPv4ClientMembers {
    eDHCPv4Interface,
    eDHCPv4Dnsservers,
    eDHCPv4Iprouters
}DHCPv4ClientMembers;

/** @} */ //End of the Doxygen tag TR69_HOSTIF_DHCPv4_CLIENT_DSSTRUCT

/**
 * @brief This class provides the interface for getting device DHCPv4 Client information.
 * @ingroup TR69_HOSTIF_DHCPv4_CLIENT_CLASSES
 */
class hostIf_DHCPv4Client {
    int dev_id;
    static  GHashTable  *dhcpv4ClientHash;
    static GMutex *m_mutex;
    hostIf_DHCPv4Client(int dev_id);
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
        
};
#endif

