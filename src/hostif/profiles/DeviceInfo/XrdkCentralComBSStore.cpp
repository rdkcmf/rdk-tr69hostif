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

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <ctype.h>
#include <curl/curl.h>
#include "cJSON.h"

#include "XrdkCentralComBSStore.h"
#include "hostIf_utils.h"
#include "rfcapi.h"

#define BS_STORE_KEY "BS_STORE_FILENAME"
#define BS_JOURNAL_KEY "BS_JOURNAL_FILENAME"
#define RFC_PROPERTIES_FILE "/etc/rfc.properties"
#define BS_JSON_FILE "/etc/partners_defaults.json"
#define TR181_PARTNER_ID_KEY "Device.DeviceInfo.X_RDKCENTRAL-COM_Syndication.PartnerId"
#define BS_CLEAR_DB_START "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.Control.ClearDB"
#define BS_CLEAR_DB_END "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Bootstrap.Control.ClearDBEnd"
#define RFC_SERVICE_LOCK "/tmp/.rfcServiceLock"

XBSStore* XBSStore::xbsInstance = NULL;
XBSStoreJournal* XBSStore::xbsJournalInstance = NULL;
recursive_mutex XBSStore::mtx;
thread XBSStore::partnerIdThread;
bool XBSStore::m_stopped = false;
mutex XBSStore::mtx_stopped;
condition_variable XBSStore::cv;

size_t static writeCurlResponse(void *ptr, size_t size, size_t nmemb, string stream)
{
   size_t realsize = size * nmemb;
   string temp(static_cast<const char*>(ptr), realsize);
   stream.append(temp);
   return realsize;
}
void XBSStore::getAuthServicePartnerID()
{
    bool partnerIdChanged = false;
    auto sec = chrono::seconds(1);

    while(!m_stopped)
    {
        if ( partnerIdChanged )
        {
            ifstream f(RFC_SERVICE_LOCK);
            if (f.good())
            {
                RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: RFC service in progress. Retry after 30 sec\n", __FUNCTION__);
                unique_lock<mutex> lck(mtx_stopped);
                cv.wait_for(lck, 30*sec, []{return m_stopped;});
                continue;
            }
            else
            {
                //Invalidate RFC hash and retrigger so we can apply RFC overrides to the new partner configuration
                WDMP_STATUS status = setRFCParameter("tr69hostif", "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.ConfigSetHash", "INVALIDATE", WDMP_STRING);
                RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: RFC SET to Invalidate Hash status = %d \n", __FUNCTION__, status);

                status = setRFCParameter("tr69hostif", "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Control.RetrieveNow", "1", WDMP_UINT);
                RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: RFC SET to retrieveNow status = %d \n", __FUNCTION__, status);
                break;
            }
        }
        
        string newPartnerId = "";
        string response;
        CURL *curl = curl_easy_init();
        if(curl)
        {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: call curl to get partner ID..\n", __FUNCTION__);
            curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:50050/authService/getDeviceId");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCurlResponse);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            CURLcode res = curl_easy_perform(curl);
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: curl response : %d http response code: %ld\n", __FUNCTION__, res, http_code);
            curl_easy_cleanup(curl);

            if(res == CURLE_OK)
            {
                RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: curl response string = %s\n", __FUNCTION__, response.c_str());
                cJSON* root = cJSON_Parse(response.c_str());
                if(root)
                {
                    cJSON* partnerID    = cJSON_GetObjectItem(root, "partnerId");
                    if(partnerID->type == cJSON_String && partnerID->valuestring && strlen(partnerID->valuestring) > 0)
                    {
                        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "Found partnerID value = %s\n", partnerID->valuestring);
                        newPartnerId = partnerID->valuestring;
                    }
                    cJSON_Delete(root);
                }
                else
                {
                    RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: json parse error\n", __FUNCTION__);
                }
            }
        }
        else
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: curl init failed\n", __FUNCTION__);
        }
        if(newPartnerId.length() > 0)
        {
            string storedPartnerId = xbsInstance->getRawValue(TR181_PARTNER_ID_KEY);
            if (newPartnerId.compare(storedPartnerId) != 0)
            {
                RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF,"partnerId has changed\n");
                partnerIdChanged = true;
                mtx.lock();
                xbsInstance->resetCacheAndStore();
                xbsInstance->setRawValue(TR181_PARTNER_ID_KEY, newPartnerId.c_str(), HOSTIF_SRC_DEFAULT);
                xbsInstance->m_initDone = false;
                xbsInstance->init();
                mtx.unlock();
            }
            else
                break;
        }
        else
        {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: partnerId not found. Retry after 30 sec\n", __FUNCTION__);
            unique_lock<mutex> lck(mtx_stopped);
            cv.wait_for(lck, 30*sec, []{return m_stopped;});
        }
    }
}

