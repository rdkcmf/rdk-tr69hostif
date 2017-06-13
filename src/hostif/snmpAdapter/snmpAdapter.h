/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2017 RDK Management
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
 * @file snmpAdapter.h
 * @brief The header file provides TR181 SNMP device RDK Central APIs.
 */

/**
 * @defgroup TR181_HOSTIF_SNMPRDKCENTRAL TR-181 Object (Device.X_RDKCENTRAL)
 *
 *
 */

/**
 * @file snmpAdapter.h
 *
 * @brief Device.X_RDKCENTRAL API.
 *
 * Description of SNMP Adapter module.
 *
 *
 * @par Document
 * Document reference.
 *
 *
 * @par Open Issues (in no particular order)
 * -# Issue 1
 * -# Issue 2
 *
 *
 * @par Assumptions
 * -# Assumption
 * -# Assumption
 *
 *
 * @par Abbreviations
 * - ACK:     Acknowledge.
 * - BE:      Big-Endian.
 * - cb:      Callback function (suffix).
 * - config:  Configuration.
 * - desc:    Descriptor.
 * - dword:   Double word quantity, i.e., four bytes or 32 bits in size.
 * - intfc:   Interface.
 * - LE:      Little-Endian.
 * - LS:      Least Significant.
 * - MBZ:     Must be zero.
 * - MS:      Most Significant.
 * - _t:      Type (suffix).
 * - word:    Two byte quantity, i.e. 16 bits in size.
 * - xfer:    Transfer.
 *
 *
 * @par Implementation Notes
 * -# Note
 * -# Note
 *
 */


#include <string>
#ifndef SNMP_ADAPTER_H_
#define SNMP_ADAPTER_H_

/*****************************************************************************
 * TR181-SNMP SPECIFIC INCLUDE FILES
 *****************************************************************************/
#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"
#include "hostIf_updateHandler.h"
#include <map>

/**
 * @brief This class provides the interface for getting device information.
 * @ingroup TR181_HOSTIF_SNMPADAPTER_CLASSES
 */
class hostIf_snmpAdapter {

//    static  GHashTable  *m_ifHash;

    static GMutex *m_mutex;

    static  GHashTable  *m_notifyHash;

    int m_dev_id;
    static map<string, string> m_tr181SNMPMap;

    hostIf_snmpAdapter(int dev_id);

    ~hostIf_snmpAdapter();

public:
    static void init(void);
    static void unInit(void);
//    static hostIf_snmpAdapter *getInstance(int dev_id);
    static hostIf_snmpAdapter *getInstance(int dev_id=0);

    static void closeInstance(hostIf_snmpAdapter *);

//    static GList* getAllInstances();
//
//    static void closeAllInstances();

    static void getLock();

    static void releaseLock();

    GHashTable* getNotifyHash();

    /**
    * Description. This is the getter api for SNMP API for
    * Device.X_RDKCENTRAL-COM_DocsIf Profile.
    *
    * @param[in]  name  Complete path name of the parameter.
    * @param[in]  type  It is a user data type of ParameterType.
    * @param[out] value It is the value of the parameter.
    *
    * @retval OK if successful.
    * @retval XXX_ERR_BADPARAM if a bad parameter was supplied.
    *
    * @execution Synchronous.
    * @sideeffect None.
    *
    * @note This function must not suspend and must not invoke any blocking system
    * calls. It should probably just a device inventory message from the platform.
    *
    * @see XXX_SomeOtherFunction.
    */


    int get_ValueFromSNMPAdapter(HOSTIF_MsgData_t *);
    int set_ValueToSNMPAdapter(HOSTIF_MsgData_t *);
};
/* End of doxygen group */
/**
 * @}
 */

#endif /* SNMP_ADAPTER_H_ */


/** @} */
/** @} */
