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

#include <boost/spirit/include/lex.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>

namespace Firrtlator {

namespace lex = boost::spirit::lex;

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
			while (scopeLevels_.top() != 0) {
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

class HandleEmpty {
public:
	HandleEmpty() {}

	template <typename Iterator, typename IdType, typename Context>
	void operator() (
			Iterator& start,
			Iterator& end,
			BOOST_SCOPED_ENUM(lex::pass_flags)& passFlag,
			IdType& id, Context& context) {

		passFlag = lex::pass_flags::pass_ignore;
		context.set_state_name("NORMAL");
	}
};

class HandleInfo {
public:
	HandleInfo() {}
	template <typename Iterator, typename IdType, typename Context>
	void operator() (
			Iterator& start,
			Iterator& end,
			BOOST_SCOPED_ENUM(lex::pass_flags)& passFlag,
			IdType& id, Context& context) {
		context.set_value(std::string(start+2,end-1));
	}
};

class HandleSingleQuote {
public:
	HandleSingleQuote() {}
	static bool unquote(std::string& val)
	{
		auto outidx = 0;
		for(auto in = val.begin(); in!=val.end(); ++in) {
			switch(*in) {
			case 0x27:
				if (++in == val.end()) { // eat the escape
					// end of input reached
					val.resize(outidx); // resize to effective chars
					return true;
				}
				// fall through
			default:
				val[outidx++] = *in; // append the character
			}
		}

		return false; // not ended with double quote as expected
	}

	template <typename Iterator, typename IdType, typename Context>
	void operator() (
			Iterator& start,
			Iterator& end,
			BOOST_SCOPED_ENUM(lex::pass_flags)& passFlag,
			IdType& id, Context& context) {
		std::string raw(start,end);
			if (unquote(raw))
				context.set_value(raw);
			else
				passFlag = lex::pass_flags::pass_fail;
	}
};

class HandleDoubleQuote {
public:
	HandleDoubleQuote() {}
	static bool unquote(std::string& val)
	{
		auto outidx = 0;
		for(auto in = val.begin(); in!=val.end(); ++in) {
			switch(*in) {
			case '"':
				if (++in == val.end()) { // eat the escape
					// end of input reached
					val.resize(outidx); // resize to effective chars
					return true;
				}
				// fall through
			default:
				val[outidx++] = *in; // append the character
			}
		}

		return false; // not ended with double quote as expected
	}

	template <typename Iterator, typename IdType, typename Context>
	void operator() (
			Iterator& start,
			Iterator& end,
			BOOST_SCOPED_ENUM(lex::pass_flags)& passFlag,
			IdType& id, Context& context) {
		std::string raw(start,end);
			if (unquote(raw))
				context.set_value(raw);
			else
				passFlag = lex::pass_flags::pass_fail;
	}
};

class HandlePrimOp {
public:
	HandlePrimOp() {}

	template <typename Iterator, typename IdType, typename Context>
	void operator() (
			Iterator& start,
			Iterator& end,
			BOOST_SCOPED_ENUM(lex::pass_flags)& passFlag,
			IdType& id, Context& context) {
		std::string raw(start,end-1);
		PrimOp::Operation op;
		if (PrimOp::lookup(raw, op)) {
			context.set_value(raw);
		} else {
			passFlag = lex::pass_flags::pass_fail;
		}
	}
};

}
