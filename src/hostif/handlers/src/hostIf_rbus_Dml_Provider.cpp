/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2019 RDK Management
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
 * @file hostIf_msgHandler.cpp
 * @brief The header file provides HostIf message handler information APIs.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


/** @} */
/** @} */

#include <stdlib.h>
#ifdef __cplusplus
extern "C"
{
#include "rbus.h"
#endif
#ifdef __cplusplus
}
#endif

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_msgHandler.h"
#include "waldb.h"
//#include "rbus.h"
#include "exception.hpp"
#include "illegalArgumentException.hpp"


#define MAX_NUM_PARAMETERS 2048

rbusError_t TR_Dml_GetHandler(rbusHandle_t handle, rbusProperty_t property, rbusGetHandlerOptions_t* opts);


static void convert2hostIfDataType(char *ParamDataType, HostIf_ParamType_t* pParamType)
{
    if(!strcmp(ParamDataType,"string"))
        *pParamType = hostIf_StringType;
    else if(!strcmp(ParamDataType,"unsignedInt"))
        *pParamType = hostIf_UnsignedIntType;
    else if(!strcmp(ParamDataType,"int"))
        *pParamType = hostIf_IntegerType;
    else if(!strcmp(ParamDataType,"unsignedLong"))
        *pParamType = hostIf_UnsignedLongType;
    else if(!strcmp(ParamDataType,"boolean"))
        *pParamType = hostIf_BooleanType;
    else if(!strcmp(ParamDataType,"hexBinary"))
        *pParamType = hostIf_StringType;
    else
        *pParamType = hostIf_StringType;
}

rbusError_t TR_Dml_GetHandler(rbusHandle_t handle, rbusProperty_t inProperty, rbusGetHandlerOptions_t* opts)
{
    LOG_ENTRY_EXIT;

    rbusError_t rc = RBUS_ERROR_SUCCESS;

    const char *name = NULL;
    rbusValue_t value;

    int status = NOK;
    DataModelParam dmParam = {0};
    void *dataBaseHandle = NULL;

    rbusValue_Init(&value);

    name = rbusProperty_GetName(inProperty);

    if(!name)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s][rbusdml] The parameter requested through rbus interface is invalid..!!! \n ", __FUNCTION__);
        rc = RBUS_ERROR_INVALID_INPUT;
    }
    else
    {
        dataBaseHandle = getDataModelHandle();

        if (!dataBaseHandle)
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s][rbusdml] Failed to initialized data base handle..!!! \n ", __FUNCTION__);
            rc = RBUS_ERROR_DESTINATION_RESPONSE_FAILURE;
        }
        else
        {
            if (getParamInfoFromDataModel(dataBaseHandle, name, &dmParam) == 0)
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s][rbusdml] Invalid Parameter [%d]. \n ", __FUNCTION__);
                rc = RBUS_ERROR_INVALID_INPUT;
            }
            else
            {
                RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s][rbusdml] Valid Parameter..! \n ", __FUNCTION__);

                HOSTIF_MsgData_t param = { 0 };
                param.reqType = HOSTIF_GET;
                strncpy (param.paramName, name, TR69HOSTIFMGR_MAX_PARAM_LEN - 1);
                convert2hostIfDataType(dmParam.dataType, &(param.paramtype));

                freeDataModelParam(dmParam);

                status = hostIf_GetMsgHandler(&param);


                if(status == OK && param.faultCode == fcNoFault)
                {
                    switch (param.paramtype)
                    {
                    case hostIf_IntegerType:
                        rbusValue_SetInt32(value, *((int *)param.paramValue));
                        break;
                    case hostIf_BooleanType:
                        rbusValue_SetBoolean(value, *((int *)param.paramValue));
                        break;
                    case hostIf_UnsignedIntType:
                        rbusValue_SetUInt32(value, *((unsigned int *)param.paramValue));
                        break;
                    case hostIf_UnsignedLongType:
                        rbusValue_SetUInt64(value, *((unsigned long long *)param.paramValue));
                        break;
                    case hostIf_StringType:
                        rbusValue_SetString(value, param.paramValue);
                        break;
                    default: // handle as string
                        rbusValue_SetString(value, param.paramValue);
                        break;
                    }
                    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s][rbusdml] Successfully return the get value for the Parameter [%s].\n", __FUNCTION__, name);
                    rbusProperty_SetValue(inProperty, value);
                    rc = RBUS_ERROR_SUCCESS;
                }
                else
                {
                    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s][rbusdml] Get Parameter [%s] Invalid format to send across.\n", __FUNCTION__, name);
                    rc = RBUS_ERROR_BUS_ERROR;
                }
            }
        }
    }

    rbusValue_Release(value);
    return rc;
}


