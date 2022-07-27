/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2017 RDK Management
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
 * @file snmpAdapter.cpp
 * @brief This source file contains the APIs for getting device information.
 */

/**
 * @file snmpAdapter.cpp
 *
 * @brief SNMP RDKCENTRAL API Implementation.
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

#include "snmpAdapter.h"
#include <fstream>
#include "safec_lib.h"
#include <vector>
#include <cstring>

#ifdef YOCTO_BUILD
#include "secure_wrapper.h"
#endif

#define TR181_SNMPOID_FILE              "/etc/tr181_snmpOID.conf"
#define interface_STB                   "STB"
#define interface_CM                    "CM"
#define SNMP_AGENT_CM_IP_ADDRESS        "192.168.100.1" 
#define SNMP_AGENT_STB_IP_ADDRESS       "127.0.0.1"
#define SNMP_COMMUNITY                  "hDaFHJG7"

GHashTable* hostIf_snmpAdapter::ifHash = NULL;
GHashTable* hostIf_snmpAdapter::m_notifyHash = NULL;
GMutex* hostIf_snmpAdapter::m_mutex = NULL;
map<string, vector<pair <string, string>>> hostIf_snmpAdapter::tr181Map;
/****************************************************************************************************************************************************/
// Device.X_RDKCENTRAL Profile. Getters:
/****************************************************************************************************************************************************/

/**
 * @brief Class Constructor of the class hostIf_snmpAdapter.
 *
 */
hostIf_snmpAdapter::hostIf_snmpAdapter(int dev_id):
    dev_id(dev_id)
{

}

/**
 * @brief Class Destructor of the class hostIf_snmpAdapter.
 *
 */
hostIf_snmpAdapter::~hostIf_snmpAdapter()
{
    if(m_notifyHash)
    {
        g_hash_table_destroy(m_notifyHash);
    }
}

/**
 * @brief This function opens the RF_DocsIf_tr181_snmp map file, 
 * parse the TR181 parameter and its correspoinding OID, fill it in map container.
 * 
 */
void hostIf_snmpAdapter::init(void)
{
    string line;
    ifstream fileStream (TR181_SNMPOID_FILE);
    char delimeter[] = " \t\n\r\f\v";

    tr181Map.clear();
    if (fileStream.is_open())
    {
        while(getline(fileStream, line))
        {
            int pos = line.find('=');
            if(pos != string::npos)
            {
                string OID_value,interface_value;
                string key = line.substr(0, pos);
                int result = line.find(interface_STB);
                if(result>0)
                {
                    interface_value = interface_STB;
                    line.erase(line.find(interface_value));
                    OID_value = line.substr(pos+1);
                }
                else
                {
                    interface_value = interface_CM;
                    line.erase(line.find(interface_value));
                    OID_value = line.substr(pos+1);
                }
                key.erase(0, key.find_first_not_of(delimeter));
                key.erase(key.find_last_not_of(delimeter) + 1);

                OID_value.erase(0, OID_value.find_first_not_of(delimeter));
                OID_value.erase(OID_value.find_last_not_of(delimeter) + 1);

                tr181Map.insert( { key, {{ OID_value, interface_value }} } );
            }
            
        }
    }
    else
        RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] Error opening %s fileStream.", __FUNCTION__, __LINE__, TR181_SNMPOID_FILE );
}

/**
 * @brief This function clear the TR181 OID map container.
 * 
 */
void hostIf_snmpAdapter::unInit(void)
{    
    tr181Map.clear();
}

hostIf_snmpAdapter* hostIf_snmpAdapter::getInstance(int dev_id)
{
    hostIf_snmpAdapter* pRet = NULL;

    if(ifHash)
        pRet = (hostIf_snmpAdapter *)g_hash_table_lookup(ifHash,(gpointer) dev_id);
    else
        ifHash = g_hash_table_new(NULL,NULL);

    if(!pRet)
    {
        try {
            pRet = new hostIf_snmpAdapter(dev_id);
        } catch(int e)
        {
            RDK_LOG(RDK_LOG_WARN,LOG_TR69HOSTIF,"Caught exception, not able create SNMP Device RDK Central instance..\n");
        }
        g_hash_table_insert(ifHash, (gpointer)dev_id, pRet);
    }
    return pRet;
}

GList* hostIf_snmpAdapter::getAllInstances()
{
    if(ifHash)
        return g_hash_table_get_keys(ifHash);
    return NULL;
}

void hostIf_snmpAdapter::closeInstance(hostIf_snmpAdapter *pDev)
{
    if(pDev)
    {
        g_hash_table_remove(ifHash, (gconstpointer)pDev->dev_id);
        delete pDev;
    }
}

void hostIf_snmpAdapter::closeAllInstances()
{
    if(ifHash)
    {
        GList* tmp_list = g_hash_table_get_values (ifHash);

        while(tmp_list)
        {
            hostIf_snmpAdapter* pDev = (hostIf_snmpAdapter *)tmp_list->data;
            tmp_list = tmp_list->next;
            closeInstance(pDev);
        }
    }
}

void hostIf_snmpAdapter::getLock()
{
    if(!m_mutex)
    {
        m_mutex = g_mutex_new();
    }
    g_mutex_lock(m_mutex);
}

void hostIf_snmpAdapter::releaseLock()
{
    g_mutex_unlock(m_mutex);
}

GHashTable*  hostIf_snmpAdapter::getNotifyHash()
{
    if(m_notifyHash)
        return m_notifyHash;
    else
        return m_notifyHash = g_hash_table_new(g_str_hash, g_str_equal);
}

/**
 * @brief This function fetch the SNMP OID for the corresponding TR181 param,
 * and run the snmpget command with the OID. The result will be return back as string.
 * 
 */
int hostIf_snmpAdapter::get_ValueFromSNMPAdapter(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    char cmd[BUFF_LENGTH_256] = { 0 };
    char resultBuff[BUFF_LENGTH_256] = { 0 };
    char delimeter[] = " \t\n\r\f\v";
    map<string, vector<pair <string, string>>>::iterator it;
    string consoleString("");
    errno_t rc = -1;
  
    if(stMsgData)
    {
        it = tr181Map.find(stMsgData->paramName);
        if (it != tr181Map.end())
        {
            string value = it->second[0].second;
            if (value.compare(interface_STB) == 0)
              {
               snprintf (cmd, BUFF_LENGTH_256, "snmpget -OQ -Ir -v 2c -c %s %s %s",
                SNMP_COMMUNITY,
                SNMP_AGENT_STB_IP_ADDRESS,
                it->second[0].first.c_str());
              }
            else
            {
               snprintf (cmd, BUFF_LENGTH_256, "snmpget -OQ -Ir -v 2c -c %s %s %s",
                SNMP_COMMUNITY,
                SNMP_AGENT_CM_IP_ADDRESS,
                it->second[0].first.c_str());
            }

            RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] %s\n", __FUNCTION__, cmd);
            ret = GetStdoutFromCommand( cmd, consoleString);
            if (ret == OK)
            {
                int pos = consoleString.find('=');
                if(pos != string::npos)
                {
                    string subStr = consoleString.substr(pos + 1);
                    subStr.erase(0, subStr.find_first_not_of(delimeter));
                    subStr.erase(subStr.find_last_not_of(delimeter) + 1);
                    rc=strcpy_s(stMsgData->paramValue,sizeof(stMsgData->paramValue), subStr.c_str());
                    if(rc!=EOK)
		    {
			ERR_CHK(rc);
		    }
		}
                else
		{
                    rc=strcpy_s(stMsgData->paramValue,sizeof(stMsgData->paramValue), resultBuff);
		    if(rc!=EOK)
                    {
                        ERR_CHK(rc);
                    }
		}
                stMsgData->paramtype = hostIf_StringType;
                RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] %s %s\n", __FUNCTION__, stMsgData->paramName, stMsgData->paramValue);
                ret = OK;
            }
        }
        else
        {
            ret = NOK;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] %s NOT found in the map.\n", __FUNCTION__, __LINE__, stMsgData->paramName );
        }
    }
    return ret;
}

/**
 * @brief This function fetch the SNMP OID for the corresponding TR181 param,
 * and run the snmpset command with the OID.
 * 
 */
int hostIf_snmpAdapter::set_ValueToSNMPAdapter(HOSTIF_MsgData_t *stMsgData)
{
    int ret = NOT_HANDLED;
    char cmd[BUFF_LENGTH_256] = { 0 };
    char resultBuff[BUFF_LENGTH_256] = { 0 };
    map<string, vector<pair <string, string>>>::iterator it;

#ifdef YOCTO_BUILD
    FILE *fp;
#define CMD(cmd, length, args...) ({ snprintf(cmd, length, args); fp = (v_secure_popen("r", args); )})
#else
#define CMD(cmd, length, args...) ({ snprintf(cmd, length, args); })
#endif

    if(stMsgData)
    {
        it = tr181Map.find(stMsgData->paramName);
        if (it != tr181Map.end())
        {
            string value = it->second[0].second;
            switch(stMsgData->paramtype)
            {
                case hostIf_StringType:
                    if (value.compare(interface_STB) == 0){
                       CMD(cmd, BUFF_LENGTH_256, "snmpset -v 2c -c %s %s %s s %s", 
                        SNMP_COMMUNITY, SNMP_AGENT_STB_IP_ADDRESS, 
                        it->second[0].first.c_str(),
                        stMsgData->paramValue);
                        }
                    else{
                        CMD(cmd, BUFF_LENGTH_256, "snmpset -v 2c -c %s %s %s s %s", 
                        SNMP_COMMUNITY, SNMP_AGENT_CM_IP_ADDRESS, 
                        it->second[0].first.c_str(),
                        stMsgData->paramValue);
                        }
                    break;

                case hostIf_IntegerType:
                    if (value.compare(interface_STB) == 0)
                    {
                        CMD(cmd, BUFF_LENGTH_256, "snmpset -v 2c -c %s %s %s i %d",
                        SNMP_COMMUNITY, SNMP_AGENT_STB_IP_ADDRESS,
                        it->second[0].first.c_str(),
                        stMsgData->paramValue);
                    }
                   else{
                        CMD(cmd, BUFF_LENGTH_256, "snmpset -v 2c -c %s %s %s i %d",
                        SNMP_COMMUNITY, SNMP_AGENT_CM_IP_ADDRESS,
                        it->second[0].first.c_str(),
                        stMsgData->paramValue);
                        }
                    break;

                case hostIf_UnsignedIntType:
                    if (value.compare(interface_STB) == 0)
                    {
                        CMD(cmd, BUFF_LENGTH_256, "snmpset -v 2c -c %s %s %s u %d",
                        SNMP_COMMUNITY,
                        SNMP_AGENT_STB_IP_ADDRESS,
                        it->second[0].first.c_str(),
                        stMsgData->paramValue);
                    }
                   else{
                        CMD(cmd, BUFF_LENGTH_256, "snmpset -v 2c -c %s %s %s u %d",
                        SNMP_COMMUNITY,
                        SNMP_AGENT_CM_IP_ADDRESS,
                        it->second[0].first.c_str(),
                        stMsgData->paramValue);
                       }
                    break;

                case hostIf_BooleanType:
                case hostIf_DateTimeType:
                case hostIf_UnsignedLongType:
                default:
                    RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] %s not supported type %d\n", __FUNCTION__, __LINE__, stMsgData->paramName, stMsgData->paramtype);
                    return NOK;
            }

            RDK_LOG(RDK_LOG_TRACE1,LOG_TR69HOSTIF,"[%s] %s\n", __FUNCTION__, cmd);
#ifdef YOCTO_BUILD
	if (fp == NULL) {
		RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s]: cannot run command [%s]\n", __FUNCTION__, cmd);
		ret = NOK;
	} else if (fgets (resultBuff, BUFF_LENGTH_256, fp) == NULL) {
		RDK_LOG (RDK_LOG_ERROR, LOG_TR69HOSTIF, "[%s]: cannot read output from command [%s]\n", __FUNCTION__, cmd);
		v_secure_pclose (fp);
		ret = NOK;
	} else {
		ret = v_secure_pclose(fp);
	}

	RDK_LOG (RDK_LOG_DEBUG, LOG_TR69HOSTIF, "[%s]: command [%s] returned [%s]\n", __FUNCTION__, cmd, resultBuff);
#else
            ret = read_command_output (cmd, resultBuff, BUFF_LENGTH_256);
#endif
            stMsgData->faultCode = (OK == ret)?fcNoFault:fcRequestDenied;
        }
        else
        {
            ret = NOK;
            RDK_LOG(RDK_LOG_ERROR,LOG_TR69HOSTIF,"[%s:%d] %s NOT found in the map.\n", __FUNCTION__,__LINE__, stMsgData->paramName );
        }
    }
#undef CMD
    return ret;
}

/* End of doxygen group */
/**
 * @}
 */

/* End of file xxx_api.c. */


/** @} */
/** @} */
