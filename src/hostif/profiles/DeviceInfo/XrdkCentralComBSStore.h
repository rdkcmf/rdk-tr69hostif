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

#ifndef XRDKCENTRALCOMBSSTORE_H
#define XRDKCENTRALCOMBSSTORE_H

#include <unordered_map>
#include <string>
#include <thread>
#include <mutex>
#include <XrdkCentralComBSStoreJournal.h>
using namespace std;

class XBSStore
{
public:
    static XBSStore* getInstance();
    void stop();
    faultCode_t  getValue(HOSTIF_MsgData_t *stMsgData);
    faultCode_t  overrideValue(HOSTIF_MsgData_t *stMsgData);

private:
    static XBSStore* xbsInstance;
    static XBSStoreJournal* xbsJournalInstance;
    string m_filename;
    bool m_initDone;
    bool m_initialUpdate;
    bool m_rfcUpdateInProgress;
    thread partnerIdThread;
    static string m_partnerId;
    static mutex mtx;
    static bool m_stopped;
    std::unordered_map<std::string, std::string> m_dict;

    XBSStore();
    XBSStore(XBSStore const&){};
    XBSStore& operator=(XBSStore const&){};

    bool init();
    string getRawValue(const string &key);
    bool setRawValue(const string &key, const string &value, HostIf_Source_Type_t sourceType);

    void initBSPropertiesFileName();
    bool loadBSPropertiesIntoCache();
    void resetCacheAndStore();
    bool loadFromJson();
    bool clearRfcValues();
    static void getAuthServicePartnerID();
};

#endif // XRDKCENTRALCOMBSSTORE_H
