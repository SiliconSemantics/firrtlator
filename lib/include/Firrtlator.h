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

#include <vector>
#include <memory>

namespace Firrtlator {

class
__attribute__ ((visibility ("default")))
Firrtlator {
public:
	Firrtlator();
	~Firrtlator();
	bool parse(std::string::const_iterator begin,
			std::string::const_iterator end, std::string type = "");
	bool parseFile(std::string filename, std::string type = "");
	bool parseString(std::string string, std::string type = "");

	void elaborate();
	void pass(std::string id);

	typedef struct {
		std::string name;
		std::string description;
		std::vector<std::string> filetypes;
	} FrontendDescriptor;

	static std::vector<FrontendDescriptor> getFrontends();
	static std::string getFrontend(std::string type);

	typedef struct {
		std::string name;
		std::string description;
		std::vector<std::string> filetypes;
	} BackendDescriptor;


	static std::vector<BackendDescriptor> getBackends();
	static std::string getBackend(std::string type);
	void generate(std::string filename, std::string backend = "FIRRTL");
private:
	class impl;
	std::unique_ptr<impl> pimpl;
};

}

