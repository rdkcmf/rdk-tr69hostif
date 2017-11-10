#ifndef XRDKCENTRALCOMRFC_H
#define XRDKCENTRALCOMRFC_H

#include "hostIf_tr69ReqHandler.h"

#include <string>
#include <map>

using namespace std;

class IniFile
{
public:
    IniFile();
    bool                load(const string &filename);
    string              value(const string &key, const string &defaultValue = "") const;
    bool                setValue(const string &key, const string &value);

private:
    string m_filename;
    map<string, string> m_dict;
};

class XRFCStorage
{
public:
    XRFCStorage();
    int  getValue(HOSTIF_MsgData_t *stMsgData);
    int  setValue(HOSTIF_MsgData_t *stMsgData);

    string getRawValue(const string &key);
    bool setRawValue(const string &key, const string &value);

private:
    bool                init();
    IniFile             m_storage;
    string              m_storageFile;
    bool                m_storageLoaded;
};

#endif // XRDKCENTRALCOMRFC_H
