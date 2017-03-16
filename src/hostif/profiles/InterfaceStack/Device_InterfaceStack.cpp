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
 * @file Device_InterfaceStack.cpp
 * @brief This source file contains the APIs of device interface stack.
 */

/**
 * @file Device_InterfaceStack.cpp
 *
 * @brief Device.InterfaceStack API Implementation.
 *
 * This is the implementation of the InterfaceStack API.
 *
 * @author vejuturu@cisco.com
 */


/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/
#include <string>
#include <sstream>
#include <map>
#include <utility> // for std::pair
#include <typeinfo>
#include <list>

#include <string.h>
#include "Device_InterfaceStack.h"
#include "Device_IP.h"
#include "Device_IP_Interface.h"
#include "Device_Ethernet_Interface.h"
#include "hostIf_utils.h"

#ifdef USE_MoCA_PROFILE
#include "Device_MoCA_Interface.h"
#endif

#define MAX_BUF_LEN 128
#define MAX_CMD_LEN 256
#define MAX_IFNAME_LEN 64
#define SYS_CLASS_NET_PATH  "/sys/class/net/"

#define IN
#define OUT
#define INOUT
#define DEVICE_ETHERNET_INTERFACE(PORT_NUM)  std::string("Device.Ethernet.Interface.") + int_to_string(PORT_NUM)
#ifdef USE_MoCA_PROFILE
#define DEVICE_MOCA_INTERFACE(PORT_NUM)  std::string("Device.MoCA.Interface.") + int_to_string(PORT_NUM)
#endif
#define DEVICE_IP_INTERFACE(PORT_NUM) std::string("Device.IP.Interface.") + int_to_string(PORT_NUM)
#define DEVICE_BRIDGING_BRIDGE(BRIDGE_NUM, PORT_NUM) (std::string("Device.Bridging.Bridge.") + \
                                                      int_to_string(BRIDGE_NUM) + \
                                                      ".PORT." + \
                                                      int_to_string(PORT_NUM))

/*****************************************************************************
 * Initialize all the static variables of the class
 *****************************************************************************/
GHashTable* hostif_InterfaceStack::stIshash = NULL;
GHashTable* hostif_InterfaceStack::stBridgeTableHash = NULL;
GMutex* hostif_InterfaceStack::stMutex = NULL;
GHashTable* hostif_InterfaceStack::m_notifyHash = NULL;

/*
 * hostif_InterfaceStack Constructor
 */

/**
 * @brief Class Constructor of the class hostif_InterfaceStack.
 *
 * It will initialize the device id.
 *
 * @param[in] dev_id Identification number of the device.
 * @param[in] _higherLayer String of the higher layer i.e "Device.Bridging.Bridge.1.port.2".
 * @param[in] _lowerLayer String of the lower layer i.e "Device.Ethernet.Interface.1".
 */
hostif_InterfaceStack::hostif_InterfaceStack(int dev_id, char *_higherLayer, char *_lowerLayer):dev_id(dev_id)
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Inside constructor for dev_id:%d\n", dev_id);

    bCalledHigherLayer = 0;
    bCalledLowerLayer = 0;

    memset(backupHigherLayer, '\0', sizeof(backupHigherLayer));
    memset(backupLowerLayer, '\0', sizeof(backupLowerLayer));
    if(_higherLayer)
    {
        strncpy(higherLayer, _higherLayer, sizeof(higherLayer));
    }
    else
    {
        memset(higherLayer, '\0', sizeof(higherLayer));
    }

    if(_lowerLayer)
    {
        strncpy(lowerLayer, _lowerLayer, sizeof(lowerLayer));
    }
    else
    {
        memset(lowerLayer, '\0', sizeof(lowerLayer));
    }
}

