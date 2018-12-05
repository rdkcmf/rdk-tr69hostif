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
