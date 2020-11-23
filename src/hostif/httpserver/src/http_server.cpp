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

#include <glib.h>
#include <cJSON.h>
#include <stdlib.h>
#include <fstream>

#ifdef __cplusplus
extern "C"
{
#endif
#include <wdmp-c.h>
#include <wdmp_internal.h>
#ifdef __cplusplus
}
#endif

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "http_server.h"
#include "waldb.h"
#include "request_handler.h"
#include "libsoup/soup.h"
#include <mutex>
#include <condition_variable>

extern std::mutex mtx_httpServerThreadDone;
extern std::condition_variable cv_httpServerThreadDone;
extern bool httpServerThreadDone;

extern T_ARGLIST argList;
static SoupServer  *http_server = NULL;

static void HTTPRequestHandler(
    SoupServer        *server,
    SoupMessage       *msg,
    const char        *path,
    GHashTable        *query,
    SoupClientContext *client,
    void           *user_data)
{
    cJSON *jsonRequest = NULL;
    cJSON *jsonResponse = NULL;
    req_struct *reqSt = NULL;
    res_struct *respSt = NULL;

    struct timespec start,end,*startPtr,*endPtr;
    startPtr = &start;
    endPtr = &end;

    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    getCurrentTime(startPtr);
    if (!msg->request_body ||
            !msg->request_body->data ||
            !msg->request_body->length)
    {
        soup_message_set_status_full (msg, SOUP_STATUS_BAD_REQUEST, "No request data.");
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s:%s] Exiting.. Failed due to no message data.\n", __FUNCTION__, __FILE__);
        return;
    }

    const char *pcCallerID = (char *)soup_message_headers_get_one(msg->request_headers, "CallerID");

    jsonRequest = cJSON_Parse((const char *) msg->request_body->data);

    if(jsonRequest)
    {
        reqSt = (req_struct *)malloc(sizeof(req_struct));
        if(reqSt == NULL)
        {
            soup_message_set_status_full (msg, SOUP_STATUS_INTERNAL_SERVER_ERROR, "Cannot create return object");
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s:%s] Exiting.. Failed to create req_struct\n", __FUNCTION__, __FILE__);
            return;
        }
        memset(reqSt, 0, sizeof(req_struct));

        if(!strcmp(msg->method, "GET"))
        {
            if(!pcCallerID || !strlen(pcCallerID))
            {
                pcCallerID = "Unknown";
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s:%s] Unknown Caller ID, GET is allowed by default\n", __FUNCTION__, __FILE__);
            }
            else
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%s] GET with CallerID : %s..\n", __FUNCTION__, __FILE__, pcCallerID);
            parse_get_request(jsonRequest, &reqSt, WDMP_TR181);
            respSt = handleRequest(pcCallerID, reqSt);
            if(respSt)
            {
                jsonResponse = cJSON_CreateObject();
                wdmp_form_get_response(respSt, jsonResponse);

                // WDMP Code sets a generic statusCode, the following lines replace it with an actual error code.
		int new_st_code = 0;

                for(int paramIndex = 0; paramIndex < respSt->paramCnt; paramIndex++)
                {
                   if(respSt->retStatus[paramIndex] != 0 || paramIndex == respSt->paramCnt-1)
                   {
		      new_st_code =  respSt->retStatus[paramIndex];
                      break;
                   }
                }
		cJSON * stcode = cJSON_GetObjectItem(jsonResponse, "statusCode");
		if( NULL != stcode)
		{
		     cJSON_SetIntValue(stcode, new_st_code);
		}
            }
            else
            {
                soup_message_set_status_full (msg, SOUP_STATUS_INTERNAL_SERVER_ERROR, "Invalid request format");
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s:%s] Exiting.. Request couldn't be processed\n", __FUNCTION__, __FILE__);
                return;
            }
        }
        else if(!strcmp(msg->method, "POST"))
        {
            if(!pcCallerID || !strlen(pcCallerID))
            {
                soup_message_set_status_full (msg, SOUP_STATUS_INTERNAL_SERVER_ERROR, "POST Not Allowed without CallerID");
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s:%s] Exiting.. POST operation not allowed with unknown CallerID\n", __FUNCTION__, __FILE__);
                wdmp_free_req_struct(reqSt);
                reqSt = NULL;
                return;
            }
            else
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%s] POST with CallerID : %s..\n", __FUNCTION__, __FILE__, pcCallerID);

            parse_set_request(jsonRequest, &reqSt, WDMP_TR181);
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"Calling handleRequest...\n");
            respSt = handleRequest(pcCallerID, reqSt);
            if(respSt)
            {
                jsonResponse = cJSON_CreateObject();
                wdmp_form_set_response(respSt, jsonResponse);
                // WDMP Code sets a generic statusCode, the following lines replace it with an actual error code.
                int new_st_code = 0;

                for(int paramIndex = 0; paramIndex < respSt->paramCnt; paramIndex++)
                {
                   if(respSt->retStatus[paramIndex] != 0 || paramIndex == respSt->paramCnt-1)
                   {
                      new_st_code =  respSt->retStatus[paramIndex];
                      break;
                   }
                }
		cJSON * stcode = cJSON_GetObjectItem(jsonResponse, "statusCode");
                if( NULL != stcode)
                {
                     cJSON_SetIntValue(stcode, new_st_code);
                }
             }
             else
             {
                 soup_message_set_status_full (msg, SOUP_STATUS_INTERNAL_SERVER_ERROR, "Invalid request format");
                 RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s:%s] Exiting.. Request couldn't be processed\n", __FUNCTION__, __FILE__);
                 wdmp_free_req_struct(reqSt);
                 reqSt = NULL;
                 return;
             }
        }
        else
        {
            soup_message_set_status_full (msg, SOUP_STATUS_NOT_IMPLEMENTED, "Method not implemented");
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s:%s] Exiting.. Unsupported operation \n", __FUNCTION__, __FILE__);
            wdmp_free_req_struct(reqSt);
            reqSt = NULL;
            return;
        }
        char *buf = cJSON_Print(jsonResponse);
        soup_message_set_response(msg, (const char *) "application/json", SOUP_MEMORY_COPY, buf, strlen(buf));
        soup_message_set_status (msg, SOUP_STATUS_OK);

        wdmp_free_req_struct(reqSt);
        reqSt = NULL;
        cJSON_Delete(jsonRequest);
        cJSON_Delete(jsonResponse);
        wdmp_free_res_struct(respSt);
        respSt = NULL;
        free(buf);
        buf = NULL;
    }
    else
    {
        soup_message_set_status_full (msg, SOUP_STATUS_BAD_REQUEST, "Bad Request");
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s:%s] Exiting.. Failed to parse JSON Message \n", __FUNCTION__, __FILE__);
        return;
    }

    getCurrentTime(endPtr);
    RDK_LOG(RDK_LOG_DEBUG,LOG_TR69HOSTIF,"Curl Request Processing Time : %lu ms\n", timeValDiff(startPtr, endPtr));
    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return;
}


