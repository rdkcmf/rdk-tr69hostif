/**
 * @file webpa_notification.cpp
 *
 * @description This file describes the Webpa Parameter Request Handling
 *
 * Copyright (c) 2017  Comcast
 */

/*
 * Notification Structure to be filled
 */



#include "webpa_adapter.h"
/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
/**
 * @brief Initializes WebPA configuration file
 *
 * @return void.
 */
void setNotifyConfigurationFile(const char* nofityConfigFile);

/**
 * @brief Initializes WebPA configuration file parameters
 *
 * @return void.
 */
int getnotifyparamList(char ***notifyParamList,int *ptrnotifyListSize);

/**
 * @brief Initializes WebPA configuration file parameters
 *
 * @return void.
 */
char* processNotification(NotifyData *notifyMsg,char* payload);

/**
 * @brief Initializes WebPA notification source
 *
 * @return void.
 */
char* getNotifyDestination(char *notifyDest);

/**
 * @brief Initializes WebPA notification Destination
 *
 * @return void.
 */
char * getNotifySource();
