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
 * @file hostIf_jsonReqHandlerThread.cpp
 * @brief The header file provides HostIf IP json request handler information APIs.
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#include "hostIf_jsonReqHandler.h"
#include "hostIf_msgHandler.h"
#include "hostIf_utils.h"
#include <glib.h>
#include "libsoup-2.4/libsoup/soup.h"
#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>
extern T_ARGLIST argList;

typedef enum { STATE_BEFORE = 0, STATE_PARAM, STATE_PROCESSING, STATE_DONE } state_t;

typedef  struct {
    GList       *list;
    state_t     state;
    gboolean    grabString;
} parser_state_t;

SoupServer  *server = NULL;


/*
 * YAJL Parsing functions
 */

/* First, a couple nop's.  We don't care about these values.  Just proceed */
static int process_null(void * ctx) {
    return 1;
}
static int process_boolean(void * ctx, int boolVal) {
    return 1;
}
static int process_integer(void * ctx, long long integerVal) {
    return 1;
}
static int process_double(void * ctx, double doubleVal) {
    return 1;
}
static int process_number(void * ctx, const char * numberVal, unsigned int numberLen) {
    return 1;
}

/*
 * Grab a string, assuming we're in the right mode.
 */
static int process_string(void * ctx, const unsigned char * stringVal, unsigned int stringLen)
{
    parser_state_t  *context = (parser_state_t *) ctx;

    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"Process string: %d\n", context->state);
    if ((context->state == STATE_PROCESSING) && (context->grabString) && (stringLen<TR69HOSTIFMGR_MAX_PARAM_LEN-1))
    {
        char *string = strndup((const char *)stringVal, stringLen);
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"Adding string %s\n", string);
        context->list = g_list_append(context->list, string);
        context->grabString = FALSE;
    }

    return 1;
}

static int process_start_map(void * ctx)
{
    return 1;
}

static int process_map_key(void * ctx, const unsigned char * key, unsigned int stringLen)
{
    parser_state_t  *context = (parser_state_t *) ctx;
    char buffer[256];

    //RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"Process Map Key: %d %s\n", context->state, strof(key, stringLen));
    if ((context->state == STATE_BEFORE) &&
            (stringLen == strlen("paramList")) &&
            (strncmp("paramList", (const char *) key, stringLen) == 0))
    {
        context->state = STATE_PARAM;
    }
    else if ((context->state == STATE_PROCESSING) &&
             (stringLen == 4) &&
             (strncmp("name", (const char *) key, stringLen) == 0))
    {
        context->grabString = TRUE;
    }
    else
    {
        context->grabString = FALSE;
    }
    return 1;
}

static int process_end_map(void * ctx)
{
    // Do nothing right now
    return 1;
}

static int process_start_array(void * ctx)
{
    parser_state_t  *context = (parser_state_t *) ctx;
    if (context->state == STATE_PARAM)
    {
        context->state = STATE_PROCESSING;
    }
    return 1;
}

static int process_end_array(void * ctx)
{
    parser_state_t  *context = (parser_state_t *) ctx;
    if ((context->state == STATE_PROCESSING) || (context->state == STATE_PARAM))
    {
        context->state = STATE_DONE;
    }
    return 1;
}

static yajl_callbacks callbacks = {
    process_null,
    process_boolean,
    process_integer,
    process_double,
    process_number,
    process_string,
    process_start_map,
    process_map_key,
    process_end_map,
    process_start_array,
    process_end_array,
};

// Specify that we use the GLib memory allocation primitives.
/*
static yajl_alloc_funcs allocFuncs = {
    g_malloc,
    g_realloc,
    g_free
};
*/

void *jsonIfHandlerThread(void *msg)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    /*Start HTTP server */
    hostIf_HttpServerStart();

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return NULL;
}


GList *
hostIf_HTTPJsonParse(const unsigned char *message, int length)
{
    parser_state_t  context = { NULL, STATE_BEFORE, FALSE };
    yajl_status stat;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    yajl_handle parser = yajl_alloc(&callbacks, NULL, /* &allocFuncs */ NULL, (void *) (&context));
    //yajl_handle parser = yajl_alloc(&callbacks, NULL, NULL);
    stat = yajl_parse(parser, message, length);
    if (stat != yajl_status_ok)
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to parse in yajl_parse()\n", __FUNCTION__, __FILE__);
    } else {
        stat = yajl_parse_complete(parser);
        if (stat != yajl_status_ok)
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Failed to parse in yajl_parse_complete()\n", __FUNCTION__, __FILE__);
        }
    }
    if (parser) {
        yajl_free(parser);
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return context.list;
}


