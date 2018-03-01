#!/bin/sh

# If not stated otherwise in this file or this component's Licenses.txt file the
# following copyright and licenses apply:
#
# Copyright 2016 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

. /etc/device.properties
. /etc/include.properties
. $RDK_PATH/utils.sh
WEBPA_CFG_OVERIDE_FILE="/opt/webpa_cfg.json"
SSL_CERT_FILE="/etc/ssl/certs/ca-certificates.crt"

if [ -f "$WEBPA_CFG_OVERIDE_FILE" ] && [ "$BUILD_TYPE" != "prod" ]; then
    WEBPA_CFG_FILE=$WEBPA_CFG_OVERIDE_FILE
else
    WEBPA_CFG_FILE="/etc/webpa_cfg.json"
fi

MAX_PARODUS_WAIT_TIME=120

get_webpa_string_parameter()
{
    param_name=$1
    param_value=$( sed -n 's/.*"'$param_name'": "\(.*\)",/\1/p' $WEBPA_CFG_FILE )
    echo "$param_value"
}

get_webpa_number_parameter()
{
    param_name=$1
    param_value=$( sed -n 's/.*"'$param_name'": \(.*\),/\1/p' $WEBPA_CFG_FILE )
    echo "$param_value"
}

get_webpa_max_waiting_time()
{
    param_name=$1
    param_value=$( sed -n 's/.*"'$param_name'": \(.*\)/\1/p' $WEBPA_CFG_FILE )
    echo "$param_value"
}

get_hardware_mac()
{
    hw_mac=`getEstbMacAddressWithoutColon`
    if [ -z $hw_mac ]; then
       timer=0
       while :
           do
           if [ -e "/tmp/.macAddress" ]; then
              hw_mac=`cat /tmp/.macAddress | sed 's/://g'`;
              echo "$hw_mac"
              break
           else
              #echo "Device details file not exists waiting for 2 min."
              sleep 2
              timer=`expr $timer + 2`
              if [ $timer -eq $MAX_PARODUS_WAIT_TIME ]; then
                 #echo "Waited for 2 min. /tmp/.macAddress does not exists, unable to start parodus"
                 break
              fi
           fi
       done
    else  
       echo "$hw_mac"
    fi

}

parodus_start_up()
{
    # Getting Webpa Parameters
    #ServerIP=`get_webpa_string_parameter "ServerIP"`
    # Hardcoded URL to fix DELIA-26528, regression of RDKB-16206
    ServerIP="https://fabric.webpa.comcast.net:8080"

    NwInterface=`get_webpa_string_parameter "DeviceNetworkInterface"`
    ServerPort=`get_webpa_number_parameter "ServerPort"`
    PingWaitTime=`get_webpa_max_waiting_time "MaxPingWaitTimeInSec"`
    HwMac=`get_hardware_mac`
    if [ -z $HwMac ]; then
       echo "Failed to start Parodus, Can't fetch macAddress "
    else
       echo "Starting parodus with arguments hw-mac=$HwMac webpa-ping-time=$PingWaitTime webpa-interface-used=$NwInterface webpa-url=$ServerIP" 
       /bin/systemctl set-environment PARODUS_CMD=" --hw-mac=$HwMac --webpa-ping-time=$PingWaitTime --webpa-interface-used=$NwInterface --webpa-url=$ServerIP --partner-id=comcast --webpa-backoff-max=9 --ssl-cert-path=$SSL_CERT_FILE"
       echo "Parodus command set.." 
    fi
}

parodus_start_up
