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
#include "FirrtlatorFrontend.h"
#include "FirrtlatorPass.h"
#include "FirrtlatorBackend.h"

namespace Firrtlator {

class Firrtlator::impl {
public:
	std::shared_ptr<Circuit> mIR;
};

Firrtlator::Firrtlator() : pimpl(new impl()) {}

Firrtlator::~Firrtlator() {}

std::vector<Firrtlator::FrontendDescriptor> Firrtlator::getFrontends() {
	return Frontend::Registry::getDescriptors();
}

std::string Firrtlator::getFrontend(std::string type) {
	auto desc = Frontend::Registry::getDescriptors();

	for (auto f : desc) {
		if (std::find(f.filetypes.begin(), f.filetypes.end(), type)
			!= f.filetypes.end()) {
			return f.name;
		}
	}

	throw std::runtime_error("Cannot find backend for: " + type);
}

bool Firrtlator::parse(std::string::const_iterator begin,
		std::string::const_iterator end, std::string type) {

	std::shared_ptr<Frontend::FrontendBase> frontend;
	frontend = Frontend::Registry::create(type);

	if (!frontend->parseString(begin, end))
		return false;
	pimpl->mIR = frontend->getIR();

	return true;
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

std::vector<Firrtlator::PassDescriptor> Firrtlator::getPasses() {
	return Pass::Registry::getDescriptors();
}


void Firrtlator::pass(std::string id) {
	std::shared_ptr<Pass::PassBase> p = Pass::Registry::create(id);
	p->run(pimpl->mIR);
}

void Firrtlator::generate(std::string filename, std::string type) {
	std::shared_ptr<Backend::BackendBase> backend;

	std::fstream fs;
    fs.open (filename, std::fstream::out);

	backend = Backend::Registry::create(type, fs);
	backend->generate(pimpl->mIR);

	fs.close();
}

std::vector<Firrtlator::BackendDescriptor> Firrtlator::getBackends() {
	return Backend::Registry::getDescriptors();
}

std::string Firrtlator::getBackend(std::string type) {
	auto desc = Backend::Registry::getDescriptors();

	for (auto b : desc) {
		if (std::find(b.filetypes.begin(), b.filetypes.end(), type)
			!= b.filetypes.end()) {
			return b.name;
		}
	}

	throw std::runtime_error("Cannot find backend for: " + type);
}


}