void XBSStore::resetCacheAndStore()
{
   m_dict.clear();
   if (!m_filename.empty())
      remove(m_filename.c_str());
   xbsJournalInstance->resetCacheAndStore();
}

bool XBSStore::loadFromJson()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

    string partnerId = getRawValue(TR181_PARTNER_ID_KEY);

    if (partnerId.length() == 0)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: waiting on partnerId..\n", __FUNCTION__);
        return false;
    }

    ifstream ifs_json(BS_JSON_FILE);
    ifs_json.seekg(0, std::ios::end);
    size_t size = ifs_json.tellg();
    string buffer(size, ' ');
    ifs_json.seekg(0);
    ifs_json.read(&buffer[0], size);
    const char *jsonText = buffer.c_str();

    cJSON *json = cJSON_Parse(jsonText);
    if (!json)
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: json parse error \n", __FUNCTION__);
        return false;
    }

    cJSON* partnerConfig = cJSON_GetObjectItem(json, partnerId.c_str());
    if (!partnerConfig)
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"Unknown partner:%s, using default.\n", partnerId.c_str() );
        partnerConfig = cJSON_GetObjectItem(json, "default" );

        if (!partnerConfig)
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"Error for partner:%s, error = [%s]\n", partnerId.c_str(), cJSON_GetErrorPtr());
            return false;
        }
    }
    if(partnerConfig->type == cJSON_Object) {
        ifstream ifs_bsini(m_filename);
        if (ifs_bsini.is_open()) {
            RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "%s: File [%s] exist. Look for any default parameter changes.\n", __FUNCTION__, m_filename.c_str());
            ifs_bsini.close();
        }
        else
        {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: Initial Update of Bootstrap stores\n", __FUNCTION__);
            m_initialUpdate = true;
            xbsJournalInstance->setInitialUpdate(true);
        }

        cJSON *configObject = partnerConfig->child;

        //Check if any params were removed in firmware update
        bool removedEntries = false;
        for (unordered_map<string, string>::iterator it=m_dict.begin(); it!=m_dict.end();)
        {
            string key = it->first;
            if (!cJSON_GetObjectItem(partnerConfig, key.c_str()) && key.compare(TR181_PARTNER_ID_KEY) != 0)
            {
                RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: Remove param %s from bootstrap store, size=%d\n", __FUNCTION__, key.c_str(), m_dict.size());
                removedEntries = true;
                it = m_dict.erase(it);
                xbsJournalInstance->removeRecord(key);
            }
            else
            {
                it++;
            }
        }
        if (removedEntries)
        {
            ofstream ofs(m_filename, ios::trunc | ios::out);
            if(!ofs.is_open())
            {
                RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Failed to open : %s \n", m_filename.c_str());
                return false;
            }
            for (unordered_map<string, string>::iterator it=m_dict.begin(); it!=m_dict.end(); ++it)
            {
                ofs << it->first << '=' << it->second << endl;
            }
            ofs.flush();
            ofs.close();
            xbsJournalInstance->rfcUpdateEnd();
        }

        while( configObject )
        {
            char *configKey = configObject->string;
            char *configValue = configObject->valuestring;
            RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"bootstrap json parser: key=%s value=%s\n",configKey, configValue);
            setRawValue(configKey, configValue, HOSTIF_SRC_DEFAULT);
            configObject = configObject->next;
        }
        m_initialUpdate = false;
        xbsJournalInstance->setInitialUpdate(false);
    }
    else
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF,"partnerConfig->type not of object type...");
    cJSON_Delete(json);
    return true;
}

string XBSStore::getRawValue(const string &key)
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

    mtx.lock();
    unordered_map<string,string>::const_iterator it = m_dict.find(key);
    if (it == m_dict.end()) {
        mtx.unlock();
        return "";
    }
    mtx.unlock();
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s : Value = %s \n", __FUNCTION__, it->second.c_str());

    return it->second;
}

