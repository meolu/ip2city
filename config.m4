dnl $Id$
dnl config.m4 for extension ip2city

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(ip2city, for ip2city support,
dnl Make sure that the comment is aligned:
dnl [  --with-ip2city             Include ip2city support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(ip2city, whether to enable ip2city support,
dnl Make sure that the comment is aligned:
dnl [  --enable-ip2city           Enable ip2city support])

if test "$PHP_IP2CITY" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-ip2city -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/ip2city.h"  # you most likely want to change this
  dnl if test -r $PHP_IP2CITY/$SEARCH_FOR; then # path given as parameter
  dnl   IP2CITY_DIR=$PHP_IP2CITY
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for ip2city files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       IP2CITY_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$IP2CITY_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the ip2city distribution])
  dnl fi

  dnl # --with-ip2city -> add include path
  dnl PHP_ADD_INCLUDE($IP2CITY_DIR/include)

  dnl # --with-ip2city -> check for lib and symbol presence
  dnl LIBNAME=ip2city # you may want to change this
  dnl LIBSYMBOL=ip2city # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $IP2CITY_DIR/$PHP_LIBDIR, IP2CITY_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_IP2CITYLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong ip2city lib version or lib not found])
  dnl ],[
  dnl   -L$IP2CITY_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(IP2CITY_SHARED_LIBADD)

  PHP_NEW_EXTENSION(ip2city, ip2city.c, $ext_shared)
fi
