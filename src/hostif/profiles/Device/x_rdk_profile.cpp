/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2022 RDK Management
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

/**
 * @file X_rdk_profile.cpp
 * @brief This source file contains the APIs for getting bluetooth device information.
 */

/**
 * @file X_rdk_profile.cpp
 *
 * @brief DeviceInfo X_rdk_profile API Implementation.
 *
 * This is the implementation of the DeviceInfo API.
 *
 * @par Document
 * TBD Relevant design or API documentation.
 *
 */


/*****************************************************************************
 * STANDARD INCLUDE FILES
 *****************************************************************************/


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/

#include "x_rdk_profile.h"
std::mutex X_rdk_profile::m;
XBSStore* X_rdk_profile::m_bsStore;

X_rdk_profile* X_rdk_profile::m_instance = NULL;

X_rdk_profile* X_rdk_profile::getInstance()
{
    if(!m_instance)
    {
        try {
            m_instance = new X_rdk_profile();
        } catch(const std::exception& e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s:%d] X_rdk_profile: Caught exception \" %s\"\n", __FUNCTION__, __LINE__, e.what());
        }
    }
    return m_instance;
}


void X_rdk_profile::closeInstance()
{
    if(m_instance)
    {
        delete m_instance;
    }
}


/**
 * @brief Class default Constructor.
 */
X_rdk_profile::X_rdk_profile()
{
    m_bsStore = XBSStore::getInstance();
}

/**
 * @brief This function set the bluetooth profile attributes
 *
 *
 * @param[in] stMsgData  HostIf Message Request param contains the attribute value.
 *
 * @return Returns an Integer value.
 * @ingroup DEVICE_X_rdk_profile_API
 */
int X_rdk_profile::handleSetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = OK;
    const char* paramName = NULL;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]X_rdk_profile: Entering...  \n", __FUNCTION__, __LINE__);

    try {

        paramName = stMsgData->paramName;

        if(NULL == paramName) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]X_rdk_profile: Failed : Parameter is NULL.\n",  __FUNCTION__, __LINE__);
            return ret;
        }
        if (strncasecmp(paramName, X_RDK_WebPA_DNSText_URL_STRING, strlen(X_RDK_WebPA_DNSText_URL_STRING)) == 0)
        {
            return set_WebPA_DNSText_URL(stMsgData);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]X_rdk_profile: Parameter \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, paramName);
            stMsgData->faultCode = fcInvalidParameterName;
            ret = NOK;
        }
    }
    catch (const std::exception& e )
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]X_rdk_profile: Exception caught %s   \n", __FUNCTION__, __LINE__, e.what());
        stMsgData->faultCode = fcInternalError;
        return NOK;
    }

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]X_rdk_profile: Exiting..\n", __FUNCTION__, __LINE__);

    return ret;
}

/**
 * @brief This function get the bluetooth attributes such as name, profile, count and others
 * for paired and connected devices..
 *
 * @param[in] stMsgData  HostIf Message Request param contains the external SD Card attribute value.
 *
 * @return Returns an Integer value.
 * @retval 0 If successfully get the attribute values.
 * @retval -1 If failed and Not able to get.
 * @retval -2 If Not handle the requested attribute.
 * @ingroup X_rdk_profile_API
 */
int X_rdk_profile::handleGetMsg(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    const char* paramName = NULL;

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]X_rdk_profile: Entering...  \n", __FUNCTION__, __LINE__);

    try {

        paramName = stMsgData->paramName;

        if(NULL == paramName) {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]X_rdk_profile: Failed : Parameter is NULL.\n",  __FUNCTION__, __LINE__);
            return ret;
        }

        if (strncasecmp(paramName, X_RDK_WebPA_SERVER_URL_STPRING, strlen(X_RDK_WebPA_SERVER_URL_STPRING)) == 0)
        {
            ret = get_WebPA_Server_URL(stMsgData);
        }
        else if (strncasecmp(paramName, X_RDK_WebPA_TokenServer_URL_STRING, strlen(X_RDK_WebPA_TokenServer_URL_STRING)) == 0)
        {
            ret = get_WebPA_TokenServer_URL(stMsgData);
        }
        else if (strncasecmp(paramName, X_RDK_WebPA_DNSText_URL_STRING, strlen(X_RDK_WebPA_DNSText_URL_STRING)) == 0)
        {
            ret = get_WebPA_DNSText_URL(stMsgData);
        }
        else
        {
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]X_rdk_profile: Parameter \'%s\' is Not Supported  \n", __FUNCTION__, __LINE__, paramName);
            stMsgData->faultCode = fcInvalidParameterName;
            ret = NOK;
        }
    }
    catch (const std::exception& e )
    {
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d]X_rdk_profile: Exception caught %s   \n", __FUNCTION__, __LINE__, e.what());
        stMsgData->faultCode = fcInternalError;
        return NOK;
    }
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]X_rdk_profile: Exiting..\n", __FUNCTION__, __LINE__);
    return ret;
}



/************************************************************
 * Description  : Get the 'Device.X_RDK_WebPA_Server.URL' status.
 * 				 'true(1)' if enabled or 'false(0)'
 * Precondition : partner_default.json should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> 0: false
                        -> 1: true
************************************************************/
int X_rdk_profile::get_WebPA_Server_URL(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]Entering..\n", __FUNCTION__, __LINE__);

    try {
        m_bsStore->getValue(stMsgData);
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s:%d] Exception : %s\r\n",__FUNCTION__, __LINE__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_StringType;
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] The param name [ %s ], and value [ %s ] \r\n",__FUNCTION__, __LINE__,
            stMsgData->paramName,stMsgData->paramValue );
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d] Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}


/************************************************************
 * Description  : Get the 'Device.X_RDK_WebPA_TokenServer.URL' status.
 * 				 'true(1)' if enabled or 'false(0)'
 * Precondition : partner_default.json should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> 0: false
                        -> 1: true
************************************************************/
int X_rdk_profile::get_WebPA_TokenServer_URL(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]Entering..\n", __FUNCTION__, __LINE__);

    try {
        m_bsStore->getValue(stMsgData);
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s:%d]Exception : %s\r\n",__FUNCTION__, __LINE__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_StringType;
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] The param name [ %s ], and value [ %s ] \r\n",__FUNCTION__, __LINE__,
            stMsgData->paramName,stMsgData->paramValue );

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d] Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}

/************************************************************
 * Description  : Get the 'Device.X_RDK_WebPA_DNSText.URL' status.
 * 				 'true(1)' if enabled or 'false(0)'
 * Precondition : partner_default.json should be present.
 * Input        :

 * Return       : OK -> Success
                  NOK -> Failure
                  value -> 0: false
                        -> 1: true
************************************************************/
int X_rdk_profile::get_WebPA_DNSText_URL(HOSTIF_MsgData_t *stMsgData)
{
    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d]Entering..\n", __FUNCTION__, __LINE__);

    try {
        m_bsStore->getValue(stMsgData);
    } catch (const std::exception& e) {
        RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"[%s:%d] Exception : %s\r\n",__FUNCTION__, __LINE__, e.what());
        return NOK;
    }
    stMsgData->paramtype=hostIf_StringType;
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] The param name [ %s ], and value [ %s ] \r\n",__FUNCTION__, __LINE__,
            stMsgData->paramName,stMsgData->paramValue );

    RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s:%d] Exiting..\n", __FUNCTION__, __LINE__);
    return OK;
}


int X_rdk_profile::set_WebPA_DNSText_URL(HOSTIF_MsgData_t * stMsgData)
{
    m_bsStore->overrideValue(stMsgData);
    RDK_LOG(RDK_LOG_INFO,LOG_TR69HOSTIF,"[%s:%d] The [ %s ] Set Successfully value as [ %s ]. \n",__FUNCTION__, __LINE__,
            stMsgData->paramName, stMsgData->paramValue);
    return OK;
}


/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */


/** @} */
/** @} */
