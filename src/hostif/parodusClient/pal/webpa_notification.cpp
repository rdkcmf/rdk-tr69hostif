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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "webpa_notification.h"
#include "webpa_adapter.h"
#include "webpa_parameter.h"
#include <stddef.h>
#include "cJSON.h"
#include "rdk_debug.h"


#define WEBPA_NOTIFY_TYPE		   "VALUE_CHANGE_NOTIFICATION"
#define DEVICE_ESTB_MAC_PARAM 		"Device.DeviceInfo.X_COMCAST-COM_STB_MAC"
#define WEBPA_UNKNOWN_PARAM_VALUE 	"Unknown"
#define WEBPA_NOTIFY_SRC_LEN		32


char **g_notifyParamList = NULL;
unsigned int g_notifyListSize = 0;
const char* webpaNotifyConfigFile = NULL;
char* notificationSource = NULL;

void macToLower(char macValue[],char macConverted[]);


/**
 * @brief Initializes WebPA configuration file
 *
 * @return void.
 */
void setNotifyConfigurationFile(const char* nofityConfigFile)
{
    if(NULL != nofityConfigFile)
    {
        webpaNotifyConfigFile = nofityConfigFile;
        RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"Notify Configuration file set %s \n",webpaNotifyConfigFile);
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"Unable to set Notify Configuration file\n");
    }
}

/**
 * @brief Process Notification and generate payload
 *
 * @return void.
 */
char* processNotification(NotifyData *notifyMsg, char* payload)
{
	RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Entering processNotification...-\n");
	char* stringifiedNotifyPayload = NULL;

    if(notifyMsg)
    {

        cJSON * notifyPayload = NULL;
        ParamNotify *param = NULL;

        switch (notifyMsg->type)
        {
        case PARAM_NOTIFY:
        case TRANS_STATUS:
        case CONNECTED_CLIENT_NOTIFY:
        case UPSTREAM_MSG:
        case PARAM_VALUE_CHANGE_NOTIFY:
        {
            if(notifyMsg->data)
            {
                param = notifyMsg->data->notify;
                if(param)
                {
                    notifyPayload = cJSON_CreateObject();
                    cJSON_AddNumberToObject(notifyPayload, "datatype",param->type);
                    cJSON_AddStringToObject(notifyPayload, "paramName",param->paramName);
                    cJSON_AddStringToObject(notifyPayload, "notificationType",WEBPA_NOTIFY_TYPE);
                    switch(param->type)
                    {
                    case WAL_STRING:
                    {
                        cJSON_AddStringToObject(notifyPayload,"paramValue",param->newValue );
                        break;
                    }
                    case WAL_INT:
                    {
                        const int *paramNewValue = (const int *)param->newValue;
                        cJSON_AddNumberToObject(notifyPayload,"paramValue",*paramNewValue );
                        break;
                    }
                    case WAL_UINT:
                    {
                        const unsigned int *paramNewValue = (const unsigned int *)param->newValue;
                        cJSON_AddNumberToObject(notifyPayload,"paramValue",*paramNewValue );
                        break;
                    }
                    case WAL_BOOLEAN:
                    {
                        const bool  *paramNewValue = (const bool *)param->newValue;
                        cJSON_AddBoolToObject(notifyPayload,"paramValue",*paramNewValue);
                        break;
                    }
                    case WAL_ULONG:
                    {
                        const unsigned long *paramNewValue = (const unsigned long *)param->newValue;
                        cJSON_AddNumberToObject(notifyPayload,"paramValue",*paramNewValue);
                        break;
                    }
                    default:
                    {
                        cJSON_AddStringToObject(notifyPayload,"paramValue",param->newValue );
                        break;
                    }
                    }
		    stringifiedNotifyPayload = cJSON_PrintUnformatted(notifyPayload);
                    payload = strdup(stringifiedNotifyPayload);
                    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Notification Processed , Payload = %s\n",payload);
		    cJSON_Delete(notifyPayload);
		}
                else
                {
                    RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"ParamNotify is NULL.. !!\n");
                }
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"Notify data is NULL!!\n");
            }
        }
        break;
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF," Invalid Notification Message..!!\n");
    }
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Exiting processNotification...\n");
    return payload;
}
/**
 * @brief Get the Notification source
 *
 * @return void.
 */
