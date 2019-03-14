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
 * @file webpa_parameter.cpp
 *
 * @description This file describes the Webpa Parameter Request Handling
 *
 */

#include <string.h>
#include <math.h>
#include <stdio.h>

#include "webpa_parameter.h"
#include "rdk_debug.h"
#include "waldb.h"
#include "hostIf_msgHandler.h"


/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
static WDMP_STATUS GetParamInfo (const char *pParameterName, param_t ***parametervalPtrPtr, int *paramCountPtr,int paramIndex);
static WDMP_STATUS get_ParamValues_tr69hostIf(HOSTIF_MsgData_t *ptrParam);
int isWildCardParam(const char *paramName);
static void converttohostIfType(char *ParamDataType,HostIf_ParamType_t* pParamType);
static void converttoWalType(HostIf_ParamType_t paramType,WAL_DATA_TYPE* pwalType);
static WAL_STATUS SetParamInfo(ParamVal paramVal);
static WAL_STATUS set_ParamValues_tr69hostIf (HOSTIF_MsgData_t param);
static WAL_STATUS convertFaultCodeToWalStatus(faultCode_t faultCode);

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/



/**
 * @brief getValues interface returns the parameter values.
 *
 * getValues supports an option to pass wildward parameters. This can be achieved by passing an object name followed by '.'
 * instead of parameter name.
 *
 * @param[in] paramName List of Parameters.
 * @param[in] paramCount Number of parameters.
 * @param[out] paramValArr Two dimentional array of parameter name/value pairs.
 * @param[out] retValCount List of "number of parameters" for each input paramName. Usually retValCount will be 1 except
 * for wildcards request where it represents the number of param/value pairs retrieved for the particular wildcard parameter.
 * @param[out] retStatus List of Return status.
 */
void getValues (const char *paramName[], const unsigned int paramCount, param_t ***paramValArr,size_t **retValCount, WDMP_STATUS **retStatus)
{
    // Generic code mallocs paramValArr to hold paramCount items but only paramValArr[0] is ever accessed.
    // Generic code uses "paramValArr[0][cnt2]" (iterating over the 2nd dimension instead of the 1st). This means
    // paramValArr[0] (which is of type "ParamVal**") is expected to point to an array of "ParamVal*" objects
    int cnt = 0;
    int numParams = 0;
    for (cnt = 0; cnt < paramCount; cnt++)
    {
        // GetParamInfo is responsible for generating the output response including wild card. Allocate the memory for
        (*retStatus)[cnt] =  GetParamInfo (paramName[cnt], paramValArr, &numParams,cnt);
        (*retValCount)[cnt] = numParams;
        RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "Parameter Name: %s return: %d\n", paramName[cnt], (*retStatus)[cnt]);
    }
}
/**
 * @brief setValues interface sets the parameter value.
 *
 * @param[in] paramVal List of Parameter name/value pairs.
 * @param[in] paramCount Number of parameters.
 * @param[out] retStatus List of Return status.
 */
void setValues(const ParamVal paramVal[], const unsigned int paramCount, const WEBPA_SET_TYPE setType, money_trace_spans *timeSpan, WDMP_STATUS **retStatus,char * transaction_id)
{
    int cnt=0;
    for(cnt = 0; cnt < paramCount; cnt++)
    {
        (*retStatus)[cnt] = (WDMP_STATUS) SetParamInfo(paramVal[cnt]);
    }
}

