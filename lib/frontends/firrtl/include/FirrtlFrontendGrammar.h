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

namespace Firrtlator {
namespace Frontend {
namespace Firrtl {

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
			>> -(qi::token(INDENT)
			>> *module [bind(&Circuit::addModule, _val, _1)]
			>> qi::token(DEDENT))
			;
		BOOST_SPIRIT_DEBUG_NODE(circuit);

		info = tok.info[_val = make_shared<Info>()(_1)]
			;
		BOOST_SPIRIT_DEBUG_NODE(info);

		module %= intmodule |
				extmodule
				;
		BOOST_SPIRIT_DEBUG_NODE(module);

		intmodule =
				tok.module
				>> tok.identifier [_val = make_shared<Module>()(_1, false)]
				>> ":"
				>> -info [bind(&Module::setInfo, _val, _1)]
				>> -(
					 qi::token(INDENT)
					>> *port [bind(&Module::addPort, _val, _1)]
					>> stmt_group [bind(&Module::setStatementGroup, _val, _1)]
					>> qi::token(DEDENT)
		            )
				;
		BOOST_SPIRIT_DEBUG_NODE(intmodule);

		extmodule =
				tok.extmodule
				>> tok.identifier [_val = make_shared<Module>()(_1, true)]
				>> ":"
				>> -info [bind(&Module::setInfo, _val, _1)]
				>> -(
					 qi::token(INDENT)
					 >> *port [bind(&Module::addPort, _val, _1)]
					 >>	-defname [bind(&Module::setDefname, _val, _1)]
					 >> *parameter [bind(&Module::addParameter, _val, _1)]
					 >> qi::token(DEDENT)
	               )
				;
		BOOST_SPIRIT_DEBUG_NODE(extmodule);

