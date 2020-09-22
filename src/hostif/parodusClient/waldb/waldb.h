/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2017 RDK Management
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
#ifndef _WALDB_H
#define _WALDB_H
#ifdef __cplusplus
extern "C" {
#endif
#include<glib.h>
/**
 * @brief Database Error codes.
 */
typedef enum
{
    DB_SUCCESS = 0,                    /**< Success. */
    DB_FAILURE,                        /**< General Failure */
    DB_ERR_WILDCARD_NOT_SUPPORTED,
    DB_ERR_INVALID_PARAMETER,
    DB_ERR_TIMEOUT,
    DB_ERR_NOT_EXIST
}
DB_STATUS;

typedef struct __DataModelParam
{
    char *objectName;
    char *paramName;
    char *dataType;
    char *access;
    char *defaultValue;
    char *bsUpdate;
} DataModelParam;


/* @brief Loads the Parameter count query string to memory
 *
 * @return void
 */
void initNumEntityParamList();


DB_STATUS loadDataModel(void);

int isWildCardParam(char *paramName);

void* getDataModelHandle();

DB_STATUS checkDataModelStatus();

void freeDataModelParam(DataModelParam dmParam);
/* @brief Returns a parameter list and count given an input paramName with wildcard characters
 *
 * @dbhandle[in] database handle to query in to
 * @paramName[in] parameter name with wildcard(*)
 * @ParamList[out] parameter list extended by the input parameter
 * @ParamDataTypeList[out] parameter data type list extended by the input wildcard parameter
 * @paramCount[out] parameter count
 * @return DB_STATUS
 */
DB_STATUS getChildParamNamesFromDataModel(void *dbhandle,char *paramName,char **ParamList,char **ParamDataTypeList,int *paramCount);

/* @brief Returns a parameter list and count given an input paramName with wildcard characters
 *
 * @dbhandle[in] database handle
 * @paramName[in] parameter name to search for in xml and retrieeve node details
 * @dmParam[out] XML node details of paramName if found
 * @return int returned 0 or 1 based on availability in XML
 */
int getParamInfoFromDataModel(void *handle,const char *paramName,DataModelParam *dmParam);

DB_STATUS get_complete_param_list (char **out_param_list, int *out_param_count);
#ifdef __cplusplus
}
#endif
#endif /*_WALDB_H*/

