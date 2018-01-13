/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
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

#include "IniFile.h"
#include "hostIf_main.h"
#include <fstream>
#include <algorithm>

using namespace std;

IniFile::IniFile()
{
}

bool IniFile::load(const string &filename)
{
    m_filename = filename;

    // get rid of quotes, it is quite common with properties files
    m_filename.erase( remove(m_filename.begin(), m_filename.end(), '\"' ), m_filename.end() );

    m_dict.clear();

    ifstream inputStream(m_filename.c_str());

    if (!inputStream.is_open()) {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Trying to open a non-existent file [%s] \n", __FUNCTION__, m_filename.c_str());
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
   ofstream outputStream(m_filename.c_str());
   if (!outputStream.is_open()) {
       RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "%s: Will not write, open on file [%s] failed \n", __FUNCTION__, m_filename.c_str());
       return false;
   }

   map<string,string>::const_iterator it = m_dict.begin();
   do {
       outputStream << it->first << '=' << it->second << endl;
       it++;
   }  while (it != m_dict.end());

   return true;
}
