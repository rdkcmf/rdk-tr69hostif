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
 * @file Service_Storage_PhyMedium.cpp
 * @brief This source file contains the APIs of TR069 storage service physical medium.
 */
#include <string.h>
#include <errno.h>
#include "Service_Storage_PhyMedium.h"
#include "Service_Storage.h"

#define MAX_CMD_LEN 256
#define MAX_BUF_LEN 256
#define MAX_IP_LEN 16
#define MAX_MEDIUM_LEN 64
#define CMD_TO_GET_MED_NUM "fdisk -l | grep Disk | egrep -v \"mtdblock|mmcblk\"| wc -l"
#define CMD_TO_GET_MED_NAME "fdisk -l | grep Disk | egrep -v \"mtdblock|mmcblk\"| sed -n %dp | awk '{print $2}'"
#define CMD_TO_CHECK_SMART_CAP "smartctl --scan | awk 'BEGIN {ORS=\",\"}{ print $1}'"
#define CMD_TO_CHECK_SMART_HEALTH "smartctl -A %s | egrep \"%s\" | awk 'BEGIN {ORS=\",\"} {print $9}'"
#define STORAGE_PHYMED_SMARTPARAMS "Raw_Read_Error_Rate|\
Reported_Uncorrect|\
Airflow_Temperature_Cel|\
G-Sense_Error_Rate|\
Reallocated_Sector_Ct|\
Temperature_Celsius"


PhysicalMediumMembers_t hostIf_PhysicalMedium::physicalMediumMembers = {{'\0'}, {'\0'}, {'\0'}, {'\0'}, {'\0'}, {'\0'}, {'\0'}, false, 0, {'\0'}, 0, false, {'\0'}, false};
GHashTable *hostIf_PhysicalMedium::phyMedHash = NULL;
GMutex *hostIf_PhysicalMedium::m_mutex = NULL;

/**
 * @enum ePhyMediumErrorCode_t
 * @brief The enum holds the required error code parameters such as command execution fail, invalid
 * param etc.. for the error code physical medium storage service.
 */
typedef enum 
{
   PHYMED_GEN_FAILURE = -1, 
   PHYMED_CMD_EXEC_FAIL = -2,
   PHYMED_INVALID_PARAM = -3
}ePhyMediumErrorCode_t;

/**
 * @enum ePhyMedHealthCode_t
 * @brief The enum holds the required health code parameters such as health invalid, health ok etc..
 * for the health code physical medium storage service.
 */
typedef enum
{
    PHYMED_HEALTH_INVALID = 100,
    PHYMED_HEALTH_OK = 101,
    PHYMED_HEALTH_FAILING = 102,
    PHYMED_HEALTH_ERROR = 103,
}ePhyMedHealthCode_t;

/* Constructor for hostIf_PhysicalMedium*/

/**
 * @brief Class Constructor of the class hostIf_PhysicalMedium.
 *
 * It will initialize the device id and instance number of the storage service.
 *
 * @param[in] storageServiceInstanceNumber Instance number of the storage service.
 * @param[in] dev_id Device identification number.
 */
hostIf_PhysicalMedium::hostIf_PhysicalMedium(int storageServiceInstanceNumber, int dev_id):storageServiceInstanceNumber(storageServiceInstanceNumber),dev_id(dev_id) 
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Inside constructor for dev_id:%d\n", dev_id);
}

/**
 * @brief This function get the lock before setting or getting the attributes of the
 * host interface physical medium.
 * @ingroup TR69_HOSTIF_STORAGE_PHYSICALMEDIUM_API
 */
void hostIf_PhysicalMedium::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

/**
 * @brief This function use to release the lock before setting or getting the
 * attributes of the host interface physical medium.
 * @ingroup TR69_HOSTIF_STORAGE_PHYSICALMEDIUM_API
 */
void hostIf_PhysicalMedium::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