		port = (tok.input [_a = Port::Direction::INPUT]
				| tok.output [_a = Port::Direction::OUTPUT])
				>> (tok.identifier
		        >> ":" >> type ) [_val = make_shared<Port>()(_1, _a, _2)]
				>> -info [bind(&Port::setInfo, _val, _1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(port);

		defname = tok.defname >> "=" >> tok.identifier [_val = _1]
				;
		BOOST_SPIRIT_DEBUG_NODE(defname);

		parameter = tok.parameter [_val = make_shared<Parameter>()()]
				>> tok.identifier >> "=" >>
				(tok.int_ | tok.double_ | tok.string_double | tok.string_single)
				;
		BOOST_SPIRIT_DEBUG_NODE(parameter);

		type = type_int [_val = _1]
				| tok.Clock [_val = make_shared<TypeClock>()()]
				| type_bundle [_val = _1]
				| type_vector [_val = _1]
				;
		BOOST_SPIRIT_DEBUG_NODE(type);

		type_int = (tok.UInt [_val = make_shared<TypeInt>()(false)]
			        | tok.SInt [_val = make_shared<TypeInt>()(true)])
				>> -("<" >> tok.int_ [bind(&TypeInt::setWidth, _val, _1)] >> ">")
				;
		BOOST_SPIRIT_DEBUG_NODE(type_int);

		type_bundle = lit("{") [_val = make_shared<TypeBundle>()()]
				>> *field [bind(&TypeBundle::addField, _val, _1)]
				>> "}"
				;
		BOOST_SPIRIT_DEBUG_NODE(type_bundle);

		type_vector = type [_val = make_shared<TypeVector>()()]
				>> "["
				>> tok.int_ [bind(&TypeVector::setSize, _val, _1)]
				>> "]"
				;
		BOOST_SPIRIT_DEBUG_NODE(type_vector);

		field = (eps [_a = false]
				| tok.flip [_a = true])
				>> (tok.identifier
			    >> ":"
				>> type) [_val = make_shared<Field>()(_1, _2, _a)]
				;
		BOOST_SPIRIT_DEBUG_NODE(field);

		stmt %= wire | reg | mem | inst | node | connect | partconnect
				| invalidate | conditional | stop | printf_ | empty
				;
		BOOST_SPIRIT_DEBUG_NODE(stmt);

		stmt_group = eps [_val = make_shared<StmtGroup>()()]
				>> +stmt [bind(&StmtGroup::addStatement,_val,_1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(stmt_group);

		stmt_suite = (qi::tokenid(INDENT)
			    >> stmt_group [_val = _1]
				>> qi::tokenid(DEDENT))
				;
		BOOST_SPIRIT_DEBUG_NODE(stmt_suite);

		wire = tok.wire
				>> (tok.identifier
				>> ":"
				>> type) [_val = make_shared<Wire>()(_1, _2)]
				>> -info [bind(&Stmt::setInfo, _val, _1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(wire);

		reg = tok.reg
				>> (tok.identifier
				>> ":"
				>> type
				>> exp_) [_val = make_shared<Reg>()(_1, _2, _3)]
				>> -(tok.with >> ":" >> '('
				>> tok.identifier [ qi::_pass = (_1 == "reset") ]
				>> tok.assign >> '('
				>> exp_ [bind(&Reg::setResetTrigger, _val, _1)]
				>> exp_ [bind(&Reg::setResetValue, _val, _1)]
				>> ')' >> ')' )
				>> -info [bind(&Stmt::setInfo, _val, _1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(reg);

		mem = tok.mem
				>> tok.identifier [_val = make_shared<Memory>()(_1)]
				>> ":" >> "("
				>> *(mem_dtype(_val) | mem_depth(_val) | mem_readlat(_val)
					| mem_writelat(_val) | mem_ruw(_val) | mem_reader(_val)
					| mem_writer(_val) | mem_readwriter(_val))
				>> ")"
				;
		BOOST_SPIRIT_DEBUG_NODE(mem);

		mem_dtype = tok.dtype >> tok.assign
				>> type [bind(&Memory::setDType, _r1, _1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(mem_dtype);

		mem_depth = tok.depth >> tok.assign
				>> tok.int_ [bind(&Memory::setDepth, _r1, _1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(mem_depth);

		mem_readlat = tok.readlat >> tok.assign
				>> tok.int_ [bind(&Memory::setReadLatency, _r1, _1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(mem_readlat);

		mem_writelat = tok.writelat >> tok.assign
				>> tok.int_ [bind(&Memory::setWriteLatency, _r1, _1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(mem_writelat);

		mem_ruw = tok.ruw >> tok.assign >>
				(tok.old [_a = Memory::RuwFlag::OLD]
				| tok.new_ [_a = Memory::RuwFlag::NEW]
				| tok.undefined [_a = Memory::RuwFlag::UNDEFINED]
				) [bind(&Memory::setRuwFlag, _r1, _a)]
				;
		BOOST_SPIRIT_DEBUG_NODE(mem_ruw);

		mem_reader = tok.reader >> tok.assign
				>> tok.identifier [bind(&Memory::addReader, _r1, _1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(mem_reader);

		mem_writer = tok.writer >> tok.assign
				>> tok.identifier [bind(&Memory::addWriter, _r1, _1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(mem_writer);

		mem_readwriter = tok.readwriter >> tok.assign
				>> tok.identifier [bind(&Memory::addReadWriter, _r1, _1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(mem_readwriter);

		inst = tok.inst
				>> (tok.identifier
				>> tok.of
				>> reference) [_val = make_shared<Instance>()(_1, _3)]
				>> -info [bind(&Stmt::setInfo, _val, _1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(inst);

		node = tok.node
				>> (tok.identifier
				>> "="
				>> exp_) [_val = make_shared<Node>()(_1, _2)]
				>> -info [bind(&Stmt::setInfo, _val, _1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(node);

		connect = (exp_
				>> tok.connect
				>> exp_) [_val = make_shared<Connect>()(_1, _3)]
				>> -info [bind(&Connect::setInfo, _val, _1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(connect);

		partconnect = (exp_
				>> tok.partconnect
				>> exp_) [_val = make_shared<Connect>()(_1, _3, true)]
				>> -info [bind(&Connect::setInfo, _val, _1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(partconnect);

		invalidate = exp_ [_val = make_shared<Invalid>()(_1)]
				>> tok.is >> tok.invalid
				>> -info [bind(&Invalid::setInfo, _val, _1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(invalidate);

		conditional = tok.when
				>> exp_ [_val = make_shared<Conditional>()(_1)]
				>> ":"
				>> -info  [bind(&Conditional::setInfo, _val, _1)]
				>> -(stmt_group [bind(&Conditional::setThen, _val, _1)]
					| stmt_suite [bind(&Conditional::setThen, _val, _1)]
					)
				>> -conditional_else
				;
		BOOST_SPIRIT_DEBUG_NODE(conditional);

		conditional_else = tok.else_ [_val = make_shared<ConditionalElse>()()]
				>> (conditional [_a = make_shared<StmtGroup>()(_1)]
				   | (':'
					 >> -info [bind(&ConditionalElse::setInfo, _val, _1)]
					 >> (stmt_group [_a = _1]
					    | stmt_suite [_a = _1]
					    )
				      )
				   ) [bind(&ConditionalElse::setStmts, _val, _a)]
				;
		BOOST_SPIRIT_DEBUG_NODE(conditional_else);

		stop = tok.stop
				>> "("
				>> (exp_ >> exp_ >>
						tok.int_) [_val = make_shared<Stop>()(_1, _2, _3)]
				>> ")"
				>> -info [bind(&Stop::setInfo, _val, _1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(stop);

		printf_ = tok.printf
				>> "("
				>> (exp_ >> exp_
				>> tok.string_double) [_val = make_shared<Printf>()(_1, _2, _3)]
				>> *exp_ /*[bind(&Printf::addArgument, _val, _1)]*/
				>> ")"
				>> -info [bind(&Printf::setInfo, _val, _1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(printf_);

		empty = tok.skip [_val = make_shared<Empty>()()]
				>> -info  [bind(&Empty::setInfo, _val, _1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(empty);

		reference = tok.identifier [_val = make_shared<Reference>()(_1)]
				;
		BOOST_SPIRIT_DEBUG_NODE(reference);

		exp_ = (reference [_a = _1] | exp_int [_a = _1] | mux [_a = _1]
				| validif [_a = _1] | primop [_a = _1] )
				>> exp_helper(_a) [_val = _1]
				;
		BOOST_SPIRIT_DEBUG_NODE(exp_);

		exp_helper = exp_subfield(_r1) [_val = _1]
				| exp_subindex(_r1) [_val = _1]
				| exp_subaccess(_r1) [_val = _1]
				| eps [_val = _r1]
				;
		BOOST_SPIRIT_DEBUG_NODE(exp_helper);

		exp_int = type_int [_a = _1]
				>> "("
				>> ( tok.int_ [_val = make_shared<Constant>()(_a, _1)]
					| tok.string_double [_val = make_shared<Constant>()(_a, _1)]
				   ) >> ")"
				;
		BOOST_SPIRIT_DEBUG_NODE(exp_int);

		exp_subfield = "."
				>> reference [_a = make_shared<SubField>()(_1, _r1)]
				>> exp_helper(_a) [_val = _1]
				;
		BOOST_SPIRIT_DEBUG_NODE(exp_subfield);

		exp_subindex = "[" >> tok.int_ [_a = make_shared<SubIndex>()(_1, _r1)]
				>> "]" >> exp_helper(_a) [_val = _1]
				;
		BOOST_SPIRIT_DEBUG_NODE(exp_subindex);

		exp_subaccess = "["
				>> exp_ [_a = make_shared<SubAccess>()(_1, _r1)]
				>> "]" >> exp_helper(_a) [_val = _1]
				;
		BOOST_SPIRIT_DEBUG_NODE(exp_subaccess);

		mux = tok.mux >> "(" >> (exp_ >> exp_ >>
				exp_ )[_val = make_shared<Mux>()(_1, _2, _3)]
				>> ")"
				;
		BOOST_SPIRIT_DEBUG_NODE(mux);

		validif = tok.validif >> "(" >> (exp_
				>> exp_) [_val = make_shared<CondValid>()(_1, _2)]
				>> ")"
				;
		BOOST_SPIRIT_DEBUG_NODE(validif);

		primop = tok.primop [_val = gen_primop()(_1)]
				>> *exp_ [bind(&PrimOp::addOperand, _val, _1)]
				>> *tok.int_ [bind(&PrimOp::addParameter, _val, _1)]
				>> ")"
				;
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
    qi::rule<Iterator, std::shared_ptr<Field>(), qi::locals<bool> > field;

    qi::rule<Iterator, std::shared_ptr<Stmt>()> stmt;
    qi::rule<Iterator, std::shared_ptr<Wire>()> wire;
    qi::rule<Iterator, std::shared_ptr<Reg>()> reg;
    qi::rule<Iterator, std::shared_ptr<Memory>()> mem;
    qi::rule<Iterator, std::shared_ptr<Instance>() > inst;
    qi::rule<Iterator, std::shared_ptr<Node>()> node;
    qi::rule<Iterator, std::shared_ptr<Connect>()> connect, partconnect;
    qi::rule<Iterator, std::shared_ptr<Invalid>()> invalidate;
    qi::rule<Iterator, std::shared_ptr<Stop>()> stop;
    qi::rule<Iterator, std::shared_ptr<Printf>()> printf_;
    qi::rule<Iterator, std::shared_ptr<Empty>()> empty;

    qi::rule<Iterator, std::shared_ptr<Conditional>()> conditional;
    qi::rule<Iterator, std::shared_ptr<ConditionalElse>(),
    		qi::locals<std::shared_ptr<StmtGroup> > > conditional_else;
    qi::rule<Iterator, std::shared_ptr<StmtGroup>()> stmt_group, stmt_suite;

    qi::rule<Iterator, void(std::string)> reset_block, simple_reset, simple_reset0;

    qi::rule<Iterator, void(std::shared_ptr<Memory>)> mem_dtype, mem_depth,
    		mem_readlat, mem_writelat, mem_reader, mem_writer, mem_readwriter;
    qi::rule<Iterator, void(std::shared_ptr<Memory>),
    		qi::locals<Memory::RuwFlag> > mem_ruw;

    qi::rule<Iterator, std::shared_ptr<Expression>(),
    		qi::locals<std::shared_ptr<Expression> > >  exp_;
    qi::rule<Iterator,
			std::shared_ptr<Expression>(std::shared_ptr<Expression>) > exp_helper;
    qi::rule<Iterator, std::shared_ptr<Expression>()> mux, validif;
    qi::rule<Iterator, std::shared_ptr<Constant>(),
    		qi::locals<std::shared_ptr<TypeInt> > > exp_int;
    qi::rule<Iterator, std::shared_ptr<Reference>()> reference;

    qi::rule<Iterator,
		std::shared_ptr<Expression>(std::shared_ptr<Expression>),
		qi::locals<std::shared_ptr<Expression> > > exp_subaccess,
			exp_subfield, exp_subindex;

    qi::rule<Iterator, std::shared_ptr<PrimOp>()> primop;
};

}
}
}
