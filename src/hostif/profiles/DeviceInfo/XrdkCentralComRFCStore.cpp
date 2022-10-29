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
#include <string.h>
#include <ctype.h>
#include <sstream>
#include "semaphore.h"
#include <fcntl.h>

#include "XrdkCentralComRFCStore.h"
#include "hostIf_utils.h"

#define TR181_RFC_STORE_KEY "TR181_STORE_FILENAME"
#define RFC_PROPERTIES_FILE "/etc/rfc.properties"
#define RFCDEFAULTS_FILE "/tmp/rfcdefaults.ini"
#define TR181_STORE_NONPERSISTENT_FILE "/opt/secure/RFC/tr181store_nonpersist.ini"
#define TR181_RFC_NONPERSISTENT_PREFIX "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.NonPersistent."
#define RFCDEFAULTS_ETC_DIR "/etc/rfcdefaults/"

#ifdef ENABLE_LLAMA_PLATCO
   #define TR181_LOCAL_STORE_KEY "TR181_LOCAL_STORE_AMLOGIC_FILENAME"
#else
   #define TR181_LOCAL_STORE_KEY "TR181_LOCAL_STORE_FILENAME"
#endif

#define TR181_CLEAR_PARAM "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.ClearParam"
XRFCStore* XRFCStore::xrfcInstance = NULL;
const char *semName = "localstore";

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

    ofstream ofs2(TR181_STORE_NONPERSISTENT_FILE, ofstream::trunc);
    ofs2.close();

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

    if (strstr((char *)key.c_str(),TR181_RFC_NONPERSISTENT_PREFIX) != NULL)
    {
        ifstream ifs_rfc("/tmp/.rfcSyncDone");
        if(!ifs_rfc.is_open())
        {
           RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: file /tmp/.rfcSyncDone doesn't exist, use default value.\n", __FUNCTION__);
           return "";
        }
        unordered_map<string,string>::const_iterator it = m_nonpersist_dict.find(key);
        if (it == m_nonpersist_dict.end()) {
            return "";
        }
        RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s : Value = %s \n", __FUNCTION__, it->second.c_str());

        return it->second;
    }

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

bool XRFCStore::writeHashToFile(const string &key, const string &value, unordered_map<string, string> &dict, const string &filename)
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

    sem_t *sem_id = sem_open(semName, O_CREAT, 0600, 1);
    if (sem_id == SEM_FAILED){
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: sem_open failed \n", __FUNCTION__);
        return fcInternalError;
    }

    if (sem_wait(sem_id) < 0)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: sem_wait failed \n", __FUNCTION__);
    }

    loadFileToCache(m_local_filename, m_local_dict);
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

    if (sem_post(sem_id) < 0)
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: sem_post failed \n", __FUNCTION__);

    if (sem_close(sem_id) != 0){
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: sem_close failed \n", __FUNCTION__);
    }

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
           RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s : Parameter Not Found in rfcdefaults\n", stMsgData->paramName);
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

    if (strstr(stMsgData->paramName,TR181_RFC_NONPERSISTENT_PREFIX) != NULL)
    {
        if(stMsgData->requestor == HOSTIF_SRC_RFC)
        {
            if(!writeHashToFile(stMsgData->paramName, givenValue, m_nonpersist_dict, TR181_STORE_NONPERSISTENT_FILE))
            {
                RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Failed to write to nonpersistent file \n");
                return fcInternalError;
            }
            else
            {
                return fcNoFault;
            }
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Only RFC is allowed to set NonPersistent params.\n");
            return fcInternalError;
        }
    }

    if (stMsgData->requestor == HOSTIF_SRC_WEBPA) {

        sem_t *sem_id = sem_open(semName, O_CREAT, 0600, 1);
        if (sem_id == SEM_FAILED){
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: sem_open failed \n", __FUNCTION__);
            return fcInternalError;
        }

        if (sem_wait(sem_id) < 0)
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: sem_wait failed \n", __FUNCTION__);
        }

        loadFileToCache(m_local_filename, m_local_dict);
        bool writeStatus = writeHashToFile(stMsgData->paramName, givenValue, m_local_dict, m_local_filename);

        if (sem_post(sem_id) < 0)
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: sem_post failed \n", __FUNCTION__);

        if (sem_close(sem_id) != 0){
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: sem_close failed \n", __FUNCTION__);
        }

        if(!writeStatus)
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
                    // get rid of quotes, it is quite common with properties files
                    m_filename.erase(remove(m_filename.begin(), m_filename.end(), '\"'), m_filename.end());
                    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "TR181 Properties FileName = %s\n", m_filename.c_str());
                }
                else if(!strcmp(key.c_str(), TR181_LOCAL_STORE_KEY))
                {   
                    m_local_filename = value;
                    // get rid of quotes, it is quite common with properties files
                    m_local_filename.erase(remove(m_local_filename.begin(), m_local_filename.end(), '\"'), m_local_filename.end());
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

bool XRFCStore::loadFileToCache(const string &filename, unordered_map<string, string> &dict)
{
    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    if(filename.empty())
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s Invalid/empty filename, Unable to load properties\n", __FUNCTION__);
        return false;
    }
    dict.clear();

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s TR181 Properties File :  %s \n", __FUNCTION__, filename.c_str());
    ifstream ifs_tr181(filename);
    if (!ifs_tr181.is_open()) {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Trying to open a non-existent file [%s] \n", __FUNCTION__, filename.c_str());
    }
    else
    {
        string line;
        while (getline(ifs_tr181, line)) {
            size_t splitterPos = line.find('=');
            if (splitterPos < line.length()) {
                string key = line.substr(0, splitterPos);
                string value = line.substr(splitterPos+1, line.length());
                dict[key] = value;
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Key = %s : Value = %s\n", key.c_str(), value.c_str());
            }
        }
        ifs_tr181.close();
    }
    return true;
}

bool XRFCStore::loadTR181PropertiesIntoCache()
{
    loadFileToCache(m_filename, m_dict);

    sem_t *sem_id = sem_open(semName, O_CREAT, 0600, 1);
    if (sem_id == SEM_FAILED){
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: sem_open failed \n", __FUNCTION__);
        return fcInternalError;
    }

    if (sem_wait(sem_id) < 0)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: sem_wait failed \n", __FUNCTION__);
    }

    loadFileToCache(m_local_filename, m_local_dict);

    if (sem_post(sem_id) < 0)
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: sem_post failed \n", __FUNCTION__);

    if (sem_close(sem_id) != 0){
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: sem_close failed \n", __FUNCTION__);
    }

    loadFileToCache(TR181_STORE_NONPERSISTENT_FILE, m_nonpersist_dict);

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
