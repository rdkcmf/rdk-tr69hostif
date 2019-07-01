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
 * @file Components_XrdkEMMC.h
 * @brief eMMC diagnostic APIs.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef STBSERVICE_COMPONENTS_X_RDKCENTRAL_COM_EMMC_H_
#define STBSERVICE_COMPONENTS_X_RDKCENTRAL_COM_EMMC_H_

#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_main.h"

#include "rdkStorageMgrTypes.h"

#define X_EMMC_OBJ "Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_eMMCFlash."

class hostIf_STBServiceXeMMC
{
private:
    static GHashTable *ifHash;
    static hostIf_STBServiceXeMMC *instance;

    char emmcDeviceID[RDK_STMGR_MAX_STRING_LENGTH] = "";
    char emmcPartitionID[RDK_STMGR_MAX_STRING_LENGTH] = "";

    hostIf_STBServiceXeMMC(char* emmcDeviceID, char* emmcPartitionID);
    ~hostIf_STBServiceXeMMC() {};

    int getCapacity(HOSTIF_MsgData_t *);
    int getLifeElapsedA(HOSTIF_MsgData_t *);
    int getLifeElapsedB(HOSTIF_MsgData_t *);
    int getLifeElapsed(HOSTIF_MsgData_t *stMsgData, const char* life_elapsed_type);
    int getLotID(HOSTIF_MsgData_t *);
    int getManufacturer(HOSTIF_MsgData_t *);
    int getModel(HOSTIF_MsgData_t *);
    int getReadOnly(HOSTIF_MsgData_t *);
    int getSerialNumber(HOSTIF_MsgData_t *);
    int getTSBQualified(HOSTIF_MsgData_t *);
    int getPreEOLStateSystem(HOSTIF_MsgData_t *);
    int getPreEOLStateEUDA(HOSTIF_MsgData_t *);
    int getPreEOLStateMLC(HOSTIF_MsgData_t *);
    int getPreEOLState(HOSTIF_MsgData_t *stMsgData, const char* pre_eol_state_type);
    int getFirmwareVersion(HOSTIF_MsgData_t *);
    int getDeviceReport(HOSTIF_MsgData_t *);

public:
    static hostIf_STBServiceXeMMC *getInstance();
    static void closeInstance(hostIf_STBServiceXeMMC *);

    int handleGetMsg(HOSTIF_MsgData_t *);
    int handleSetMsg(HOSTIF_MsgData_t *);
};

#endif /* STBSERVICE_COMPONENTS_X_RDKCENTRAL_COM_EMMC_H_ */


/** @} */
/** @} */
