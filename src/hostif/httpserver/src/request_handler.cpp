/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2018 RDK Management
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

#include <stdlib.h>
#ifdef __cplusplus
extern "C"
{
#endif
#include <wdmp-c.h>
#ifdef __cplusplus
}
#endif

#include "hostIf_main.h"
#include "hostIf_msgHandler.h"
#include "request_handler.h"
#include "waldb.h"
#include "XrdkCentralComRFCVar.h"
#include "hostIf_utils.h"

#define MAX_NUM_PARAMETERS 2048
#define DEVICE_REBOOT_PARAM          "Device.X_CISCO_COM_DeviceControl.RebootDevice"

static DATA_TYPE getWdmpDataType(char *dmParamDataType)
{
    DATA_TYPE wdmpDataType = WDMP_NONE;

    if(!strcmp(dmParamDataType, "string"))
        wdmpDataType = WDMP_STRING;
    else if(!strcmp(dmParamDataType, "boolean"))
        wdmpDataType = WDMP_BOOLEAN;
    else if(!strcmp(dmParamDataType, "unsignedInt"))
        wdmpDataType = WDMP_UINT;
    else if(!strcmp(dmParamDataType, "int"))
        wdmpDataType = WDMP_INT;
    else if(!strcmp(dmParamDataType, "unsignedLong"))
        wdmpDataType = WDMP_ULONG;
    else if(!strcmp(dmParamDataType, "dataTime"))
        wdmpDataType = WDMP_DATETIME;
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Unknown data type identified...\n");
        wdmpDataType = WDMP_NONE;
    }
    return wdmpDataType;
}

static HostIf_ParamType_t getHostIfParamType(DATA_TYPE wdmpDataType)
{
    HostIf_ParamType_t hostIfDataType = hostIf_StringType;

    switch(wdmpDataType)
    {
        case WDMP_STRING:
            hostIfDataType = hostIf_StringType;
            break;

        case WDMP_INT:
            hostIfDataType = hostIf_IntegerType;
            break;

        case WDMP_UINT:
        case WDMP_ULONG:
            hostIfDataType = hostIf_UnsignedIntType;
            break;

        case WDMP_BOOLEAN:
            hostIfDataType = hostIf_BooleanType;
            break;

        case WDMP_DATETIME:
            hostIfDataType = hostIf_DateTimeType;
            break;

        case WDMP_BASE64:
        case WDMP_LONG:
        case WDMP_FLOAT:
        case WDMP_DOUBLE:
        case WDMP_BYTE:
        default:
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"No matching hostif datatype - default to String\n");
            break;
    };
    return hostIfDataType;
}

static void convertAndAssignParamValue (HOSTIF_MsgData_t *param, char *value)
{
    switch (param->paramtype)
    {
        case hostIf_StringType:
        case hostIf_DateTimeType:
            snprintf(param->paramValue,TR69HOSTIFMGR_MAX_PARAM_LEN, "%s", value);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF, ">> Type: String/Date, Value = %s\n", param->paramValue);
            break;
        case hostIf_IntegerType:
        case hostIf_UnsignedIntType:
            {
                int ivalue = atoi(value);
                int *iptr = (int *)param->paramValue;
                *iptr = ivalue;
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF, ">> Type: Integer/UnsignedInt, Value = %d\n", param->paramValue);
            }
            break;
        case hostIf_UnsignedLongType:
            {
                long lvalue = atol(value);
                long *lptr = (long *)param->paramValue;
                *lptr = lvalue;
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF, ">> Type: UnsignedLong, Value = %ul\n", param->paramValue);
            }
            break;
        case hostIf_BooleanType:
            {
                bool *bptr = (bool *)param->paramValue;
                *bptr = (0 == strncasecmp(value, "TRUE", 4)|| (isdigit(value[0]) && value[0] != '0' ));
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF, ">> Type: Boolean, Value = %d\n", param->paramValue);
            }
            break;
        default:
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF, ">> This path should never be reached. param type is %d\n", param->paramtype);
    }
}
static char* getStringValue(HostIf_ParamType_t paramType, char *value)
{
    char tempValue[TR69HOSTIFMGR_MAX_PARAM_LEN] = {'\0'};
    switch (paramType)
    {
        case hostIf_StringType:
        case hostIf_DateTimeType:
            snprintf(tempValue,TR69HOSTIFMGR_MAX_PARAM_LEN, "%s", value);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF, ">> String value of String = %s\n", tempValue);
            break;
        case hostIf_IntegerType:
        case hostIf_UnsignedIntType:
            {
                snprintf(tempValue,TR69HOSTIFMGR_MAX_PARAM_LEN, "%d", *((int *)value));
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF, ">> String value of Integer/UnsignedInt = %s\n", tempValue);
            }
            break;
        case hostIf_UnsignedLongType:
            {
                snprintf(tempValue,TR69HOSTIFMGR_MAX_PARAM_LEN, "%lu", *((unsigned long *)value));
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF, ">> String value of UnsignedLong = %s\n", tempValue);
            }
            break;
        case hostIf_BooleanType:
            {
                if(*((bool*)value) == true)
                    strncpy(tempValue, "true", 4);
                else
                    strncpy(tempValue, "false", 5);
                RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF, ">> String value of Boolean = %s\n", tempValue);
            }
            break;
        default:
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF, ">> This path should never be reached. param type is %d\n", paramType);
    }
    return strdup(tempValue);
}

