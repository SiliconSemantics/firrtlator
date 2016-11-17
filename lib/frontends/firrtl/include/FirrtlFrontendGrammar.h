/*
 * Copyright (c) 2016 Stefan Wallentowitz <wallento@silicon-semantics.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */#pragma once

//#define BOOST_SPIRIT_DEBUG 1

#include "FirrtlFrontendLexer.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

BOOST_FUSION_ADAPT_STRUCT(
    Firrtlator::MemoryCharacteristics,
    (std::shared_ptr<Firrtlator::Type>, type)
	(int,depth)
	(int,readlatency)
	(int,writelatency)
	(Firrtlator::MemoryCharacteristics::RuwFlag,ruwflag)
)

namespace Firrtlator {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

template <typename T>
struct make_shared_f
{
    template <typename... A> struct result
        { typedef std::shared_ptr<T> type; };

    template <typename... A>
    typename result<A...>::type operator()(A&&... a) const {
        return std::make_shared<T>(std::forward<A>(a)...);
    }
};

template <typename T>
using make_shared = phoenix::function<make_shared_f<T> >;

struct gen_primop_f
{
    struct result { typedef std::shared_ptr<PrimOp> type; };

    typename result::type operator()(std::string &op) const {
        return PrimOp::generate(op);
    }
};

using gen_primop = phoenix::function<gen_primop_f >;

template <typename Iterator>
struct FirrtlGrammar : qi::grammar<Iterator, std::shared_ptr<Circuit>()>
{
	template< typename TokenDef >
	FirrtlGrammar(const TokenDef& tok) : FirrtlGrammar::base_type(circuit)
	{
		using qi::lit;
		using qi::lexeme;
		using ascii::char_;
		using ascii::alpha;
		using ascii::alnum;
		using ascii::string;
		using namespace qi::labels;

		using phoenix::at_c;
		using phoenix::push_back;
		using phoenix::construct;
		using phoenix::function;
		using phoenix::bind;

		using boost::spirit::eps;

		circuit = tok.circuit
			>> tok.identifier [_val = make_shared<Circuit>()(_1)]
			>> ":"
			>> -info [bind(&Circuit::setInfo, _val, _1)]
			>> -(qi::token(INDENT) >> *module >> qi::token(DEDENT))
			;

		info = tok.info[_val = make_shared<Info>()(_1)]
			;

		module %= intmodule |
				extmodule;

		intmodule =
				tok.module
				>> tok.identifier [_val = make_shared<Module>()(_1, false)]
				>> ":"
				>> -info
				>> -(
					 qi::token(INDENT)
					>> *port
					>> +stmt
					>> qi::token(DEDENT)
		            )
				;

		extmodule =
				tok.extmodule
				>> tok.identifier [_val = make_shared<Module>()(_1, true)]
				>> ":"
				>> -info [bind(&Module::setInfo, _val, _1)]
				>> -(
					 qi::token(INDENT)
					 >> *port [bind(&Module::addPort, _val, _1)]
					 >>	-defname
					 >> *parameter
					 >> qi::token(DEDENT)
	               )
				;

		port = (tok.input [_a = Port::Direction::INPUT]
				| tok.output [_a = Port::Direction::OUTPUT])
				>> tok.identifier [_val = make_shared<Port>()(_1, _a)]
		        >> ":" >> type
				>> -info [bind(&Port::setInfo, _val, _1)]
				;

		defname = tok.defname >> "=" >> tok.identifier;

		parameter = tok.parameter [_val = make_shared<Parameter>()()]
				>> tok.identifier >> "=" >>
				(tok.int_ | tok.double_ | tok.string_double | tok.string_single)
				;

		type = type_int
				| tok.Clock [_val = make_shared<TypeClock>()()]
				| type_bundle
				| type_vector;

		type_int = (tok.UInt [_val = make_shared<TypeInt>()(false)]
			        | tok.SInt [_val = make_shared<TypeInt>()(true)])
				>> -("<" >> tok.int_ [bind(&TypeInt::setWidth, _val, _1)] >> ">")
				;

		type_bundle = lit("{") [_val = make_shared<TypeBundle>()()]
				>> *field
				>> "}"
				;

		type_vector = type [_val = make_shared<TypeVector>()()]
				>> "["
				>> tok.int_ [bind(&TypeVector::setSize, _val, _1)]
				>> "]";

		field = eps [_val = make_shared<Field>()()]
				>> -tok.flip [bind(&Field::setFlip, _val, true)]
				>> tok.identifier [bind(&Field::setId, _val, _1)]
			    >> ":"
				>> type [bind(&Field::setType, _val, _1)]
				;

		stmt %= wire | reg | mem | inst | node | connect | partconnect
				| invalidate | conditional | stop | printf_ | empty
				;

		stmt_group = lit("(") [_val = make_shared<Stmt>()()]
				>> +stmt
				>> ")"
				;

		stmt_suite = stmt
				| (qi::tokenid(INDENT) [_val = make_shared<Stmt>()()]
			    >> +stmt
				>> qi::tokenid(DEDENT))
				;

		wire = tok.wire [_val = make_shared<Stmt>()()]
				>> tok.identifier
				>> ":"
				>> type
				;

		reg = tok.reg [_val = make_shared<Stmt>()()]
				>> tok.identifier
				>> ":"
				>> type
				>> exp_
				>> -(tok.with >> ":" >> '('
				>> tok.identifier [ qi::_pass = (_1 == "reset") ]
				>> tok.assign >> '(' >> exp_ >> exp_ >> ')' >> ')' )
				>> -info
				;

		mem = tok.mem [_val = make_shared<Stmt>()()]
				>> tok.identifier
				>> ":" >> "("
				>> mem_char
				>> *(tok.reader >> tok.assign >> tok.identifier)
				>> *(tok.writer >> tok.assign >> tok.identifier)
				>> *(tok.readwriter >> tok.assign >> tok.identifier)
				>> ")"
				;

		mem_char = mem_dtype [at_c<0>(_val)=_1]
				>> mem_depth [at_c<1>(_val)=_1]
                >> mem_readlat [at_c<2>(_val)=_1]
				>> mem_writelat [at_c<3>(_val)=_1]
				>> mem_ruw [at_c<4>(_val)=_1]
				;

		mem_dtype = tok.dtype >> tok.assign >> type [_val = _1];
		mem_depth = tok.depth >> tok.assign >> tok.int_;
		mem_readlat = tok.readlat >> tok.assign >> tok.int_;
		mem_writelat = tok.writelat >> tok.assign >> tok.int_;
		mem_ruw = tok.ruw >> tok.assign >>
				(tok.old [_val = MemoryCharacteristics::RuwFlag::OLD]
				| tok.new_ [_val = MemoryCharacteristics::RuwFlag::NEW]
				| tok.undefined [_val = MemoryCharacteristics::RuwFlag::UNDEFINED]
				);

		inst = tok.inst [_val = make_shared<Stmt>()()]
				>> tok.identifier
				>> tok.of
				>> tok.identifier
				>> -info
				;

		node = tok.node [_val = make_shared<Stmt>()()]
				>> tok.identifier
				>> "="
				>> exp_
				>> -info
				;

		connect = exp_ [_val = make_shared<Stmt>()()]
				>> tok.connect
				>> exp_
				>> -info
				;

		partconnect = exp_ [_val = make_shared<Stmt>()()]
				>> tok.partconnect
				>> exp_
				>> -info
				;

		invalidate = exp_ [_val = make_shared<Stmt>()()]
				>> tok.is >> tok.invalid
				>> -info
				;

		conditional = tok.when [_val = make_shared<Stmt>()()]
				>> exp_
				>> ":"
				>> -info
				>> -stmt_suite
				>> -(tok.else_ >>
					 (conditional
					 | (':' >> -info >> -stmt_suite)
					 )
					)
				;

		stop = tok.stop [_val = make_shared<Stmt>()()]
				>> "("
				>> exp_ >> exp_ >> tok.int_
				>> ")"
				>> -info
				;

		printf_ = tok.printf [_val = make_shared<Stmt>()()]
				>> "("
				>> exp_ >> exp_
				>> tok.string_double
				>> *exp_
				>> ")"
				>> -info
				;

		empty = tok.skip [_val = make_shared<Stmt>()()]
				>> -info
				;

		reference = tok.identifier [_val = make_shared<Reference>()(_1)]
				;

		exp_ = (reference [_a = _1] | exp_int [_a = _1] | mux [_a = _1]
				| validif [_a = _1] | primop [_a = _1] )
				>> exp_helper(_a) [_val = _1]
				;

		exp_helper = exp_subfield(_r1) [_val = _1]
				| exp_subindex(_r1) [_val = _1]
				| exp_subaccess(_r1) [_val = _1]
				| eps [_val = _r1]
				;

		exp_int = (tok.UInt
				   | tok.SInt) [_val = make_shared<Expression>()()]
			    >> -("<" >> tok.int_ >> ">")
				>> "("
				>> ( tok.int_
					| tok.string_double)
				>> ")"
				;

		exp_subfield = "."
				>> reference [_a = make_shared<SubField>()(_1, _r1)]
				>> exp_helper(_a) [_val = _1]
				;

		exp_subindex = "[" >> tok.int_ [_a = make_shared<SubIndex>()(_1, _r1)]
				>> "]" >> exp_helper(_a) [_val = _1]
				;

		exp_subaccess = "["
				>> reference [_a = make_shared<SubAccess>()(_1, _r1)]
				>> "]" >> exp_helper(_a) [_val = _1]
				;

		mux = tok.mux [_val = make_shared<Expression>()()]
				>> "(" >> exp_ >> exp_ >> exp_ >> ")"
				;

		validif = tok.validif [_val = make_shared<Expression>()()]
				>> "(" >> exp_ >> exp_ >> ")"
				;

		primop = tok.primop [_val = gen_primop()(_1)]
				>> *exp_ [bind(&PrimOp::addOperand, _val, _1)]
				>> *tok.int_ [bind(&PrimOp::addParameter, _val, _1)]
				>> ")"
				;

	    BOOST_SPIRIT_DEBUG_NODE(circuit);
	    BOOST_SPIRIT_DEBUG_NODE(info);
	    BOOST_SPIRIT_DEBUG_NODE(module);
	    BOOST_SPIRIT_DEBUG_NODE(intmodule);
	    BOOST_SPIRIT_DEBUG_NODE(extmodule);
	    BOOST_SPIRIT_DEBUG_NODE(port);
	    BOOST_SPIRIT_DEBUG_NODE(defname);
	    BOOST_SPIRIT_DEBUG_NODE(parameter);
	    BOOST_SPIRIT_DEBUG_NODE(type);
	    BOOST_SPIRIT_DEBUG_NODE(type_int);
	    BOOST_SPIRIT_DEBUG_NODE(type_bundle);
	    BOOST_SPIRIT_DEBUG_NODE(type_vector);
	    BOOST_SPIRIT_DEBUG_NODE(field);
	    BOOST_SPIRIT_DEBUG_NODE(stmt);
	    BOOST_SPIRIT_DEBUG_NODE(stmt_suite);
	    BOOST_SPIRIT_DEBUG_NODE(wire);
	    BOOST_SPIRIT_DEBUG_NODE(reg);
	    BOOST_SPIRIT_DEBUG_NODE(mem);
	    BOOST_SPIRIT_DEBUG_NODE(node);
	    BOOST_SPIRIT_DEBUG_NODE(invalidate);
	    BOOST_SPIRIT_DEBUG_NODE(inst);
	    BOOST_SPIRIT_DEBUG_NODE(empty);
	    BOOST_SPIRIT_DEBUG_NODE(exp_);
	    BOOST_SPIRIT_DEBUG_NODE(exp_int);
	    BOOST_SPIRIT_DEBUG_NODE(exp_helper);
	    BOOST_SPIRIT_DEBUG_NODE(exp_subindex);
	    BOOST_SPIRIT_DEBUG_NODE(primop);
	}