void hostIf_HTTPJsonMsgHandler(
    SoupServer        *server,
    SoupMessage       *msg,
    const gchar       *path,
    GHashTable        *query,
    SoupClientContext *client,
    gpointer           user_data)
{
    GList   *params;
    GList   *requestList = NULL;

    bool ret = true;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    if (!msg->request_body ||
            !msg->request_body->data ||
            !msg->request_body->length)
    {
        soup_message_set_status_full (msg, SOUP_STATUS_BAD_REQUEST, "No request data.");
        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting.. Failed due to no message data.\n", __FUNCTION__, __FILE__);
        return;
    }

    params = hostIf_HTTPJsonParse((const unsigned char *) msg->request_body->data, msg->request_body->length);
    if (!params)
    {
        soup_message_set_status_full (msg, SOUP_STATUS_BAD_REQUEST, "No request data.");
        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting... Failed due to Parse HTTP Json messages. \n", __FUNCTION__, __FILE__);
        return;
    }

    yajl_gen        json;
    json = yajl_gen_alloc(/* &allocFuncs */ NULL, NULL);
    if (!json)
    {
        soup_message_set_status_full (msg, SOUP_STATUS_INTERNAL_SERVER_ERROR, "Cannot create return object");
        RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting.. Failed to create json object\n", __FUNCTION__, __FILE__);
        return;
    }

    yajl_gen_map_open(json);
    yajl_gen_string(json, (const unsigned char *) "paramList", 9);
    yajl_gen_array_open(json);

    GList *l = params;
    while (l)
    {
        HOSTIF_MsgData_t *param = (HOSTIF_MsgData_t *) g_malloc0(sizeof(HOSTIF_MsgData_t));
        strncpy( param->paramName,(char *) l->data,TR69HOSTIFMGR_MAX_PARAM_LEN );
        // requestList = g_list_append(requestList, param);

        if (hostIf_GetMsgHandler(param) == OK) //We are expecting on Get call from JSON
        {
            yajl_gen_map_open(json);
            yajl_gen_string(json, (const unsigned char *) "name", 4);
            yajl_gen_string(json, (const unsigned char *) param->paramName, strlen(param->paramName));

            yajl_gen_string(json, (const unsigned char *) "value", 5);
            switch (param->paramtype) {
            case hostIf_StringType:
                yajl_gen_string(json, (const unsigned char*) param->paramValue, strlen((char*)param->paramValue));
                break;
            case hostIf_IntegerType:
            case hostIf_UnsignedIntType:
                yajl_gen_integer(json, get_int(param->paramValue));
                break;
            case hostIf_UnsignedLongType:
                yajl_gen_integer(json, get_ulong(param->paramValue));
                break;
            case hostIf_BooleanType:
                yajl_gen_bool(json, get_boolean(param->paramValue));
                break;
            case hostIf_DateTimeType:
                // TODO: What to do here?  What is the actual data representation?
                yajl_gen_string(json, (const unsigned char *) "Unknown", 7);
                break;
            default:
                RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"Unknown data type: %d", param->paramtype);
                break;
            }

            yajl_gen_map_close(json);
        }
        hostIf_Free_stMsgData(param);


        l = l->next;
    }
    // Free the list, but do NOT deallocate the strings.  They're now in the requestList
    g_list_free_full(params, g_free);
    params = NULL;

    // Close out the structures
    yajl_gen_array_close(json);
    yajl_gen_map_close(json);

    // Get the string
    const unsigned char     *buf;
    size_t                  len;
    yajl_gen_get_buf(json, &buf, &len);

    // TODO: What is the correct MIME type?
    soup_message_set_response(msg, (const char *) "application/json", SOUP_MEMORY_COPY, (const char *) buf, len);
    soup_message_set_status (msg, SOUP_STATUS_OK);

    yajl_gen_free(json);

    //json_t* incommingReq = NULL;
    //json_t* outRes = NULL;
    //	ret = hostIf_JsonReqResHandler (incommingReq, outRes);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return;
}

/**
 * @brief This API is used to initialize and start the HTTP server process. It use to serve the
 * http json client request.
 * @ingroup TR-069HOSTIF_JSON_REQHANDLER_API
 */
void hostIf_HttpServerStart()
{
    GError *error = NULL;
    guint port = argList.httpPort;
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);

    g_type_init();

    if(server == NULL)
        //server = soup_server_new (SOUP_SERVER_PORT, port, NULL);
        server = soup_server_new (SOUP_SERVER_SERVER_HEADER, "hostif", NULL);

    if (!server)
    {
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"SERVER: Could not create server.\n");
        // TODO: Return code
        return;
    }
    else
    {
        soup_server_add_handler (server, "/", hostIf_HTTPJsonMsgHandler, NULL, NULL);

        if(FALSE == soup_server_listen_local (server,  port, SOUP_SERVER_LISTEN_IPV4_ONLY, &error))
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "SERVER: failed in soup_server_listen_local. (%s).\n", error->message);
        }

        //soup_server_run(server);
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"SERVER: Started server successfully.\n");
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
}

/**
 * @brief This function is used to stop the http server. To stop the server
 * it disconnects the soup server with the server handle.
 * @ingroup TR-069HOSTIF_JSON_REQHANDLER_API
 */
void hostIf_HttpServerStop()
{
    if(server) {
        soup_server_disconnect(server);
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"SERVER: Stopped server successfully.\n");
    }
}



/** @} */
/** @} */
