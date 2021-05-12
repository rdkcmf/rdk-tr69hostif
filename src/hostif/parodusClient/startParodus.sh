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
DNS_TEXT_URL=$(tr181 -g Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.HostIf.ParodusDnsTextUrl 2>&1)
if [ -z "$DNS_TEXT_URL" ]; then
   DNS_TEXT_URL="fabric.xmidt.comcast.net"
fi
TOKEN_SERVER_URL=$(tr181 -g Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.HostIf.ParodusTokenServerUrl 2>&1)
if [ -z "$TOKEN_SERVER_URL" ]; then
   TOKEN_SERVER_URL="https://issuer.xmidt.comcast.net:8080/issue"
fi

REBOOT_REASON_SECURE_FILE="/opt/secure/reboot/previousreboot.info"
REBOOT_REASON_REGULAR_FILE="/opt/persistent/previousreboot.info"

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

if [ -f "$REBOOT_REASON_SECURE_FILE" ]; then
    REBOOT_INFO=$REBOOT_REASON_SECURE_FILE
else
    REBOOT_INFO=$REBOOT_REASON_REGULAR_FILE
fi

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
    hw_mac=$(getEstbMacAddressWithoutColon)
    [ -z "$hw_mac" ] && [ -e "/tmp/.macAddress" ] && hw_mac=$(sed 's/://g' /tmp/.macAddress)
    echo "$hw_mac"
}

get_ImageName()
{
    image=$(grep "^imagename:$versionTag2" /version.txt | cut -d ":" -f 2)
    echo "$image"
}

get_SerialNumber()
{
    local serial
    serial=$(grep "serial_number" /tmp/.deviceDetails.cache | cut -d'=' -f2)

    if [ -n "$serial" ]; then
        Serial=$serial
    else
        serial=$(curl -d '{"paramList" : [{"name" : "Device.DeviceInfo.SerialNumber"}]}' http://127.0.0.1:10999 | rev | cut -d\" -f2 | rev)
        if [ -z "$serial" ]; then
            echo "Failed to fetch the Serial Number."
        else
            Serial=$serial
        fi
    fi
}

get_BootTime()
{
    local boot_time=$(curl -d '{"paramList" : [{"name" : "Device.DeviceInfo.X_RDKCENTRAL-COM_BootTime"}]}' http://127.0.0.1:10999 | grep -oE "[^:]+$"  | cut -d\} -f1)
    if [ -z "$boot_time" ]; then
        echo "Failed to fetch the Boot time value."
    else
        BootTime=$boot_time
    fi
}

parodus_start_up()
{
    # Getting Webpa Parameters
    ServerIP=$(tr181 -g Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.HostIf.ParodusServerUrl 2>&1)
    if [ -z "$ServerIP" ]; then
         ServerIP=$(get_webpa_string_parameter "ServerIP")
    fi

    ACQUIRE_JWT=$(get_webpa_number_parameter "acquire-jwt")
    if [ -z "$ACQUIRE_JWT" ]; then
        ACQUIRE_JWT=1
    fi
    NwInterface=$(get_webpa_string_parameter "DeviceNetworkInterface")
    ServerPort=$(get_webpa_number_parameter "ServerPort")
    PingWaitTime=$(get_webpa_max_waiting_time "MaxPingWaitTimeInSec")
    HwMac=$(get_hardware_mac)
    if [ -z "$HwMac" ]; then
        echo "Failed to fetch macAddress. Exiting."
        # sleep 10 # add any more delay needed for a macAddress fetch retry (beyond the RestartSec=10s in parodus.service)
        exit 1
    fi

    local Manufacture
    if [ -z "${MANUFACTURE}" ]; then
        echo "Manufacturer is undefined."
    else
        Manufacture=${MANUFACTURE}
    fi

    local Model
    if [ -z "${MODEL_NUM}" ]; then
        echo "Model number is undefined"
    else
        Model=${MODEL_NUM}
    fi

    # use partner-id from auth service to start parodus
    if ! getDeviceIdResponse=$(curl -f -s -d POST http://127.0.0.1:50050/authService/getDeviceId); then
        echo "Could not get Partner-id from authservice. Exiting."
        exit 2 # Retry handled by parodus.service (Restart=always)
    fi
    PartnerId=$(echo $getDeviceIdResponse | cut -d\, -f2 | tr -d \} | cut -d: -f2 | tr -d \" | xargs)
    echo "$(date) authservice returned Partner-id as \"$PartnerId\""

    if [ "$PartnerId" = "unknown" ]; then
        PartnerId=""
    else
        PartnerId="*,$PartnerId"
    fi

    local reboot_reason=$(cat $REBOOT_INFO | grep "\"reason\"" | cut -d: -f2 | cut -d, -f1)
    if [ -z "$reboot_reason" ] || [ ' ' = "$reboot_reason" ]; then
        echo "Last Reboot reason is not Known."
        reboot_reason="Unknown"
    fi

    Image=$(get_ImageName)

    get_SerialNumber
    get_BootTime

    if [ -f $CONFIG_RES_FILE ]; then
        clientCertPath=$CONFIG_RES_FILE
    else
        echo "Failure in CFG response"
        clientCertPath=""
    fi

    echo "Starting parodus with arguments hw-mac=$HwMac webpa-ping-time=$PingWaitTime webpa-interface-used=$NwInterface webpa-url=$ServerIP partner-id=$PartnerId --hw-manufacturer=$Manufacture --fw-name=$Image --hw-model=$Model --hw-serial-number=$Serial --boot-time=$BootTime --hw-last-reboot-reason=$reboot_reason"
    /bin/systemctl set-environment PARODUS_CMD=" --hw-mac=$HwMac --webpa-ping-time=$PingWaitTime --webpa-interface-used=$NwInterface --webpa-url=$ServerIP --partner-id=$PartnerId --webpa-backoff-max=9 --ssl-cert-path=$SSL_CERT_FILE  --acquire-jwt=$ACQUIRE_JWT --dns-txt-url=$DNS_TEXT_URL --jwt-public-key-file=$JWT_KEY --jwt-algo=RS256 --crud-config-file=$CRUD_CONFIG_FILE  --hw-manufacturer=$Manufacture --fw-name=$Image --hw-model=$Model --hw-serial-number=$Serial --boot-time=$BootTime --hw-last-reboot-reason=$reboot_reason --client-cert-path=$clientCertPath --token-server-url=$TOKEN_SERVER_URL"
    echo "Parodus command set.."
}

parodus_start_up
sleep 10
