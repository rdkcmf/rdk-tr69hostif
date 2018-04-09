#ifndef HOSTIF_ESTORIACLIENT_REQHANDLER_H_
#define HOSTIF_ESTORIACLIENT_REQHANDLER_H_

#include "hostIf_msgHandler.h"
#include "hostIf_updateHandler.h"

class EstoriaReqHandler : public msgHandler
{
    EstoriaReqHandler() {};
    ~EstoriaReqHandler() {};
    static class EstoriaReqHandler *pInstance;
    static updateCallback mUpdateCallback;
    std::string lastCall;
    std::string lastCallResult;

public:
    virtual bool init();
    virtual bool unInit();
    virtual int handleSetMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleGetMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData);
    virtual int handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData);
    static msgHandler* getInstance();
    static void registerUpdateCallback(updateCallback cb);
};


#endif /* HOSTIF_ESTORIACLIENT_REQHANDLER_H_ */

