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

#ifndef INIFILE_H_
#define INIFILE_H_

#include <string>
#include <map>

class IniFile
{
public:
    IniFile();
    bool                load(const std::string &filename);
    bool                clear();
    std::string         value(const std::string &key, const std::string &defaultValue = "") const;
    bool                setValue(const std::string &key, const std::string &value);
    bool                flush();

private:
    std::string m_filename;
    std::map<std::string, std::string> m_dict;
};

#endif /* INIFILE_H_ */
