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
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([hostIfMgr],[1.0],[rajnish_dey2@cable.comcast.com])
AC_CONFIG_SRCDIR([src/hostif/src/hostIf_main.cpp])

AC_CONFIG_MACRO_DIR([cfg])
AC_CONFIG_AUX_DIR([cfg])
AM_INIT_AUTOMAKE([foreign no-dist-gzip dist-bzip2 1.9])
#AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_HEADERS([cfg/config.h])

LT_INIT

XREMGR_FLAGS=" "
MOCAMGR_FLAGS=" "
SUBDIRS_MOCA=" "
SUBDIRS_WIFI=" "
MOCA2_FLAG=" "
WIFI_PROFILE_FLAGS=" "
XRDK_SDCARD_PROFILE_FLAG=" "
SUBDIRS_STORAGESERVICE=" "
SUBDIRS_INTFSTACK=" "
SUBDIRS_DHCPv4=" "
DHCPv4_PROFILE_FLAG=" "
STORAGESERVICE_PROFILE_FLAG=" "
INTFSTACK_PROFILE_FLAG=" "
XRDK_RF4CE_PROFILE_FLAG=" "
XRDK_BT_PROFILE_FLAG=" "
HAVE_VALUE_CHANGE_EVENT_FLAG=" "
SNMP_ADAPTER_FLAG=" "
SYSTEMD_SDNOTIFY_CFLAGS=" "
SYSTEMD_SDNOTIFY_LDFLAGS=" "

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])],
[AM_DEFAULT_VERBOSITY=1
AC_SUBST(AM_DEFAULT_VERBOSITY)]) 

AC_ARG_ENABLE([xre],
	[ AS_HELP_STRING([--enable-xre], [enable xre profile (default is no)])],
        [
        case "${enableval}" in
	yes) XREMGR_FLAGS="-DUSE_XRESRC"
	     xre=true;;
	no)  AC_MSG_ERROR([xre profile is disabled])
             xre=false;;
	*) AC_MSG_ERROR([bad value ${enableval} for --enable-xre])
           xre=false;;
	esac
	],
        [xre=false; echo "xre profile is disabled"])
AM_CONDITIONAL([XRELIB_FLAG], [test "x$xre" = xtrue])

# This adds the option of compiling with/without using the  profile source code
AC_ARG_ENABLE([moca],
	[AS_HELP_STRING([--enable-moca],[enable moca profile (default is yes)])],
	[
	AC_MSG_NOTICE([moca11 profile is Enabled])
	MOCAMGR_FLAGS="-DUSE_MoCA_PROFILE"
	enable_moca=yes
	],
	[
	AC_MSG_NOTICE([moca11 profile is disabled])])



AC_ARG_ENABLE([moca2],
	[AS_HELP_STRING([--enable-moca2],[enable moca20 profile (default is yes)])],
	[
	AC_MSG_NOTICE([moca2 profile is Enabled])
	MOCAMGR_FLAGS="-DUSE_MoCA_PROFILE"
	enable_moca=yes
        enable_moca2=yes
        MOCA2_FLAG="-DUSE_MOCA20"
	],
	[
	AC_MSG_NOTICE([moca20 profile is disabled])])


m4_syscmd([test -d soc/hostif/profiles/moca])
m4_if(m4_sysval,[0],[AC_CONFIG_FILES([soc/hostif/profiles/moca/Makefile])])
m4_if(m4_sysval,[0],[SUBDIRS_MOCA="soc/hostif/profiles/moca"])

AC_ARG_ENABLE([wifi],
        AS_HELP_STRING([--enable-wifi],[enable WiFi profile (default is no)]),
        [
          case "${enableval}" in
           yes) WIFI_PROFILE_ENABLE=true 
                WIFI_PROFILE_FLAG="-DUSE_WIFI_PROFILE"
		m4_syscmd([test -d soc/hostif/profiles/wifi])
		m4_if(m4_sysval,[0],[AC_CONFIG_FILES([soc/hostif/profiles/wifi/Makefile])])
		m4_if(m4_sysval,[0],[SUBDIRS_WIFI="soc/hostif/profiles/wifi"])  ;;
           no) WIFI_PROFILE_ENABLE=false AC_MSG_ERROR([wifi profile is disabled]) ;;
          *) AC_MSG_ERROR([bad value ${enableval} for --enable-wifi ]) ;;
           esac
           ],
         [echo "WiFi profile is disabled"])

