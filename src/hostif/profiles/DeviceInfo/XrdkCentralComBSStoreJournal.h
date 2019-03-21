/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2019 RDK Management
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

#ifndef XRDKCENTRALCOMBSSTOREJOURNAL_H
#define XRDKCENTRALCOMBSSTOREJOURNAL_H

#include <fstream>
#include <iostream>
#include <sstream>
#include "cJSON.h"
#include <unordered_map>
#include <algorithm>
#include "hostIf_msgHandler.h"

using namespace std;

typedef struct _BS_JournalData_t
{
   string fwValue;
   string buildTime;
   string updatedValue;
   string updatedTime;
   HostIf_Source_Type_t updatedSource;
   bool clearRfc;
} BS_JournalData_t;

class XBSStoreJournal
{
public:
    static XBSStoreJournal* getInstance(const char *fileName);

    HostIf_Source_Type_t getJournalSource(const string &key);
    bool setJournalValue(const string &key, const string &value, HostIf_Source_Type_t bsSource);
    void setInitialUpdate(bool initialUpdate);
    void resetCacheAndStore();
    bool rfcUpdateStarted();
    bool rfcUpdateEnd();
    bool clearRfcAndGetDefaultValue(const string &key, string &defaultValue);
    bool resetClearRfc(const string &key);
    bool removeRecord(const string &key);
    string getUpdatedSourceString(HostIf_Source_Type_t updatedSource);

private:
    static XBSStoreJournal* xbsJournalInstance;
    string m_filename;
    string m_buildTime;
    bool m_initDone;
    bool m_initialUpdate;

    std::unordered_map<std::string, BS_JournalData_t> m_dict;

    XBSStoreJournal();
    XBSStoreJournal(const char *);
    XBSStoreJournal(XBSStoreJournal const&){};
    XBSStoreJournal& operator=(XBSStoreJournal const&){};

    bool init();
    bool loadJournalRecordsIntoCache();
    string getBuildTime();
    string getTime();
};

#endif //XRDKCENTRALCOMBSSTOREJOURNAL_H
