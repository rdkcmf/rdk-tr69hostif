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
 * @brief TR-069 Device.WiFi.Radio.Stats object Public API.
 *
 * Description of Device.WiFi.Radio.Stats module.
 *
 *
 * @par Document
 * Document reference.
 *
 *
 * @par Open Issues (in no particular order)
 * -# Issue 1
 * -# Issue 2
 *
 *
 * @par Assumptions
 * -# Assumption
 * -# Assumption
 *
 *
 * @par Abbreviations
 * - ACK:     Acknowledge.
 * - BE:      Big-Endian.
 * - cb:      Callback function (suffix).
 * - config:  Configuration.
 * - desc:    Descriptor.
 * - dword:   Double word quantity, i.e., four bytes or 32 bits in size.
 * - intfc:   Interface.
 * - LE:      Little-Endian.
 * - LS:      Least Significant.
 * - MBZ:     Must be zero.
 * - MS:      Most Significant.
 * - _t:      Type (suffix).
 * - word:    Two byte quantity, i.e. 16 bits in size.
 * - xfer:    Transfer.
 *
 *
 * @par Implementation Notes
 * -# Note
 * -# Note
 *
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
     * @brief Get the value of 'Device.WiFi.Radio.Stats.BytesSent'.
     * This function provides the total number of
     * bytes transmitted out of the interface, including framing characters.
     *
     */
    int get_Device_WiFi_Radio_Stats_BytesSent(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
     * @brief Get the value of 'Device.WiFi.Radio.Stats.BytesReceived'.
     * This function provides the total number of bytes received on
     * the interface, including framing characters..
     */
    int get_Device_WiFi_Radio_Stats_BytesReceived(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
     * @brief Get the value of 'Device.WiFi.Radio.Stats.PacketsSent'.
     * This function provides the total number of packets transmitted out of the interface.
     */
    int get_Device_WiFi_Radio_Stats_PacketsSent(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
         * @brief Get the value of 'Device.WiFi.Radio.Stats.PacketsReceived'.
         * This function provides the total number of packets received on the interface..
     */
    int get_Device_WiFi_Radio_Stats_PacketsReceived(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
     * @brief Get the value of 'Device.WiFi.Radio.Stats.ErrorsSent'.
     * This function provides the total number of outbound packets that
     * could not be transmitted because of errors.
     */
    int get_Device_WiFi_Radio_Stats_ErrorsSent(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
     * @brief Get the value of 'Device.WiFi.Radio.Stats.ErrorsReceived'.
     * This function provides the total number of inbound packets that
     * contained errors preventing them from being delivered to a higher-layer protocol.
     */
    int get_Device_WiFi_Radio_Stats_ErrorsReceived(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
     * @brief Get the value of 'Device.WiFi.Radio.Stats.DiscardPacketsSent'.
     * This function provides the total number of outbound packets which were
     * chosen to be discarded even though no errors had been detected to prevent their being transmitted.
     * One possible reason for discarding such a packet could be to free up buffer space.
     */
    int get_Device_WiFi_Radio_Stats_DiscardPacketsSent(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    /**
     * @brief Get the value of 'Device.WiFi.Radio.Stats.DiscardPacketsReceived'.
     * This function provides the The total number of inbound packets which were chosen
     * to be discarded even though no errors had been detected to prevent their being delivered.
     * One possible reason for discarding such a packet could be to free up buffer space.
     */
    int get_Device_WiFi_Radio_Stats_DiscardPacketsReceived(HOSTIF_MsgData_t *stMsgData,int radioIndex );

    int get_Device_WiFi_Radio_Stats_NoiseFloor(HOSTIF_MsgData_t *stMsgData,int radioIndex );

};

/* End of DEVICE_WIFI_RADIO_STATS_H_ doxygen group */
/**
 * @}
 */

#endif /* DEVICE_WIFI_RADIO_STATS_H_ */


/** @} */
/** @} */
