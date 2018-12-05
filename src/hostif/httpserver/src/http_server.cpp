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

extern T_ARGLIST argList;
static SoupServer  *http_server = NULL;

void printRespSt(res_struct *respSt)
{
    if(respSt != NULL)
    {
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"reqType = %d\n", respSt->reqType);
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"paramCnt = %d\n", respSt->paramCnt);
        switch(respSt->reqType)
        {
            case GET:
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"get_res_t -> paramCnt = %d\n", respSt->u.getRes->paramCnt);
                for(int i = 0; i < respSt->u.getRes->paramCnt; i++)
                {
                    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"paramName[%d] : %s\n", i, respSt->u.getRes->paramNames[i]);
                    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"retParamCnt = %d\n", respSt->u.getRes->retParamCnt[i]);
                    for(int j = 0; j < respSt->u.getRes->retParamCnt[i]; j++)
                    {
                        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"param[%d][%d].name = %s\n", i, j, respSt->u.getRes->params[i][j].name);
                        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"param[%d][%d].value = %s\n", i, j, respSt->u.getRes->params[i][j].value);
                        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"param[%d][%d].type = %d\n", i, j, respSt->u.getRes->params[i][j].type);
                    }
                    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"retStatus[%d] = %d\n", i, respSt->retStatus[i]);
                }
                break;
            default:
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"Not a GET response\n");
        };
    }
}

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

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    getCurrentTime(startPtr);
    if (!msg->request_body ||
            !msg->request_body->data ||
            !msg->request_body->length)
    {
        soup_message_set_status_full (msg, SOUP_STATUS_BAD_REQUEST, "No request data.");
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s:%s] Exiting.. Failed due to no message data.\n", __FUNCTION__, __FILE__);
        return;
    }

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
            parse_get_request(jsonRequest, &reqSt, WDMP_TR181);
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"Calling handleWebPARequest...\n");
            respSt = handleRequest(reqSt);
            if(respSt)
            {
                jsonResponse = cJSON_CreateObject();
                wdmp_form_get_response(respSt, jsonResponse);

                // WDMP Code sets a generic statusCode, the following lines replace it with an actual error code.
                cJSON_DeleteItemFromObject(jsonResponse, "statusCode");
                cJSON_AddNumberToObject(jsonResponse, "statusCode", respSt->retStatus[respSt->paramCnt-1]);
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
            parse_set_request(jsonRequest, &reqSt, WDMP_TR181);
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"Calling handleWebPARequest...\n");
            respSt = handleRequest(reqSt);
            if(respSt)
            {
                jsonResponse = cJSON_CreateObject();
                wdmp_form_set_response(respSt, jsonResponse);
                // WDMP Code sets a generic statusCode, the following lines replace it with an actual error code.
                cJSON_DeleteItemFromObject(jsonResponse, "statusCode");
                cJSON_AddNumberToObject(jsonResponse, "statusCode", respSt->retStatus[respSt->paramCnt-1]);
             }
             else
             {
                 soup_message_set_status_full (msg, SOUP_STATUS_INTERNAL_SERVER_ERROR, "Invalid request format");
                 RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s:%s] Exiting.. Request couldn't be processed\n", __FUNCTION__, __FILE__);
                 return;
             }
        }
        else
        {
            soup_message_set_status_full (msg, SOUP_STATUS_NOT_IMPLEMENTED, "Method not implemented");
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s:%s] Exiting.. Unsupported operation \n", __FUNCTION__, __FILE__);
            return;
        }
        char *buf = cJSON_Print(jsonResponse);
        soup_message_set_response(msg, (const char *) "application/json", SOUP_MEMORY_COPY, buf, strlen(buf));
        soup_message_set_status (msg, SOUP_STATUS_OK);

        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"freeing reqSt\n");
        wdmp_free_req_struct(reqSt);
        reqSt = NULL;
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"freeing jsonRequest\n");
        cJSON_Delete(jsonRequest);

        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"freeing jsonResponse\n");
        cJSON_Delete(jsonResponse);
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"freeing respSt\n");
//        printRespSt(respSt);
        wdmp_free_res_struct(respSt);
        respSt = NULL;

        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"freeing json buf\n");
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
    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return;
}


void *HTTPServerStartThread(void *msg)
{
    /*Start HTTP server */
    GError *error = NULL;
    int status =-1;
    guint httpServerPort = argList.httpServerPort;

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

#ifndef GLIB_VERSION_2_36
    g_type_init ();
#endif

    status = loadDataModel();
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

        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"SERVER: Started server successfully.\n");
    }

    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return NULL;
}

void HttpServerStop()
{
    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"SERVER: Stopping HTTP Server....\n");
    if(http_server) {
        soup_server_disconnect(http_server);
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"SERVER: Stopped server successfully.\n");
    }
}

