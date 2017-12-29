#include "XrdkCentralComRFC.h"
#include "hostIf_main.h"
#include "hostIf_utils.h"

#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

const char *TR181_RFC_STORE_KEY = "TR181_STORE_FILENAME";
const char *RFC_PROPERTIES_FILE = "/etc/rfc.properties";

IniFile::IniFile()
{
}

bool IniFile::load(const string &filename)
{
    m_filename = filename;

    // get rid of quotes, it is quite common with properties files
    m_filename.erase( remove(m_filename.begin(), m_filename.end(), '\"' ), m_filename.end() );

    m_dict.clear();

    ifstream inputStream(m_filename);

    if (!inputStream.is_open()) {
        RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF, "Trying to open a non existant file %s \n", __FUNCTION__, __LINE__, m_filename.c_str());
        return false;
    }

    string line;
    while (getline(inputStream, line)) {
        size_t splitterPos = line.find('=');
        if (splitterPos < line.length()) {
            string key = line.substr(0, splitterPos);
            string value = line.substr(splitterPos+1, line.length());
            m_dict[key] = value;
        }
    }

    return true;
}

void IniFile::clear()
{
    m_dict.clear();
}

string IniFile::value(const string &key, const string &defaultValue) const
{
    map<string,string>::const_iterator it = m_dict.find(key);
    if (it == m_dict.end()) {
        return defaultValue;
    }

    return it->second;
}

bool IniFile::setValue(const string &key, const string &value)
{
    if (m_filename.empty()) {
        return false;
    }

    m_dict[key] = value;

   // set results in a write back.
   ofstream outputStream(m_filename);
   if (!outputStream.is_open()) {
       RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF, "will not write, open on file %s failed \n", __FUNCTION__, __LINE__, m_filename.c_str());
       return false;
   }

   map<string,string>::const_iterator it = m_dict.begin();
   do {
       outputStream << it->first << '=' << it->second << endl;
       it++;
   }  while (it != m_dict.end());

   return true;
}

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