int hostIf_PhysicalMedium::rebuildHash()
{
    HOSTIF_MsgData_t stMsgData;
    int storageServiceMaxInstance =0;
    int phyMedMaxInstance = 0;
    int retVal = OK;
    hostIf_PhysicalMedium* pRet = NULL;


    if(phyMedHash)
    {
        closeAllInstances();
    }
    else
    {
        phyMedHash = g_hash_table_new(NULL, NULL);
    }
    
    memset(&stMsgData, 0 ,sizeof(stMsgData));
    if(OK == hostIf_StorageSrvc :: get_Device_StorageSrvc_ClientNumberOfEntries(&stMsgData))
    {
        memcpy(&storageServiceMaxInstance ,stMsgData.paramValue,sizeof(int));
        for(int storageServiceInstance = 1; storageServiceInstance<= storageServiceMaxInstance; storageServiceInstance++)
        {
            memset(&stMsgData, 0 ,sizeof(stMsgData));
            if((phyMedMaxInstance = getPhysicalMediumNumberOfEntries (storageServiceInstance))!=0)
            {
                for( int phyMedInstance = 1; phyMedInstance<= phyMedMaxInstance; phyMedInstance++)
                {
                    try 
                    {
                        pRet = new hostIf_PhysicalMedium(storageServiceInstance, phyMedInstance);
                        g_hash_table_insert(phyMedHash, (gpointer)((storageServiceInstance * 100 ) + phyMedInstance), pRet);
                    } 
                    catch(int e)
                    {
                        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create physical medium instance..\n");
                    }
                }
            }
            else
            {
                retVal = NOK;
            }
        }
    }
    else
    {
        retVal = NOK;
    }
    return retVal;
}

hostIf_PhysicalMedium* hostIf_PhysicalMedium::getInstance(int storageServiceInstanceNumber, int dev_id) 
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Entering [%s]\n", __FUNCTION__);
    int maxInstance = 0;
    HOSTIF_MsgData_t stMsgData;
    hostIf_PhysicalMedium* pRet = NULL;
    if(NULL == phyMedHash)
    {
        rebuildHash();
    }
    pRet = (hostIf_PhysicalMedium *)g_hash_table_lookup(phyMedHash, (gpointer)  ((storageServiceInstanceNumber * 100) + dev_id));
    return pRet;
}


GList* hostIf_PhysicalMedium::getAllInstances()
{
    if(phyMedHash)
        return g_hash_table_get_keys(phyMedHash);
    return NULL;
}


void hostIf_PhysicalMedium::closeInstance(hostIf_PhysicalMedium *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(phyMedHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}


void hostIf_PhysicalMedium::closeAllInstances()
{
    if(phyMedHash)
    {
        GList* tmp_list = g_hash_table_get_values (phyMedHash);

        while(tmp_list)
        {
           hostIf_PhysicalMedium* pDev = (hostIf_PhysicalMedium *)tmp_list->data;
           tmp_list = tmp_list->next;
           closeInstance(pDev);
        }
    }
}


int hostIf_PhysicalMedium :: getPhysicalMediumNumberOfEntries(int StorageServiceId)
{
    int num = 0;
    char cmd[MAX_CMD_LEN]={'\0'};
    FILE* cmdOp = NULL;
    char buffer[MAX_BUF_LEN]={'\0'};

    if (1 == StorageServiceId) 
    {
        snprintf(cmd, MAX_CMD_LEN, CMD_TO_GET_MED_NUM);
        cmdOp = popen(cmd, "r");
        if(NULL != cmdOp)
        {
            fgets(buffer, MAX_BUF_LEN, cmdOp);
            num = strtol(buffer,NULL,10); 
            pclose(cmdOp);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error in execuing the command:%s errno:%d\n", cmd, errno);
        }
    }
    return num;

}

int hostIf_PhysicalMedium:: get_Device_Service_StorageMedium_ClientNumberOfEntries(HOSTIF_MsgData_t *stMsgData, int dev_id)
{

    int num = 0;
    int retVal = NOK;
    errno = 0;
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"Received dev_id:%d\n",dev_id);
    num = getPhysicalMediumNumberOfEntries(dev_id);
    put_int(stMsgData->paramValue, num);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(unsigned int);
    retVal = OK;
    if(OK != rebuildHash())
    {
        retVal = NOK;
    }
    return retVal;
}

