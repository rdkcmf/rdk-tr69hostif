#
# ============================================================================
# RDK MANAGEMENT, LLC CONFIDENTIAL AND PROPRIETARY
# ============================================================================
# This file (and its contents) are the intellectual property of RDK Management, LLC.
# It may not be used, copied, distributed or otherwise  disclosed in whole or in
# part without the express written permission of RDK Management, LLC.
# ============================================================================
# Copyright (c) 2016 RDK Management, LLC. All rights reserved.
# Copyright (C) 2017 Broadcom. The term "Broadcom" refers to Broadcom Limited and/or its subsidiaries.
# ============================================================================
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