/*
 * It populates an interface stack table hash and returns the number of entries in the table by
 * following below steps.
 *  - Deletes existing interface stack table and bridge table
 *  - Populated bridge table with the list of bridges and bridge interfaces
 *
 *  - Build lower layer entries for
 *     - All available ethernet interfaces
 *     - All available Moca Interfaces (If MoCA is enabled)
 *  - Build higher layer and lower layer entries for all bridges and underlying interfaces that are part of the bridge.
 *     - Note: bridges are filled up only with lower layer entries
 *             underlying interfaces are filled up with higher layer entries
 *             lower layer entries for underlying bridge interfaces have been built in previous step.
 *  - Fill up the remaining higher layer entries with the IP interfaces.
 *     - 'remaining higher layer entries' is meant that the unfilled higher layer entries in previous steps.
 *
 *  - Create interface stack instances by parsing the 'layer info' map that was filled up in previous steps.
 *
 * Consider the following example:
 * Given the 'Lower level Interfaces' and 'Bridge Table', the program should build the Interface Stack as below.

 * Lower level Interfaces
 * ----------------------
 * InterfaceName	TypeOfInterface	LowerLevelDM
 * bcm0                 Ethernet        Device.Ethernet.Interface.1
 * eth2	                Ethernet        Device.Ethernet.Interface.2
 * eth1	                MoCA            Device.MoCA.Interface.1
 * Bridge Table
 * ------------
 * BridgeName	ParentInterface	ChildInterface
 * hnbr0        hnbr0	        -
 * hnbr0        -               bcm0
 * hnbr0        -               MoCA
 * Interface Stack to be built
 * ---------------------------
 * Row	HigherLayer	                   LowerLayer
 * 1    Device.Bridging.Bridge.1.port.2	   Device.Ethernet.Interface.1
 * 3    Device.Bridging.Bridge.1.port.3	   Device.MoCA.Interface.1
 * 4    Device.IP.Interface.8	           Device.Ethernet.Interface.2
 * 5    Device.IP.Interface.9	           Device.Bridging.Bridge.1.port.1
 * 6    Device.Bridging.Bridge.1.port.1	   Device.Bridging.Bridge.1.port.2
 * 7    Device.Bridging.Bridge.1.port.1	   Device.Bridging.Bridge.1.port.3
 *
 */
int hostif_InterfaceStack:: get_Device_InterfaceStackNumberOfEntries(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOK;
    int interfaceStackNumOfEntries = 0;
    char lowerLayerPath[MAX_HIGHERLAYER_LEN] = {'\0'};
    char higherLayerPath[MAX_HIGHERLAYER_LEN] = {'\0'};

    InterfaceStackMap_t layerInfo;
    InterfaceStackMap_t::iterator layerIterator;
    IPInterfacesMap_t ipInterfaceMap;

    // Close all instances of the interface stack table
    hostif_InterfaceStack::closeAllInstances();

    // Delete existing Bridge table
    hostif_InterfaceStack::deleteBridgeTable();

    // Populate bridge table
    hostif_InterfaceStack::populateBridgeTable();

    do
    {
        if( OK != getIPInterfaces(ipInterfaceMap))
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s:%d get_Device_IP_InterfaceNumberOfEntries failed\n", __FILE__, __LINE__);
            break;
        }

        if( OK != buildLowerLayerInfo<hostIf_EthernetInterface>(layerInfo))
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s:%d Failed while building layer info for Ethernet interfaces\n", __FILE__, __LINE__);
            break;
        }

#ifdef USE_MoCA_PROFILE
        if( OK != buildLowerLayerInfo<hostIf_MoCAInterface>(layerInfo))
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s:%d Failed while building layer info for MoCA interfaces\n", __FILE__, __LINE__);
            break;
        }
#endif

        if( OK != buildBridgeTableLayerInfo(layerInfo))
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s:%d Failed while building layer info for bridges\n", __FILE__, __LINE__);
            break;
        }

        fillHigherLayersWithIP(layerInfo, ipInterfaceMap);

        for(layerIterator = layerInfo.begin(); layerIterator != layerInfo.end(); layerIterator++)
        {
            interfaceStackNumOfEntries++;

            memset(lowerLayerPath, '\0', sizeof(lowerLayerPath));
            memset(higherLayerPath, '\0', sizeof(higherLayerPath));

            snprintf(higherLayerPath, sizeof(higherLayerPath), layerIterator->second.higherLayer.c_str());
            snprintf(lowerLayerPath, sizeof(lowerLayerPath), layerIterator->second.lowerLayer.c_str());

            if(OK == hostif_InterfaceStack::createInstance(interfaceStackNumOfEntries, higherLayerPath, lowerLayerPath ))
            {
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s:%d successfully created instance [%d] with higher layer [%s] lower layer [%s]\n",
                      __FUNCTION__, __LINE__, interfaceStackNumOfEntries, higherLayerPath, lowerLayerPath);
            }
        }
    } while(0);

    if(interfaceStackNumOfEntries > 0)
    {
        ret = OK;
        stMsgData->paramtype=hostIf_IntegerType;
        stMsgData->paramLen = sizeof(int);
        put_int(stMsgData->paramValue, interfaceStackNumOfEntries);
    }

    return ret;
}

