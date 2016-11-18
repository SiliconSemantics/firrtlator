
pkginclude_HEADERS = include/Firrtlator.h include/IR.h
lib_LTLIBRARIES = libfirrtlator.la

libfirrtlator_la_SOURCES = \
    src/Firrtlator.cpp \
    src/IRNode.cpp \
    src/Circuit.cpp \
    src/Module.cpp \
    src/Expressions.cpp \
	frontends/generic/src/Frontends.cpp \
	frontends/firrtl/src/FirrtlFrontend.cpp \
	backends/firrtl/src/FirrtlBackend.cpp \
	backends/generic/src/IndentationBuffer.cpp

libfirrtlator_la_LDFLAGS = $(AM_LDFLAGS) $(LTLDFLAGS)
libfirrtlator_la_CPPFLAGS = $(AM_CPPFLAGS) -include $(top_builddir)/config.h \
	-I $(srcdir)/include \
	-I $(srcdir)/frontends \
	-I $(srcdir)/frontends/firrtl/include \
	-I $(srcdir)/backends \
	-I $(srcdir)/backends/firrtl/include \
	-I $(srcdir)/backends/generic/include 
