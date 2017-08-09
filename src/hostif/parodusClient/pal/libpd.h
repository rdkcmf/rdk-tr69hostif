/**
 * @file libpd.h
 *
 * @description This header defines the WebPA Abstraction APIs
 *
 * Copyright (c) 2015  Comcast
 */
#include "webpa_adapter.h"
/**
 * @brief Set Initial Notify config files
 */
void libpd_set_notifyConfigFile(const char* configFile);

/**
 * @brief Initialize libparodus connection and load DB
 */
void *libpd_client_mgr(void *args);



/**
 * Send Notification Message
 * @param[in] - Notification payload
 * @param[in] - Notification Source
 * @param[in] -  Notification Destination
 */
void sendNotification(char *payload, char *source, char *destination);


