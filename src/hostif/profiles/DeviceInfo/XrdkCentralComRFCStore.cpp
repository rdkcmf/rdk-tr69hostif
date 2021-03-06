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

#include <algorithm>
#include <fstream>
#include <string>
#include <ctype.h>
#include <sstream>

#include "XrdkCentralComRFCStore.h"
#include "hostIf_utils.h"

#define TR181_RFC_STORE_KEY "TR181_STORE_FILENAME"
#define RFC_PROPERTIES_FILE "/etc/rfc.properties"
#define RFCDEFAULTS_FILE "/tmp/rfcdefaults.ini"
#define RFCDEFAULTS_ETC_DIR "/etc/rfcdefaults/"

#ifdef ENABLE_LLAMA_PLATCO
   #define TR181_LOCAL_STORE_KEY "TR181_LOCAL_STORE_AMLOGIC_FILENAME"
#else
   #define TR181_LOCAL_STORE_KEY "TR181_LOCAL_STORE_FILENAME"
#endif

#define TR181_CLEAR_PARAM "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.ClearParam"
XRFCStore* XRFCStore::xrfcInstance = NULL;

void XRFCStore::clearAll()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    if(!init())
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Init Failed, can't handle the request\n");
        return;
    }
    if(m_updateInProgress)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Duplicate clear DB request.. ignoring\n");
        return;
    }
    m_updateInProgress = true;

    ofstream ofs(m_filename, ofstream::trunc);
    ofs.close();

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
}

void XRFCStore::reloadCache()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

    if(!m_updateInProgress)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "ClearDB is not issued yet or Duplicate ClearDBEnd, .. ignoring\n");
        return;
    }
    m_initDone = loadTR181PropertiesIntoCache();

    m_updateInProgress = false;

    //copy the local settings from tr181localstore.ini to main DB tr181store.ini
    for (unordered_map<string, string>::iterator it=m_local_dict.begin(); it!=m_local_dict.end(); ++it)
    {
        if(m_dict.find(it->first) == m_dict.end())
            m_dict[it->first] = it->second;
    }
    ofstream ofs(m_filename, ios::trunc | ios::out);
    if(!ofs.is_open())
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Failed to open : %s \n", m_filename.c_str());
        return;
    }

    for (unordered_map<string, string>::iterator it=m_dict.begin(); it!=m_dict.end(); ++it)
    {
        ofs << it->first << '=' << it->second << endl;
    }
    ofs.flush();
    ofs.close();
    
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
}

string XRFCStore::getRawValue(const string &key)
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

    unordered_map<string,string>::const_iterator it = m_dict.find(key);
    if (it == m_dict.end()) {
        return "";
    }
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s : Value = %s \n", __FUNCTION__, it->second.c_str());

    return it->second;
}

bool XRFCStore::setRawValue(const string &key, const string &value)
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

    if(m_updateInProgress)
    {
        ofstream ofs(m_filename, ios::out | ios::app);

        if(!ofs.is_open())
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Failed to open : %s \n", m_filename.c_str());
            return false;
        }
        ofs << key << '=' << value << endl;
        ofs.flush();
        ofs.close();
    }
    else
    {
        return writeHashToFile(key, value, m_dict, m_filename);
    }

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    return true;
}

bool XRFCStore::writeHashToFile(const string &key, const string &value, unordered_map<string, string> &dict, string &filename)
{
    ofstream ofs(filename, ios::trunc | ios::out);

    if(!ofs.is_open())
    {   
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Failed to open : %s \n", filename.c_str());
        return false;
    }

    if (key.compare(TR181_CLEAR_PARAM))
    {
        dict[key] = value;
    }

    for (unordered_map<string, string>::iterator it=dict.begin(); it!=dict.end(); ++it)
    {   
        ofs << it->first << '=' << it->second << endl;
    }
    ofs.flush();
    ofs.close();

    return true;
}

faultCode_t XRFCStore::clearValue(const string &key, const string &value)
{
    bool foundInLocalStore = false;
    unordered_map<string, string> temp_local_dict(m_local_dict);

    for (unordered_map<string, string>::iterator it=temp_local_dict.begin(); it!=temp_local_dict.end(); ++it)
    {
        //Check if the param to be cleared is present in tr181localstore.ini
        //(Or) if the param to be cleared is a wild card, clear all the params from local store that match with the wild card.
        if(!it->first.compare(value) ||  (value.back() == '.' && it->first.find(value) != string::npos) )
        {
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "Clearing param: %s\n", it->first.c_str());
            m_local_dict.erase(it->first);
            m_dict.erase(it->first);
            foundInLocalStore = true;
        }
    }

    if (foundInLocalStore)
    {
        writeHashToFile(key, value, m_local_dict, m_local_filename);
        writeHashToFile(key, value, m_dict, m_filename);
    }
    else
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Parameter to be cleared is not present in tr181localstore.ini\n");

    return fcNoFault;
}

