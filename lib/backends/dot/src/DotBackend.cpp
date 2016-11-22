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

#include <iostream>
#include "../include/DotBackend.h"

namespace Firrtlator {
namespace Backend {
namespace Dot {

std::string Backend::name = "dot";
std::string Backend::description = "Generates graphviz dot files";
std::vector<std::string> Backend::filetypes = { "dot" };

REGISTER_BACKEND(Backend);

Backend::Backend(std::ostream &os) : BackendBase(os) {

}

void Backend::generate(std::shared_ptr<Circuit> ir) {
	Visitor v(mStream);

	ir->accept(v);
}

Visitor::Visitor(std::ostream *os) {
	mStream = os;
}

Visitor::~Visitor() {

}

bool Visitor::visit(Circuit &c) {
	addNode(&c, "circuit\n"+c.getId());

	for (auto m : c.getModules()) {
		addEdge(&c, m.get(), "");
	}

	return true;
}

void Visitor::leave(Circuit &c) {
	*mStream << "diGraph " << c.getId() << " {" << indent << endl;

	for (auto n : mNodes)
		*mStream << n << endl;

	for (auto e : mEdges) {
		int from = mNodeDictionary[std::get<0>(e)];
		int to = mNodeDictionary[std::get<1>(e)];
		std::string text = std::get<2>(e);

		*mStream << from << "->" << to;
		if (text.length() > 0)
			*mStream << "[label=\"" << text << "\"]";
		*mStream << ";" << endl;
	}

	*mStream << dedent << endl << "}" << endl;
}

bool Visitor::visit(Module &m) {
	addNode(&m, "module\n"+m.getId());

	for (auto p : m.getPorts())
		addEdge(&m, p.get(), "");

	addEdge(&m, m.getStmts().get(), "");

	return true;
}

void Visitor::leave(Module &m) {
}

bool Visitor::visit(Port &p) {
	addNode(&p, "port\n"+p.getId());

	return true;
}

void Visitor::leave(Port &p) {
}

bool Visitor::visit(Parameter &) {
	return true;
}

void Visitor::visit(TypeInt &t) {
}

void Visitor::visit(TypeClock &t) {
}

bool Visitor::visit(Field &) {
	return true;
}

bool Visitor::visit(TypeBundle &) {
	return true;
}

bool Visitor::visit(TypeVector &) {
	return true;
}


bool Visitor::visit(StmtGroup &g) {
	addNode(&g, "stmt_group\n");

	int i = 0;
	for (auto s : g)
		addEdge(&g, s.get(), "[" + std::to_string(i++) + "]");

	return true;
}

bool Visitor::visit(Wire &w) {
	addNode(&w, "wire\n"+w.getId());
	return true;
}

bool Visitor::visit(Reg &r) {
	addNode(&r, "reg\n"+r.getId());
	return false;
}

bool Visitor::visit(Instance &i) {
	addNode(&i, "inst:\n"+i.getId());
	return true;
}

bool Visitor::visit(Memory &m) {
	addNode(&m, "memory:\n"+m.getId());
	return true;
}
bool Visitor::visit(Node &n) {
	addNode(&n, "node:\n"+n.getId());
	return true;
}

bool Visitor::visit(Connect &c) {
	addNode(&c, "connect");

	addEdge(&c, c.getFrom().get(), "from");
	c.getFrom()->accept(*this);

	addEdge(&c, c.getTo().get(), "to");
	c.getTo()->accept(*this);

	return false;
}

bool Visitor::visit(Invalid &i) {
	addNode(&i, "invalid");
	return true;
}

void Visitor::leave(Invalid& in) {
}

bool Visitor::visit(Conditional &c) {
	addNode(&c, "conditional");

	addEdge(&c, c.getCondition().get(), "cond");

	addEdge(&c, c.getThen().get(), "then");

	if (c.getElse())
		addEdge(&c, c.getElse().get(), "else");

	return true;
}

bool Visitor::visit(ConditionalElse &c) {
	addNode(&c, "else");
	return true;
}

bool Visitor::visit(Stop &s) {
	addNode(&s, "stop");
	return true;
}

bool Visitor::visit(Printf &p) {
	addNode(&p, "printf");
	return true;
}

void Visitor::visit(Empty &e) {
	addNode(&e, "skip");
}

void Visitor::visit(Reference &r) {
	addNode(&r, "ref\n" + r.getToString());
}

void Visitor::visit(Constant &c) {
	addNode(&c, "const");
}

bool Visitor::visit(SubField &s) {
	addNode(&s, "subfield");
	return true;
}

bool Visitor::visit(SubIndex &s) {
	addNode(&s, "subindex");
	return true;
}

bool Visitor::visit(SubAccess &s) {
	addNode(&s, "subaccess");
	return true;
}

bool Visitor::visit(Mux &m) {
	addNode(&m, "mux");
	return true;
}

bool Visitor::visit(CondValid &c) {
	addNode(&c, "condvalid");
	return true;
}

bool Visitor::visit(PrimOp &op) {
	addNode(&op, op.operationName());

	int i = 0;
	for (auto o: op.getOperands())
		addEdge(&op, o.get(), "["+std::to_string(i++)+"]");

	return true;
}

void Visitor::addNode(IRNode* node, std::string text) {
	std::string n;
	int id = mNodes.size();
	mNodeDictionary[node] = id;
	n += std::to_string(id) + " [label=\"" + text + "\"];";
	mNodes.push_back(n);
}

void Visitor::addEdge(IRNode* from, IRNode* to, std::string text) {
	mEdges.push_back(std::make_tuple(from, to, text));
}

}
}
}
