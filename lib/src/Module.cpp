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

#include "IR.h"
#include "IndentationBuffer.h"
#include "Util.h"

namespace Firrtlator {

Module::Module() : Module("") {}
Module::Module(std::string id, bool external)
: IRNode(id), mExternal(external) {}

void Module::addPort(std::shared_ptr<Port> port) {
	mPorts.push_back(port);
}
void Module::addStmt(std::shared_ptr<Stmt> stmt) {
	throwAssert(!mExternal, "Cannot add statements to extmodule");

	mStmts.push_back(stmt);
}
void Module::setDefname(std::string defname) {
	throwAssert(mExternal, "Cannot assign defname to module");

	mDefname = defname;
}
void Module::addParameter(std::shared_ptr<Parameter> param) {
	throwAssert(mExternal, "Cannot add parameter to module");

	mParameters.push_back(param);
}

void Module::emit(std::ostream& os) const {
	os << (mExternal ? "extmodule" : "module");
	os << " " << mId << " : ";

	if (mInfo) {
		os << *mInfo;
	}

	os << indent << endl;

	for (auto p : mPorts)
		os << *p;

	os << dedent << endl;
}

void Module::accept(Visitor& v) {

}

}
