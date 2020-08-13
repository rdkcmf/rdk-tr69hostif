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
 * @file webcfg_utils.cpp
 *
 * @description This file describes the utility functions for webconfig module
 *
 */

#include <webcfg_utils.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#include <webcfg_generic.h>

#define UNUSED(x) (void )(x)
#define RET_SUCCESS 100
#define RET_FAILURE 102

static char fw_version[64] = {0};
static char model_name[64] = {0};
static char device_mac[64] = {0};
static char force_sync[256] = {0};
static char force_sync_transID[256] = {0};

char *get_DB_BLOB_base64(void);
bool get_global_shutdown();
void set_global_shutdown(bool shutdown);
pthread_cond_t *get_global_sync_condition(void);
pthread_mutex_t *get_global_sync_mutex(void);
void initWebConfigMultipartTask(unsigned long status);
char *readWebcfgURL(void);

#define TR181_CMD "tr181 -s %s -v %s "

static int getKeyVal(char *key, char *value)
{
    FILE *fp = fopen(DEV_DETAILS_FILE, "r");
    int i,j;
    int rval = ERR;
    if (NULL != fp)
    {
        char appendkey[] = "=";
        char str[512] = {'\0'};
        char searchkey[64] = {'\0'};

        strncpy(searchkey, key, strlen(key));
        strcat(searchkey, appendkey);
        while(fscanf(fp,"%s", str) != EOF)
        {
            char *searchVal = NULL;
            if(searchVal = strstr(str, searchkey))
            {
                searchVal = searchVal + strlen(searchkey);
                strncpy(value, searchVal, (strlen(str) - strlen(searchkey))+1);
                rval = SUCCESS;
            }
        }
        fclose(fp);
        for (i=j=0; i<strlen(value); i++)
        {
            if (value[i] != ':')
                value[j++] = value[i];
        }
        value[j] = '\0';

        for(i=0;i<=strlen(value);i++){
            if(value[i]>=65&&value[i]<=90)
                value[i]=value[i]+32;
        }
    }
    if((strlen(value))<1)/*It means only key is populated in the file & value is yet to be*/
    {
        rval = ERR;
    }
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webcfg: [%s] %d key = %s, value = %s rval= %d\n\n", __FUNCTION__, __LINE__, key, value, rval);
    return rval;
}

char* get_deviceMAC(void)
{
    if(0 == device_mac[0])
        getKeyVal("estb_mac", device_mac);
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webcfg: [%s] %d %s \n", __FUNCTION__, __LINE__, device_mac);
    return device_mac;
}

int Get_Webconfig_URL( char *pString)
{
    char *webcfg_url = readWebcfgURL();

    if(webcfg_url) {
        strcpy(pString,webcfg_url);
        free(webcfg_url);
    } else {
        strcpy(pString, BLE_DETECTION_WEBCFG_ENDPOINT);
    }
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webcfg: [%s] %d %s \n", __FUNCTION__, __LINE__, pString);
    return 0;
}

char *getSerialNumber(void)
{
    char *serialNumber = " ";
    return strdup(serialNumber);
}

char *getModelName(void)
{
    char *device_model_name = NULL;
    if(0 == model_name[0])
        getKeyVal("model_number", model_name);
    device_model_name = strdup(model_name);
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webcfg: [%s] %d %s \n", __FUNCTION__, __LINE__, device_model_name);
    return device_model_name;
}

char *getFirmwareVersion(void)
{
    char *firmware_version = NULL;
    if(0 == fw_version[0])
        getKeyVal("imageVersion", fw_version);
    firmware_version = strdup(fw_version);
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webcfg: [%s] %d %s \n", __FUNCTION__, __LINE__, firmware_version);
    return firmware_version;
}

char *get_global_systemReadyTime(void)
{
    return strdup(" ");
}

char *getDeviceBootTime(void)
{
    return strdup(" ");
}

char *getProductClass(void)
{
    return strdup(" ");
}

char *getRebootReason(void)
{
    return strdup(" ");
}

void setValues(const param_t paramVal[], const unsigned int paramCount, const int setType, char *transactionId, money_trace_spans *timeSpan, WDMP_STATUS *retStatus, int *ccspStatus)
{
    UNUSED(setType);
    UNUSED(transactionId);
    UNUSED(timeSpan);

    int ret = 0, count = 0, len = 0;
    FILE *fp = NULL;
    char *cmd = NULL;
    for (count = 0; count < paramCount ; count++) {
        len = strlen(TR181_CMD) + strlen(paramVal[count].name) + strlen(paramVal[count].value);
        cmd = (char*)malloc(len);
        if (cmd) {
            memset(cmd,0,len);
            sprintf(cmd, TR181_CMD, paramVal[count].name, paramVal[count].value);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webcfg: [%s] %d command to execute %s \n", __FUNCTION__, __LINE__, cmd);
            fp = popen(cmd,"r");
            free(cmd);
        }
        if(fp) {
            if(-1 == fgetc(fp)) {
                *ccspStatus = RET_SUCCESS;
                *retStatus = WDMP_SUCCESS;
            } else {
                *ccspStatus = RET_FAILURE;
                *retStatus = WDMP_FAILURE;
            }
            pclose(fp);
            fp = NULL;
        } else {
            *ccspStatus = RET_FAILURE;
            *retStatus = WDMP_FAILURE;
        }
        if(WDMP_SUCCESS != *retStatus)
            break;
    }

    return;
}

int setForceSync(char* pString, char *transactionId,int *pStatus)
{
    memset(force_sync,0,sizeof(force_sync));
    strcpy(force_sync,pString);

    if((force_sync[0] !='\0') && (strlen(force_sync)>0)) {
        if(strlen(force_sync_transID)>0) {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webcfg: [%s] %d Force sync is already in progress, Ignoring this request.\n",__FUNCTION__, __LINE__);
            *pStatus = 1;
            return 0;
        } else {
            pthread_mutex_lock (get_global_sync_mutex());

            if(transactionId !=NULL && (strlen(transactionId)>0)) {
                strcpy(force_sync_transID, transactionId);
            }
            pthread_cond_signal(get_global_sync_condition());
            pthread_mutex_unlock(get_global_sync_mutex());
        }
    } else {
        memset(force_sync_transID,0,sizeof(force_sync_transID));
    }
    return 1;
}

int getForceSync(char** pString, char **transactionId )
{
    if((force_sync[0] != '\0') && strlen(force_sync)>0) {
        *pString = strdup(force_sync);
        *transactionId = strdup(force_sync_transID);
    } else {
        *pString = NULL;
        *transactionId = NULL;
        return 0;
    }
    return 1;
}


}
#endif