faultCode_t XRFCStore::getValue(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    if(!init())
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Init Failed, can't handle the request\n");
        return fcInternalError;
    }
    string rawValue = getRawValue(stMsgData->paramName);
    if(rawValue.length() > 0)
    {
        putValue(stMsgData, rawValue.c_str());
        stMsgData->faultCode = fcNoFault;
    }
    else
    {
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s : Parameter Not Found in %s\n", stMsgData->paramName, m_filename.c_str());
        unordered_map<string,string>::const_iterator it = m_dict_rfcdefaults.find(stMsgData->paramName);
        if (it != m_dict_rfcdefaults.end())
        {
           RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s : Param found in rfcdefaults\n", stMsgData->paramName);
           rawValue = it->second;
           if(rawValue.length() > 0)
           {
              putValue(stMsgData, rawValue.c_str());
              stMsgData->faultCode = fcNoFault;
           }
           else
           {
              RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s : Parameter Found in rfcdefaults is empty\n", stMsgData->paramName);
              stMsgData->faultCode = fcInternalError;
           }
        }
        else
        {
           RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s : Parameter Not Found in rfcdefaults\n", stMsgData->paramName);
           stMsgData->faultCode = fcInternalError;
        }
    }
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s\n", __FUNCTION__);
    return stMsgData->faultCode;
}

faultCode_t  XRFCStore::setValue(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    if(!init())
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Init Failed, can't handle the request\n");
        return fcInternalError;
    }
    const string &givenValue = getStringValue(stMsgData);

    if (!strcmp(stMsgData->paramName,TR181_CLEAR_PARAM))
    {
        return clearValue(stMsgData->paramName, givenValue);
    }

    if (stMsgData->requestor == HOSTIF_SRC_WEBPA) {
        if(!writeHashToFile(stMsgData->paramName, givenValue, m_local_dict, m_local_filename))
        {
            return fcInternalError;
        }
        //if the RFC processing is going on, do not perform local/webpa set on tr181store.ini
        if(m_updateInProgress)
        {
            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Local tr181 set during RFC processing for %s\n", stMsgData->paramName);
            return fcNoFault;
        }
    }
    
    if(!m_updateInProgress)
    {
        const string &currentValue = getRawValue(stMsgData->paramName);
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Given Value : %s ---- Current value :  %s \n", givenValue.c_str(), currentValue.c_str());

        if(strlen(currentValue.c_str()) > 0)
        {
            if(!strcasecmp(currentValue.c_str(), givenValue.c_str()))
            {
                RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Property value exists, don't have to overwrite\n");
                return fcNoFault;
            }
        }
    }

    if(setRawValue(stMsgData->paramName, givenValue))
    {
        stMsgData->faultCode = fcNoFault;
    }
    else
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Unable to Set Value for given Param\n");
        stMsgData->faultCode = fcInternalError;
    }
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    return stMsgData->faultCode;
}

void XRFCStore::initTR181PropertiesFileName()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    ifstream ifs_rfc(RFC_PROPERTIES_FILE);
    if(!ifs_rfc.is_open())
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Trying to open a non-existent file [%s] \n", __FUNCTION__, RFC_PROPERTIES_FILE);
    }
    else
    {
        string line;
        while (getline(ifs_rfc, line)) {
            size_t splitterPos = line.find('=');
            if (splitterPos < line.length()) {
                string key = line.substr(0, splitterPos);
                string value = line.substr(splitterPos+1, line.length());
                if(!strcmp(key.c_str(), TR181_RFC_STORE_KEY))
                {
                    m_filename = value;
                    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "TR181 Properties FileName = %s\n", m_filename.c_str());
                }
                else if(!strcmp(key.c_str(), TR181_LOCAL_STORE_KEY))
                {   
                    m_local_filename = value;
                    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "TR181 Local FileName = %s\n", m_local_filename.c_str());
                }
            }
        }
        ifs_rfc.close();

        if(m_filename.empty())
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Didn't find %s in %s\n", TR181_RFC_STORE_KEY, RFC_PROPERTIES_FILE);
        }
    }
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
}

