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
#ifndef XRDKCENTRALCOMRFC_H
#define XRDKCENTRALCOMRFC_H

#include "hostIf_tr69ReqHandler.h"
#include "IniFile.h"
#include <string>

using namespace std;

class XRFCStorage
{
public:
    XRFCStorage();
    void clearAll();
    int  getValue(HOSTIF_MsgData_t *stMsgData);
    int  setValue(HOSTIF_MsgData_t *stMsgData);

    string getRawValue(const string &key);
    bool setRawValue(const string &key, const string &value);

private:
    bool                init();
    IniFile             m_storage;
    string              m_storageFile;
    bool                m_storageLoaded;
};

#endif // XRDKCENTRALCOMRFC_H
