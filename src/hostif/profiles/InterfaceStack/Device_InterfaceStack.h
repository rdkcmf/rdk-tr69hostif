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
 * @file Device_InterfaceStack.h
 * @brief The header file provides TR069 device interface stack information APIs.
 */

/**
 * @defgroup TR69_HOSTIF_INTERFACESTACK_INTERFACE TR-069 Object (Device.InterfaceStack.{i})
 *  The Device.InterfaceStack table contains information about the relationships between
 *  the multiple layers of interface objects.  In particular, it contains information on
 *  which interfaces run on top of which other interfaces.
 *
 *  This table is auto-generated by the CPE based on the LowerLayers parameters on
 *  individual interface objects.
 *
 *  Each table row represents a "link" between two interface objects, a higher-layer
 *  interface object (referenced by HigherLayer) and a lower-layer interface object
 *  (referenced by LowerLayer). Consequently, if a referenced interface object is
 *  deleted, the CPE MUST delete the corresponding InterfaceStack row(s) that had
 *  referenced it.
 *
 * @note    At most one entry in this table can exist with the same values for
 *          HigherLayer and LowerLayer.
 *
 * @ingroup TR69_HOSTIF_PROFILE
 *
 * @defgroup TR69_HOSTIF_INTERFACESTACK_INTERFACE_API TR-069 Object (Device.InterfaceStack.{i}) Public APIs
 * Describe the details about TR-069 Device interface stack  APIs specifications.
 * @ingroup TR69_HOSTIF_INTERFACESTACK_INTERFACE
 *
 * @defgroup TR69_HOSTIF_INTERFACESTACK_INTERFACE_CLASSES TR-069 Object (Device.InterfaceStack.{i}) Public Classes
 * Describe the details about classes used in TR069 Device interface stack.
 * @ingroup TR69_HOSTIF_INTERFACESTACK_INTERFACE
 *
 * @defgroup TR69_HOSTIF_INTERFACESTACK_INTERFACE_DSSTRUCT TR-069 Object (Device.InterfaceStack.{i}) Public DataStructure
 * Describe the details about structure used in TR069 Device interface stack.
 * @ingroup TR69_HOSTIF_INTERFACESTACK_INTERFACE
 *
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef DEVICE_INTERFACESTACK_H_
#define DEVICE_INTERFACESTACK_H_

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/
#ifdef USE_INTFSTACK_PROFILE
/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/
#include <string>
#include <map>
#include <list>

#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"

/**
 * The Device.InterfaceStack table contains information about the relationships between
 * the multiple layers of interface objects.  In particular, it contains information on
 * which interfaces run on top of which other interfaces.
 *
 * This table is auto-generated by the CPE based on the LowerLayers parameters on
 * individual interface objects.
 *
 * Each table row represents a "link" between two interface objects, a higher-layer
 * interface object (referenced by HigherLayer) and a lower-layer interface object
 * (referenced by LowerLayer). Consequently, if a referenced interface object is
 * deleted, the CPE MUST delete the corresponding InterfaceStack row(s) that had
 * referenced it.
 *
 * @note    At most one entry in this table can exist with the same values for
 *          HigherLayer and LowerLayer.
 *
 */

#define MAX_HIGHERLAYER_LEN 256
#define MAX_LOWERLAYER_LEN 256

/**
 * @addtogroup TR69_HOSTIF_INTERFACESTACK_INTERFACE_DSSTRUCT
 * @{
 */
/**
 * @enum EInterfaceStackMembers
 * @brief These values are the members of the EInterfaceStackMembers.
 */
typedef enum EDeviceInterfaceStackMembers
{
    eHigherLayer,
    eLowerLayer
}EInterfaceStackMembers;

/**
 * @brief It contains the members variables of the LayerInfo_t structure.
 */
typedef struct 
{
    std::string higherLayer;
    std::string lowerLayer;
} LayerInfo_t;
/** @} */ //End of the Doxygen tag TR69_HOSTIF_INTERFACESTACK_INTERFACE_DSSTRUCT

