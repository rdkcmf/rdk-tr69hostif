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
    -I$(top_srcdir)/src/hostif/profiles/DeviceInfo \
    $(GLIB_CFLAGS) $(G_THREAD_CFLAGS)


AM_LDFLAGS = " "

if IS_YOCTO_ENABLED
AM_LDFLAGS += -lsecure_wrapper
endif

if WITH_WEB_CONFIG
AM_CXXFLAGS += -DWEB_CONFIG_ENABLED
AM_LDFLAGS += -lwebcfg -lcurl -luuid
AM_CXXFLAGS += -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/wdmp-c
AM_CXXFLAGS += -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/webcfg
AM_CXXFLAGS += -I$(top_srcdir)/src/hostif/webcfg/
endif

noinst_LTLIBRARIES = libdevice.la
libdevice_la_SOURCES = x_rdk_profile.cpp
libdevice_la_CXXFLAGS = $(AM_CXXFLAGS) -std=c++0x
