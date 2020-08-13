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
 * @file x_rdk_webconfig_dml.c
 *
 * @brief Device.X_RDK_WebConfig API Implementation.
 *
 * This is the implementation of the Device.X_RDK_WebConfig API.
 *
 * @par Document
 * TBD Relevant design or API documentation.
 *
 */


/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/

#include "x_rdk_webconfig_dml.h"
#include <sys/file.h>

#define MAX_CONFIG_COUNT 100
#define MAX_CONFIG_FILE_NAME_LEN 256
#define MAX_CONFIG_GEN_STRING_LEN 64

#define PERSIST_FILE  "/opt/persistent/webconfig.json"
#define WEBCFG_URL_FILE  "/opt/webcfg_url"

std::mutex X_RDK_WebConfig_dml::mlg;
X_RDK_WebConfig_dml* X_RDK_WebConfig_dml::m_instance = NULL;

extern "C" {
    char *get_DB_BLOB_base64(void);
    char *readWebcfgURL(void);
    int setForceSync(char* , char *,int *);
}

char *readWebcfgURL()
{
    FILE *fp = NULL;
    char *val = NULL;
    long int bytes = 0;

    fp = fopen(WEBCFG_URL_FILE,"r");
    if(fp) {
        fseek(fp, 0, SEEK_END);
        bytes = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        if(bytes) {
            val = (char *)malloc(sizeof(char) *(bytes + 1));
            memset(val,0, sizeof(char) *(bytes + 1));
            fread(val,bytes,1,fp);
        }
        fclose(fp);
    }
    return val;
}

int writeWebcfgURL(const char *url)
{
    int ret = -1;
    FILE *fp = NULL;

    fp = fopen(WEBCFG_URL_FILE,"w");

    if(fp) {
        ret = fprintf(fp,"%s",url);
        fclose(fp);
    }
    return ret;
}

X_RDK_WebConfig_dml* X_RDK_WebConfig_dml::getInstance()
{
    if(!m_instance)
    {
        try {
            m_instance = new X_RDK_WebConfig_dml();
        } catch(const std::exception& e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s:%d] X_RDK_WebConfig_dml Caught exception \" %s\"\n", __FUNCTION__, __LINE__, e.what());
        }
    }
    return m_instance;
}

void X_RDK_WebConfig_dml::closeInstance()
{
    if(m_instance)
    {
        delete m_instance;
    }
}

/*
 * Get APIs for WebConfig Parameters
 */

int X_RDK_WebConfig_dml::handleGetReq(HOSTIF_MsgData_t * stMsgData) {

    int ret = NOT_HANDLED;
    const char *path = NULL, *paramName = NULL, *tblAttName = NULL;
    int index = 0;
    std::lock_guard<std::mutex> lg(mlg);

    try {
        int str_len = strlen(X_RDK_WEBCONFIG_OBJ);
        path = stMsgData->paramName;

        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Entering...  \n", __FUNCTION__, __LINE__);

        if(NULL == path) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Failed : Parameter is NULL, %s  \n",  __FUNCTION__, __LINE__, path);
            return ret;
        }

        if((strncasecmp(path, X_RDK_WEBCONFIG_OBJ, str_len) != 0)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Failed due to Mismatch parameter path : %s  \n", __FUNCTION__, __LINE__, path);
            return ret;
        }
        const char *tmp_ptr = strchr(path+str_len-1,'.');
        if(tmp_ptr == NULL)  {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Parameter is NULL  \n", __FUNCTION__, __LINE__);
            return ret;
        }

        tmp_ptr++;
        paramName = tmp_ptr;
        if (strncasecmp(paramName, X_RDK_WEBCONFIG_URL_STRING, strlen(X_RDK_WEBCONFIG_URL_STRING)) == 0)
        {
            ret = getWebcfgURL(stMsgData);
        }
        else if (strncasecmp(paramName, X_RDK_WEBCONFIG_DATA_STRING, strlen(X_RDK_WEBCONFIG_DATA_STRING)) == 0)
        {
            ret = getWebcfgData(stMsgData);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Parameter \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, paramName);
            stMsgData->faultCode = fcInvalidParameterName;
            ret = NOK;
        }
    }
    catch (const std::exception& e )
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Exception caught %s   \n", __FUNCTION__, __LINE__, e.what());
        stMsgData->faultCode = fcInternalError;
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Exiting..\n", __FUNCTION__, __LINE__);
    return ret;
}

/*
 * Set APIs for WebConfig Parameters
 */
