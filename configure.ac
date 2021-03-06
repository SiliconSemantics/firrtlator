AC_PREREQ(2.60)
AC_INIT([firrtlator],
        [0.1],
        [wallento@silicon-semantics.com],
        [firrtlator],
        [http://www.firrtlator.org])
AC_CONFIG_SRCDIR([lib/include/Firrtlator.h])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([check-news foreign 1.11 -Wall -Wno-portability silent-rules tar-pax no-dist-gzip dist-xz subdir-objects])

FIRRTLATOR_CURRENT=0
FIRRTLATOR_REVISION=1
FIRRTLATOR_AGE=0
LTLDFLAGS="-version-info ${FIRRTLATOR_CURRENT}:${FIRRTLATOR_REVISION}:${FIRRTLATOR_AGE}"
AC_SUBST(LTLDFLAGS)

AC_USE_SYSTEM_EXTENSIONS
AC_PROG_CXX
AC_PROG_CXXCPP
AX_CXX_COMPILE_STDCXX_11
AC_SYS_LARGEFILE
AC_CONFIG_MACRO_DIR([m4])
AM_SILENT_RULES([yes])
LT_INIT([disable-static pic-only])
AC_PREFIX_DEFAULT([/usr])

BOOST_REQUIRE([1.58])

AM_CXXFLAGS="-Wall \
    -Wmissing-declarations -Wpointer-arith \
    -Wsign-compare -Wchar-subscripts -Wshadow \
    -Wformat-security -Wtype-limits \
    -pedantic -pedantic-errors -Werror\
    -fvisibility=hidden"
AC_SUBST([AM_CXXFLAGS])

AM_LDFLAGS="-Wl,--gc-sections \
    -Wl,--as-needed \
    -pthread \
    -lrt \
    ${libglip_LIBS}"
AC_SUBST(AM_LDFLAGS)

AC_CONFIG_HEADERS(config.h)
AC_CONFIG_FILES([
	firrtlator.pc
	Makefile
	lib/Makefile
	firrtlator/Makefile
])

AC_OUTPUT
AC_MSG_RESULT([
        $PACKAGE $VERSION
        =====

        prefix:                 ${prefix}
        libdir:                 ${libdir}
        includedir:             ${includedir}
        datarootdir:            ${datarootdir}
        datadir:                ${datadir}

        compiler:               ${CC}
        cflags:                 ${CFLAGS}
        ldflags:                ${LDFLAGS}
])
