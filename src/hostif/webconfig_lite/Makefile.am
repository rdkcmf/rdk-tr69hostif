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


lib_LTLIBRARIES = libwebconfiglite.la
AM_CXXFLAGS= -I$(top_srcdir)/src/hostif/parodusClient/pal \
            -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/libparodus \
            -I$(top_srcdir)/src/hostif/include  \
            -I$(top_srcdir)/src/hostif/parodusClient/waldb \
            -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/wrp-c \
            -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/glib-2.0 \
	    -I${PKG_CONFIG_SYSROOT_DIR}$(libdir)/glib-2.0/include \
            -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/wdmp-c 

libwebconfiglite_la_LDFLAGS= -lpthread -lcjson -ltrower-base64 -lwrp-c  -lm -luuid -lstdc++ -lcurl  -lrdkloggers
libwebconfiglite_la_LIBADD = $(top_builddir)/src/hostif/parodusClient/waldb/libwaldb.la
libwebconfiglite_la_LIBADD += $(top_builddir)/src/hostif/parodusClient/pal/libparodusclient.la
libwebconfiglite_la_SOURCES = webconfig_lite.cpp webconfig_utils.cpp 
#include_HEADERS=libpd.h webpa_adapter.h webpa_parameter.h 


