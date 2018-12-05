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
 /*
*
* FileName  : Service_Storage.h
* Purpose   : This class acts as a subhandler for Storage Service. 
* 
*/
#ifndef _SERVICE_STORAGE_H_
#define _SERVICE_STORAGE_H_
#include "hostIf_msgHandler.h"
#include "hostIf_updateHandler.h"


class hostIf_StorageSrvc
{
    
    static  GHashTable  *storageSrvHash;

    static GMutex *m_mutex;

    int dev_id;
    
    hostIf_StorageSrvc(int dev_id);

    public:
        
        static hostIf_StorageSrvc* getInstance(int dev_id);

        static void closeInstance(hostIf_StorageSrvc*);

        static GList* getAllInstances();

        static void closeAllInstances();
        
        static unsigned int get_Device_StorageSrvc_ClientNumberOfEntries(HOSTIF_MsgData_t *stMsgData);

        int handleGetMsg(HOSTIF_MsgData_t *stMsgData);

        int handleSetMsg(HOSTIF_MsgData_t *stMsgData);

        static void getLock();

        static void releaseLock();

};
#endif