void freeDataModelParam(DataModelParam dmParam)
{
	if(dmParam.objectName)
		free(dmParam.objectName);
	if(dmParam.paramName)
		free(dmParam.paramName);
	if(dmParam.access)
		free(dmParam.access);
	if(dmParam.dataType)
		free(dmParam.dataType);
	if(dmParam.defaultValue)
		free(dmParam.defaultValue);
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/

static WDMP_STATUS GetParamInfo (const char *pParameterName, param_t ***parametervalPtrPtr, int *paramCountPtr,int index)
{

    void *dataBaseHandle = NULL;
    DB_STATUS dbRet = DB_FAILURE;
    HOSTIF_MsgData_t Param = { 0 };
    WDMP_STATUS ret = WDMP_FAILURE;

    // Memset to 0
    memset(&Param, '\0', sizeof(HOSTIF_MsgData_t));

    // Get DB handle
    dataBaseHandle = getDataModelHandle();
    if (dataBaseHandle)
    {
        if (isWildCardParam(pParameterName)) // It is a wildcard Param
        {
            /* Translate wildcard to list of parameters */
            char **getParamList = (char**) calloc (MAX_NUM_PARAMETERS, sizeof(char*));
            if (NULL == getParamList)
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Error allocating memory\n");
                return WDMP_FAILURE;
            }
            char **ParamDataTypeList = (char**) calloc (MAX_NUM_PARAMETERS, sizeof(char*));
            if(NULL == ParamDataTypeList)
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Error allocating memory\n");
                free(getParamList);
                return WDMP_FAILURE;
            }
            *paramCountPtr = 0;
            int wc_cnt = 0;
            WDMP_STATUS getRet = WDMP_FAILURE;
            int successWcCnt = 0;

            dbRet = getChildParamNamesFromDataModel((void *) dataBaseHandle,const_cast<char*> (pParameterName), getParamList, ParamDataTypeList, paramCountPtr);
            if(*paramCountPtr != 0 && dbRet == DB_SUCCESS)
            {
                (*parametervalPtrPtr)[index] = (param_t *) calloc(sizeof(param_t),*paramCountPtr);
                for(wc_cnt = 0; wc_cnt < *paramCountPtr; wc_cnt++)
                {
                    strncpy (Param.paramName, getParamList[wc_cnt], MAX_PARAM_LENGTH - 1);
                    free(getParamList[wc_cnt]);
                    Param.paramName[MAX_PARAM_LENGTH - 1] = '\0';

                    // Convert ParamDataType to hostIf datatype
                    converttohostIfType (ParamDataTypeList[wc_cnt], &(Param.paramtype));
                    Param.instanceNum = 0;
                    free(ParamDataTypeList[wc_cnt]);

                    // Initialize Name and Value to NULL
                    (*parametervalPtrPtr)[index][wc_cnt].name = NULL;
                    (*parametervalPtrPtr)[index][wc_cnt].value = NULL;

                    // Convert Param.paramtype to ParamVal.type
                    converttoWalType (Param.paramtype, (WAL_DATA_TYPE *) &(*parametervalPtrPtr)[index][wc_cnt].type);
                    getRet = get_ParamValues_tr69hostIf (&Param);

                    // Fill Only if we can able to get Proper value
                    if(WDMP_SUCCESS == getRet)
                    {
                        (*parametervalPtrPtr)[index][successWcCnt].name = (char*) calloc (MAX_PARAM_LENGTH, sizeof(char));
                        (*parametervalPtrPtr)[index][successWcCnt].value = (char*) calloc (MAX_PARAM_LENGTH, sizeof(char));
                        if ((*parametervalPtrPtr)[index][successWcCnt].name == NULL || (*parametervalPtrPtr)[index][successWcCnt].value == NULL)
                        {
                            RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Error allocating memory\n");
                            ret = WDMP_FAILURE;
                            break;
                        }
                        // Copy Param Name
                        strncat((*parametervalPtrPtr)[index][successWcCnt].name, Param.paramName, MAX_PARAM_LENGTH - 1);

                        // Copy param value
                        switch (Param.paramtype)
                        {
                        case hostIf_IntegerType:
                        case hostIf_BooleanType:
                            snprintf ((*parametervalPtrPtr)[index][successWcCnt].value, MAX_PARAM_LENGTH, "%d", *((int *) Param.paramValue));
                            break;
                        case hostIf_UnsignedIntType:
                            snprintf ((*parametervalPtrPtr)[index][successWcCnt].value, MAX_PARAM_LENGTH, "%u", *((unsigned int *) Param.paramValue));
                            break;
                        case hostIf_UnsignedLongType:
                            snprintf ((*parametervalPtrPtr)[index][successWcCnt].value, MAX_PARAM_LENGTH, "%u", *((unsigned long *) Param.paramValue));
                            break;
                        case hostIf_StringType:
                            strncat ((*parametervalPtrPtr)[index][successWcCnt].value, Param.paramValue, MAX_PARAM_LENGTH - 1);
                            break;
                        default: // handle as string
                            strncat ((*parametervalPtrPtr)[index][successWcCnt].value, Param.paramValue, MAX_PARAM_LENGTH - 1);
                            break;
                        }
                        memset(&Param, '\0', sizeof(HOSTIF_MsgData_t));
                        successWcCnt++;
                    }
                } // End of Wild card for loop
                // Lets Free GetParameter List
                free(getParamList);
                free(ParamDataTypeList);
                *paramCountPtr = successWcCnt;
                ret = WDMP_SUCCESS;
            }
            else
            {
                free(getParamList);
                free(ParamDataTypeList);
                RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, " Wild card Param list is empty\n");
                ret = WDMP_FAILURE;
            }
        }
        else // Not a wildcard Parameter Lets fill it
        {
            RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "Get Request for a Non-WildCard Parameter \n");
            *paramCountPtr = 1;
            DataModelParam dmParam = {0};

            // allocate parametervalPtr as a single param_t and initialize varribales
            (*parametervalPtrPtr)[index] = (param_t*) calloc (1, sizeof(param_t));
            (*parametervalPtrPtr)[index][0].name = (char*) calloc (strlen(pParameterName)+1, sizeof(char));
            (*parametervalPtrPtr)[index][0].value = NULL;
            (*parametervalPtrPtr)[index][0].type = WDMP_STRING;

            if (NULL == (*parametervalPtrPtr)[index])
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Error allocating memory\n");
                ret = WDMP_FAILURE;
            }
            else if (getParamInfoFromDataModel(dataBaseHandle, const_cast<char*> (pParameterName), &dmParam))
            {
                RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "Valid Parameter..! \n ");
                strncpy (Param.paramName, pParameterName, MAX_PARAM_LENGTH - 1);
                Param.paramName[MAX_PARAM_LENGTH - 1] = '\0';

                converttohostIfType (dmParam.dataType, &(Param.paramtype));
                freeDataModelParam(dmParam);
                Param.instanceNum = 0;

                // Convert Param.paramtype to ParamVal.type
                RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "CMCSA:: GetParamInfo Param.paramtype is %d\n", Param.paramtype);
                converttoWalType (Param.paramtype, (WAL_DATA_TYPE *) &(*parametervalPtrPtr)[index][0].type);
                RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "CMCSA:: GetParamInfo parametervalPtr->type is %d\n",((*parametervalPtrPtr)[index][0].type));
                RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "CMCSA:: GetParamInfo\n");
                ret = get_ParamValues_tr69hostIf (&Param);
                if (ret == WDMP_SUCCESS)
                {
                    (*parametervalPtrPtr)[index][0].value = (char*) calloc (MAX_PARAM_LENGTH, sizeof(char));
                    if (NULL == (*parametervalPtrPtr)[index][0].name || NULL == (*parametervalPtrPtr)[index][0].value)
                    {
                        RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Error allocating memory\n");
                        ret = WDMP_FAILURE;
                    }
                    else {
                        strncpy((*parametervalPtrPtr)[index][0].name, Param.paramName, strlen(pParameterName)+1);
                        switch (Param.paramtype)
                        {
                        case hostIf_IntegerType:
                        case hostIf_BooleanType:
                            snprintf ((*parametervalPtrPtr)[index][0].value, MAX_PARAM_LENGTH, "%d", *((int *) Param.paramValue));
                            break;
                        case hostIf_UnsignedIntType:
                            snprintf ((*parametervalPtrPtr)[index][0].value, MAX_PARAM_LENGTH, "%u", *((unsigned int *) Param.paramValue));
                            break;
                        case hostIf_UnsignedLongType:
                            snprintf ((*parametervalPtrPtr)[index][0].value, MAX_PARAM_LENGTH, "%u", *((unsigned long *) Param.paramValue));
                            break;
                        case hostIf_StringType:
                            strncat ((*parametervalPtrPtr)[index][0].value, Param.paramValue, MAX_PARAM_LENGTH - 1);
                            break;
                        default: // handle as string
                            strncat ((*parametervalPtrPtr)[index][0].value, Param.paramValue, MAX_PARAM_LENGTH - 1);
                            break;
                        }
                        RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "CMCSA:: GetParamInfo value is %s\n", (*parametervalPtrPtr)[index][0].value);
                        ret = WDMP_SUCCESS;
                    }
                }
                else
                {
                    RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Failed get_ParamValues_tr69hostIf() Param Name :-  %s \n",pParameterName);                   
                    *paramCountPtr = 0;
                }
            }
            else
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Invalid Parameter Name  :-  %s \n",pParameterName);
                (*parametervalPtrPtr)[index][0].value = strdup("");
                ret = WDMP_ERR_INVALID_PARAMETER_NAME;
            }
        }
    }
    else
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Data base Handle is not Initialized %s\n", pParameterName);
    }
    return ret;
}

