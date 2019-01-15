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
. $RDK_PATH/getPartnerId.sh
WEBPA_CFG_OVERIDE_FILE="/opt/webpa_cfg.json"
CRUD_CONFIG_FILE="/opt/parodus_cfg.json"
SSL_CERT_FILE="/etc/ssl/certs/ca-certificates.crt"
CONFIG_RES_FILE="/tmp/adzvfchig-res.mch"
JWT_KEY="/etc/ssl/certs/webpa-rs256.pem"
DNS_TEXT_URL="fabric.xmidt.comcast.net"
TOKEN_SERVER_URL="https://issuer.xmidt.comcast.net:8080/issue"

Serial=""
BootTime=""

if [ -f "$WEBPA_CFG_OVERIDE_FILE" ] && [ "$BUILD_TYPE" != "prod" ]; then
    WEBPA_CFG_FILE=$WEBPA_CFG_OVERIDE_FILE
else
    WEBPA_CFG_FILE="/etc/webpa_cfg.json"
fi

if [ -f /usr/bin/GetConfigFile ];then
	GetConfigFile $CONFIG_RES_FILE
else
	echo "Error: GetConfigFile Not Found"
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

get_ImageName()
{
  image=$(cat /version.txt | grep ^imagename:$versionTag2 | cut -d ":" -f 2)
  echo "$image"
}

get_SerialNumber()
{
   local serial
   serial=$(grep "serial_number" /tmp/.deviceDetails.cache | cut -d'=' -f2)

   if [[ ! -z "$serial" ]]; then
       Serial=$serial
   else
       serial=$(curl -d '{"paramList" : [{"name" : "Device.DeviceInfo.SerialNumber"}]}' http://127.0.0.1:10999 | rev | cut -d\" -f2 | rev)
       if [[ -z "$serial" ]]; then
           echo "Failed to fetch the Serial Number."
       else
           Serial=$serial
       fi
    fi
}

get_BootTime()
{
   local boot_time=$(curl -d '{"paramList" : [{"name" : "Device.DeviceInfo.X_RDKCENTRAL-COM_BootTime"}]}' http://127.0.0.1:10999 | grep -oE "[^:]+$"  | cut -d\} -f1)
   if [[ -z "$boot_time" ]]; then
       echo "Failed to fetch the Boot time value."
   else
       BootTime=$boot_time
   fi
}

parodus_start_up()
{
    # Getting Webpa Parameters
    ServerIP=`get_webpa_string_parameter "ServerIP"`
    ACQUIRE_JWT=`get_webpa_number_parameter "acquire-jwt"`
    if [ -z $ACQUIRE_JWT ]; then
	ACQUIRE_JWT=1
    fi
    NwInterface=`get_webpa_string_parameter "DeviceNetworkInterface"`
    ServerPort=`get_webpa_number_parameter "ServerPort"`
    PingWaitTime=`get_webpa_max_waiting_time "MaxPingWaitTimeInSec"`
    HwMac=`get_hardware_mac`

    local Manufacture
    if [[ -z ${MANUFACTURE} ]]; then
        echo "Manufacturer is undefined."
    else
        Manufacture=${MANUFACTURE}
    fi

    local Model
    if [[ -z ${MODEL_NUM} ]]; then
        echo "Model number is undefined"
    else
        Model=${MODEL_NUM}
    fi

    # Get Partner ID :
    # Added Auth service query to get the Partner ID,
    # If empty then, then again query after 2 sec interval 
    # till max of 2 min. After max timeout, leave it empty.
    timer=0
    time_interval=2

    while :
      do
        param=$(curl -s -d POST http://127.0.0.1:50050/authService/getDeviceId | cut -d\, -f2 | tr -d \} | cut -d: -f2 | tr -d \" | xargs)
        if [[ -z "$param" ]]; then
          timer=$(( $timer + $time_interval ))
          echo "$(date) Failed to get the Partner-id, retrying after $time_interval sec."
          sleep $time_interval
          if [ "$timer" = "$MAX_PARODUS_WAIT_TIME" ]; then
            echo "$(date) Failed to get the Partner-id, retried till max time out of $MAX_PARODUS_WAIT_TIME sec."
            break
          fi
       else
         echo "$(date) Got the Partner id as \"$param"\"
         break
       fi
    done

    PartnerId=$param
    if [ $PartnerId = "unknown" ]; then
	PartnerId=""
    else 
	PartnerId=*,$PartnerId
    fi

    local reboot_reason=$(grep PreviousRebootReason /opt/logs/rebootInfo.log | cut -d: -f2)
    if [[ -z "reboot_reason" || ' ' = $reboot_reason  ]]; then
       echo "Last Reboot reason is not Known."
    fi

    Image=`get_ImageName`

    get_SerialNumber
    get_BootTime

    if [ -f $CONFIG_RES_FILE ]; then
	clientCertPath=$CONFIG_RES_FILE
    else
	echo "Failure in CFG response"
	clientCertPath=""
    fi

    if [ -z $HwMac ]; then
       echo "Failed to start Parodus, Can't fetch macAddress "
    else
       echo "Starting parodus with arguments hw-mac=$HwMac webpa-ping-time=$PingWaitTime webpa-interface-used=$NwInterface webpa-url=$ServerIP partner-id=$PartnerId --hw-manufacturer=$Manufacture --fw-name=$Image --hw-model=$Model --hw-serial-number=$Serial --boot-time=$BootTime --hw-last-reboot-reason=$reboot_reason"
       /bin/systemctl set-environment PARODUS_CMD=" --hw-mac=$HwMac --webpa-ping-time=$PingWaitTime --webpa-interface-used=$NwInterface --webpa-url=$ServerIP --partner-id=$PartnerId --webpa-backoff-max=9 --ssl-cert-path=$SSL_CERT_FILE  --acquire-jwt=$ACQUIRE_JWT --dns-txt-url=$DNS_TEXT_URL --jwt-public-key-file=$JWT_KEY --jwt-algo=RS256 --crud-config-file=$CRUD_CONFIG_FILE  --hw-manufacturer=$Manufacture --fw-name=$Image --hw-model=$Model --hw-serial-number=$Serial --boot-time=$BootTime --hw-last-reboot-reason=$reboot_reason --client-cert-path=$clientCertPath --token-server-url=$TOKEN_SERVER_URL"
       echo "Parodus command set.." 
    fi
}

parodus_start_up
