#!/bin/bash

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

#TODO add it common config file
#CLICMD="/tmp/cliCMD"
CURL_RESPONSE="/tmp/.cURLresponse"
#CFG_RESPONSE="/tmp/.CFGresponse"

#source /etc/utopia/service.d/log_capture_path.sh

#echo "Parodus: read file script is called"

#if [ -f /usr/bin/configparamgen ] && [ -f $CLICMD ]; then
	#echo "Parodus: CURL_RESPONSE path is $CURL_RESPONSE"
	#configparamgen jx $CLICMD $CFG_RESPONSE
	if [ -f $CURL_RESPONSE ]; then
		printf `cat $CURL_RESPONSE`
		#rm -f $CURL_RESPONSE
	else
		printf "ERROR"
	fi
#else
#	printf "ERROR" 1>&3
#fi
