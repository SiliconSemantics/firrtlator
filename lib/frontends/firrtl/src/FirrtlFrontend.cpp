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

#include "FirrtlFrontend.h"

#include "FirrtlFrontendLexer.h"
#include "FirrtlFrontendGrammar.h"

#include <iostream>

namespace Firrtlator {
namespace Frontend {
namespace Firrtl {

bool FirrtlFrontend::parseString(std::string::const_iterator begin,
        std::string::const_iterator end) {

		typedef lex::lexertl::token<std::string::const_iterator,
				boost::mpl::vector<std::string, int> > token_type;
		typedef lex::lexertl::actor_lexer<token_type> lexer_type;
		typedef Tokens<lexer_type>::iterator_type iterator_type;

		Tokens<lexer_type> token_lexer;

		lexer_type::iterator_type first = token_lexer.begin(begin, end);
		lexer_type::iterator_type last = token_lexer.end();

		FirrtlGrammar<iterator_type> g(token_lexer);

		bool res = lex::tokenize_and_parse(begin, end, token_lexer, g, mIR);

		if (begin != end)
			res = false;

	if (!res) {
		return false;
	}

	return true;
}

}
}
}
