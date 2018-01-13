#ifndef XRDKCENTRALCOMRFC_H
#define XRDKCENTRALCOMRFC_H

#include "hostIf_tr69ReqHandler.h"
#include "IniFile.h"
#include <string>

using namespace std;

class XRFCStorage
{
public:
    XRFCStorage();
    void clearAll();
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
