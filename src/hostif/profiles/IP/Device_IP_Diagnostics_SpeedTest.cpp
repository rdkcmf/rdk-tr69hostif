/*
 * If not stated otherwise in this file or this component's LICENSE file the
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
 *
 * author: Derric_Lynns@cable.comcast.com
 *
*/


/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/

#include "Device_IP_Diagnostics_SpeedTest.h"
#include "hostIf_utils.h"
#include <string>
#include <fstream>

// the below directory holds the database for TR objects
#define TR69DIR "/opt/tr-181/"
#define STORE_EXTENSION "sdb"

// the diagnostics directory is used only in developer builds
#define DIAGNOSTICS_DIR "/opt/diagnostics/"
#define PROD_DIR "/usr/bin/"

// script name
#define SPEEDTEST_BIN "speedtest.sh"
#define NON_BLOCKING " &"

// profile name
const char* hostIf_IP_Diagnostics_SpeedTest::SpeedTestProfile = "Device.IP.Diagnostics.X_RDKCENTRAL-COM_SpeedTest.";

/**
 * @brief The constructor checks for the database file at TR69DIR and loads the database
 * It checks for the database file and if doesnt exist, it creates one
 *
 */
hostIf_IP_Diagnostics_SpeedTest::hostIf_IP_Diagnostics_SpeedTest ()
{
    std::string filename;
    filename.append (TR69DIR).append(SpeedTestProfile).append (STORE_EXTENSION);

    bCalledEnable = false;
    bCalledRun = false;

    // load sdb data file
    if (!dbStore.load (filename))
    {
        // creates a new file if it doesn't exist
        std::ofstream output_stream (filename.c_str());
    }
    else
    {
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] dbStore for SpeedTest is already present\n", __FUNCTION__);
    }
}

/**
 * @brief This function provides single instance of hostIF_IP_Diagnostics_SpeedTest
 *
 */
hostIf_IP_Diagnostics_SpeedTest& hostIf_IP_Diagnostics_SpeedTest::getInstance ()
{
    // singleton implementation
    static hostIf_IP_Diagnostics_SpeedTest instance;
    return instance;
}

/**
 * @brief This function is called when profile parameter needs to be read
 * It gets the values of Enable_Speedtest, Run, Argument, ClientType, Authentication ans Status
 * If any other parameter is requested the fault code is set to invalid paramtername
 *
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device else NOK
 *
 */
int hostIf_IP_Diagnostics_SpeedTest::handleGetMsg (HOSTIF_MsgData_t* stMsgData)
{
    LOG_ENTRY_EXIT;

    int ret = NOK;
    std::string spdtst_parameter_name (stMsgData->paramName + strlen (SpeedTestProfile));
    // the order of if condition is step by step and must not be changed
    if (spdtst_parameter_name.compare("Enable_Speedtest") &&
            spdtst_parameter_name.compare ("Run") &&
            spdtst_parameter_name.compare ("Argument") &&
            spdtst_parameter_name.compare ("ClientType") &&
            spdtst_parameter_name.compare ("Authentication") &&
            spdtst_parameter_name.compare ("Status"))
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Unsupported SpeedTest parameter '%s'\n", __FUNCTION__, stMsgData->paramName);
        stMsgData->faultCode = fcInvalidParameterName;
    }
    else
    {
        RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] SpeedTest parameter '%s' is being set\n", __FUNCTION__, stMsgData->paramName);
        std::string value = dbStore.value (stMsgData->paramName, "");
        putValue(stMsgData, value);
        stMsgData->faultCode = fcNoFault;
        ret = OK;
    }

    return ret;
}

/**
 * @brief This function sets the profile parameter data
 * It sets the values of Enable_Speedtest, Run, Argument, ClientType, Authentication ans Status
 * If any other parameter is requested the fault code is set to invalid paramtername
 *
 * @return Returns the status of the operation.
 *
 * @retval OK if it is successfully fetch the data from the device, else it returns NOK
 *
 */
int hostIf_IP_Diagnostics_SpeedTest::handleSetMsg (HOSTIF_MsgData_t* stMsgData)
{
    LOG_ENTRY_EXIT;

    int ret = NOK;
    std::string spdtst_parameter_name (stMsgData->paramName + strlen (SpeedTestProfile));
    // the order of if condition is step by step and must not be changed
    if (spdtst_parameter_name.compare("Enable_Speedtest") &&
            spdtst_parameter_name.compare ("Run") &&
            spdtst_parameter_name.compare ("Argument") &&
            spdtst_parameter_name.compare ("ClientType") &&
            spdtst_parameter_name.compare ("Authentication") &&
            spdtst_parameter_name.compare ("Status"))
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] Unsupported SpeedTest parameter '%s'\n", __FUNCTION__, stMsgData->paramName);
        stMsgData->faultCode = fcInvalidParameterName;
    }
    else if (false == dbStore.setValue (stMsgData->paramName, getStringValue(stMsgData)))
    {
        RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] SpeedTest setValue failed '%s'\n", __FUNCTION__, stMsgData->paramName);
        stMsgData->faultCode = fcInternalError;
    }
    else
    {
        ret = OK;
        stMsgData->faultCode = fcNoFault;
        processMsg(stMsgData);

        if( stMsgData->faultCode == fcInternalError )
        {
            ret = NOK;
            RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s] SpeedTest setValue failed '%s'\n", __FUNCTION__, stMsgData->paramName);
        }
        else
        {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] SpeedTest setValue succeeded '%s'\n", __FUNCTION__, stMsgData->paramName);
        }
    }

    return ret;
}

/**
 * @brief This function processes the set message for triggering speedtest-client
 * It checks for Enable_Speedtest and Run profile parameters and sets to launch
 * internet measurement platform application from the respective location.
 * the failure of the function is handled in handleSetMsg function when it meets fcInternalError
 *
 */
void hostIf_IP_Diagnostics_SpeedTest::processMsg(HOSTIF_MsgData_t* stMsgData)
{
    std::string spdtst_parameter_name (stMsgData->paramName + strlen (SpeedTestProfile));

    if (spdtst_parameter_name.compare ("Enable_Speedtest") == 0)
    {
        bCalledEnable = get_boolean(stMsgData->paramValue);
    }

    if (spdtst_parameter_name.compare ("Run") == 0)
    {
        bCalledRun = get_boolean(stMsgData->paramValue);

        if (bCalledEnable && bCalledRun)
        {
            RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Speedtest application is being launched '%s'\n", __FUNCTION__, stMsgData->paramName);
            std::string application_diagnostics;
            std::string application_prod;
            std::string application;
            // read only
            ifstream launch_script;

            // the below variables depends on macro, hence do not try optimizing with one variable as compiler could optimize out
            application_prod.append (PROD_DIR).append(SPEEDTEST_BIN);
            application_diagnostics.append (DIAGNOSTICS_DIR).append(SPEEDTEST_BIN);

            // initialize before the SPEEDTEST_DIAGNOSTICS macro check
            application = application_prod;

#ifdef SPEEDTEST_DIAGNOSTICS
            application = application_diagnostics;
#endif

            // check and launch
            launch_script.open(application);
            if (launch_script.fail())
            {
                application = application_prod;
                launch_script.open(application);
                if (launch_script.fail())
                {
                    RDK_LOG (RDK_LOG_INFO, LOG_TR69HOSTIF, "[%s] Speedtest launch script is not found anywhere\n", __FUNCTION__);
                }
            }

            application.append(NON_BLOCKING);
            system(application.c_str());

            // the condition here is for parameter Run and its being reset
            if( false == dbStore.setValue (stMsgData->paramName, "false"))
            {
                // cannot reset then handle return as NOK
                stMsgData->faultCode = fcInternalError;
            }

            bCalledRun = false;
        }
    }

}

/** @} */
/** @} */
