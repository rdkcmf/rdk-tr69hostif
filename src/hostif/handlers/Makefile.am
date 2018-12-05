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

SUBDIRS = 
DIST_SUBDIRS = 

AM_CXXFLAGS = -I$(top_srcdir)/src/hostif/include \
	-I$(top_srcdir)/src/hostif/profiles/STBService \
	-I$(top_srcdir)/src/hostif/profiles/X_COMCAST-COM/X_COMCAST-COM_Xcalibur/include \
	-I$(top_srcdir)/src/hostif/profiles/moca \
	-I$(top_srcdir)/src/hostif/profiles/Ethernet \
	-I$(top_srcdir)/src/hostif/profiles/DeviceInfo \
	-I$(top_srcdir)/src/hostif/profiles/IP \
	-I$(top_srcdir)/src/hostif/profiles/Time \
	-I$(top_srcdir)/src/hostif/handlers/include \
	-I$(top_srcdir)/src/hostif/profiles/DHCPv4 \
	-I$(top_srcdir)/src/hostif/profiles/InterfaceStack \
	-I$(top_srcdir)/src/hostif/profiles/StorageService \
	-I$(PKG_CONFIG_SYSROOT_DIR)$(includedir)/directfb \
	-I$(PKG_CONFIG_SYSROOT_DIR)$(includedir)/rdk/iarmmgrs/sysmgr \
	-I$(PKG_CONFIG_SYSROOT_DIR)$(includedir)/rdk/iarmmgrs/tr69Bus \
	-I$(top_srcdir)/src/hostif/profiles/moca \
	-I$(top_srcdir)/src/hostif/profiles/wifi/include \
	-I./include $(GLIB_CFLAGS) $(G_THREAD_CFLAGS) \
	$(XREMGR_FLAGS) $(MOCAMGR_FLAGS) $(SOUP_CFLAGS) $(MOCA2_FLAG) \
	$(WIFI_PROFILE_FLAG) $(XRDK_SDCARD_PROFILE_FLAG)\
        -I=/usr/include/rdk/iarmbus/ \
        -I=/usr/include/rdk/iarmmgrs/sysmgr/ \
        -I$(PKG_CONFIG_SYSROOT_DIR)/usr/include/rdk/tr69agent/ \
        -I=/usr/include/rdk/ds/ \
        -I=/usr/include/rdk/ds-hal/ \
	$(XRDK_RF4CE_PROFILE_FLAG)

AM_CXXFLAGS += "-std=c++11" $(PROCPS_CFLAGS)

if WITH_XRDK_SDCARD_PROFILE
AM_CXXFLAGS += $(XRDK_SDCARD_PROFILE_FLAG)
endif

if WITH_INTFSTACK_PROFILE
AM_CXXFLAGS += $(INTFSTACK_PROFILE_FLAG) -I$(top_srcdir)/src/hostif/profiles/InterfaceStack
endif

if WITH_DHCP_PROFILE
AM_CXXFLAGS += -DUSE_DHCPv4_PROFILE -I$(top_srcdir)/src/hostif/profiles/DHCPv4
endif

if WITH_STORAGESERVICE_PROFILE
AM_CXXFLAGS += -DUSE_STORAGESERVICE_PROFILE -I$(top_srcdir)/src/hostif/profiles/StorageService
endif

if WITH_XRDK_RF4CE_PROFILE
AM_CXXFLAGS += $(XRDK_RF4CE_PROFILE_FLAG)
endif

if WITH_XRDK_BT_PROFILE
AM_CXXFLAGS += $(XRDK_BT_PROFILE_FLAG)
AM_CXXFLAGS += -DBTMGR_ENABLE_IARM_INTERFACE
AM_CXXFLAGS += -I$(top_srcdir)/src/hostif/profiles/DeviceInfo
endif

if WITH_HWSELFTEST_PROFILE
AM_CXXFLAGS += $(HWSELFTEST_PROFILE_FLAG)
endif

if WITH_SNMP_ADAPTER
AM_CXXFLAGS += -DSNMP_ADAPTER_ENABLED -I$(top_srcdir)/src/hostif/snmpAdapter
endif

AM_LDFLAGS = $(GLIB_LIBS) $(G_THREAD_LIBS) $(SOUP_LIBS) $(PROCPS_LIBS) -lIARMBus -lyajl -ldirect -lfusion -lds -ldshalcli -ldbus-1
if WIFI_CLIENT_ROAMING
AM_CXXFLAGS += -DWIFI_CLIENT_ROAMING
endif


noinst_LTLIBRARIES = libMsgHandlers.la
libMsgHandlers_la_SOURCES =	src/hostIf_IARM_ReqHandler.cpp \
				src/hostIf_msgHandler.cpp \
				src/hostIf_jsonReqHandlerThread.cpp \
				src/hostIf_dsClient_ReqHandler.cpp \
				src/hostIf_DeviceClient_ReqHandler.cpp \
				src/hostIf_XREClient_ReqHandler.cpp \
				src/hostIf_updateHandler.cpp \
				src/hostIf_MoCAClient_ReqHandler.cpp \
				src/hostIf_TimeClient_ReqHandler.cpp \
				src/hostIf_IPClient_ReqHandler.cpp \
				src/hostIf_EthernetClient_ReqHandler.cpp \
				src/hostIf_WiFi_ReqHandler.cpp 
if WITH_DHCP_PROFILE
libMsgHandlers_la_SOURCES +=	src/hostIf_DHCPv4Client_ReqHandler.cpp
endif

if WITH_INTFSTACK_PROFILE
AM_CXXFLAGS += $(INTFSTACK_PROFILE_FLAG)
libMsgHandlers_la_SOURCES +=	src/hostIf_InterfaceStackClient_ReqHandler.cpp
endif

if WITH_STORAGESERVICE_PROFILE
libMsgHandlers_la_SOURCES +=     src/hostIf_StorageService_ReqHandler.cpp
endif

if WITH_SNMP_ADAPTER
libMsgHandlers_la_SOURCES +=     src/hostIf_SNMPClient_ReqHandler.cpp
endif

if WITH_IPV6_SUPPORT
AM_CXXFLAGS += $(IPV6_SUPPORT_FLAG)
endif

if WITH_NOTIFICATION_SUPPORT
AM_CXXFLAGS += $(HAVE_VALUE_CHANGE_EVENT_FLAG)
endif

AM_CXXFLAGS += -I$(top_srcdir)/src/hostif/parodusClient/pal \
               -I$(top_srcdir)/src/hostif/parodusClient/waldb
libMsgHandlers_la_SOURCES += src/hostIf_NotificationHandler.cpp
include_HEADERS = include/hostIf_msgHandler.h \
                  include/hostIf_NotificationHandler.h
