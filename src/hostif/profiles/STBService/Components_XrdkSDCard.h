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
/*
 * Components_X_SDCard.h
 *
 *  Created on: May 5, 2015
 *      Author: rdey
 */

/**
 * @file Components_XrdkSDCard.h
 * @brief The header file provides components Xrdk SDCard information APIs.
 */

/**
 * @defgroup TR69_HOSTIF_STBSERVICES_SDCARD TR-069 Object (Device.STBServices.{i}.Components.XrdkSDCard.{i})
 * @par External rdk SDCard
 * @ingroup TR69_HOSTIF_STBSERVICES
 *
 * @defgroup TR69_HOSTIF_STBSERVICES_SDCARD_API TR-069 Object (Device.STBServices.{i}.Components.XrdkSDCard.{i}) Public APIs
 * Describe the details about RDK TR-069 components XRDK_SDK APIs specifications.
 * @ingroup TR69_HOSTIF_STBSERVICES_SDCARD
 *
 * @defgroup TR69_HOSTIF_STBSERVICES_SDCARD_CLASSES TR-069 Object (Device.STBServices.{i}.Components.XrdkSDCard.{i}) Public Classes
 * Describe the details about classes used in TR069 components XRDK_SDCARD.
 * @ingroup TR69_HOSTIF_STBSERVICES_SDCARD
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef STBSERVICE_COMPONENTS_X_RDKCENTRAL_COM_SDCARD_H_
#define STBSERVICE_COMPONENTS_X_RDKCENTRAL_COM_SDCARD_H_

//#ifdef USE_XRDK_SDCARD_PROFILE
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_main.h"

#define SD_PARAM_LEN 64

#define X_SDCARD_OBJ "Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_SDCard."

#define CAPACITY_STRING "capacity"
#define CARDFAILED_STRING "cardFailed"
#define LIFEELAPSED_STRING "lifeElapsed"
#define LOTID_STRING "lotID"
#define MANUFACTURED_STRING "manufacturer"
#define MODEL_STRING "model"
#define READONLY_STRING "readOnly"
#define SERIALNUMBER_STRING "serialNumber"
#define TSBQUALIFIED_STRING "tsbQualified"
#define SDCARD_STATUS "Status"

/**
 * @brief This class provides the TR-069 components XSD Card information.
 * @ingroup TR69_HOSTIF_STBSERVICES_SDCARD_CLASSES
 */
class hostIf_STBServiceXSDCard
{
    static GHashTable *ifHash;
    hostIf_STBServiceXSDCard();
    ~hostIf_STBServiceXSDCard() {};

private:
    int getCapacity(HOSTIF_MsgData_t *);
    int getCardFailed(HOSTIF_MsgData_t *);
    int getLifeElapsed(HOSTIF_MsgData_t *);
    int getLotID(HOSTIF_MsgData_t *);
    int getManufacturer(HOSTIF_MsgData_t *);
    int getModel(HOSTIF_MsgData_t *);
    int getReadOnly(HOSTIF_MsgData_t *);
    int getSerialNumber(HOSTIF_MsgData_t *);
    int getTSBQualified(HOSTIF_MsgData_t *);
    int getStatus(HOSTIF_MsgData_t *);

public:
    static hostIf_STBServiceXSDCard *getInstance();
    static void closeInstance(hostIf_STBServiceXSDCard *);
    int handleGetMsg(HOSTIF_MsgData_t *);
    int handleSetMsg(HOSTIF_MsgData_t *);
};
//#endif /*#USE_XRDK_SDCARD_PROFILE*/
#endif /* STBSERVICE_COMPONENTS_X_RDKCENTRAL_COM_SDCARD_H_ */


/** @} */
/** @} */
