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

#include "../backends/generic/include/StreamIndentation.h"
#include "IR.h"
#include "Util.h"

#include "Visitor.h"

namespace Firrtlator {

Module::Module() : Module("") {}
Module::Module(std::string id, bool external)
: IRNode(id), mExternal(external) {}

void Module::addPort(std::shared_ptr<Port> port) {
	mPorts.push_back(port);
}

void Module::setStatementGroup(std::shared_ptr<StmtGroup> stmts) {
	mStmts = stmts;
}

void Module::setDefname(std::string defname) {
	throwAssert(mExternal, "Cannot assign defname to module");

	mDefname = defname;
}
void Module::addParameter(std::shared_ptr<Parameter> param) {
	throwAssert(mExternal, "Cannot add parameter to module");

	mParameters.push_back(param);
}

bool Module::isExternal() {
	return mExternal;
}

std::string Module::getDefname() {
	return mDefname;
}

std::vector<std::shared_ptr<Port> > Module::getPorts() {
	return mPorts;
}

std::shared_ptr<StmtGroup> Module::getStmts() {
	return mStmts;
}

std::vector<std::shared_ptr<Parameter> > Module::getParameters() {
	return mParameters;
}

void Module::accept(Visitor& v) {
	if (!v.visit(shared_from_base<Module>()))
		return;

	for (auto p : mPorts)
		p->accept(v);

	if (mStmts)
		mStmts->accept(v);

	v.leave(shared_from_base<Module>());
}

}