static bool validateParamValue(const string &paramValue, HostIf_ParamType_t dataType)
{
    bool ret = true;
    size_t index = 0;
    switch(dataType)
    {
        case hostIf_StringType:
        case hostIf_DateTimeType:
            if(paramValue.length() > TR69HOSTIFMGR_MAX_PARAM_LEN)
            {
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Parameter Value greater than allowed %d\n", TR69HOSTIFMGR_MAX_PARAM_LEN);
                ret = false;
            }
            break;

        case hostIf_IntegerType:
            if(isdigit(paramValue[0]) || (paramValue[0] == '-' && isdigit(paramValue[1])))
            {
                int value = stoi(paramValue, &index);
                if(index != paramValue.length())
                {
                    RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Invalid Parameter Value for an Integer Type\n");
                    ret = false;
                }
            }
            else
            {
                ret = false;
            }
            break;

        case hostIf_UnsignedIntType:
        case hostIf_UnsignedLongType:
            if(isdigit(paramValue[0]))
            {
                unsigned long value = stoul(paramValue, &index);
                if(index != paramValue.length())
                {
                    RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Invalid Parameter Value for an UnsignedInt or UnsignedLong Type\n");
                    ret = false;
                }
            }
            else
            {
                ret = false;
            }
            break;

        case hostIf_BooleanType:
            if(!strcasecmp(paramValue.c_str(), "true")
                    || !strcasecmp(paramValue.c_str(), "false")
                    || !strcmp(paramValue.c_str(), "1")
                    || !strcmp(paramValue.c_str(), "0"))
            {
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Valid Boolean Value\n");
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Invalid Parameter Value for a Boolean\n");
                ret = false;
            }
            break;

        default:
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Unknown Data Type\n");
            ret = false;
            break;
    };

    return ret;
}

static void getHostIfParamStFromRequest(REQ_TYPE reqType, param_t *param, HOSTIF_MsgData_t *hostIfParam)
{
   strncpy(hostIfParam->paramName, param->name, strlen(param->name));
   hostIfParam->paramName[strlen(param->name)] = '\0';

   RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"Entering %s\n", __FUNCTION__);
   switch(reqType)
   {
        case GET:
            hostIfParam->reqType = HOSTIF_GET;
            hostIfParam->paramtype = getHostIfParamType(param->type);
             break;

        case SET:
            hostIfParam->reqType = HOSTIF_SET;
            hostIfParam->paramtype = getHostIfParamType(param->type);
            convertAndAssignParamValue(hostIfParam, param->value);
            break;

        default:
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s : Operation not handled : Shouldn't come here\n", __FUNCTION__);
            break;
   };

   RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"Leaving %s\n", __FUNCTION__);
}