void hostif_InterfaceStack::getLock()
{
    if(!stMutex)
    {
        stMutex = g_mutex_new();
    }
    g_mutex_lock(stMutex);
}

void hostif_InterfaceStack::releaseLock()
{
    g_mutex_unlock(stMutex);
}

GHashTable* hostif_InterfaceStack::getNotifyHash()
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

hostif_InterfaceStack::~hostif_InterfaceStack()
{
    if(m_notifyHash)
    {
        g_hash_table_destroy(m_notifyHash);
    }
}

hostif_InterfaceStack* hostif_InterfaceStack::getInstance(int dev_id)
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Entering [%s]\n", __FUNCTION__);
    hostif_InterfaceStack* pRet = NULL;

    if(stIshash)
    {
        pRet = (hostif_InterfaceStack *)g_hash_table_lookup(stIshash, (gpointer) dev_id);
    }

    if(!pRet)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"No instance is found with dev_id %d\n", dev_id);
    }

    return pRet;
}

int hostif_InterfaceStack::createInstance(int dev_id, char *higherLayer, char *lowerLayer)
{
    int ret = NOK;
    hostif_InterfaceStack *instance = NULL;

    if(higherLayer && lowerLayer)
    {
        if(!stIshash)
        {
            stIshash = g_hash_table_new(NULL, NULL);
        }

        instance = new hostif_InterfaceStack(dev_id, higherLayer, lowerLayer );

        if(instance)
        {
            g_hash_table_insert(stIshash, (gpointer)dev_id, instance);
            ret = OK;
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Unable to create InterfaceStack instance\n");
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s:%d NULL parameters passed\n", __FUNCTION__, __LINE__);
    }

    return ret;
}


GList* hostif_InterfaceStack::getAllInstances()
{
    if(stIshash)
        return g_hash_table_get_keys(stIshash);
    return NULL;
}


void hostif_InterfaceStack::closeInstance(hostif_InterfaceStack *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(stIshash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}


void hostif_InterfaceStack::closeAllInstances()
{
    if(stIshash)
    {
        GList* tmp_list = g_hash_table_get_values (stIshash);

        while(tmp_list)
        {
           hostif_InterfaceStack* pDev = (hostif_InterfaceStack *)tmp_list->data;
           tmp_list = tmp_list->next;
           closeInstance(pDev);
        }
    }
}

/*
 *  hostif_InterfaceStack::populateBridgeTable()
 *
 *  It identifies the list of bridges and its interfaces by following steps
 *      - Iterate through the sub-directories of /sys/class/net and check if bridge directory exists
 *      - If bridge directory exists, it checks if the sub-directories of /sys/class/net has brif directory
 *      - If brif directory exists, it gets the the list of interfaces present in brif directory in comma separated format
 *      - And then it inserts a new row in stBridgeTableHash with key as bridge name and value as comma separated interfaces *
 */
int hostif_InterfaceStack::populateBridgeTable()
{
    int ret = NOK;
    struct dirent **nameList;
    int noOfDirEntries = -1;
    char cmd[MAX_CMD_LEN] = {'\0'};
    char result_buff[MAX_BUF_LEN] = {'\0'};
    FILE *fp = NULL;

    if(0 > (noOfDirEntries = scandir(SYS_CLASS_NET_PATH, &nameList, NULL, alphasort)))
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s:%s:%d error opening %s\n", __FILE__, __FUNCTION__, __LINE__, SYS_CLASS_NET_PATH);
        return NOK;
    }

    // Account for extra 2 '/..' and '/.' directives
    for(int i = 2; i < noOfDirEntries; i++)
    {
        // Check if bride directory exists in sys/class/net/d_name
        snprintf(cmd, sizeof(cmd), "%s%s/bridge", SYS_CLASS_NET_PATH, nameList[i]->d_name);
        if(-1 != access(cmd, F_OK))
        {
            // Check if brif directory exists in sys/class/net/d_name
            snprintf(cmd, sizeof(cmd), "%s%s/brif", SYS_CLASS_NET_PATH, nameList[i]->d_name);
            if(-1 != access(cmd, F_OK))
            {
                // Get the list of interfaces present inside brif director in comma separated format
                snprintf(cmd, sizeof(cmd), "ls %s%s/brif|awk 'BEGIN{ORS=\",\";}{print $1}'", SYS_CLASS_NET_PATH, nameList[i]->d_name);
                fp = popen(cmd, "r");
                if(fp)
                {
                    memset(result_buff, '\0', sizeof(result_buff));
                    fgets(result_buff, sizeof(result_buff), fp);
                    // Insert a row in bridge table with bridge as key and comma separated interface as value
                    ret = insertRowIntoBridgeTable(nameList[i]->d_name, result_buff);
                    pclose(fp);
                    fp = NULL;
                }
            }
        }

    }

    while(noOfDirEntries--)
    {
        free (nameList[noOfDirEntries]);
    }
    free (nameList);

    return ret;
}

/*
 * Destructor to free key data in stBridgeTableHash when a row is removed from the hash
 */
void hostif_InterfaceStack::bridgeInteface_key_data_free(gpointer key)
{
    char *myKey = (char*)key;

    if(myKey)
    {
        g_free(myKey);
    }
}

/*
 * Destructor to free value date in stBridgeTableHash when a row is removed from the hash
 */
void hostif_InterfaceStack::bridgeInteface_value_data_free(gpointer value)
{
    char *val = (char*)value;

    if(val)
    {
        g_free(val);
    }
}

/*
 * Deletes the bridge table hash (stBridgeTableHash)
 */
void hostif_InterfaceStack::deleteBridgeTable()
{
    GList *keyList = NULL;
    GList *elem = NULL;

    if(stBridgeTableHash)
    {
        keyList = g_hash_table_get_keys(stBridgeTableHash);
        for(elem = keyList; elem; elem = elem->next)
        {
            g_hash_table_remove(stBridgeTableHash, elem->data);
        }
        g_hash_table_destroy(stBridgeTableHash);
        stBridgeTableHash = NULL;
    }
    if(keyList)
        g_list_free(keyList);
}

/*
 * Inserts a new into stBridgeTableHash with bridge name as key and bridge interfaces as value
 */
int hostif_InterfaceStack::insertRowIntoBridgeTable(IN char *bridge, IN char *bridgeInterfaces)
{
    int ret = NOK;

    if(!stBridgeTableHash)
    {
        stBridgeTableHash = g_hash_table_new_full(NULL, NULL, (GDestroyNotify)bridgeInteface_key_data_free,
                                                  (GDestroyNotify)bridgeInteface_value_data_free);
    }

    if(bridge && bridgeInterfaces)
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s:%d Adding bridge %s bridgeInterfaces %s to hash table\n", __FUNCTION__, __LINE__, bridge, bridgeInterfaces);
        g_hash_table_insert(stBridgeTableHash, g_strdup(bridge), g_strdup(bridgeInterfaces));
        ret = OK;
    }

    return ret;
}

