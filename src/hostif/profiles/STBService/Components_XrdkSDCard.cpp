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
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/



/**
 * @file Components_XrdkSDCard.cpp
 * @brief This source file contains the APIs of TR069 Components external SD Card.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifdef USE_XRDK_SDCARD_PROFILE
#include "rdk_debug.h" 
#include "hostIf_main.h"
#include "Components_XrdkSDCard.h"

#ifdef USE_RDK_STORAGE_MANAGER_V2
#include "rdkStorageMgr.h"
/* Declare variable types that can be used to reduce amount change */
typedef union _uSDCardParamValue {
    char uchVal[128];
    unsigned int ui32Val;
    int iVal;
    bool bVal;
} uSDCard_Param_Val;

typedef enum _eSD_PROPERTY_Type {
    SD_Capacity,
    SD_CardFailed,
    SD_LifeElapsed,
    SD_LotID,
    SD_Manufacturer,
    SD_Model,
    SD_ReadOnly,
    SD_SerialNumber,
    SD_TSBQualified,
    SD_Status
} eSD_ParamPropertyType;

typedef struct _strMgrSDcardPropParam_t {
    uSDCard_Param_Val sdCardProp;
    eSD_ParamPropertyType eSDPropType;
} strMgrSDcardPropParam_t;

#else
#include "storageMgr.h"
#endif /* USE_RDK_STORAGE_MANAGER_V2 */
#include "safec_lib.h"

static bool getSDCardProperties(strMgrSDcardPropParam_t *);

hostIf_STBServiceXSDCard *hostIf_STBServiceXSDCard::instance = NULL;

hostIf_STBServiceXSDCard* hostIf_STBServiceXSDCard::getInstance()
{
    if (!instance)
    {
        try
        {
            instance = new hostIf_STBServiceXSDCard();
        }
        catch(const std::exception& e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Caught exception \" %s\"\n", __FUNCTION__, e.what());
        }
    }
    return instance;
}


void hostIf_STBServiceXSDCard::closeInstance(hostIf_STBServiceXSDCard *pDev)
{
    if(pDev)
    {
        delete pDev;
    }
}

/**
 * @brief Class default Constructor.
 */
hostIf_STBServiceXSDCard::hostIf_STBServiceXSDCard()
{
    /*    capacity = 0;
        cardFailed = false;
        lifeElapsed = 0;
        readOnly = false;
        tsbQualified = false;
        memset(lotID, '\0', SD_PARAM_LEN);
        memset(manufacturer, '\0', SD_PARAM_LEN);
        memset(model, '\0', SD_PARAM_LEN);
        memset(serialNumber, '\0', SD_PARAM_LEN); */
}

/**
 * @brief This function set the external SD Card interface attribute such as capacity,
 * cardFailed, lifeElapsed, lotID, manufacturer, model, readOnly, serialNumber, tsbQualified
 * in a connected external SD Card. Currently not implemented.
 *
 * @param[in] stMsgData  HostIf Message Request param contains the external SD Card attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully set the hostIf external SD Card interface attribute.
 * @retval -1 If Not able to set the hostIf external SD Card interface attribute.
 * @retval -2 If Not handle the hostIf external SD Card interface attribute.
 * @ingroup TR69_HOSTIF_STBSERVICES_SDCARD_API
 */
int hostIf_STBServiceXSDCard::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    return ret;
}

/**
 * @brief This function get the external SD Card interface attribute such as capacity,
 * cardFailed, lifeElapsed, lotID, manufacturer, model, readOnly, serialNumber, tsbQualified
 * in a connected external SD Card.
 *
 * @param[in] stMsgData  HostIf Message Request param contains the external SD Card attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the hostIf external SD Card interface attribute.
 * @retval -1 If Not able to get the hostIf external SD Card interface attribute.
 * @retval -2 If Not handle the hostIf external SD Card interface attribute.
 * @ingroup TR69_HOSTIF_STBSERVICES_SDCARD_API
 */
