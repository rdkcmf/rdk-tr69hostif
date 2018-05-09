/*
* ============================================================================
* RDK MANAGEMENT, LLC CONFIDENTIAL AND PROPRIETARY
* ============================================================================
* This file (and its contents) are the intellectual property of RDK Management, LLC.
* It may not be used, copied, distributed or otherwise  disclosed in whole or in
* part without the express written permission of RDK Management, LLC.
* ============================================================================
* Copyright (c) 2016 RDK Management, LLC. All rights reserved.
* Copyright (C) 2017 Broadcom. The term "Broadcom" refers to Broadcom Limited and/or its subsidiaries.
* ============================================================================
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
