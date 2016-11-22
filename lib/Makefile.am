
pkginclude_HEADERS = include/Firrtlator.h include/IR.h
lib_LTLIBRARIES = libfirrtlator.la

libfirrtlator_la_SOURCES = \
    src/Firrtlator.cpp \
    src/Visitor.cpp \
    ir/src/Circuit.cpp \
    ir/src/Expression.cpp \
    ir/src/IRNode.cpp \
    ir/src/Memory.cpp \
    ir/src/Module.cpp \
    ir/src/Parameter.cpp \
    ir/src/Port.cpp \
    ir/src/Stmt.cpp \
    ir/src/Type.cpp \
	frontends/generic/src/Frontends.cpp \
	frontends/firrtl/src/FirrtlFrontend.cpp \
	backends/generic/src/Backends.cpp \
	backends/firrtl/src/FirrtlBackend.cpp \
	backends/dot/src/DotBackend.cpp \
	backends/tree/src/TreeBackend.cpp \
	backends/generic/src/StreamIndentation.cpp

libfirrtlator_la_LDFLAGS = $(AM_LDFLAGS) $(LTLDFLAGS)
libfirrtlator_la_CPPFLAGS = $(AM_CPPFLAGS) $(BOOST_CPPFLAGS) \
	-include $(top_builddir)/config.h \
	-I $(srcdir)/include \
	-I $(srcdir)/frontends \
	-I $(srcdir)/frontends/firrtl/include \
	-I $(srcdir)/backends \
	-I $(srcdir)/backends/generic/include \
	-I $(srcdir)/backends/firrtl/include \
	-I $(srcdir)/backends/tree/include
libfirrtlator_la_CXXFLAGS = $(AM_CXXFLAGS)
