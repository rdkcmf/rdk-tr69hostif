#!/bin/sh

WEBPA_CFG_FILE="/etc/webpa_cfg.json"
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
    hw_mac=`cat /tmp/.macAddress | sed 's/://g'`;
    echo "$hw_mac"
}

parodus_start_up()
{
    # Getting Webpa Parameters
    ServerIP=`get_webpa_string_parameter "ServerIP"`
    NwInterface=`get_webpa_string_parameter "DeviceNetworkInterface"`
    ServerPort=`get_webpa_number_parameter "ServerPort"`
    PingWaitTime=`get_webpa_max_waiting_time "MaxPingWaitTimeInSec"`
    HwMac=`get_hardware_mac`

     echo "Starting parodus with arguments hw-mac=$HwMac webpa-ping-time=$PingWaitTime webpa-inteface-used=$NwInterface webpa-url=$ServerIP" 
     /bin/systemctl set-environment PARODUS_CMD="parodus --hw-mac=$HwMac --webpa-ping-time=$PingWaitTime --webpa-inteface-used=$NwInterface --webpa-url=$ServerIP --partner-id=comcast --webpa-backoff-max=9"
     echo "Parodus command set.." 
}

timer=0
while :
    do
    if [ -e "/tmp/.macAddress" ]; then
	parodus_start_up
	echo "parodus process is started"
	break
    else
	echo "Device details file not exists waiting for 2 min."
	sleep 5
	timer=`expr $timer + 5`
	if [ $timer -eq $MAX_PARODUS_WAIT_TIME ]; then
	    echo "Waited for 2 min. /tmp/.macAddress does not exists, unable to start parodus"
	    break
        fi
    fi
done