typedef std::multimap<std::string, LayerInfo_t> InterfaceStackMap_t;
typedef std::map<std::string, int> IPInterfacesMap_t;

/**
 * @brief This class provides the interface for getting Device interface stack information.
 * @ingroup TR69_HOSTIF_INTERFACESTACK_INTERFACE_CLASSES
 */
class hostif_InterfaceStack {
    int     dev_id;

    /*
     * Holds interface stack class instances
     */
    static  GHashTable  *stIshash;

    /*
     * Holds Notification Hash table
     */
    static  GHashTable  *m_notifyHash;

    /*
    * Contains the details about the bridges and its related bridge interfaces in CPE
    *
    * Key is the bridge name
    * Value is the comma separated bride interfaces
    */
    static  GHashTable  *stBridgeTableHash;

    static  GMutex *stMutex;

    char    higherLayer[MAX_HIGHERLAYER_LEN];
    char    lowerLayer[MAX_LOWERLAYER_LEN];

    bool    bCalledHigherLayer;
    bool    bCalledLowerLayer;

    char    backupHigherLayer[MAX_HIGHERLAYER_LEN];
    char    backupLowerLayer[MAX_LOWERLAYER_LEN];

    hostif_InterfaceStack(int dev_id, char *higherLayer, char *lowerLayer);
    ~hostif_InterfaceStack();
    static int createInstance(int dev_id, char *higherLayer, char *lowerLayer);
    static int populateBridgeTable();
    static void deleteBridgeTable();
    static int insertRowIntoBridgeTable(char *bridge, char *bridgeInterfaces);
    static void bridgeInteface_key_data_free(gpointer key);
    static void  bridgeInteface_value_data_free(gpointer value);

    template<typename T> static int getLowerInterfaceNumberOfEntries();
    template<typename T> static std::string getInterfaceName(T* pIface);
    template<typename T> static std::string getLowerLayerName(int index); 
    template<typename T> static int buildLowerLayerInfo(InterfaceStackMap_t &layerInfo);
    static int buildBridgeTableLayerInfo(InterfaceStackMap_t &layerInfo);

    static std::list<std::string> getBridgeElements(char* elementsCSV);
    static std::string getDMForBridgePort(int bridgeNum, int portNum);
    static void addBridgeNameLayerInfo(InterfaceStackMap_t &layerInfo, std::string ifname, std::string bridgeLowerLayer);
    static int addBridgeChildLayerInfo(InterfaceStackMap_t &layerInfo, std::string ifname, std::string bridgeHigherLayer);
    static void addBridgeUnmanagedLayerInfo(InterfaceStackMap_t &layerInfo, std::string ifname, std::string bridgeHigherLayer, std::string bridgeLowerLayer);
    static int getIPInterfaces(IPInterfacesMap_t& interfaceList);
    static std::string getDMForIPLayer(int portNum);
    static void fillHigherLayersWithIP(InterfaceStackMap_t &layerInfo, IPInterfacesMap_t& ipInterfaceMap);
    static void print_map(InterfaceStackMap_t &layerInfo);

    public:
        static hostif_InterfaceStack* getInstance(int dev_id);
        static GList* getAllInstances();
        static void closeInstance(hostif_InterfaceStack *pDev);
        static void closeAllInstances();
        static int get_Device_InterfaceStackNumberOfEntries(HOSTIF_MsgData_t *stMsgData);
        int get_Device_InterfaceStack_HigherLayer(HOSTIF_MsgData_t *stMsgData,bool *pChanged = NULL);
        int get_Device_InterfaceStack_LowerLayer(HOSTIF_MsgData_t *stMsgData,bool* pChanged = NULL);
        static void getLock();
        static void releaseLock();
        static GHashTable* getNotifyHash();

};
/* End of TR_069_DEVICE_INTERFACESTACK_GETTER_API doxygen group */
/**
 * @}
 */

#endif /* USE_INTFSTACK_PROFILE */

#endif /* DEVICE_INTERFACESTACK_H_ */


/** @} */
/** @} */
