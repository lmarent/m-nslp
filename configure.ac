#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(mnslp-ua, 0.97, la.marentes455@uniandes.edu.co)
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_CPP

AC_CHECK_LIB([cppunit], [main])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h netdb.h netinet/in.h stdlib.h string.h sys/param.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MEMCMP
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([gethostname gettimeofday memset socket strerror])

AC_PROG_RANLIB

#check for unordered map
[SAVECXXFLAGS=$CXXFLAGS; CXXFLAGS="-std=c++0x"]
AC_LANG_CPLUSPLUS

AC_CACHE_CHECK([whether unordered_map is available],
			[ac_cv_unordered_map_exists],
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <unordered_map>]],
				[[std::unordered_map<int,int> testmap;]])],
				[ac_cv_unordered_map_exists=yes],
				[ac_cv_unordered_map_exists=no])])
[CXXFLAGS=$SAVECXXFLAGS]


PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([LIBXML], [libxml-2.0])
PKG_CHECK_MODULES([LIBXML2], [libxml-2.0])

# Checks for libxslt libraries
LIBXSLT_REQUIRED=1.1.10
PKG_CHECK_MODULES(LIBXSLT, libxslt >= $LIBXSLT_REQUIRED)
AC_SUBST(LIBXSLT_CFLAGS)
AC_SUBST(LIBXSLT_LIBS)

# Check CURL, needed for install netmate policy rules.
CURL_LIBS=
PKG_CHECK_MODULES(CURL, 
  libcurl, 
  [AC_DEFINE([HAVE_CURL],[1],[Use CURL])])
AM_CONDITIONAL(CURL, test -n "$CURL_LIBS")
AC_SUBST(CURL_CFLAGS)
AC_SUBST(CURL_LIBS)


PKG_CHECK_MODULES([LIBUUID], [libuuid],
   [LIBUUID_FOUND=yes], [LIBUUID_FOUND=no])
   if test "$LIBUUID_FOUND" = "no" ; then
        PKG_CHECK_MODULES([LIBUUID], [uuid],
			      [LIBUUID_FOUND=yes], [LIBUUID_FOUND=no])
	    if test "$LIBUUID_FOUND" = "no" ; then
	  	  AC_MSG_ERROR([libuuid development files required])
	    else
		  LIBUUID_INCLUDEDIR=$(pkg-config --variable=includedir uuid)
		  LIBUUID_CFLAGS+=" -I$LIBUUID_INCLUDEDIR/uuid "
	    fi
   fi
AC_SUBST([LIBUUID_CFLAGS])
AC_SUBST([LIBUUID_LIBS])


AC_ARG_ENABLE([sctp],
	[AS_HELP_STRING([--enable-sctp], [include SCTP transport capability (default: enabled)])],
	[enable_sctp=$enableval],
	[enable_sctp=no])
AM_CONDITIONAL(USE_WITH_SCTP, test "$enable_sctp" = yes)

AM_CONDITIONAL(NSIS_NO_WARN_HASHMAP, test "$ac_cv_unordered_map_exists" = yes)

AC_CONFIG_FILES([Makefile
                 src/msg/Makefile
                 src/Makefile
                 test/Makefile])


AC_OUTPUT
