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
 * @file Device_MoCA_Interface_Stats.c
 *
 * @brief MoCA_Interface_Stats API Implementation.
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



/*MoCA include files*/
#include "Device_MoCA_Interface_Stats.h"
#include "rdk_moca_hal.h"

MoCAInterfaceStats* MoCAInterfaceStats::pInstance = NULL;

MoCAInterfaceStats::MoCAInterfaceStats(int _dev_id):
    m_i32DevID(0),
    m_ulBytesSent(0),
    m_ulBytesReceived(0),
    m_ulPacketsSent(0),
    m_ulPacketsReceived(0),
    m_ulErrorsSent(0),
    m_ulErrorsReceived(0),
    m_ulUnicastPacketsSent(0),
    m_ulUnicastPacketsReceived(0),
    m_ulDiscardPacketsSent(0),
    m_ulDiscardPacketsReceived(0),
    m_ulMulticastPacketsSent(0),
    m_ulMulticastPacketsReceived(0),
    m_ulBroadcastPacketsSent(0),
    m_ulBroadcastPacketsReceived(0),
    m_ui32UnknownProtoPacketsReceived(0)
{
    m_i32DevID = _dev_id;
}


MoCAInterfaceStats* MoCAInterfaceStats::getInstance()
{
    if(NULL == pInstance)
    {
        try {
            pInstance = new MoCAInterfaceStats(0);
        } catch(int e) {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create MoCA Interface Status instance..\n");
        }
    }
    return pInstance;
}


/****************************************************************************************************************************************************/
// Device.MoCA.Interfacei.Stats Table Profile. Getters:
/****************************************************************************************************************************************************/

int MoCAInterfaceStats::get_BytesSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen=sizeof(unsigned long);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t response;
        if(RMH_SUCCESS == RMH_Stats_GetTxTotalBytes(rmh, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}
int MoCAInterfaceStats::get_BytesReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen=sizeof(unsigned long);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t response;
        if(RMH_SUCCESS == RMH_Stats_GetRxTotalBytes(rmh, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}

int MoCAInterfaceStats::get_PacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen=sizeof(unsigned long);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t response;
        if(RMH_SUCCESS == RMH_Stats_GetTxTotalPackets(rmh, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}
int MoCAInterfaceStats::get_PacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen=sizeof(unsigned long);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t response;
        if(RMH_SUCCESS == RMH_Stats_GetRxTotalPackets(rmh, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}
int MoCAInterfaceStats::get_ErrorsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen=sizeof(unsigned long);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t response;
        if(RMH_SUCCESS == RMH_Stats_GetTxTotalErrors(rmh, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}

int MoCAInterfaceStats::get_ErrorsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen=sizeof(unsigned long);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t response;
        if(RMH_SUCCESS == RMH_Stats_GetRxTotalErrors(rmh, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}

int MoCAInterfaceStats::get_UnicastPacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen=sizeof(unsigned long);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t response;
        if(RMH_SUCCESS == RMH_Stats_GetTxUnicastPackets(rmh, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}
int MoCAInterfaceStats::get_UnicastPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen=sizeof(unsigned long);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t response;
        if(RMH_SUCCESS == RMH_Stats_GetRxUnicastPackets(rmh, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}
int MoCAInterfaceStats::get_DiscardPacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen=sizeof(unsigned long);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t response;
        if(RMH_SUCCESS == RMH_Stats_GetTxDroppedPackets(rmh, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}
int MoCAInterfaceStats::get_DiscardPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen=sizeof(unsigned long);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t response;
        if(RMH_SUCCESS == RMH_Stats_GetRxDroppedPackets(rmh, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}
int MoCAInterfaceStats::get_MulticastPacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen=sizeof(unsigned long);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t response;
        if(RMH_SUCCESS == RMH_Stats_GetTxMulticastPackets(rmh, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}

int MoCAInterfaceStats::get_MulticastPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen=sizeof(unsigned long);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t response;
        if(RMH_SUCCESS == RMH_Stats_GetRxMulticastPackets(rmh, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}

int MoCAInterfaceStats::get_BroadcastPacketsSent(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen=sizeof(unsigned long);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t response;
        if(RMH_SUCCESS == RMH_Stats_GetTxBroadcastPackets(rmh, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}
int MoCAInterfaceStats::get_BroadcastPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen=sizeof(unsigned long);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t response;
        if(RMH_SUCCESS == RMH_Stats_GetRxBroadcastPackets(rmh, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}
int MoCAInterfaceStats::get_UnknownProtoPacketsReceived(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    int ret = NOK;
    stMsgData->paramtype = hostIf_UnsignedLongType;
    stMsgData->paramLen=sizeof(unsigned long);

    const RMH_Handle rmh = (RMH_Handle)MoCADevice::getRmhContext();
    if(rmh) {
        uint32_t response;
        if(RMH_SUCCESS == RMH_Stats_GetRxUnknownProtocolPackets(rmh, &response)) {
            put_int(stMsgData->paramValue, response);
            ret = OK;
        }
    }
    return ret;
}
/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */
