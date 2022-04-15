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
 * @file webconfig_internal.c
 *
 * @description This file describes the webconfig Abstraction Layer
 *
 */
#include <iostream>
#include <cstring>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include "webconfig_lite.h"
#include <curl/curl.h>
#include "cJSON.h"
#include <uuid/uuid.h>
#include <webconfig_utils.h>
#include <libpd.h>
#include <unistd.h>
#include "rfcapi.h"
#include "safec_lib.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include <wdmp_internal.h>
#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* Macros */
#define CURL_TIMEOUT_SEC	   25L
#define CA_CERT_PATH 		   "/etc/ssl/certs"
#define MAX_BUF_SIZE	           256
#define MAX_HEADER_LEN			4096
#define MAX_PARAMETERNAME_LENGTH       512
#define WEBPA_READ_HEADER             "/etc/parodus/parodus_read_file.sh"
#define WEBPA_CREATE_HEADER           "/etc/parodus/parodus_create_file.sh"
#define BACKOFF_SLEEP_DELAY_SEC 	    20
#define BACKOFF_SLEEP_DELAY_5_SEC 	    5
#define ETAG_HEADER 		    "ETag:"
#define BLE_DETECTION_WEBCONFIG_ENDPOINT "https://cpe-config.xdp.comcast.net/api/v1/device/{mac}/config/ble"
#define BLE_DETECTION_WEBCONFIG_SUFIX "/ble"
#define LOG_TR69HOSTIF  "LOG.RDK.TR69HOSTIF"
#define CURL_FILE "/tmp/adzvfchig-res.mch"
#define CONFIG_VERSION_FILE "/opt/persistent/webconfig_Version"
#define MAX_UUID_SIZE 64

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
struct token_data {
    size_t size;
    char* data;
};

typedef struct _notify_params
{
    char * url;
    long status_code;
    char * application_status;
    int application_details;
    char * request_timestamp;
    char * version;
    char * transaction_uuid;
} notify_params_t;

typedef struct _tr181Data
{
    //size_t paramCnt;
    std::string name;
    std::string value;
    int datatype;
} tr181Data;
/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
static char serialNum[64]= {'\0'};
char webpa_auth_token[4096]= {'\0'};
static char g_ETAG[64]= {'\0'};
static char deviceMac[64] = {'\0'};
static char *webconfigEndPoint=NULL;
tr181Data tr181data;
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
static int processJsonDocument(char *jsonData, int *retStatus, char **docVersion);
static int validateConfigFormat(cJSON *json, char **eTag);
static int requestWebConfigData(char **configData,long *code,char **transaction_id);
static void createCurlheader(struct curl_slist *list, struct curl_slist **header_list, char ** trans_uuid);
static int parseJsonData(char* jsonData, char **version);
static size_t write_callback_fn(void *buffer, size_t size, size_t nmemb, struct token_data *data);
static void getAuthToken();
static void createNewAuthToken(char *newToken, size_t len, char *hw_mac, char* hw_serial_number);
static int handleHttpResponse(long response_code, char *webConfigData, int retry_count,char* transaction_uuid);
static char* generate_trans_uuid();
static void macToLowerCase(char macValue[]);
static void processWebConfigNotification(notify_params_t *n_parm);
static void addWebConfigNotifyMsg(char *url, long status_code, char *application_status, int application_details, char *request_timestamp, char *version, char *transaction_uuid);
static void free_notify_params_struct(notify_params_t *param);
static char *replaceMacWord(const char *s, const char *macW, const char *deviceMACW);
static void processWebconfigSync();
static size_t header_callback(char *buffer, size_t size, size_t nitems);
static void stripSpaces(char *str, char **final_str);

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

void parse_set_request(cJSON *request)
{
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Inside parse set Request \n");
    cJSON *reqParamObj = NULL,*paramArray = NULL;
    size_t paramCount, i;

    paramArray = cJSON_GetObjectItem(request, "parameters");

    paramCount = cJSON_GetArraySize(paramArray);


    for (i = 0; i < paramCount; i++)
    {
        reqParamObj = cJSON_GetArrayItem(paramArray, i);

        if(cJSON_GetObjectItem(reqParamObj, "name") != NULL)
        {
            tr181data.name = cJSON_GetObjectItem(reqParamObj, "name")->valuestring;
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:The Name = %s\n",tr181data.name.c_str());
        }
        else
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:The Name value is NULL \n");
        }

        if (cJSON_GetObjectItem(reqParamObj, "value") != NULL )
        {
            if(cJSON_GetObjectItem(reqParamObj, "value")->valuestring != NULL)
            {
                tr181data.value = cJSON_GetObjectItem(reqParamObj, "value")->valuestring;
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:The param value =%s \n",tr181data.value.c_str());
            }
            else
            {
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Parameter value field is not a string \n");
            }
        }

        if (cJSON_GetObjectItem(reqParamObj, "dataType") != NULL)
        {
            tr181data.datatype = cJSON_GetObjectItem(reqParamObj, "dataType")->valueint;
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:The data Type = %d \n", tr181data.datatype);
        }
    }

}


void * initWebConfigTask(void *)
{
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite: [%s] %d **** Entering webconfig task thread ****** \n", __FUNCTION__, __LINE__);

    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:**** calling processWebconfigSync  ****** \n");

    processWebconfigSync();

    if(webconfigEndPoint != NULL)
    {
        WAL_FREE(webconfigEndPoint);
    }
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:**** End of webconfig task thread ****** \n");
    pthread_detach(pthread_self());
}

static void processWebconfigSync()
{
    int retry_count=0;
    int configRet = -1;
    char *webConfigData = NULL;
    long res_code;
    int rv=0,ret=ERR;
    char *transaction_uuid =NULL;

    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:========= Start of processWebconfigSync ============= \n");
    while(1)
    {
        if(retry_count > 25)
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:retry_count has reached max limit. Exiting.\n");
            retry_count=0;
            break;
        }
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Checking for MAC address in Device file Cache \n");
        ret = getKeyValue("estb_mac", deviceMac);
        if(ret==SUCCESS)
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:***** calling requestWebConfigData ***** \n");
            configRet = requestWebConfigData(&webConfigData, &res_code, &transaction_uuid);
            if(configRet == SUCCESS)
            {
                rv = handleHttpResponse(res_code, webConfigData, retry_count, transaction_uuid);
            }
            else
            {
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Failed to get webConfigData from cloud\n");
            }
            if(webConfigData != NULL)
            {
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:=========Delete webconfig data =============\n");
                WAL_FREE(webConfigData);
            }
            if(transaction_uuid != NULL)
            {
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:=========transaction_uuid =============\n");
                WAL_FREE(transaction_uuid);
            }
            if(rv == SUCCESS)
            {
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:No retries are required. Exiting..\n");
                break;
            }

            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:requestWebConfigData BACKOFF_SLEEP_DELAY_SEC is %d seconds\n", BACKOFF_SLEEP_DELAY_SEC);
        }
        else
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite: MAC address not populated yet in Device file Cache BACKOFF_SLEEP_DELAY_SEC is %d seconds\n", BACKOFF_SLEEP_DELAY_SEC);
        }
        sleep(BACKOFF_SLEEP_DELAY_SEC);
        retry_count++;
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Webconfig retry_count is %d\n", retry_count);
    }

    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:========= End of processWebconfigSync =============\n");
    return;
}

static int handleHttpResponse(long response_code, char *webConfigData, int retry_count, char* transaction_uuid)
{
    int first_digit=0;
    int json_status=0;
    int setRet = 0;
    char configVersion[MAX_BUF_SIZE] = {'\0'};
    char *newDocVersion = NULL;
    time_t current_time;
    char currentTime[32];
    current_time = time(NULL);
    snprintf(currentTime,sizeof(currentTime),"%d",(int)current_time);
    //read the configversion from /opt/data

    FILE *ConfigFilePtr = NULL;
    /*write the configVersion/docVersion to the file "/opt/persistent/webconfig_Version"*/
    if(ConfigFilePtr = fopen(CONFIG_VERSION_FILE,"r"))
    {

        fread(configVersion, MAX_BUF_SIZE, 1, ConfigFilePtr);
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"The config version file exists with value =%s \n",configVersion);
        fclose(ConfigFilePtr);
    }

    if(response_code == 304)
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:webConfig is in sync with cloud. response_code:%d\n", response_code);
        addWebConfigNotifyMsg(webconfigEndPoint, response_code, NULL, 0, currentTime , configVersion, transaction_uuid);
        return SUCCESS;
    }
    else if(response_code == 200)
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:webConfig is not in sync with cloud. response_code:%d\n", response_code);

        if(webConfigData !=NULL)
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:webConfigData fetched successfully\n");
            json_status = processJsonDocument(webConfigData, &setRet, &newDocVersion);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:setRet after process Json is %d\n", setRet);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:newDocVersion is %s\n", newDocVersion);
            if(json_status == SUCCESS)
            {
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:processJsonDocument success\n");
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:The transcation UUID =%s \n",transaction_uuid);
                addWebConfigNotifyMsg(webconfigEndPoint, response_code, "success", setRet, currentTime , newDocVersion, transaction_uuid);
            }
            else
            {
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Failure in processJsonDocument\n");
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Configuration settings  version %s FAILED\n", newDocVersion );
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Sending Webconfig apply Failure Notification\n");
                addWebConfigNotifyMsg(webconfigEndPoint, response_code, "failed", setRet, currentTime , newDocVersion, transaction_uuid);
            }
            if(newDocVersion != NULL)
            {
                WAL_FREE(newDocVersion);
            }
            return SUCCESS;
        }
        else
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:webConfigData is empty, need to retry\n");
        }
    }
    else if(response_code == 204)
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:No configuration available for this device. response_code:%d\n", response_code);
        addWebConfigNotifyMsg(webconfigEndPoint, response_code, NULL, 0, currentTime , configVersion, transaction_uuid);
        return SUCCESS;
    }
    else if(response_code == 403)
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Token is expired, fetch new token. response_code:%d\n", response_code);
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:The device mac =%s \n",deviceMac);
        createNewAuthToken(webpa_auth_token, sizeof(webpa_auth_token), deviceMac, serialNum );
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:createNewAuthToken done in 403 case\n");
        //retun error based on retry count at the end of the function
    }
    else if(response_code == 404)
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Action not supported. response_code:%d\n", response_code);
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite: Something went wrong on the device side re try again \n");
        //addWebConfigNotifyMsg(webconfigEndPoint, response_code, NULL, 0, currentTime , configVersion, transaction_uuid);
        return ERR;

    }
    else if(response_code == 429)
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:No action required from client. response_code:%d\n", response_code);
        return SUCCESS;
    }
    first_digit = (int)(response_code / pow(10, (int)log10(response_code)));
    if((response_code !=403) && (first_digit == 4)) //4xx
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Action not supported. response_code:%d\n", response_code);
        addWebConfigNotifyMsg(webconfigEndPoint, response_code, NULL, 0, currentTime , configVersion, transaction_uuid);
        return SUCCESS;
    }
    else //5xx & all other errors
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Error code returned, need to retry. response_code:%d\n", response_code);
        if(retry_count >= 25 )
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Sending Notification after %d retry attempts\n",retry_count);
            addWebConfigNotifyMsg(webconfigEndPoint, response_code, NULL, 0, currentTime , configVersion, transaction_uuid);
            return ERR;
        }
    }
    return ERR;
}

/*
 * @brief Initialize curl object with required options. create configData using libcurl.
 * @param[out] configData
 * @param[in] len total configData size
 * @param[in] r_count Number of curl retries on ipv4 and ipv6 mode during failure
 * @return returns 0 if success, otherwise failed to fetch auth token and will be retried.
 */
static int requestWebConfigData(char **configData, long *code, char **transaction_id)
{
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite: **** inside requestWebConfigData***** \n");
    CURL *curl;
    CURLcode res;
    CURLcode time_res;
    struct curl_slist *list = NULL;
    struct curl_slist *headers_list = NULL;
    double total;
    long response_code = 0;
    char *ct = NULL;
    char *transID = NULL;
    int content_res=0;
    struct token_data data;
    data.size = 0;
    char c[] = "{mac}";
    errno_t rc = -1;
    void * dataVal = NULL;
    curl = curl_easy_init();
    if(curl)
    {
        //this memory will be dynamically grown by write call back fn as required
        data.data = (char *) malloc(sizeof(char) * 1);
        if(NULL == data.data)
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Failed to allocate memory.\n");
            return ERR;
        }
        data.data[0] = '\0';
        createCurlheader(list, &headers_list, &transID);
        if(transID !=NULL)
        {
            *transaction_id = strdup(transID);
            WAL_FREE(transID);
        }

        char endPoint[128] = {'\0'};
        //Replace {mac} string from default init url with actual deviceMAC
        RFC_ParamData_t param = {0};
        WDMP_STATUS status = getRFCParameter((char*)"webcfg", "Device.X_RDK_WebConfig.URL", &param);

        if (status == WDMP_SUCCESS) {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webcfg: name = %s, type = %d, value = %s\n", param.name, param.type, param.value);
            rc=strcpy_s(endPoint,sizeof(param.value) ,param.value);
            if(rc==EOK)
            {
                rc=strcat_s(endPoint,strlen(BLE_DETECTION_WEBCFG_ENDPOINT),BLE_DETECTION_WEBCFG_ENDPOINT);
            }
            if(rc==EOK)
            {
                webconfigEndPoint = replaceMacWord(endPoint, c, deviceMac);
            }
        }

        if(status != WDMP_SUCCESS || rc!=EOK)
        {
            webconfigEndPoint = replaceMacWord(BLE_DETECTION_WEBCONFIG_ENDPOINT, c, deviceMac);
        }


        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:webConfigURL is %s \n", webconfigEndPoint);
        curl_easy_setopt(curl, CURLOPT_URL, webconfigEndPoint );
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, CURL_TIMEOUT_SEC);

        // set callback for writing received data
        dataVal = &data;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_fn);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, dataVal);

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers_list);

        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Set CURLOPT_HEADERFUNCTION option\n");
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);

        // setting curl resolve option as default mode.
        //If any failure, retry with v4 first and then v6 mode.
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:curl Ip resolve option set as default mode\n");
        curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_WHATEVER);
        curl_easy_setopt(curl, CURLOPT_CAPATH, CA_CERT_PATH);
        // disconnect if it is failed to validate server's cert
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        // Verify the certificate's name against host
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
        // To use TLS version 1.2 or later
        curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
        // To follow HTTP 3xx redirections
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:webConfig curl response %d http_code %d\n", res, response_code);
        *code = response_code;
        time_res = curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total);
        if(time_res == 0)
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:curl response Time: %.1f seconds\n", total);
        }
        curl_slist_free_all(headers_list);
        if(res != 0)
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:checking content type\n");
            content_res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
            if(!content_res && ct)
            {
                if(strcmp(ct, "application/json") !=0)
                {
                    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Invalid Content-Type\n");
                }
                else if(response_code == 200)
                {
                    *configData = strdup(data.data);
                    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:configData received from cloud is %s\n", *configData);
                }
            }
        }
        WAL_FREE(data.data);
        curl_easy_cleanup(curl);
        return SUCCESS;
    }
    else
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:curl init failure\n");
    }
    return ERR;
}

/* @brief callback function for writing libcurl received data
 * @param[in] buffer curl delivered data which need to be saved.
 * @param[in] size size is always 1
 * @param[in] nmemb size of delivered data
 * @param[out] data curl response data saved.
 */
static size_t write_callback_fn(void *buffer, size_t size, size_t nmemb, struct token_data *data)
{
    size_t index = data->size;
    size_t n = (size * nmemb);
    char* tmp;
    errno_t safec_rc = -1;

    data->size += (size * nmemb);

    tmp = (char *)realloc(data->data, data->size + 1); // +1 for '\0'

    if(tmp) {
        data->data = tmp;
    } else {
        if(data->data) {
            WAL_FREE(data->data);
        }
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Failed to allocate memory for data\n");
        return 0;
    }

    safec_rc=memcpy_s((data->data + index),sizeof(data->data)-index, buffer, n);
    if(safec_rc!=EOK)
	{
		ERR_CHK(safec_rc);
	}
    data->data[data->size] = '\0';

    return size * nmemb;
}

/* @brief callback function to extract response header data.
*/
static size_t header_callback(char *buffer, size_t size, size_t nitems)
{
    size_t etag_len = 0;
    char* header_value = NULL;
    char* final_header = NULL;
    char header_str[64] = {'\0'};
    //int i=0, j=0;

    etag_len = strlen(ETAG_HEADER);
    if( nitems > etag_len )
    {
        if( strncasecmp(ETAG_HEADER, buffer, etag_len) == 0 )
        {
            header_value = strtok(buffer, ":");
            while( header_value != NULL )
            {
                header_value = strtok(NULL, ":");
                if(header_value !=NULL)
                {
                    strncpy(header_str, header_value, sizeof(header_str)-1);
                    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:header_str is %s\n", header_str);
                    stripSpaces(header_str, &final_header);

                    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:final_header is %s len %lu\n", final_header, strlen(final_header));
                    strncpy(g_ETAG, final_header, sizeof(g_ETAG)-1);
                }
            }
        }
    }
    return nitems;
}

//To strip all spaces , new line & carriage return characters from header output
static void stripSpaces(char *str, char **final_str)
{
    int i=0, j=0;

    for(i=0; str[i]!='\0'; ++i)
    {
        if(str[i]!=' ')
        {
            if(str[i]!='\n')
            {
                if(str[i]!='\r')
                {
                    str[j++]=str[i];
                }
            }
        }
    }
    str[j]='\0';
    *final_str = str;
}

static int processJsonDocument(char *jsonData, int *retStatus, char **docVersion)
{
    int parseStatus = 0;
    char *version = NULL;
    std:: string command;
    int exitstatus;

    parseStatus = parseJsonData(jsonData, &version);
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:After parseJsonData version is %s\n", version);
    if(version!=NULL)
    {
        *docVersion = strdup(version);
        FILE *ConfigFilePtr = NULL;
        /*write the configVersion/docVersion to the file "/opt/persistent/webconfig_Version"*/
        if(ConfigFilePtr = fopen(CONFIG_VERSION_FILE,"w"))
        {
            fwrite(version, strlen(version), 1, ConfigFilePtr);
            fclose(ConfigFilePtr);
        }
        else
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite: FAILED opening </opt/persistent/webconfig_Version> configVersion file \n");
        }
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:docVersion is %s\n", *docVersion);
        WAL_FREE(version);
    }
    if(parseStatus ==1)
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite: ****webconfig set Request **** \n");
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:The param name =%s \n",tr181data.name.c_str());
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:The param value =%s \n",tr181data.value.c_str());
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:The param dataType =%d \n",tr181data.datatype);
        DATA_TYPE data_type = (DATA_TYPE)tr181data.datatype;

        WDMP_STATUS rfcStatus = setRFCParameter((char*)"webconfig",tr181data.name.c_str() , tr181data.value.c_str(), data_type);

        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:The setRFCParameter return = %s \n",rfcStatus);

        return (WDMP_SUCCESS == rfcStatus)? SUCCESS : ERR;
    }
    else
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:parseJsonData failed. parseStatus is %d\n", parseStatus);
        return ERR;
    }
}

static int parseJsonData(char* jsonData, char **version)
{
    cJSON *json = NULL;
    int isValid =0;
    int rv =-1;
    char *configVersion= NULL;

    if((jsonData !=NULL) && (strlen(jsonData)>0))
    {
        json = cJSON_Parse(jsonData);

        if( json != NULL )
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:WebConfig Parse successi \n");
            isValid = validateConfigFormat(json, &configVersion);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:configVersion is %s\n", configVersion);
            if(configVersion !=NULL)
            {
                *version = strdup(configVersion);
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:version copied from configVersion is %s\n", *version);
                WAL_FREE(configVersion);
            }
            if(!isValid)
            {
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:validateConfigFormat failed\n");
                return rv;
            }
            parse_set_request(json);
            cJSON_Delete(json);
            rv = 1;
        }
        else
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:****Error parsing Json data **** \n");
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:jsonData is empty\n");
    }
    return rv;
}

static int validateConfigFormat(cJSON *json, char **eTag)
{
    cJSON *versionObj =NULL;
    cJSON *paramArray = NULL;
    int itemSize=0;
    char *jsonversion=NULL;

    versionObj = cJSON_GetObjectItem( json, "version" );
    if(versionObj !=NULL)
    {
        if(cJSON_GetObjectItem( json, "version" )->type == cJSON_String)
        {
            jsonversion = cJSON_GetObjectItem( json, "version" )->valuestring;
            if(jsonversion !=NULL)
            {
                //version & eTag header validation
                if(strlen(g_ETAG)>0)
                {
                    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:jsonversion :%s len %lu\n", jsonversion, strlen(jsonversion));
                    if(strncmp(jsonversion, g_ETAG, strlen(g_ETAG)) == 0)
                    {
                        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Config Version and ETAG header are matching\n");
                        *eTag = strdup(jsonversion);
                        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:eTag is %s\n", *eTag);
                        //check parameters
                        paramArray = cJSON_GetObjectItem( json, "parameters" );
                        if( paramArray != NULL )
                        {
                            itemSize = cJSON_GetArraySize( json );
                            if(itemSize ==2)
                            {
                                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Config document format is valid\n");
                                return 1;
                            }
                            else
                            {
                                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:config contains fields other than version and parameters\n");
                                return 0;
                            }
                        }
                        else
                        {
                            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Invalid config json, parameters field is not present\n");
                            return 0;
                        }
                    }
                    else
                    {
                        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Invalid config json, version & ETAG are not same\n");
                        return 0;
                    }
                }
                else
                {
                    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Failed to fetch ETAG header from config response\n");
                    return 0;
                }
            }
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Invalid config json, version field is not present\n");
        return 0;
    }

    return 0;
}


/* @brief Function to create curl header options
 * @param[in] list temp curl header list
 * @param[in] device status value
 * @param[out] header_list output curl header list
 */
static void createCurlheader( struct curl_slist *list, struct curl_slist **header_list, char ** trans_uuid)
{
    time_t current_time;
    char currentTime[32];
    char *transaction_uuid = NULL;
    char current_version[MAX_BUF_SIZE] = {0};
    FILE *ConfigFilePtr = NULL;
    char auth_header[MAX_HEADER_LEN] = {0};
    char version_header[MAX_BUF_SIZE] = {0};
    char schema_header[MAX_BUF_SIZE] = {0};
    char currentTime_header[MAX_BUF_SIZE] = {0};
    char uuid_header[MAX_BUF_SIZE] = {0};

    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Start of createCurlheader\n");
    //Fetch auth JWT token from cloud.
    getAuthToken();

    // auth_header
    snprintf(auth_header, MAX_HEADER_LEN, "Authorization:Bearer %s", (0 < strlen(webpa_auth_token) ? webpa_auth_token : NULL));
    list = curl_slist_append(list, auth_header);

    //version_header
    if(ConfigFilePtr = fopen(CONFIG_VERSION_FILE,"r"))
    {
        fread(current_version, MAX_BUF_SIZE, 1, ConfigFilePtr);
        if(strlen(current_version)!=0)
        {
            snprintf(version_header, MAX_BUF_SIZE, "IF-NONE-MATCH:%s", current_version);
        }
        else
        {
            snprintf(version_header, MAX_BUF_SIZE, "IF-NONE-MATCH:%s",  "NONE");
        }
        fclose(ConfigFilePtr);
    }
    else
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite: FAILED opening </opt/persistent/webconfig_Version> configVersion file \n");
        snprintf(version_header, MAX_BUF_SIZE, "IF-NONE-MATCH:%s",  "NONE");
    }
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:version_header formed %s\n", version_header);
    list = curl_slist_append(list, version_header);


    //schema_header
    snprintf(schema_header, MAX_BUF_SIZE, "Schema-Version: %s", "v1.0");
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:schema_header formed %s\n", schema_header);
    list = curl_slist_append(list, schema_header);


    memset(currentTime, 0, sizeof(currentTime));
    current_time = time(NULL);
    snprintf(currentTime,sizeof(currentTime),"%d",(int)current_time);
    //currentTime_header
    snprintf(currentTime_header, MAX_BUF_SIZE, "X-System-Current-Time: %s", currentTime);
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:currentTime_header formed %s\n", currentTime_header);
    list = curl_slist_append(list, currentTime_header);
    if(transaction_uuid == NULL)
    {
        transaction_uuid = generate_trans_uuid();
    }

    if(transaction_uuid !=NULL)
    {
        //uuid_header
        snprintf(uuid_header, MAX_BUF_SIZE, "Transaction-ID: %s", transaction_uuid);
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:uuid_header formed %s\n", uuid_header);
        list = curl_slist_append(list, uuid_header);
        *trans_uuid = strdup(transaction_uuid);
        WAL_FREE(transaction_uuid);
    }
    else
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Failed to generate transaction_uuid\n");
    }
    *header_list = list;
}

static char* generate_trans_uuid()
{
    char *transID = NULL;
    uuid_t transaction_Id;
    char *trans_id = NULL;
    trans_id = (char *)malloc(MAX_UUID_SIZE);
    uuid_generate_random(transaction_Id);
    uuid_unparse(transaction_Id, trans_id);

    if(trans_id !=NULL)
    {
        transID = trans_id;
    }
    return transID;
}

static void execute_token_script(char *token, char *name, size_t len, char *mac, char *serNum)
{
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite: ****Inside execute token script**** \n");
    FILE* out = NULL, *file = NULL;
    char command[MAX_BUF_SIZE] = {'\0'};
    if(strlen(name)>0)
    {
        file = fopen(name, "r");
        if(file)
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Runn the commannd \n");
            snprintf(command,sizeof(command),"%s %s %s",name,serNum,mac);
            out = popen(command, "r");
            if(out)
            {
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Read the token \n");
                fgets(token, len, out);
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"webconfig_lite:Token =%s \n",token);
                pclose(out);
            }
            fclose(file);
        }
        else
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"File %s open error\n", name);
        }
    }
}

/*
 * call parodus create/acquisition script to create new auth token, if success then calls
 * execute_token_script func with args as parodus read script.
 */

static void createNewAuthToken(char *newToken, size_t len, char *hw_mac, char* hw_serial_number)
{
    int token_retry_count=0;

    while(1)
    {
        if(( access( CURL_FILE, F_OK ) != -1 ))
        {
            //Call create script
            char output[12] = {'\0'};
            execute_token_script(output,WEBPA_CREATE_HEADER,sizeof(output),hw_mac,hw_serial_number);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:***** inside create New Auth Token ***** \n");
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:**** the outout string =%s and lenngth =%d *** \n",output,strlen(output));
            if (strlen(output)>0  && strcmp(output,"SUCCESS")==0)
            {
                //Call read script
                execute_token_script(newToken,WEBPA_READ_HEADER,len,hw_mac,hw_serial_number);
            }
            else
            {
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Failed to create new token\n");
            }
            /*no retries are required*/
            break;
        }
        else
        {
            // file doesn't exist
            if(token_retry_count >60)
            {
                RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:token_retry_count has reached max limit. Exiting.\n");
                //token_retry_count=0;
                break;
            }
            sleep(BACKOFF_SLEEP_DELAY_5_SEC);
            token_retry_count++;
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Webconfig token_retry_count is %d\n", token_retry_count);
        }

    }

}

/*
 * Fetches authorization token from the output of read script. If read script returns "ERROR"
 * it will call createNewAuthToken to create and read new token
 */

static void getAuthToken()
{
    //local var to update webpa_auth_token only in success case
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:****Inside of getAuthToken ***** \n");
    char output[4069] = {'\0'};
    memset (webpa_auth_token, 0, sizeof(webpa_auth_token));

    if( strlen(WEBPA_READ_HEADER) !=0 && strlen(WEBPA_CREATE_HEADER) !=0)
    {
        execute_token_script(output, WEBPA_READ_HEADER, sizeof(output), deviceMac, serialNum);
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:The OutPut =%s \n",output);
        if ((strlen(output) == 0))
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Unable to get auth token\n");
        }
        else if(strcmp(output,"ERROR")==0)
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Failed to read token from %s. Proceeding to create new token.\n",WEBPA_READ_HEADER);
            //Call create/acquisition script
            createNewAuthToken(webpa_auth_token, sizeof(webpa_auth_token), deviceMac, serialNum );
        }
        else
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:update webpa_auth_token in success case\n");
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:The Aut Token =%s \n",output);
            //walStrncpy(webpa_auth_token, output, sizeof(webpa_auth_token));
            strncpy(webpa_auth_token, output, (sizeof(webpa_auth_token)-1));
        }

    }
    else
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Both read and write file are NULL \n");
    }
}


static void addWebConfigNotifyMsg(char *url, long status_code, char *application_status, int application_details, char *request_timestamp, char *version, char *transaction_uuid)
{
    notify_params_t *args = NULL;
    args = (notify_params_t *)malloc(sizeof(notify_params_t));

    if(args != NULL)
    {
        memset(args, 0, sizeof(notify_params_t));
        if(url != NULL)
        {
            args->url = strdup(url);
        }
        args->status_code = status_code;
        if(application_status != NULL)
        {
            args->application_status = strdup(application_status);
        }
        args->application_details = application_details;
        if(request_timestamp != NULL)
        {
            args->request_timestamp = strdup(request_timestamp);
        }
        if(version != NULL)
        {
            args->version = strdup(version);
        }
        if(transaction_uuid != NULL)
        {
            args->transaction_uuid = strdup(transaction_uuid);
        }
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:args->url:%s args->status_code:%d \
                args->application_status:%s args->application_details:%d args->request_timestamp:%s\
                args->version:%s args->transaction_uuid:%s\n", args->url, args->status_code,\
                args->application_status, args->application_details, args->request_timestamp,\
                args->version, args->transaction_uuid );

        processWebConfigNotification(args);
    }
}

//Notify thread function waiting for notify msgs
static void processWebConfigNotification(notify_params_t *n_param)
{
    char device_id[32] = { '\0' };
    cJSON *notifyPayload = NULL;
    char  * stringifiedNotifyPayload = NULL;
    char dest[512] = {'\0'};
    char source[MAX_BUF_SIZE] = {0};
    cJSON * reports, *one_report;

    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite: Entering processWebConfigNotification\n");
    if(n_param !=NULL)
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:Processing msg\n");
        snprintf(device_id, sizeof(device_id), "mac:%s", deviceMac);
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:webconfig Device_id %s\n", device_id);

        notifyPayload = cJSON_CreateObject();

        if(notifyPayload != NULL)
        {
            cJSON_AddStringToObject(notifyPayload,"device_id", device_id);

            cJSON_AddItemToObject(notifyPayload, "reports", reports = cJSON_CreateArray());
            cJSON_AddItemToArray(reports, one_report = cJSON_CreateObject());
            cJSON_AddStringToObject(one_report, "url", (NULL != n_param->url) ? n_param->url : "unknown");
            cJSON_AddNumberToObject(one_report,"http_status_code", n_param->status_code);
            if(n_param->status_code == 200)
            {
                cJSON_AddStringToObject(one_report,"document_application_status", (NULL != n_param->application_status) ? n_param->application_status : "unknown");
                cJSON_AddNumberToObject(one_report,"document_application_details", n_param->application_details);
            }
            cJSON_AddNumberToObject(one_report, "request_timestamp", (NULL != n_param->request_timestamp) ? atoi(n_param->request_timestamp) : 0);
            cJSON_AddStringToObject(one_report,"version", (NULL != n_param->version && (strlen(n_param->version)!=0)) ? n_param->version : "NONE");
            cJSON_AddStringToObject(one_report,"transaction_uuid", (NULL != n_param->transaction_uuid && (strlen(n_param->transaction_uuid)!=0)) ? n_param->transaction_uuid : "unknown");
            stringifiedNotifyPayload = cJSON_PrintUnformatted(notifyPayload);
            cJSON_Delete(notifyPayload);
        }

        snprintf(dest,sizeof(dest),"event:config-version-report/%s",device_id);
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:dest is %s\n", dest);

        if (stringifiedNotifyPayload != NULL && strlen(device_id) != 0)
        {
            strncpy(source, device_id, sizeof(device_id));
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:source is %s\n", source);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite:stringifiedNotifyPayload is %s\n", stringifiedNotifyPayload);
            sendNotification(stringifiedNotifyPayload, source, dest);
        }
        if(n_param != NULL)
        {
            free_notify_params_struct(n_param);
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite: n_param is NULL\n");
    }

}

static void free_notify_params_struct(notify_params_t *param)
{
    if(param != NULL)
    {
        if(param->url != NULL)
        {
            WAL_FREE(param->url);
        }
        if(param->application_status != NULL)
        {
            WAL_FREE(param->application_status);
        }
        if(param->request_timestamp != NULL)
        {
            WAL_FREE(param->request_timestamp);
        }
        if(param->version != NULL)
        {
            WAL_FREE(param->version);
        }
        if(param->transaction_uuid != NULL)
        {
            WAL_FREE(param->transaction_uuid);
        }
        WAL_FREE(param);
    }
}

static char *replaceMacWord(const char *s, const char *macW, const char *deviceMACW)
{
    char *result;
    int i, cnt = 0;
    int temp_var;
    int deviceMACWlen = strlen(deviceMACW);
    int macWlen = strlen(macW);
    // Counting the number of times mac word occur in the string
    for (i = 0; s[i] != '\0'; i++)
    {
        if (strstr(&s[i], macW) == &s[i])
        {
            cnt++;
            // Jumping to index after the mac word.
            i += macWlen - 1;
        }
    }

    result = (char *)malloc(i + cnt * (deviceMACWlen - macWlen) + 1);
    temp_var=(i + cnt * (deviceMACWlen - macWlen) + 1);
    i = 0;
    while (*s)
    {
        if (strstr(s, macW) == s)
        {
	    errno_t rc = -1;
            rc=strcpy_s(&result[i],(temp_var - i), deviceMACW);
	    if(rc!=EOK)
       	    {
    		    ERR_CHK(rc);
	    }
            i += deviceMACWlen;
            s += macWlen;
        }
        else
            result[i++] = *s++;
    }
    result[i] = '\0';
    return result;
}
