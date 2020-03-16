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