static WAL_STATUS SetParamInfo(ParamVal paramVal)
{
    WAL_STATUS ret = WAL_SUCCESS;
    void *dataBaseHandle = NULL;
    HOSTIF_MsgData_t Param = {0};
    DataModelParam dmParam = {0};
    memset(&Param, '\0', sizeof(HOSTIF_MsgData_t));

    dataBaseHandle = getDataModelHandle();
    if(!dataBaseHandle)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"Data Model Not Initialized... Unable to process the SET operation\n");
        return WAL_FAILURE;
    }

    if(getParamInfoFromDataModel((void *)dataBaseHandle,paramVal.name,&dmParam))
    {
        WAL_DATA_TYPE walType;
        converttohostIfType(dmParam.dataType,&(Param.paramtype));
        freeDataModelParam(dmParam);
        // Convert Param.paramtype to ParamVal.type
        converttoWalType(Param.paramtype,&walType);

        if(walType != paramVal.type)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF," Invalid Parameter type for %s\n",paramVal.name);
            return WAL_ERR_INVALID_PARAMETER_TYPE;
        }

        strncpy(Param.paramName, paramVal.name,MAX_PARAM_LENGTH-1);
        Param.paramName[MAX_PARAM_LENGTH-1]='\0';

        if (Param.paramtype == hostIf_BooleanType)
        {
            bool* boolPtr = (bool*) Param.paramValue;
            if (strcmp (paramVal.value, "1") == 0 || strcasecmp (paramVal.value, "true") == 0)
            {
                *boolPtr = 1;
            }
            else if (strcmp (paramVal.value, "0") == 0 || strcasecmp (paramVal.value, "false") == 0)
            {
                *boolPtr = 0;
            }
            else
            {
                return WAL_ERR_INVALID_PARAMETER_VALUE;
            }
        }
        else if (Param.paramtype == hostIf_IntegerType)
        {
            char *tailPtr;
            long int value = (int) strtol (paramVal.value, &tailPtr, 10);
            if (strlen (tailPtr)) // "whole" string cannot be interpreted as integer
                return WAL_ERR_INVALID_PARAMETER_VALUE;
            *((int*) Param.paramValue) = (int) value;
        }
        else if (Param.paramtype == hostIf_UnsignedIntType)
        {
            char *tailPtr;
            long int value = (int) strtol (paramVal.value, &tailPtr, 10);
            if (strlen (tailPtr) || value < 0) // "whole" string cannot be interpreted as unsigned integer
                return WAL_ERR_INVALID_PARAMETER_VALUE;
            *((int*) Param.paramValue) = (int) value;
        }
        else
        {
            strncpy(Param.paramValue, paramVal.value,MAX_PARAM_LENGTH-1);
            Param.paramValue[MAX_PARAM_LENGTH-1]='\0';
        }

        ret = set_ParamValues_tr69hostIf(Param);
        RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"set_ParamValues_tr69hostIf %d\n",ret);
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF," Invalid Parameter name %s\n",paramVal.name);
        ret = WAL_ERR_INVALID_PARAMETER_NAME;
    }
    return ret;
}