char * getNotifySource()
{
    int ret = 0;
    int retCount = 0;

    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Entering .... %s \n", __FUNCTION__);

    if(NULL == notificationSource)
    {
        const char *getParamList[]= {DEVICE_ESTB_MAC_PARAM};
        ParamVal ***parametervalArr = (ParamVal ***) malloc(sizeof(ParamVal **) );
        getValues(getParamList, 1, NULL, parametervalArr, &retCount, (WAL_STATUS *)&ret);
        notificationSource = (char*) malloc(WEBPA_NOTIFY_SRC_LEN);
        if(notificationSource && parametervalArr[0][0] && parametervalArr[0][0]->value)
        {
            strncpy(notificationSource, parametervalArr[0][0]->value,strlen(parametervalArr[0][0]->value));
            RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"[%s] Estb MAC :-  %s ", __FUNCTION__,notificationSource);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"[%s] Unable to get Notification Source .!! \n", __FUNCTION__);
            strncpy(notificationSource, WEBPA_UNKNOWN_PARAM_VALUE,sizeof(WEBPA_UNKNOWN_PARAM_VALUE));
        }
        if(parametervalArr[0] && parametervalArr[0][0])
        {
            WAL_FREE(parametervalArr[0][0]->name);
            WAL_FREE(parametervalArr[0][0]->value);
        }
        if(parametervalArr[0])
        {
            WAL_FREE(parametervalArr[0][0]);
        }
        WAL_FREE(parametervalArr[0]);
    }
    return notificationSource;
}

/**
 * @brief Get the Notification Destination
 *
 * @return void.
 */
char* getNotifyDestination(char *notifyDest)
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Entering .... %s \n", __FUNCTION__);
    notifyDest = (char*) malloc(WEBPA_NOTIFY_SRC_LEN);
    if(notifyDest)
    {
        strcpy(notifyDest,"event:VALUE_CHANGE_NOTIFICATION");
        RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Dest = %s... %s \n",notifyDest, __FUNCTION__);
    }
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Exiting .... %s \n", __FUNCTION__);
    return notifyDest;
}

/**
 * @brief Read Notify configuration file from config file
 *
 * @return void.
 */
int getnotifyparamList(char ***notifyParamList,int *ptrnotifyListSize)
{
    char *temp_ptr = NULL;
    char *notifycfg_file_content = NULL;
    int i = 0;
    int ch_count = 0;
    FILE *fp = NULL;

    // Read file notification Configuration file
    if(NULL == webpaNotifyConfigFile)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"WebPA notification file path not set");
        return -1;
    }
    RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"Inside getnotifyparamList trying to open %s\n", webpaNotifyConfigFile);
    fp = fopen(webpaNotifyConfigFile, "r");
    if (fp == NULL)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"Failed to open cfg file %s\n", webpaNotifyConfigFile);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    ch_count = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    notifycfg_file_content = (char *) malloc(sizeof(char) * (ch_count + 1));
    fread(notifycfg_file_content, 1, ch_count,fp);
    notifycfg_file_content[ch_count] ='\0';
    fclose(fp);
    if(ch_count < 1)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"WebPA notification file is Empty %s\n", webpaNotifyConfigFile);
        return -1;
    }
    cJSON *notify_cfg = cJSON_Parse(notifycfg_file_content);
    cJSON *notifyArray = cJSON_GetObjectItem(notify_cfg,"Notify");
    if(NULL != notifyArray)
    {
        *ptrnotifyListSize =(int)cJSON_GetArraySize(notifyArray);
        *notifyParamList = (char **)malloc(sizeof(char *) **ptrnotifyListSize);
        for (i = 0 ; i < cJSON_GetArraySize(notifyArray) ; i++)
        {
            temp_ptr = cJSON_GetArrayItem(notifyArray, i)->valuestring;
            if(temp_ptr)
            {
                (*notifyParamList)[i] = (char *)malloc(sizeof(char ) * (strlen(temp_ptr)+1));
                strcpy((*notifyParamList)[i],temp_ptr);
                RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Notify Param  = %s\n", temp_ptr);
            }
        }
        // Update local Parameter list from generic layer
        if(NULL != notifyParamList && NULL != ptrnotifyListSize)
        {
            g_notifyParamList = *notifyParamList;
            g_notifyListSize = *ptrnotifyListSize;
        }
    }
    else
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"Unable to parse Configuration file");
    }
}





