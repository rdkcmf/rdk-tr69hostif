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
 * @file XrdkBlueTooth.h
 * @brief The header file provides components Xrdk SDCard information APIs.
 */

/**
 * @defgroup TR69_HOSTIF_STBSERVICES_SDCARD TR-069 Object (Device.STBServices.{i}.Components.XrdkSDCard.{i})
 * @par External rdk SDCard
 * @ingroup TR69_HOSTIF_STBSERVICES
 *
 * @defgroup TR69_HOSTIF_STBSERVICES_SDCARD_API TR-069 Object (Device.STBServices.{i}.Components.XrdkSDCard.{i}) Public APIs
 * Describe the details about RDK TR-069 components XRDK_SDK APIs specifications.
 * @ingroup TR69_HOSTIF_STBSERVICES_SDCARD
 *
 * @defgroup TR69_HOSTIF_STBSERVICES_SDCARD_CLASSES TR-069 Object (Device.STBServices.{i}.Components.XrdkSDCard.{i}) Public Classes
 * Describe the details about classes used in TR069 components XRDK_SDCARD.
 * @ingroup TR69_HOSTIF_STBSERVICES_SDCARD
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/

#ifndef X_RDKCENTRAL_COM_XBLUETOOTH_H_
#define X_RDKCENTRAL_COM_XBLUETOOTH_H_

#ifdef USE_XRDK_BT_PROFILE

#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"
#include "hostIf_main.h"
#include <mutex>
extern "C" {
#include "btmgr.h"
}

#define X_BT_ROOT_OBJ 			"Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth."
#define BT_ENABLE_STRING 							"enable"
#define BT_DISCOVERY_ENABLED_STRING 				"DiscoveryEnabled"
#define BT_DISCOVERY_DEVICE_CNT_STRING 				"DiscoveredDeviceCnt"
#define BT_PAIRED_DEVICE_CNT_STRING 				"PairedDeviceCnt"
#define BT_CONNECTED_DEVICE_CNT_STRING				"ConnectedDeviceCnt"

#define X_BT_DISCOVERED_DEV_OBJ "Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DiscoveredDevice"
#define X_BT_PAIRED_DEV_OBJ 	"Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.PairedDevice"
#define X_BT_CONNECTED_DEV_OBJ	"Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.ConnectedDevice"
#define X_BT_DEVICEINFO_OBJ		"Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.DeviceInfo"

#ifdef BLE_TILE_PROFILE
//#define X_BT_BLE_TILE_OBJ		"Device.DeviceInfo.X_RDKCENTRAL-COM_xBlueTooth.BLE.Tile"
#define BT_TILE_ID_STRING               "BLE.Tile.Ring.Id"
#define BT_TILE_SESSION_ID_STRING       "BLE.Tile.Ring.SessionId"
#define BT_TILE_TRIGGER_STRING          "BLE.Tile.Ring.Trigger"
#define BT_TILE_CMD_REQUEST_STRING      "BLE.Tile.Cmd.Request"
#endif 

#define BT_DEV_NAME_STRING 				"Name"
#define BT_DEV_DEVICE_ID_STRING 		"DeviceID"
#define BT_DEV_LOWENERGYENABLED_STRING	"LowEnergyEnabled"
#define BT_DEV_ACTIVE_STRING			"Active"
#define BT_DEV_CONNECTED_STRING         "Connected"
#define BT_DEV_DEVICE_TYPE_STRING       "DeviceType"
#define BT_DEV_PAIRED_STRING            "Paired"

#define	BT_DEV_GETDEVICEINFO_STRING		"GetDeviceInfo"
#define BT_DEV_INFO_DEVICE_ID_STRING 		"DeviceInfo.DeviceID"
#define BT_DEV_INFO_PROFILE_STRING 			"DeviceInfo.Profile"
#define BT_DEV_INFO_MAC_STRING 				"DeviceInfo.MAC"
#define BT_DEV_INFO_MANUFACTURER_STRING 	"DeviceInfo.Manufacturer"
#define BT_DEV_INFO_RSSI_STRING             "DeviceInfo.RSSI"
#define BT_DEV_INFO_SIGNALLEVEL_STRING		"DeviceInfo.SignalStrength"

/**
 * @brief This class provides the TR-069 components Bluetooth devices information.
 * @ingroup TR69_HOSTIF_X_RDKCENTRAL-COM_xBlueTooth_CLASSES
 */
class hostIf_DeviceInfoRdk_xBT
{
private:
    hostIf_DeviceInfoRdk_xBT();
    ~hostIf_DeviceInfoRdk_xBT() {};

    static std::mutex m;

    static hostIf_DeviceInfoRdk_xBT* m_instance;
    static int noOfDiscoveredDevice;
    static int noOfPairedDevice;
    static int noOfConnectedDevices;

    static BTRMGR_DiscoveredDevicesList_t disDevList;
    static BTRMGR_PairedDevicesList_t pairedDevList;
    static BTRMGR_ConnectedDevicesList_t connectedDevList;

    static updateCallback mUpdateCallback;

    string tile_Id;
    string sessionId;
    bool   triggerFlag;
    int do_Ring_A_Tile(bool );
    int process_TileCmdRequest(HOSTIF_MsgData_t *stMsgData);

    int isEnabled(HOSTIF_MsgData_t *);
    int isDiscoveryEnabled(HOSTIF_MsgData_t *);
    int getDiscoveredDeviceCnt(HOSTIF_MsgData_t *);
    int getPairedDeviceCnt(HOSTIF_MsgData_t *);
    int getConnectedDeviceCnt(HOSTIF_MsgData_t *);

    /* Discovered Device Methods */
    int getDiscoveredDevice_Name(HOSTIF_MsgData_t *);
    int getDiscoveredDevice_Profile(HOSTIF_MsgData_t *);
    int getDiscoveredDevice_MAC(HOSTIF_MsgData_t *);
    int getDiscoveredDevice_DeviceID(HOSTIF_MsgData_t *);
    int getDiscoveredDevice_Manufacturer(HOSTIF_MsgData_t *);
    int getDiscoveredDevice_Connected(HOSTIF_MsgData_t *);
    int getDiscoveredDevice_DeviceType(HOSTIF_MsgData_t *);
    int getDiscoveredDevice_Paired(HOSTIF_MsgData_t *);


    /* Paired Device Methods */
    int getPairedDevice_Name(HOSTIF_MsgData_t *);
    int getPairedDevice_Profile(HOSTIF_MsgData_t *);
    int getPairedDevice_MAC(HOSTIF_MsgData_t *);
    int getPairedDevice_Manufacturer(HOSTIF_MsgData_t *);
    int getPairedDevice_Connected(HOSTIF_MsgData_t *);
    int getPairedDevice_DeviceType(HOSTIF_MsgData_t *);
    int getPairedDevice_DeviceID(HOSTIF_MsgData_t *);

    /* Connected Device methods */
    int getConnectedDevice_Name(HOSTIF_MsgData_t *);
    int getConnectedDevice_Active(HOSTIF_MsgData_t *);
    int getConnectedDevice_DeviceType(HOSTIF_MsgData_t *);
    int getConnectedDevice_DeviceID(HOSTIF_MsgData_t *);

    /* DeviceInfo methods */
    int getDeviceInfo_DeviceID(HOSTIF_MsgData_t *);
    int getDeviceInfo_Manufacturer(HOSTIF_MsgData_t *);
    int getDeviceInfo_Profile(HOSTIF_MsgData_t *);
    int getDeviceInfo_MAC(HOSTIF_MsgData_t *);
    int getDeviceInfo_SignalLevel(HOSTIF_MsgData_t *);
    int getDeviceInfo_RSSI(HOSTIF_MsgData_t *);

    int getDeviceInfo(HOSTIF_MsgData_t *);
    int setDeviceInfo(HOSTIF_MsgData_t *);

    static void fetch_Bluetooth_DiscoveredDevicesList();
    static void fetch_Bluetooth_PairedDevicesList();
    static void fetch_Bluetooth_ConnectedDevicesList();

    void notifyValueChangeEvent(updateCallback mUpdateCallback);

public:
    static void reset();
    static hostIf_DeviceInfoRdk_xBT *getInstance();
    static void closeInstance();
    int handleGetMsg(HOSTIF_MsgData_t *);
    int handleSetMsg(HOSTIF_MsgData_t *);

    static void registerUpdateCallback(updateCallback cb);
    static void checkForUpdates();
};

#endif /*#USE_XRDK_BLUETOOTH_PROFILE*/
#endif /* X_RDKCENTRAL_COM_XBLUETOOTH_H_ */


/** @} */
/** @} */
