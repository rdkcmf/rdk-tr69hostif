/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2018 RDK Management
 * Copyright 2018 Broadcom Inc.
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
 * @file Device_MoCA_Interface_X_RDKCENTRAL_COM_MeshTable.cpp
 *
 * @brief Device_MoCA_Interface_X_RDKCENTRAL_COM_MeshTable API Implementation.
 *
 * This is the implementation of the Device_MoCA_Interface_X_RDKCENTRAL_COM_MeshTable API.
 *
 * @par Document
 * TBD Relevant design or API documentation.
 *
 */

/** @addtogroup Device_MoCA_Interface_X_RDKCENTRAL-COM_MeshTable Implementation
 *  This is the implementation of the Device Public API.
 *  @{
 */

/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/
/*MoCA include files*/

#include "Device_MoCA_Interface_X_RDKCENTRAL_COM_MeshTable.h"
#include "rdk_moca_hal.h"


#define QUERY_INTERVAL 5

static RMH_NodeMesh_Uint32_t gResponse;
static RMH_NodeMesh_Uint32_t gResBondedConn;
static time_t firstExTime = 0;


X_RDKCENTRAL_COM_MeshTable::X_RDKCENTRAL_COM_MeshTable(int _dev_id):
    backupTxNodeID(0),
    backupRxNodeID(0),
    backupPHYTxRate(0),

    bCalledTxNodeID(false),
    bCalledRxNodeID(false),
    bCalledPHYTxRate(false)
{
    dev_id = _dev_id;
}

GHashTable *X_RDKCENTRAL_COM_MeshTable::ifHash = NULL;

X_RDKCENTRAL_COM_MeshTable* X_RDKCENTRAL_COM_MeshTable::getInstance(int dev_id) {

    X_RDKCENTRAL_COM_MeshTable* instance = NULL;

    if (ifHash)
    {
        instance = (X_RDKCENTRAL_COM_MeshTable*) g_hash_table_lookup(ifHash, (gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL, NULL);
    }

    if (!instance)
    {
        try
        {
            instance = new X_RDKCENTRAL_COM_MeshTable(dev_id);
            g_hash_table_insert(ifHash, (gpointer)dev_id, instance);
        }
        catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create X_RDKCENTRAL_COM_MeshTable instance..\n");
        }
    }

    return instance;
}

void X_RDKCENTRAL_COM_MeshTable::closeInstance(X_RDKCENTRAL_COM_MeshTable *pDev) {
    if(pDev) {
        delete pDev;
    }
}

void X_RDKCENTRAL_COM_MeshTable::closeAllInstances() {}

GList* X_RDKCENTRAL_COM_MeshTable::getAllAssociateDevs() {
    return NULL;
}


/****************************************************************************************************************************************************/
// Device.MoCA.Interface.X_RDKCENTRAL-COM.MeshTable Table Profile. Getters:
/****************************************************************************************************************************************************/

