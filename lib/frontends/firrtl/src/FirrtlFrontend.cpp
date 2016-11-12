#include "FirrtlFrontend.h"
#include "FirrtlFrontendLexer.h"
#include "FirrtlFrontendGrammar.h"

#include <iostream>

namespace Firrtlator {

bool FirrtlFrontend::parseString(std::string::const_iterator begin,
        std::string::const_iterator end) {
		std::shared_ptr<Circuit> c;

		typedef lex::lexertl::token<std::string::const_iterator, boost::mpl::vector<std::string> > token_type;
		typedef lex::lexertl::actor_lexer<token_type> lexer_type;
		typedef Tokens<lexer_type>::iterator_type iterator_type;

		Tokens<lexer_type> token_lexer;

		lexer_type::iterator_type first = token_lexer.begin(begin, end);
		lexer_type::iterator_type last = token_lexer.end();

		/*while (first != last && token_is_valid(*first))
		{
			std::cout << "Token: " << token_lexer.nameof(first)
	        				<< " ('" << first->value() << "')\n";
			++first;
		}

		return false;*/
	    typedef qi::in_state_skipper<Tokens<lexer_type>::lexer_def> skipper_type;
		FirrtlGrammar<iterator_type, skipper_type> g(token_lexer);

		bool res = lex::tokenize_and_phrase_parse(begin, end, token_lexer, g, qi::in_state( "NORMAL" )[ token_lexer.self ], c);

		return res;


/*	std::shared_ptr<Circuit> c;

	FirrtlGrammar<std::string::const_iterator> g;
		FirrtlSkipper<std::string::const_iterator> s;




	if (!res) {
		return false;
	}

	std::cout << "Circuit '" << c->getId() << "' with info \"" << c->getInfo()->getValue() << "\"" << std::endl;

	return true;*/
}


std::shared_ptr<Circuit> FirrtlFrontend::getIR() {

}
}
