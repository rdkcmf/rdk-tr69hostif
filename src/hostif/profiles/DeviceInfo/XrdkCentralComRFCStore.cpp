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

#include "XrdkCentralComRFCStore.h"
#include "hostIf_utils.h"

#define TR181_RFC_STORE_KEY "TR181_STORE_NEW_FILENAME"
#define RFC_PROPERTIES_FILE "/etc/rfc.properties"

XRFCStore* XRFCStore::xrfcInstance = NULL;

void XRFCStore::clearAll()
{
	RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
	if(!initDone)
	{
		RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Init Failed, can't handle the request\n");
		return;
	}
	if(updateInProgress)
	{
		RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Duplicate clear DB request.. ignoring\n");
		return;
	}
	updateInProgress = true;

	ofstream ofs(m_filename, ofstream::trunc);
	ofs.close();

	RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
}

void XRFCStore::reloadCache()
{
	RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

	initDone = loadTR181PropertiesIntoCache();

	updateInProgress = false;

	RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
}

bool XRFCStore::validateParamValue(const string &paramValue, HostIf_ParamType_t dataType)
{
	bool ret = true;
	size_t index = 0;
	switch(dataType)
	{
		case hostIf_StringType:
			if(paramValue.length() > TR69HOSTIFMGR_MAX_PARAM_LEN)
			{
				RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Parameter Value greater than allowed %d\n", TR69HOSTIFMGR_MAX_PARAM_LEN);
				ret = false;
			}
			break;

		case hostIf_IntegerType:
			if(isdigit(paramValue[0]))
			{
				int value = stoi(paramValue, &index);
				if(index != paramValue.length())
				{
					RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Invalid Parameter Value for an Integer Type\n");
					ret = false;
				}
			}
			else
			{
				ret = false;
			}
			break;

		case hostIf_UnsignedIntType:
		case hostIf_UnsignedLongType:
			if(isdigit(paramValue[0]))
			{
				unsigned long value = stoul(paramValue, &index);
				if(index != paramValue.length())
				{
					RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Invalid Parameter Value for an UnsignedInt or UnsignedLong Type\n");
					ret = false;
				}
			}
			else
			{
				ret = false;
			}
			break;

		case hostIf_BooleanType:
			if(!strcasecmp(paramValue.c_str(), "true")
					|| !strcasecmp(paramValue.c_str(), "false")
					|| !strcmp(paramValue.c_str(), "1")
					|| !strcmp(paramValue.c_str(), "0"))
			{
				RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Valid Boolean Value\n");
			}
			else
			{
				RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Invalid Parameter Value for a Boolean\n");
				ret = false;
			}
			break;

		case hostIf_DateTimeType:
			break;

		default:
			RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Unknown Data Type\n");
			ret = false;
			break;
	};

	return ret;
}

string XRFCStore::getRawValue(const string &key)
{
	RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
	if(!initDone)
	{
		RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Init Failed, can't handle the request\n");
		return "";
	}

    unordered_map<string,string>::const_iterator it = m_dict.find(key);
    if (it == m_dict.end()) {
        return "";
    }
    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Leaving %s : Value = %s \n", __FUNCTION__, it->second.c_str());

    return it->second;
}

bool XRFCStore::setRawValue(const string &key, const string &value)
{
	RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
	if(!initDone || updateInProgress)
	{
		RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Init Failed/reload DB in progress, can't handle SET request\n");
		return false;
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


    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    return true;
}

faultCode_t XRFCStore::getValue(HOSTIF_MsgData_t *stMsgData)
{
	RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
	if(!initDone)
	{
		RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Init Failed, can't handle the request\n");
		return fcInternalError;
	}

	sprintf(stMsgData->paramValue, "%s", getRawValue(stMsgData->paramName).c_str());
	stMsgData->paramLen = strlen(stMsgData->paramValue);

	if(stMsgData->paramLen == 0)
		return fcInternalError;

	RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Leaving %s : Value %s\n", __FUNCTION__, stMsgData->paramValue);
	return fcNoFault;
}

faultCode_t  XRFCStore::setValue(HOSTIF_MsgData_t *stMsgData)
{
	RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
	if(!initDone)
	{
		RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Init Failed, can't handle the request\n");
		return fcInternalError;
	}

    if(!validateParamValue(stMsgData->paramValue, stMsgData->paramtype))
    {
    	RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Invalid Parameter Value : DataType and Value doesn't match\n");
    	return fcInvalidParameterValue;
    }

    const string& currentValue = getRawValue(stMsgData->paramName);
    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Current value :  %s \n", currentValue.c_str());

    if(strlen(currentValue.c_str()) > 0)
    {
    	if(!strcasecmp(currentValue.c_str(), stMsgData->paramValue))
    	{
    		RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Property value exists, don't have to overwrite\n");
    		return fcNoFault;
    	}
    }

    if(setRawValue(stMsgData->paramName, stMsgData->paramValue))
    {
    	RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    	stMsgData->faultCode = fcNoFault;
    	return fcNoFault;
    }
    else
    {
    	stMsgData->faultCode = fcInternalError;
    	return fcInternalError;
    }
}

void XRFCStore::initTR181PropertiesFileName()
{
	RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
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
            }
        }
        ifs_rfc.close();

        if(m_filename.empty())
		{
			RDK_LOG(RDK_LOG_ERROR, LOG_TR69HOSTIF, "Didn't find %s in %s\n", TR181_RFC_STORE_KEY, RFC_PROPERTIES_FILE);
		}
	}
	RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
}

bool XRFCStore::loadTR181PropertiesIntoCache()
{
	RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);
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
        return false;
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
//
//        for (unordered_map<string, string>::iterator it=m_dict.begin(); it!=m_dict.end(); ++it)
//            RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Property Key = %s : Value = %s\n", it->first.c_str(), it->second.c_str());

        ifs_tr181.close();
    }
    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
    return true;
}

XRFCStore::XRFCStore()
{
	RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

	initTR181PropertiesFileName();

	initDone = loadTR181PropertiesIntoCache();

	updateInProgress = false;

    RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
}

XRFCStore* XRFCStore::getInstance()
{
	RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Entering %s \n", __FUNCTION__);

	if(!xrfcInstance)
		xrfcInstance = new XRFCStore;

	RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "Leaving %s \n", __FUNCTION__);
	return xrfcInstance;
}





