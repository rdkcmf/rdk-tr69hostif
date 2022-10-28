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

#ifndef XRDKCENTRALCOMRFCVARSTORE_H
#define XRDKCENTRALCOMRFCVARSTORE_H

#include <unordered_map>
#include <string>

using namespace std;

#define XRFC_VAR_STORE_RELOADCACHE     "RFC_CONTROL_RELOADCACHE"

class XRFCVarStore
{
public:
    static XRFCVarStore* getInstance();

    string  getValue(const string &key);
    void reloadCache();

private:

    static XRFCVarStore* xrfcVarStoreInstance;
    string m_filename;
    bool initDone;


    std::unordered_map<std::string, std::string> m_dict;

    XRFCVarStore();
    XRFCVarStore(XRFCVarStore const&){};

    void initRFCVarFileName();
    bool loadRFCVarIntoCache();
};

#endif // XRDKCENTRALCOMRFCVARSTORE_H
