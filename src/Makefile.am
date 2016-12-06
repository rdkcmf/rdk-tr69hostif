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
#####################################################################

SUBDIRS = hostif/profiles hostif/handlers
DIST_SUBDIRS = hostif/profiles hostif/handlers

AM_CXXFLAGS = $(GLIB_CFLAGS) $(GTHREAD_CFLAGS) \
	$(SOUP_CFLAGS) $(DIRECTFB_CFLAGS) \
	-I$(top_srcdir)/src/hostif/include \
	-I$(top_srcdir)/src/hostif/handlers/include \
	-I$(top_srcdir)/src/hostif/profiles/STBService \
	-I$(top_srcdir)/src/hostif/profiles/Ethernet \
	-I$(top_srcdir)/src/hostif/profiles/IP \
	-I$(top_srcdir)/src/hostif/profiles/Time

if WITH_MOCA_PROFILE
NEXUS_LIB = -lnexus
AM_CXXFLAGS += -I$(top_srcdir)/src/hostif/profiles/moca/include
else
NEXUS_LIB =
endif

if WITH_WIFI_PROFILE
AM_CXXFLAGS += -DUSE_WIFI_PROFILE
AM_CXXFLAGS += -I$(top_srcdir)/src/hostif/profiles/wifi/include
endif
if WITH_DHCP_PROFILE
AM_CXXFLAGS += -DUSE_DHCPv4_PROFILE
AM_CXXFLAGS += -I$(top_srcdir)/src/hostif/profiles/DHCPv4
endif

if WITH_STORAGESERVICE_PROFILE
AM_CXXFLAGS += -DUSE_STORAGESERVICE_PROFILE
AM_CXXFLAGS += -I$(top_srcdir)/src/hostif/profiles/StorageService
endif

if WITH_INTFSTACK_PROFILE
AM_CXXFLAGS += -DUSE_INTFSTACK_PROFILE
AM_CXXFLAGS += -I$(top_srcdir)/src/hostif/profiles/InterfaceStack
endif

if XRELIB_FLAG
AM_LDFLAGS = $(GLIB_LDFLAGS) $(GLIB_LIBS) \
	$(G_THREAD_LIBS) -lyajl $(SOUP_LIBS) \
	$(NEXUS_LIB) -ldbus-1 -ldirect \
	-lfusion ${DIRECTFB_LIBS} -lIARMBus -lrdkloggers\
	-lds -ldshalcli -ltr69ProfileXcaliber
else
AM_LDFLAGS = $(GLIB_LDFLAGS) $(GLIB_LIBS) \
	$(G_THREAD_LIBS) -lyajl $(SOUP_LIBS) \
	$(NEXUS_LIB) -ldbus-1 -ldirect \
	-lfusion ${DIRECTFB_LIBS} -lIARMBus -lrdkloggers\
	-lds -ldshalcli
endif


AM_CFLAGS =  $(GLIB_CFLAGS) $(GTHREAD_CFLAGS)
AM_CPPFLAGS =  $(GLIB_CFLAGS) $(GTHREAD_CFLAGS)


# Start : Support for DeviceSetting client.
#ENABLE_WITH_DS_CLIENT=@DEF_ENABLE_WITH_DS_CLIENT@
#ifeq ($(ENABLE_WITH_DS_CLIENT),yes)
#        WITH_DS_CLIENT=TRUE
#endif


bin_PROGRAMS = tr69hostif
tr69hostif_SOURCES = $(top_srcdir)/src/hostif/src/hostIf_main.cpp $(top_srcdir)/src/hostif/src/hostIf_utils.cpp
tr69hostif_LDADD = $(top_builddir)/src/hostif/handlers/libMsgHandlers.la
tr69hostif_LDADD += $(top_builddir)/src/hostif/profiles/STBService/libstbservice.la
tr69hostif_LDADD += $(top_builddir)/src/hostif/profiles/IP/libhostIfIP.la
tr69hostif_LDADD += $(top_builddir)/src/hostif/profiles/Time/libhostIfTime.la
tr69hostif_LDADD += $(top_builddir)/src/hostif/profiles/Ethernet/libhostIfEthernet.la
tr69hostif_LDADD += $(top_builddir)/src/hostif/profiles/DeviceInfo/libdeviceinfo.la

if WITH_DHCP_PROFILE
tr69hostif_LDADD += $(top_builddir)/src/hostif/profiles/DHCPv4/libhostIfDHCPv4Client.la
endif

if WITH_INTFSTACK_PROFILE
tr69hostif_LDADD += $(top_builddir)/src/hostif/profiles/InterfaceStack/libhostIfInterfaceStack.la
endif

if WITH_STORAGESERVICE_PROFILE
tr69hostif_LDADD += $(top_builddir)/src/hostif/profiles/StorageService/libstorageservice.la
endif

if WITH_MOCA_PROFILE
tr69hostif_LDADD += $(top_builddir)/soc/hostif/profiles/moca/libhostIfMoCA.la
endif

if XRELIB_FLAG
tr69hostif_LDADD += -ltr69ProfileXcaliber
endif

if WITH_WIFI_PROFILE
tr69hostif_LDADD += $(top_builddir)/soc/hostif/profiles/wifi/libhostIfWiFi.la
endif
