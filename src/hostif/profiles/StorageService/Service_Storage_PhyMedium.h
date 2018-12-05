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
 /*
*   FileName: Service_Storage_PhyMedium.h
*
*   Purpose : This class is used to implement Physical Medium DM
*
*/
#ifndef _SERVICE_STORAGE_PHYMED_H_
#define _SERVICE_STORAGE_PHYMED_H_
#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "hostIf_updateHandler.h"
#define MAX_PHY_MED_ALIAS_LEN 64
#define MAX_PHY_MED_NAME_LEN 64
#define MAX_PHY_MED_VENDOR_LEN 64
#define MAX_PHY_MED_MODEL_LEN 128
#define MAX_PHY_MED_SERIAL_NUM_LEN 64
#define MAX_PHY_MED_FIRMWARE_VER_LEN 64
#define MAX_PHY_MED_CONNECTION_TYPE_LEN 64
#define MAX_PHY_MED_STATUS_LEN 64
#define MAX_PHY_MED_HEALTH_LEN 64
typedef struct physicalMediumMembers
{
    char alias[MAX_PHY_MED_ALIAS_LEN];
    char name[MAX_PHY_MED_NAME_LEN];
    char vendor[MAX_PHY_MED_VENDOR_LEN];
    char model[MAX_PHY_MED_MODEL_LEN];
    char serialNumber[MAX_PHY_MED_SERIAL_NUM_LEN];
    char firmwareVersion[MAX_PHY_MED_FIRMWARE_VER_LEN];
    char connectionType[MAX_PHY_MED_CONNECTION_TYPE_LEN];
    bool removable;
    unsigned int capacity;
    char status[MAX_PHY_MED_STATUS_LEN];
    unsigned int uptime;
    bool smartCapable;
    char health[MAX_PHY_MED_HEALTH_LEN];
    bool hotSwappable;
}PhysicalMediumMembers_t;

typedef enum 
{
    eAlias,
    eName,
    eVendor,
    eModel,
    eSerialNumber,
    eFirmwareVersion,
    eConnectionType,
    eRemovable,
    eCapacity,
    eStatus,
    eUptime,
    eSmartCapable,
    eHealth,
    eHotSwappable
}ePhysicalMediumMembers_t;


class hostIf_PhysicalMedium
{
    
    static  GHashTable  *phyMedHash;

    static GMutex *m_mutex;

    int dev_id;

    static PhysicalMediumMembers_t physicalMediumMembers;

    int get_StorageService_PhyMed_Fields(ePhysicalMediumMembers_t phyMedMember);

    hostIf_PhysicalMedium(int StorageServiceInstanceNumber, int dev_id);
    
    int getMediumName(char* mediumName);
    
    bool isSmartCapable();

    int isMediumHealthOK();

    static int rebuildHash();

    static int getPhysicalMediumNumberOfEntries(int storageServiceInstanceNumber);

    int storageServiceInstanceNumber;

    public:
        
        static hostIf_PhysicalMedium* getInstance(int storageService, int dev_id);

        static void closeInstance(hostIf_PhysicalMedium*);

        static GList* getAllInstances();

        static void closeAllInstances();

        static int get_Device_Service_StorageMedium_ClientNumberOfEntries(HOSTIF_MsgData_t *stMsgData, int dev_id);

        static int updateCallbackFunction(updateCallback cb);

        int get_Device_Service_StorageMedium_Alias(HOSTIF_MsgData_t *stMsgData);

        int get_Device_Service_StorageMedium_Name(HOSTIF_MsgData_t *stMsgData);

        int get_Device_Service_StorageMedium_Vendor(HOSTIF_MsgData_t *stMsgData);

        int get_Device_Service_StorageMedium_Model(HOSTIF_MsgData_t *stMsgData);

        int get_Device_Service_StorageMedium_SerialNumber(HOSTIF_MsgData_t *stMsgData);

        int get_Device_Service_StorageMedium_FirmwareVersion(HOSTIF_MsgData_t *stMsgData);

        int get_Device_Service_StorageMedium_ConnectionType(HOSTIF_MsgData_t *stMsgData);

        int get_Device_Service_StorageMedium_Removable(HOSTIF_MsgData_t *stMsgData);

        int get_Device_Service_StorageMedium_Status(HOSTIF_MsgData_t *stMsgData);

        int get_Device_Service_StorageMedium_Uptime(HOSTIF_MsgData_t *stMsgData);

        int get_Device_Service_StorageMedium_SMARTCapable(HOSTIF_MsgData_t *stMsgData);

        int get_Device_Service_StorageMedium_Health(HOSTIF_MsgData_t *stMsgData);

        int get_Device_Service_StorageMedium_HotSwappable(HOSTIF_MsgData_t *stMsgData);

        void getLock();

        void releaseLock();
};
#endif