/*
 * This function returns the number of lower level interfaces available.
 *
 */
template<typename T>
int hostif_InterfaceStack::getLowerInterfaceNumberOfEntries()
{
    int numEntries = 0;
    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(msgData));


    if(0 == strcmp(typeid(T).name(), typeid(hostIf_EthernetInterface).name()))
    {
        if(OK == hostIf_EthernetInterface::get_Device_Ethernet_InterfaceNumberOfEntries(&msgData))
        {
            numEntries = get_int(msgData.paramValue);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s:%d get_Device_Ethernet_InterfaceNumberOfEntries failed\n", __FILE__, __LINE__);
        }
    }
#ifdef USE_MoCA_PROFILE
    else if(0 == strcmp(typeid(T).name(), typeid(hostIf_MoCAInterface).name()))
    {
        if(OK == hostIf_MoCAInterface::get_Device_MoCA_InterfaceNumberOfEntries(&msgData))
        {
            numEntries = get_int(msgData.paramValue);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s:%d get_Device_MoCA_InterfaceNumberOfEntries failed\n", __FILE__, __LINE__);
        }
    }
#endif
    
    return(numEntries);
}

/*
 * This function returns the interface name (such as 'eth0', 'bcm0') for lower level interfaces.
 */
template<typename T>
std::string hostif_InterfaceStack::getInterfaceName(T* pIface)
{
    std::string ifname;
    HOSTIF_MsgData_t msgData;
    memset(&msgData, 0, sizeof(msgData));

    if(0 == strcmp(typeid(T).name(), typeid(hostIf_EthernetInterface).name()))
    {
        if(OK == ((hostIf_EthernetInterface*)pIface)->get_Device_Ethernet_Interface_Name(&msgData))
        {
            ifname.append(msgData.paramValue);
        }
        else
        {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s:%d get_Device_Ethernet_Interface_Name failed\n", __FILE__, __LINE__);
        }
    }
#ifdef USE_MoCA_PROFILE
    else if(0 == strcmp(typeid(T).name(), typeid(hostIf_MoCAInterface).name()))
    {
        if(OK == ((hostIf_MoCAInterface*)pIface)->get_Device_MoCA_Interface_Name(&msgData))
        {
            ifname.append(msgData.paramValue);
        }
        else
        {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s:%d get_Device_MoCA_Interface_Name failed\n", __FILE__, __LINE__);
        }
    }
#endif

    return(ifname);
}

