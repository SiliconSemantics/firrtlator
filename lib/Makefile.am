
pkginclude_HEADERS = include/Firrtlator.h include/IR.h
lib_LTLIBRARIES = libfirrtlator.la

libfirrtlator_la_SOURCES = \
    src/Circuit.cpp \
    src/Expression.cpp \
    src/IRNode.cpp \
    src/Memory.cpp \
    src/Module.cpp \
    src/Firrtlator.cpp \
    src/Parameter.cpp \
    src/Port.cpp \
    src/Stmt.cpp \
    src/Type.cpp \
    src/Visitor.cpp \
	frontends/generic/src/Frontends.cpp \
	frontends/firrtl/src/FirrtlFrontend.cpp \
	backends/generic/src/Backends.cpp \
	backends/firrtl/src/FirrtlBackend.cpp \
	backends/dot/src/DotBackend.cpp \
	backends/tree/src/TreeBackend.cpp \
	backends/generic/src/StreamIndentation.cpp

libfirrtlator_la_LDFLAGS = $(AM_LDFLAGS) $(LTLDFLAGS)

libfirrtlator_la_CXXFLAGS = $(AM_CXXFLAGS) -include $(top_builddir)/config.h \
	-I $(srcdir)/include \
	-I $(srcdir)/frontends \
	-I $(srcdir)/frontends/firrtl/include \
	-I $(srcdir)/backends \
	-I $(srcdir)/backends/generic/include \
	-I $(srcdir)/backends/firrtl/include \
	-I $(srcdir)/backends/tree/include 
	