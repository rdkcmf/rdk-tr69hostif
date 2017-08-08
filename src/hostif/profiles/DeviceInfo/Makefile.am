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
-I$(top_srcdir)/src/hostif/handlers/include \
-I$(top_srcdir)/soc/hostif/profiles/moca \
-I$(top_srcdir)/src/hostif/profiles/X_COMCAST-COM/X_COMCAST-COM_Xcalibur/include \
-I$(top_srcdir)/src/hostif/profiles/moca/include \
$(GLIB_CFLAGS) $(G_THREAD_CFLAGS) $(XREMGR_FLAGS) $(MOCAMGR_FLAGS) $(PROCPS_CFLAGS) \
-I=/usr/include/rdk/iarmbus/ \
-I=/usr/include/rdk/iarmmgrs/mfr/ \
-I=/usr/include/rdk/iarmmgrs-hal/

AM_LDFLAGS = $(PROCPS_LIBS) -lIARMBus -lyajl -ldirect -lfusion -lds -ldshalcli -ldbus-1

if WITH_XRDK_BT_PROFILE
AM_CXXFLAGS += $(XRDK_BT_PROFILE_FLAG)
AM_LDFLAGS += -lBTMgr
endif

if WITH_HWSELFTEST_PROFILE
AM_CXXFLAGS += $(HWSELFTEST_PROFILE_FLAG)
endif

if WITH_TELEMETRY_LOGGER
AM_CXXFLAGS += $(TELEMETRY_LOGGER_FLAG)
endif

noinst_LTLIBRARIES = libdeviceinfo.la
libdeviceinfo_la_SOURCES = 	Device_DeviceInfo.cpp\
				Device_DeviceInfo_Processor.cpp\
				Device_DeviceInfo_ProcessStatus_Process.cpp \
				Device_DeviceInfo_ProcessStatus.cpp
if WITH_XRDK_BT_PROFILE
libdeviceinfo_la_SOURCES +=	XrdkBlueTooth.cpp
endif
libdeviceinfo_la_CXXFLAGS = $(AM_CXXFLAGS) -std=c++0x
