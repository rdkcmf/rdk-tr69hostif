
/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/

#include "hostIf_main.h"
#include "hostIf_EstoriaClient_ReqHandler.h"
#include "Estoria.h"

EstoriaReqHandler* EstoriaReqHandler::pInstance = NULL;
updateCallback EstoriaReqHandler::mUpdateCallback = NULL;
msgHandler* EstoriaReqHandler::getInstance()
{
    if(!pInstance)
        pInstance = new EstoriaReqHandler();
    return pInstance;
}

bool EstoriaReqHandler::init()
{
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

bool EstoriaReqHandler::unInit()
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Entering..\n", __FUNCTION__, __FILE__);
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%s] Exiting..\n", __FUNCTION__, __FILE__);
    return true;
}

int EstoriaReqHandler::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;

    if(strncasecmp(stMsgData->paramName,"Device.X_COMCAST_COM_Estoria", strlen("Device.X_COMCAST_COM_Estoria"))==0)
    {
        if (strcasecmp(stMsgData->paramName,"Device.X_COMCAST_COM_Estoria.put") == 0)
        {
            lastCall = stMsgData->paramName;

            Estoria::getLock();
            lastCallResult = Estoria::getInstance()->putJson(stMsgData->paramValue) ? "Success" : "Failed";
            Estoria::releaseLock();

        }
        else if (strcasecmp(stMsgData->paramName,"Device.X_COMCAST_COM_Estoria.get") == 0)
        {
            lastCall = stMsgData->paramName;

            Estoria::getLock();
            lastCallResult = Estoria::getInstance()->getJson(stMsgData->paramValue);
            Estoria::releaseLock();

        }
        else if (strcasecmp(stMsgData->paramName,"Device.X_COMCAST_COM_Estoria.getSigned") == 0)
        {
            lastCall = stMsgData->paramName;

            Estoria::getLock();
            lastCallResult = Estoria::getInstance()->getSignedJson(stMsgData->paramValue);
            Estoria::releaseLock();
        }
    }

    return ret;
}

int EstoriaReqHandler::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;

    if(strncasecmp(stMsgData->paramName,"Device.X_COMCAST_COM_Estoria", strlen("Device.X_COMCAST_COM_Estoria"))==0)
    {
        if (strcasecmp(stMsgData->paramName,"Device.X_COMCAST_COM_Estoria.put") == 0)
        {
            if(lastCall == "Device.X_COMCAST_COM_Estoria.put")
            {
                Estoria::getLock();

                strcpy(stMsgData->paramValue, lastCallResult.c_str());
                stMsgData->paramtype = hostIf_StringType;
                stMsgData->paramLen = strlen(stMsgData->paramValue);

                Estoria::releaseLock();

                ret = OK;
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] %s is not last method called, can't read this value\n", __FILE__, __FUNCTION__, stMsgData->paramName);
                ret = NOK;
            }
        }

        else if (strcasecmp(stMsgData->paramName,"Device.X_COMCAST_COM_Estoria.get") == 0)
        {
            if(lastCall == "Device.X_COMCAST_COM_Estoria.get")
            {
                Estoria::getLock();

                strcpy(stMsgData->paramValue, lastCallResult.c_str());
                stMsgData->paramtype = hostIf_StringType;
                stMsgData->paramLen = strlen(stMsgData->paramValue);

                Estoria::releaseLock();

                ret = OK;
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] %s is not last method called, can't read this value\n", __FILE__, __FUNCTION__, stMsgData->paramName);
                ret = NOK;
            }
        }

        else if (strcasecmp(stMsgData->paramName,"Device.X_COMCAST_COM_Estoria.getSigned") == 0)
        {
            if(lastCall == "Device.X_COMCAST_COM_Estoria.getSigned")
            {
                Estoria::getLock();

                strcpy(stMsgData->paramValue, lastCallResult.c_str());
                stMsgData->paramtype = hostIf_StringType;
                stMsgData->paramLen = strlen(stMsgData->paramValue);

                Estoria::releaseLock();

                ret = OK;
            }
            else
            {
                RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%s] %s is not last method called, can't read this value\n", __FILE__, __FUNCTION__, stMsgData->paramName);
                ret = NOK;
            }
        }
    }
    return ret;
}

void EstoriaReqHandler::registerUpdateCallback(updateCallback cb)
{
    mUpdateCallback = cb;
}

int EstoriaReqHandler::handleGetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    return ret;
}

int EstoriaReqHandler::handleSetAttributesMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    return ret;
}

/** @} */
/** @} */