/**
 * @brief This function get the name of the storage service medium.
 *
 * @param[in] stMsgData  HostIf Message Request param contains the storage medium attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hostIf storage medium interface attribute.
 * @retval -1 If Not able to get the hostIf storage medium interface attribute.
 * @retval -2 If Not handle the hostIf storage medium interface attribute.
 * @ingroup TR69_HOSTIF_STORAGE_PHYSICALMEDIUM_API
 */
int hostIf_PhysicalMedium :: get_Device_Service_StorageMedium_Name(HOSTIF_MsgData_t *stMsgData)
{
    int ret = get_StorageService_PhyMed_Fields(eName);
    if(OK == ret)
    {
        strncpy(stMsgData->paramValue, physicalMediumMembers.name, TR69HOSTIFMGR_MAX_PARAM_LEN -1);
        stMsgData->paramValue[TR69HOSTIFMGR_MAX_PARAM_LEN -1]='\0';
        stMsgData->paramLen=strlen(physicalMediumMembers.name);
        stMsgData->paramtype = hostIf_StringType;
    } 
    return ret;
}

/**
 * @brief This function get the health of the storage service medium.
 *
 * @param[in] stMsgData  HostIf Message Request param contains the storage medium attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hostIf storage medium interface attribute.
 * @retval -1 If Not able to get the hostIf storage medium interface attribute.
 * @retval -2 If Not handle the hostIf storage medium interface attribute.
 * @ingroup TR69_HOSTIF_STORAGE_PHYSICALMEDIUM_API
 */
int hostIf_PhysicalMedium :: get_Device_Service_StorageMedium_Health(HOSTIF_MsgData_t *stMsgData)
{
    int ret = get_StorageService_PhyMed_Fields(eHealth);
    if(OK == ret)
    {
        strncpy(stMsgData->paramValue, physicalMediumMembers.health, TR69HOSTIFMGR_MAX_PARAM_LEN -1);
        stMsgData->paramValue[TR69HOSTIFMGR_MAX_PARAM_LEN -1]='\0';
        stMsgData->paramLen=strlen(physicalMediumMembers.health);
        stMsgData->paramtype = hostIf_StringType;
    } 
    return ret;
}

/**
 * @brief This function get the SMART capability of the storage service medium.
 *
 * @param[in] stMsgData  HostIf Message Request param contains the storage medium attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hostIf storage medium interface attribute.
 * @retval -1 If Not able to get the hostIf storage medium interface attribute.
 * @retval -2 If Not handle the hostIf storage medium interface attribute.
 * @ingroup TR69_HOSTIF_STORAGE_PHYSICALMEDIUM_API
 */
int hostIf_PhysicalMedium :: get_Device_Service_StorageMedium_SMARTCapable(HOSTIF_MsgData_t *stMsgData)
{
    int ret = get_StorageService_PhyMed_Fields(eSmartCapable);
    if(OK == ret)
    {
        put_int(stMsgData->paramValue, physicalMediumMembers.smartCapable);
        stMsgData->paramLen=sizeof(bool);
        stMsgData->paramtype = hostIf_BooleanType;
    } 
    return ret;
}

/**
 * @brief This function get the alias of storage service medium.
 * Currently not implemented.
 *
 * @param[in] stMsgData  HostIf Message Request param contains the storage medium attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hostIf storage medium interface attribute.
 * @retval -1 If Not able to get the hostIf storage medium interface attribute.
 * @retval -2 If Not handle the hostIf storage medium interface attribute.
 * @ingroup TR69_HOSTIF_STORAGE_PHYSICALMEDIUM_API
 */
int hostIf_PhysicalMedium :: get_Device_Service_StorageMedium_Alias(HOSTIF_MsgData_t *stMsgData)
{
    // TODO
    return NOK;
}

