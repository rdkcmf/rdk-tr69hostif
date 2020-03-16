#!/bin/bash
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
