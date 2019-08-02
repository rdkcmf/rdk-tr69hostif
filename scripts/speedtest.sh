#!/bin/sh
#
# March 2019, derric_lynns@comcast.com
#
# launch script for running speedtest-client this script takes care of two existing speedtest application ( the c-client and  nodejs )
#
# default data is set before reading /etc/device.properties
#
#

# defaults
SPDTST_NAME="speedtest-client"
NODE_SPDTST_NAME="run_speedtest.sh"
SPDTST_PATH=/usr/bin
NODE_SPDTST_PATH=/etc/speedtest
SPDTST_DBG_PATH=/opt/diagnostics
SPDTST_LOGPATH=/opt/logs

# make sure the below logfile name is same as in log4c
SPDTST_LOGFILE=speedtest_log.txt
SPDTST_DEVICE="XI6"
#default is Xi6
SPDTST_CLIENT_TYPE=1

# uncomment the following lines for diagnostics mode launch
# echo "diagnostics launch"
# if debug path exists then take it from there
# if [ -d "$SPDTST_DBG_PATH" ]; then
#    SPDTST_PATH="$SPDTST_DBG_PATH"
# fi

# read from the device
if [ -f /etc/device.properties ]; then
    . /etc/device.properties
	if [ "$SPDTST_DEVICE_PATH" ]; then
	    SPDTST_PATH=${SPDTST_DEVICE_PATH}
	fi

	if [ "$SPDTST_DEVICE_BIN" ]; then
	    SPDTST_NAME=${SPDTST_DEVICE_BIN}
	fi

	if [ "$SPDTST_DEVICE_LOGPATH" ]; then
	    SPDTST_LOGPATH=${SPDTST_DEVICE_LOGPATH}
	fi

	if [ "$SPDTST_DEVICE_LOGFILE" ]; then
	    SPDTST_LOGFILE=${SPDTST_DEVICE_LOGFILE}
	fi

	if [ "$BOX_TYPE" ]; then
        SPDTST_DEVICE=${BOX_TYPE}
	fi
fi

# fuel the speedtest
SPDTST_BIN=${SPDTST_PATH}/${SPDTST_NAME}
NODE_SPDTST_BIN=${NODE_SPDTST_PATH}/${NODE_SPDTST_NAME}
SPDTST_LOG=${SPDTST_LOGPATH}/${SPDTST_LOGFILE}

# launch script and log to file
SPDTST_LAUNCH_CMD="${SPDTST_BIN} &"
SPDTST_NODEJS_CMD="${NODE_SPDTST_BIN} &"

# decide client type for technicolor or arris XI6
if [ "$SPDTST_DEVICE" = "XI6" ]; then
    TYPE_CMD="curl -d '{\"paramList\" : [{\"name\" : \"Device.IP.Diagnostics.X_RDKCENTRAL-COM_SpeedTest.ClientType\"}]}'"
    TYPE_CMD="${TYPE_CMD} http://127.0.0.1:10999 2>&1 | grep value | cut -d \\\" -f 10"

else # rest of the devices on dmcli
    TYPE_CMD="dmcli eRT getv Device.IP.Diagnostics.X_RDKCENTRAL-COM_SpeedTest.ClientType"
    TYPE_CMD="$TYPE_CMD | grep value | cut -d ":" -f 3 | tr -d ' '"
fi

# default and set, prepare to launch
LAUNCH_CMD=${SPDTST_NODEJS_CMD}

# get the client type and fall back on command/cli error
SPDTST_CLIENT_TYPE=$(eval "$TYPE_CMD")
nummer='^[0-9]+$'

if ! [[ $SPDTST_CLIENT_TYPE =~ $nummer ]] ; then
   SPDTST_CLIENT_TYPE=1
fi

if [ "$SPDTST_CLIENT_TYPE" = "1" ]; then
    LAUNCH_CMD=${SPDTST_LAUNCH_CMD}
fi

# launch now
echo "launching speedtest" > /dev/console
echo $LAUNCH_CMD > /dev/console
$(eval "$LAUNCH_CMD")
