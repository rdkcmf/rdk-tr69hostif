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
 * @file libpd.cpp
 *
 * @description This file describes the wrapper function for libparodus library
 *
 */
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include "libpd.h"
#include "rdk_debug.h"
#include "cJSON.h"
#include "waldb.h"

#define CONTENT_TYPE_JSON     "application/json"
#define DEVICE_PROPS_FILE     "/etc/device.properties"
#define CLIENT_PORT_NUM       6667
#define URL_SIZE 	          64
#define LOG_FILE 	          "./tr69hostIflog.txt"
#define WEBPA_CONFIG_FILE     "/etc/webpa_cfg.json"
#define PARODUS_URL           "tcp://127.0.0.1:6666"
#define PAROUDUS_CLIENT_URL   "tcp://127.0.0.1:6667"


#ifdef __cplusplus
extern "C"
{
#endif
#include <libparodus.h>
const char *rdk_logger_module_fetch(void);
#ifdef __cplusplus
}
#endif

static void connect_parodus();
static void get_parodus_url(char *parodus_url, char *client_url);
static void parodus_receive_wait();
static long timeValDiff(struct timespec *starttime, struct timespec *finishtime);

/*----------------------------------------------------------------------------*/
/*                             Global Variables                             */
/*----------------------------------------------------------------------------*/
libpd_instance_t libparodus_instance;
char parodus_url[URL_SIZE] = {'\0'};
char client_url[URL_SIZE] = {'\0'};
bool exit_parodus_recv = false;
pthread_cond_t parodus_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t parodus_lock = PTHREAD_MUTEX_INITIALIZER;
/*----------------------------------------------------------------------------*/
/*                             External functions                             */
/*----------------------------------------------------------------------------*/


/**
 * Initialize libpd and Load Data model, Invoke connection to parodus
 */
void libpd_set_notifyConfigFile(const char* configFile)
{
    setInitialNotifyConfigFile(configFile);
}

void stop_parodus_recv_wait()
{
   int ret = 0;
   exit_parodus_recv = true;

   // Close libparodus receiver 
   ret = libparodus_close_receiver (libparodus_instance);
   if(ret == 0)
   {
       RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"Successfully closed libparodus receiver. \n");
   }
   else
   {
       RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"Failure in closing libparodus receiver, Ret = %d\n",ret);
   }
   // Unblock parodus thread from conditional wait.
   pthread_cond_signal(&parodus_cond);
}
/**
 * Initialize libpd and Load Data model, Invoke connection to parodus
 */
void *libpd_client_mgr(void *)
{
    // Load Data model
    int status =-1;
    RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"Loading DB \n");

    status = checkDataModelStatus();
    if(status != 0)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"Error in Database loading,Webpa Cannot be initialized..!! \n");
        return NULL;
    }
    else
    {
        // First connect to parodus
        connect_parodus();

        // Lets Register the Notify Callback function
        registerNotifyCallback();

        // Lets set the initial Notification
        setInitialNotify();

        // Call Parodus receive wait function
        parodus_receive_wait();
    }
    return NULL;
}


/**
 * Listen for Requests coming from Parodus and process
 */
