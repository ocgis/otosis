dnl aclocal.m4 generated automatically by aclocal 1.3

dnl Copyright (C) 1994, 1995, 1996, 1997, 1998 Free Software Foundation, Inc.
dnl This Makefile.in is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.

# Do all the work for Automake.  This macro actually does too much --
# some checks are only needed if your package does certain things.
# But this isn't really a big deal.

# serial 1

dnl Usage:
dnl AM_INIT_AUTOMAKE(package,version, [no-define])

AC_DEFUN(AM_INIT_AUTOMAKE,
[AC_REQUIRE([AM_PROG_INSTALL])
PACKAGE=[$1]
AC_SUBST(PACKAGE)
VERSION=[$2]
AC_SUBST(VERSION)
dnl test to see if srcdir already configured
if test "`cd $srcdir && pwd`" != "`pwd`" && test -f $srcdir/config.status; then
  AC_MSG_ERROR([source directory already configured; run "make distclean" there first])
fi
ifelse([$3],,
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE")
AC_DEFINE_UNQUOTED(VERSION, "$VERSION"))
AC_REQUIRE([AM_SANITY_CHECK])
AC_REQUIRE([AC_ARG_PROGRAM])
dnl FIXME This is truly gross.
missing_dir=`cd $ac_aux_dir && pwd`
AM_MISSING_PROG(ACLOCAL, aclocal, $missing_dir)
AM_MISSING_PROG(AUTOCONF, autoconf, $missing_dir)
AM_MISSING_PROG(AUTOMAKE, automake, $missing_dir)
AM_MISSING_PROG(AUTOHEADER, autoheader, $missing_dir)
AM_MISSING_PROG(MAKEINFO, makeinfo, $missing_dir)
AC_REQUIRE([AC_PROG_MAKE_SET])])


# serial 1

AC_DEFUN(AM_PROG_INSTALL,
[AC_REQUIRE([AC_PROG_INSTALL])
test -z "$INSTALL_SCRIPT" && INSTALL_SCRIPT='${INSTALL_PROGRAM}'
AC_SUBST(INSTALL_SCRIPT)dnl
])

#
# Check to make sure that the build environment is sane.
#

AC_DEFUN(AM_SANITY_CHECK,
[AC_MSG_CHECKING([whether build environment is sane])
# Just in case
sleep 1
echo timestamp > conftestfile
# Do `set' in a subshell so we don't clobber the current shell's
# arguments.  Must try -L first in case configure is actually a
# symlink; some systems play weird games with the mod time of symlinks
# (eg FreeBSD returns the mod time of the symlink's containing
# directory).
if (
   set X `ls -Lt $srcdir/configure conftestfile 2> /dev/null`
   if test "[$]*" = "X"; then
      # -L didn't work.
      set X `ls -t $srcdir/configure conftestfile`
   fi
   if test "[$]*" != "X $srcdir/configure conftestfile" \
      && test "[$]*" != "X conftestfile $srcdir/configure"; then

      # If neither matched, then we have a broken ls.  This can happen
      # if, for instance, CONFIG_SHELL is bash and it inherits a
      # broken ls alias from the environment.  This has actually
      # happened.  Such a system could not be considered "sane".
      AC_MSG_ERROR([ls -t appears to fail.  Make sure there is not a broken
alias in your environment])
   fi

   test "[$]2" = conftestfile
   )
then
   # Ok.
   :
else
   AC_MSG_ERROR([newly created file is older than distributed files!
Check your system clock])
fi
rm -f conftest*
AC_MSG_RESULT(yes)])

dnl AM_MISSING_PROG(NAME, PROGRAM, DIRECTORY)
dnl The program must properly implement --version.
AC_DEFUN(AM_MISSING_PROG,
[AC_MSG_CHECKING(for working $2)
# Run test in a subshell; some versions of sh will print an error if
# an executable is not found, even if stderr is redirected.
# Redirect stdin to placate older versions of autoconf.  Sigh.
if ($2 --version) < /dev/null > /dev/null 2>&1; then
   $1=$2
   AC_MSG_RESULT(found)
else
   $1="$3/missing $2"
   AC_MSG_RESULT(missing)
fi
AC_SUBST($1)])

# Like AC_CONFIG_HEADER, but automatically create stamp file.

AC_DEFUN(AM_CONFIG_HEADER,
[AC_PREREQ([2.12])
AC_CONFIG_HEADER([$1])
dnl When config.status generates a header, we must update the stamp-h file.
dnl This file resides in the same directory as the config header
dnl that is generated.  We must strip everything past the first ":",
dnl and everything past the last "/".
AC_OUTPUT_COMMANDS(changequote(<<,>>)dnl
ifelse(patsubst(<<$1>>, <<[^ ]>>, <<>>), <<>>,
<<test -z "<<$>>CONFIG_HEADERS" || echo timestamp > patsubst(<<$1>>, <<^\([^:]*/\)?.*>>, <<\1>>)stamp-h<<>>dnl>>,
<<am_indx=1
for am_file in <<$1>>; do
  case " <<$>>CONFIG_HEADERS " in
  *" <<$>>am_file "*<<)>>
    echo timestamp > `echo <<$>>am_file | sed -e 's%:.*%%' -e 's%[^/]*$%%'`stamp-h$am_indx
    ;;
  esac
  am_indx=`expr "<<$>>am_indx" + 1`
done<<>>dnl>>)
changequote([,]))])

# Configure paths for oVDIsis
# Christer Gustavsson 98-09-20

dnl AM_PATH_OVDISIS([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for oVDIsis, and define OVDISIS_CFLAGS and OVDISIS_LIBS

AC_DEFUN(AM_PATH_OVDISIS,
[dnl 
dnl Get the cflags and libraries from the ovdisis-config script
dnl
AC_ARG_WITH(ovdisis-prefix,[  --with-ovdisis-prefix=PFX   Prefix where oVDIsis is installed (optional)],
            ovdisis_config_prefix="$withval", ovdisis_config_prefix="")
AC_ARG_WITH(ovdisis-exec-prefix,[  --with-ovdisis-exec-prefix=PFX Exec prefix where oVDIsis is installed (optional)],
            ovdisis_config_exec_prefix="$withval", ovdisis_config_exec_prefix="")
AC_ARG_ENABLE(ovdisistest, [  --disable-ovdisistest       Do not try to compile and run a test oVDIsis program],
		    , enable_ovdisistest=yes)

  if test x$ovdisis_config_exec_prefix != x ; then
     ovdisis_config_args="$ovdisis_config_args --exec-prefix=$ovdisis_config_exec_prefix"
     if test x${OVDISIS_CONFIG+set} != xset ; then
        OVDISIS_CONFIG=$ovdisis_config_exec_prefix/bin/ovdisis-config
     fi
  fi
  if test x$ovdisis_config_prefix != x ; then
     ovdisis_config_args="$ovdisis_config_args --prefix=$ovdisis_config_prefix"
     if test x${OVDISIS_CONFIG+set} != xset ; then
        OVDISIS_CONFIG=$ovdisis_config_prefix/bin/ovdisis-config
     fi
  fi

  AC_PATH_PROG(OVDISIS_CONFIG, ovdisis-config, no)
  min_ovdisis_version=ifelse([$1], ,0.0.6,$1)
  AC_MSG_CHECKING(for oVDIsis - version >= $min_ovdisis_version)
  no_ovdisis=""
  if test "$OVDISIS_CONFIG" = "no" ; then
    no_ovdisis=yes
  else
    OVDISIS_CFLAGS=`$OVDISIS_CONFIG $ovdisis_config_args --cflags`
    OVDISIS_LIBS=`$OVDISIS_CONFIG $ovdisis_config_args --libs`
    ovdisis_config_major_version=`$OVDISIS_CONFIG $ovdisis_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    ovdisis_config_minor_version=`$OVDISIS_CONFIG $ovdisis_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    ovdisis_config_micro_version=`$OVDISIS_CONFIG $ovdisis_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_ovdisistest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $OVDISIS_CFLAGS"
      LIBS="$LIBS $OVDISIS_LIBS"
     fi
  fi
  if test "x$no_ovdisis" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$OVDISIS_CONFIG" = "no" ; then
       echo "*** The ovdisis-config script installed by oVDIsis could not be found"
       echo "*** If oVDIsis was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the OVDISIS_CONFIG environment variable to the"
       echo "*** full path to ovdisis-config."
     else
        :
     fi
     OVDISIS_CFLAGS=""
     OVDISIS_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(OVDISIS_CFLAGS)
  AC_SUBST(OVDISIS_LIBS)
])

# Configure paths for oaesis
# Christer Gustavsson 98-09-20

dnl AM_PATH_OAESIS([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for oaesis, and define OAESIS_CFLAGS and OAESIS_LIBS

AC_DEFUN(AM_PATH_OAESIS,
[dnl 
dnl Get the cflags and libraries from the oaesis-config script
dnl
AC_ARG_WITH(oaesis-prefix,[  --with-oaesis-prefix=PFX   Prefix where oaesis is installed (optional)],
            oaesis_config_prefix="$withval", oaesis_config_prefix="")
AC_ARG_WITH(oaesis-exec-prefix,[  --with-oaesis-exec-prefix=PFX Exec prefix where oaesis is installed (optional)],
            oaesis_config_exec_prefix="$withval", oaesis_config_exec_prefix="")
AC_ARG_ENABLE(oaesistest, [  --disable-oaesistest       Do not try to compile and run a test oaesis program],
		    , enable_oaesistest=yes)

  if test x$oaesis_config_exec_prefix != x ; then
     oaesis_config_args="$oaesis_config_args --exec-prefix=$oaesis_config_exec_prefix"
     if test x${OAESIS_CONFIG+set} != xset ; then
        OAESIS_CONFIG=$oaesis_config_exec_prefix/bin/oaesis-config
     fi
  fi
  if test x$oaesis_config_prefix != x ; then
     oaesis_config_args="$oaesis_config_args --prefix=$oaesis_config_prefix"
     if test x${OAESIS_CONFIG+set} != xset ; then
        OAESIS_CONFIG=$oaesis_config_prefix/bin/oaesis-config
     fi
  fi

  AC_PATH_PROG(OAESIS_CONFIG, oaesis-config, no)
  min_oaesis_version=ifelse([$1], ,0.0.6,$1)
  AC_MSG_CHECKING(for oAESis - version >= $min_oaesis_version)
  no_oaesis=""
  if test "$OAESIS_CONFIG" = "no" ; then
    no_oaesis=yes
  else
    OAESIS_CFLAGS=`$OAESIS_CONFIG $oaesis_config_args --cflags`
    OAESIS_LIBS=`$OAESIS_CONFIG $oaesis_config_args --libs`
    oaesis_config_major_version=`$OAESIS_CONFIG $oaesis_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    oaesis_config_minor_version=`$OAESIS_CONFIG $oaesis_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    oaesis_config_micro_version=`$OAESIS_CONFIG $oaesis_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_oaesistest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $OAESIS_CFLAGS"
      LIBS="$LIBS $OAESIS_LIBS"
     fi
  fi
  if test "x$no_oaesis" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$OAESIS_CONFIG" = "no" ; then
       echo "*** The oaesis-config script installed by oaesis could not be found"
       echo "*** If oaesis was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the OAESIS_CONFIG environment variable to the"
       echo "*** full path to oaesis-config."
     else
        :
     fi
     OAESIS_CFLAGS=""
     OAESIS_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(OAESIS_CFLAGS)
  AC_SUBST(OAESIS_LIBS)
])

