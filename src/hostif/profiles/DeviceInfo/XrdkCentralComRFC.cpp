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

#include "XrdkCentralComRFC.h"
#include "hostIf_main.h"
#include "hostIf_utils.h"

using namespace std;

const char *TR181_RFC_STORE_KEY = "TR181_STORE_FILENAME";
const char *RFC_PROPERTIES_FILE = "/etc/rfc.properties";

XRFCStorage::XRFCStorage()
{
    m_storageLoaded = false;
}

void XRFCStorage::clearAll()
{
    m_storage.clear();
}

int XRFCStorage::getValue(HOSTIF_MsgData_t *stMsgData)
{
    if (!init()) {
        return NOK;
    }

    int ret = NOK;

    const std::string &paramValue = m_storage.value(stMsgData->paramName);
    ret = paramValue.empty() ? NOK : OK;

    // FIXME:: problem with tr181Set, it expects OK and will call set on a value if the
    // get return ok. This override should be removed one tr181Set is fixed.
    ret = OK;

    // if there is no value no need to unnecessary work, just return failure
    if (ret == NOK) {
        //stMsgData->faultCode = fcInvalidParameterName;
        return ret;
    }

    // FIXME:: we don't set type of param. It should have happen before the profiles.
    // everyone should follow data-model.xml and no one should be expected to set it
    // or overrirde.

    putValue(stMsgData, paramValue);

    return ret;
}

int  XRFCStorage::setValue(HOSTIF_MsgData_t *stMsgData)
{
    if (!init()) {
        return NOK;
    }

    // FixMe:: validate
    const string &valueString = getStringValue(stMsgData);

    const string &oldValue = m_storage.value(stMsgData->paramName);

    // if nothing changed, don't do anything
    if (oldValue == valueString) {
        return OK;
    }

    // store
    int ret = NOK;
    if ((m_storage.setValue(stMsgData->paramName, valueString))) {
        ret = OK;
    }
    return ret;
}

string XRFCStorage::getRawValue(const string &key)
{
    if (!init()) {
        return "";
    }

    return m_storage.value(key);
}

bool XRFCStorage::setRawValue(const string &key, const string &value)
{
    if (!init()) {
        return false;
    }

    return m_storage.setValue(key, value);
}


bool XRFCStorage::init()
{
    if (m_storageLoaded) {
        return true;
    }

    // get the file path
    IniFile file;
    file.load(RFC_PROPERTIES_FILE);
    m_storageFile = file.value(TR181_RFC_STORE_KEY);

    if (m_storageFile.empty()) {
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF, "Looks like %s doesn't contain the key %s\n", __FUNCTION__, __LINE__, RFC_PROPERTIES_FILE, TR181_RFC_STORE_KEY);
        return false;
    }

    m_storage.load(m_storageFile);

    m_storageLoaded = true;

    return true;
}
