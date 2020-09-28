/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2018 RDK Management
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
 * @file Device_WiFi_AccessPoint_AssociatedDevice.cpp
 *
 * @brief WiFi AccessPoint AssociatedDevice API Implementation.
 *
 * This is the implementation of the WiFi API.
 *
 * @par Document
 */
/** @addtogroup TR-069 WiFi Implementation
 *  This is the implementation of the Device Public API.
 *  @{
 */

#ifdef USE_WIFI_PROFILE

/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/
#include "Device_WiFi_AccessPoint_AssociatedDevice.h"

GHashTable* hostIf_WiFi_AccessPoint_AssociatedDevice::ifHash = NULL;

hostIf_WiFi_AccessPoint_AssociatedDevice* hostIf_WiFi_AccessPoint_AssociatedDevice::getInstance(int dev_id)
{
    hostIf_WiFi_AccessPoint_AssociatedDevice* pRet = NULL;

    if(ifHash)
    {
        pRet = (hostIf_WiFi_AccessPoint_AssociatedDevice *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    }
    else
    {
        ifHash = g_hash_table_new(NULL,NULL);
    }

    if(!pRet)
    {
        try {
            pRet = new hostIf_WiFi_AccessPoint_AssociatedDevice(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create hostIf_WiFi_AccessPoint_AssociatedDevice instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_WiFi_AccessPoint_AssociatedDevice::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_WiFi_AccessPoint_AssociatedDevice::closeInstance(hostIf_WiFi_AccessPoint_AssociatedDevice *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_WiFi_AccessPoint_AssociatedDevice::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_WiFi_AccessPoint_AssociatedDevice* pDev = (hostIf_WiFi_AccessPoint_AssociatedDevice *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}


hostIf_WiFi_AccessPoint_AssociatedDevice::hostIf_WiFi_AccessPoint_AssociatedDevice(int dev_id):
    dev_id(0),
    AuthenticationState(false),
    LastDataDownlinkRate(0),
    LastDataUplinkRate(0),
    SignalStrength(0),
    Retransmissions(0),
    Active(false)
{
    memset(MACAddress, 0, 17);
}




/**
 * @brief    Get the MAC Address of an Associated Device of a MoCA Interface.
 *
 * This function provides the MAC address of the MoCA interface of the device associated
 * with this MoCA interface.
 *
 * See @ref dev_moca_if_assocdev_getter
 *
 */
int hostIf_WiFi_AccessPoint_AssociatedDevice::get_hostIf_WiFi_AccessPoint_AssociatedDevice_MACAddress(HOSTIF_MsgData_t *stMsgData )
{
 return 0;
}

int hostIf_WiFi_AccessPoint_AssociatedDevice::get_hostIf_WiFi_AccessPoint_AssociatedDevice_AuthenticationState(HOSTIF_MsgData_t *stMsgData )
{
 return 0;

}

int hostIf_WiFi_AccessPoint_AssociatedDevice::get_hostIf_WiFi_AccessPoint_AssociatedDevice_LastDataDownlinkRate(HOSTIF_MsgData_t *stMsgData )
{
 return 0;

}
int hostIf_WiFi_AccessPoint_AssociatedDevice::get_hostIf_WiFi_AccessPoint_AssociatedDevice_LastDataUplinkRate(HOSTIF_MsgData_t *stMsgData )
{
 return 0;

}
int hostIf_WiFi_AccessPoint_AssociatedDevice::get_hostIf_WiFi_AccessPoint_AssociatedDevice_SignalStrength(HOSTIF_MsgData_t *stMsgData )
{
 return 0;

}
int hostIf_WiFi_AccessPoint_AssociatedDevice::get_hostIf_WiFi_AccessPoint_AssociatedDevice_Retransmissions(HOSTIF_MsgData_t *stMsgData )
{
 return 0;

}
int hostIf_WiFi_AccessPoint_AssociatedDevice::get_hostIf_WiFi_AccessPoint_AssociatedDevice_Active(HOSTIF_MsgData_t *stMsgData )
{
 return 0;

}

#endif /* #ifdef USE_WIFI_PROFILE */

