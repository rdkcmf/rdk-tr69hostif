##########################################################################
# If not stated otherwise in this file or this component's Licenses.txt
# file the following copyright and licenses apply:
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
##########################################################################

SUBDIRS =   STBService DeviceInfo Ethernet IP Time 
if WITH_DHCP_PROFILE
SUBDIRS += DHCPv4
endif
if WITH_STORAGESERVICE_PROFILE
SUBDIRS += StorageService 
endif
if WITH_INTFSTACK_PROFILE
SUBDIRS += InterfaceStack 
endif
if WITH_WIFI_PROFILE
SUBDIRS += wifi
endif


DIST_SUBDIRS =  STBService DeviceInfo Ethernet IP Time  
if WITH_DHCP_PROFILE
DIST_SUBDIRS += DHCPv4
endif
if WITH_STORAGESERVICE_PROFILE
DIST_SUBDIRS += StorageService 
endif
if WITH_INTFSTACK_PROFILE
DIST_SUBDIRS += InterfaceStack 
endif
if WITH_WIFI_PROFILE
DIST_SUBDIRS += wifi
endif
