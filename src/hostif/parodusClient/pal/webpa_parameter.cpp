/**
 * @file webpa_parameter.cpp
 *
 * @description This file describes the Webpa Parameter Request Handling
 *
 * Copyright (c) 2017  Comcast
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
static WAL_STATUS GetParamInfo (const char *pParameterName, ParamVal **parametervalPtrPtr, int *paramCountPtr);
static WAL_STATUS get_ParamValues_tr69hostIf(HOSTIF_MsgData_t *ptrParam);
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
void getValues (const char *paramName[], const unsigned int paramCount, money_trace_spans *timeSpan, ParamVal ***paramValArr,
                int *retValCount, WAL_STATUS *retStatus)
{
    // Generic code mallocs paramValArr to hold paramCount items but only paramValArr[0] is ever accessed.
    RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "Inside GetValues \n ");
    // Generic code uses "paramValArr[0][cnt2]" (iterating over the 2nd dimension instead of the 1st). This means
    // paramValArr[0] (which is of type "ParamVal**") is expected to point to an array of "ParamVal*" objects
    paramValArr[0] = (ParamVal**)calloc (paramCount, sizeof(ParamVal*));

    int cnt = 0;
    int numParams = 0;
    for (cnt = 0; cnt < paramCount; cnt++)
    {
        // Because GetParamInfo is responsible for producing results (including wildcard explansion) for only 1 input
        // parameter, the address of the correct "ParamVal*" object from the above allocated array has to be given to
        // GetParamInfo for initialization. So GetParamInfo has to take a "ParamVal**" as input.
        retStatus[cnt] =  GetParamInfo (paramName[cnt], &paramValArr[0][cnt], &numParams);
        retValCount[cnt] = numParams;
        RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "Parameter Name: %s return: %d\n", paramName[cnt], retStatus[cnt]);
    }
}
/**
 * @brief setValues interface sets the parameter value.
 *
 * @param[in] paramVal List of Parameter name/value pairs.
 * @param[in] paramCount Number of parameters.
 * @param[out] retStatus List of Return status.
 */
