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

#include "TreeBackend.h"

#include <sstream>

namespace Firrtlator {
namespace Backend {
namespace Tree {

std::string Backend::name = "tree";
std::string Backend::description = "Dump the IR tree in ASCII";
std::vector<std::string> Backend::filetypes = { "tree" };

REGISTER_BACKEND(Backend)

Backend::Backend(std::ostream &os) : BackendBase(os) {

}

void Backend::generate(std::shared_ptr<Circuit> ir) {
	std::basic_stringstream<char> sstream;
	Visitor v(&sstream);

	ir->accept(v);

	*mStream << sstream.str();
}

Visitor::Visitor(std::basic_stringstream<char> *os) {
	mStream = os;
}

Visitor::~Visitor() {

}

bool Visitor::visit(std::shared_ptr<Circuit> c) {
	*mStream << "(circuit) id=" << c->getId();
	outputInfo(c);
	*mStream << indent << endl;
	return true;
}

void Visitor::leave(std::shared_ptr<Circuit> c) {
	*mStream << dedent;
}

bool Visitor::visit(std::shared_ptr<Module> m) {
	*mStream << "(module) id=" << m->getId();
	outputInfo(m);
	*mStream << indent << endl;
	return true;
}

void Visitor::leave(std::shared_ptr<Module> m) {
	*mStream << dedent;
}

bool Visitor::visit(std::shared_ptr<Port> p) {
	*mStream << "(port) id=" << p->getId() << ", dir=";
	if (p->getDirection() == Port::INPUT)
		*mStream << "input";
	else
		*mStream << "output";
	outputInfo(p);
	*mStream << indent << endl;
	return true;
}

void Visitor::leave(std::shared_ptr<Port> p) {
	*mStream << dedent;
}

bool Visitor::visit(std::shared_ptr<Parameter> ) {
	return true;
}

void Visitor::visit(std::shared_ptr<TypeInt> t) {
	*mStream << "(type int) signed=" << (t->getSigned() ? "true" : "false");
	*mStream << ", width=" << t->getWidth() << endl;
}

void Visitor::visit(std::shared_ptr<TypeClock> t) {
	*mStream << "(type clock)" << endl;
}

bool Visitor::visit(std::shared_ptr<Field> f) {
	*mStream << "(field) id=" << f->getId();
	*mStream << ", flipped=" << (f->getFlip() ? "true" : "false");
	*mStream << indent;
	return true;
}

void Visitor::leave(std::shared_ptr<Field> ) {
	*mStream << dedent;
}

bool Visitor::visit(std::shared_ptr<TypeBundle> b) {
	*mStream << "(type bundle)" << indent << endl;
	return true;
}

void Visitor::leave(std::shared_ptr<TypeBundle> b) {
	*mStream << dedent;
}

bool Visitor::visit(std::shared_ptr<TypeVector> v) {
	*mStream << "(type vector) size=" << v->getSize();
	*mStream << indent << endl;
	return true;
}

void Visitor::leave(std::shared_ptr<TypeVector> b) {
	*mStream << dedent;
}

bool Visitor::visit(std::shared_ptr<StmtGroup> ) {
	*mStream << "(stmt group)" << indent << endl;
	return true;
}

void Visitor::leave(std::shared_ptr<StmtGroup> ) {
	*mStream << dedent;
}


bool Visitor::visit(std::shared_ptr<Wire> w) {
	*mStream << "(wire) id=" << w->getId() << indent << endl;;
	return true;
}

void Visitor::leave(std::shared_ptr<Wire> ) {
	*mStream << dedent;
}

bool Visitor::visit(std::shared_ptr<Reg> r) {
	*mStream << "(reg) id=" << r->getId();
	outputInfo(r);
	*mStream << indent << endl;
	*mStream << "[type] ";
	r->getType()->accept(*this);
	*mStream << "[clock] ";
	r->getClock()->accept(*this);
	if (r->getResetTrigger()) {
		*mStream << "[reset trigger] ";
		r->getResetTrigger()->accept(*this);
		*mStream << "[reset value] ";
		r->getResetValue()->accept(*this);
	}
	*mStream << dedent;
	return false;
}

bool Visitor::visit(std::shared_ptr<Instance> i) {
	*mStream << "(inst) id=" << i->getId();
	*mStream << indent << endl;
	return true;
}

void Visitor::leave(std::shared_ptr<Instance> ) {
	*mStream << dedent;
}

bool Visitor::visit(std::shared_ptr<Memory> m) {
	*mStream << "(memory)";
	return false;
}

bool Visitor::visit(std::shared_ptr<Node> n) {
	*mStream << "(node) id=" << n->getId();
	*mStream << indent << endl;
	return true;
}

void Visitor::leave(std::shared_ptr<Node> n) {
	*mStream << dedent;
}

bool Visitor::visit(std::shared_ptr<Connect> c) {
	*mStream << "(connect) partial=" << (c->getPartial() ? "true" : "false");
	*mStream << indent << endl << "[to]";
	c->getTo()->accept(*this);
	*mStream << "[from]";
	c->getFrom()->accept(*this);
	*mStream << dedent;
	return false;
}

bool Visitor::visit(std::shared_ptr<Invalid> i) {
	*mStream << "(invalid)" << indent << endl;
	return true;
}

void Visitor::leave(std::shared_ptr<Invalid>) {
	*mStream << dedent;
}

bool Visitor::visit(std::shared_ptr<Conditional> c) {
	*mStream << "(when)" << indent << endl;
	*mStream << "[cond]";

	return true;
}

void Visitor::leave(std::shared_ptr<Conditional>) {
	*mStream << dedent;
}

bool Visitor::visit(std::shared_ptr<ConditionalElse> e) {
	*mStream << "(else)" << indent << endl;
	return true;
}

void Visitor::leave(std::shared_ptr<ConditionalElse>) {
	*mStream << dedent;
}

bool Visitor::visit(std::shared_ptr<Stop> s) {
	*mStream << "(stop) code=" << s->getCode() << indent << endl;
	*mStream << "[clk]";
	s->getClock()->accept(*this);
	*mStream << "[cond]";
	s->getCondition()->accept(*this);
	*mStream << dedent;
	return false;
}

bool Visitor::visit(std::shared_ptr<Printf> p) {
	*mStream << "(printf) format=\"" << p->getFormat() << "\"" << endl;
	*mStream << indent << "[clock]";
	p->getClock()->accept(*this);
	*mStream << "[condition]";
	p->getCondition()->accept(*this);

	for (auto a : p->getArguments())
		a->accept(*this);

	*mStream << dedent;
	return false;
}

void Visitor::visit(std::shared_ptr<Empty> ) {
	*mStream << "(skip)" << endl;
}

void Visitor::visit(std::shared_ptr<Reference> r) {
	*mStream << "(ref) to=" << r->getToString() << endl;
}

void Visitor::visit(std::shared_ptr<Constant> c) {
	*mStream << "(const) value=" << c->getValue() << endl;
	*mStream << indent;
	c->getType()->accept(*this);
	*mStream << dedent;
}

bool Visitor::visit(std::shared_ptr<SubField> s) {
	*mStream << "(subfield)" << indent << endl;
	*mStream << "[field]";
	s->getField()->accept(*this);
	*mStream << "[of]";
	s->getOf()->accept(*this);
	*mStream << dedent;
	return false;
}

bool Visitor::visit(std::shared_ptr<SubIndex> s) {
	*mStream << "(subindex) index=" << s->getIndex() << indent << endl;
	*mStream << "[of]";
	s->getOf()->accept(*this);
	*mStream << dedent;
	return false;
}

bool Visitor::visit(std::shared_ptr<SubAccess> s) {
	*mStream << "(subaccess)" << indent << endl;
	*mStream << "[expr]";
	s->getExp()->accept(*this);
	*mStream << "[of]";
	s->getOf()->accept(*this);
	*mStream << dedent;
	return false;
}

bool Visitor::visit(std::shared_ptr<Mux> m) {
	*mStream << "(mux)" << indent << endl;
	*mStream << "[sel]";
	m->getSel()->accept(*this);
	*mStream << "[a]";
	m->getA()->accept(*this);
	*mStream << "[b]";
	m->getB()->accept(*this);
	*mStream << dedent;
	return false;
}

bool Visitor::visit(std::shared_ptr<CondValid> c) {
	*mStream << "(condvalid)" << indent << endl;
	*mStream << "[sel]";
	c->getSel()->accept(*this);
	*mStream << "[a]";
	c->getA()->accept(*this);
	*mStream << dedent;
	return false;
}

bool Visitor::visit(std::shared_ptr<PrimOp> op) {
	*mStream << "(" << op->operationName() << ")" << indent << endl;
	return true;
}

void Visitor::leave(std::shared_ptr<PrimOp> op) {
	*mStream << dedent;
}
void Visitor::outputInfo(std::shared_ptr<IRNode> n) {
	if (n->getInfo()) {
		*mStream << ", info=\"" << n->getInfo()->getValue() << "\"";
	}
}

}
}
}
