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
 */

#pragma once

//#define BOOST_SPIRIT_LEXERTL_DEBUG

#include <FirrtlFrontendLexerHelper.h>

#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/lex.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>

#include <stack>

namespace Firrtlator {

namespace lex = boost::spirit::lex;
namespace phoenix = boost::phoenix; 

// The indent and dedent handling was inspired from:
// http://zhuoqiang.me/lexing-python-indentation-using-spirit-lex.html

template <typename Lexer>
class Tokens : public lex::lexer<Lexer> {
public:
	Tokens()
: indent_("[ \\t]*")
, newline_("[\\n\\r\\f]+")
, whitespace_("[ \\t,]+")
, emptyline("[ \\t]*$")
, comment_ (";.*$")
, identifier("[A-Za-z_][A-Za-z0-9_]*")
, info ("@\\[[^\\]]*\\]")
#define TERM(x) x(#x)
, TERM(circuit)
, TERM(module)
, TERM(extmodule)
, TERM(input)
, TERM(output)
, TERM(defname)
, TERM(parameter)
, TERM(UInt)
, TERM(SInt)
, TERM(Clock)
, TERM(wire)
, TERM(reg)
, TERM(mem)
, TERM(cmem)
, TERM(smem)
, TERM(inst)
, TERM(of)
, TERM(node)
, TERM(mport)
, TERM(infer)
, TERM(flip)
, TERM(with)
, TERM(is)
, TERM(invalid)
, dtype("datatype")
, TERM(depth)
, readlat("read-latency")
, writelat("write-latency")
, ruw("read-under-write")
, TERM(reader)
, TERM(writer)
, TERM(readwriter)
, TERM(when)
, else_("else")
, TERM(stop)
, TERM(printf)
, TERM(skip)
, TERM(old)
, new_("new")
, TERM(undefined)
, TERM(mux)
, TERM(validif)
//, TERM(reset) // Don't match
, connect("<=")
, partconnect("<-")
, assign("=>")
, primop("(add|sub|mul|div|mod|lt|leq|gt|geq|eq|neq|pad|asUInt|asSint|"
		"asClock|shl|shr|dshl|dshr|cvt|neg|not|and|or|xor|andr|orr|"
		"xorr|cat|bits|head|tail)\\(")

, int_ ("(-?[0-9]+)|(0x[A-F0-9_]+)|(0o[0-7]+)|(0x[01]+)")
, double_ ("-?[0-9]+.[0-9]+")
, string_double ("\\\"(\\\\\\\"|[^\"]|\\\"\\\")*\\\"")
, string_single ("'([^']|'')*'")
#undef TERM
{
		this->self = emptyline [HandleEmpty()] |
				indent_[HandleIndent(scopeLevels_)];

		this->self("NORMAL") =
				circuit | module | extmodule | input | output | defname | parameter |
				UInt | SInt | Clock |
				wire | reg | mem | cmem | smem | inst | of | node |
				flip | with | reset | is | invalid |
				dtype | depth | readlat | writelat | ruw | reader | writer | readwriter |
				when | else_ | stop | printf | skip | old | new_ | undefined |
				mux | validif |

				connect | partconnect | assign |

				primop [HandlePrimOp()]|

				string_double [HandleDoubleQuote()] |
				string_single [HandleSingleQuote()] |
				int_ | double_ |
				identifier | info [HandleInfo()] |

				":" | "<" | ">" | "(" | ")" | "=" | "{" | "}" | "." |

				comment_[lex::_pass=lex::pass_flags::pass_ignore]
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
		EMIT(identifier);
		EMIT(comment_);
		EMIT(int_);
		EMIT(circuit);
		EMIT(info);
		return "other";
	}
#undef EMIT

	lex::token_def<> indent_;
	lex::token_def<lex::omit> newline_, whitespace_, emptyline;
	lex::token_def<> comment_;
	lex::token_def<std::string> identifier;
	lex::token_def<std::string> info;

	lex::token_def<> circuit, module, extmodule, defname, parameter;
	lex::token_def<> input, output;
	lex::token_def<> UInt, SInt, Clock;
	lex::token_def<> wire, reg, mem, cmem, smem, mport, inst, of, node, infer;
	lex::token_def<> flip, with, reset, is, invalid;
	lex::token_def<> dtype, depth, readlat, writelat, ruw, reader, writer, readwriter;
	lex::token_def<> when, else_, stop, printf, skip, old, new_, undefined;
	lex::token_def<> mux, validif;

	lex::token_def<> connect, partconnect, assign;

	lex::token_def<std::string> primop;

	lex::token_def<int> int_;
	lex::token_def<> double_;
	lex::token_def<std::string> string_double, string_single;

	std::stack<int> scopeLevels_;
};
}