int hostIf_STBServiceXSDCard::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char *path = NULL, *paramName = NULL;
    try {
        int str_len = strlen(X_SDCARD_OBJ);
        path = stMsgData->paramName;

        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s]Entering...  \n", __FILE__, __FUNCTION__);

        if(NULL == path) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d]Failed : Parameter is NULL, %s  \n", __FILE__, __FUNCTION__, __LINE__, path);
            return ret;
        }

        if((strncasecmp(path, X_SDCARD_OBJ, str_len) != 0)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s:%d]Failed : Mismatch parameter path : %s  \n", __FILE__, __FUNCTION__, __LINE__, path);
            return ret;
        }
        const char *tmp_ptr = strchr(path+str_len-1,'.');
        if(tmp_ptr == NULL)  {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Parameter is NULL  \n", __FILE__, __FUNCTION__);
            return ret;
        }

        tmp_ptr++;
        paramName = tmp_ptr;
        if (strcasecmp(paramName, CAPACITY_STRING) == 0)
        {
            ret = getCapacity(stMsgData);
        }
        else if (strcasecmp(paramName, CARDFAILED_STRING) == 0)
        {
            ret = getCardFailed(stMsgData);
        }
        else if (strcasecmp(paramName, LIFEELAPSED_STRING) == 0)
        {
            ret = getLifeElapsed(stMsgData);
        }
        else if (strcasecmp(paramName, LOTID_STRING) == 0)
        {
            ret = getLotID(stMsgData);
        }
        else if (strcasecmp(paramName, MANUFACTURED_STRING) == 0)
        {
            ret = getManufacturer(stMsgData);
        }
        else if (strcasecmp(paramName, MODEL_STRING) == 0)
        {
            ret = getModel(stMsgData);
        }
        else if (strcasecmp(paramName, READONLY_STRING) == 0)
        {
            ret = getReadOnly(stMsgData);
        }
        else if (strcasecmp(paramName, SERIALNUMBER_STRING) == 0)
        {
            ret = getSerialNumber(stMsgData);
        }
        else if (strcasecmp(paramName, TSBQUALIFIED_STRING) == 0)
        {
            ret = getTSBQualified(stMsgData);
        }
        else if (strcasecmp(paramName, SDCARD_STATUS) == 0)
        {
            ret = getStatus(stMsgData);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Parameter \'%s\' is Not Supported  \n", __FILE__, __FUNCTION__, paramName);
            ret = NOK;
        }
    }
    catch (const std::exception& e )
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Exception caught %s   \n", __FILE__, __FUNCTION__, e.what());
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s]Exiting... \n", __FILE__, __FUNCTION__);
    return ret;
}

/************************************************************
 * Description  : Get SD Card memory size in MB
 * Precondition : SDCard should be connected to STB
 * Input        : stMsgData ->  HOSTIF_MsgData_t

 * Return       : OK -> Success
                  NOK -> Failure
************************************************************/
int hostIf_STBServiceXSDCard::getCapacity(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    try {
        strMgrSDcardPropParam_t param;
        memset(&param, '\0', sizeof(param));
        param.eSDPropType = SD_Capacity;
        if(getSDCardProperties(&param) ) {
            // Capacity from getSDCardProperties is in bytes, Need to convert it to MB.
            unsigned long long capacityBytes = 0;
            int capacityMegaBytes = 0;
            capacityBytes = (unsigned long long) param.sdCardProp.ui32Val;
            capacityMegaBytes = (capacityBytes /1024/1024) ;
            put_int(stMsgData->paramValue, capacityMegaBytes);
        }
        stMsgData->paramtype=hostIf_UnsignedIntType;
    }
    catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}


/************************************************************
 * Description  : Get the cardfailed status.
 * 				 'true' if the card is no longer readable/writable
 * 				 which could be due to Card/Host interface failure
 * Precondition : SDCard should be connected to STB
 * Input        : CardFailed-> Complete Parameter path

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> 0: false
                        -> 1: true

************************************************************/

int hostIf_STBServiceXSDCard::getCardFailed(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    try {
        strMgrSDcardPropParam_t param;
        memset(&param, '\0', sizeof(param));
        param.eSDPropType = SD_CardFailed;
        if(getSDCardProperties(&param) ) {
            put_boolean(stMsgData->paramValue, param.sdCardProp.bVal);
        }
        stMsgData->paramtype=hostIf_BooleanType;
    }
    catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}


/************************************************************
 * Description  : Get Life Elapsed of SDcard. The value range
 * 				  from 0 to 100. It may increment in 10% or lower
 * 				  increments (e.g. 5% or 1%) depending on
 * 				  Card+Host capabilities. Refer CMD56.
 * Precondition : SDCard should be connected to STB
 * Input        : stMsgData -> Complete Parameter path
 * 				"Device.Services.STBService.1.Components.X_RDKCENTRAL-COM_SDCard.LifeElapsed"
 *
 * Return       : OK -> Success
 *                NOK -> Failure
 *
************************************************************/