/*
 *  get Data Model path for the lower level interfaces.
 */
template<typename T>
std::string hostif_InterfaceStack::getLowerLayerName(int index)
{
    std::string baseName;
    if(0 == strcmp(typeid(T).name(), typeid(hostIf_EthernetInterface).name()))
    {
        baseName.append(DEVICE_ETHERNET_INTERFACE(index));
    }
#ifdef USE_MoCA_PROFILE
    else if(0 == strcmp(typeid(T).name(), typeid(hostIf_MoCAInterface).name()))
    {
        baseName.append(DEVICE_MOCA_INTERFACE(index));
    }
#endif

    return(baseName);
}

/*
 * This function builds the lower layer entries in the supplied 'layerInfo' map for the lower level physical interfaces
 *
 * In the example given, the buildLowerLayerInfo function builds the Interface stack table as below:
 * Note that the higherlayers have not been filled up yet.
 * InterfaceStackMap 
 * ---------------
 * InterfaceName      HigherLayer	                   LowerLayer
 * bcm0                                       Device.Ethernet.Interface.1
 * eth1                                       Device.MoCA.Interface.1
 * eth2                                       Device.Ethernet.Interface.2
 *
 */
template<typename T>
int hostif_InterfaceStack::buildLowerLayerInfo (InterfaceStackMap_t &layerInfo)
{
    int numOfEntries = getLowerInterfaceNumberOfEntries<T> ();
    for (int index = 1; index <= numOfEntries; index++)
    {
        T *pIface = T::getInstance (index);
        if (pIface == NULL)
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s:%d getInstance failed\n", __FILE__, __LINE__);
            return NOK;
        }

        std::string ifname = getInterfaceName<T> (pIface);
        if (!ifname.empty ())
        {
            std::pair<std::string, LayerInfo_t> map_element;
            map_element.first = ifname;
            map_element.second.higherLayer = std::string ("");
            map_element.second.lowerLayer = getLowerLayerName<T> (index);

            layerInfo.insert (map_element);
        }
    }

    return OK;
}

/*
 *  This function get the list of bridge elements from the 
 *  CSV format for bridge elements.
 */
