#
# If not stated otherwise in this file or this component's Licenses.txt file the
# following copyright and licenses apply:
#
# Copyright 2018 RDK Management
# Copyright 2018 Broadcom Inc.
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
#

SUBDIRS = 

AM_CXXFLAGS = -I$(top_srcdir)/src/hostif/include \
	-I$(top_srcdir)/src/hostif/handlers/include \
	-I$(top_srcdir)/src/hostif/profiles/moca \
	$(GLIB_CFLAGS) $(G_THREAD_CFLAGS) $(MOCA2_FLAG)



if WITH_MOCA_PROFILE
noinst_LTLIBRARIES = libhostIfMoCA.la
libhostIfMoCA_la_SOURCES = 	Device_MoCA_Interface.cpp \
				Device_MoCA_Interface_Stats.cpp \
				Device_MoCA_Interface_AssociatedDevice.cpp \
				Device_MoCA_Interface_QoS.cpp \
				Device_MoCA_Interface_QoS_FlowStats.cpp \
				Device_MoCA_Interface_X_RDKCENTRAL_COM_MeshTable.cpp
libhostIfMoCA_la_LDFLAGS=-lrdkmocahal
endif
