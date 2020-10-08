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
 * @defgroup TR69_HOSTIF_WIFI_SSID_STAT TR-069 Object (Device.WiFi.SSID.{i}.Stats) Public APIs
 *
 * Throughput statistics for this interface.
 *
 * The CPE MUST reset the interface's Stats parameters (unless otherwise stated in individual object or
 * parameter descriptions) either when the interface becomes operationally down due to a previous
 * administrative down (i.e. the interface's Status parameter transitions to a down state after the
 * interface is disabled) or when the interface becomes administratively up (i.e. the interface's Enable
 * parameter transitions from false to true). Administrative and operational interface status is discussed
 * in [Section 4.2.2/TR-181i2].
 *
 * @ingroup TR69_HOSTIF_WIFI
 */

/**
* @defgroup tr69hostif
* @{
* @defgroup hostif
* @{
**/


#ifndef DEVICE_WIFI_SSID_STATS_H_
#define DEVICE_WIFI_SSID_STATS_H_

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"


class hostIf_WiFi_SSID_Stats {

    static  GHashTable *ifHash;
    int dev_id  = 0;  //CID:103281 - UNINIT_CTOR
    hostIf_WiFi_SSID_Stats(int dev_id);
    ~hostIf_WiFi_SSID_Stats() {};

public:
    static class hostIf_WiFi_SSID_Stats *getInstance(int dev_id);
    GList* getAllInstances();
    static void closeInstance(hostIf_WiFi_SSID_Stats *);
    static void closeAllInstances();

    unsigned long	bytesSent;
    unsigned long	bytesReceived;
    unsigned long	packetsSent;
    unsigned long	packetsReceived;
    unsigned int	ErrorsSent;
    unsigned int	ErrorsReceived;
    unsigned long	UnicastPacketsSent;
    unsigned long	UnicastPacketsReceived;
    unsigned int	DiscardPacketsSent;
    unsigned int	DiscardPacketsReceived;
    unsigned long	MulticastPacketsSent;
    unsigned long	MulticastPacketsReceived;
    unsigned long	BroadcastPacketsSent;
    unsigned long	BroadcastPacketsReceived;
    unsigned int	UnknownProtoPacketsReceived;

   /**
    * @ingroup TR69_HOSTIF_WIFI_SSID_STAT
    * @{
    */
   /**
    * @brief Get the total number of bytes transmitted out of the interface, including framing characters.
    *
    * This function provides the output as a unsinged long value available in
    * Device.WiFi.SSID.Stats.BytesSent parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_Stats_BytesSent(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the total number of bytes received on the interface, including framing characters.
    *
    * This function provides the output as integer value available in Device.WiFi.SSID.Stats.BytesReceived parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_Stats_BytesReceived(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the total number of packets transmitted out of the interface.
    *
    * This function provides the output as integer value available in Device.WiFi.SSID.Stats.PacketsSent parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_Stats_PacketsSent(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the total number of packets received on the interface.
    *
    * This function provides the output as a integer value available in Device.WiFi.SSID.Stats.PacketsReceived parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_Stats_PacketsReceived(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the total number of outbound packets that could not be transmitted because of errors.
    *
    * This function provides the output as a integer value available in Device.WiFi.SSID.Stats.ErrorsSent parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_Stats_ErrorsSent(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the total number of inbound packets that contained errors preventing them from being delivered
    * to a higher-layer protocol.
    *
    * This function provides the output as a integer value available in Device.WiFi.SSID.Stats.ErrorsReceived parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_Stats_ErrorsReceived(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the total number of packets requested for transmission which were not addressed to a multicast
    * or broadcast address at this layer, including those that were discarded or not sent.
    *
    * This function provides the output as integer value available in
    * Device.WiFi.SSID.Stats.UnicastPacketsSent parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_Stats_UnicastPacketsSent(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the total number of received packets, delivered by this layer to a higher layer,
    * which were not addressed to a multicast or broadcast address at this layer.
    *
    * This function provides the output as a integer value available in
    * Device.WiFi.SSID.Stats.UnicastPacketsReceived parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_Stats_UnicastPacketsReceived(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the total number of outbound packets which were chosen to be discarded even though no errors
    * had been detected to prevent their being transmitted. One possible reason for discarding such a packet
    * could be to free up buffer space.
    *
    * This function provides the output as a integer value available in
    * Device.WiFi.SSID.Stats.DiscardPacketsSent parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_Stats_DiscardPacketsSent(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the total number of inbound packets which were chosen to be discarded even though no
    * errors had been detected to prevent their being delivered. One possible reason for discarding such a
    * packet could be to free up buffer space.
    *
    * This function provides the output as a integer value available in
    * Device.WiFi.SSID.Stats.DiscardPacketsReceived parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_Stats_DiscardPacketsReceived(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the total number of packets that higher-level protocols requested for transmission and
    * which were addressed to a multicast address at this layer, including those that were discarded or not sent.
    *
    * This function provides the output as numeric value available in
    * Device.WiFi.SSID.Stats.MulticastPacketsSent parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_Stats_MulticastPacketsSent(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the total number of received packets, delivered by this layer to a higher layer,
    * which were addressed to a multicast address at this layer.
    *
    * This function provides the output as a numeric value available in
    * Device.WiFi.SSID.Stats.MulticastPacketsReceived parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_Stats_MulticastPacketsReceived(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the total number of packets that higher-level protocols requested for transmission and
    * which were addressed to a broadcast address at this layer, including those that were discarded or not sent.
    *
    * This function provides the output as a numeric value available in
    * Device.WiFi.SSID.Stats.BroadcastPacketsSent parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_Stats_BroadcastPacketsSent(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the total number of received packets, delivered by this layer to a higher layer,
    * which were addressed to a broadcast address at this layer.
    *
    * This function provides the output as a numeric value available in
    * Device.WiFi.SSID.Stats.BroadcastPacketsReceived parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_Stats_BroadcastPacketsReceived(HOSTIF_MsgData_t *stMsgData );

   /**
    * @brief Get the total number of packets received via the interface which were discarded because
    * of an unknown or unsupported protocol.
    *
    * This function provides the output as a numeric value available in
    * Device.WiFi.SSID.Stats.UnknownProtoPacketsReceived parameter.
    *
    * @param[out] stMsgData TR-069 Host interface message request.
    *
    * @return Returns 0 on success, otherwise will return the appropriate error code.
    */
    int get_Device_WiFi_SSID_Stats_UnknownProtoPacketsReceived(HOSTIF_MsgData_t *stMsgData );

    /** @} */ //End of Doxygen tag TR69_HOSTIF_WIFI_SSID_STAT
};


#endif /* DEVICE_WIFI_SSID_STATS_H_ */


/** @} */
/** @} */