void setValues(const ParamVal paramVal[], const unsigned int paramCount, const WEBPA_SET_TYPE setType, money_trace_spans *timeSpan, WAL_STATUS *retStatus,char * transaction_id)
{
    int cnt=0;
    for(cnt = 0; cnt < paramCount; cnt++)
    {
        retStatus[cnt] = SetParamInfo(paramVal[cnt]);
    }
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/

static WAL_STATUS GetParamInfo (const char *pParameterName, ParamVal **parametervalPtrPtr, int *paramCountPtr)
{
    //Check if pParameterName is in the tree and convert to a list if a wildcard/branch
    int i = 0;
    WAL_STATUS ret = WAL_SUCCESS;
    int dataBaseHandle = 0;
    DB_STATUS dbRet = DB_FAILURE;
    HOSTIF_MsgData_t Param = { 0 };
    memset(&Param, '\0', sizeof(HOSTIF_MsgData_t));
    RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "Inside GetParamInfo()\n" );

    dataBaseHandle = getDBHandle();
    if (dataBaseHandle)
    {
        ParamVal* parametervalPtr = NULL;
        if (isWildCardParam(pParameterName))
        {
            /* Translate wildcard to list of parameters */
            char **getParamList = (char**) calloc (MAX_NUM_PARAMETERS, sizeof(char*));
            char **ParamDataTypeList = (char**) calloc (MAX_NUM_PARAMETERS, sizeof(char*));
            if (getParamList == NULL || ParamDataTypeList == NULL)
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Error allocating memory\n");
                ret = WAL_FAILURE;
                goto exit0;
            }

            *paramCountPtr = 0;
            dbRet = getParameterList ((void *) dataBaseHandle,const_cast<char*> (pParameterName), getParamList, ParamDataTypeList, paramCountPtr);
            // allocate parametervalPtr as an array of ParamVal elements (wildcard case)
            parametervalPtr = (ParamVal*) calloc (*paramCountPtr, sizeof(ParamVal));
            if (parametervalPtr == NULL)
            {
                RDK_LOG (RDK_LOG_INFO, LOG_PARODUS_IF, "Error allocating memory\n");
                ret = WAL_FAILURE;
                goto exit0;
            }
            for (i = 0; i < *paramCountPtr; i++)
            {
                strncpy (Param.paramName, getParamList[i], MAX_PARAM_LENGTH - 1);
                Param.paramName[MAX_PARAM_LENGTH - 1] = '\0';

                // Convert ParamDataType to hostIf datatype
                converttohostIfType (ParamDataTypeList[i], &(Param.paramtype));
                Param.instanceNum = 0;

                // Convert Param.paramtype to ParamVal.type
                converttoWalType (Param.paramtype, &(parametervalPtr[i].type));
                get_ParamValues_tr69hostIf (&Param);

                parametervalPtr[i].name = (char*) calloc (MAX_PARAM_LENGTH, sizeof(char));
                parametervalPtr[i].value = (char*) calloc (MAX_PARAM_LENGTH, sizeof(char));
                if (parametervalPtr[i].name == NULL || parametervalPtr[i].value == NULL)
                {
                    RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Error allocating memory\n");
                    ret = WAL_FAILURE;
                    goto exit0;
                }

                strncat(parametervalPtr[i].name, Param.paramName, MAX_PARAM_LENGTH - 1);
                switch (Param.paramtype)
                {
                case hostIf_IntegerType:
                case hostIf_BooleanType:
                    snprintf (parametervalPtr[i].value, MAX_PARAM_LENGTH, "%d", *((int *) Param.paramValue));
                    break;
                case hostIf_UnsignedIntType:
                    snprintf (parametervalPtr[i].value, MAX_PARAM_LENGTH, "%u", *((unsigned int *) Param.paramValue));
                    break;
                case hostIf_UnsignedLongType:
                    snprintf (parametervalPtr[i].value, MAX_PARAM_LENGTH, "%u", *((unsigned long *) Param.paramValue));
                    break;
                case hostIf_StringType:
                    strncat (parametervalPtr[i].value, Param.paramValue, MAX_PARAM_LENGTH - 1);
                    break;
                default: // handle as string
                    strncat (parametervalPtr[i].value, Param.paramValue, MAX_PARAM_LENGTH - 1);
                    break;
                }

                free (getParamList[i]);
                free (ParamDataTypeList[i]);
                memset(&Param, '\0', sizeof(HOSTIF_MsgData_t));
            }
exit0:
            // For success case generic layer would free up parametervalPtr after consuming data
            if (ret != WAL_SUCCESS && parametervalPtr != NULL)
            {
                int j;
                for (j = 0; j < i; j++)
                {
                    if (parametervalPtr[j].name != NULL)
                    {
                        free (parametervalPtr[j].name);
                        parametervalPtr[j].name = NULL;
                    }
                    if (parametervalPtr[j].value != NULL)
                    {
                        free (parametervalPtr[j].value);
                        parametervalPtr[j].value = NULL;
                    }
                }
                free (parametervalPtr);
                parametervalPtr = NULL;
            }
            if (getParamList != NULL)
                free (getParamList);
            if (ParamDataTypeList != NULL)
                free (ParamDataTypeList);
        }
        else /* No wildcard, check whether given parameter is valid */
        {
            RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "Get Request for a Non-WildCard Parameter \n");
            char *dataType = (char*) calloc (MAX_DATATYPE_LENGTH, sizeof(char));
            if (dataType == NULL)
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Error allocating memory\n");
                ret = WAL_FAILURE;
                goto exit1;
            }
            // allocate parametervalPtr as a single ParamVal element (the usual case)
            parametervalPtr = (ParamVal*) calloc (1, sizeof(ParamVal));
            if (parametervalPtr == NULL)
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Error allocating memory\n");
                ret = WAL_FAILURE;
                goto exit1;
            }

            if (isParameterValid ((void *) dataBaseHandle,const_cast<char*> (pParameterName), dataType))
            {
                RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "Valid Parameter..! \n ");
                *paramCountPtr = 1;
                strncpy (Param.paramName, pParameterName, MAX_PARAM_LENGTH - 1);
                Param.paramName[MAX_PARAM_LENGTH - 1] = '\0';

                converttohostIfType (dataType, &(Param.paramtype));
                Param.instanceNum = 0;

                // Convert Param.paramtype to ParamVal.type
                RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "CMCSA:: GetParamInfo Param.paramtype is %d\n", Param.paramtype);
                converttoWalType (Param.paramtype, &(parametervalPtr->type));
                RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "CMCSA:: GetParamInfo parametervalPtr->type is %d\n",parametervalPtr->type);
                RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "CMCSA:: GetParamInfo\n");
                ret = get_ParamValues_tr69hostIf (&Param);
                if (ret == WAL_SUCCESS)
                {
                    parametervalPtr->name = (char*) calloc (MAX_PARAM_LENGTH, sizeof(char));
                    parametervalPtr->value = (char*) calloc (MAX_PARAM_LENGTH, sizeof(char));
                    if (parametervalPtr->name == NULL || parametervalPtr->value == NULL)
                    {
                        RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "Error allocating memory\n");
                        ret = WAL_FAILURE;
                        goto exit1;
                    }

                    strncat (parametervalPtr->name, Param.paramName, MAX_PARAM_LENGTH - 1);

                    switch (Param.paramtype)
                    {
                    case hostIf_IntegerType:
                    case hostIf_BooleanType:
                        snprintf (parametervalPtr->value, MAX_PARAM_LENGTH, "%d", *((int *) Param.paramValue));
                        break;
                    case hostIf_UnsignedIntType:
                        snprintf (parametervalPtr->value, MAX_PARAM_LENGTH, "%u", *((unsigned int *) Param.paramValue));
                        break;
                    case hostIf_UnsignedLongType:
                        snprintf (parametervalPtr->value, MAX_PARAM_LENGTH, "%u", *((unsigned long *) Param.paramValue));
                        break;
                    case hostIf_StringType:
                        strncat (parametervalPtr->value, Param.paramValue, MAX_PARAM_LENGTH - 1);
                        break;
                    default: // handle as string
                        strncat (parametervalPtr->value, Param.paramValue, MAX_PARAM_LENGTH - 1);
                        break;
                    }
                    RDK_LOG (RDK_LOG_DEBUG, LOG_PARODUS_IF, "CMCSA:: GetParamInfo value is %s\n", parametervalPtr->value);
                }
                else
                {
                    RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, "get_ParamValues_tr69hostIf failed:ret is %d\n", ret);
                    ret = WAL_FAILURE;
                }
            }
            else
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_PARODUS_IF, " Invalid Parameter name %s\n", pParameterName);
                ret = WAL_ERR_INVALID_PARAMETER_NAME;
            }
