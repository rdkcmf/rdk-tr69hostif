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

/**
 * @file Device_DeviceInfo.cpp
 * @brief This source file contains the APIs for logging telemetry parameter
 *  information.
 */

/**
 * @file telemetry_logger.cpp
 *
 * @brief telemetry logger API Implementation.
 *
 * This is the implementation of the telemetry logging APIs.
 *
 * @par Document
 * TBD Relevant design or API documentation.
 *
 */


/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/
#include "Device_DeviceInfo.h"
#include "telemetry_logger.h"

/**
* @defgroup telemetry_logger
* @{
* @defgroup telemetry_logger
* @{
**/

TLOG::TelemetryLogger* TLOG::TelemetryLogger::m_instance = NULL;
extern pthread_cond_t cond_telemetry;
extern pthread_mutex_t mutex_telemetry;

const gchar *doscIfTelemeteryFile = "/etc/docsIf_telemetry_parameters.conf";

TLOG::TelemetryLogger::TelemetryLogger()
    : m_status(false), m_poll_interval(0)
{
//    m_status = false;
    setLogInterval(0);
    read_TelemeteryParamListFile();
}

TLOG::TelemetryLogger::~TelemetryLogger()
{
    //delete m_instance;
//    if (m_docsIf_telemetryParams.paramlist != NULL)    {
//        g_list_free_full(m_docsIf_telemetryParams.paramlist, (GDestroyNotify)free_TelemetryParamList);
//    }
}

#if 0
TLOG::TelemetryLogger* TLOG::TelemetryLogger::getInstance()
{
    if(NULL != m_instance)
    {
        m_instance = new TelemetryLogger();
        //m_instance->read_TelemeteryParamListFile();
    }
    return m_instance;
}
#endif

void TLOG::TelemetryLogger::setLogInterval( int poll_interval)
{
    m_poll_interval = poll_interval;
}

void TLOG::TelemetryLogger::startLogging()
{
#ifdef SNMP_ADAPTER_ENABLED
    log_docsIf_parameters();
#endif
    return;
}

void TLOG::TelemetryLogger::stop()
{

}

#ifdef SNMP_ADAPTER_ENABLED
void TLOG::TelemetryLogger::log_docsIf_parameters()
{
    RDK_LOG( RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s:%d] Enter\n", __FUNCTION__, __LINE__ );
    if(NULL != m_docsIf_telemetryParams.paramlist)
    {
        RDK_LOG( RDK_LOG_INFO, LOG_TR69HOSTIF, "***** Telemetry Log (START) ***** \n");

        HOSTIF_MsgData_t stMsgData;
        GList *iter = g_list_first(m_docsIf_telemetryParams.paramlist);

        while(iter)
        {
            RDK_LOG( RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s()] Parameter Name: %s \n", __FUNCTION__,  (char *)iter->data);

            memset(&stMsgData, 0, sizeof(HOSTIF_MsgData_t));
            hostIf_snmpAdapter::getLock();
            hostIf_snmpAdapter *snmpIns = hostIf_snmpAdapter::getInstance(0);

            if(snmpIns) {
                sprintf(stMsgData.paramName, "%s", (const char*)iter->data);
                stMsgData.instanceNum = 0;
                stMsgData.paramtype = hostIf_StringType;
                if(OK == snmpIns->get_ValueFromSNMPAdapter(&stMsgData)) {

                    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "%s:%s  \n",stMsgData.paramName, stMsgData.paramValue);
                }
            }
            hostIf_snmpAdapter::releaseLock();
            iter = g_list_next(iter);
        }
        RDK_LOG( RDK_LOG_INFO, LOG_TR69HOSTIF, "***** Telemetry Log (END) ***** \n");
    }

    RDK_LOG( RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s:%d] Exit\n",__FUNCTION__, __LINE__ );
}
#endif

bool TLOG::TelemetryLogger::read_TelemeteryParamListFile()
{
    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s()] Entering... \n",__FUNCTION__);
    gchar *contents = NULL;
    gsize length = 0;
    GError *error = NULL;

    gboolean fstatus =  g_file_get_contents (doscIfTelemeteryFile, &contents, &length, &error);
    if(!fstatus) {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s()] Failed to read \"%s\" file using g_file_get_contents() due to %s(%d) \n",
                __FUNCTION__, doscIfTelemeteryFile, error->message, error->code);
        return false;
    }
    else {
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s]Successfully read \"%s\". The file Contents are \"%s\" with length (%d).\n ",\
                __FUNCTION__, doscIfTelemeteryFile,  contents, (int)length);
    }
    if(contents)  {
        parse_Telemetry_logging_configuration(contents);
        g_free(contents);
    }

    return true;
    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s()] Exiting... \n",__FUNCTION__);
}

