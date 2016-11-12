#pragma once

#define BOOST_SPIRIT_DEBUG 1

#include "FirrtlFrontendLexer.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

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

template <typename Iterator, typename Skipper>
struct FirrtlGrammar : qi::grammar<Iterator, std::shared_ptr<Circuit>(), Skipper>
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

		using boost::spirit::int_;

		circuit = tok.circuit_
			>> tok.identifier_ [_val = make_shared<Circuit>()(_1)];
			/*
			>> lit(":")
			>> -info [bind(&Circuit::setInfo, _val, _1)]
			>> lit ("<INDENT>")
		    >> module [bind(&Circuit::addModule, _val, _1)]
			;

		id %= lexeme[(alpha | '_')
			>> *(alnum | '_')]
			;

		info = lit("@[")
				>> quoted_string [_val = make_shared<Info>()(_1)]
				>> lit("]");

		quoted_string %= lexeme['"' >> +(char_ - '"') >> '"'];

		module %= //intmodule |
				extmodule;

		intmodule =
				lit("module")
				>> id
				>> lit(':')
				>> -info
				>> *port
				>> +stmt;

		extmodule =
				lit("extmodule")
				>> id [_val = make_shared<Module>()(_1, true)]
				>> lit(':')
				>> -info [bind(&Module::setInfo, _val, _1)]
				>> *port [bind(&Module::addPort, _val, _1)];

		port = (lit("input") [_a = Port::Direction::INPUT]
				| lit("output") [_a = Port::Direction::OUTPUT])
				>> id [_val = make_shared<Port>()(_1, _a)]
		        >> lit(':') >> type
				>> -info [bind(&Port::setInfo, _val, _1)];

		type = type_int
				| lit("Clock") [_val = make_shared<TypeClock>()()]
				| type_bundle
				| type_vector;

		type_int = (lit("UInt") [_val = make_shared<TypeInt>()(false)]
					 | lit("SInt") [_val = make_shared<TypeInt>()(true)]
				   )
				>> -((lit('<')
			         >> int_ [bind(&TypeInt::setWidth, _val, _1)]
				     >> lit('>')));

		type_bundle = lit('{') [_val = make_shared<TypeBundle>()()];

		type_vector = type[_val = make_shared<TypeVector>()()]
				>> lit('[')
				>> int_ [bind(&TypeVector::setSize, _val, _1)]
				>> lit(']');

		field = -lit("flip") [_a = true]
				>> id [_val = make_shared<Field>()(_a)]
			    >> lit(':')
				>> type [bind(&Field::setType, _val, _1)];*/

		/*stmt = wire
//				| reg
//              | mem
				| inst
				;

		wire = lit("wire")
				>> id
				>> lit(':')
				>> type;

		inst = lit("inst")
				>> id
				>> lit("of")
				>> id;*/

	    BOOST_SPIRIT_DEBUG_NODE(circuit);
	    /*  BOOST_SPIRIT_DEBUG_NODE(id);
	    BOOST_SPIRIT_DEBUG_NODE(info);
	    BOOST_SPIRIT_DEBUG_NODE(quoted_string);
	    BOOST_SPIRIT_DEBUG_NODE(module);
	    BOOST_SPIRIT_DEBUG_NODE(extmodule);
	    BOOST_SPIRIT_DEBUG_NODE(port);
	    BOOST_SPIRIT_DEBUG_NODE(type);
	    BOOST_SPIRIT_DEBUG_NODE(type_int);
	    BOOST_SPIRIT_DEBUG_NODE(type_bundle);
	    BOOST_SPIRIT_DEBUG_NODE(type_vector);
	    BOOST_SPIRIT_DEBUG_NODE(field);*/
	}

	qi::rule<Iterator, std::shared_ptr<Circuit>(), Skipper> circuit;
/*	qi::rule<Iterator, std::string(), Skipper> id;
	qi::rule<Iterator, std::shared_ptr<Info>(), Skipper> info;
    qi::rule<Iterator, std::string(), Skipper> quoted_string;
    qi::rule<Iterator, std::shared_ptr<Module>(), Skipper> module;
    qi::rule<Iterator, std::shared_ptr<Module>(), Skipper> extmodule;
    qi::rule<Iterator, std::shared_ptr<Port>(), qi::locals<Port::Direction>, Skipper> port;
    qi::rule<Iterator, std::shared_ptr<Type>(), Skipper> type;
    qi::rule<Iterator, std::shared_ptr<TypeInt>(), Skipper> type_int;
    qi::rule<Iterator, std::shared_ptr<TypeBundle>(), Skipper> type_bundle;
    qi::rule<Iterator, std::shared_ptr<TypeVector>(), Skipper> type_vector;
    qi::rule<Iterator, std::shared_ptr<Field>(), qi::locals<bool>, Skipper> field;*/
};

}
