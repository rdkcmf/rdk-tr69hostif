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

#include <XrdkCentralComBSStoreJournal.h>
#include "hostIf_utils.h"
#define VERSION_TXT_FILE "/version.txt"
#define BS_RFC_STR "rfc"
#define BS_WEBPA_STR "webpa"

XBSStoreJournal* XBSStoreJournal::xbsJournalInstance = NULL;

string XBSStoreJournal::getUpdatedSourceString(HostIf_Source_Type_t updatedSource)
{
   switch(updatedSource)
   {
      case HOSTIF_SRC_RFC:
         return BS_RFC_STR;
      case HOSTIF_SRC_WEBPA:
         return BS_WEBPA_STR;
      default:
         return "-";
   }
}

bool XBSStoreJournal::rfcUpdateStarted()
{
   RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
   if(!init())
   {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Init Failed, can't handle the request\n", __FUNCTION__);
        return false;
   }
   for (unordered_map<string, BS_JournalData_t>::iterator it=m_dict.begin(); it!=m_dict.end(); ++it)
   {
      string key = it->first;
      BS_JournalData_t journalData = it->second;
      if(journalData.updatedSource == HOSTIF_SRC_RFC)
      {
         RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "%s: clearRfc done for key = %s \n", __FUNCTION__, key.c_str());
         journalData.clearRfc = true;
         m_dict[key] = journalData;
      }
   }
}

bool XBSStoreJournal::rfcUpdateEnd()
{
   RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
   if(!init())
   {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Init Failed, can't handle the request\n", __FUNCTION__);
        return false;
   }
   ofstream ofs(m_filename, ios::trunc | ios::out);
   if(!ofs.is_open())
   {
       RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Failed to open : %s \n", m_filename.c_str());
       return false;
   }
   for (unordered_map<string, BS_JournalData_t>::iterator it=m_dict.begin(); it!=m_dict.end(); ++it)
   {
      BS_JournalData_t journalData = it->second;
      ofs << '"' << it->first << "\",\"" << journalData.fwValue << "\",\"" << journalData.buildTime <<"\",\"" << journalData.updatedValue <<"\",\"" << journalData.updatedTime <<"\",\"" << getUpdatedSourceString(journalData.updatedSource) << '"' << endl;
   }
   ofs.flush();
   ofs.close();
}

bool XBSStoreJournal::clearRfcAndGetDefaultValue(const string &key, string &defaultValue)
{
   if(!init())
   {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Init Failed, can't handle the request\n", __FUNCTION__);
        return false;
   }
   for (unordered_map<string, BS_JournalData_t>::iterator it=m_dict.begin(); it!=m_dict.end(); ++it)
   {
      if (key.compare(it->first) == 0)
      {
         BS_JournalData_t journalData = it->second;
         if(journalData.clearRfc)
         {
            journalData.updatedValue = "-";
            journalData.updatedTime = "-";
            journalData.updatedSource = HOSTIF_SRC_DEFAULT;
            journalData.clearRfc = false;
            m_dict[key] = journalData;
            defaultValue = journalData.fwValue;
            RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "%s: key=%s, defaultValue=%s \n", __FUNCTION__, key.c_str(), defaultValue.c_str());
            return true;
         }
         return false;
      }
   }
   return false;
}

bool XBSStoreJournal::resetClearRfc(const string &key)
{
   if(!init())
   {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Init Failed, can't handle the request\n", __FUNCTION__);
        return false;
   }
   for (unordered_map<string, BS_JournalData_t>::iterator it=m_dict.begin(); it!=m_dict.end(); ++it)
   {
      if (key.compare(it->first) == 0)
      {
         BS_JournalData_t journalData = it->second;
         if(journalData.clearRfc)
         {
            journalData.clearRfc = false;
            m_dict[key] = journalData;
            RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "%s: reset done for key=%s \n", __FUNCTION__, key.c_str());
            return true;
         }
         return false;
      }
   }
   return false;
}

bool XBSStoreJournal::removeRecord(const string &key)
{
   if(!init())
   {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Init Failed, can't handle the request\n", __FUNCTION__);
        return false;
   }
   m_dict.erase(key);
}

void XBSStoreJournal::resetCacheAndStore()
{
   m_dict.clear();
   if (!m_filename.empty())
      remove(m_filename.c_str());
}