std::list<std::string> hostif_InterfaceStack::getBridgeElements(char* elementsCSV)
{
    char *token = NULL;
    char *savePtr = NULL;
    std::list<std::string> bridgeElements;

    if(elementsCSV)
    {
        token  = strtok_r(elementsCSV, ",", &savePtr);
        while(token != NULL)
        {
            bridgeElements.push_back(std::string(token));
            token = strtok_r(NULL, ",", &savePtr);
        }
    }

    return bridgeElements;
}

/*
 *  This function adds the lower layer info for bridges.
 *  Lower layer info contains the higher layer of unmanaged interface of the link
 *
 * In the example given, the buildLowerLayerInfo function builds the Interface stack table as below:
 * Note that the higherlayers have not been filled up yet.
 * InterfaceStackMap 
 * ---------------
 * InterfaceName      HigherLayer	                   LowerLayer
 * bcm0                                       Device.Ethernet.Interface.1
 * eth1                                       Device.MoCA.Interface.1
 * eth2                                       Device.Ethernet.Interface.2
 * hnbr0                                      Device.Bridging.Bridge.1.Port.1
 *
 */
void hostif_InterfaceStack::addBridgeNameLayerInfo(InterfaceStackMap_t &layerInfo, std::string ifname, std::string bridgeLowerLayer)
{
    InterfaceStackMap_t::iterator it = layerInfo.find(ifname);

    if(it != layerInfo.end())
    {
        it->second.lowerLayer = bridgeLowerLayer;
    }
    else
    {
        LayerInfo_t tempLayerInfo;
        std::pair<std::string, LayerInfo_t> map_element;

        tempLayerInfo.higherLayer = std::string("");
        tempLayerInfo.lowerLayer = bridgeLowerLayer;

        map_element.first = ifname;
        map_element.second = tempLayerInfo;

        layerInfo.insert(map_element);
    }
}

/*
 *  This function adds the higher layer info for bridge interfaces.
 *  Higher layer info contains the lower layer of unmanaged interface of the link
 *
 *
 * In the example given, the buildLowerLayerInfo function builds the Interface stack table as below:
 * The example illustrates for the one child entry only. Remaining child entries will be filled up same.
 *
 * InterfaceStackMap 
 * ---------------
 * InterfaceName      HigherLayer	                   LowerLayer
 * bcm0               Device.Bridging.Bridge.1.Port.1  Device.Ethernet.Interface.1
 * eth1                                                Device.MoCA.Interface.1
 * eth2                                                Device.Ethernet.Interface.2
 * hnbr0                                               Device.Bridging.Bridge.1.Port.1
 * hnbr0-chld         Device.Bridging.Bridge.1.Port.1  Device.Bridging.Bridge.1.Port.2
 *
 */
int hostif_InterfaceStack::addBridgeChildLayerInfo(InterfaceStackMap_t &layerInfo, std::string ifname, std::string bridgeHigherLayer)
{
    int rc = OK;
    InterfaceStackMap_t::iterator it = layerInfo.find(ifname);

    if(it != layerInfo.end())
    {
        it->second.higherLayer = bridgeHigherLayer;
    }
    else
    {
        gpointer origKey;
        gpointer value;

        if( TRUE == g_hash_table_lookup_extended (stBridgeTableHash, ifname.c_str(), &origKey, &value) )
        {
            LayerInfo_t tempLayerInfo;

            tempLayerInfo.higherLayer = bridgeHigherLayer;
            tempLayerInfo.lowerLayer = std::string("");

            layerInfo.insert( std::pair<std::string, LayerInfo_t>(ifname, tempLayerInfo));
        }
        else
        {
            rc = NOK;
        }
    }
    return(rc);
}

/*
 *  This function creates unmanaged layer info for the links between bridge and underlying bridge interfaces.
 *
 *
 * In the example given, the buildLowerLayerInfo function builds the Interface stack table as below:
 * The example illustrates for the one child entry only. Remaining child entries will be filled up same.
 *
 * InterfaceStackMap 
 * ---------------
 * InterfaceName      HigherLayer	                   LowerLayer
 * bcm0                                                Device.Ethernet.Interface.1
 * eth1                                                Device.MoCA.Interface.1
 * eth2                                                Device.Ethernet.Interface.2
 * hnbr0                                               Device.Bridging.Bridge.1.Port.1
 * hnbr0-chld         Device.Bridging.Bridge.1.Port.1  Device.Bridging.Bridge.1.Port.2
 *
 */