bool XBSStore::setRawValue(const string &key, const string &value, HostIf_Source_Type_t sourceType)
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, " %s: m_initialUpdate=%d\n", __FUNCTION__, m_initialUpdate);
    if(m_initialUpdate)
    {
        ofstream ofs(m_filename, ios::out | ios::app);

        if(!ofs.is_open())
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Failed to open : %s \n", m_filename.c_str());
            return false;
        }
        m_dict[key] = value;
        ofs << key << '=' << value << endl;
        ofs.flush();
        ofs.close();
    }
    else
    {
        const string &currentValue = getRawValue(key);
        RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Given Value : %s ---- Current value :  %s \n", value.c_str(), currentValue.c_str());

        if(strlen(currentValue.c_str()) > 0)
        {
            if(!strcasecmp(currentValue.c_str(), value.c_str()))
            {
                RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Property value exists, don't have to overwrite\n");
                if(sourceType != xbsJournalInstance->getJournalSource(key))
                   xbsJournalInstance->setJournalValue(key, value, sourceType); //Update the journal with the latest source so that the value will not be overriden by less favored source later.
                return true;
            }
        }

        //Keep an updated firmware value in the journal even though it is not active configuration.
        if(sourceType == HOSTIF_SRC_DEFAULT &&
           (xbsJournalInstance->getJournalSource(key) == HOSTIF_SRC_RFC || xbsJournalInstance->getJournalSource(key) == HOSTIF_SRC_WEBPA) )
        {
           RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "Update firmware value in journal even though it is not active..\n");
           xbsJournalInstance->setJournalValue(key, value, sourceType);
           return true;
        }

        ofstream ofs(m_filename, ios::trunc | ios::out);
        if(!ofs.is_open())
        {
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Failed to open : %s \n", m_filename.c_str());
            return false;
        }
        m_dict[key] = value;

        for (unordered_map<string, string>::iterator it=m_dict.begin(); it!=m_dict.end(); ++it)
        {
            ofs << it->first << '=' << it->second << endl;
        }
        ofs.flush();
        ofs.close();
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "Bootstrap config update: %s, %s, %s, %d-%s \n", key.c_str(), value.c_str(), currentValue.c_str(), sourceType, xbsJournalInstance->getUpdatedSourceString(sourceType).c_str());
    }
    xbsJournalInstance->setJournalValue(key, value, sourceType);

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    return true;
}

faultCode_t XBSStore::getValue(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    if(!init())
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "Init not complete...value may not be for latest partner ID\n");
    }
    string rawValue = getRawValue(stMsgData->paramName);
    if(rawValue.length() > 0)
    {
        putValue(stMsgData, rawValue.c_str());
        stMsgData->faultCode = fcNoFault;
    }
    else
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "%s : Parameter Not Found in %s\n", stMsgData->paramName, m_filename.c_str());
        stMsgData->faultCode = fcInternalError;
    }
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s\n", __FUNCTION__);
    return stMsgData->faultCode;
}

bool XBSStore::clearRfcValues()
{
    ofstream ofs(m_filename, ios::trunc | ios::out);
    if(!ofs.is_open())
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Failed to open : %s \n", m_filename.c_str());
        return false;
    }

    for (unordered_map<string, string>::iterator it=m_dict.begin(); it!=m_dict.end(); ++it)
    {
        string key = it->first;
        string defaultValue = "";
        if(xbsJournalInstance->clearRfcAndGetDefaultValue(key, defaultValue))
        {
            RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "%s: key=%s, defaultValue=%s\n", __FUNCTION__, key.c_str(), defaultValue.c_str());
            m_dict[key] = defaultValue;
        }
    }

    for (unordered_map<string, string>::iterator it=m_dict.begin(); it!=m_dict.end(); ++it)
    {
       ofs << it->first << '=' << it->second << endl;
    }
    ofs.flush();
    ofs.close();
    return true;
}

