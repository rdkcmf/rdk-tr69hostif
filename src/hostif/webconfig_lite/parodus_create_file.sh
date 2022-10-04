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

#. /etc/include.properties
#source /etc/utopia/service.d/log_capture_path.sh
#source /etc/log_timestamp.sh

#SER_NUM=$1
MAC=$1
TIMEOUT=30
#TODO add it common config file
CURL_RESPONSE="/tmp/.cURLresponse"
#CLICMD="/tmp/cliCMD"
SERVER_URL=$(tr181 -g Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.HostIf.ParodusTokenServerUrl 2>&1)
if [ -z "$SERVER_URL" ]; then
   SERVER_URL="https://issuer.xmidt.comcast.net:8080/issue"
fi

HTTP_CODE="/tmp/cfg_http_code"
CURL_FILE="/tmp/adzvfchig-res.mch"
CURL_FILE_RESPONSE="/tmp/adzvfchig-conf.mch"

#source /etc/utopia/service.d/log_capture_path.sh

get_PartnerIdFromAuthService()
{
    # use partner-id from auth service
    if getDeviceIdResponse=$(/lib/rdk/getDeviceId.sh); then
	PartnerId=$(echo $getDeviceIdResponse | cut -d\, -f2 | tr -d \} | cut -d: -f2 | tr -d \" | xargs)
	#echo "$(date) authservice returned Partner-id as \"$PartnerId\""
    fi
    if [ "$PartnerId" == "" ] || [ "$PartnerId" == "unknown" ]; then
        PartnerId=""
    else
        PartnerId="*,$PartnerId"
    fi
}

runcURL()
{
		if [ -f /usr/bin/configparamgen ]; then
			
			if [ -f $CURL_FILE ]; then
				
				#configparamgen jx $CURL_FILE $CURL_FILE_RESPONSE
				
				#if [ -f $CURL_FILE_RESPONSE ]; then	
					UUID=$(uuidgen -r)
					#echo "Parodus: generated uuid is $UUID"	
					#echo "Parodus: MAC is $MAC"
					#echo "Parodus: SER_NUM is $SER_NUM"
					#echo "Parodus: Transaction-Id is $UUID"
					#echo "Parodus: WEBCONFIG_INTERFACE is $WEBCONFIG_INTERFACE"
					get_PartnerIdFromAuthService
					#echo "WEBCONFIG: PartnerId is $PartnerId"
					retry_count=1
					while [ true ]
					do
						CURL_TOKEN="curl -w '%{http_code}\n'  --output $CURL_RESPONSE --cacert $CERT_PATH  -E $CURL_FILE --header X-Midt-Mac-Address:\"$MAC\" --header X-Midt-Serial-Number:\"$SER_NUM\" --header X-Midt-Uuid:\"$UUID\" --header X-Midt-Transaction-Id:\"$UUID\" --header X-Midt-Partner-Id:\"$PartnerId\" $SERVER_URL"
						result= eval $CURL_TOKEN > $HTTP_CODE
						ret=$?
						sleep 1
						http_code=$(awk -F\" '{print $1}' $HTTP_CODE)
						#echo "Parodus: ret = $ret http_code: $http_code"
						if [ $ret == 0 ] && [ $http_code == 200 ]; then
							#echo "Parodus: cURL success."
							break;
						elif [ $retry_count == 3 ]; then
							#echo "Parodus: Curl retry is reached to max 3 attempts, exiting the script."
							break;
						fi
						#echo "Parodus: Curl execution is failed, retry attempt: $retry_count."
						retry_count=$((retry_count+1));
					done
					rm -f $CURL_FILE_RESPONSE
					rm -f $HTTP_CODE
					#Check for cURL success and its response file
					if [ $http_code -eq 200 ] && [ -f $CURL_RESPONSE ]; then
							#echo "Parodus: cURL success"
							#configparamgen mi $CURL_RESPONSE $CLICMD
							#if [ -f $CLICMD ]; then
								#echo "Parodus: File generated successfully"
								printf "SUCCESS"
                                                                #echo "Parodus: After update"
							#else
							#	echo_t "Parodus: Failed to generate file"
							#fi
						
					#else
						#echo "Parodus: cURL request failed or it's response file not created "
					fi
						#rm -f $CURL_RESPONSE
				#else
				#	echo "Parodus: Failure in CFG response"
				#fi
			#else
				#echo "Parodus: CFG process file is not available"
			fi
		#else
			#echo "Parodus: CFG not available"			
		fi		
}

	#echo "Parodus: Remove all the existing file"
	rm -f $CURL_RESPONSE
	#rm -f $CLICMD
	runcURL