static WDMP_STATUS handleRFCRequest(REQ_TYPE reqType, param_t *param)
{
   WDMP_STATUS wdmpStatus = WDMP_SUCCESS;

   RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"Entering %s\n", __FUNCTION__);

   switch(reqType)
   {
      case GET:
         param->value = strdup(XRFCVarStore::getInstance()->getValue(param->name).c_str());
         if(strlen(param->value) == 0)
             wdmpStatus = WDMP_ERR_VALUE_IS_NULL;
         break;

      case SET:
         if(!strcmp(param->name, XRFC_VAR_STORE_RELOADCACHE) && (!strcasecmp(param->value, "true") || !strcmp(param->value, "1")))
             XRFCVarStore::getInstance()->reloadCache();
         else
         {
             RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"SET operation not supported\n");
             wdmpStatus = WDMP_ERR_METHOD_NOT_SUPPORTED;
         }
         break;

      default:
         RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s : Invalid operation - Not Supported\n", __FUNCTION__);
   };
   RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"Leaving %s\n", __FUNCTION__);
   return wdmpStatus;
}

static WDMP_STATUS invokeHostIfAPI(REQ_TYPE reqType, param_t *param, HostIf_Source_Type_t bsUpdate, const char *pcCallerID)
{
   WDMP_STATUS wdmpStatus = WDMP_SUCCESS;
   int result = NOK;
   HOSTIF_MsgData_t hostIfParam;
   memset(&hostIfParam,0,sizeof(HOSTIF_MsgData_t));
   if (strcmp(pcCallerID, "rfc") == 0)
      hostIfParam.requestor = HOSTIF_SRC_RFC;
   else
      hostIfParam.requestor = HOSTIF_SRC_WEBPA;
   hostIfParam.bsUpdate = bsUpdate;
   RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"Entering %s\n", __FUNCTION__);

   getHostIfParamStFromRequest(reqType, param, &hostIfParam);

   switch(reqType)
   {
      case GET:
            result = hostIf_GetMsgHandler(&hostIfParam);
            break;

      case SET:
            if(!validateParamValue(param->value, getHostIfParamType(param->type)))
            {
                result = fcInvalidParameterValue;
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Invalid Parameter Value\n");
            }
            else
                result = hostIf_SetMsgHandler(&hostIfParam);
            break;

      default:
         RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s : Invalid operation - Not Supported\n", __FUNCTION__);
   };

   if(result == OK)
   {
        if(hostIfParam.faultCode == fcNoFault)
        {
        	param->value = getStringValue(hostIfParam.paramtype, hostIfParam.paramValue);
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"The value for param: %s = %s\n", hostIfParam.paramName, param->value);
        }
        else
        {
            param->value = strdup("");
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error in HostIf : %d\n", hostIfParam.faultCode);
            wdmpStatus = WDMP_FAILURE;
        }
   }
   else
   {
      param->value = strdup("");
      RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed in hostIf_msgHandler, with return value: %d\n", result);
      if(result == fcInvalidParameterValue)
         wdmpStatus = WDMP_ERR_INVALID_PARAMETER_VALUE;
      else
         wdmpStatus = WDMP_ERR_INTERNAL_ERROR;
   }

   RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"Leaving %s\n", __FUNCTION__);
   return wdmpStatus;
}

