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

#ifndef XRDKCENTRALCOMRFCVAR_H
#define XRDKCENTRALCOMRFCVAR_H

#include "hostIf_tr69ReqHandler.h"

#include <unordered_map>
#include <string>

using namespace std;

class XRFCVarStorage
{
public:
    static XRFCVarStorage* getInstance();
    void clearAll();
    void reloadCache();

    faultCode_t  getValue(HOSTIF_MsgData_t *stMsgData);
    faultCode_t  setValue(HOSTIF_MsgData_t *stMsgData);

    string getRawValue(const string &key);
    bool setRawValue(const string &key, const string &value);

private:

    static XRFCVarStorage* xrfcVarInstance;
    string m_filename;
    bool initDone;
    bool updateInProgress;


    std::unordered_map<std::string, std::string> m_dict;

    XRFCVarStorage();
    XRFCVarStorage(XRFCVarStorage const&){};
    XRFCVarStorage& operator=(XRFCVarStorage const&){};

    void initRFCVarFileName();
    bool loadRFCVarIntoCache();
    bool validateParamValue(const string &paramName, HostIf_ParamType_t dataType);
};

#endif // XRDKCENTRALCOMRFCVAR_H