void hostif_InterfaceStack::addBridgeUnmanagedLayerInfo(InterfaceStackMap_t &layerInfo, std::string ifname, std::string bridgeHigherLayer, std::string bridgeLowerLayer)
{
    LayerInfo_t tempLayerInfo;
    std::pair<std::string, LayerInfo_t> map_element;

    tempLayerInfo.higherLayer = bridgeHigherLayer;
    tempLayerInfo.lowerLayer = bridgeLowerLayer; 

    map_element.first = ifname;
    map_element.second = tempLayerInfo;

    layerInfo.insert(map_element);
}

/*
 *   This function parses all the bridge list and underlying bridge interfaces
 *   creates all the needed layer info (higher layer and lower layer) in layerInfo map.
*/
int hostif_InterfaceStack::buildBridgeTableLayerInfo(InterfaceStackMap_t &layerInfo)
{
    int rc = OK;
    GList *bridgeList = NULL;
    GList *elem = NULL;
    int bridgeNum = 1;

    if(stBridgeTableHash)
    {
        GHashTableIter iter;
        gpointer key, value;

        g_hash_table_iter_init(&iter, stBridgeTableHash);
        while (g_hash_table_iter_next (&iter, &key, &value)) 
        {
            /* 
             * The Bridges and underlying interfaces have the ports.
             * The port number '1' is dedicated to the port number of the bridge.
             * The port numbers for the underlying bridges start from '2'.
             *
             */
            int portNum = 1; 
            std::string bridgeName;
            std::list<std::string> bridgeElements;
            std::string bridgeNameLayer; // LowerLayer string to be inserted in InterfaceStackMap
            std::list<std::string>::iterator it;

            if(key == NULL)
                continue;

            bridgeName.append((const char*)key);
            bridgeNameLayer.append( DEVICE_BRIDGING_BRIDGE(bridgeNum, portNum) );
            addBridgeNameLayerInfo(layerInfo, bridgeName, bridgeNameLayer);

            if(value == NULL)
                continue;

            bridgeElements = getBridgeElements( (char*) value);
            portNum++;
            for(it = bridgeElements.begin(); it != bridgeElements.end(); ++it, portNum++)
            {
                std::string unmanagedBridgeName;
                std::string ifname = *it;

                // HigherLayer String to be inserted in InterfaceStackMap
                std::string bridgeElementLayer = DEVICE_BRIDGING_BRIDGE(bridgeNum, portNum); 

                if(OK == addBridgeChildLayerInfo(layerInfo, ifname, bridgeElementLayer))
                {
                    //If Child Entry is added successfully, then create unmanaged bridges.
                    unmanagedBridgeName.append(bridgeName+"-child");
                    addBridgeUnmanagedLayerInfo(layerInfo, unmanagedBridgeName, bridgeNameLayer, bridgeElementLayer);
                }
            }
        }
    }

    if(bridgeList)
        g_list_free(bridgeList);

    return(rc);
}

/* getIPInterfaces
*  This function builds up a map for all the available IP interfaces.
*/
int hostif_InterfaceStack::getIPInterfaces(IPInterfacesMap_t& interfaceList)
{
    int rc = OK;
    int ipNumOfEntries = 0;
    int ipIndex = 1;
    HOSTIF_MsgData_t msgData;

    if( OK != hostIf_IP::get_Device_IP_InterfaceNumberOfEntries(&msgData))
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s:%d get_Device_IP_InterfaceNumberOfEntries failed\n", __FILE__, __LINE__);
        rc = NOK;
    }
    else
    {
        ipNumOfEntries=get_int(msgData.paramValue);
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s:%d ipNumOfEntries = %d\n", __FUNCTION__, __LINE__, ipNumOfEntries);

        for(ipIndex=1; ipIndex <= ipNumOfEntries; ipIndex++)
        {
            std::string ipIfName;
            hostIf_IPInterface *pIface = hostIf_IPInterface::getInstance(ipIndex);

            if(!pIface)
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s:%d hostIf_IPInterface::getInstance failed\n", __FILE__, __LINE__);
                rc=NOK;
                break;
            }

            memset(&msgData, '\0', sizeof(msgData));

            if (OK == pIface->get_Interface_Name(&msgData))
            {
                ipIfName.append(msgData.paramValue);
                interfaceList.insert( std::pair<std::string, int>(ipIfName, ipIndex));
            }
        }
    }
    return(rc);
}