bool XBSStoreJournal::loadJournalRecordsIntoCache()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    if(m_filename.empty())
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Invalid Journal filename, Unable to load properties\n");
        return false;
    }
    // get rid of quotes, it is quite common with properties files
    m_filename.erase(remove(m_filename.begin(), m_filename.end(), '\"'), m_filename.end());
    m_dict.clear();

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Bootstrap Journal File :  %s \n", m_filename.c_str());
    ifstream ifs_bsj(m_filename);
    if (!ifs_bsj.is_open()) {
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: Trying to open a non-existent file [%s] \n", __FUNCTION__, m_filename.c_str());
        return true;
    }
    else
    {
        string line;
        while (getline(ifs_bsj, line)) {
            stringstream ss(line);
            BS_JournalData_t journalData;
            if( ss.good() )
            {
               string key;
               getline( ss, key, ',' );
               key.erase(remove(key.begin(), key.end(), '\"'), key.end());

               getline( ss, journalData.fwValue, ',' );
               journalData.fwValue.erase(remove(journalData.fwValue.begin(), journalData.fwValue.end(), '\"'), journalData.fwValue.end());

               getline( ss, journalData.buildTime, ',' );
               journalData.buildTime.erase(remove(journalData.buildTime.begin(), journalData.buildTime.end(), '\"'), journalData.buildTime.end());

               getline( ss, journalData.updatedValue, ',' );
               journalData.updatedValue.erase(remove(journalData.updatedValue.begin(), journalData.updatedValue.end(), '\"'), journalData.updatedValue.end());

               getline( ss, journalData.updatedTime, ',' );
               journalData.updatedTime.erase(remove(journalData.updatedTime.begin(), journalData.updatedTime.end(), '\"'), journalData.updatedTime.end());

               string updatedSourceStr;
               getline( ss, updatedSourceStr, ',' );
               updatedSourceStr.erase(remove(updatedSourceStr.begin(), updatedSourceStr.end(), '\"'), updatedSourceStr.end());
               if (updatedSourceStr.compare(BS_RFC_STR) == 0)
                  journalData.updatedSource = HOSTIF_SRC_RFC;
               else if (updatedSourceStr.compare(BS_WEBPA_STR) == 0)
                  journalData.updatedSource = HOSTIF_SRC_WEBPA;
               else
                  journalData.updatedSource = HOSTIF_SRC_DEFAULT;

               m_dict[key] = journalData;
               RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF,"key = %s, fwValue = %s, buildTime = %s, updatedValue = %s, updatedTime = %s, updatedSource = %s\n", key.c_str(), journalData.fwValue.c_str(), journalData.buildTime.c_str(), journalData.updatedValue.c_str(), journalData.updatedTime.c_str(), getUpdatedSourceString(journalData.updatedSource).c_str());
           }
        }
        ifs_bsj.close();
    }
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    return true;
}

HostIf_Source_Type_t XBSStoreJournal::getJournalSource(const string &key)
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    unordered_map<string,BS_JournalData_t>::const_iterator it = m_dict.find(key);
    if (it == m_dict.end()) {
        return HOSTIF_NONE;
    }
    BS_JournalData_t journalData = it->second;
    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "getJournalSource UpdatedSource = %s \n", getUpdatedSourceString(journalData.updatedSource).c_str());

    return journalData.updatedSource;
}

string XBSStoreJournal::getBuildTime()
{
    string buildTime = "";
    ifstream ifs_version(VERSION_TXT_FILE);
    if(!ifs_version.is_open())
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Trying to open a non-existent file [%s] \n", __FUNCTION__, VERSION_TXT_FILE);
    }
    else
    {
        string line;
        while (getline(ifs_version, line)) {
            size_t splitterPos = line.find('=');
            if (splitterPos < line.length()) {
                string key = line.substr(0, splitterPos);
                string value = line.substr(splitterPos+1, line.length());
                if(!strcmp(key.c_str(), "BUILD_TIME"))
                {
                    buildTime = value;
                    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF, "buildTime = %s\n", buildTime.c_str());
                }
            }
        }
        ifs_version.close();

        if(!buildTime.empty())
        {
            // get rid of quotes, it is quite common with properties files
            buildTime.erase(remove(buildTime.begin(), buildTime.end(), '\"'), buildTime.end());
        }
    }
    return buildTime;
}