static WDMP_STATUS validateAgainstDataModel(REQ_TYPE reqType, char* paramName, const char* paramValue, DATA_TYPE *dataType, char **defaultValue, HostIf_Source_Type_t *bsUpdate)
{
   DataModelParam dmParam = {0};
   WDMP_STATUS ret = WDMP_SUCCESS;

   RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"Entering... %s\n", __FUNCTION__);

   const char* dbParamName = paramName;
   if(!getParamInfoFromDataModel(getDataModelHandle(), dbParamName, &dmParam))
   {
      RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Invalid parameter name %s: doesn't exist in data-model\n", paramName);
      return WDMP_ERR_INVALID_PARAMETER_NAME;
   }
   *bsUpdate = getBSUpdateEnum(dmParam.bsUpdate);
   if(reqType == GET)
   {
       *dataType = getWdmpDataType(dmParam.dataType);
       if(dmParam.defaultValue)
          *defaultValue = strdup(dmParam.defaultValue);
   }
   else if(reqType == SET)
   {
      RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Validating paramValue and dataType against data-model\n");
      if(paramValue == NULL || *dataType >= WDMP_NONE)
      {
         RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"ParameterValue is NULL or Unknown Datatype\n");
         ret = WDMP_ERR_VALUE_IS_NULL;
      }
      else if(dmParam.access != NULL && !strcasecmp(dmParam.access, "readOnly"))
      {
         RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Parameter is readOnly\n");
         ret = WDMP_ERR_NOT_WRITABLE;
      }
      else if (getWdmpDataType(dmParam.dataType) != *dataType)
      {
         RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Datatype doesn't match\n");
         ret = WDMP_ERR_INVALID_PARAMETER_TYPE;
      }
   }
   else
   {
       RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s : Invalid operation - Not Supported\n", __FUNCTION__);
       ret = WDMP_FAILURE;
   }

   if(dmParam.objectName)
      free(dmParam.objectName);
   if(dmParam.paramName)
      free(dmParam.paramName);
   if(dmParam.bsUpdate)
      free(dmParam.bsUpdate);
   if(dmParam.access)
      free(dmParam.access);
   if(dmParam.dataType)
      free(dmParam.dataType);
   if(dmParam.defaultValue)
      free(dmParam.defaultValue);

   RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"Leaving... %s\n", __FUNCTION__);
   return ret;
}

static void freeParam(param_t *param)
{
   if(param)
   {
      free(param->name);
      free(param->value);
      free(param);
   }
}

static bool isAuthorized(const char* pcCallerID, const char* pcParamName)
{
    if(!strcmp(pcParamName, DEVICE_REBOOT_PARAM))
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"%s : Invalid operation - Device Reboot Not Allowed Using tr181 command/RFC API\n", __FUNCTION__);
        return false;
    }
    else
        return true;
}

static bool allocate_res_struct_members(res_struct *respSt, req_struct *reqSt)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"Entering... %s\n", __FUNCTION__);
    respSt->reqType = reqSt->reqType;
    respSt->timeSpan = NULL;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"Allocating res_struct for reqType : ... %d\n", reqSt->reqType);
    switch(reqSt->reqType)
    {
        case GET:
            if(reqSt->u.getReq->paramCnt == 0)
               return false;

            respSt->paramCnt = reqSt->u.getReq->paramCnt;
            respSt->retStatus = (WDMP_STATUS *)calloc(respSt->paramCnt, sizeof(WDMP_STATUS));
            respSt->u.getRes = (get_res_t *)malloc(sizeof(get_res_t));
            if(respSt->u.getRes != NULL && respSt->retStatus != NULL)
            {
                memset(respSt->u.getRes, 0, sizeof(get_res_t));

                respSt->u.getRes->paramCnt = reqSt->u.getReq->paramCnt;
                respSt->u.getRes->paramNames = (char **)malloc(sizeof(char *) * (reqSt->u.getReq->paramCnt));
                respSt->u.getRes->params = (param_t **)malloc(sizeof(param_t *) * reqSt->u.getReq->paramCnt);
                respSt->u.getRes->retParamCnt = (size_t *)malloc(sizeof(size_t) * reqSt->u.getReq->paramCnt);
                
                if(respSt->u.getRes->paramNames == NULL || respSt->u.getRes->params == NULL || respSt->u.getRes->retParamCnt == NULL)
                {
                    if(respSt->u.getRes->paramNames) free(respSt->u.getRes->paramNames);
                    if(respSt->u.getRes->params) free(respSt->u.getRes->params);
                    if(respSt->u.getRes->retParamCnt) free(respSt->u.getRes->retParamCnt);
                    free(respSt);
                    return false;
                }
                else
                {
                    memset(respSt->u.getRes->paramNames, 0, sizeof(char *) * reqSt->u.getReq->paramCnt);
                    memset(respSt->u.getRes->params, 0, sizeof(param_t *) * reqSt->u.getReq->paramCnt);
                    memset(respSt->u.getRes->retParamCnt, 0, sizeof(size_t) * reqSt->u.getReq->paramCnt);
                }
            }
            else
            {
                
                return false;
            }
            break;
            
        case SET:
            if(reqSt->u.setReq->paramCnt == 0)
               return false;

            respSt->paramCnt = reqSt->u.setReq->paramCnt;
            respSt->retStatus = (WDMP_STATUS *)calloc(respSt->paramCnt, sizeof(WDMP_STATUS));
            respSt->u.paramRes = (param_res_t *)malloc(sizeof(param_res_t));
            if(respSt->u.paramRes != NULL)
            {
                memset(respSt->u.paramRes, 0, sizeof(param_res_t));
                
                respSt->u.paramRes->params = (param_t *)malloc(sizeof(param_t) * reqSt->u.setReq->paramCnt);
                if(respSt->u.paramRes->params == NULL)
                {
                    free(respSt->u.paramRes);
                    return false;
                }
                else
                {
                    memset(respSt->u.paramRes->params, 0, sizeof(param_t) * reqSt->u.setReq->paramCnt);
                }
            }
            break;
        default:
            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s : Invalid Request Type\n", __FUNCTION__);
            return false;
    };
    return true;
}

