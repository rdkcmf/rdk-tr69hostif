/**
 * @file webpa_parameter.h
 *
 * @description This file describes the Webpa Abstraction Layer
 *
 * Copyright (c) 2017  Comcast
 */
#ifdef __cplusplus
extern "C"
{
#endif
#include <wdmp-c.h>
#ifdef __cplusplus
}
#endif
#include "webpa_adapter.h"


#define WEBPA_DATA_MODEL_FILE "/etc/data-model.xml"
#define MAX_NUM_PARAMETERS 2048
#define MAX_DATATYPE_LENGTH 48
#define MAX_PARAM_LENGTH TR69HOSTIFMGR_MAX_PARAM_LEN
#define MAX_PARAMETER_LENGTH 512
#define MAX_PARAMETERVALUE_LEN 128



/**
 * @brief getValues interface returns the parameter values.
 *
 * getValues supports an option to pass wildward parameters. This can be achieved by passing an object name followed by '.'
 * instead of parameter name.
 *
 * @param[in] paramName List of Parameters.
 * @param[in] paramCount Number of parameters.
 * @param[out] timeSpan timing_values for each component.
 * @param[out] paramValArr Two dimentional array of parameter name/value pairs.
 * @param[out] retValCount List of "number of parameters" for each input paramName. Usually retValCount will be 1 except
 * for wildcards request where it represents the number of param/value pairs retrieved for the particular wildcard parameter.
 * @param[out] retStatus List of Return status.
 */
void getValues(const char *paramName[], const unsigned int paramCount, money_trace_spans *timeSpan, ParamVal ***paramValArr, int *retValCount, WAL_STATUS *retStatus);

/**
 * @brief setValues interface sets the parameter value.
 *
 * @param[in] paramVal List of Parameter name/value pairs.
 * @param[in] paramCount Number of parameters.
 * @param[in] setType Flag to specify the type of set operation.
 * @param[out] timeSpan timing_values for each component.
 * @param[out] retStatus List of Return status.
 */
void setValues(const ParamVal paramVal[], const unsigned int paramCount, const WEBPA_SET_TYPE setType, money_trace_spans *timeSpan, WAL_STATUS *retStatus,char * transaction_id);