faultCode_t  XBSStore::overrideValue(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    if(!init())
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Init Failed, can't handle the request\n");
        return fcInternalError;
    }

    if (strcasecmp(stMsgData->paramName,BS_CLEAR_DB_START) == 0)
    {
        m_rfcUpdateInProgress = true;
        xbsJournalInstance->rfcUpdateStarted();
        stMsgData->faultCode = fcNoFault;
        return stMsgData->faultCode;
    }
    else if (strcasecmp(stMsgData->paramName,BS_CLEAR_DB_END) == 0)
    {
       clearRfcValues();
       xbsJournalInstance->rfcUpdateEnd();
       m_rfcUpdateInProgress = false;
       stMsgData->faultCode = fcNoFault;
       return stMsgData->faultCode;
    }

    mtx.lock();
    if (m_dict.find(stMsgData->paramName) == m_dict.end())
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Param does not exist in bootstrap store. Cannot override.\n");
        mtx.unlock();
        return fcInternalError;
    }

    const string &givenValue = getStringValue(stMsgData);

    HostIf_Source_Type_t sourceType;
    if(stMsgData->requestor == HOSTIF_SRC_WEBPA)
    {
        sourceType = HOSTIF_SRC_WEBPA;
    }
    else if(stMsgData->requestor == HOSTIF_SRC_RFC && xbsJournalInstance->getJournalSource(stMsgData->paramName) != HOSTIF_SRC_WEBPA)
    {
       sourceType = HOSTIF_SRC_RFC;
    }
    else
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "Bootstrap override rules not met: [requestor=%d]\n");
        mtx.unlock();
        return fcInternalError;
    }

    if (m_rfcUpdateInProgress)
        xbsJournalInstance->resetClearRfc(stMsgData->paramName);

    if(setRawValue(stMsgData->paramName, givenValue, sourceType))
    {
        stMsgData->faultCode = fcNoFault;
    }
    else
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "Unable to Set Value for given Param\n");
        stMsgData->faultCode = fcInternalError;
    }
    mtx.unlock();
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    return stMsgData->faultCode;
}

void XBSStore::initBSPropertiesFileName()
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
                if(!strcmp(key.c_str(), BS_STORE_KEY))
                {
                    m_filename = value;
                    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Bootstrap Properties FileName = %s\n", m_filename.c_str());
                }
                else if(!strcmp(key.c_str(), BS_JOURNAL_KEY))
                {
                    RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Bootstrap Journal FileName = %s\n", value.c_str());
                    xbsJournalInstance = XBSStoreJournal::getInstance(value.c_str());
                }
            }
        }
        ifs_rfc.close();

        if(m_filename.empty())
        {
            RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Didn't find %s in %s\n", BS_STORE_KEY, RFC_PROPERTIES_FILE);
        }
    }
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
}

bool XBSStore::loadBSPropertiesIntoCache()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    if(m_filename.empty())
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Invalid Bootstrap Properties filename, Unable to load properties\n");
        return false;
    }
    // get rid of quotes, it is quite common with properties files
    m_filename.erase(remove(m_filename.begin(), m_filename.end(), '\"'), m_filename.end());
    m_dict.clear();
    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "Bootstrap Properties File :  %s \n", m_filename.c_str());

    ifstream ifs_bsini(m_filename);
    if (ifs_bsini.is_open())
    {
        string line;
        while (getline(ifs_bsini, line)) {
            size_t splitterPos = line.find('=');
            if (splitterPos < line.length()) {
                string key = line.substr(0, splitterPos);
                string value = line.substr(splitterPos+1, line.length());
                m_dict[key] = value;
            }
        }
        ifs_bsini.close();
    }
    else
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: bootstrap ini does not exist...\n", __FUNCTION__);
    }

    if (!loadFromJson())
    {
        RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: loadFromJson() failed \n", __FUNCTION__);
        return false;
    }

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    return true;
}

bool XBSStore::init()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
    mtx.lock();
    if(m_initDone)
    {
        mtx.unlock();
        return m_initDone;
    }
    initBSPropertiesFileName();

    m_initDone = loadBSPropertiesIntoCache();

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    mtx.unlock();
    return m_initDone;
}

XBSStore::XBSStore()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

    m_initDone = false;
    m_initialUpdate = false;
    m_rfcUpdateInProgress = false;

    init();

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
}

XBSStore* XBSStore::getInstance()
{
    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

    if(!xbsInstance)
    {
        xbsInstance = new XBSStore;
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "%s: Start thread getAuthServicePartnerID \n", __FUNCTION__);
        partnerIdThread = thread(getAuthServicePartnerID);
    }

    RDK_LOG (RDK_LOG_TRACE1, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    return xbsInstance;
}

void XBSStore::stop()
{
    m_stopped = true;
    cv.notify_one();
    partnerIdThread.join();
}