/**
 * generic Api for get HostIf parameters
 **/
static WDMP_STATUS get_ParamValues_tr69hostIf(HOSTIF_MsgData_t *ptrParam)
{
    int status = -1;
    ptrParam->reqType = HOSTIF_GET;
    WDMP_STATUS retStatus = WDMP_FAILURE;    
    status = hostIf_GetMsgHandler(ptrParam);

    if(status != 0) {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"[%s:%s:%d] Error in Get Message Handler : %d\n", __FILE__, __FUNCTION__, __LINE__, status);
        retStatus =(WDMP_STATUS) convertFaultCodeToWalStatus(ptrParam->faultCode); // returning appropriate fault code for get
        RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"[%s:%d] return status of fault code: %d\n", __FUNCTION__, __LINE__, retStatus);            
    }
    else
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"[%s:%s:%d] The value for param: %s is %s paramLen : %d\n", __FILE__, __FUNCTION__, __LINE__, ptrParam->paramName,ptrParam->paramValue, ptrParam->paramLen);
        retStatus = WDMP_SUCCESS;
    }
    return retStatus;
}

/**
 * generic Api for get HostIf parameters through IARM_TR69Bus
 **/
static WAL_STATUS set_ParamValues_tr69hostIf (HOSTIF_MsgData_t ptrParam)
{
    int status = -1;
    WAL_STATUS retStatus = WAL_FAILURE;

    ptrParam.reqType = HOSTIF_SET;
    status = hostIf_SetMsgHandler(&ptrParam);
    if(status != 0) {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"[%s:%s:%d] Error in Set Message Handler Status : %d\n", __FILE__, __FUNCTION__, __LINE__, status);
        retStatus = convertFaultCodeToWalStatus(ptrParam.faultCode);
    }
    else
    {
        retStatus = WAL_SUCCESS;
        RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"[%s:%s:%d] The value for param: %s is %s paramLen : %d\n", __FILE__, __FUNCTION__, __LINE__, ptrParam.paramName,ptrParam.paramValue, ptrParam.paramLen);
    }
    return retStatus;
}

