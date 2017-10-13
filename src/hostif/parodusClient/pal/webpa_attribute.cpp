/**
 * @file webpa_adapter.cpp
 *
 * @description This file describes the Webpa Attribute request handling
 *
 * Copyright (c) 2017  Comcast
 */
#include <string.h>
#include "webpa_attribute.h"
#include "hostIf_msgHandler.h"
#include "rdk_debug.h"
#include "webpa_parameter.h"
#include "webpa_notification.h"

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/

extern char **g_notifyParamList;
extern unsigned int g_notifyListSize;
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
static WAL_STATUS get_AttribValues_tr69hostIf(HOSTIF_MsgData_t *ptrParam);
static WAL_STATUS set_AttribValues_tr69hostIf (HOSTIF_MsgData_t param);
static WAL_STATUS getParamAttributes(const char *pParameterName, AttrVal ***attr, int *TotalParams);
static WAL_STATUS setParamAttributes(const char *pParameterName, const AttrVal *attArr);


/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/**
 *
 *
 * @param[in] paramName List of Parameters.
 * @param[in] paramCount Number of parameters.
 * @param[out] attr Two dimentional array of attribute name/value pairs.
 * @param[out] retAttrCount List of "number of attributes" for each input paramName.
 * @param[out] retStatus List of Return status.
 */
void getAttributes(const char *paramName[], const unsigned int paramCount, money_trace_spans *timeSpan, AttrVal ***attr, int *retAttrCount, WAL_STATUS *retStatus)
{
    int cnt=0;
    for(cnt=0; cnt<paramCount; cnt++)
    {
        retStatus[cnt]=getParamAttributes(paramName[cnt], &attr[cnt], &retAttrCount[cnt]);
        RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Parameter Name: %s, Parameter Attributes return: %d\n",paramName[cnt],retStatus[cnt]);
    }
}

/**
 * @brief setAttributes interface sets the attribute values.
 *
 * @param[in] paramName List of Parameters.
 * @param[in] paramCount Number of parameters.
 * @param[in] attr List of attribute name/value pairs.
 * @param[out] retStatus List of Return status.
 */
void setAttributes(ParamVal params[], const unsigned int paramCount, money_trace_spans *timeSpan, const AttrVal *attr[], WDMP_STATUS **retStatus)
{
    RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"[%s:%s:%d] Inside setAttributes, Param Count = %d\n", __FILE__, __FUNCTION__, __LINE__,paramCount);
    int cnt=0;
    for(cnt=0; cnt<paramCount; cnt++)
    {
        (*retStatus)[cnt] = (WDMP_STATUS) setParamAttributes(const_cast<const char*>(params[cnt].name),attr[cnt]);
    }
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
/**
 * @brief Get Attribute value
 *
 * @param[in] Parameter struct to fill
 */
static WAL_STATUS get_AttribValues_tr69hostIf(HOSTIF_MsgData_t *ptrParam)
{
    int retStatus = -1;
    ptrParam->reqType = HOSTIF_GETATTRIB;
    hostIf_GetAttributesMsgHandler(ptrParam);
    if(retStatus != 0) {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"[%s:%s:%d] Failed in IARM_Bus_Call(), with return value: %d\n", __FILE__, __FUNCTION__, __LINE__, retStatus);
        return WAL_ERR_INVALID_PARAM;
    }
    else
    {
        RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"[%s:%s:%d] The value for param: %s is %s paramLen : %d\n", __FILE__, __FUNCTION__, __LINE__, ptrParam->paramName,ptrParam->paramValue, ptrParam->paramLen);
    }
    return WAL_SUCCESS; // Mock Response
}

/**
 * @brief Get Attribute value
 *
 */
static WAL_STATUS getParamAttributes(const char *pParameterName, AttrVal ***attr, int *TotalParams)
{
    int ret = WAL_SUCCESS;
    int sizeAttrArr = 1; // Currently support only Notification parameter
    int i = 0;
    HOSTIF_MsgData_t Param = {0};

    memset(&Param, '\0', sizeof(HOSTIF_MsgData_t));

    // Check if pParameterName is in the list of notification parameters and check if the parameter is one among them
    int found = 0;
    for(i = 0; i < g_notifyListSize; i++)
    {
        if(!strcmp(pParameterName,g_notifyParamList[i]))
        {
            found = 1;
            break;
        }
    }
    if(!found)
    {
        return WAL_ERR_INVALID_PARAM;
    }

    *TotalParams = sizeAttrArr;
    attr[0] = (AttrVal **) malloc(sizeof(AttrVal *) * sizeAttrArr);
    for(i = 0; i < sizeAttrArr; i++)
    {
        attr[0][i] = (AttrVal *) malloc(sizeof(AttrVal) * 1);
        attr[0][i]->name = (char *) malloc(sizeof(char) * MAX_PARAMETER_LENGTH);
        attr[0][i]->value = (char *) malloc(sizeof(char) * MAX_PARAMETERVALUE_LEN);

        strcpy(attr[0][i]->name,pParameterName); // Currently only one attribute ie., notification, so use the parameter name to get its value
        /* Get Notification value for the parameter from hostif */
        strncpy(Param.paramName,pParameterName,strlen(pParameterName)+1);
        Param.instanceNum = 0;
        Param.paramtype = hostIf_IntegerType;
        ret = get_AttribValues_tr69hostIf(&Param);
        strncpy(attr[0][i]->value,Param.paramValue, strlen(Param.paramValue));
        attr[0][i]->value[strlen(Param.paramValue)] = '\0';
        attr[0][i]->type = WAL_INT; // Currently only notification which is a int
    }
    return WAL_SUCCESS;
}


/**
 * generic Api for set attribute HostIf parameters through IARM_TR69Bus
 **/
static WAL_STATUS set_AttribValues_tr69hostIf (HOSTIF_MsgData_t param)
{
    int retStatus = -1;
    param.reqType = HOSTIF_SETATTRIB;

    // Try to set value
    retStatus = hostIf_SetAttributesMsgHandler(&param);

    if(retStatus != 0)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"[%s:%s:%d] Failed in Set Attribute call, with return value: %d\n", __FILE__, __FUNCTION__, __LINE__, retStatus);
        return WAL_ERR_INVALID_PARAMETER_NAME;
    }
    else
    {
        RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"[%s:%s:%d] Set Successful for value : %s\n", __FILE__, __FUNCTION__, __LINE__, (char *)param.paramValue);
    }
    return WAL_SUCCESS;
}
/**
 * @brief Set Attribute value
 *
 */
static WAL_STATUS setParamAttributes(const char *pParameterName, const AttrVal *attArr)
{
    WAL_STATUS ret = WAL_SUCCESS;
    int i = 0;
    HOSTIF_MsgData_t Param = {0};
    memset(&Param, '\0', sizeof(HOSTIF_MsgData_t));
    // Enable only for notification parameters in the config file
    int found = 0;
    for(i = 0; i < g_notifyListSize; i++)
    {
        if(!strcmp(pParameterName,g_notifyParamList[i]))
        {
            found = 1;
            RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"[%s:%s:%d] Inside setParamAttributes, Param Found in Glist \n", __FILE__, __FUNCTION__, __LINE__,pParameterName);
            break;
        }
    }
    if(!found)
    {
        return WAL_SUCCESS; //Fake success for all setattributes now
    }

    strcpy(Param.paramName, pParameterName);
    strcpy(Param.paramValue, attArr->value);
    Param.paramtype = hostIf_IntegerType;
    ret = set_AttribValues_tr69hostIf (Param);
    return ret;
}
