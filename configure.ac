dnl File generated by Zed2Soft Project Manager for the Zed2Soft Science Environment
dnl Creation date: Thu Sep 21 19:57:21 2017

AC_PREREQ(2.65)

AC_INIT(lfsrcrack, 17.908997160)

AC_CONFIG_HEADERS(src/config.h)
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11.1])

cflags_were_set=false
if test "${CFLAGS+set}" = set; then
  GLOBAL_CFLAGS=$CFLAGS
  CFLAGS=
  cflags_were_set=:
fi

AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_AS

AC_PROG_LIBTOOL
AC_PROG_INSTALL
dnl the following needs to come after the expansion of AC_PROG_CC
if $cflags_were_set; then :; else
  GLOBAL_CFLAGS=$CFLAGS
  CFLAGS=
fi

AC_SUBST(AM_CFLAGS)

AC_C_INLINE

if test "$ac_cv_c_inline" != no ; then
  AC_DEFINE([HAVE_INLINE], [0], [This compiler supports inline functions])
  AC_DEFINE(HAVE_INLINE,1)
  AC_SUBST(HAVE_INLINE)
fi

AC_HEADER_TIME

dnl Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_MALLOC

GLOBAL_LDFLAGS="-lm -lpthread -ldl -export-dynamic -rdynamic"


dnl Macro snippets imported from dependency `util'
AC_SUBST(GLOBAL_CFLAGS)
AC_SUBST(GLOBAL_LDFLAGS)
AC_OUTPUT([
  Makefile
  src/Makefile
  util/Makefile
])