static void parodus_receive_wait()
{
    int rtn;
    wrp_msg_t *wrp_msg;
    wrp_msg_t *res_wrp_msg ;

    struct timespec start,end,*startPtr,*endPtr,currTime;
    
    startPtr = &start;
    endPtr = &end;
    char *contentType = NULL;

    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Entering parodus_receive_wait.. \n");

    while (!exit_parodus_recv)
    {
        rtn = libparodus_receive (libparodus_instance, &wrp_msg, 2000);
        if (rtn == 1)
        {
            continue;
        }
        if (rtn != 0)
        {
            clock_gettime(CLOCK_MONOTONIC, &currTime);
            currTime.tv_sec += 5;
            pthread_mutex_lock(&parodus_lock);
            pthread_cond_timedwait(&parodus_cond, &parodus_lock,&currTime );
            pthread_mutex_unlock(&parodus_lock);
            continue;
        }

        if (wrp_msg && wrp_msg->msg_type == WRP_MSG_TYPE__REQ)
        {
            res_wrp_msg = (wrp_msg_t *)malloc(sizeof(wrp_msg_t));
            memset(res_wrp_msg, 0, sizeof(wrp_msg_t));

            getCurrentTime(startPtr);
            processRequest((char*)wrp_msg->u.req.payload, wrp_msg->u.req.transaction_uuid, ((char **)(&(res_wrp_msg->u.req.payload))));

            RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Response payload is %s\n",(char *)(res_wrp_msg->u.req.payload));
            if(res_wrp_msg->u.req.payload !=NULL)
            {
                res_wrp_msg->u.req.payload_size = strlen((const char *)res_wrp_msg->u.req.payload);
            }
            res_wrp_msg->msg_type = wrp_msg->msg_type;
            if(wrp_msg->u.req.dest != NULL)
                res_wrp_msg->u.req.source = strdup(wrp_msg->u.req.dest);
            if(wrp_msg->u.req.source != NULL) 
                res_wrp_msg->u.req.dest = strdup(wrp_msg->u.req.source);
            if(wrp_msg->u.req.transaction_uuid != NULL) 
                res_wrp_msg->u.req.transaction_uuid = strdup(wrp_msg->u.req.transaction_uuid);
            
            contentType = (char *)malloc(sizeof(char)*(strlen(CONTENT_TYPE_JSON)+1));
            strncpy(contentType,CONTENT_TYPE_JSON,strlen(CONTENT_TYPE_JSON)+1);
            res_wrp_msg->u.req.content_type = contentType;
            int sendStatus = libparodus_send(libparodus_instance, res_wrp_msg);
            
            if(sendStatus == 0)
            {
                RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"Sent message successfully to parodus\n");
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"Failed to send message: '%s'\n",libparodus_strerror((libpd_error_t )sendStatus));
            }
            getCurrentTime(endPtr);
            RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"Elapsed time : %ld ms\n", timeValDiff(startPtr, endPtr));
            wrp_free_struct (res_wrp_msg);
            wrp_free_struct(wrp_msg);
        }
    }
    libparodus_shutdown(&libparodus_instance);
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"End of parodus_upstream\n");
}

/**
 * Send Notification Message
 * @param[in] - Notification payload
 * @param[in] - Notification Source
 * @param[in] -  Notification Destination
 */
void sendNotification(char *payload, char *source, char *destination)
{
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"************Inside sendNotification ************\n");
    wrp_msg_t *notif_wrp_msg = NULL;
    int retry_count = 0;
    int sendStatus = -1;
    int backoffRetryTime = 0;
    int c=2;
    char *contentType = NULL;

    notif_wrp_msg = (wrp_msg_t *)malloc(sizeof(wrp_msg_t));
    memset(notif_wrp_msg, 0, sizeof(wrp_msg_t));

    notif_wrp_msg ->msg_type = WRP_MSG_TYPE__EVENT;
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"source: %s\n",source);
    notif_wrp_msg ->u.event.source = strdup(source);
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"destination: %s\n", destination);
    notif_wrp_msg ->u.event.dest = strdup(destination);
    contentType = (char *)malloc(sizeof(char)*(strlen(CONTENT_TYPE_JSON)+1));
    strncpy(contentType,CONTENT_TYPE_JSON,strlen(CONTENT_TYPE_JSON)+1);
    notif_wrp_msg->u.event.content_type = contentType;
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"content_type is %s\n",notif_wrp_msg->u.event.content_type);

    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Notification payload: %s\n",payload);
    notif_wrp_msg ->u.event.payload = (void *)payload;
    notif_wrp_msg ->u.event.payload_size = strlen((const char*)notif_wrp_msg ->u.event.payload);

    while(retry_count<=3)
    {
        backoffRetryTime = (int) pow(2, c) -1;

        sendStatus = libparodus_send(libparodus_instance, notif_wrp_msg );
        if(sendStatus == 0)
        {
            retry_count = 0;
            RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"Notification successfully sent to parodus\n");
            break;
        }
        else
        {
            RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"sendNotification backoffRetryTime %d seconds\n", backoffRetryTime);
            sleep(backoffRetryTime);
            c++;
            retry_count++;
        }
    }

    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"sendStatus is %d\n",sendStatus);
    wrp_free_struct (notif_wrp_msg );
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Freed notif_wrp_msg struct.\n");
}


/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/


/**
 * @brief initialize logging Module
 * @param[out] -  Logger module name
 */
const char *rdk_logger_module_fetch(void)
{
    return "LOG.RDK.PARADUSIF";
}