string XBSStoreJournal::getTime()
{
    time_t timer;
    char buffer[50];
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);
    strftime(buffer, 50, "%Y-%m-%d %H:%M:%S ", tm_info);
    return string(buffer);
}

bool XBSStoreJournal::setJournalValue(const string &key, const string &value, HostIf_Source_Type_t bsSource)
{
   if(!init())
   {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Init Failed, can't handle the request\n", __FUNCTION__);
        return false;
   }
   BS_JournalData_t journalData;
   auto iter = m_dict.find(key);
   if (iter == m_dict.end())
   {
       RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "key %s not found\n", key.c_str());
       if (bsSource != HOSTIF_SRC_DEFAULT)
       {
          journalData.fwValue = "-";
          journalData.buildTime = m_buildTime;
          journalData.updatedValue = value;
          journalData.updatedTime = getTime();
          journalData.updatedSource = bsSource;
          journalData.clearRfc = false;
          RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "Ths should not happen...Param is missing in journal file..Adding it..\n");
       }
       else
       {
          journalData.fwValue = value;
          journalData.buildTime = m_buildTime;
          journalData.updatedValue = "-";
          journalData.updatedTime = "-";
          journalData.updatedSource = HOSTIF_SRC_DEFAULT;
       }
   }
   else
   {
      journalData = iter->second;
      if (bsSource == HOSTIF_SRC_DEFAULT)
      {
         RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, " %s: Updating existing firmware value\n", __FUNCTION__);
         journalData.fwValue = value;
         journalData.buildTime = m_buildTime;
      }
      else
      {
         journalData.updatedValue = value;
         journalData.updatedTime = getTime();
         journalData.updatedSource = bsSource;
         journalData.clearRfc = false;
      }
   }

   RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, " %s: m_initialUpdate=%d\n", __FUNCTION__, m_initialUpdate);
   if (m_initialUpdate)
   {
      ofstream ofs(m_filename, ios::out | ios::app);

      if(!ofs.is_open())
      {
          RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Failed to open : %s \n", m_filename.c_str());
          return false;
      }
      m_dict[key] = journalData;
      ofs << '"' << key << "\",\"" << journalData.fwValue << "\",\"" << journalData.buildTime <<"\",\"" << journalData.updatedValue <<"\",\"" << journalData.updatedTime <<"\",\"" << getUpdatedSourceString(journalData.updatedSource) << '"' << endl;

        ofs.flush();
        ofs.close();
   }
   else
   {
      ofstream ofs(m_filename, ios::trunc | ios::out);
      if(!ofs.is_open())
      {
         RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Failed to open : %s \n", m_filename.c_str());
         return false;
      }

      m_dict[key] = journalData;

      for (unordered_map<string, BS_JournalData_t>::iterator it=m_dict.begin(); it!=m_dict.end(); ++it)
      {
         journalData = it->second;
         ofs << '"' << it->first << "\",\"" << journalData.fwValue << "\",\"" << journalData.buildTime <<"\",\"" << journalData.updatedValue <<"\",\"" << journalData.updatedTime <<"\",\"" << getUpdatedSourceString(journalData.updatedSource) << '"' << endl;
      }
      ofs.flush();
      ofs.close();
   }
   return true;
}

void XBSStoreJournal::setInitialUpdate(bool initialUpdate)
{
   m_initialUpdate = initialUpdate;
}

bool XBSStoreJournal::init()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    if(m_initDone)
        return m_initDone;

    m_initDone = loadJournalRecordsIntoCache();

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    return m_initDone;
}

XBSStoreJournal::XBSStoreJournal()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

    m_buildTime = getBuildTime();
    m_initDone = false;
    m_initialUpdate = false;

    init();

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
}

XBSStoreJournal::XBSStoreJournal(const char *fileName):m_filename(fileName)
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

    m_buildTime = getBuildTime();
    m_initDone = false;
    m_initialUpdate = false;

    init();

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
}

XBSStoreJournal* XBSStoreJournal::getInstance(const char *fileName)
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

    if(!xbsJournalInstance)
        xbsJournalInstance = new XBSStoreJournal(fileName);

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    return xbsJournalInstance;
}
