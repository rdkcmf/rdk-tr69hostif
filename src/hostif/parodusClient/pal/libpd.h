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
 * @file libpd.h
 *
 * @description This header defines the WebPA Abstraction APIs
 *
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
 * @brief Stops parodus receive wait thread
 */
void stop_parodus_recv_wait();

/**
 * Send Notification Message
 * @param[in] - Notification payload
 * @param[in] - Notification Source
 * @param[in] -  Notification Destination
 */
void sendNotification(char *payload, char *source, char *destination);