AC_ARG_ENABLE([sdcard],
        AS_HELP_STRING([--enable-sdcard],[enable X_RDKCENTRAL_COM SDcard profile (default is no)]),
        [
          case "${enableval}" in
           yes) XRDK_SDCARD_PROFILE_ENABLE=true
                XRDK_SDCARD_PROFILE_FLAG="-DUSE_XRDK_SDCARD_PROFILE" ;;
           no)  XRDK_SDCARD_PROFILE_ENABLE=false
                [echo "X_RDKCENTRAL_COM SDcard profile is disabled"] ;;
          *) AC_MSG_ERROR([bad value ${enableval} for --enable-sdcard ]) ;;
           esac
           ],
         [echo "X_RDKCENTRAL_COM SDcard  profile is disabled"])

AC_ARG_ENABLE([DHCPv4],
        AS_HELP_STRING([--enable-DHCPv4],[enable DHCPv4 profile (default is no)]),
        [
          case "${enableval}" in
           yes) DHCPv4_PROFILE_ENABLE=true
                DHCPv4_PROFILE_FLAG="-DUSE_DHCPv4_PROFILE"
                m4_syscmd([test -d src/hostif/profiles/DHCPv4])
                m4_if(m4_sysval,[0],[AC_CONFIG_FILES([src/hostif/profiles/DHCPv4/Makefile])])
                m4_if(m4_sysval,[0],[SUBDIRS_DHCPv4="src/hostif/profiles/DHCPv4"])  ;;
           no) DHCPv4_PROFILE_ENABLE=false AC_MSG_ERROR([DHCPv4 profile is disabled]) ;;
          *) AC_MSG_ERROR([bad value ${enableval} for --enable-DHCPv4 ]) ;;
           esac
           ],
         [echo "DHCPv4 profile is disabled"])

AC_ARG_ENABLE([StorageService],
        AS_HELP_STRING([--enable-StorageService],[enable StorageService profile (default is no)]),
        [
          case "${enableval}" in
           yes) STORAGESERVICE_PROFILE_ENABLE=true
                STORAGESERVICE_PROFILE_FLAG="-DUSE_STORAGESERVICE_PROFILE"
                m4_syscmd([test -d src/hostif/profiles/StorageService])
                m4_if(m4_sysval,[0],[AC_CONFIG_FILES([src/hostif/profiles/StorageService/Makefile])])
                m4_if(m4_sysval,[0],[SUBDIRS_STORAGESERVICE="src/hostif/profiles/StorageService"])  ;;
           no) STORAGESERVICE_PROFILE_ENABLE=false AC_MSG_ERROR([StorageService profile is disabled]) ;;
          *) AC_MSG_ERROR([bad value ${enableval} for .enable- StorageService ]) ;;
           esac
           ],
         [echo "StorageService profile is disabled"])
		 
AC_ARG_ENABLE([InterfaceStack],
        AS_HELP_STRING([--enable-InterfaceStack],[enable interfacestack profile (default is no)]),
        [
          case "${enableval}" in
           yes) INTFSTACK_PROFILE_ENABLE=true
                INTFSTACK_PROFILE_FLAG="-DUSE_INTFSTACK_PROFILE"
                m4_syscmd([test -d src/hostif/profiles/InterfaceStack])
                m4_if(m4_sysval,[0],[AC_CONFIG_FILES([src/hostif/profiles/InterfaceStack/Makefile])])
                m4_if(m4_sysval,[0],[SUBDIRS_INTFSTACK="src/hostif/profiles/InterfaceStack"])  ;;
           no) INTFSTACK_PROFILE_ENABLE=false AC_MSG_ERROR([InterfaceStack profile is disabled]) ;;
          *) AC_MSG_ERROR([bad value ${enableval} for .enable-InterfaceStack ]) ;;
           esac
           ],
         [echo "InterfaceStack profile is disabled"])
AC_ARG_ENABLE([IPv6],
        AS_HELP_STRING([--enable-IPv6],[enable IPv6 support in ip profile (default is no)]),
        [
          case "${enableval}" in
           yes) IPV6_SUPPORT_ENABLE=true
                IPV6_SUPPORT_FLAG="-DIPV6_SUPPORT" ;;
           no) IPV6_SUPPORT_ENABLE=false AC_MSG_ERROR([IPv6 support is disabled]) ;;
          *) AC_MSG_ERROR([bad value ${enableval} for --enable-IPv6 ]) ;;
           esac
           ],
         [echo "IPv6 support is disabled"])


