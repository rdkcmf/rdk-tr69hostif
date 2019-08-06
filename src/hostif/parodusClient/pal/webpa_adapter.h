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
 * @file webpa_adapter.h
 *
 * @description This header defines the WebPA Abstraction APIs
 *
 */

#ifndef _WEBPA_ADAPTER_H_
#define _WEBPA_ADAPTER_H_

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include "waldb.h"
/**
 * @brief Platform specific defines.
 */

#define WEBPA_CFG_FILE_NAME             "webpa_cfg.json"
#define WEBPA_CFG_SERVER_IP             "ServerIP"
#define WEBPA_CFG_SERVER_PORT           "ServerPort"
#define WEBPA_CFG_SERVER_SECURE         "Secure"
#define WEBPA_CFG_DEVICE_NW_INTERFACE   "DeviceNetworkInterface"
#define WEBPA_CFG_RETRY_INTERVAL        "RetryIntervalInSec"
#define WEBPA_CFG_PING_WAIT_TIME        "MaxPingWaitTimeInSec"
#define WEBPA_CFG_NOTIFY                "Notify"
#define WEBPA_CFG_FIRMWARE_VER			"oldFirmwareVersion"
#define PARAM_CID                      "Device.DeviceInfo.Webpa.X_COMCAST-COM_CID"
#define PARAM_CMC                      "Device.DeviceInfo.Webpa.X_COMCAST-COM_CMC"

#define LOG_PARODUS_IF  "LOG.RDK.PARODUSIF"
#define WAL_FREE(__x__) if(__x__ != NULL) { free((void*)(__x__)); __x__ = NULL;}

#define MAX_PARAMETER_LEN 512
#define RDKC_XPC_SYNC_PARAM_CID
#define MAX_PARAMETERNAME_LEN 256

/**
 * @brief WebPA Error codes.
 */
typedef enum
{
    WAL_SUCCESS = 0,                    /**< Success. */
    WAL_FAILURE,                        /**< General Failure */
    WAL_ERR_TIMEOUT,
    WAL_ERR_NOT_EXIST,
    WAL_ERR_INVALID_PARAMETER_NAME,
    WAL_ERR_INVALID_PARAMETER_TYPE,
    WAL_ERR_INVALID_PARAMETER_VALUE,
    WAL_ERR_NOT_WRITABLE,
    WAL_ERR_SETATTRIBUTE_REJECTED,
    WAL_ERR_NAMESPACE_OVERLAP,
    WAL_ERR_UNKNOWN_COMPONENT,
    WAL_ERR_NAMESPACE_MISMATCH,
    WAL_ERR_UNSUPPORTED_NAMESPACE,
    WAL_ERR_DP_COMPONENT_VERSION_MISMATCH,
    WAL_ERR_INVALID_PARAM,
    WAL_ERR_UNSUPPORTED_DATATYPE,
    WAL_STATUS_RESOURCES,
    WAL_ERR_WIFI_BUSY
} WAL_STATUS;

/**
 * @brief Component or source that changed a param value.
 */
typedef enum
{
    CHANGED_BY_FACTORY_DEFAULT      = (1<<0), /**< Factory Defaults */
    CHANGED_BY_ACS                  = (1<<1), /**< ACS/TR-069 */
    CHANGED_BY_WEBPA                = (1<<2), /**< WebPA */
    CHANGED_BY_CLI                  = (1<<3), /**< Command Line Interface (CLI) */
    CHANGED_BY_SNMP                 = (1<<4), /**< SNMP */
    CHANGED_BY_FIRMWARE_UPGRADE     = (1<<5), /**< Firmware Upgrade */
    CHANGED_BY_WEBUI                = (1<<7), /**< Local Web UI (HTTP) */
    CHANGED_BY_UNKNOWN              = (1<<8), /**< Unknown */
    CHANGED_BY_XPC                  = (1<<9)  /**< xPC */
} PARAMVAL_CHANGE_SOURCE;

/**
 * @brief Set operations supported by WebPA.
 */
typedef enum
{
    WEBPA_SET = 0,
    WEBPA_ATOMIC_SET,
    WEBPA_ATOMIC_SET_XPC
} WEBPA_SET_TYPE;


/**
 * @brief WebPA Data types.
 */
typedef enum
{
    WAL_STRING = 0,
    WAL_INT,
    WAL_UINT,
    WAL_BOOLEAN,
    WAL_DATETIME,
    WAL_BASE64,
    WAL_LONG,
    WAL_ULONG,
    WAL_FLOAT,
    WAL_DOUBLE,
    WAL_BYTE,
    WAL_NONE
} WAL_DATA_TYPE;


/**
 * @brief WebPA Config params.
 */
typedef enum
{
    WCFG_COMPONENT_NAME = 0,
    WCFG_CFG_FILE,
    WCFG_CFG_FILE_SRC,
    WCFG_DEVICE_INTERFACE,
    WCFG_DEVICE_MAC,
    WCFG_DEVICE_REBOOT_PARAM,
    WCFG_DEVICE_REBOOT_VALUE,
    WCFG_XPC_SYNC_PARAM_CID,
    WCFG_XPC_SYNC_PARAM_CMC,
    WCFG_FIRMWARE_VERSION,
    WCFG_DEVICE_UP_TIME,
    WCFG_MANUFACTURER,
    WCFG_MODEL_NAME,
    WCFG_XPC_SYNC_PARAM_SPV,
    WCFG_PARAM_HOSTS_NAME,
    WCFG_PARAM_HOSTS_VERSION,
    WCFG_PARAM_SYSTEM_TIME,
    WCFG_RECONNECT_REASON,
    WCFG_REBOOT_REASON,
    WCFG_REBOOT_COUNTER
} WCFG_PARAM_NAME;

/**
 * @brief Structure to store Parameter info or Attribute info.
 */
typedef struct
{
    char *name;
    char *value;
    WAL_DATA_TYPE type;
} ParamVal, AttrVal;

/**
 * @brief WEBPA Notification types.
 */
typedef enum
{
    PARAM_NOTIFY = 0,
    UPSTREAM_MSG,
    TRANS_STATUS,
    CONNECTED_CLIENT_NOTIFY,
    PARAM_VALUE_CHANGE_NOTIFY,
    NOTIFY_TYPE_MAX
} NOTIFY_TYPE;


/**
 * @brief Structure to return Parameter info in Notification callback.
 */
typedef struct
{
    char* notifyDest;
    char* notifyPayload;
} ParamNotify;


typedef struct
{
    char *transId;
} TransData;

typedef struct
{
    char *nodeMacId;
    char *status;
} NodeData;

typedef union
{
    ParamNotify *notify;
    TransData * status;
    NodeData * node;
} Notify_Data;

typedef struct
{
    NOTIFY_TYPE type;
    Notify_Data *data;
} NotifyData;


/**
 * @brief WebPA Configuration parameters.
 */
typedef struct
{
    char interfaceName[16];
    char serverIP[256];
    unsigned int serverPort;
    int secureFlag;
    unsigned int retryIntervalInSec;
    unsigned int maxPingWaitTimeInSec;
    char oldFirmwareVersion[256];
} WebPaCfg;

/**
 * @brief Initializes the Message Bus and registers WebPA component with the stack.
 *
 * @param[in] name WebPA Component Name.
 * @return WAL_STATUS.
 */
WAL_STATUS msgBusInit(const char *name);

/**
 * @brief Registers the notification callback function.
 *
 * @param[in] cb Notification callback function.
 * @return WAL_STATUS.
 */
void registerNotifyCallback();


/**
 * @brief Set Notify configuration file
 *
 * @param[in] config file name
 */

void setInitialNotifyConfigFile(const char* nofityConfigFile);

/**
 * @brief processRequest processes the request and returns response payload
 *
 * @param[in] reqPayload input request to process
 * @param[in] resPayload retuns response payload
 */
void processRequest(char *reqPayload, char *transactionId, char **resPayload);

/* @brief Loads the data-model xml data
 *
 * @return DB_STATUS
 */
DB_STATUS LoadDataModel(void);

/* @brief Loads the data-model xml data
 *
 * @return DB_STATUS
 */
int  getDBHandle();

/* @brief Set the Initial Notify Parameters from config file
 *
 * @return void
 */
void setInitialNotify(void);

/* @brief Set the Initial Notify Parameters from config file
 *
 * @return void
 */
void registerNotifyCallback();

/**
 * @brief Loads the WebPA config file, parses it and extracts the WebPA config parameters.
 *
 * @param[in] cfgFileName Config filename (with absolute path)
 * @param[out] cfg WebPA config parameters
 * @return WAL_STATUS
 */
WAL_STATUS loadCfgFile(const char *cfgFileName, WebPaCfg *cfg);

/**
 * @brief WALInit Initalize wal
 */
void WALInit();

/**
 * @brief LOGInit Initialize RDK Logger
 */
void LOGInit();

/**
 * @brief waitForConnectReadyCondition wait till all dependent components
 * required for connecting to server are initialized
 */
void waitForConnectReadyCondition();

/**
 * @brief waitForOperationalReadyCondition wait till all dependent components
 * required for being operational are initialized
 */
void waitForOperationalReadyCondition();

/**
 * @brief _WEBPA_LOG WEBPA RDK logger API
 */

void _WEBPA_LOG(unsigned int level, const char *msg, ...)
__attribute__((format (printf, 2, 3)));

/**
 * @brief getWebPAConfig interface returns the WebPA config data.
 *
 * @param[in] param WebPA config param name.
 * @return const char* WebPA config param value.
 */
//const char* getWebPAConfig(WCFG_PARAM_NAME param);

/**
 * @brief sendIoTMessage interface sends message to IoT.
 *
 * @param[in] msg Message to be sent to IoT.
 * @return WAL_STATUS
 */
WAL_STATUS sendIoTMessage(const void *msg);



void getCurrentTime(struct timespec *timer);

#endif /* _WEBPA_ADAPTER_H_ */

