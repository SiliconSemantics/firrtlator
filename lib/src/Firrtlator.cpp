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

#include <Firrtlator.h>
#include <IR.h>
#include "FirrtlFrontend.h"
#include "FirrtlBackend.h"

namespace Firrtlator {

class Firrtlator::impl {
public:
	std::shared_ptr<Circuit> mIR;
};

Firrtlator::Firrtlator() : pimpl(new impl()) {}

Firrtlator::~Firrtlator() {}

bool Firrtlator::parse(std::string::const_iterator begin,
		std::string::const_iterator end, std::string type) {
	if (type == "fir") {
		FirrtlFrontend frontend;
		if (!frontend.parseString(begin, end))
			return false;
		pimpl->mIR = frontend.getIR();
		return true;
	}

	return false;
}

bool Firrtlator::parseFile(std::string filename, std::string type) {
    std::ifstream in(filename, std::ios_base::in);
    if (!in) {
        // TODO: log
        return false;
    }
    std::string str((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());

    return parse(str.begin(), str.end(), type);
}

bool Firrtlator::parseString(std::string content, std::string type) {
	return parse(content.begin(), content.end(), type);

}

void Firrtlator::elaborate() {
	// Nothing for now
}

void pass(std::string id) {
	// None so far
}

void Firrtlator::generate(std::string filename, std::string type) {
	if (type == "fir") {
		Backend::Firrtl::Backend backend(std::cout);
		backend.generate(pimpl->mIR);
	} else {
		throw std::runtime_error("Unknown backend");
	}
}

std::vector<Firrtlator::BackendDescriptor> Firrtlator::getBackends() {
	return Backend::Registry::getDescriptors();
}


}