exit1:
            // For success case generic layer would free up parametervalPtr after consuming data
            if (ret != WAL_SUCCESS && parametervalPtr != NULL)
            {
                if (parametervalPtr->name != NULL)
                {
                    free (parametervalPtr->name);
                    parametervalPtr->name = NULL;
                }
                if (parametervalPtr->value != NULL)
                {
                    free (parametervalPtr->value);
                    parametervalPtr->value = NULL;
                }
                free (parametervalPtr);
                parametervalPtr = NULL;
            }
            if (dataType != NULL)
            {
                free (dataType);
                dataType = NULL;
            }
        }
        *parametervalPtrPtr = parametervalPtr;
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
    int dataBaseHandle = 0;
    HOSTIF_MsgData_t Param = {0};
    memset(&Param, '\0', sizeof(HOSTIF_MsgData_t));

    char *pdataType = NULL;
    pdataType = (char *)malloc(sizeof(char) * MAX_DATATYPE_LENGTH);
    if(pdataType == NULL)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"Error allocating memory\n");
        return WAL_FAILURE;
    }
    dataBaseHandle = getDBHandle();
    if(isParameterValid((void *)dataBaseHandle,paramVal.name,pdataType))
    {
        WAL_DATA_TYPE walType;
        converttohostIfType(pdataType,&(Param.paramtype));
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
    free (pdataType);
    return ret;
}

/**
 * generic Api for get HostIf parameters
 **/
static WAL_STATUS get_ParamValues_tr69hostIf(HOSTIF_MsgData_t *ptrParam)
{
    int status = -1;
    ptrParam->reqType = HOSTIF_GET;
    status = hostIf_GetMsgHandler(ptrParam);

    if(status != 0) {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"[%s:%s:%d] Error in Get Message Handler Status : %d\n", __FILE__, __FUNCTION__, __LINE__, status);
        return WAL_ERR_INVALID_PARAM;
    }
    else
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"[%s:%s:%d] The value for param: %s is %s paramLen : %d\n", __FILE__, __FUNCTION__, __LINE__, ptrParam->paramName,ptrParam->paramValue, ptrParam->paramLen);
    }
    return WAL_SUCCESS;
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