void init_rbus_dml_provider()
{

    LOG_ENTRY_EXIT;

    /* Check for rbus status */
    rbusStatus_t rs;

    int max_retry = 6;
    int retry_count = 0;

    while(true) {
        rs = rbus_checkStatus();
        if(rs == RBUS_ENABLED)
        {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] [rbusdml] The rbus_checkStatus() is Enabled (%d). \n", __FUNCTION__, rs);
            break;
        }
        else {
            if(retry_count >= max_retry) {
                RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] [rbusdml] The rbus_checkStatus() is Not Enabled, exiting rbus support after max retry (%d). \n",__FUNCTION__, max_retry);
                break;
            }
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s][rbusdml] The rbus_checkStatus() is Not Enabled, returned with status (%d), so retrying... \n", __FUNCTION__, rs);
            sleep(5);
            retry_count++;
        }
    }

    if(rs != RBUS_ENABLED) {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s][rbusdml] Failed to initialized, rbus_checkStatus() returned with status (%d). \n", __FUNCTION__, rs);
        return;
    }

    rbusError_t rc = RBUS_ERROR_SUCCESS;
    rbusHandle_t rbusHandle = NULL;
    rbusDataElement_t* dataElements = NULL;
    int i = 0;

    rbus_open(&rbusHandle, "tr69hostif");

    char **pParamNameList = (char**) calloc (MAX_NUM_PARAMETERS, sizeof(char*));
    int num_of_params = 0;

    try {

        /* Get the list of properties that are supported by Data Model Xml */
        if(DB_SUCCESS != get_complete_param_list (pParamNameList, &num_of_params))
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s][rbusdml] Failed to get complete parameter list. The number of parameter is :%d\n", __FUNCTION__, num_of_params);
        }
        else
        {
            RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d][rbusdml]Successfully get the complete parameter list, the Parameter counts is = %d\n", __FUNCTION__, __LINE__, num_of_params);

            dataElements = (rbusDataElement_t*)calloc(num_of_params, sizeof(rbusDataElement_t));

            if(dataElements != NULL)
            {
                for(i=0; i< num_of_params; i++)
                {
                    dataElements[i].name = strdup(pParamNameList[i]);

                    if (strcmp(pParamNameList[i] + strlen(pParamNameList[i])-5,".{i}.") == 0 )
                    {
                        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF, "[%s] [rbusdml] Parameter [%d] : [ %s ] registered as [RBUS_ELEMENT_TYPE_TABLE]\n", __FUNCTION__, i +1, pParamNameList[i]);
                        dataElements[i].type = RBUS_ELEMENT_TYPE_TABLE;
                    }
                    else
                    {
                        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF, "[%s] [rbusdml]Parameter [%d] : [ %s ] registered as [RBUS_ELEMENT_TYPE_PROPERTY]\n", __FUNCTION__, i +1, pParamNameList[i]);
                        dataElements[i].type = RBUS_ELEMENT_TYPE_PROPERTY;
                        dataElements[i].cbTable.getHandler = TR_Dml_GetHandler;
                    }
                }

                rc = rbus_regDataElements(rbusHandle, num_of_params, dataElements);

                if(rc != RBUS_ERROR_SUCCESS)
                {
                    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF, "[%s][rbusdml] rbusPropertyProvider_Register failed: %d\n", __FUNCTION__, rc);
                    free(dataElements);
                    rbus_close(rbusHandle);
                }
                else
                {

                    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s][rbusdml] rbus_regDataElements registered successfully.\n", __FUNCTION__);
                    free(dataElements);
                }

                for (i=0; i< num_of_params; i++) {
                    if(pParamNameList[i]) free(pParamNameList[i]);
                }

                if(pParamNameList) free(pParamNameList);
            }
        }
    }
    catch (const std::exception e)
    {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s][rbusdml] Exception: %s\r\n",__FUNCTION__, e.what());
    }
}
