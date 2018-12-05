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


lib_LTLIBRARIES = libparodusclient.la
AM_CXXFLAGS=  -I$(top_srcdir)/src/hostif/parodusClient/pal \
            -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/libparodus \
            -I$(top_srcdir)/src/hostif/include  \
            -I$(top_srcdir)/src/hostif/handlers/include \
            -I$(top_srcdir)/src/hostif/parodusClient/waldb \
            -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/wdmp-c \
            -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/wrp-c $(GLIB_CFLAGS)

libparodusclient_la_LDFLAGS= -llibparodus -lrdkloggers -lwdmp-c -lwrp-c  -lcjson -lpthread
libparodusclient_la_LIBADD = $(top_builddir)/src/hostif/parodusClient/waldb/libwaldb.la \
                             $(top_builddir)/src/hostif/handlers/libMsgHandlers.la 
libparodusclient_la_SOURCES = webpa_notification.cpp libpd.cpp webpa_parameter.cpp webpa_adapter.cpp webpa_attribute.cpp 
include_HEADERS=libpd.h 