/**
 * @brief Fetch Parodus and client URL from device details
 * @param[in] -  parodus URL
 * @param[in] -  Clinet URL
 */
static void get_parodus_url(char *parodus_url, char *client_url)
{
    FILE *fp = NULL;
    int getStatus = 0;
    char *webpaCfgFile = NULL;
    int ch_count = 0;

    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Reading Parodus URL...\n");
    fp = fopen(WEBPA_CONFIG_FILE, "r");
    if (fp == NULL)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"Failed to open Webpa cfg file %s\n", WEBPA_CONFIG_FILE);
    }
    else
    {
        fseek(fp, 0, SEEK_END);
        ch_count = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        webpaCfgFile = (char *) malloc(sizeof(char) * (ch_count + 1));
        fread(webpaCfgFile, 1, ch_count,fp);
        webpaCfgFile[ch_count] ='\0';
        fclose(fp);
        if(ch_count < 1)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_PARODUS_IF,"WebPA config file is Empty %s\n", WEBPA_CONFIG_FILE);
        }
        else
        {
            cJSON *webpa_cfg = cJSON_Parse(webpaCfgFile);
            if(webpa_cfg)
            {
                cJSON *pUrl = NULL;
                cJSON *cUrl = NULL;

                pUrl = cJSON_GetObjectItem(webpa_cfg,"ParodusURL");
                cUrl = cJSON_GetObjectItem(webpa_cfg,"ParodusClientURL");
                if((NULL != pUrl && NULL != cUrl) && (NULL != pUrl->valuestring && NULL != pUrl->valuestring))
                {
                    strncpy(parodus_url,pUrl->valuestring,strlen(pUrl->valuestring));
                    strncpy(client_url,cUrl->valuestring,strlen(pUrl->valuestring));
                    getStatus = 1;
                }
            }
        }
    }
    // Set Default value if not configured
    if(!getStatus)
    {
        strncpy(parodus_url,PARODUS_URL,strlen(PARODUS_URL));
        strncpy(client_url,PAROUDUS_CLIENT_URL,strlen(PAROUDUS_CLIENT_URL));
    }
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Parodus URL formed = %s \n",parodus_url);
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"Client URL formed = %s \n",client_url);
}

/**
 * Open Connect to parodus service using libparodus
 */
static void connect_parodus()
{
    int backoffRetryTime = 0;
    int backoff_max_time = 5;
    int max_retry_sleep;
    //Retry Backoff count shall start at c=2 & calculate 2^c - 1.
    int c =2;
    int retval=-1;

    pthread_detach(pthread_self());

    max_retry_sleep = (int) pow(2, backoff_max_time) -1;
    RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"max_retry_sleep is %d\n", max_retry_sleep );

    get_parodus_url(parodus_url, client_url);

    libpd_cfg_t cfg1 = {.service_name = "config",
                        .receive = true, .keepalive_timeout_secs = 64,
                        .parodus_url = parodus_url,
                        .client_url = client_url
                       };

    RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"libparodus_init with parodus url %s and client url %s\n",cfg1.parodus_url,cfg1.client_url);

    while(1)
    {
        if(backoffRetryTime < max_retry_sleep)
        {
            backoffRetryTime = (int) pow(2, c) -1;
        }

        RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"New backoffRetryTime value calculated as %d seconds\n", backoffRetryTime);
        int ret =libparodus_init (&libparodus_instance, &cfg1);
        if(ret ==0)
        {
            RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"Init for parodus Success..!!\n");
            RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"WebPA is now ready to process requests\n");
            break;
        }
        else
        {
            sleep(backoffRetryTime);
            c++;

            if(backoffRetryTime == max_retry_sleep)
            {
                c = 2;
                backoffRetryTime = 0;
                RDK_LOG(RDK_LOG_INFO,LOG_PARODUS_IF,"backoffRetryTime reached max value, reseting to initial value\n");
            }
        }
        retval = libparodus_shutdown(&libparodus_instance);
        RDK_LOG(RDK_LOG_DEBUG,LOG_PARODUS_IF,"libparodus_shutdown retval %d\n", retval);
    }
}


static long timeValDiff(struct timespec *starttime, struct timespec *finishtime)
{
    long msec;
    msec=(finishtime->tv_sec-starttime->tv_sec)*1000;
    msec+=(finishtime->tv_nsec-starttime->tv_nsec)/1000000;
    return msec;
}