AC_ARG_ENABLE([notification],
        AS_HELP_STRING([--enable-notification],[enable notification support support for all profiles (default is no)]),
        [
          case "${enableval}" in
           yes) HAVE_VALUE_CHANGE_EVENT_ENABLE=true
                HAVE_VALUE_CHANGE_EVENT_FLAG="-DHAVE_VALUE_CHANGE_EVENT" ;;
           no) HAVE_VALUE_CHANGE_EVENT_ENABLE=false AC_MSG_ERROR([Notification support is disabled]) ;;
          *) AC_MSG_ERROR([bad value ${enableval} for --enable-notification ]) ;;
           esac
           ],
         [echo "Notification support is disabled"])


AC_ARG_ENABLE([rf4ce],
        AS_HELP_STRING([--enable-rf4ce],[enable X_RDKCENTRAL_COM RF4CE profile (default is no)]),
        [
          case "${enableval}" in
           yes) XRDK_RF4CE_PROFILE_ENABLE=true
                XRDK_RF4CE_PROFILE_FLAG="-DUSE_XRDK_RF4CE_PROFILE" ;;	
           no) XRDK_RF4CE_PROFILE_ENABLE=false AC_MSG_ERROR([X_RDKCENTRAL_COM RF4CE profile is disabled]) ;;
          *) AC_MSG_ERROR([bad value ${enableval} for --enable-rf4ce ]) ;;
           esac
           ],
         [echo "X_RDKCENTRAL_COM RF4CE  profile is disabled"])

AC_ARG_ENABLE([bt],
        AS_HELP_STRING([--enable-bt],[enable X_RDKCENTRAL_COM BlueTooth profile (default is no)]),
        [
          case "${enableval}" in
           yes) XRDK_BT_PROFILE_ENABLE=true
                XRDK_BT_PROFILE_FLAG="-DUSE_XRDK_BT_PROFILE" ;;
           no) XRDK_BT_PROFILE_ENABLE=false AC_MSG_ERROR([X_RDKCENTRAL_COM BlueTooth profile is disabled]) ;;
          *) AC_MSG_ERROR([bad value ${enableval} for --enable-bt ]) ;;
           esac
           ],
         [echo "X_RDKCENTRAL_COM BlueTooth profile is disabled"])

AC_ARG_ENABLE([webpaRFC],
        AS_HELP_STRING([--enable-webpaRFC],[enable WEBPA_RFC profile (default is no)]),
        [
          case "${enableval}" in
           yes) WEBPA_RFC_ENABLE=true
                WEBPA_RFC_FLAG="-DWEBPA_RFC_ENABLED" ;;
           no) WEBPA_RFC_ENABLE=false AC_MSG_ERROR([WEBPA_RFC is disabled]) ;;
          *) AC_MSG_ERROR([bad value ${enableval} for --enable-webpaRFC ]) ;;
           esac
           ],
         [echo "WEBPA_RFC is disabled"])

AC_ARG_ENABLE([snmpAdapter],
        AS_HELP_STRING([--enable-snmpAdapter],[enable SNMP_ADAPTER (default is no)]),
        [
          case "${enableval}" in
           yes) SNMP_ADAPTER_ENABLE=true
                SNMP_ADAPTER_FLAG="-DSNMP_ADAPTER_ENABLED"
                m4_syscmd([test -d src/hostif/snmpAdapter])
                m4_if(m4_sysval,[0],[AC_CONFIG_FILES([src/hostif/snmpAdapter/Makefile])])
                m4_if(m4_sysval,[0],[SUBDIRS_SNMPADAPTER="src/hostif/snmpAdapter"])  ;;
           no) SNMP_ADAPTER_ENABLE=false AC_MSG_ERROR([SNMP_ADAPTER is disabled]) ;;
          *) AC_MSG_ERROR([bad value ${enableval} for --enable-snmpAdapter ]) ;;
           esac
           ],
         [echo "SNMP_ADAPTER profile is disabled"])

AC_ARG_ENABLE([systemd-notify],
        AS_HELP_STRING([--enable-systemd-notify],[enable systemd sd notify]),
        [
                case "${enableval}" in
                yes) SYSTEMD_SDNOTIFY_CFLAGS="-DENABLE_SD_NOTIFY"
                     SYSTEMD_SDNOTIFY_LDFLAGS="-lsystemd" ;;
                no) AC_MSG_ERROR([systemd sd notify  is disabled]) ;;
                *) AC_MSG_ERROR([bad value ${enableval} for --enable-systemd ]) ;;
                esac
                ],
        [echo "systemd sd notify  is disable"])



