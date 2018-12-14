##########################################################################
# If not stated otherwise in this file or this component's Licenses.txt
# file the following copyright and licenses apply:
#
# Copyright 2018 RDK Management
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

SUBDIRS = 

AM_CXXFLAGS = -I$(top_srcdir)/src/hostif/include \
	-I$(top_srcdir)/src/hostif/handlers/include \
	$(GLIB_CFLAGS) $(G_THREAD_CFLAGS) $(WIFI_PROFILE_FLAG)

AM_CXXFLAGS += "-std=c++11"

#AM_LDFLAGS = -lwifi_hal

noinst_LTLIBRARIES = libhostIfWiFi.la
libhostIfWiFi_la_SOURCES = Device_WiFi.cpp \
				Device_WiFi_Radio.cpp \
				Device_WiFi_Radio_Stats.cpp \
				Device_WiFi_SSID.cpp \
				Device_WiFi_SSID_Stats.cpp \
				Device_WiFi_EndPoint.cpp \
				Device_WiFi_EndPoint_Security.cpp \
				Device_WiFi_EndPoint_WPS.cpp \
				Device_WiFi_EndPoint_Profile.cpp \
				Device_WiFi_EndPoint_Profile_Security.cpp \
				Device_WiFi_AccessPoint.cpp \
				Device_WiFi_AccessPoint_WPS.cpp \
				Device_WiFi_AccessPoint_Security.cpp \
				Device_WiFi_AccessPoint_AssociatedDevice.cpp

if WIFI_CLIENT_ROAMING
AM_CXXFLAGS += -DWIFI_CLIENT_ROAMING
libhostIfWiFi_la_SOURCES += Device_WiFi_X_RDKCENTRAL_COM_ClientRoaming.cpp
endif

