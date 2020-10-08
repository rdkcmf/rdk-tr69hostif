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

#include <iostream>
#include <string>
#include <mutex>
#include "waldb.h"
#include "tinyxml2.h"
#include "stdlib.h"
#include "libIBus.h"
#include "webpa_parameter.h"
#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"

using namespace tinyxml2;
#define MAX_PARAMETER_LENGTH 512
#define MAX_DATATYPE_LENGTH 48
#define MAX_NUM_PARAMETERS 2048
#define INSTANCE_NUMBER_INDICATOR "{i}."
#define INSTANCE_CHAR_INDICATOR ".{i}."
#define MAX_PARAM_LENGTH TR69HOSTIFMGR_MAX_PARAM_LEN
#define BLUETOOTH_DISCOVERED_DEVICE_COUNT_PARAM  "Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDeviceCnt"
#define BLUETOOTH_PAIRED_DEVICE_COUNT_PARAM  "Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDeviceCnt"
#define BLUETOOTH_CONNECTED_DEVICE_COUNT_PARAM   "Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDeviceCnt"

/**
 * @briefGHashTable For holding Parameter count query strings
 */
GHashTable  *instanceNumParamHash = NULL;


//static void getList(TiXmlNode *pParent,char *paramName,char **ptrParamList,char **pParamDataTypeList,int *paramCount);
static XMLNode* getList(XMLNode *pParent,char *paramName,char* currentParam,char **ptrParamList,char **pParamDataTypeList,int *paramCount);
static void removeUnnecessaryAttributes(XMLDocument *pParent);
void checkforParameterMatch(XMLNode *pParent,char *paramName,int *pMatch,DataModelParam *dmParam);
void appendNextObject(char* currentParam, const char* pAttparam);
int isParamEndsWithInstance(const char* paramName);
void replaceWithInstanceNumber(char *paramName, int instanceNumber);
int checkMatchingParameter(const char* attrValue, char* paramName, int* ret);
void appendNextObject(char* currentParam, const char* pAttparam);
int getNumberofInstances(const char* paramName);


#define WEBPA_DATA_MODEL_FILE "/etc/data-model.xml"
static void *g_dbhandle = NULL;
std::mutex g_db_mutex;

/* @brief Loads the data-model xml data
 *
 * @filename[in] data-model xml filename (with absolute path)
 * @dbhandle[out] database handle
 * @return DB_STATUS
 */
DB_STATUS loadDataModel(void)
{
    DB_STATUS dbRet = DB_FAILURE;
    XMLDocument *doc = NULL;

    if(g_dbhandle)
        return DB_SUCCESS;

    const std::lock_guard<std::mutex> lock(g_db_mutex);

    // Load Document model
    doc = new XMLDocument();
    if(doc != NULL)
    {
        doc->LoadFile(WEBPA_DATA_MODEL_FILE);
        if( doc->ErrorID() == 0 )
        {
            removeUnnecessaryAttributes(doc);
            g_dbhandle = (void *)doc;
            // Initialize Number of entity param list
            initNumEntityParamList();
            return DB_SUCCESS;
        }
        else
        {
            return DB_FAILURE;
        }
    }
    return DB_FAILURE;
}
static void removeUnnecessaryAttributes(XMLDocument *pParent)
{
    XMLNode* pChild;
    if( pParent->ToElement() == NULL )
    {
        for ( pChild = pParent->FirstChild(); pChild != 0; )
        {
            if( pChild->ToElement() == NULL )
            {
                pChild = pChild->NextSibling();
            }
            else
            {
                if( !strcmp (pChild->Value(), "object") )
                    break;
                pChild = pChild->FirstChild();
            }
        }
    }
    else
    {
        pChild = pParent;
    }
    while(pChild)
    {
        XMLElement* pElement =  pChild->ToElement();
        const XMLAttribute* pAttrib = pElement->FirstAttribute();
        if(!strcmp(pChild->Value(),"object"))
        {
            pElement->DeleteAttribute("minEntries");
            pElement->DeleteAttribute("addObjIdx");
            pElement->DeleteAttribute("delObjIdx");
            XMLNode* param = pChild->FirstChild();
            if(param != NULL && !strcmp(param->Value(),"parameter"))
            {
                while( param )
                {
                    param->ToElement()->DeleteAttribute("notification");
                    param->ToElement()->DeleteAttribute("maxNotification");
                    param->ToElement()->DeleteAttribute("rebootIdx");
                    param->ToElement()->DeleteAttribute("initIdx");
                    param->ToElement()->DeleteAttribute("setIdx");
                    param=param->NextSibling();
                }
            }
        }
        pChild = pChild->NextSibling();
    }
}