res_struct* handleRequest(const char* pcCallerID, req_struct *reqSt)
{
    unsigned int paramIndex = 0;
    res_struct *respSt = NULL;
    DATA_TYPE dataType = WDMP_NONE;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"Entering... %s\n", __FUNCTION__);
    respSt = (res_struct *) malloc(sizeof(res_struct));
    if(respSt == NULL)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed to allocate respSt - returning\n");
        return NULL;
    }
    memset(respSt, 0, sizeof(res_struct));
    if(!allocate_res_struct_members(respSt, reqSt))
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Failed to allocate respSt members\n");
        wdmp_free_res_struct(respSt);
        respSt = NULL;
        return NULL;
    }

    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"processing each paramName ... with reqType - %d\n", respSt->reqType);

    switch(respSt->reqType)
    {
        case GET:
                for(paramIndex = 0; paramIndex < respSt->paramCnt; paramIndex++)
                {
                    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Request:> paramNames[%d] = %s\n",paramIndex,reqSt->u.getReq->paramNames[paramIndex]);
                    respSt->u.getRes->paramNames[paramIndex] = strdup(reqSt->u.getReq->paramNames[paramIndex]);

                    if(strlen(respSt->u.getRes->paramNames[paramIndex]) > MAX_PARAMETERNAME_LEN)
                    {
                        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"paramName > MAX_PARAMETERNAME_LEN for : %s\n", reqSt->u.getReq->paramNames[paramIndex]);
                        respSt->u.getRes->retParamCnt[paramIndex] = 1;
                        respSt->u.getRes->params[paramIndex] = (param_t *)malloc(sizeof(param_t) * respSt->u.getRes->retParamCnt[paramIndex]);
                        respSt->u.getRes->params[paramIndex][0].name = strdup(respSt->u.getRes->paramNames[paramIndex]);
                        respSt->u.getRes->params[paramIndex][0].value = NULL;
                        respSt->u.getRes->params[paramIndex][0].type = WDMP_NONE;

                        respSt->retStatus[paramIndex] = WDMP_ERR_INVALID_PARAMETER_NAME;
                        continue;
                    }

                    if(isWildCardParam(respSt->u.getRes->paramNames[paramIndex]))
                    {
                        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"%s is a WildCardParam \n", respSt->u.getRes->paramNames[paramIndex]);
                        char **childParamNames = (char**) calloc (MAX_NUM_PARAMETERS, sizeof(char*));
                        if (NULL == childParamNames)
                        {
                            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error allocating memory\n");
                            wdmp_free_res_struct(respSt);
                            respSt = NULL;
                            return respSt;
                        }
                        char **childParamDataTypes = (char**) calloc (MAX_NUM_PARAMETERS, sizeof(char*));
                        if(NULL == childParamDataTypes)
                        {
                            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error allocating memory\n");
                            free(childParamNames);
                            wdmp_free_res_struct(respSt);
                            respSt = NULL;
                            return respSt;
                        }
                        param_t *childParams = NULL;

                        if(DB_SUCCESS == getChildParamNamesFromDataModel(getDataModelHandle(), respSt->u.getRes->paramNames[paramIndex], childParamNames, childParamDataTypes, (int *)&respSt->u.getRes->retParamCnt[paramIndex]))
                        {
                            childParams = (param_t *)malloc(sizeof(param_t) * (int) respSt->u.getRes->retParamCnt[paramIndex]);
                            respSt->retStatus[paramIndex] = WDMP_SUCCESS;

                            RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"No of child params = %d\n", respSt->u.getRes->retParamCnt[paramIndex]);
                            for(int childParamIndex = 0; childParamIndex < (int)respSt->u.getRes->retParamCnt[paramIndex]; childParamIndex++)
                            {
                                childParams[childParamIndex].name = childParamNames[childParamIndex];
                                childParams[childParamIndex].type = getWdmpDataType(childParamDataTypes[childParamIndex]);
                                childParams[childParamIndex].value = NULL;

                                //retStatus represents the first error while trying to retrieve the values of a wild card parameter name.
                                if(WDMP_SUCCESS != invokeHostIfAPI(reqSt->reqType, childParams+childParamIndex, HOSTIF_NONE, pcCallerID) && respSt->retStatus[paramIndex] == WDMP_SUCCESS)
                                {
                                   respSt->retStatus[paramIndex] = WDMP_ERR_VALUE_IS_EMPTY;
                                }
                            }
                            respSt->u.getRes->params[paramIndex] = childParams;
                        }
                        else
                        {
                           free(childParamDataTypes);
                           free(childParamNames);

                           respSt->u.getRes->retParamCnt[paramIndex] = 1;
                           respSt->u.getRes->params[paramIndex] = (param_t *)malloc(sizeof(param_t) * respSt->u.getRes->retParamCnt[paramIndex]);

                           respSt->u.getRes->params[paramIndex][0].name = strdup(respSt->u.getRes->paramNames[paramIndex]);
                           respSt->u.getRes->params[paramIndex][0].value = strdup("");
                           respSt->u.getRes->params[paramIndex][0].type = WDMP_NONE;
                           respSt->retStatus[paramIndex] = WDMP_ERR_INVALID_PARAMETER_NAME;
                        }
                    }
                    else
                    {
                        RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Not a wildcard param - validating against data model...\n");
                        char *defaultValue = NULL;
                        bool rfcParam = false;
                        HostIf_Source_Type_t bsUpdate = HOSTIF_NONE;

                        // Temporary handling for RFC Variables - Which are not validated against data-model.xml
                        if(strncmp(respSt->u.getRes->paramNames[paramIndex], "RFC_", 4) == 0 && strchr(respSt->u.getRes->paramNames[paramIndex], '.') == NULL)
                        {
                           dataType = WDMP_STRING;
                           rfcParam = true;
                           respSt->retStatus[paramIndex] = WDMP_SUCCESS;
                        }
                        else
                        {
                           respSt->retStatus[paramIndex] = validateAgainstDataModel(respSt->reqType, respSt->u.getRes->paramNames[paramIndex], NULL, &dataType, &defaultValue, &bsUpdate);
                        }

                        respSt->u.getRes->retParamCnt[paramIndex] = 1;
                        respSt->u.getRes->params[paramIndex] = (param_t *)malloc(sizeof(param_t) * respSt->u.getRes->retParamCnt[paramIndex]);

                        respSt->u.getRes->params[paramIndex][0].name = strdup(respSt->u.getRes->paramNames[paramIndex]);
                        respSt->u.getRes->params[paramIndex][0].value = NULL;
                        respSt->u.getRes->params[paramIndex][0].type = dataType;

                        if(WDMP_SUCCESS == respSt->retStatus[paramIndex])
                        {
                           if(!rfcParam)
                              respSt->retStatus[paramIndex] = invokeHostIfAPI(reqSt->reqType, respSt->u.getRes->params[paramIndex], bsUpdate, pcCallerID);
                           else // Temporary handling for RFC Variables - handled using RFC API.
                              respSt->retStatus[paramIndex] = handleRFCRequest(reqSt->reqType, respSt->u.getRes->params[paramIndex]);

                           if(respSt->retStatus[paramIndex] != WDMP_SUCCESS)
                           {
                              if(defaultValue)
                              {
                                 respSt->u.getRes->params[paramIndex][0].value = strdup(defaultValue);
                                 respSt->retStatus[paramIndex] = WDMP_ERR_DEFAULT_VALUE;
                                 RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Setting Default Value : %s\n", respSt->u.getRes->params[paramIndex][0].value);
                              }
                              else
                              {
                                 respSt->retStatus[paramIndex] = WDMP_ERR_VALUE_IS_EMPTY;
                              }
                           }
                        }
                        else
                        {
                           RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Failed in data-model validation\n");
                           respSt->u.getRes->params[paramIndex][0].value = strdup("");
                        }
                        if(defaultValue)
                           free(defaultValue);
                    }
                }
                break;

        case SET:
                for(paramIndex = 0; paramIndex < respSt->paramCnt; paramIndex++)
                {
                    bool rfcParam = false;

                    respSt->u.paramRes->params[paramIndex].name = strdup(reqSt->u.setReq->param[paramIndex].name);
                    if(!isAuthorized(pcCallerID, reqSt->u.setReq->param[paramIndex].name))
                    {
                        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"CallerID : %s is not allowed to SET : %s\n", pcCallerID, reqSt->u.setReq->param[paramIndex].name);
                        respSt->retStatus[paramIndex] = WDMP_FAILURE;
                        continue;
                    }
                    else if(isWildCardParam(reqSt->u.setReq->param[paramIndex].name))
                    {
                        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Wildcard SET/SET-ATTRIBUTES is not supported \n");
                        respSt->retStatus[paramIndex] = WDMP_ERR_WILDCARD_NOT_SUPPORTED;
                        continue;
                    }
                    else if(reqSt->u.setReq->param[paramIndex].value == NULL)
                    {
                        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Parameter value is null\n");
                        respSt->retStatus[paramIndex] = WDMP_ERR_VALUE_IS_NULL;
                        continue;
                    }
                    else
                    {
                        respSt->u.paramRes->params[paramIndex].value = strdup(reqSt->u.setReq->param[paramIndex].value);
                    }

                    // Temporary handling for RFC Variables - Which are not validated against data-model.xml
                    if(strncmp(reqSt->u.setReq->param[paramIndex].name, "RFC_", 4) == 0 && strchr(reqSt->u.setReq->param[paramIndex].name, '.') == NULL)
                    {
                        dataType = WDMP_STRING;
                        rfcParam = true;
                        respSt->retStatus[paramIndex] = handleRFCRequest(reqSt->reqType, reqSt->u.setReq->param + paramIndex);
                    }
                    else
                    {
                        HostIf_Source_Type_t bsUpdate = HOSTIF_NONE;
                        respSt->retStatus[paramIndex] = validateAgainstDataModel(reqSt->reqType, reqSt->u.setReq->param[paramIndex].name, reqSt->u.setReq->param[paramIndex].value, &reqSt->u.setReq->param[paramIndex].type, NULL, &bsUpdate);
                        if(WDMP_SUCCESS != respSt->retStatus[paramIndex])
                        {
                            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Operation not permitted : %d\n", respSt->retStatus[paramIndex]);
                            continue;
                        }
                        respSt->retStatus[paramIndex] = invokeHostIfAPI(reqSt->reqType, reqSt->u.setReq->param + paramIndex, bsUpdate, pcCallerID);
                    }
                }
                break;

        default:
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Invalid operation type : %d\n", reqSt->reqType);
                wdmp_free_res_struct(respSt);
                respSt = NULL;
    };

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"Leaving... %s\n", __FUNCTION__);
    return respSt;
}
