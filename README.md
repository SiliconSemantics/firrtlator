![Logo](http://firrtlator.org/logo_wide.png)

[![Build Status](https://travis-ci.org/SiliconSemantics/firrtlator.svg?branch=master)](https://travis-ci.org/SiliconSemantics/firrtlator)

Firrtlator is a C++ library and tool for the
[Flexible Intermediate Representation for RTL (FIRRTL)](https://github.com/ucb-bar/firrtl). It
provides the representation of FIRRTL in C++ and comes with the
frontend to parse FIRRTL and backend to emit FIRRTL (and other
representations). Passes can operate on a FIRRTL design to optimize
it, inject logic, etc. Firrtlator can be easily extended with custom
frontend, passes and backends.

The FIRRTL grammar itself is in alpha status. Firrtlator is even
before that, as currently semantics are not fully implemented.

## Build Firrtlator

You need g++, autotools and the [Boost](http://www.boost.org/) headers
to build firrtlator. The Boost headers are usually found as the boost
development package in your Linux distribution (such as `libboost-dev`
in Debian/Ubuntu).

To build Firrtlator, simply run

    ./autogen.sh
    mkdir build; cd build
	../configure --prefix=~/firrtlator-install/
	make
	make install

You will find the firrlator library, pkgconfig and firrtlator tool
installed in your home directory for testing (`~/firrtlator-install`).

Firrtlator (specifically the FIRRTL frontend partser) needs a recent
version of Boost. Your system's Boost version may not be recent enough
and you will see:

	checking for Boost headers version >= 1.58.0... no
	configure: error: cannot find Boost headers version >= 1.58.0

If that is the case, download and extract a recent Boost version. You
will not need to build Boost, it is headers-only which means you can
also delete it after building. For example:

	wget https://sourceforge.net/projects/boost/files/boost/1.62.0/boost_1_62_0.zip
	unzip boost_1_62_0.zip
	../configure --prefix=~/firrtlator-install/ --with-boost=$PWD/boost_1_62_0
	make
	make install

## Try out Firrtlator

To try out Firrtlator, create a simple design `simple.fir`:

	; Simple test circuit
	circuit test : @[simple.v 4:1]
	  module test : @[simple.v 4:1]
        input a : UInt<1> @[simple.v 5:1]
        input b : UInt<1> @[simple.v 5:1]
        input clk : Clock @[simple.v 6:1]
        input rst : UInt<1> @[simple.v 5:1]
        output c : UInt<1> @[simple.v 7:1]
        reg creg : UInt<1> clk with: ( reset => ( rst, UInt<1>(0) ) ) @[simple.v 9:3]
        creg <= add(a,b) @[simple.v 11:3]
        c <= creg @[simple.v 12:3]

To ease the command lines in this example, add Firrtlator to your
path:

	export PATH=~/firrtlator-install/bin:$PATH

First, have a look at the help that explains the usage:

    firrtlator --help

Now, we just write the design back as FIRRTL:

    firrtlator -i simple.ir out.fir

If you now inspect `out.fir` you will find that the comment (line 1)
is gone, but the design itself is preserved.

In the next step, we can let Firrtlator print the design as a tree:

	firrtlator -i simple.ir out.tree

The resulting `out.tree` shows the IR tree:

	(circuit) id=test, info="simple.v 4:1"
	  (module) id=test, info="simple.v 4:1"
        (port) id=a, dir=input, info="simple.v 5:1"
          (type int) signed=false, width=1
        (port) id=b, dir=input, info="simple.v 5:1"
          (type int) signed=false, width=1
        (port) id=clk, dir=input, info="simple.v 6:1"
          (type clock)
        (port) id=rst, dir=input, info="simple.v 5:1"
          (type int) signed=false, width=1
        (port) id=c, dir=output, info="simple.v 7:1"
          (type int) signed=false, width=1
        (stmt group)
          (reg) id=creg
            [type] (type int) signed=false, width=1
            [clock] (ref) to=clk
            [reset trigger] (ref) to=rst
            [reset value] (const) value=0
              (type int) signed=false, width=1
          (connect) partial=false, info="simple.v 11:3"
            [to](ref) to=creg
            [from](add)
              (ref) to=a
              (ref) to=b
          (connect) partial=false, info="simple.v 12:3"
            [to](ref) to=c
            [from](ref) to=creg

It is also possible to print it with dot:

    firrtlator -i simple.ir out.dot
    dot -Tpng out.dot > out.png

This will produce this output:

![Dot](http://firrtlator.org/example-simple.png)

Finally we want to use a very, very simple pass that removes all
annotated source information from the source:

    firrtlator -i simple.fir -p stripinfo our.fir

## Get involved

As mentioned, Firrtlator is right at the beginning. For the moment it
does not do much semantics, including width inference etc. So
manipulating the IR tree requires care.

The next steps I want to do are:

 * Create a Verilator frontend using Firrtlator
 * Add semantics
 * Add a pass for injection of Vivado-compatible debug probes

You are invited to contribute ideas, thoughts and code. Please open an
[issue](https://github.com/SiliconSemantics/firrtlator/issues) with
your input or send me an email to
[stefan@wallentowitz.de](mailto:stefan@wallentowitz.de).