void* getDataModelHandle()
{
    return g_dbhandle;
}

DB_STATUS checkDataModelStatus()
{
    return (g_dbhandle != NULL)?DB_SUCCESS:DB_FAILURE;
}

/* @brief Loads the Parameter count query string to memory
 *
 * @return void
 */
void initNumEntityParamList()
{
    instanceNumParamHash = g_hash_table_new(g_str_hash, g_str_equal);
    if(NULL != instanceNumParamHash)
    {
        g_hash_table_insert(instanceNumParamHash, (gpointer)"Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice", (gpointer)BLUETOOTH_DISCOVERED_DEVICE_COUNT_PARAM);
        g_hash_table_insert(instanceNumParamHash,(gpointer) "Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice",(gpointer) BLUETOOTH_PAIRED_DEVICE_COUNT_PARAM);
        g_hash_table_insert(instanceNumParamHash, (gpointer)"Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice", (gpointer)BLUETOOTH_CONNECTED_DEVICE_COUNT_PARAM);
    }
}

/**
 * @brief Get the number of Instances of particular parameter ending with {i}
 *
 * @param[in] paramName Name of the Parameter ending with {i}
 * @param[out] Number of instances present for particular Parameter
 */
int getNumberofInstances(const char* paramName)
{
    int instanceCount = 0;
    if(NULL != paramName)
    {
        char *position = NULL;
        char numberOfEntitiesParam[MAX_PARAMETER_LENGTH] = "\0";
        char parameter[MAX_PARAMETER_LENGTH] = "\0";
        strncpy(numberOfEntitiesParam,paramName,sizeof(numberOfEntitiesParam) -1);
        numberOfEntitiesParam[sizeof(numberOfEntitiesParam) -1] = '\0';
        if(NULL != (position = strstr(numberOfEntitiesParam, INSTANCE_NUMBER_INDICATOR)))
        {
            // Check if Parameter is present in numParam Hash List
            strncpy(parameter,numberOfEntitiesParam,strlen(numberOfEntitiesParam)-5);
            const char* paramValue = (const char *) g_hash_table_lookup(instanceNumParamHash,parameter);
            if(NULL != paramValue)
            {
                strcpy(numberOfEntitiesParam,paramValue);
            }
            else // Normal Parameters Number of Entity param = ParamName + "NumberOfEntries"
            {
                strcpy(position-1,"NumberOfEntries");
            }
            // Get the number of instances using getValues
            WDMP_STATUS *ret ;
            size_t *retCount = 0;
            char *getParamList[1];
            getParamList[0] = (char*) calloc(1,MAX_PARAMETER_LENGTH);
            strncpy(getParamList[0],numberOfEntitiesParam,MAX_PARAMETER_LENGTH);
            param_t **parametervalArr = (param_t **) malloc(sizeof(param_t**));
            *parametervalArr = NULL;
            ret = (WDMP_STATUS *) malloc(sizeof(WDMP_STATUS *)*1);
            retCount = (size_t *) malloc(sizeof(size_t) * 1);
            getValues(const_cast<const char**>(getParamList), 1, &parametervalArr, &retCount,&ret);
            if( *retCount > 0 && (NULL != parametervalArr) && NULL != (*parametervalArr)[0].value )
            {
                instanceCount = strtol(const_cast<const char*>((*parametervalArr)[0].value),NULL,10);
            }
            // Lets free all allocated values
            if(getParamList[0])
                free(getParamList[0]);
            if(ret)
                free(ret);
            if(retCount)
                free(retCount);
            if(NULL != parametervalArr && NULL != *parametervalArr)
            {
                if((*parametervalArr)[0].value)
                    free((*parametervalArr)[0].value);
                if((*parametervalArr)[0].name)
                    free((*parametervalArr)[0].name);
                free(*parametervalArr);
            }
            if(NULL != parametervalArr) {
                free(parametervalArr);
            }
        }
    }
    return instanceCount;
}
/**
 * @brief Check if Parameter Name ends with {i}.
 *
 * @param[in] paramName Name of the Parameter.
 * @param[out] retValue 0 if present and 1 if not
 */
int isParamEndsWithInstance(const char* paramName)
{
    int retValue = 1;
    if(NULL != paramName)
    {
        retValue = strcmp(paramName+strlen(paramName)-4,INSTANCE_NUMBER_INDICATOR);
    }
    return retValue;
}

/**
 * @brief Check if Parameter Name ends with . If yes it is a wild card param
 *
 * @param[in] paramName Name of the Parameter.
 * @param[out] retValue 0 if present and 1 if not
 */
int isWildCardParam(char *paramName)
{
    int isWildCard = 0;
    if(NULL != paramName)
    {
        if(!strcmp(paramName+strlen(paramName)-1,"."))
        {
            isWildCard = 1;
        }
    }
    return isWildCard;
}
/**
 * @brief If Parameter Name ends with {i}. replace it with instance number eg:- a.{i} to a.1
 *
 * @param[in] paramName Name of the Parameter.
 * @param[in] instanceNumber , The number which we need to replace
 */
void replaceWithInstanceNumber(char *paramName, int instanceNumber)
{
    char *position;
    char number[10];

    if(!(position = strstr(paramName, INSTANCE_NUMBER_INDICATOR)))
        return;
    sprintf(number,"%d.",instanceNumber);
    strcpy(paramName+(strlen(paramName)-4),number);
}

/**
 * @brief Check if the parameter name and current attribute value is matching. This will consider with instance number.
 * ie "a.b.c.{i}." is matching to "a.b.c.1."
 *
 * @param[in] attrValue Current value of the Object with {i} from xml file
 * @param[in] paramName , Current parameter name from wild card.
 * @param[in] ret , This will be filled with Instance number if attrValue ends with {i}
 * @param[out] Status , returns 1 if strings matches and 0 if not
 */
int checkMatchingParameter(const char* attrValue, char* paramName, int* ret)
{
    int i=10;
    int inst = 0;
    int status = 0;
    while(true)
    {
        if(!(*attrValue && *paramName && (*attrValue == *paramName)))
        {
            *ret =0;
            if(*attrValue == '{' && *paramName >= 48 && *paramName<=56)
            {
                attrValue += 3;
                while(*paramName && *paramName != '.')
                {
                    *ret = *ret*i + (*paramName-48);
                    paramName++;
                    inst = *ret;
                }
            }
            else
            {
                *ret =0;
                if(!*paramName)
                {
                    if(!*attrValue && (*(attrValue-2) == '}'))
                    {
                        *ret = inst;
                    }
                    status = 1;
                }
                break;
            }
        }
        attrValue++;
        paramName++;
    }
    return status;
}

/**
 * @brief Append the next object name with current Object name,
 *
 * @param[in] currentParam , Current Name of the Parameter.
 * @param[in] pAttparam , Current attribute value from xml
 */
void appendNextObject(char* currentParam, const char* pAttparam)
{
    while(true)
    {
        if(!(*currentParam == *pAttparam) )
        {
            // Skip instance numbers
            if(*pAttparam == '{')
            {
                if (*currentParam)
                {
                    pAttparam += 3;
                    currentParam = strstr(currentParam, ".");
                }
                else
                    break;
            }
            else
                break;
        }
        if(!*currentParam && !*pAttparam ) break;

        currentParam++;
        pAttparam++;
    }
    // Copy rest of the un matching strings to currentParam
    strcpy(currentParam, pAttparam);
}
/**
 * @brief Get the list of parameters which is matching with paramName
 *
 * @param[in] pParent , The current node in tinyxml node
 * @param[in] paramName , Wildcard parameter name
 * @param[in] currentParam , current parameter name replaced {i} with instance num
 */
static XMLNode* getList(XMLNode *pParent,char *paramName,char* currentParam,char **ptrParamList,char **pParamDataTypeList,int *paramCount)
{
    XMLNode* pChild;
    const char* maxEntries;
    int isReccursiveCall = 0;
    char zeroInstance[MAX_PARAMETER_LENGTH] = "\0";

    // If parent is Null Return
    if(!pParent) {
        return NULL;
    }
    // Identify whether the call is recursive or initial call
    if(!strcmp(currentParam,""))
    {
        isReccursiveCall = 0;
    }
    else
    {
        isReccursiveCall = 1;
    }

    // Goto actual Object node ie "Device."
    if( pParent->ToElement() == NULL )
    {
        for ( pChild = pParent->FirstChild(); pChild != 0; )
        {
            if( pChild->ToElement() == NULL )
            {
                pChild = pChild->NextSibling();
            }
            else
            {
                if( !strcmp (pChild->Value(), "object") )
                    break;
                pChild = pChild->FirstChild();
            }
        }
    }
    else
    {
        pChild = pParent;
    }
    // Traverse through the nodes and get matching parameters
    while(pChild)
    {
        XMLElement* pElement =  pChild->ToElement();
        const XMLAttribute* pAttrib = pElement->FirstAttribute();
        int inst = 0;
        int status = 0;
        char* endPtr = NULL;

        // Check if node is an Object
        if(!strcmp(pChild->Value(),"object"))
        {
            // Check if the Object is matching with given input wild card
            if(strstr(pAttrib->Value(),paramName) || (status = checkMatchingParameter(pAttrib->Value(),paramName,&inst)))
            {
                // If the number of instances are 0 then skip this object and go to next sibling
                if (*zeroInstance && strstr(pAttrib->Value(),zeroInstance))
                {
                    pChild = pChild->NextSibling();
                    continue;
                }
                else if(*zeroInstance)
                {
                    zeroInstance[0] = '\0';
                }
                // If matching found update the current parameter with wild card input string
                if( status && !isReccursiveCall) strcpy(currentParam, paramName);
                // Append if current attribute contains {i} to current param
                appendNextObject(currentParam, pAttrib->Value());
                XMLNode* bChild,*sChild;
                bChild = pChild;
                // Goto the parameters
                pChild = pChild->FirstChild();

                // Object not having any parameter thus go to next Sibling
                if(NULL == pChild)
                {
                    pChild = bChild->NextSibling();
                }
                maxEntries = pElement->Attribute("maxEntries");
                // Seems like a {i} instance
                if(maxEntries && ((!strcmp(maxEntries,"unbounded")) || (strtol(maxEntries,&endPtr, 10) > 1)))
                {
                    // Make Sure that its ends with {i}
                    if(isParamEndsWithInstance(pAttrib->Value()) == 0 )
                    {
                        int instanceNumber = 0;
                        int i=1;
                        // Get the Number of instances for that attribute
                        int actualInstance = getNumberofInstances(pAttrib->Value());
                        if(inst)
                        {
                            // Check if valid instance count is given in input wild card if not make it as zero, this will skip current branch
                            if(actualInstance >= inst)
                                i = instanceNumber = inst;
                            else
                                instanceNumber = 0;
                        }
                        else
                        {
                            instanceNumber = actualInstance;
                        }
                        sChild = pChild;
                        // Number of instances are > 0 go through each and populate data for each instance
                        char tparaName[MAX_PARAMETER_LENGTH];
                        while(i<=instanceNumber && inst==0)
                        {
                            memset(tparaName, 0,MAX_PARAMETER_LENGTH);
                            int len=strlen(currentParam)-4;
                            strcpy(tparaName, pChild->Parent()->ToElement()->FirstAttribute()->Value());
                            // Replace {i} with current instance number and call recursively
                            replaceWithInstanceNumber(currentParam,i);
                            sChild = getList(pChild,tparaName,currentParam,ptrParamList,pParamDataTypeList,paramCount);
                            strcpy(currentParam+len, INSTANCE_NUMBER_INDICATOR);
                            i++;
                        }
                        pChild = sChild;
                        // Seems like instance count is empty
                        if (!instanceNumber)
                        {   strcpy(zeroInstance, pAttrib->Value());
                            pChild = pChild->Parent();
                        }
                    }
                }
            }
            else if(isReccursiveCall) // Tree found once and processed and going to another branch so break
            {
                return pChild;
            }
            else // Tree not found yet goto next sibling and get it
            {
                pChild = pChild->NextSibling();
            }
        }
        // Found the Parameter
        else if(!strcmp(pChild->Value(),"parameter"))
        {
            XMLNode* bChild;
            // Find all parameters
            for(bChild = pChild ; pChild ; pChild=pChild->NextSibling() )
            {
                if(*paramCount < MAX_NUM_PARAMETERS)
                {
                    if(strlen(currentParam) > 0)
                    {
                        if(pChild->ToElement()->Attribute("getIdx") && strtol(pChild->ToElement()->Attribute("getIdx"),NULL,10) >= 1)
                        {
                            ptrParamList[*paramCount] = (char *) malloc(MAX_PARAMETER_LENGTH * sizeof(char));
                            pParamDataTypeList[*paramCount] = (char *) malloc(MAX_DATATYPE_LENGTH * sizeof(char));
                            snprintf(ptrParamList[*paramCount],MAX_PARAMETER_LENGTH,"%s%s",currentParam,pChild->ToElement()->FirstAttribute()->Value());
                            strncpy(pParamDataTypeList[*paramCount],pChild->FirstChild()->FirstChild()->Value(),MAX_DATATYPE_LENGTH-1);
                            (*paramCount)++;
                        }
                    }
                }
            }
            // Go to next object
            pChild = bChild->Parent();
            pChild = pChild->NextSibling();
        }
    }
    return pChild;

}
/* @brief Returns a parameter list and count given an input paramName with wildcard characters
 *
 * @dbhandle[in] database handle to query in to
 * @paramName[in] parameter name with wildcard(*)
 * @ParamList[out] parameter list extended by the input wildcard parameter
 * @ParamDataTypeList[out] parameter data type list extended by the input wildcard parameter
 * @paramCount[out] parameter count
 * @return DB_STATUS
 */
DB_STATUS getChildParamNamesFromDataModel(void *dbhandle,char *paramName,char **ParamList,char **ParamDataTypeList,int *paramCount)
{
    char parameterName[MAX_PARAMETER_LENGTH];
    char currentParam[MAX_PARAMETER_LENGTH] = "\0";
    strncpy(parameterName,paramName,MAX_PARAMETER_LENGTH-1);

    if(dbhandle == NULL)
        return DB_FAILURE;

    if(isWildCardParam(parameterName))
    {
        XMLDocument *doc = (XMLDocument *) dbhandle;
        getList(doc,parameterName,currentParam,ParamList,ParamDataTypeList,paramCount);
        if(*paramCount == 0)
        {
            return DB_ERR_INVALID_PARAMETER;
        }
    }
    else
    {
        return DB_ERR_WILDCARD_NOT_SUPPORTED;
    }
    return DB_SUCCESS;
}

void checkforParameterNameMatch(XMLNode *pParent, const char *ObjectName, const char *paramName, int *pMatch, DataModelParam *dmParam)
{
    if(!pParent)
    {
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"pParent Node is NULL.. returning form checkforAttributeMatch\n");
        return;
    }

    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"checkforAttributeMatch : pParent->Value() : %s\n", pParent->Value());
    if(pParent->ToElement() != NULL)
    {
        XMLElement* pElement = pParent->ToElement();
        const XMLAttribute* pAttrib = pElement->FirstAttribute();
        if(!strcmp(pParent->Value(),"parameter") && pAttrib)
        {
            char *ParameterName = (char *) malloc(sizeof(char) * MAX_PARAMETER_LENGTH);
            strncpy(ParameterName,ObjectName,MAX_PARAMETER_LENGTH-1);
            strncat(ParameterName,pAttrib->Value(),MAX_PARAMETER_LENGTH-1);
            ParameterName[MAX_PARAMETER_LENGTH-1] = '\0';
            bool match = !strcmp(ParameterName,paramName);
            free(ParameterName);
            if (match)
            {
                XMLNode *pSyntaxNode = NULL;
                XMLNode *pSyntaxChildNode = NULL;
                RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"Match found : Extracting parameter attributes\n");
                dmParam->objectName = strdup(paramName);
                if(pElement->Attribute("base") != NULL)
                    dmParam->paramName = strdup(pElement->Attribute("base"));
                if(pElement->Attribute("bsUpdate") != NULL)
                    dmParam->bsUpdate = strdup(pElement->Attribute("bsUpdate"));
                if(pElement->Attribute("access") != NULL)
                    dmParam->access = strdup(pElement->Attribute("access"));

                pSyntaxNode = pElement->FirstChildElement("syntax");
                for(pSyntaxChildNode = pSyntaxNode->FirstChild(); pSyntaxChildNode != NULL; pSyntaxChildNode = pSyntaxChildNode->NextSibling())
                {
                    if(!strcmp("default", pSyntaxChildNode->Value()))
                    {
                        XMLElement *pDefaultElement = pSyntaxChildNode->ToElement();
                        dmParam->defaultValue = strdup(pDefaultElement->Attribute("value"));
                        RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"Default Value : %s\n", dmParam->defaultValue);
                    }
                    else
                    {
                        dmParam->dataType = strdup(pSyntaxChildNode->Value());
                    }
                }
                *pMatch = 1;
                return;
            }
        }
    }
}

void checkforObjectMatch(XMLNode *pParent,const char *objectName,int *pMatch,DataModelParam *dmParam)
{
    if(!pParent)
    {
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"pParent Node is NULL.. returning form checkforParameterMatch\n");
        return;
    }

    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"checkforObjectMatch : pParent->Value() : %s\n", pParent->Value());
    if(pParent->ToElement() != NULL)
    {
        XMLElement* pElement = pParent->ToElement();
        const XMLAttribute* pAttrib = pElement->FirstAttribute();
        if(!strcmp(pParent->Value(),"object") && pAttrib)
        {
            RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"Comparing with object : %s\n", pAttrib->Value());

            if(!strcmp(pAttrib->Value(),objectName))
            {
                *pMatch = 1;
                return;
            }
        }
    }
}

void checkforParameterMatch(XMLNode *pParent,const char *paramName,int *pMatch,DataModelParam *dmParam)
{
    XMLNode *pChild = NULL;
    XMLNode *pParam = NULL;
    if(!pParent)
    {
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"pParent is Null - returning\n");
        return;
    }
    if(!paramName)
    {
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"paramName is Null - returning\n");
        return;
    }
    // Construct Object without parameter from input ParamName
    std::string *str1 = new std::string(paramName);
    std::size_t found = str1->find_last_of(".");
    delete str1;

    char *paramObject = (char *) malloc(sizeof(char) * MAX_PARAMETER_LENGTH);
    if(!paramObject)
    {
        RDK_LOG(RDK_LOG_DEBUG, LOG_TR69HOSTIF,"paramObject Malloc is Null - returning\n");
        return;
    }
    if(found != std::string::npos)
    {
        strncpy(paramObject,paramName,found);
        paramObject[found]='.';
        paramObject[found+1]='\0';
    }
    else
    {
        strncpy(paramObject,paramName,MAX_PARAMETER_LENGTH-1);
        paramObject[MAX_PARAMETER_LENGTH-1] = '\0';
    }

    RDK_LOG(RDK_LOG_TRACE1, LOG_TR69HOSTIF,"Looking for object : %s\n", paramObject);
    for(pChild = pParent->FirstChildElement("dm:document")->FirstChildElement("model")->FirstChildElement("object"); pChild != NULL; pChild = pChild->NextSibling())
    {
        if(pChild->ToElement() == NULL)
            continue;
        else
        {
            checkforObjectMatch(pChild,paramObject,pMatch,dmParam);
            if(*pMatch)
            {
                *pMatch = 0;
                for(pParam = pChild->FirstChild(); pParam != NULL; pParam = pParam->NextSibling())
                {
                    checkforParameterNameMatch(pParam, paramObject, paramName, pMatch, dmParam);
                    if(*pMatch)
                        goto end;
                }
            }
        }
    }
end:
    free(paramObject);
    return;
}

int getNumberOfDigitsInInstanceNumber(const char* paramName,int position)
{
    int digitCount=0;
    int len = 0;

    if(!paramName)
        return digitCount;
    len = strlen(paramName);
    while((position < len) && isdigit(paramName[position]))
    {
        digitCount++;
        position++;
    }
    return digitCount;
}

/* @brief Returns a parameter list and count given an input paramName with wildcard characters
 *
 * @filename[in] data-model xml filename (with absolute path)
 * @dbhandle[out] database handle
 * @dataType[out] Parameter DataType output
 * @return DB_STATUS
 */
int getParamInfoFromDataModel(void *dbhandle,const char *paramName, DataModelParam *dmParam)
{
    int Match = 0;
    int first_i = 0;
    char *newparamName = NULL;
    XMLDocument *doc = NULL;

    const std::lock_guard<std::mutex> lock(g_db_mutex);

    doc = (XMLDocument *) dbhandle;


    /* Check if Parameter is one of {i} entriesi ex:Device.WiFi.Radio.1.Status should become Device.WiFi.Radio.{i}.Status */
    std::string str(paramName);
    std::size_t found = str.find_first_of("0123456789");
    if(found != std::string::npos)
    {
        /* Check if match happens without a {i} */
        checkforParameterMatch(doc,paramName,&Match,dmParam);
        if(Match)
            return true;
        int numOfDigits = getNumberOfDigitsInInstanceNumber(paramName,found);
        first_i = found+1;
        newparamName =(char *) malloc(sizeof(char) * MAX_PARAMETER_LENGTH);
        char splitParam[MAX_PARAMETER_LENGTH] = "{i}";
        if(paramName[found+numOfDigits] == '.' && paramName[found-1] == '.')
        {
            strncpy(newparamName,paramName,found);
            newparamName[found]='\0';
            strncat(splitParam,str.substr(found+numOfDigits).data(),MAX_PARAMETER_LENGTH-1);
            splitParam[MAX_PARAMETER_LENGTH-1]='\0';
            // Check for Parameter Match with first {i}
            strncat(newparamName,splitParam,MAX_PARAMETER_LENGTH-1);
            newparamName[MAX_PARAMETER_LENGTH-1]='\0';
            checkforParameterMatch(doc,(const char*)newparamName,&Match,dmParam);
            if(Match)
                goto freeResources;
        }
        else
        {
            strncpy(newparamName,paramName,found+1);
            newparamName[found+1]='\0';
            strncpy(splitParam,paramName+found+1,MAX_PARAMETER_LENGTH);
            splitParam[MAX_PARAMETER_LENGTH-1]='\0';
        }

        // Check if splitParam has a digit
        std::string str(splitParam);
        std::size_t found = str.find_first_of("0123456789");
        if(found != std::string::npos)
        {
            strncpy(newparamName,paramName,first_i);
            newparamName[first_i] = '\0';
            numOfDigits = getNumberOfDigitsInInstanceNumber(splitParam,found);

            if(splitParam[found+numOfDigits] == '.' && splitParam[found-1] == '.')
            {
                splitParam[found]='\0';
                strcat(splitParam,"{i}");
                strncat(splitParam,str.substr(found+numOfDigits).data(),MAX_PARAMETER_LENGTH-1);
                splitParam[MAX_PARAMETER_LENGTH-1]='\0';

                strncat(newparamName,splitParam+3,MAX_PARAMETER_LENGTH-1);
                newparamName[MAX_PARAMETER_LENGTH-1]='\0';
                checkforParameterMatch(doc,(const char*)newparamName,&Match,dmParam);
                if(Match)
                    goto freeResources;
            }
            else
            {
                /* Find if there are more {i} entries */
                int first_num = found;
                std::string str(splitParam+first_num+1);
                std::size_t found = str.find_first_of("0123456789");
                if(found != std::string::npos)
                {
                    splitParam[found+first_num]='\0';
                    strcat(splitParam,".{i}");
                    strncat(splitParam,str.substr(found+1).data(),MAX_PARAMETER_LENGTH-1);
                    splitParam[MAX_PARAMETER_LENGTH-1]='\0';
                    //Check for parameter match with second {i}
                    strncat(newparamName,splitParam+3,MAX_PARAMETER_LENGTH-1);
                    newparamName[MAX_PARAMETER_LENGTH-1]='\0';
                    checkforParameterMatch(doc,(const char*)newparamName,&Match,dmParam);
                    if(Match)
                        goto freeResources;
                }
            }
        }

        if(first_i)
            newparamName[first_i-1]='\0';
        strncat(newparamName,splitParam,MAX_PARAMETER_LENGTH-1);
        newparamName[MAX_PARAMETER_LENGTH-1]='\0';
        checkforParameterMatch(doc,(const char *)newparamName,&Match,dmParam);
    }
    else
    {
        checkforParameterMatch(doc, paramName, &Match, dmParam);
    }
freeResources:
    if (newparamName)
    {
        free(newparamName);
    }
    return Match;
}

/**
 * @brief Get complete list of parameters from data model xml.
 * @param[in] pParent , The current node in tinyxml node
 * @param[out] param_list , Complete list of Parameter Name and type
 * @return DB_STATUS
 */
static DB_STATUS get_complete_parameter_list_from_dml_xml (
    XMLNode *pInParentXmlNode,
    char **pOutParamNameList,
    int *num_of_params)
{
    int params_count = 0;
    XMLNode* pChild = NULL;

    char currentParam[MAX_PARAMETER_LENGTH] = "\0";
    const char *top_node_name = "Device.";

    // If parent is Null Return
    if(!pInParentXmlNode) {
        return DB_FAILURE;
    }


    if( pInParentXmlNode->ToElement() == NULL )
    {
        for ( pChild = pInParentXmlNode->FirstChild(); pChild != 0; )
        {
            if( pChild->ToElement() == NULL )
            {
                pChild = pChild->NextSibling();
            }
            else
            {
                if( !strcmp (pChild->Value(), "object") )
                    break;
                pChild = pChild->FirstChild();
            }
        }
    }
    else
    {
        pChild = pInParentXmlNode;
    }

    // Traverse through the nodes and get matching parameters
    while(pChild)
    {
        XMLElement* pElement =  pChild->ToElement();
        const XMLAttribute* pAttrib = pElement->FirstAttribute();

        // Check if node is an Object
        if(!strcmp(pChild->Value(),"object"))
        {
            // Check if the Object is matching with given input wild card
            if(strstr(pAttrib->Value(),top_node_name))
            {
                appendNextObject(currentParam, pAttrib->Value());
                std::string str = pAttrib->Value();

                if (str.compare(str.size()-5,5,".{i}.") == 0) {
                    if(params_count < MAX_NUM_PARAMETERS)
                    {
                        pOutParamNameList[params_count] = (char *) calloc(MAX_PARAMETER_LENGTH, sizeof(char));
                        snprintf(pOutParamNameList[params_count], MAX_PARAMETER_LENGTH,"%s",pAttrib->Value());
                        params_count++;
                    }
                }
                XMLNode* bChild;
                bChild = pChild;
                // Goto the parameters
                pChild = pChild->FirstChild();

                // Object not having any parameter thus go to next Sibling
                if(NULL == pChild)
                {
                    pChild = bChild->NextSibling();
                }
            }
            else // Tree not found yet goto next sibling and get it
            {
                pChild = pChild->NextSibling();
            }
        }
        // Found the Parameter
        else if(!strcmp(pChild->Value(),"parameter"))
        {
            XMLNode* bChild;
            // Find all parameters
            for(bChild = pChild ; pChild ; pChild=pChild->NextSibling() )
            {
                if(params_count < MAX_NUM_PARAMETERS)
                {
                    if(strlen(currentParam) > 0)
                    {
                        if(pChild->ToElement()->Attribute("getIdx") && strtol(pChild->ToElement()->Attribute("getIdx"),NULL,10) >= 1)
                        {
                            pOutParamNameList[params_count] = (char *) calloc(MAX_PARAMETER_LENGTH, sizeof(char));
                            snprintf(pOutParamNameList[params_count],MAX_PARAMETER_LENGTH,"%s%s",currentParam,pChild->ToElement()->FirstAttribute()->Value());
                            params_count++;
                        }
                    }
                }
            }
            // Go to next object
            pChild = bChild->Parent();
            pChild = pChild->NextSibling();
        }
    }
    *num_of_params = params_count;
    return DB_SUCCESS;

}

/* @brief Get the complete list of parameter name and data type.
 *
 * @out_param_list[out] Complete list of Parameter Name and type
 * @return DB_STATUS
 */
DB_STATUS get_complete_param_list (char **out_param_list, int *out_param_count)
{
    DB_STATUS status = DB_SUCCESS;

    XMLDocument *in_xml_doc = (XMLDocument *)getDataModelHandle();

    if(in_xml_doc == NULL)
        return DB_FAILURE;
    else
    {
        get_complete_parameter_list_from_dml_xml(in_xml_doc, out_param_list, out_param_count);

        if(out_param_count == 0)
        {
            return status = DB_FAILURE;
        }
    }
    return status;
}

void freeDataModelParam(DataModelParam dmParam)
{
    if(dmParam.objectName)
        free(dmParam.objectName);
    if(dmParam.paramName)
        free(dmParam.paramName);
    if(dmParam.access)
        free(dmParam.access);
    if(dmParam.dataType)
        free(dmParam.dataType);
    if(dmParam.defaultValue)
        free(dmParam.defaultValue);
    if(dmParam.bsUpdate)
        free(dmParam.bsUpdate);
}

/* @brief to test get_complete_param_list.
 *
 */
void test_get_complete_param_list()
{

    char **pParam_name_list;

    pParam_name_list =  (char**) calloc (MAX_NUM_PARAMETERS, sizeof(char*));
    int num_of_params = 0;

    if(DB_SUCCESS != loadDataModel())
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"Failed to load data-model.xml file\n" );
    }

    if(DB_SUCCESS != get_complete_param_list (pParam_name_list, &num_of_params))
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"Failed to get complete parameter list.\n");
    }

    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"Count = %d \n", num_of_params);
    int i = 0;
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"======================================================================\n");
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"Iteration:1 => Starts\n ");
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"======================================================================\n");
    for(i=0; i< num_of_params; i++)
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF," [%d] Parameter : [ %s (%s)]\n", i +1, pParam_name_list[i]);
        free(pParam_name_list[i]);
    }
    free(pParam_name_list);
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"======================================================================\n");
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"Iteration:1 => Ends\n ");
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"======================================================================\n");
    DataModelParam dmParam = {0};

    const char *pParameterName = "Device.WiFi.EndPoint.1.Profile.1.Status";

    if (getParamInfoFromDataModel(g_dbhandle, pParameterName, &dmParam))
    {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "Found Parameter...!!!  [%s (%s)]\n ", pParameterName, dmParam.dataType);
    }
    else {
        RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF, "Invalid Parameter : [%s] \n ", pParameterName);
    }

    ((XMLDocument *)g_dbhandle)->Clear();
}
