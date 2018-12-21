#
# ============================================================================
# RDK MANAGEMENT, LLC CONFIDENTIAL AND PROPRIETARY
# ============================================================================
# This file (and its contents) are the intellectual property of RDK Management, LLC.
# It may not be used, copied, distributed or otherwise  disclosed in whole or in
# part without the express written permission of RDK Management, LLC.
# ============================================================================
# Copyright (c) 2016 RDK Management, LLC. All rights reserved.
# ============================================================================
#

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

