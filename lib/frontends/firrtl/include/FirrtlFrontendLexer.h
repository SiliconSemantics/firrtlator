#pragma once

//#define BOOST_SPIRIT_LEXERTL_DEBUG

#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/lex.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>

#include <stack>

namespace Firrtlator {

namespace lex = boost::spirit::lex;
namespace phoenix = boost::phoenix; 

// From: http://zhuoqiang.me/lexing-python-indentation-using-spirit-lex.html

enum TokenIds {
	INDENT = 1000,
	DEDENT = 1001
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
			id = INDENT;
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
				id = DEDENT;
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
				id = DEDENT;
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
: indent_("[ \\t]*")
, newline_("[\\n\\r\\f]+")
, whitespace_("[ \\t]+")
, comment_ (";.*$")
, identifier_("[A-Za-z_][A-Za-z0-9_]*")
, colon_ (":")
, circuit_ ("circuit")
, module_ ("module")
, extmodule_ ("extmodule")
, input_ ("input")
, output_ ("output")
{
		this->self = indent_[HandleIndent(scopeLevels_)];

		this->self("NORMAL") =
				identifier_
				| circuit_
				| comment_[lex::_pass=lex::pass_flags::pass_ignore]
			    | newline_[HandleNewline(scopeLevels_)]
			    | whitespace_[lex::_pass=lex::pass_flags::pass_ignore]
				;
}

#define EMIT(x) if (it->id() == x.id()) { return #x; }

	template <typename TokenIter>
	std::string nameof(TokenIter it)
	{
		if (it->id() == INDENT) { return "INDENT"; }
		if (it->id() == DEDENT) { return "DEDENT"; }
		EMIT(whitespace_);
		EMIT(identifier_);
		EMIT(comment_);
		EMIT(circuit_);
		EMIT(colon_);
		return "other";
	}
#undef EMUT

	lex::token_def<> indent_;
	lex::token_def<> comment_;
	lex::token_def<> text_;
	lex::token_def<lex::omit> newline_, whitespace_;
	lex::token_def<std::string> identifier_;
	lex::token_def<> colon_;

	lex::token_def<> circuit_;
	lex::token_def<> module_;
	lex::token_def<> extmodule_;
	lex::token_def<> input_;
	lex::token_def<> output_;

	std::stack<int> scopeLevels_;
};

}
