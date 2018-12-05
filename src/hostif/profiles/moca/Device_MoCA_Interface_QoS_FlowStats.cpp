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
 * @file Device_MoCA_Interface_QoS_FlowStats.c
 *
 * @brief MoCA_Interface_QoS_FlowStats API Implementation.
 *
 * This is the implementation of the MoCA_Interface_QoS_FlowStats API.
 *
 * @par Document
 * TBD Relevant design or API documentation.
 *
 */

/** @addtogroup MoCA_Interface_QoS_FlowStats Implementation
 *  This is the implementation of the Device Public API.
 *  @{
 */

/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/

#include "Device_MoCA_Interface_QoS_FlowStats.h"

MoCAInterfaceQoSFlowStats* MoCAInterfaceQoSFlowStats::Instance = NULL;


/****************************************************************************************************************************************************/
// Device.MoCA.Interface.QoS.FlowStats Table Profile. Getters:
/****************************************************************************************************************************************************/
MoCAInterfaceQoSFlowStats* MoCAInterfaceQoSFlowStats::getInstance()
{
    if(!Instance)
    {
        Instance = new MoCAInterfaceQoSFlowStats();
    }
    return Instance;
}

int MoCAInterfaceQoSFlowStats::get_FlowID(HOSTIF_MsgData_t *stMsgData,unsigned int entryNum,bool *pChanged)
{
    /*Retrieving value */
    // Implementation for SOC vendor
    // value->out_cval =

    return NOK;
}
int MoCAInterfaceQoSFlowStats::get_PacketDA(HOSTIF_MsgData_t *stMsgData,unsigned int entryNum,bool *pChanged)
{
    /*Retrieving value */
    // Implementation for SOC vendor
    // value->out_cval =

    return NOK;
}

int MoCAInterfaceQoSFlowStats::get_MaxRate(HOSTIF_MsgData_t *stMsgData,unsigned int entryNum,bool *pChanged)
{
    /*Retrieving value */
    // Implementation for SOC vendor
    // value->out_cval =

    return NOK;
}
int MoCAInterfaceQoSFlowStats::get_MaxBurstSize(HOSTIF_MsgData_t *stMsgData,unsigned int entryNum,bool *pChanged)
{
    /*Retrieving value */
    // Implementation for SOC vendor
    // value->out_cval =
    return NOK;
}
int MoCAInterfaceQoSFlowStats::get_LeaseTime(HOSTIF_MsgData_t *stMsgData,unsigned int entryNum,bool *pChanged)
{
    /*Retrieving value */
    // Implementation for SOC vendor
    // value->out_cval =
    return NOK;
}
int MoCAInterfaceQoSFlowStats::get_LeaseTimeLeft(HOSTIF_MsgData_t *stMsgData,unsigned int entryNum,bool *pChanged)
{
    /*Retrieving value */
    // Implementation for SOC vendor
    // value->out_cval =
    return NOK;
}
int MoCAInterfaceQoSFlowStats::get_FlowPackets(HOSTIF_MsgData_t *stMsgData,unsigned int entryNum,bool *pChanged)
{
    /*Retrieving value */
    // Implementation for SOC vendor
    // value->out_cval =
    return NOK;
}
/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */
