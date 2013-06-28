AC_INIT([mintaka], [1.0.8], [david.rodenas@gmail.com])


dnl On estem ?
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET


dnl Volem un autoconf "contemporani"
AC_PREREQ(2.59)

dnl Inicialitzem Automake
AM_INIT_AUTOMAKE([dist-bzip2])

dnl Per comprovar que hi ha el codi
AC_CONFIG_SRCDIR(src/mintaka.h)

dnl Establim el config header
AC_CONFIG_HEADERS([config.h])


dnl Les proves en C
AC_LANG([C])

dnl Comprovem install
AC_PROG_INSTALL

dnl Comprovem que hi ha compilador
AC_PROG_CC

dnl Comprovem que hi ha preprocessador
AC_PROG_CPP

dnl Comprovem que hi ha c++
AC_PROG_CXX

dnl Ranlib, imperatiu per a libs
# AC_PROG_RANLIB

dnl Libtool :D
AC_PROG_LIBTOOL



AC_CONFIG_FILES([Makefile
                 src/Makefile
                 test/Makefile])

AC_OUTPUT