static
bool InstanceToIndex(unsigned int instanceNumber, unsigned int *_i, unsigned int *_j) {
    unsigned int i,j;
    int nodeInstance = 1;

    time_t currExTime = time (NULL);
    if ((currExTime - firstExTime ) > QUERY_INTERVAL) {
        RMH_Handle rmh=RMH_Initialize(NULL, NULL);
        if (rmh) {
            RMH_Result ret = RMH_Network_GetTxUnicastPhyRate(rmh, &gResponse);
            if (ret == RMH_SUCCESS) {
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetTxUnicastPhyRate PHY rates refreshed\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
                firstExTime = time (NULL);
            } else {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%u] RMH_Network_GetTxUnicastPhyRate failed with result %s. Not updating cache.\n", __FUNCTION__, __LINE__,  RMH_ResultToString(ret));
            }
            RMH_Network_GetBondedConnections(rmh, &gResBondedConn);
            RMH_Destroy(rmh);
        }
        else {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s:%u] Failed in RMH_Initialize. Not updating cache.\n", __FUNCTION__, __LINE__);
        }
    }

    for (i = 0; i < RMH_MAX_MOCA_NODES; i++) {
        if (gResponse.nodePresent[i]) {
            for (j = 0; j < RMH_MAX_MOCA_NODES; j++) {
                if (i!=j && gResponse.nodePresent[j]) {
                    if (instanceNumber == nodeInstance++ ) {
                        *_i=i;
                        *_j=j;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}


int X_RDKCENTRAL_COM_MeshTable::get_MeshTxNodeId(HOSTIF_MsgData_t *stMsgData,unsigned int instanceNumber,bool *pChanged) {
    int ret=OK;
    unsigned int i,j;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entry \n", __FILE__, __FUNCTION__);

    if (InstanceToIndex(instanceNumber, &i, &j)) {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] Index %d : MeshTxNode %d : MeshRxNode %d : MeshPHYTxRate %d \n", \
                __FILE__, __FUNCTION__, instanceNumber, i, j, gResponse.nodeValue[i].nodeValue[j]);
        put_int(stMsgData->paramValue, i);
        stMsgData->paramtype = hostIf_UnsignedIntType;
        stMsgData->paramLen = 4;
    }
    else {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] failed to look up instance %d!\n", __FILE__, __FUNCTION__, instanceNumber);
        ret=NOK;
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exit \n", __FILE__, __FUNCTION__);
    return ret;
}


int X_RDKCENTRAL_COM_MeshTable::get_MeshRxNodeId(HOSTIF_MsgData_t *stMsgData,unsigned int instanceNumber,bool *pChanged) {
    int ret=OK;
    unsigned int i,j;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entry \n", __FILE__, __FUNCTION__);

    if (InstanceToIndex(instanceNumber, &i, &j)) {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] Index %d : MeshTxNode %d : MeshRxNode %d : MeshPHYTxRate %d \n", \
                __FILE__, __FUNCTION__, instanceNumber, i, j, gResponse.nodeValue[i].nodeValue[j]);
        put_int(stMsgData->paramValue, j);
        stMsgData->paramtype = hostIf_UnsignedIntType;
        stMsgData->paramLen = 4;
    }
    else {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] failed to look up instance %d!\n", __FILE__, __FUNCTION__, instanceNumber);
        ret=NOK;
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exit \n", __FILE__, __FUNCTION__);
    return ret;
}


int X_RDKCENTRAL_COM_MeshTable::get_MeshPHYTxRate(HOSTIF_MsgData_t *stMsgData,unsigned int instanceNumber,bool *pChanged) {
    int ret=OK;
    unsigned int i,j;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entry \n", __FILE__, __FUNCTION__);

    if (InstanceToIndex(instanceNumber, &i, &j)) {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] Index %d : MeshTxNode %d : MeshRxNode %d : MeshPHYTxRate %d \n", \
                __FILE__, __FUNCTION__, instanceNumber, i, j, gResponse.nodeValue[i].nodeValue[j]);
        put_int(stMsgData->paramValue, gResponse.nodeValue[i].nodeValue[j]);
        stMsgData->paramtype = hostIf_UnsignedIntType;
        stMsgData->paramLen = 4;
    }
    else {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] failed to look up instance %d!\n", __FILE__, __FUNCTION__, instanceNumber);
        ret=NOK;
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exit \n", __FILE__, __FUNCTION__);
    return ret;
}

int X_RDKCENTRAL_COM_MeshTable::get_BondedChannel(HOSTIF_MsgData_t *stMsgData,unsigned int instanceNumber,bool *pChanged) {
    int ret=OK;
    unsigned int i,j;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entry \n", __FILE__, __FUNCTION__);

    if (InstanceToIndex(instanceNumber, &i, &j)) {
        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] Index %d : MeshTxNode %d : MeshRxNode %d : MeshPHYTxRate %d \n", \
                __FILE__, __FUNCTION__, instanceNumber, i, j, gResponse.nodeValue[i].nodeValue[j]);
        put_int(stMsgData->paramValue, gResBondedConn.nodeValue[i].nodeValue[j]);
        stMsgData->paramtype = hostIf_UnsignedIntType;
        stMsgData->paramLen = 4;
    }
    else {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] failed to look up instance %d!\n", __FILE__, __FUNCTION__, instanceNumber);
        ret=NOK;
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exit \n", __FILE__, __FUNCTION__);
    return ret;
}
/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */
