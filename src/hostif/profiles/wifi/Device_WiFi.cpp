/*
* ============================================================================
* RDK MANAGEMENT, LLC CONFIDENTIAL AND PROPRIETARY
* ============================================================================
* This file (and its contents) are the intellectual property of RDK Management, LLC.
* It may not be used, copied, distributed or otherwise  disclosed in whole or in
* part without the express written permission of RDK Management, LLC.
* ============================================================================
* Copyright (c) 2016 RDK Management, LLC. All rights reserved.
* ============================================================================
*/

#ifdef USE_WIFI_PROFILE

/**
 * @file Device_WiFi.c
 *
 * @brief MoCA_Interface API Implementation.
 *
 * This is the implementation of the MoCA_Interface API.
 *
 * @par Document
 * TBD Relevant design or API documentation.
 *
 */

/** @addtogroup MoCA_Interface Implementation
 *  This is the implementation of the Device Public API.
 *  @{
 */

/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/
#include "Device_WiFi.h"
#include <string.h>
#include <math.h>

extern "C" {
    /* #include "c_only_header.h"*/
#include "wifi_client_hal.h"
#include "wifiSrvMgrIarmIf.h"
#include "netsrvmgrIarm.h"
};

//char *moca_interface = NULL;
GHashTable* WiFiDevice::devHash = NULL;
GHashTable* hostIf_WiFi::ifHash = NULL;

WiFiDevice::WiFiDevice(int dev_id):dev_id(dev_id)
{
//    ctxt = WiFiCtl_Open(interface);

    if(!ctxt)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error!  Unable to connect to WiFi Device instance %d\n",dev_id);
        throw 1;
    }
}

WiFiDevice* WiFiDevice::getInstance(int dev_id)
{
    WiFiDevice* pRet = NULL;

    if(devHash)
    {
        pRet = (WiFiDevice *)g_hash_table_lookup(devHash, (gpointer) dev_id);
    }
    else
    {
        devHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new WiFiDevice(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create WiFi device instance..\n");
        }
        g_hash_table_insert(devHash,(gpointer)dev_id, pRet);
    }
    return pRet;
}
void* WiFiDevice::getContext()
{
    return ctxt;
}

void WiFiDevice::closeInstance(WiFiDevice *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(devHash, (gconstpointer)pDev->dev_id);
        if(pDev->ctxt)
        {
//           WiFiCtl_Close(pDev->ctxt);
        }
        delete pDev;
    }
}

void WiFiDevice::closeAllInstances()
{
    if(devHash)
    {
        GList* tmp_list = g_hash_table_get_values (devHash);

        while(tmp_list)
        {
            WiFiDevice* pDev = (WiFiDevice *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

//------------------------------------------------------------------------------
// init: Perform the necessary operations to initialise the WiFi device.
//       Returns 0 on success, -1 on failure.
//------------------------------------------------------------------------------
int WiFiDevice::init()
{
    /* Initialise the WiFi HAL */
    /*   int ret = wifi_init();

       if (ret != 0)
       {
          RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error! Unable to initialise WiFi HAL\n");
          throw 1;
       }*/
    return 1;
}

//------------------------------------------------------------------------------
// shutdown: Perform the necessary operations to shut down the WiFi device.
//------------------------------------------------------------------------------
void WiFiDevice::shutdown()
{
    /* Shut down the WiFi HAL */
//   (void)wifi_down();
}

hostIf_WiFi::hostIf_WiFi(int dev_id):
    dev_id(dev_id)
{

}


hostIf_WiFi* hostIf_WiFi::getInstance(int dev_id)
{
    hostIf_WiFi* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_WiFi *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_WiFi(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create Wifi Interface instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }

    return pRet;
}

GList* hostIf_WiFi::getAllIntefaces()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_WiFi::closeInstance(hostIf_WiFi *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_WiFi::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_WiFi* pDev = (hostIf_WiFi *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

/****************************************************************************************************************************************************/
// Device.WiFi. Profile. Getters:
/****************************************************************************************************************************************************/

int hostIf_WiFi::get_Device_WiFi_RadioNumberOfEntries(HOSTIF_MsgData_t *stMsgData)
{
    ULONG       radioNumOfEntries = 0;
    IARM_Result_t retVal = IARM_RESULT_SUCCESS;
    IARM_BUS_WiFi_DiagsPropParam_t param;
    int ret = OK;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    param.numEntry=IARM_BUS_WIFI_MGR_RadioEntry;
//    ret = wifi_getRadioNumberOfEntries(&radioNumOfEntries);

    retVal = IARM_Bus_Call(IARM_BUS_NM_SRV_MGR_NAME, IARM_BUS_WIFI_MGR_API_getSSIDProps, (void *)&param, sizeof(param));
    if (IARM_RESULT_SUCCESS != retVal)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] IARM BUS CALL failed with  : %d.\n", __FILE__, __FUNCTION__, retVal);
        return NOK;
    }
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] param.data.radioNumberOfEntries  returned %d radioNumOfEntries:%lu\n",
             __FUNCTION__, __FILE__, ret, param.data.radioNumberOfEntries);
    radioNumOfEntries=param.data.radioNumberOfEntries;
    put_int(stMsgData->paramValue,radioNumOfEntries);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof(radioNumOfEntries);

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);

    return ret;
}

int hostIf_WiFi::get_Device_WiFi_SSIDNumberOfEntries(HOSTIF_MsgData_t *stMsgData)
{
    ULONG ssidNumOfEntries = 0;
    IARM_Result_t retVal = IARM_RESULT_SUCCESS;
    int ret = OK;
    IARM_BUS_WiFi_DiagsPropParam_t param;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    param.numEntry=IARM_BUS_WIFI_MGR_SSIDEntry;

    retVal = IARM_Bus_Call(IARM_BUS_NM_SRV_MGR_NAME, IARM_BUS_WIFI_MGR_API_getSSIDProps, (void *)&param, sizeof(param));
    if (IARM_RESULT_SUCCESS != retVal)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] IARM BUS CALL failed with  : %d.\n", __FILE__, __FUNCTION__, retVal);
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering.. param.data.ssidNumberOfEntries %d \n", __FUNCTION__, __FILE__,param.data.ssidNumberOfEntries);
    ssidNumOfEntries=param.data.ssidNumberOfEntries;
    put_int(stMsgData->paramValue,ssidNumOfEntries);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = 4;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);

    return ret;
}

int hostIf_WiFi::get_Device_WiFi_AccessPointNumberOfEntries(HOSTIF_MsgData_t *stMsgData)
{
    unsigned int accessPointNumOfEntries = 1;
    int ret = OK;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    put_int(stMsgData->paramValue, accessPointNumOfEntries);

    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=4;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);

    return OK;
}

int hostIf_WiFi::get_Device_WiFi_EndPointNumberOfEntries(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    unsigned int endPointNumOfEntries = 1;
    put_int(stMsgData->paramValue, endPointNumOfEntries);
    stMsgData->paramtype = hostIf_UnsignedIntType;
    stMsgData->paramLen = sizeof (unsigned int);

    return OK;
}

int hostIf_WiFi::get_Device_WiFi_EnableWiFi(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    IARM_BUS_NetSrvMgr_Iface_EventData_t param = {0};
    snprintf (param.setInterface, INTERFACE_SIZE, "WIFI");
    IARM_Bus_Call(IARM_BUS_NM_SRV_MGR_NAME, IARM_BUS_NETSRVMGR_API_isInterfaceEnabled, (void*)&param, sizeof(param));
    put_boolean(stMsgData->paramValue, param.isInterfaceEnabled);
    stMsgData->paramtype = hostIf_BooleanType;
    stMsgData->paramLen=1;
    return OK;
}

int hostIf_WiFi::set_Device_WiFi_EnableWiFi(HOSTIF_MsgData_t *stMsgData)
{
    LOG_ENTRY_EXIT;

    int ret = NOK;
    if (stMsgData->paramtype == hostIf_BooleanType)
    {
        bool enabled = get_boolean(stMsgData->paramValue);

        IARM_BUS_NetSrvMgr_Iface_EventData_t iarmData = { 0 };

        snprintf (iarmData.setInterface, INTERFACE_SIZE, "WIFI");
        iarmData.isInterfaceEnabled = true; // set interface control persistence = true, whether WiFi is asked to be enabled or not

        if (IARM_RESULT_SUCCESS == IARM_Bus_BroadcastEvent (IARM_BUS_NM_SRV_MGR_NAME,
                (IARM_EventId_t) IARM_BUS_NETWORK_MANAGER_EVENT_SET_INTERFACE_CONTROL_PERSISTENCE,
                (void *)&iarmData, sizeof(iarmData)))
        {
            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF,
                    "[%s] Broadcasted IARM_BUS_NETWORK_MANAGER_EVENT_SET_INTERFACE_CONTROL_PERSISTENCE = %d for interface '%s'\n",
                    __FUNCTION__, iarmData.isInterfaceEnabled, iarmData.setInterface);
        }
        else
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF,
                    "[%s] Failed to broadcast IARM_BUS_NETWORK_MANAGER_EVENT_SET_INTERFACE_CONTROL_PERSISTENCE = %d for interface '%s'\n",
                    __FUNCTION__, iarmData.isInterfaceEnabled, iarmData.setInterface);
        }

        memset(&iarmData, 0, sizeof(IARM_BUS_NetSrvMgr_Iface_EventData_t));

        snprintf (iarmData.setInterface, INTERFACE_SIZE, "WIFI");
        iarmData.isInterfaceEnabled = enabled;

        if (IARM_RESULT_SUCCESS == IARM_Bus_BroadcastEvent (IARM_BUS_NM_SRV_MGR_NAME,
                (IARM_EventId_t) IARM_BUS_NETWORK_MANAGER_EVENT_SET_INTERFACE_ENABLED,
                (void *)&iarmData, sizeof(iarmData)))
        {
            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF,
                    "[%s] Broadcasted IARM_BUS_NETWORK_MANAGER_EVENT_SET_INTERFACE_ENABLED = %d for interface '%s'\n",
                    __FUNCTION__, iarmData.isInterfaceEnabled, iarmData.setInterface);
        }
        else
        {
            RDK_LOG (RDK_LOG_ERROR ,LOG_TR69HOSTIF,
                    "[%s] Failed to broadcast IARM_BUS_NETWORK_MANAGER_EVENT_SET_INTERFACE_ENABLED = %d for interface '%s'\n",
                    __FUNCTION__, iarmData.isInterfaceEnabled, iarmData.setInterface);
        }

        ret = OK;
    }
    else
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Failed due to wrong data type for %s, please use boolean(0/1) to set.\n",
                __FUNCTION__, stMsgData->paramName);
        stMsgData->faultCode = fcInvalidParameterType;
    }
    return ret;
}

#endif
/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */
