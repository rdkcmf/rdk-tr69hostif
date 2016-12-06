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
 * @file Device_WiFi_Radio_Stats.h
 *
 * TR-069 Device.WiFi.Radio.Stats object Public API.
 */

/**
 * @defgroup TR69_HOSTIF_WIFI_RADIO_STAT TR-069 Object (Device.WiFi.Radio.{i}.Stat.) Public APIs
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


#ifndef DEVICE_WIFI_RADIO_STATS_H_
#define DEVICE_WIFI_RADIO_STATS_H_

#include "hostIf_main.h"
#include "hostIf_utils.h"
#include "hostIf_tr69ReqHandler.h"
#include "hostIf_updateHandler.h"
#include "Device_WiFi.h"

/** @defgroup TR_069_DEVICE_WIFI API TR-069 Device.WiFi.Radio.Stats object API.
 *  @ingroup TR_069_DEVICE_WIFI_RADIO_API
 *
 *  Throughput statistics for this interface.
 *
 *  @{
 */

class hostIf_WiFi_Radio_Stats {

    static  GHashTable *ifHash;
    int dev_id;
    hostIf_WiFi_Radio_Stats(int dev_id);
    ~hostIf_WiFi_Radio_Stats() {};

public:
    static class hostIf_WiFi_Radio_Stats *getInstance(int dev_id);
    static GList* getAllAssociateDevs();
    static void closeInstance(hostIf_WiFi_Radio_Stats *);
    static void closeAllInstances();
    int get_Device_WiFi_Radio_Stats_Props_Fields(int radioIndex);
    void checkWifiRadioPropsFetch(int radioIndex);

    unsigned long BytesSent;
    unsigned long BytesReceived;
    unsigned long PacketsSent;
    unsigned long PacketsReceived;
    unsigned int ErrorsSent;
    unsigned int ErrorsReceived;
    unsigned int DiscardPacketsSent;
    unsigned int DiscardPacketsReceived;
    unsigned int NoiseFloor;

   /**
     * @ingroup TR69_HOSTIF_WIFI_RADIO_STAT
     * @{
     */
   /**
     * @brief Get total number of bytes transmitted out of the interface, including framing characters.
     *
     * This function provides the output as unsigned long value available in
     * Device.WiFi.Radio.{i}.Stats.BytesSent parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[in] radioIndex Index number.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_Device_WiFi_Radio_Stats_BytesSent(HOSTIF_MsgData_t *stMsgData,int radioIndex );

   /**
     * @brief Get the total number of bytes received on the interface, including framing characters.
     *
     * This function provides the output as unsigned long value available in
     * Device.WiFi.Radio.{i}.Stats.BytesReceived parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[in] radioIndex Index number.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_Device_WiFi_Radio_Stats_BytesReceived(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
     * @brief Get the value of 'Device.WiFi.Radio.Stats.PacketsSent'.
     * This function provides the total number of packets transmitted out of the interface.
     */
   /**
     * @brief Get the total number of packets transmitted out of the interface.
     *
     * 
     * This function provides the output as unsigned long value available in
     * Device.WiFi.Radio.{i}.Stats.PacketsSent parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[in] radioIndex Index number.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_Device_WiFi_Radio_Stats_PacketsSent(HOSTIF_MsgData_t *stMsgData,int radioIndex );

   /**
     * @brief Get the total number of packets received on the interface.
     *
     * This function provides the output as unsigned long value available in
     * Device.WiFi.Radio.{i}.Stats.PacketReceived parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[in] radioIndex Index number.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_Device_WiFi_Radio_Stats_PacketsReceived(HOSTIF_MsgData_t *stMsgData,int radioIndex );

   /**
     * @brief Get the total number of outbound packets that could not be transmitted because of errors.
     *
     * This function provides the output as unsigned int value available in
     * Device.WiFi.Radio.{i}.Stats.ErrorsSent parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[in] radioIndex Index number.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_Device_WiFi_Radio_Stats_ErrorsSent(HOSTIF_MsgData_t *stMsgData,int radioIndex );

   /**
     * @brief Get the total number of inbound packets that contained errors preventing them from being
     * delivered to a higher-layer protocol.
     *
     * This function provides the output as unsigned int value available in
     * Device.WiFi.Radio.{i}.Stats.ErrorsReceived parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[in] radioIndex Index number.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_Device_WiFi_Radio_Stats_ErrorsReceived(HOSTIF_MsgData_t *stMsgData,int radioIndex );

   /**
     * @brief Get the total number of outbound packets which were chosen to be discarded even though no
     * errors had been detected to prevent their being transmitted. One possible reason for discarding
     * such a packet could be to free up buffer space.
     *
     * This function provides the output as unsigned int value available in
     * Device.WiFi.Radio.{i}.Stats.DiscardPacketsSent parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[in] radioIndex Index number.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_Device_WiFi_Radio_Stats_DiscardPacketsSent(HOSTIF_MsgData_t *stMsgData,int radioIndex );

   /**
     * @brief Get the total number of inbound packets which were chosen to be discarded even though no errors
     * had been detected to prevent their being delivered. One possible reason for discarding such a packet
     * could be to free up buffer space.
     *
     * This function provides the output as unsigned int value available in
     * Device.WiFi.Radio.{i}.Stats.DiscardPacketsReceived parameter.
     *
     * @param[out] stMsgData TR-069 Host interface message request.
     * @param[in] radioIndex Index number.
     *
     * @return Returns 0 on success, otherwise will return the appropriate error code.
     */
    int get_Device_WiFi_Radio_Stats_DiscardPacketsReceived(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    int get_Device_WiFi_Radio_Stats_NoiseFloor(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /** @ */ //End of Doxygen tag TR69_HOSTIF_WIFI_RADIO_STAT
};

/* End of DEVICE_WIFI_RADIO_STATS_H_ doxygen group */
/**
 * @}
 */

#endif /* DEVICE_WIFI_RADIO_STATS_H_ */


/** @} */
/** @} */