AM_CONDITIONAL([WITH_MOCA_PROFILE], [test "x$enable_moca" = "xyes"])
AM_CONDITIONAL([WITH_MOCA20], [test "x$enable_moca2" = "xyes"])
AM_CONDITIONAL([WITH_WIFI_PROFILE], [test x$WIFI_PROFILE_ENABLE = xtrue])
AM_CONDITIONAL([WITH_XRDK_SDCARD_PROFILE], [test x$XRDK_SDCARD_PROFILE_ENABLE = xtrue])
AM_CONDITIONAL([WITH_DHCP_PROFILE], [test x$DHCPv4_PROFILE_ENABLE = xtrue])
AM_CONDITIONAL([WITH_INTFSTACK_PROFILE], [test x$INTFSTACK_PROFILE_ENABLE = xtrue])
AM_CONDITIONAL([WITH_STORAGESERVICE_PROFILE], [test x$STORAGESERVICE_PROFILE_ENABLE = xtrue])
AM_CONDITIONAL([WITH_IPV6_SUPPORT], [test x$IPV6_SUPPORT_ENABLE = xtrue])
AM_CONDITIONAL([WITH_XRDK_RF4CE_PROFILE], [test x$XRDK_RF4CE_PROFILE_ENABLE = xtrue])
AM_CONDITIONAL([WITH_XRDK_BT_PROFILE], [test x$XRDK_BT_PROFILE_ENABLE = xtrue])
AM_CONDITIONAL([WITH_WEBPA_RFC], [test x$WEBPA_RFC_ENABLE = xtrue])
AM_CONDITIONAL([WITH_NOTIFICATION_SUPPORT], [test x$HAVE_VALUE_CHANGE_EVENT_ENABLE = xtrue])
AM_CONDITIONAL([WITH_SNMP_ADAPTER], [test x$SNMP_ADAPTER_ENABLE = xtrue])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h unistd.h])
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.24.0])
PKG_CHECK_MODULES([GTHREAD], [gthread-2.0 >= 2.24.0])
PKG_CHECK_MODULES([DIRECTFB], [directfb >= 1.4.3])
PKG_CHECK_MODULES([SOUP], [libsoup-2.4 >= 2.31.6])

AC_CHECK_LIB(gthread-2.0, g_thread_init)
# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for Storage Manager header file.
AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([rdkStorageMgr.h], [HAVE_RDK_STMGR_V2_H=yes], [HAVE_RDK_STMGR_V2_H=no], [])
AC_LANG_POP
AM_CONDITIONAL([ENABLE_RDK_STMGR_V2], [test "x$HAVE_RDK_STMGR_V2_H" = "xyes" -a "x$XRDK_SDCARD_PROFILE_ENABLE" = "xtrue"])

# Checks for library functions.
AC_FUNC_MALLOC

AC_SUBST(PROFILE_SRC)
AC_SUBST(XREMGR_FLAGS)
AC_SUBST(MOCAMGR_FLAGS)
AC_SUBST(SUBDIRS_MOCA)
AC_SUBST(SUBDIRS_WIFI)
AC_SUBST(SUBDIRS_STORAGESERVICE)
AC_SUBST(SUBDIRS_DHCPv4)
AC_SUBST(SUBDIRS_INTFSTACK)
AC_SUBST(MOCA2_FLAG)
AC_SUBST(WIFI_PROFILE_ENABLE)
AC_SUBST(WIFI_PROFILE_FLAG)
AC_SUBST(XRDK_SDCARD_PROFILE_FLAG)
AC_SUBST(DHCPv4_PROFILE_FLAG)
AC_SUBST(STORAGESERVICE_PROFILE_FLAG)
AC_SUBST(INTFSTACK_PROFILE_FLAG)
AC_SUBST(XRDK_RF4CE_PROFILE_FLAG)
AC_SUBST(XRDK_BT_PROFILE_FLAG)
AC_SUBST(IPV6_SUPPORT_FLAG)
AC_SUBST(HAVE_VALUE_CHANGE_EVENT_FLAG)
AC_SUBST(SNMP_ADAPTER_FLAG)
AC_SUBST(SYSTEMD_SDNOTIFY_CFLAGS)
AC_SUBST(SYSTEMD_SDNOTIFY_LDFLAGS)

# files to generate via autotools (.am or .in source files)
AC_CONFIG_FILES([Makefile])


AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/hostif/handlers/Makefile])
AC_CONFIG_FILES([src/hostif/profiles/Makefile ])
AC_CONFIG_FILES([src/hostif/profiles/STBService/Makefile])
AC_CONFIG_FILES([src/hostif/profiles/DeviceInfo/Makefile])
AC_CONFIG_FILES([src/hostif/profiles/Ethernet/Makefile])
AC_CONFIG_FILES([src/hostif/profiles/IP/Makefile])
AC_CONFIG_FILES([src/hostif/profiles/Time/Makefile])
AC_CONFIG_FILES([src/tr69BusAgent/Makefile])

AC_OUTPUT