/*
 *   fillHigherLayersWithIP
 *   This function fills up the empty 'higherLayer' entries in 'layerInfo' map
 *   interface names are the key to get the 'Device.IP.Interface.<interface number>'
 */

void hostif_InterfaceStack::fillHigherLayersWithIP(InterfaceStackMap_t &layerInfo, IPInterfacesMap_t& ipInterfaceMap)
{
    InterfaceStackMap_t::iterator layerIterator;

    for(layerIterator = layerInfo.begin(); layerIterator != layerInfo.end(); layerIterator++)
    {
        if(layerIterator->second.higherLayer.empty())
        {
            IPInterfacesMap_t::iterator interfaceIterator;
            std::string ifname = layerIterator->first;
            interfaceIterator = ipInterfaceMap.find(ifname);

            if(interfaceIterator != ipInterfaceMap.end())
            {
                int portNum = interfaceIterator->second;
                std::string higherLayer = DEVICE_IP_INTERFACE(portNum);
                layerIterator->second.higherLayer = higherLayer;
            }
        }
    }
}

void hostif_InterfaceStack::print_map(InterfaceStackMap_t &layerInfo)
{
    InterfaceStackMap_t::iterator layerIterator;

    for(layerIterator = layerInfo.begin(); layerIterator != layerInfo.end(); layerIterator++)
    {
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"%s - %s --- %s\n", layerIterator->first.c_str(), layerIterator->second.higherLayer.c_str(), layerIterator->second.lowerLayer.c_str());
    }
}

/*
 * Gets the HigherLayer of Interface Stack table for a given instance
 */

/**
 * @brief This function gets the higher layer of the device interface stack.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully get the higher layer of interface stack
 * else returns '-1'.
 * @ingroup TR69_HOSTIF_INTERFACESTACK_INTERFACE_API
 */
int hostif_InterfaceStack::get_Device_InterfaceStack_HigherLayer(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int ret = OK;

    if(bCalledHigherLayer && pChanged && strncmp(backupHigherLayer, higherLayer, sizeof(backupHigherLayer)))
    {
        *pChanged = true;
    }
    bCalledHigherLayer = true;
    strncpy(backupHigherLayer, higherLayer, sizeof(backupHigherLayer));
    strncpy(stMsgData->paramValue, higherLayer, TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);

    return ret;
}

/*
 * Gets the LowerLayer of Interface Stack table for a given instance
 */
/**
 * @brief This function gets the lower layer of the device interface stack.
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 * @param[in] pChanged  Status of the operation.
 *
 * @returns Returns '0' if the method successfully get the lower layer of interface  stack else returns '-1'.
 * @ingroup TR69_HOSTIF_INTERFACESTACK_INTERFACE_API
 */
int hostif_InterfaceStack::get_Device_InterfaceStack_LowerLayer(HOSTIF_MsgData_t *stMsgData, bool *pChanged)
{
    int ret = OK;

    if(bCalledLowerLayer && pChanged && strncmp(backupLowerLayer, lowerLayer, sizeof(backupLowerLayer)))
    {
        *pChanged = true;
    }
    bCalledLowerLayer = true;
    strncpy(backupLowerLayer, lowerLayer, sizeof(backupLowerLayer));
    strncpy(stMsgData->paramValue, lowerLayer, TR69HOSTIFMGR_MAX_PARAM_LEN );
    stMsgData->paramtype = hostIf_StringType;
    stMsgData->paramLen = strlen(stMsgData->paramValue);

    return ret;
}

/* End of doxygen group */
/**
 * @}
 */