/**
 * @brief This function get the vendor of the storage service medium.
 * Currently not implemented.
 *
 * @param[in] stMsgData  HostIf Message Request param contains the storage medium attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hostIf storage medium interface attribute.
 * @retval -1 If Not able to get the hostIf storage medium interface attribute.
 * @retval -2 If Not handle the hostIf storage medium interface attribute.
 * @ingroup TR69_HOSTIF_STORAGE_PHYSICALMEDIUM_API
 */
int hostIf_PhysicalMedium :: get_Device_Service_StorageMedium_Vendor(HOSTIF_MsgData_t *stMsgData)
{
    // TODO
    return NOK;
}

/**
 * @brief This function get the model number of the storage service medium.
 * Currently not implemented.
 *
 * @param[in] stMsgData  HostIf Message Request param contains the storage medium attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hostIf storage medium interface attribute.
 * @retval -1 If Not able to get the hostIf storage medium interface attribute.
 * @retval -2 If Not handle the hostIf storage medium interface attribute.
 * @ingroup TR69_HOSTIF_STORAGE_PHYSICALMEDIUM_API
 */
int hostIf_PhysicalMedium :: get_Device_Service_StorageMedium_Model(HOSTIF_MsgData_t *stMsgData)
{
    // TODO
    return NOK;
}

/**
 * @brief This function get the serial number of the storage service medium.
 * Currently not implemented.
 *
 * @param[in] stMsgData  HostIf Message Request param contains the storage medium attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hostIf storage medium interface attribute.
 * @retval -1 If Not able to get the hostIf storage medium interface attribute.
 * @retval -2 If Not handle the hostIf storage medium interface attribute.
 * @ingroup TR69_HOSTIF_STORAGE_PHYSICALMEDIUM_API
 */
int hostIf_PhysicalMedium :: get_Device_Service_StorageMedium_SerialNumber(HOSTIF_MsgData_t *stMsgData)
{
    // TODO
    return NOK;
}

/**
 * @brief This function get the firmware version of the storage service medium.
 * Currently not implemented.
 *
 * @param[in] stMsgData  HostIf Message Request param contains the storage medium attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hostIf storage medium interface attribute.
 * @retval -1 If Not able to get the hostIf storage medium interface attribute.
 * @retval -2 If Not handle the hostIf storage medium interface attribute.
 * @ingroup TR69_HOSTIF_STORAGE_PHYSICALMEDIUM_API
 */
int hostIf_PhysicalMedium :: get_Device_Service_StorageMedium_FirmwareVersion(HOSTIF_MsgData_t *stMsgData)
{
    // TODO
    return NOK;
}

/**
 * @brief This function get the connection type of the storage service medium.
 * Currently not implemented.
 *
 * @param[in] stMsgData  HostIf Message Request param contains the storage medium attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hostIf storage medium interface attribute.
 * @retval -1 If Not able to get the hostIf storage medium interface attribute.
 * @retval -2 If Not handle the hostIf storage medium interface attribute.
 * @ingroup TR69_HOSTIF_STORAGE_PHYSICALMEDIUM_API
 */
int hostIf_PhysicalMedium :: get_Device_Service_StorageMedium_ConnectionType(HOSTIF_MsgData_t *stMsgData)
{
    // TODO
    return NOK;
}

/**
 * @brief This function get the storage service medium removable.
 * Currently not implemented.
 *
 * @param[in] stMsgData  HostIf Message Request param contains the storage medium attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hostIf storage medium interface attribute.
 * @retval -1 If Not able to get the hostIf storage medium interface attribute.
 * @retval -2 If Not handle the hostIf storage medium interface attribute.
 * @ingroup TR69_HOSTIF_STORAGE_PHYSICALMEDIUM_API
 */
int hostIf_PhysicalMedium :: get_Device_Service_StorageMedium_Removable(HOSTIF_MsgData_t *stMsgData)
{
    // TODO
    return NOK;
}

