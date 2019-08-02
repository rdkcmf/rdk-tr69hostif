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

AM_CXXFLAGS = -I$(top_srcdir)/src/hostif/include -I$(top_srcdir)/src/hostif/handlers/include  $(GLIB_CFLAGS) $(G_THREAD_CFLAGS) $(YOCTO_FLAG)

if WITH_IPV6_SUPPORT
AM_CXXFLAGS += $(IPV6_SUPPORT_FLAG)
endif

if WITH_SPEEDTEST_SUPPORT
AM_CXXFLAGS += $(SPEEDTEST_SUPPORT_FLAG)
endif

noinst_LTLIBRARIES = libhostIfIP.la
libhostIfIP_la_SOURCES =  Device_IP_ActivePort.cpp \
				Device_IP.cpp \
				Device_IP_Interface.cpp \
				Device_IP_Interface_IPv4Address.cpp \
				Device_IP_Interface_Stats.cpp \
				Device_IP_Diagnostics_IPPing.cpp

if IS_YOCTO_ENABLED
libhostIfIP_la_LDFLAGS = -lsecure_wrapper
endif
if WITH_IPV6_SUPPORT
libhostIfIP_la_SOURCES += Device_IP_Interface_IPv6Address.cpp
endif

if WITH_SPEEDTEST_SUPPORT
libhostIfIP_la_SOURCES += Device_IP_Diagnostics_SpeedTest.cpp
endif