void *HTTPServerStartThread(void *msg)
{
    /*Start HTTP server */
    GError *error = NULL;
    int status =-1;
    guint httpServerPort = argList.httpServerPort;

    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

#ifndef GLIB_VERSION_2_36
    g_type_init ();
#endif

    status = checkDataModelStatus();
    if(status != 0)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"Error in Data Model Initialization\n");
        return NULL;
    }

    if(http_server == NULL)
        http_server = soup_server_new (SOUP_SERVER_SERVER_HEADER, "HTTPServer", NULL);

    if (!http_server)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"SERVER: Could not create server.\n");
        return NULL;
    }
    else
    {
        soup_server_add_handler (http_server, (const char*)"/", HTTPRequestHandler, NULL, NULL);

        if(FALSE == soup_server_listen_local (http_server,  httpServerPort, SOUP_SERVER_LISTEN_IPV4_ONLY, &error))
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"SERVER: failed in soup_server_listen_local. (%s).\n", error->message);
        }

        //Create a .http_server_ready file in /tmp for RFC to check whether http server is ready to accept requests
        ofstream ofs("/tmp/.tr69hostif_http_server_ready", ios::trunc | ios::out);
        if(!ofs.is_open())
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Failed to open : /tmp/.tr69hostif_http_server_ready \n");
        else
            ofs.close();

        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"SERVER: Started server successfully.\n");
    }

    {
        std::unique_lock<std::mutex> lck(mtx_httpServerThreadDone);
        httpServerThreadDone = true;
        cv_httpServerThreadDone.notify_all();
    }

    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return NULL;
}

void HttpServerStop()
{
    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"SERVER: Stopping HTTP Server....\n");
    if(http_server) {
        soup_server_disconnect(http_server);
        RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"SERVER: Stopped server successfully.\n");
    }
}