	qi::rule<Iterator, std::shared_ptr<Circuit>()> circuit;
	qi::rule<Iterator, std::shared_ptr<Info>()> info;
    qi::rule<Iterator, std::shared_ptr<Module>()> module, extmodule, intmodule;
    qi::rule<Iterator, std::shared_ptr<Port>(), qi::locals<Port::Direction>> port;
    qi::rule<Iterator, std::string()> defname;
    qi::rule<Iterator, std::shared_ptr<Parameter>()> parameter;
    qi::rule<Iterator, std::shared_ptr<Type>()> type;
    qi::rule<Iterator, std::shared_ptr<TypeInt>()> type_int;
    qi::rule<Iterator, std::shared_ptr<TypeBundle>()> type_bundle;
    qi::rule<Iterator, std::shared_ptr<TypeVector>()> type_vector;
    qi::rule<Iterator, std::shared_ptr<Field>()> field;

    qi::rule<Iterator, std::shared_ptr<Stmt>()> stmt, wire, reg, mem, inst,
    		node, connect, partconnect, invalidate, conditional, stop, printf_,
			empty, stmt_group, stmt_suite;

    qi::rule<Iterator, void(std::string)> reset_block, simple_reset, simple_reset0;

    qi::rule<Iterator, MemoryCharacteristics()> mem_char;
    qi::rule<Iterator, std::shared_ptr<Type>()> mem_dtype;
    qi::rule<Iterator, int()> mem_depth;
    qi::rule<Iterator, int()> mem_readlat;
    qi::rule<Iterator, int()> mem_writelat;
    qi::rule<Iterator, MemoryCharacteristics::RuwFlag()> mem_ruw;

    qi::rule<Iterator, std::shared_ptr<Expression>(), qi::locals<std::shared_ptr<Expression> > >  exp_;
    qi::rule<Iterator, std::shared_ptr<Expression>(std::shared_ptr<Expression>) > exp_helper;
    qi::rule<Iterator, std::shared_ptr<Expression>()> exp_int, mux, validif;
    qi::rule<Iterator, std::shared_ptr<Reference>()> reference;

    qi::rule<Iterator,
		std::shared_ptr<Expression>(std::shared_ptr<Expression>),
		qi::locals<std::shared_ptr<Expression> > > exp_subaccess, exp_subfield, exp_subindex;

    qi::rule<Iterator, std::shared_ptr<PrimOp>()> primop;
};

}
