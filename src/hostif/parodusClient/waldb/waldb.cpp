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
#include "waldb.h"
#include "tinyxml.h"
#include "stdlib.h"
#include "libIBus.h"
#include "webpa_parameter.h"
#include "hostIf_tr69ReqHandler.h"
#define MAX_PARAMETER_LENGTH 512
#define MAX_DATATYPE_LENGTH 48
#define MAX_NUM_PARAMETERS 2048
#define INSTANCE_NUMBER_INDICATOR "{i}."
#define MAX_PARAM_LENGTH TR69HOSTIFMGR_MAX_PARAM_LEN
#define BLUETOOTH_DISCOVERED_DEVICE_COUNT_PARAM  "Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDeviceCnt"
#define BLUETOOTH_PAIRED_DEVICE_COUNT_PARAM  "Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDeviceCnt"
#define BLUETOOTH_CONNECTED_DEVICE_COUNT_PARAM   "Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDeviceCnt"

/**
 * @briefGHashTable For holding Parameter count query strings
 */
GHashTable  *instanceNumParamHash = NULL;


//static void getList(TiXmlNode *pParent,char *paramName,char **ptrParamList,char **pParamDataTypeList,int *paramCount);
static TiXmlNode* getList(TiXmlNode *pParent,char *paramName,char* currentParam,char **ptrParamList,char **pParamDataTypeList,int *paramCount);
static void checkforParameterMatch(TiXmlNode *pParent,char *paramName,int *pMatch,char *dataType);
void appendNextObject(char* currentParam, const char* pAttparam);
int isParamEndsWithInstance(const char* paramName);
void replaceWithInstanceNumber(char *paramName, int instanceNumber);
int checkMatchingParameter(const char* attrValue, char* paramName, int* ret);
void appendNextObject(char* currentParam, const char* pAttparam);
int getNumberofInstances(const char* paramName);

/* @brief Loads the data-model xml data
 *
 * @filename[in] data-model xml filename (with absolute path)
 * @dbhandle[out] database handle
 * @return DB_STATUS
 */
DB_STATUS loaddb(const char *filename,void *dbhandle)
{
    TiXmlDocument *doc = NULL;
    int *dbhdl = (int *)dbhandle;
    doc = new TiXmlDocument(filename);
    if(doc != NULL)
    {
        bool loadOK = doc->LoadFile();
        if( loadOK )
        {
            *dbhdl = (int) doc;
            return DB_SUCCESS;
        }
        else
        {
            *dbhdl = 0;
            return DB_FAILURE;
        }
    }
    else
    {
        *dbhdl = 0;
        return DB_FAILURE;
    }
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
        strcpy(numberOfEntitiesParam,paramName);
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
            int ret = 0;
            int retCount = 0;
            const char *getParamList[]={numberOfEntitiesParam};
            ParamVal ***parametervalArr = (ParamVal ***) malloc(sizeof(ParamVal **) );
            getValues(getParamList, 1, NULL, parametervalArr, &retCount, (WAL_STATUS *)&ret);
            if(parametervalArr[0][0] && parametervalArr[0][0]->value)
            {
                instanceCount = strtol(parametervalArr[0][0]->value,NULL,10);
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
static TiXmlNode* getList(TiXmlNode *pParent,char *paramName,char* currentParam,char **ptrParamList,char **pParamDataTypeList,int *paramCount)
{
    TiXmlNode* pChild;
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
    if( pParent->Type() != TiXmlNode::TINYXML_ELEMENT )
    {
        for ( pChild = pParent->FirstChild(); pChild != 0; )
        {
            if( pChild->Type() != TiXmlNode::TINYXML_ELEMENT )
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
        TiXmlElement* pElement =  pChild->ToElement();
        TiXmlAttribute* pAttrib = pElement->FirstAttribute();
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
                TiXmlNode* bChild,*sChild;
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
            TiXmlNode* bChild;
            // Find all parameters
            for(bChild = pChild ; pChild ; pChild=pChild->NextSibling() )
            {
                if(*paramCount <= MAX_NUM_PARAMETERS)
                {
                    ptrParamList[*paramCount] = (char *) malloc(MAX_PARAMETER_LENGTH * sizeof(char));
                    pParamDataTypeList[*paramCount] = (char *) malloc(MAX_DATATYPE_LENGTH * sizeof(char));
                    if(strlen(currentParam) > 0)
                    {
                        snprintf(ptrParamList[*paramCount],MAX_PARAMETER_LENGTH,"%s%s",currentParam,pChild->ToElement()->FirstAttribute()->Value());
                        strncpy(pParamDataTypeList[*paramCount],pChild->FirstChild()->FirstChild()->Value(),MAX_DATATYPE_LENGTH-1);
                    }
                    (*paramCount)++;
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
DB_STATUS getParameterList(void *dbhandle,char *paramName,char **ParamList,char **ParamDataTypeList,int *paramCount)
{
    char parameterName[MAX_PARAMETER_LENGTH];
    char currentParam[MAX_PARAMETER_LENGTH] = "\0";
    strncpy(parameterName,paramName,MAX_PARAMETER_LENGTH-1);
    TiXmlDocument *doc = (TiXmlDocument *) dbhandle;
    if(isWildCardParam(parameterName))
    {
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

static void checkforParameterMatch(TiXmlNode *pParent,char *paramName,int *pMatch,char *dataType)
{
    static int matched = 0;
    if(!pParent)
        return;

    TiXmlNode *pChild;
    static int isObject = 0;
    static char ObjectName[MAX_PARAMETER_LENGTH];

    if(pParent->Type() == TiXmlNode::TINYXML_ELEMENT)
    {
        TiXmlElement* pElement = pParent->ToElement();
        TiXmlAttribute* pAttrib = pElement->FirstAttribute();
        if(!strcmp(pParent->Value(),"object"))
            isObject = 1;

        if(pAttrib)
        {
            // Construct Object without parameter from input ParamName
            std::string *str1 = new std::string(paramName);
            std::size_t found = str1->find_last_of(".");
            delete str1;

            char *paramObject = (char *) malloc(sizeof(char) * MAX_PARAMETER_LENGTH);
            if(found != std::string::npos)
            {
                strncpy(paramObject,paramName,found);
                paramObject[found]='.';
                paramObject[found+1]='\0';
            }
            else
            {
                strncpy(paramObject,paramName,MAX_PARAMETER_LENGTH-1);
                paramObject[MAX_PARAMETER_LENGTH] = '\0';
            }

            if(!strcmp(pAttrib->Value(),paramObject))
            {
                strncpy(ObjectName,pAttrib->Value(),MAX_PARAMETER_LENGTH-1);
                ObjectName[MAX_PARAMETER_LENGTH] = '\0';
                matched = 1;
            }
            free(paramObject);

            if(matched || !isObject)
            {
                if(!strcmp(pParent->Value(),"parameter"))
                {
                    isObject = 0;
                    char *ParameterName = (char *) malloc(sizeof(char) * MAX_PARAMETER_LENGTH);
                    strncpy(ParameterName,ObjectName,MAX_PARAMETER_LENGTH-1);
                    strncat(ParameterName,pAttrib->Value(),MAX_PARAMETER_LENGTH-1);
                    ParameterName[MAX_PARAMETER_LENGTH] = '\0';
                    bool match = !strcmp(ParameterName,paramName);
                    free(ParameterName);
                    if (match)
                    {
                        strncpy(dataType,pParent->FirstChild()->FirstChild()->Value(),MAX_DATATYPE_LENGTH-1);
                        *pMatch = 1;
                        return;
                    }
                }
            }
        }
    }


    for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling())
    {
        checkforParameterMatch(pChild,paramName,pMatch,dataType);
    }
    matched = 0;
}

/* @brief Returns a parameter list and count given an input paramName with wildcard characters
 *
 * @filename[in] data-model xml filename (with absolute path)
 * @dbhandle[out] database handle
 * @dataType[out] Parameter DataType output
 * @return DB_STATUS
 */
int isParameterValid(void *dbhandle,char *paramName,char *dataType)
{
    int Match = 0;
    int first_i = 0;
    TiXmlDocument *doc = (TiXmlDocument *) dbhandle;
    char *newparamName = NULL;

    /* Check if Parameter is one of {i} entriesi ex:Device.WiFi.Radio.1.Status should become Device.WiFi.Radio.{i}.Status */
    std::string str(paramName);
    std::size_t found = str.find_first_of("0123456789");
    if(found != std::string::npos)
    {
        /* Check if match happens without a {i} */
        checkforParameterMatch(doc,paramName,&Match,dataType);
        if(Match)
            return true;
        first_i = found+1;
        newparamName =(char *) malloc(sizeof(char) * MAX_PARAMETER_LENGTH);
        char splitParam[MAX_PARAMETER_LENGTH] = "{i}";
        if(paramName[found+1] == '.')
        {
            strncpy(newparamName,paramName,found);
            newparamName[found]='\0';
            strncat(splitParam,str.substr(found+1).data(),MAX_PARAMETER_LENGTH-1);
            splitParam[MAX_PARAMETER_LENGTH-1]='\0';
            // Check for Parameter Match with first {i}
            strncat(newparamName,splitParam,MAX_PARAMETER_LENGTH-1);
            newparamName[MAX_PARAMETER_LENGTH-1]='\0';
            checkforParameterMatch(doc,newparamName,&Match,dataType);
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

            if(splitParam[found+1]=='.')
            {
                splitParam[found]='\0';
                strcat(splitParam,"{i}");
                strncat(splitParam,str.substr(found+1).data(),MAX_PARAMETER_LENGTH-1);
                splitParam[MAX_PARAMETER_LENGTH-1]='\0';

                strncat(newparamName,splitParam+3,MAX_PARAMETER_LENGTH-1);
                newparamName[MAX_PARAMETER_LENGTH-1]='\0';
                checkforParameterMatch(doc,newparamName,&Match,dataType);
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
                    checkforParameterMatch(doc,newparamName,&Match,dataType);
                    if(Match)
                        goto freeResources;
                }
            }
        }

        if(first_i)
            newparamName[first_i-1]='\0';
        strncat(newparamName,splitParam,MAX_PARAMETER_LENGTH-1);
        newparamName[MAX_PARAMETER_LENGTH-1]='\0';
        checkforParameterMatch(doc,newparamName,&Match,dataType);
    }
    else
    {
        checkforParameterMatch(doc,paramName,&Match,dataType);
    }
freeResources:
    if (newparamName)
    {
        free(newparamName);
    }
    return Match;
}