/**
 * @brief This function get the status of the storage service medium.
 * Currently not implemented.
 *
 * @param[in] stMsgData  HostIf Message Request param contains the storage medium attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hostIf storage medium interface attribute.
 * @retval -1 If Not able to get the hostIf storage medium interface attribute.
 * @retval -2 If Not handle the hostIf storage medium interface attribute.
 * @ingroup TR69_HOSTIF_STORAGE_PHYSICALMEDIUM_API
 */
int hostIf_PhysicalMedium :: get_Device_Service_StorageMedium_Status(HOSTIF_MsgData_t *stMsgData)
{
    // TODO
    return NOK;
}

/**
 * @brief This function get the uptime of the storage service medium.
 * Currently not implemented.
 *
 * @param[in] stMsgData  HostIf Message Request param contains the storage medium attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hostIf storage medium interface attribute.
 * @retval -1 If Not able to get the hostIf storage medium interface attribute.
 * @retval -2 If Not handle the hostIf storage medium interface attribute.
 * @ingroup TR69_HOSTIF_STORAGE_PHYSICALMEDIUM_API
 */
int hostIf_PhysicalMedium :: get_Device_Service_StorageMedium_Uptime(HOSTIF_MsgData_t *stMsgData)
{
    // TODO
    return NOK;
}

/**
 * @brief This function get the storage service medium hot swappable.
 * Currently not implemented.
 *
 * @param[in] stMsgData  HostIf Message Request param contains the storage medium attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hostIf storage medium interface attribute.
 * @retval -1 If Not able to get the hostIf storage medium interface attribute.
 * @retval -2 If Not handle the hostIf storage medium interface attribute.
 * @ingroup TR69_HOSTIF_STORAGE_PHYSICALMEDIUM_API
 */
int hostIf_PhysicalMedium :: get_Device_Service_StorageMedium_HotSwappable(HOSTIF_MsgData_t *stMsgData)
{
    // TODO
    return NOK;
}

int hostIf_PhysicalMedium::get_StorageService_PhyMed_Fields(ePhysicalMediumMembers_t phyMedMember)
{
    FILE* cmdOP;
    int ret=NOK;
    int len;
    char mediumName[MAX_MEDIUM_LEN]={'\0'};
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
    ret=getMediumName(mediumName);
    if(ret!=OK)
        return ret;


    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Entering [%s] asking for member %d\n", __FUNCTION__, phyMedMember);
    switch(phyMedMember)
    {
        case eName:
            strncpy(physicalMediumMembers.name,mediumName,strlen(mediumName));
            break;
        case eSmartCapable:
            physicalMediumMembers.smartCapable = isSmartCapable();
            break;
        case eHealth:
            if(!isSmartCapable())
            {
                snprintf(physicalMediumMembers.health, MAX_PHY_MED_HEALTH_LEN, "Error");
            }
            else
            {
                int health = isMediumHealthOK();
                switch(health)
                {
                    case PHYMED_HEALTH_OK: 
                        snprintf(physicalMediumMembers.health, MAX_PHY_MED_HEALTH_LEN, "OK");
                        break;
                    case PHYMED_HEALTH_FAILING: 
                        snprintf(physicalMediumMembers.health, MAX_PHY_MED_HEALTH_LEN, "Failing");
                        break;
                    case PHYMED_HEALTH_ERROR: 
                        snprintf(physicalMediumMembers.health, MAX_PHY_MED_HEALTH_LEN, "Error");
                        break;
                    default:
                        ret=NOK;
                        break;
                }
            }
            break;
        default:
            ret=NOK;
    }
    return ret;
}

