##########################################################################
# If not stated otherwise in this file or this component's Licenses.txt
# file the following copyright and licenses apply:
#
# Copyright 2017 RDK Management
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

noinst_LTLIBRARIES=libhttpserver.la
                                                                       
libhttpserver_la_SOURCES=src/http_server.cpp src/request_handler.cpp src/XrdkCentralComRFCVar.cpp

libhttpserver_la_LDFLAGS = -lrdkloggers -lwdmp-c $(SOUP_LIBS) -lcjson 

AM_CXXFLAGS = "-std=c++11" $(SOUP_CFLAGS) -I$(top_srcdir)/src/hostif/httpserver/include \
              -I$(top_srcdir)/src/hostif/include \
              -I$(top_srcdir)/src/hostif/handlers/include \
              -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/c++ \
              -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/wdmp-c \
              -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/cjson \
	      	  -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/glib-2.0 \
	      	  -I${PKG_CONFIG_SYSROOT_DIR}$(libdir)/glib-2.0/include \
              -I$(top_srcdir)/src/hostif/parodusClient/waldb \
              -I$(top_srcdir)/src/hostif/parodusClient/pal

libhttpserver_la_LIBADD = $(top_builddir)/src/hostif/parodusClient/waldb/libwaldb.la \
                             $(top_builddir)/src/hostif/handlers/libMsgHandlers.la 




                            
