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
 * @file webpa_attribute.h
 *
 * @description This file describes the Webpa Abstraction Layer
 *
 */
#include "webpa_adapter.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include <wdmp-c.h>
#ifdef __cplusplus
}
#endif

/**
 *
 *
 * @param[in] paramName List of Parameters.
 * @param[in] paramCount Number of parameters.
 * @param[out] attr Two dimentional array of attribute name/value pairs.
 * @param[out] retAttrCount List of "number of attributes" for each input paramName.
 * @param[out] retStatus List of Return status.
 */
void getAttributes(const char *paramName[], const unsigned int paramCount, money_trace_spans *timeSpan, AttrVal ***attr, int *retAttrCount, WAL_STATUS *retStatus);

/**
 * @brief setAttributes interface sets the attribute values.
 *
 * @param[in] paramName List of Parameters.
 * @param[in] paramCount Number of parameters.
 * @param[in] attr List of attribute name/value pairs.
 * @param[out] retStatus List of Return status.
 */
void setAttributes(ParamVal params[], const unsigned int paramCount, money_trace_spans *timeSpan, const AttrVal *attr[], WDMP_STATUS **retStatus);