int X_RDK_WebConfig_dml::handleSetReq(HOSTIF_MsgData_t * stMsgData) {

    int ret = NOT_HANDLED;
    const char *path = NULL, *paramName = NULL, *tblAttName = NULL;
    int index = 0;
    std::lock_guard<std::mutex> lg(mlg);

    try {
        int str_len = strlen(X_RDK_WEBCONFIG_OBJ);
        path = stMsgData->paramName;

        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Entering...  \n", __FUNCTION__, __LINE__);

        if(NULL == path) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Failed : Parameter is NULL, %s  \n",
                                                  __FUNCTION__, __LINE__, path);
            return ret;
        }

        if((strncasecmp(path, X_RDK_WEBCONFIG_OBJ, str_len) != 0)) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Failed due to Mismatch parameter path : %s  \n", __FUNCTION__, __LINE__, path);
            return ret;
        }
        const char *tmp_ptr = strchr(path+str_len-1,'.');
        if(tmp_ptr == NULL)  {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Parameter is NULL  \n", __FUNCTION__, __LINE__);
            return ret;
        }

        tmp_ptr++;
        paramName = tmp_ptr;
        if (strncasecmp(paramName, X_RDK_WEBCONFIG_FORCE_SYNC_BOOLEAN, strlen(X_RDK_WEBCONFIG_FORCE_SYNC_BOOLEAN)) == 0)
        {
            ret = setWebcfgForceSync(stMsgData);
        }
        else if (strncasecmp(paramName, X_RDK_WEBCONFIG_URL_STRING, strlen(X_RDK_WEBCONFIG_URL_STRING)) == 0)
        {
            ret = setWebcfgURL(stMsgData);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Parameter \'%s\' is Not Supported  \n",
                                                  __FUNCTION__, __LINE__, paramName);
            stMsgData->faultCode = fcInvalidParameterName;
            ret = NOK;
        }
    }
    catch (const std::exception& e )
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Exception caught %s   \n", __FUNCTION__, __LINE__, e.what());
        stMsgData->faultCode = fcInternalError;
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Exiting..\n", __FUNCTION__, __LINE__);
    return ret;
}

/****************************************************************************************************************************************************/
// Device.X_RDK_WebConfig. Profile. Getters:
/****************************************************************************************************************************************************/

/**
 * @brief This function provides the WebConfig RFC status
 *
 * @param[out] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if successfully fetched the data from the device.
 * @retval NOK if failed to fetch the data from the device.
 * @retval NOT_HANDLED if not handled by current version of code.
 */

int X_RDK_WebConfig_dml::getWebcfgData(HOSTIF_MsgData_t * stMsgData)
{
    char *data = NULL;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Entering..\n", __FUNCTION__, __LINE__);
    stMsgData->paramtype=hostIf_StringType;
    data = get_DB_BLOB_base64();
    if(data) {
        RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"[%s]xWebConfig: Data %s\r\n",__FUNCTION__, data);
        strncpy(stMsgData->paramValue,data, TR69HOSTIFMGR_MAX_PARAM_LEN);
        free(data);
    } else {
        RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"[%s]xWebConfig: Data is NULL\r\n",__FUNCTION__);
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}

int X_RDK_WebConfig_dml::getWebcfgURL(HOSTIF_MsgData_t * stMsgData)
{
    char *url = NULL;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Entering..\n", __FUNCTION__, __LINE__);
    stMsgData->paramtype=hostIf_StringType;
    url = readWebcfgURL();
    if(url) {
        RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"[%s]xWebConfig: URL %s\r\n",__FUNCTION__,url);
        strncpy(stMsgData->paramValue, url, TR69HOSTIFMGR_MAX_PARAM_LEN);
        free(url);
    } else {
        RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"[%s]xWebConfig: URL is NULL\r\n",__FUNCTION__);
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}

/****************************************************************************************************************************************************/
// Device.X_RDK_WebConfig. Profile. Setters:
/****************************************************************************************************************************************************/

/**
 * @brief This function sets the WebConfig RFC status
 *
 * @param[in] stMsgData TR-069 Host interface message request.
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if successfully set the data to the device.
 * @retval NOK if failed to set the data to the device.
 * @retval NOT_HANDLED if not handled by current version of code.
 */

int X_RDK_WebConfig_dml::setWebcfgURL(HOSTIF_MsgData_t * stMsgData)
{
    std::string keyValue;
    int ret = -1;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Entering..\n", __FUNCTION__, __LINE__);

    keyValue = stMsgData->paramValue;
    ret = writeWebcfgURL(keyValue.c_str());
    if(ret != strlen(keyValue.c_str())) {
        RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"[%s]xWebConfig: Unable to write URL \r\n",__FUNCTION__);
        return NOK;
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}

int X_RDK_WebConfig_dml::setWebcfgForceSync(HOSTIF_MsgData_t * stMsgData)
{
    std::string keyValue;
    int ret = 0;
    char *val;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Entering..\n", __FUNCTION__, __LINE__);

    keyValue = stMsgData->paramValue;
    val = strdup(keyValue.c_str());
    if (stMsgData->requestor == HOSTIF_SRC_WEBPA)
        ret = setForceSync(val, stMsgData->transactionID, 0);
    else
        ret = setForceSync(val, "", 0);

    if(val)
        free(val);
    if(1 != ret) {
        RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"[%s]xWebConfig: Set force sync failed \r\n",__FUNCTION__);
        return NOK;
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]xWebConfig: Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}

/** @} */
/** @} */