int hostIf_STBServiceXSDCard::getLifeElapsed(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    try {
        strMgrSDcardPropParam_t param;
        memset(&param, '\0', sizeof(param));
        param.eSDPropType = SD_LifeElapsed;
        if(getSDCardProperties(&param) ) {
            put_int(stMsgData->paramValue, param.sdCardProp.iVal);
        }
        stMsgData->paramtype=hostIf_IntegerType;
    }
    catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}


/************************************************************
 * Description  : Get LotID of SDCard
 * Precondition : SDCard should be connected to STB
 * Input        : stMsgData -> Complete Parameter path

 * Return       : OK -> Success
                  NOK -> Failure

************************************************************/

int hostIf_STBServiceXSDCard::getLotID(HOSTIF_MsgData_t *stMsgData)
{
    errno_t rc = -1;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    try {
        strMgrSDcardPropParam_t param;
        memset(&param, '\0', sizeof(param));
        param.eSDPropType = SD_LotID;
        if(getSDCardProperties(&param) ) {
            rc=strcpy_s((char *)stMsgData->paramValue,sizeof(stMsgData->paramValue), (const char*) param.sdCardProp.uchVal);
	    if(rc!=EOK)
    	    {	
		    ERR_CHK(rc);
	    }
        }
        stMsgData->paramtype=hostIf_StringType;
    }
    catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

/************************************************************
 * Description  : Get Manufacturer of SD Flash card
 * 					(1 byte MID field in CID register)
 * Precondition : SDCard should be connected to STB
 * Input        : stMsgData -> HOSTIF_MsgData_t, which contains
 * 					complete path path.

 * Return       : OK -> Success
                  NOK -> Failure

************************************************************/

int hostIf_STBServiceXSDCard::getManufacturer(HOSTIF_MsgData_t *stMsgData)
{
    errno_t rc = -1;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    try {
        strMgrSDcardPropParam_t param;
        memset(&param, '\0', sizeof(param));
        param.eSDPropType = SD_Manufacturer;
        if(getSDCardProperties(&param) ) {
            rc=strcpy_s((char *)stMsgData->paramValue,sizeof(stMsgData->paramValue), (const char*) param.sdCardProp.uchVal);
	    if(rc!=EOK)
 	    {
		    ERR_CHK(rc);
	    }
        }
        stMsgData->paramtype=hostIf_StringType;
    }
    catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}


/************************************************************
 * Description  : Get Model of SD Flash card
 * 					(5 character PNM field in CID register)
 * Precondition : SD card should be connected to STB
 * Input        : tMsgData -> HOSTIF_MsgData_t, which contains
 * 					complete path path.

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> the audio decibel (DB) level
************************************************************/

int hostIf_STBServiceXSDCard::getModel(HOSTIF_MsgData_t *stMsgData)
{
    errno_t rc = -1;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    try {
        strMgrSDcardPropParam_t param;
        memset(&param, '\0', sizeof(param));
        param.eSDPropType = SD_Model;
        if(getSDCardProperties(&param) ) {
            rc=strcpy_s((char *)stMsgData->paramValue,sizeof(stMsgData->paramValue), (const char*) param.sdCardProp.uchVal);
	    if(rc!=EOK)
    	    {
		    ERR_CHK(rc);
	    }
        }
        stMsgData->paramtype=hostIf_StringType;
    }
    catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

/************************************************************
 * Description  : Get ReadOnly value of SD Flash card
 * 					True if user has switched SD Card to read
 * 					only mode. False otherwise.
 * Precondition : HDMI Display should be connected to STB
 * Input        : tMsgData -> HOSTIF_MsgData_t, which contains
 * 					complete path path.

 * Return       : OK -> Success
                  NOK -> Failure

************************************************************/

int hostIf_STBServiceXSDCard::getReadOnly(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    try {
        strMgrSDcardPropParam_t param;
        memset(&param, '\0', sizeof(param));
        param.eSDPropType = SD_ReadOnly;
        if(getSDCardProperties(&param) ) {
            put_boolean(stMsgData->paramValue, param.sdCardProp.bVal);
        }
        stMsgData->paramtype=hostIf_BooleanType;
    }
    catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

/************************************************************
 * Description  : Get Unique serial number of SD Flash card
 * Precondition : SDcard should be connected to STB
 * Input        : tMsgData -> HOSTIF_MsgData_t, which contains
 * 					complete path path.

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> 32 bit PSN field in CID register
************************************************************/
int hostIf_STBServiceXSDCard::getSerialNumber(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    try {
        strMgrSDcardPropParam_t param;
        memset(&param, '\0', sizeof(param));
        param.eSDPropType = SD_SerialNumber;
        if(getSDCardProperties(&param) ) {
            sprintf(stMsgData->paramValue,"%s" ,param.sdCardProp.uchVal);
        }
        stMsgData->paramtype=hostIf_StringType;
    }
    catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

/************************************************************
 * Description  : Get TSBQualified
 * Precondition : HDMI Display should be connected to STB
 * Input        : tMsgData -> HOSTIF_MsgData_t, which contains
 * 					complete path path.

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> True if card is approved for Comcast TSB use.
                  For Xi3v1, this will be based on the expected signature.
                  False otherwise.
************************************************************/

int hostIf_STBServiceXSDCard::getTSBQualified(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    try {
        strMgrSDcardPropParam_t param;
        memset(&param, '\0', sizeof(param));
        param.eSDPropType = SD_TSBQualified;
        if(getSDCardProperties(&param) ) {
            put_boolean(stMsgData->paramValue, param.sdCardProp.bVal);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] param.sdCardProp.bVal: %d\n", __FUNCTION__, __FILE__, param.sdCardProp.bVal);
        }
        stMsgData->paramtype=hostIf_BooleanType;
    }
    catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}

/************************************************************
 * Description  : Get TSBQualified
 * Precondition : HDMI Display should be connected to STB
 * Input        : tMsgData -> HOSTIF_MsgData_t, which contains
 * 					complete path path.

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> True if card is approved for Comcast TSB use.
                  For Xi3v1, this will be based on the expected signature.
                  False otherwise.
************************************************************/

int hostIf_STBServiceXSDCard::getStatus(HOSTIF_MsgData_t *stMsgData)
{
    errno_t rc = -1;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    try {
        strMgrSDcardPropParam_t param;
        memset(&param, '\0', sizeof(param));
        param.eSDPropType = SD_Status;
        if(getSDCardProperties(&param) ) {
            rc=strcpy_s((char *)stMsgData->paramValue,sizeof(stMsgData->paramValue), (const char*) param.sdCardProp.uchVal);
	    if(rc!=EOK)
	    {
		    ERR_CHK(rc);
	    }
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"[%s:%s] param.sdCardProp.bchVal: %d\n", __FUNCTION__, __FILE__, param.sdCardProp.uchVal);
        }
        stMsgData->paramtype=hostIf_StringType;
    }
    catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception : %s\r\n",__FUNCTION__, e.what());
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return OK;
}



bool getSDCardProperties(strMgrSDcardPropParam_t *sdCardParam)
{
    errno_t safec_rc = -1;
#ifdef USE_RDK_STORAGE_MANAGER_V2
    bool rc = true;
    static char sdCardDeviceID[RDK_STMGR_MAX_STRING_LENGTH] = "";
    static char sdCardPartitionID[RDK_STMGR_MAX_STRING_LENGTH] = "";

    eSTMGRReturns stRet;
    if ('\0' == sdCardDeviceID[0])
    {
        eSTMGRDeviceInfoList deviceInfoList;
        memset (&deviceInfoList, 0, sizeof(deviceInfoList));
        stRet = rdkStorage_getDeviceInfoList(&deviceInfoList);
        for (int i = 0; i < deviceInfoList.m_numOfDevices; i++)
        {
            if (RDK_STMGR_DEVICE_TYPE_SDCARD == deviceInfoList.m_devices[i].m_type)
            {
                safec_rc=memcpy_s (&sdCardDeviceID, RDK_STMGR_MAX_STRING_LENGTH ,&deviceInfoList.m_devices[i].m_deviceID, RDK_STMGR_MAX_STRING_LENGTH);
                if(safec_rc!= EOK)
	        {
		    ERR_CHK(safec_rc);
	        }
		safec_rc=memcpy_s (&sdCardPartitionID, RDK_STMGR_MAX_STRING_LENGTH , &deviceInfoList.m_devices[i].m_partitions, RDK_STMGR_MAX_STRING_LENGTH);
                if(safec_rc!= EOK)
                {
                    ERR_CHK(safec_rc);
                }
		break;
            }
        }
    }

    if ('\0' != sdCardDeviceID[0])
    {
        if (sdCardParam->eSDPropType == SD_LifeElapsed)
        {
            eSTMGRHealthInfo healthInfo;
            memset (&healthInfo, 0 , sizeof(healthInfo));
            if (RDK_STMGR_RETURN_SUCCESS == rdkStorage_getHealth (sdCardDeviceID, &healthInfo))
            {
                sdCardParam->sdCardProp.iVal = -1;
                /* FIXME: Update string from "used" to a proper proposed value */
                for (int i = 0; i < healthInfo.m_diagnostics.m_list.m_numOfAttributes; i++)
                {
                    if (0 == strcmp (healthInfo.m_diagnostics.m_list.m_diagnostics[i].m_name, "used"))
                    {
                        sdCardParam->sdCardProp.iVal = atoi (healthInfo.m_diagnostics.m_list.m_diagnostics[i].m_value);
                        break;
                    }
                }
            }
            else
            {
                sdCardParam->sdCardProp.iVal = -1;
            }
        }
        else
        {
            eSTMGRDeviceInfo deviceInfo;
            memset (&deviceInfo, 0 , sizeof(deviceInfo));
            if (RDK_STMGR_RETURN_SUCCESS == rdkStorage_getDeviceInfo (sdCardDeviceID, &deviceInfo))
            {
                switch(sdCardParam->eSDPropType)
                {
                    case SD_Capacity:
                        sdCardParam->sdCardProp.ui32Val = deviceInfo.m_capacity;
                        break;
                    case SD_CardFailed:
                        /* FIXME: The original code always returns false. so #defined it */
#if 0
                        if(deviceInfo.status == SM_DEV_STATUS_NOT_QUALIFIED || deviceInfo.status == SM_DEV_STATUS_NOT_PRESENT)
                        {
                            sdCardParam->sdCardProp.bVal = false;
                        }
                        else
                        {
                            sdCardParam->sdCardProp.bVal = false;
                        }
#else
                        sdCardParam->sdCardProp.bVal = false;
#endif
                        break;
                    case SD_LotID:
			{
                        safec_rc=strcpy_s (sdCardParam->sdCardProp.uchVal,sizeof(sdCardParam->sdCardProp.uchVal), deviceInfo.m_hwVersion);
			if(safec_rc!=EOK)
    			{
	    			ERR_CHK(safec_rc);
    			}
                        break;
			}
                    case SD_Manufacturer:
			{
                        safec_rc=strcpy_s(sdCardParam->sdCardProp.uchVal,sizeof(sdCardParam->sdCardProp.uchVal),deviceInfo.m_manufacturer);
			if(safec_rc!=EOK)
                        {
                                ERR_CHK(safec_rc);
                        }
                        break;
			}
                    case SD_Model:
			{
                        safec_rc=strcpy_s (sdCardParam->sdCardProp.uchVal,sizeof(sdCardParam->sdCardProp.uchVal), deviceInfo.m_model);
			if(safec_rc!=EOK)
                        {
                                ERR_CHK(safec_rc);
                        }
                        break;
			}
                    case SD_ReadOnly:
                        sdCardParam->sdCardProp.bVal = (deviceInfo.m_status == RDK_STMGR_DEVICE_STATUS_READ_ONLY) ? true : false;
                        break;
                    case SD_SerialNumber:
			{
                        safec_rc=strcpy_s(sdCardParam->sdCardProp.uchVal,sizeof(sdCardParam->sdCardProp.uchVal), deviceInfo.m_serialNumber);
			if(safec_rc!=EOK)
                        {
                                ERR_CHK(safec_rc);
                        }
                        break;
			}
                    case SD_Status:
                    {
                        switch (deviceInfo.m_status)
                        {
                            case RDK_STMGR_DEVICE_STATUS_OK:
			        {
                                safec_rc=strcpy_s (sdCardParam->sdCardProp.uchVal,sizeof(sdCardParam->sdCardProp.uchVal), "SDCARD_OK");
				if(safec_rc!=EOK)
                        	{
                                	ERR_CHK(safec_rc);
                        	}
                                break;
				}
                            case RDK_STMGR_DEVICE_STATUS_READ_ONLY:
				{
                                safec_rc=strcpy_s (sdCardParam->sdCardProp.uchVal,sizeof(sdCardParam->sdCardProp.uchVal), "SDCARD_READ_ONLY");
				if(safec_rc!=EOK)
                                {
                                        ERR_CHK(safec_rc);
                                }
                                break;
				}
                            case RDK_STMGR_DEVICE_STATUS_NOT_PRESENT:
				{
                                safec_rc=strcpy_s (sdCardParam->sdCardProp.uchVal,sizeof(sdCardParam->sdCardProp.uchVal), "SDCARD_NOT_PRESENT");
				if(safec_rc!=EOK)
                                {
                                        ERR_CHK(safec_rc);
                                }
                                break;
				}
                            case RDK_STMGR_DEVICE_STATUS_NOT_QUALIFIED:
				{
                                safec_rc=strcpy_s (sdCardParam->sdCardProp.uchVal,sizeof(sdCardParam->sdCardProp.uchVal), "SDCARD_NOT_QUALIFIED");
				if(safec_rc!=EOK)
                                {
                                        ERR_CHK(safec_rc);
                                }
                                break;
				}
                            case RDK_STMGR_DEVICE_STATUS_DISK_FULL:
				{
                                safec_rc=strcpy_s (sdCardParam->sdCardProp.uchVal,sizeof(sdCardParam->sdCardProp.uchVal), "SDCARD_DISK_FULL");
				if(safec_rc!=EOK)
                                {
                                        ERR_CHK(safec_rc);
                                }
                                break;
				}
                            case RDK_STMGR_DEVICE_STATUS_READ_FAILURE:
				{
                                safec_rc=strcpy_s (sdCardParam->sdCardProp.uchVal,sizeof(sdCardParam->sdCardProp.uchVal), "SDCARD_READ_FAILURE");
				if(safec_rc!=EOK)
                                {
                                        ERR_CHK(safec_rc);
                                }
                                break;
				}
                            case RDK_STMGR_DEVICE_STATUS_WRITE_FAILURE:
				{
                                safec_rc=strcpy_s (sdCardParam->sdCardProp.uchVal,sizeof(sdCardParam->sdCardProp.uchVal), "SDCARD_WRITE_FAILURE");
				if(safec_rc!=EOK)
                                {
                                        ERR_CHK(safec_rc);
                                }
                                break;
				}
                        }
                        break;
                    }
                    case SD_TSBQualified:
                        sdCardParam->sdCardProp.bVal = (deviceInfo.m_status != RDK_STMGR_DEVICE_STATUS_NOT_QUALIFIED &&
                                deviceInfo.m_status != RDK_STMGR_DEVICE_STATUS_UNKNOWN);
                        break;
                }
            }
            else
            {
                RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] Gettng the device specific information failed\n",__FUNCTION__);
                switch(sdCardParam->eSDPropType)
                {
                    case SD_Status:
			{
                        safec_rc=strcpy_s(sdCardParam->sdCardProp.uchVal,sizeof(sdCardParam->sdCardProp.uchVal), (const char*)"None");
			if(safec_rc!=EOK)
			{
				ERR_CHK(safec_rc);
			}
                        break;
			}
                }
            }
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_WARN, LOG_TR69HOSTIF, "[%s] This platform does not have SD card\n",__FUNCTION__);
        switch(sdCardParam->eSDPropType)
        {
            case SD_Status:
		{
	                safec_rc=strcpy_s(sdCardParam->sdCardProp.uchVal,sizeof(sdCardParam->sdCardProp.uchVal), (const char*)"None");
			if(safec_rc!=EOK)
			{
				ERR_CHK(safec_rc);
			}
                	break;
		}
        }
    }


    return rc;
#else
    IARM_Result_t retVal = IARM_RESULT_SUCCESS;
    IARM_Bus_STRMgr_Param_t param;
    bool ret = true;

    memset(&param, 0, sizeof(param));

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    try {
        param.data.stSDCardParams.eSDPropType = sdCardParam->eSDPropType;

        retVal = IARM_Bus_Call(IARM_BUS_ST_MGR_NAME, IARM_BUS_STORAGE_MGR_API_GetSDcardPropertyInfo, (void *)&param, sizeof(param));

        if(retVal == IARM_RESULT_SUCCESS && (param.status))
        {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s] Successfully return from \'%s\' \n", __FUNCTION__, IARM_BUS_STORAGE_MGR_API_GetSDcardPropertyInfo);
            sdCardParam->sdCardProp = param.data.stSDCardParams.sdCardProp;
        }
        else {

            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s] Failed to return from \'%s\' \n", __FUNCTION__, IARM_BUS_STORAGE_MGR_API_GetSDcardPropertyInfo);
            ret = false;
        }

    }
    catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s] Exception %s\r\n",__FUNCTION__, e.what());
        ret = false;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return ret;
#endif
}


#endif


/** @} */
/** @} */
