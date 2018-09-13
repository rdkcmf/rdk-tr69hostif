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

#include "XrdkCentralComRFCVar.h"
#include "hostIf_utils.h"

#define RFC_VAR_KEY "RFC_VAR_FILENAME"
#define RFC_PROPERTIES_FILE "/etc/rfc.properties"

XRFCVarStore* XRFCVarStore::xrfcVarStoreInstance = NULL;

void XRFCVarStore::reloadCache()
{
   RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

   initDone = loadRFCVarIntoCache();

   RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
}

string XRFCVarStore::getValue(const string &key)
{
   RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
   if(!initDone)
   {
      RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Init Failed, can't handle the request\n");
      return "";
   }

    unordered_map<string,string>::const_iterator it = m_dict.find(key);
    if (it == m_dict.end()) {
        return "";
    }
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s : Value = %s \n", __FUNCTION__, it->second.c_str());

    return it->second;
}

void XRFCVarStore::initRFCVarFileName()
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
                if(!strcmp(key.c_str(), RFC_VAR_KEY))
                {
                  m_filename = value;
                  RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "RFC Variables FileName = %s\n", m_filename.c_str());
                }
            }
        }
        ifs_rfc.close();

        if(m_filename.empty())
      {
         RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Didn't find %s in %s\n", RFC_VAR_KEY, RFC_PROPERTIES_FILE);
      }
   }
   RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
}

bool XRFCVarStore::loadRFCVarIntoCache()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    if(m_filename.empty())
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Invalid RFC Variables filename, Unable to load properties\n");
        return false;
    }
    // get rid of quotes, it is quite common with properties files
    m_filename.erase(remove(m_filename.begin(), m_filename.end(), '\"'), m_filename.end());
    m_dict.clear();

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "RFC Variables File :  %s \n", m_filename.c_str());
    ifstream ifs_rfcVar(m_filename);
    if (!ifs_rfcVar.is_open()) {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Trying to open a non-existent file [%s] \n", __FUNCTION__, m_filename.c_str());
        return false;
    }
    else
    {
        string line;
        while (getline(ifs_rfcVar, line)) {
            line=line.substr(line.find_first_of(" \t")+1);//Remove the export word that is before the key
            size_t splitterPos = line.find('=');
            if (splitterPos < line.length()) {
                string key = line.substr(0, splitterPos);
                string value = line.substr(splitterPos+1, line.length());
                m_dict[key] = value;
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Key = %s : Value = %s\n", key.c_str(), value.c_str());
            }
        }
        ifs_rfcVar.close();
    }
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    return true;
}

XRFCVarStore::XRFCVarStore()
{
   RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

   initRFCVarFileName();

   initDone = loadRFCVarIntoCache();

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
}

XRFCVarStore* XRFCVarStore::getInstance()
{
   RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

   if(!xrfcVarStoreInstance)
      xrfcVarStoreInstance = new XRFCVarStore;

   RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
   return xrfcVarStoreInstance;
}