int hostIf_PhysicalMedium::getMediumName(char* mediumName)
{
    FILE* cmdOp = NULL;
    char cmd[MAX_CMD_LEN]= {'\0'};
    char buffer[MAX_BUF_LEN] = {'\0'};
    int retVal = NOK;
    errno = 0;
    if(1 == storageServiceInstanceNumber)
    {
        snprintf(cmd, MAX_CMD_LEN, CMD_TO_GET_MED_NAME, this->dev_id);
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"Executing command:%s\n",cmd);
        cmdOp = popen(cmd,"r");
        if (NULL!= cmdOp)
        {
            fgets(buffer,MAX_BUF_LEN,cmdOp);
            buffer[strlen(buffer)-2]='\0'; // -2 because the buffer has \n as the penaltimate character. Here we replace it with \0
            strncpy(mediumName,buffer,strlen(buffer));
            pclose(cmdOp);
            retVal = OK;
        }
        else
        {
            retVal = PHYMED_CMD_EXEC_FAIL;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error in execuing the command:%s errno:%d\n", cmd, errno);
        }
    }
    return retVal;
}

bool hostIf_PhysicalMedium::isSmartCapable()
{
    FILE* cmdOp = NULL;
    char cmd[MAX_CMD_LEN]= {'\0'};
    char buffer[MAX_BUF_LEN] = {'\0'};
    char mediumName[MAX_PHY_MED_NAME_LEN] = {'\0'};
    char *token=NULL;
    char *savePtr=NULL;
    bool smartCapable = false;
    errno = 0;

    if (OK == getMediumName(mediumName))
    {
        if(1 == storageServiceInstanceNumber)
        {
            snprintf(cmd,MAX_CMD_LEN,CMD_TO_CHECK_SMART_CAP);
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"Executing command:%s\n",cmd);
            cmdOp = popen(cmd,"r");
            if(NULL != cmdOp)
            {
                fgets(buffer,MAX_BUF_LEN,cmdOp);
                token = strtok_r(buffer,",",&savePtr);
                if(NULL!= token)
                {
                    do{
                        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s\n",token);
                        if(strcmp(token,mediumName)==0)
                        {
                            smartCapable = true;
                            break;
                        }
                    }while((token=strtok_r(NULL,",",&savePtr )));
                }
                pclose(cmdOp);
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error in execuing the command:%s errno:%d\n", cmd, errno);
            }
        }
    }
    return smartCapable;    
}

int hostIf_PhysicalMedium :: isMediumHealthOK()
{
    FILE* cmdOp = NULL;
    char cmd[MAX_CMD_LEN]= {'\0'};
    char buffer[MAX_BUF_LEN] = {'\0'};
    char mediumName[MAX_PHY_MED_NAME_LEN] = {'\0'};
    char *token=NULL;
    char *savePtr=NULL;
    int health = PHYMED_HEALTH_INVALID;
    errno = 0;

    if(OK == getMediumName(mediumName))
    {
        if( 1 == storageServiceInstanceNumber)
        {
            snprintf(cmd,MAX_CMD_LEN, CMD_TO_CHECK_SMART_HEALTH ,mediumName, STORAGE_PHYMED_SMARTPARAMS);
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"Executing command:%s\n",cmd);
            cmdOp = popen(cmd,"r");
            if(NULL != cmdOp)
            {
                fgets(buffer,MAX_BUF_LEN,cmdOp);
                token = strtok_r(buffer,",",&savePtr);
                if(NULL!= token)
                {
                    do{
                        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s\n",token);
                        if(strcmp(token,"-")==0)
                        {
                            health = (health <=PHYMED_HEALTH_OK)?PHYMED_HEALTH_OK:health;
                        }
                        if(strcmp(token,"FAILING_NOW")==0)
                        {
                            health = (health<PHYMED_HEALTH_FAILING)?PHYMED_HEALTH_FAILING:health;
                        }
                        if(strcmp(token,"In_the_past")==0)
                        {
                            health = PHYMED_HEALTH_ERROR;
                        }
                    }while((token=strtok_r(NULL,",",&savePtr )));
                }
                else
                {
                    health=PHYMED_INVALID_PARAM;
                    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error in parsing output\n");
                }
                pclose(cmdOp);
            }
            else
            {
                health = PHYMED_CMD_EXEC_FAIL;
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error in executing Command:%d \n",errno);
            }
        }
    }
    return health;    
}