/*Print parameters in list*/
void TLOG::TelemetryLogger::printf_list_info(GList *list)
{
    if(!list) {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s] Since list is NULL, Failed to print info.\n", __FUNCTION__);
        return;
    }
    GList *iter = g_list_first(list);
    while(iter)
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"[%s] Parameter Name :  \"%s\"\n", __FUNCTION__, (char *)iter->data);
        iter = g_list_next(iter);
    }
}

bool TLOG::TelemetryLogger::parse_Telemetry_logging_configuration(gchar *string)
{
    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s()] Entering... \n",__FUNCTION__);
    if(NULL == string)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s:%d] Failed due to NULL request buffer.\n", __FUNCTION__, __LINE__);
        return false;
    }

    m_docsIf_telemetryParams.timePeriod = 0;
    m_docsIf_telemetryParams.paramlist = NULL;
    const gchar *period_interval =  "time_interval";
    const gchar *param_list = "docsif_parameter_list";

    update_TelemetryParams_list(string, &m_docsIf_telemetryParams, period_interval, param_list);

    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s()] Exiting... \n",__FUNCTION__);
    return true;
}

void TLOG::TelemetryLogger::free_TelemetryParamList (gpointer val)
{
    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s()] Exiting... \n",__FUNCTION__);
    if(val) {
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"[%s] :\"%s\"\n", __FUNCTION__, (gchar *)val);
        g_free (val);
    }
    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s()] Exiting... \n",__FUNCTION__);
}

bool TLOG::TelemetryLogger::update_TelemetryParams_list(gchar *input_buffer, telemetryParams *telemery_params, const gchar *time_period_string, const gchar *param_name_string)
{
    cJSON *request_msg = NULL;
    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s()] Entering... \n",__FUNCTION__);
    request_msg = cJSON_Parse(input_buffer);
    if (!request_msg)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Failed to parse json buffer with \"%s\"\n",cJSON_GetErrorPtr());
        return false;
    }
    else
    {
        int item, arrSize = 0;
        cJSON* param_item = NULL, *param_list_obj = NULL;
        telemery_params->timePeriod =  cJSON_GetObjectItem(request_msg, time_period_string)->valueint;
        setLogInterval(telemery_params->timePeriod);
        param_list_obj = cJSON_GetObjectItem(request_msg, param_name_string);
        arrSize = cJSON_GetArraySize(param_list_obj);
        for ( item = 0; item < arrSize; item++) {
            param_item = cJSON_GetArrayItem(param_list_obj, item);
            if(!param_item) {
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"[%s]Failed in cJSON_GetArrayItem() \n", __FUNCTION__);
            }
            else {
                telemery_params->paramlist = g_list_prepend(telemery_params->paramlist, g_strdup(param_item->valuestring));
            }
        }
        if(telemery_params->paramlist)
            telemery_params->paramlist = g_list_reverse(telemery_params->paramlist);
        cJSON_Delete(request_msg);
    }
    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s()] Exiting... \n",__FUNCTION__);
    return true;
}


void *telemetryLogger_thFunc(void *arg)
{
    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s()] Entering. \n",__FUNCTION__);

    /*Initialize telemetry log class */
    //TLOG::TelemetryLogger *tele_logger = TLOG::TelemetryLogger::getInstance();
    TLOG::TelemetryLogger *tele_logger = new TLOG::TelemetryLogger();

    //RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s():%d] Sleep for log Interval %d\n",__FUNCTION__, __LINE__, tele_logger->getLogInterval());
    while(true)
    {
        pthread_mutex_lock(&mutex_telemetry);
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s()] Wailting for cond mutex. \n",__FUNCTION__);
        pthread_cond_wait(&cond_telemetry, &mutex_telemetry);
        pthread_mutex_unlock(&mutex_telemetry);

        if(!hostIf_DeviceInfo::isTelemetryRFCEnabled())
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s()] \"%s\" is %d, so disabled telemetry logging. \n",
                    __FUNCTION__,TELEMETRY_RFC_ENABLE, hostIf_DeviceInfo::isTelemetryRFCEnabled());
        }
        while(hostIf_DeviceInfo::isTelemetryRFCEnabled()) {
            tele_logger->startLogging();
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s()] Sleep for log Interval %d\n",__FUNCTION__, tele_logger->getLogInterval());
            sleep(tele_logger->getLogInterval());
        }

    }
    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF, "[%s()] Entering. \n",__FUNCTION__);
}



