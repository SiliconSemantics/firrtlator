#define BOOST_SPIRIT_DEBUG 1

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/lex.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>
#include <boost/fusion/include/adapt_adt.hpp>

#include <iostream>
#include <stack>

#include "FirrtlFrontend.h"

namespace Firrtlator {

namespace fusion = boost::fusion;
namespace phoenix = boost::phoenix;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace lex = boost::spirit::lex;

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

template<typename Iterator>
struct FirrtlSkipper : public qi::grammar<Iterator> {

    FirrtlSkipper() : FirrtlSkipper::base_type(start) {
        start = ascii::space | ',' | (';' >> *(qi::char_ - boost::spirit::eol) >> boost::spirit::eol);
    }
    qi::rule<Iterator> start;
};

enum TokenIds {
  kTokenBegin = 1000,
  kTokenEnd = 1001,
};

class HandleIndent {
 public:
  HandleIndent(std::stack<int>& scopeLevels)
      : scopeLevels_(scopeLevels)
  {}

  template <typename Iterator, typename IdType, typename Context>
  void operator() (
      Iterator& start,
      Iterator& end,
      BOOST_SCOPED_ENUM(lex::pass_flags)& passFlag,
      IdType& id, Context& context) {

    int const level = std::distance(start, end);
    if (scopeLevels_.empty()) {
      if (level != 0) {
        throw std::runtime_error("Indent must start from 0!");
      }
      scopeLevels_.push(level);
    }
    // If the level is same, just ignore it and switch to normal state
    if (level == scopeLevels_.top()) {
      passFlag = lex::pass_flags::pass_ignore;
      context.set_state_name("NORMAL");
    }
    // If the level is larger, emit BEGIN and push the new level on stack
    else if (level > scopeLevels_.top()) {
      scopeLevels_.push(level);
      id = kTokenBegin;
      context.set_state_name("NORMAL");
    }
    // If the level is smaller, emit END and pop the top level from stack
    else { //Dedent, end scope
      scopeLevels_.pop();
      // Level must be one of the numbers occurring on the stack
      if (scopeLevels_.empty() || level > scopeLevels_.top()) {
        throw std::runtime_error("Dedenting failed, could not find indent matching previews ");
      } else {
        // make it a zero match, we can generate multiple END token by this trick
        end = start;
        id = kTokenEnd;
      }
    }
  }

 private:
  std::stack<int>& scopeLevels_;
};


class HandleNewline {
 public:
  HandleNewline(std::stack<int>& scopeLevels)
      : scopeLevels_(scopeLevels)
  {}

  template <typename Iterator, typename IdType, typename Context>
  void operator() (
      Iterator& start,
      Iterator& end,
      BOOST_SCOPED_ENUM(lex::pass_flags)& passFlag,
      IdType& id, Context& context) {

    if (context.get_eoi() == end) {
      // If the "end of input" is reached
      // we pop up all the non-zero levels.
      // for each pop up we generate a corresponding END token
      // here we apply the zero-match trick again to emit multiple END token
      if (scopeLevels_.top() != 0) {
        scopeLevels_.pop();
        end = start;
        id = kTokenEnd;
        return;
      }
    }
    passFlag = lex::pass_flags::pass_ignore;
    context.set_state_name("INITIAL");
  }

 private:
  std::stack<int>& scopeLevels_;
};

template <typename Lexer>
class Tokens : public lex::lexer<Lexer> {
 public:
  Tokens()
      : indent_("[ ]*")
      , newline_("[\\n\\r\\f]+")
      , whitespace_("[ \\t]+")
  {
    this->self
        = indent_[HandleIndent(scopeLevels_)]
        ;

    this->self("NORMAL")
        = newline_[HandleNewline(scopeLevels_)]
        | whitespace_[lex::_pass=lex::pass_flags::pass_ignore]
        ;
  }

 private:
  lex::token_def<> indent_;
  lex::token_def<> newline_, whitespace_;

  std::stack<int> scopeLevels_;
};

template <typename Iterator>
struct FirrtlGrammar : qi::grammar<Iterator, std::shared_ptr<Circuit>(), qi::grammar<Iterator>>
{
	FirrtlGrammar() : FirrtlGrammar::base_type(circuit)
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

		circuit =
			lit("circuit")
			>> id [_val = make_shared<Circuit>()(_1)]
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

		/*intmodule =
				lit("module")
				>> id
				>> lit(':')
				>> -info
				>> *port
				>> +stmt;*/

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
				>> type [bind(&Field::setType, _val, _1)];

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
	    BOOST_SPIRIT_DEBUG_NODE(id);
	    BOOST_SPIRIT_DEBUG_NODE(info);
	    BOOST_SPIRIT_DEBUG_NODE(quoted_string);
	    BOOST_SPIRIT_DEBUG_NODE(module);
	    BOOST_SPIRIT_DEBUG_NODE(extmodule);
	    BOOST_SPIRIT_DEBUG_NODE(port);
	    BOOST_SPIRIT_DEBUG_NODE(type);
	    BOOST_SPIRIT_DEBUG_NODE(type_int);
	    BOOST_SPIRIT_DEBUG_NODE(type_bundle);
	    BOOST_SPIRIT_DEBUG_NODE(type_vector);
	    BOOST_SPIRIT_DEBUG_NODE(field);
	}

	qi::rule<Iterator, std::shared_ptr<Circuit>(), qi::grammar<Iterator> > circuit;
	qi::rule<Iterator, std::string(), qi::grammar<Iterator> > id;
	qi::rule<Iterator, std::shared_ptr<Info>(), qi::grammar<Iterator> > info;
    qi::rule<Iterator, std::string(), qi::grammar<Iterator> > quoted_string;
    qi::rule<Iterator, std::shared_ptr<Module>(), qi::grammar<Iterator> > module;
    qi::rule<Iterator, std::shared_ptr<Module>(), qi::grammar<Iterator> > extmodule;
    qi::rule<Iterator, std::shared_ptr<Port>(), qi::locals<Port::Direction>, qi::grammar<Iterator> > port;
    qi::rule<Iterator, std::shared_ptr<Type>(), qi::grammar<Iterator> > type;
    qi::rule<Iterator, std::shared_ptr<TypeInt>(), qi::grammar<Iterator> > type_int;
    qi::rule<Iterator, std::shared_ptr<TypeBundle>(), qi::grammar<Iterator> > type_bundle;
    qi::rule<Iterator, std::shared_ptr<TypeVector>(), qi::grammar<Iterator> > type_vector;
    qi::rule<Iterator, std::shared_ptr<Field>(), qi::locals<bool>, qi::grammar<Iterator> > field;
};

bool FirrtlFrontend::parseString(std::string::const_iterator begin,
        std::string::const_iterator end) {
	std::shared_ptr<Circuit> c;

	FirrtlGrammar<std::string::const_iterator> g;
		FirrtlSkipper<std::string::const_iterator> s;

	bool res = qi::phrase_parse(begin, end, g, s, c);



	if (!res) {
		return false;
	}

	std::cout << "Circuit '" << c->getId() << "' with info \"" << c->getInfo()->getValue() << "\"" << std::endl;

	return true;
}


std::shared_ptr<Circuit> FirrtlFrontend::getIR() {

}
}
