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

#ifndef XRDKCENTRALCOMRFCSTORE_H
#define XRDKCENTRALCOMRFCSTORE_H

#include "hostIf_tr69ReqHandler.h"

#include <unordered_map>
#include <string>

using namespace std;

class XRFCStore
{
public:
    static XRFCStore* getInstance();
    void clearAll();
    void reloadCache();

    faultCode_t  getValue(HOSTIF_MsgData_t *stMsgData);
    faultCode_t  setValue(HOSTIF_MsgData_t *stMsgData);

private:

    static XRFCStore* xrfcInstance;
    string	m_filename;
    string m_local_filename;
    bool m_initDone;
    bool m_updateInProgress;

    std::unordered_map<std::string, std::string> m_dict;
    std::unordered_map<std::string, std::string> m_local_dict;
    std::unordered_map<std::string, std::string> m_nonpersist_dict;
    std::unordered_map<std::string, std::string> m_dict_rfcdefaults;

    XRFCStore();
    XRFCStore(XRFCStore const&){};
    XRFCStore& operator=(XRFCStore const&){};

    bool init();
    string getRawValue(const string &key);
    bool setRawValue(const string &key, const string &value);
    faultCode_t clearValue(const string &key, const string &value);
    bool writeHashToFile(const string &key, const string &value, unordered_map<string, string> &dict, const string &filename);
    void initTR181PropertiesFileName();
    bool loadFileToCache(const string &filename, unordered_map<string, string> &dict);
    bool loadTR181PropertiesIntoCache();
};

#endif // XRDKCENTRALCOMRFCSTORE_H
