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

#include "StreamIndentation.h"
#include "Util.h"

#include <iostream>
#include <sstream>

namespace Firrtlator {

/* Helper function to get a storage index in a stream */
static int get_indent_index() {
    static int index = std::ios_base::xalloc();
    return index;
}

inline void beginOfLine(std::basic_stringstream<char>* stream) {
	std::stringbuf *pbuf = stream->rdbuf();
	int p, origin = stream->tellp();


	for (p = origin-1; p > 0; p--) {
		pbuf->pubseekpos(p-1);
		char c = pbuf->sgetc();
		if (c == '\n') {
			pbuf->pubseekpos(p);
		    int indent = stream->iword(get_indent_index());
		    while (indent) {
		        *stream << "  ";
		        indent--;
		    }
			break;
		} else if ((c == ' ') || (c == '\n')) {
			/* continue skipping */
		} else {
			pbuf->pubseekpos(origin);
			break;
		}
	}
}

std::basic_ostream<char, std::char_traits<char>>& indent(std::basic_ostream<char, std::char_traits<char>>& stream) {
	std::basic_stringstream<char> *sstream;

	stream.iword(get_indent_index())++;
	sstream = dynamic_cast<std::basic_stringstream<char>* >(&stream);
	if (sstream) {
		beginOfLine(sstream);
	}

    return stream;
}

std::basic_ostream<char, std::char_traits<char>>& dedent(std::basic_ostream<char, std::char_traits<char>>& stream) {
	std::basic_stringstream<char> *sstream;
	throwAssert((stream.iword(get_indent_index()) > 0), "Cannot dedent at level 0");

	stream.iword(get_indent_index())--;

	sstream = dynamic_cast<std::basic_stringstream<char>* >(&stream);
	if (sstream) {
		beginOfLine(sstream);
	}
    return stream;
}

std::basic_ostream<char, std::char_traits<char>>& endl(std::basic_ostream<char, std::char_traits<char>>& stream) {
    int indent = stream.iword(get_indent_index());
    stream.put(stream.widen('\n'));

    while (indent) {
        stream << "  ";
        indent--;
    }
    stream.flush();
    return stream;
}

}
