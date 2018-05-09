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
 * @file Device_MoCA_Interface_QoS.c
 *
 * @brief Device_MoCA_Interface_QoS API Implementation.
 *
 * This is the implementation of the Device_MoCA_Interface_QoS API.
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

#include "Device_MoCA_Interface_QoS.h"

MoCAInterfaceQoS* MoCAInterfaceQoS::Instance = NULL;


MoCAInterfaceQoS* MoCAInterfaceQoS::getInstance()
{
    if(NULL == Instance)
    {
        Instance = new MoCAInterfaceQoS();
    }
    return Instance;
}


/****************************************************************************************************************************************************/
// Device.MoCA.Interface.QoS Table Profile. Getters:
/****************************************************************************************************************************************************/

int MoCAInterfaceQoS::get_EgressNumFlows(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    return NOK;
}
int MoCAInterfaceQoS::get_IngressNumFlows(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    return NOK;
}

int MoCAInterfaceQoS::get_FlowStatsNumberOfEntries(HOSTIF_MsgData_t *stMsgData,bool *pChanged)
{
    return NOK;
}
/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */
