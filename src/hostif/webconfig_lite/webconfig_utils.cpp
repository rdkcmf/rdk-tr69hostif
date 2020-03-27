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
/**
 * @file webconfig_utils.cpp
 *
 * @description This file describes the utility functions for webconfig module
 *
 */

#include <webconfig_utils.h>

int getKeyValue(char *key, char *value)
{
    FILE *fp = fopen(DEV_DETAILS_FILE, "r");
    int i,j;
    int rval = ERR;
    if (NULL != fp)
    {
        char appendkey[] = "=";
        char str[512] = {'\0'};
        char searchkey[64] = {'\0'};

        strncpy(searchkey, key, strlen(key));
        strcat(searchkey, appendkey);
        while(fscanf(fp,"%s", str) != EOF)
        {
            char *searchVal = NULL;
            if(searchVal = strstr(str, searchkey))
            {
                searchVal = searchVal + strlen(searchkey);
                strncpy(value, searchVal, (strlen(str) - strlen(searchkey))+1);
                rval = SUCCESS;
            }
        }
        fclose(fp);
        for (i=j=0; i<strlen(value); i++) 
        {
            if (value[i] != ':') 
                value[j++] = value[i]; 
        }
        value[j] = '\0';

        for(i=0;i<=strlen(value);i++){
            if(value[i]>=65&&value[i]<=90)
                value[i]=value[i]+32;
        } 
    }
    if((strlen(value))<1)/*It means only key is populated in the file & value is yet to be*/
    {
        rval = ERR;
    }
    RDK_LOG(RDK_LOG_INFO, LOG_TR69HOSTIF,"webconfig_lite: [%s] %d key = %s, value = %s rval= %d\n\n", __FUNCTION__, __LINE__, key, value, rval);
    return rval;
}