bool init_rfcdefaults()
{
   DIR *dir;
   struct dirent *ent;
   if ((dir = opendir ( RFCDEFAULTS_ETC_DIR )) != NULL)
   {
      std::ofstream combined_file( RFCDEFAULTS_FILE, ios::out | ios::app ) ;
      while ((ent = readdir (dir)) != NULL )
      {
         if (strstr(ent->d_name, ".ini"))
         {
            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF,"rfcdefaults file: %s\n", ent->d_name);
            string filepath = RFCDEFAULTS_ETC_DIR;
            std::ifstream file1( filepath.append(ent->d_name) ) ;
            combined_file << file1.rdbuf();
            combined_file << "\n";
	 }
      }
      closedir (dir);
   }
   else
   {
      RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF,"Could not open dir %s \n", RFCDEFAULTS_ETC_DIR) ;
      return false;
   }
   return true;
}

bool XRFCStore::loadTR181PropertiesIntoCache()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    if(m_filename.empty())
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Invalid TR181 Properties filename, Unable to load properties\n");
        return false;
    }
    // get rid of quotes, it is quite common with properties files
    m_filename.erase(remove(m_filename.begin(), m_filename.end(), '\"'), m_filename.end());
    m_dict.clear();

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "TR181 Properties File :  %s \n", m_filename.c_str());
    ifstream ifs_tr181(m_filename);
    if (!ifs_tr181.is_open()) {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Trying to open a non-existent file [%s] \n", __FUNCTION__, m_filename.c_str());
    }
    else
    {
        string line;
        while (getline(ifs_tr181, line)) {
            size_t splitterPos = line.find('=');
            if (splitterPos < line.length()) {
                string key = line.substr(0, splitterPos);
                string value = line.substr(splitterPos+1, line.length());
                m_dict[key] = value;
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Key = %s : Value = %s\n", key.c_str(), value.c_str());
            }
        }
        ifs_tr181.close();
    }


    if(m_local_filename.empty())
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Invalid TR181 local filename, Unable to load properties\n");
        return false;
    }
    // get rid of quotes, it is quite common with properties files
    m_local_filename.erase(remove(m_local_filename.begin(), m_local_filename.end(), '\"'), m_local_filename.end());
    m_local_dict.clear();

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "TR181 Local File :  %s \n", m_local_filename.c_str());
    ifstream ifs_local_tr181(m_local_filename);
    if (!ifs_local_tr181.is_open()) {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Trying to open a non-existent file [%s] \n", __FUNCTION__, m_local_filename.c_str());
    }
    else
    {   
        string line;
        while (getline(ifs_local_tr181, line)) {
            size_t splitterPos = line.find('=');
            if (splitterPos < line.length()) {
                string key = line.substr(0, splitterPos);
                string value = line.substr(splitterPos+1, line.length());
                m_local_dict[key] = value;
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Key = %s : Value = %s\n", key.c_str(), value.c_str());
            }
        }
        ifs_local_tr181.close();
    }


    ifstream ifs_rfcdef(RFCDEFAULTS_FILE);
    if (!ifs_rfcdef.is_open()) {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Trying to open a non-existent file [%s] \n", __FUNCTION__, RFCDEFAULTS_FILE);

        if ( init_rfcdefaults())
        {
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Trying to open %s after newly creating\n", RFCDEFAULTS_FILE);
            ifs_rfcdef.open(RFCDEFAULTS_FILE, ifstream::in);
            if (!ifs_rfcdef.is_open())
                return false;
        }
        else
            return false;
    }
    
    string line;
    while (getline(ifs_rfcdef, line)) {
        size_t splitterPos = line.find('=');
        if (splitterPos < line.length()) {
            string key = line.substr(0, splitterPos);
            string value = line.substr(splitterPos+1, line.length());
            m_dict_rfcdefaults[key] = value;
            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Key = %s : Value = %s\n", key.c_str(), value.c_str());
        }
    }
    ifs_rfcdef.close();

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    return true;
}

bool XRFCStore::init()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    if(m_initDone)
        return m_initDone;

    initTR181PropertiesFileName();

    m_initDone = loadTR181PropertiesIntoCache();

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    return m_initDone;
}

XRFCStore::XRFCStore()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

    m_initDone = false;
    m_updateInProgress = false;

    init();

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
}

XRFCStore* XRFCStore::getInstance()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

    if(!xrfcInstance)
        xrfcInstance = new XRFCStore;

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    return xrfcInstance;
}
