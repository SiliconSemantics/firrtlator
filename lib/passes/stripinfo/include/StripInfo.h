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

#include "Visitor.h"
#include "FirrtlatorPass.h"

#include <iostream>
#include <map>

namespace Firrtlator {
namespace Pass {
namespace StripInfo {

class Pass : public ::Firrtlator::Pass::PassBase {
public:
	Pass();
	virtual void run(std::shared_ptr<Circuit> ir);
	static std::string name;
	static std::string description;
};

class Visitor : public ::Firrtlator::Visitor {
public:
	Visitor();

	virtual ~Visitor();
	virtual bool visit(std::shared_ptr<Circuit>);

	virtual bool visit(std::shared_ptr<Module>);

	virtual bool visit(std::shared_ptr<Port>);

	virtual bool visit(std::shared_ptr<Wire>);

	virtual bool visit(std::shared_ptr<Reg>);

	virtual bool visit(std::shared_ptr<Instance>);

	virtual bool visit(std::shared_ptr<Memory>);

	virtual bool visit(std::shared_ptr<Node>);

	virtual bool visit(std::shared_ptr<Connect>);

	virtual bool visit(std::shared_ptr<Invalid>);

	virtual bool visit(std::shared_ptr<Conditional>);

	virtual bool visit(std::shared_ptr<ConditionalElse>);

	virtual bool visit(std::shared_ptr<Stop>);

	virtual bool visit(std::shared_ptr<Printf>);

	virtual void visit(std::shared_ptr<Empty>);
};

}
}
}
