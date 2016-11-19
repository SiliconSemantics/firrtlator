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

#include "../include/StreamIndentation.h"

namespace Firrtlator {

/* Helper function to get a storage index in a stream */
static int get_indent_index() {
    /* ios_base::xalloc allocates indices for custom-storage locations. These indices are valid for all streams */
    static int index = std::ios_base::xalloc();
    return index;
}

std::ios_base& indent(std::ios_base& stream) {
    /* The iword(index) function gives a reference to the index-th custom storage location as a integer */
    stream.iword(get_indent_index())++;
    return stream;
}

std::ios_base& dedent(std::ios_base& stream) {
    /* The iword(index) function gives a reference to the index-th custom storage location as a integer */
    stream.iword(get_indent_index())--;
    return stream;
}

std::basic_ostream<char, std::char_traits<char>>& endl(std::basic_ostream<char, std::char_traits<char>>& stream) {
    int indent = stream.iword(get_indent_index());
    stream.put(stream.widen('\n'));
    while (indent) {
        stream.put(stream.widen('\t'));
        indent--;
    }
    stream.flush();
    return stream;
}

}
