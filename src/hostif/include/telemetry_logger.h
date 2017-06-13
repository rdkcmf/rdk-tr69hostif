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
* @defgroup telemetry_logger
* @{
* @defgroup telemetry
* @{
**/

#ifndef _TELEMETRY_LOGGER_H_
#define _TELEMETRY_LOGGER_H_

#include <string>
#include "hostIf_utils.h"
#include "snmpAdapter.h"

extern "C" {
#include "cJSON.h"
}

typedef struct {
    int timePeriod;
    GList* paramlist;
} telemetryParams;


namespace TLOG
{
class TelemetryLogger
{
private:
    bool m_status;
    int m_poll_interval;						/* <!-- Logging Poll Interval, in minutes > */
    telemetryParams m_docsIf_telemetryParams;	/* <!-- docsIf parameter list > */

    static TelemetryLogger *m_instance;

    void setLogInterval( int poll_interval);
    bool read_TelemeteryParamListFile();
    void printf_list_info(GList *);
    bool parse_Telemetry_logging_configuration( gchar *);
    bool update_TelemetryParams_list(gchar *, telemetryParams *, const gchar *, const gchar *);
    void free_TelemetryParamList (gpointer );

#ifdef SNMP_ADAPTER_ENABLED
    void log_docsIf_parameters();
#endif
//    TelemetryLogger();
//	~TelemetryLogger();
public:

    TelemetryLogger();
    ~TelemetryLogger();
    //static TelemetryLogger* getInstance();
    //bool read_TelemeteryParamListFile();
    int getLogInterval() {
        return m_poll_interval;
    }

    void startLogging();
    void stop();
};
}


#endif /* _TELEMETRY_LOGGER_H_ */
