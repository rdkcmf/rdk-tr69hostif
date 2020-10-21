#!/bin/sh
#
# If not stated otherwise in this file or this component's LICENSE file the
# following copyright and licenses apply:
#
# Copyright 2019 RDK Management
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
#
#

#
# description:
# launch script for running speedtest-client this script support
# existing speedtest application ( the c-client ) only.
# NodeJS support is removed.
# default data is set before reading /etc/device.properties
# author: derric_lynns@comcast.com
#

# defaults
SPDTST_NAME="speedtest-client"
SPDTST_PATH=/usr/bin
SPDTST_DBG_PATH=/opt/diagnostics
SPDTST_LOGPATH=/opt/logs
LOG_FILE=/opt/logs/speedtest-init.log

# make sure the below logfile name is same as in log4c
SPDTST_LOGFILE=speedtest_log.txt
SPDTST_DEVICE="XI6"
#default is Xi6
SPDTST_CLIENT_TYPE=1

# uncomment the following lines for diagnostics mode launch
# echo "$(date +"[%Y-%m-%d %H:%M:%S]" )  diagnostics launch" >> $LOG_FILE
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
SPDTST_LOG=${SPDTST_LOGPATH}/${SPDTST_LOGFILE}

# launch script and log to file
SPDTST_LAUNCH_CMD="${SPDTST_BIN} &"

# decide client type for technicolor or arris XI6
if [ "$SPDTST_DEVICE" = "XI6" ]; then
    TYPE_CMD="curl -d '{\"paramList\" : [{\"name\" : \"Device.IP.Diagnostics.X_RDKCENTRAL-COM_SpeedTest.ClientType\"}]}'"
    TYPE_CMD="${TYPE_CMD} http://127.0.0.1:10999 2>&1 | grep value | cut -d \\\" -f 10"

else # rest of the devices on dmcli
    TYPE_CMD="dmcli eRT getv Device.IP.Diagnostics.X_RDKCENTRAL-COM_SpeedTest.ClientType"
    TYPE_CMD="$TYPE_CMD | grep value | cut -d ":" -f 3 | tr -d ' '"
fi

# default and set, prepare to launch
LAUNCH_CMD=${SPDTST_LAUNCH_CMD}

# get the client type and fall back on command/cli error
SPDTST_CLIENT_TYPE=$(eval "$TYPE_CMD")

if [ "$SPDTST_CLIENT_TYPE" = "1" ]; then
    LAUNCH_CMD=${SPDTST_LAUNCH_CMD}
    # launch now
    echo "$(date +"[%Y-%m-%d %H:%M:%S]" ) launching speedtest" >> $LOG_FILE
    echo "$(date +"[%Y-%m-%d %H:%M:%S]" ) $LAUNCH_CMD " >> $LOG_FILE
    $(eval "$LAUNCH_CMD")
else
    echo "$(date +"[%Y-%m-%d %H:%M:%S]" ) Unsupported client for XI6" >> $LOG_FILE
fi
