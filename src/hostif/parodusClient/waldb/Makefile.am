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
noinst_LTLIBRARIES=libwaldb.la
libwaldb_la_SOURCES=waldb.cpp
libwaldb_la_LDFLAGS=-ltinyxml -lIARMBus
AM_CXXFLAGS = -I$(top_srcdir)/include \
              -I${PKG_CONFIG_SYSROOT_DIR}$(includedir)/wdmp-c \
              -I$(top_srcdir)/src/hostif/parodusClient/waldb \
              -I$(top_srcdir)/src/hostif/parodusClient/pal $(GLIB_CFLAGS)
