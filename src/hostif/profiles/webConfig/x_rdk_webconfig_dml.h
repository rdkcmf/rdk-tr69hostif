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
 * @file x_rdk_webConfig_dml.h
 *
 * @brief TR-069 Device.WebConfig object Public API.
 *
 * Description of WebConfig module.
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



/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef X_RDK_WEBCONFIG_DML_H_
#define X_RDK_WEBCONFIG_DML_H_

/*****************************************************************************
 * TR069-SPECIFIC INCLUDE FILES
 *****************************************************************************/
#include <mutex>
#include "hostIf_main.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_utils.h"


/** @defgroup Device.X_RDK_WebConfig object API.
 *  @ingroup TR_069_API
 *
 *  The Device.X_RDK_WebConfig object contains parameters of WebConfig profile
 *  the CPE.
 *
 */

/** @addtogroup X_RDK_WebConfig Getter and Setter API.
 *  @ingroup X_RDK_WebConfig
 *
 *  \section getter TR-069 X_RDK_WebConfig Getter API
 *
 *  @todo Clarify description of DIAG_ERROR.
 *
 *  @{
 */

#define X_RDK_WEBCONFIG_OBJ  "Device.X_RDK_WebConfig."
#define X_RDK_WEBCONFIG_FORCE_SYNC_BOOLEAN  "ForceSync"
#define X_RDK_WEBCONFIG_DATA_STRING  "Data"
#define X_RDK_WEBCONFIG_URL_STRING  "URL"

class X_RDK_WebConfig_dml {

private:
    static std::mutex mlg;   // Use std::lock_guard<std::mutex> lockGuard(mlg);

    static X_RDK_WebConfig_dml* m_instance;

    /*
     * Getter and Setter Api'ss
     */
    int getWebcfgURL(HOSTIF_MsgData_t *);
    int setWebcfgURL(HOSTIF_MsgData_t *);
    int getWebcfgData(HOSTIF_MsgData_t *);
    int setWebcfgForceSync(HOSTIF_MsgData_t *);

public:
    static X_RDK_WebConfig_dml *getInstance();
    static void closeInstance();
    int handleGetReq(HOSTIF_MsgData_t *);
    int handleSetReq(HOSTIF_MsgData_t *);
};
/* End of X_RDK_WebConfig_dml doxygen group. */
/**
 * @}
 */
int writeWebcfgURL(const char *url);


#endif /* X_RDK_WEBCONFIG_DML_H_ */


/** @} */
/** @} */