static WAL_STATUS convertFaultCodeToWalStatus(faultCode_t faultCode)
{
    WAL_STATUS retWalStatus = WAL_FAILURE;
    switch(faultCode)
    {
    case fcNoFault:
    case fcRequestDenied:
        retWalStatus = WAL_FAILURE;
        break;
    case fcAttemptToSetaNonWritableParameter:
        retWalStatus = WAL_ERR_NOT_WRITABLE;
        break;
    case fcInvalidParameterName:
        retWalStatus = WAL_ERR_INVALID_PARAMETER_NAME;
        break;
    case fcInvalidParameterType:
        retWalStatus = WAL_ERR_INVALID_PARAMETER_TYPE;
        break;
    case fcInvalidParameterValue:
        retWalStatus = WAL_ERR_INVALID_PARAMETER_VALUE;
        break;
    default:
        retWalStatus = WAL_FAILURE;
        break;
    }
    return retWalStatus;
}

/**
 * @brief Check if Parameter Name ends with . If yes it is a wild card param
 *
 * @param[in] paramName Name of the Parameter.
 * @param[out] retValue 0 if present and 1 if not
 */
int isWildCardParam(const char *paramName)
{
    int isWildCard = 0;
    if(NULL != paramName)
    {
        if(!strcmp(paramName+strlen(paramName)-1,"."))
        {
            isWildCard = 1;
        }
    }
    return isWildCard;
}

static void converttohostIfType(char *ParamDataType,HostIf_ParamType_t* pParamType)
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
static void converttoWalType(HostIf_ParamType_t paramType,WAL_DATA_TYPE* pwalType)
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Inside converttoWalType \n");
    switch(paramType)
    {
    case hostIf_StringType:
        *pwalType = WAL_STRING;
        break;
    case hostIf_UnsignedIntType:
        *pwalType = WAL_UINT;
        break;
    case hostIf_IntegerType:
        *pwalType = WAL_INT;
        break;
    case hostIf_BooleanType:
        *pwalType = WAL_BOOLEAN;
        break;
    case hostIf_UnsignedLongType:
        *pwalType = WAL_ULONG;
        break;
    case hostIf_DateTimeType:
        *pwalType = WAL_DATETIME;
        break;
    default:
        *pwalType = WAL_STRING;
        break;
    }
}

