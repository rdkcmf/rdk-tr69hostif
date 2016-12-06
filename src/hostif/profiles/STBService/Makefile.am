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


AM_CXXFLAGS = -I$(top_srcdir)/src/hostif/include -I$(top_srcdir)/src/hostif/handlers/include -I./include $(GLIB_CFLAGS) $(G_THREAD_CFLAGS) \
-I=/usr/include/rdk/ds-hal/ \
-I=/usr/include/rdk/ds/

if WITH_XRDK_SDCARD_PROFILE
AM_CXXFLAGS += $(XRDK_SDCARD_PROFILE_FLAG)
endif

if WITH_XRDK_RF4CE_PROFILE
AM_CXXFLAGS += $(XRDK_RF4CE_PROFILE_FLAG)
endif


noinst_LTLIBRARIES = libstbservice.la
libstbservice_la_SOURCES =	Components_AudioOutput.cpp \
				Components_SPDIF.cpp \
				Components_HDMI.cpp \
				Components_DisplayDevice.cpp \
				Components_VideoOutput.cpp \
				Components_VideoDecoder.cpp 
if WITH_XRDK_SDCARD_PROFILE
libstbservice_la_SOURCES += Components_XrdkSDCard.cpp
endif

