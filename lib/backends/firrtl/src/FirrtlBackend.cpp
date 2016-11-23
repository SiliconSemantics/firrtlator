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

#include "FirrtlBackend.h"
#include "StreamIndentation.h"

#include <iostream>

namespace Firrtlator {
namespace Backend {
namespace Firrtl {

std::string Backend::name = "FIRRTL";
std::string Backend::description = "Generates FIRRTL files";
std::vector<std::string> Backend::filetypes = { "fir" };

REGISTER_BACKEND(Backend)

Backend::Backend(std::ostream &os) : ::Firrtlator::Backend::BackendBase(os) {

}

void Backend::generate(std::shared_ptr<Circuit> ir) {
	std::basic_stringstream<char> sstream;
	Visitor v(&sstream);

	ir->accept(v);

	*mStream << sstream.str();
}

Visitor::Visitor(std::basic_stringstream<char, std::char_traits<char> > *os) {
	mStream = os;
}

Visitor::~Visitor() {

}

bool Visitor::visit(std::shared_ptr<Circuit> c) {
	*mStream << "circuit " << c->getId() << " :";

	outputInfo(c);

	*mStream << indent << endl;
	return true;
}

void Visitor::leave(std::shared_ptr<Circuit> c) {
	*mStream << dedent;
}

bool Visitor::visit(std::shared_ptr<Module> m) {
	bool ext = m->isExternal();

	*mStream << (ext ? "extmodule " : "module ") << m->getId() << " :";

	outputInfo(m);

	*mStream << indent << endl;
	return true;
}

void Visitor::leave(std::shared_ptr<Module> m) {
	*mStream << dedent;
}

bool Visitor::visit(std::shared_ptr<Port> p) {
	if (p->getDirection() == Port::INPUT)
		*mStream << "input ";
	else
		*mStream << "output ";
	*mStream << p->getId() << " : ";

	return true;
}

void Visitor::leave(std::shared_ptr<Port> p) {
	outputInfo(p);

	*mStream << endl;
}

bool Visitor::visit(std::shared_ptr<Parameter> ) {
	return true;
}

void Visitor::visit(std::shared_ptr<TypeInt> t) {
	if (t->getSigned())
		*mStream <<	"SInt";
	else
		*mStream << "UInt";

	*mStream << "<" << t->getWidth() << ">";
}

void Visitor::visit(std::shared_ptr<TypeClock> t) {
	*mStream << "Clock";
}

bool Visitor::visit(std::shared_ptr<Field> f) {
	if (f->getFlip())
		*mStream << "flip ";

	*mStream << f->getId();
	*mStream << " : ";

	return true;
}

bool Visitor::visit(std::shared_ptr<TypeBundle> t) {
	*mStream << "{";

	auto fields = t->getFields();

	if (fields.size() > 0) {
		fields[0]->accept(*this);
		fields.erase(fields.begin());
	}

	for (auto f : fields) {
		*mStream << ", ";
		f->accept(*this);
	}

	*mStream << " }";
	return false;
}

bool Visitor::visit(std::shared_ptr<TypeVector> ) {
	return true;
}

void Visitor::leave(std::shared_ptr<TypeVector> t) {
	*mStream << "[" << std::to_string(t->getSize()) << "]";
}

bool Visitor::visit(std::shared_ptr<StmtGroup> g) {
	return true;
}

void Visitor::leave(std::shared_ptr<StmtGroup> g) {

}

bool Visitor::visit(std::shared_ptr<Wire> w) {
	*mStream << "wire " << w->getId() << " : ";
	return true;
}

void Visitor::leave(std::shared_ptr<Wire> w) {
	*mStream << " ";
	outputInfo(w);
	*mStream << endl;
}

bool Visitor::visit(std::shared_ptr<Reg> r) {
	std::shared_ptr<Type> type = r->getType();
	std::shared_ptr<Expression> clk = r->getClock();
	std::shared_ptr<Expression> rtrig = r->getResetTrigger();
	std::shared_ptr<Expression> rval = r->getResetValue();
	*mStream << "reg " << r->getId() << " : ";

	type->accept(*this);

	*mStream << " ";

	clk->accept(*this);

	if (rtrig && rval) {
		*mStream << " with : ( reset => ( ";
		rtrig->accept(*this);
		*mStream << ", ";
		rval->accept(*this);
		*mStream << " ) ";
	}

	*mStream << ")";

	outputInfo(r);

	*mStream << endl;

	return false;
}

bool Visitor::visit(std::shared_ptr<Instance> inst) {
	*mStream << "inst " << inst->getId() << " of ";
	return true;
}

void Visitor::leave(std::shared_ptr<Instance> inst) {
	*mStream << " ";
	outputInfo(inst);
	*mStream << endl;
}

bool Visitor::visit(std::shared_ptr<Memory> m) {
	*mStream << "mem " << m->getId() << " : (";
	outputInfo(m);
	*mStream << indent << endl;

	*mStream << "data-type => ";
	m->getDType()->accept(*this);
	*mStream << endl;
	*mStream << "depth => " << std::to_string(m->getDepth()) << endl;
	*mStream << "read-latency => " << std::to_string(m->getReadlatency()) << endl;
	*mStream << "write-latency => " << std::to_string(m->getWritelatency()) << endl;
	*mStream << "read-under-write => ";
	switch (m->getRuwflag()) {
	case Memory::RuwFlag::OLD: *mStream << "old" << endl; break;
	case Memory::RuwFlag::NEW: *mStream << "new" << endl; break;
	default: *mStream << "undefined" << endl; break;
	}

	for (auto r : m->getReaders())
		*mStream << "reader => " << r << endl;

	for (auto w : m->getWriters())
		*mStream << "writer => " << w << endl;

	for (auto rw : m->getReadWriters())
		*mStream << "readwriter => " << rw << endl;

	*mStream << ")" << dedent << endl;

	return false;
}

bool Visitor::visit(std::shared_ptr<Node> n) {
	*mStream << "node " << n->getId() << " = ";
	return true;
}

void Visitor::leave(std::shared_ptr<Node> n) {
	*mStream << " ";
	outputInfo(n);
	*mStream << endl;
}

bool Visitor::visit(std::shared_ptr<Connect> c) {
	std::shared_ptr<Expression> from, to;

	from = c->getFrom();
	to = c->getTo();

	to->accept(*this);

	if (c->getPartial())
		*mStream << " <- ";
	else
		*mStream << " <= ";

	from->accept(*this);

	*mStream << endl;

	return false;
}

bool Visitor::visit(std::shared_ptr<Invalid> ) {
	return true;
}

void Visitor::leave(std::shared_ptr<Invalid>) {
	*mStream << " is invalid" << endl;
}

bool Visitor::visit(std::shared_ptr<Conditional> c) {
	*mStream << "when ";
	c->getCondition()->accept(*this);
	*mStream << " :";
	outputInfo(c);
	*mStream << indent << endl;

	c->getThen()->accept(*this);

	*mStream << dedent;

	if (c->getElse())
		c->getElse()->accept(*this);

	return false;
}

bool Visitor::visit(std::shared_ptr<ConditionalElse> e) {
	*mStream << "else :";
	outputInfo(e);
	*mStream << indent << endl;

	e->getStmts()->accept(*this);

	*mStream << dedent;

	return true;
}

bool Visitor::visit(std::shared_ptr<Stop> s) {
	*mStream << "stop(";
	s->getClock()->accept(*this);
	*mStream << ", ";
	s->getCondition()->accept(*this);
	*mStream << ", " << std::to_string(s->getCode());
	*mStream << ")";
	outputInfo(s);
	*mStream << endl;
	return false;
}

bool Visitor::visit(std::shared_ptr<Printf> p) {
	*mStream << "printf(";
	p->getClock()->accept(*this);
	*mStream << ", ";
	p->getCondition()->accept(*this);
	*mStream << ", \"" << p->getFormat() << "\")" << endl;
	return false;
}

void Visitor::visit(std::shared_ptr<Empty> e) {
	*mStream << "skip";
	outputInfo(e);
	*mStream << endl;
}

void Visitor::visit(std::shared_ptr<Reference> r) {
	*mStream << r->getToString();
}

void Visitor::visit(std::shared_ptr<Constant> c) {
	*mStream << c->getString();
}

bool Visitor::visit(std::shared_ptr<SubField> f) {
	f->getOf()->accept(*this);
	*mStream << ".";
	f->getField()->accept(*this);

	return false;
}

bool Visitor::visit(std::shared_ptr<SubIndex> i) {
	i->getOf()->accept(*this);
	*mStream << "[" << std::to_string(i->getIndex()) << "]";

	return false;
}

bool Visitor::visit(std::shared_ptr<SubAccess> a) {
	a->getOf()->accept(*this);
	*mStream << "[";
	a->getExp()->accept(*this);
	*mStream << "]";
	return false;
}

bool Visitor::visit(std::shared_ptr<Mux> m) {
	*mStream << "mux(";
	m->getSel()->accept(*this);
	*mStream << ", ";
	m->getA()->accept(*this);
	*mStream << ", ";
	m->getB()->accept(*this);
	*mStream << ")";

	return false;
}

bool Visitor::visit(std::shared_ptr<CondValid> c) {
	*mStream << "mux(";
	c->getSel()->accept(*this);
	*mStream << ", ";
	c->getA()->accept(*this);
	*mStream << ")";

	return false;
}

bool Visitor::visit(std::shared_ptr<PrimOp> op) {
	*mStream << op->operationName() << "(";

	auto ops = op->getOperands();
	auto params = op->getParameters();

	ops[0]->accept(*this);
	ops.erase(ops.begin());

	for (auto o : ops) {
		*mStream << ", ";
		o->accept(*this);
	}

	*mStream << ")";

	return false;
}

void Visitor::outputInfo(std::shared_ptr<IRNode> n) {
	std::shared_ptr<Info> info = n->getInfo();

	if (info)
		*mStream << " @[" << info->getValue() << "]";
}

}
}
}
