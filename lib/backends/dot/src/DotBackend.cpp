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

REGISTER_BACKEND(Backend)

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

bool Visitor::visit(std::shared_ptr<Circuit> c) {
	addNode(c, "circuit\n"+c->getId());

	for (auto m : c->getModules()) {
		addEdge(c, m, "");
	}

	return true;
}

void Visitor::leave(std::shared_ptr<Circuit> c) {
	*mStream << "diGraph " << c->getId() << " {" << indent << endl;

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

bool Visitor::visit(std::shared_ptr<Module> m) {
	addNode(m, "module\n"+m->getId());

	for (auto p : m->getPorts())
		addEdge(m, p, "");

	addEdge(m, m->getStmts(), "");

	return true;
}

void Visitor::leave(std::shared_ptr<Module> m) {
}

bool Visitor::visit(std::shared_ptr<Port> p) {
	addNode(p, "port\n"+p->getId());

	return true;
}

void Visitor::leave(std::shared_ptr<Port> p) {
}

bool Visitor::visit(std::shared_ptr<Parameter> ) {
	return true;
}

void Visitor::visit(std::shared_ptr<TypeInt> t) {
}

void Visitor::visit(std::shared_ptr<TypeClock> t) {
}

bool Visitor::visit(std::shared_ptr<Field> ) {
	return true;
}

bool Visitor::visit(std::shared_ptr<TypeBundle> ) {
	return true;
}

bool Visitor::visit(std::shared_ptr<TypeVector> ) {
	return true;
}


bool Visitor::visit(std::shared_ptr<StmtGroup> g) {
	addNode(g, "stmt_group\n");

	int i = 0;
	for (auto s : *g)
		addEdge(g, s, "[" + std::to_string(i++) + "]");

	return true;
}

bool Visitor::visit(std::shared_ptr<Wire> w) {
	addNode(w, "wire\n"+w->getId());
	return true;
}

bool Visitor::visit(std::shared_ptr<Reg> r) {
	addNode(r, "reg\n"+r->getId());
	return false;
}

bool Visitor::visit(std::shared_ptr<Instance> i) {
	addNode(i, "inst:\n"+i->getId());
	return true;
}

bool Visitor::visit(std::shared_ptr<Memory> m) {
	addNode(m, "memory:\n"+m->getId());
	return true;
}
bool Visitor::visit(std::shared_ptr<Node> n) {
	addNode(n, "node:\n"+n->getId());
	return true;
}

bool Visitor::visit(std::shared_ptr<Connect> c) {
	addNode(c, "connect");

	addEdge(c, c->getFrom(), "from");
	c->getFrom()->accept(*this);

	addEdge(c, c->getTo(), "to");
	c->getTo()->accept(*this);

	return false;
}

bool Visitor::visit(std::shared_ptr<Invalid> i) {
	addNode(i, "invalid");
	return true;
}

void Visitor::leave(std::shared_ptr<Invalid> in) {
}

bool Visitor::visit(std::shared_ptr<Conditional> c) {
	addNode(c, "conditional");

	addEdge(c, c->getCondition(), "cond");

	addEdge(c, c->getThen(), "then");

	if (c->getElse())
		addEdge(c, c->getElse(), "else");

	return true;
}

bool Visitor::visit(std::shared_ptr<ConditionalElse> c) {
	addNode(c, "else");
	return true;
}

bool Visitor::visit(std::shared_ptr<Stop> s) {
	addNode(s, "stop");
	return true;
}

bool Visitor::visit(std::shared_ptr<Printf> p) {
	addNode(p, "printf");
	return true;
}

void Visitor::visit(std::shared_ptr<Empty> e) {
	addNode(e, "skip");
}

void Visitor::visit(std::shared_ptr<Reference> r) {
	addNode(r, "ref\n" + r->getToString());
}

void Visitor::visit(std::shared_ptr<Constant> c) {
	addNode(c, "const");
}

bool Visitor::visit(std::shared_ptr<SubField> s) {
	addNode(s, "subfield");
	return true;
}

bool Visitor::visit(std::shared_ptr<SubIndex> s) {
	addNode(s, "subindex");
	return true;
}

bool Visitor::visit(std::shared_ptr<SubAccess> s) {
	addNode(s, "subaccess");
	return true;
}

bool Visitor::visit(std::shared_ptr<Mux> m) {
	addNode(m, "mux");
	return true;
}

bool Visitor::visit(std::shared_ptr<CondValid> c) {
	addNode(c, "condvalid");
	return true;
}

bool Visitor::visit(std::shared_ptr<PrimOp> op) {
	addNode(op, op->operationName());

	int i = 0;
	for (auto o: op->getOperands())
		addEdge(op, o, "["+std::to_string(i++)+"]");

	return true;
}

void Visitor::addNode(std::shared_ptr<IRNode> node, std::string text) {
	std::string n;
	int id = mNodes.size();
	mNodeDictionary[node] = id;
	n += std::to_string(id) + " [label=\"" + text + "\"];";
	mNodes.push_back(n);
}

void Visitor::addEdge(std::shared_ptr<IRNode> from, std::shared_ptr<IRNode> to,
		std::string text) {
	mEdges.push_back(std::make_tuple(from, to, text));
}

}
}
}
