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

REGISTER_BACKEND(Backend);

Backend::Backend(std::ostream &os) : ::Firrtlator::Backend::BackendBase(os) {

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
	*mStream << "circuit " << c.getId() << " :";

	outputInfo(c);

	*mStream << indent << endl;
	return true;
}

void Visitor::leave(Circuit &c) {
	*mStream << dedent << endl;
}

bool Visitor::visit(Module &m) {
	bool ext = m.isExternal();

	*mStream << (ext ? "extmodule " : "module ") << m.getId() << " :";

	outputInfo(m);

	*mStream << indent << endl;
	return true;
}

void Visitor::leave(Module &m) {
	*mStream << dedent << endl;
}

bool Visitor::visit(Port &p) {
	if (p.getDirection() == Port::INPUT)
		*mStream << "input ";
	else
		*mStream << "output ";
	*mStream << p.getId() << " : ";

	return true;
}

void Visitor::leave(Port &p) {
	outputInfo(p);

	*mStream << endl;
}

bool Visitor::visit(Parameter &) {
	return true;
}

void Visitor::visit(TypeInt &t) {
	if (t.getSigned())
		*mStream <<	"SInt";
	else
		*mStream << "UInt";

	*mStream << "<" << t.getWidth() << ">";
}

void Visitor::visit(TypeClock &t) {
	*mStream << "Clock";
}

bool Visitor::visit(Field &f) {
	if (f.getFlip())
		*mStream << "flip ";

	*mStream << f.getId();
	*mStream << " : ";

	return true;
}

bool Visitor::visit(TypeBundle &t) {
	*mStream << "{";

	auto fields = t.getFields();

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

bool Visitor::visit(TypeVector &) {
	return true;
}

void Visitor::leave(TypeVector &t) {
	*mStream << "[" << std::to_string(t.getSize()) << "]";
}

bool Visitor::visit(Wire &w) {
	*mStream << "wire " << w.getId() << " : ";
	return true;
}

void Visitor::leave(Wire &w) {
	*mStream << " ";
	outputInfo(w);
	*mStream << endl;
}

bool Visitor::visit(Reg &r) {
	std::shared_ptr<Type> type = r.getType();
	std::shared_ptr<Expression> clk = r.getClock();
	std::shared_ptr<Expression> rtrig = r.getResetTrigger();
	std::shared_ptr<Expression> rval = r.getResetValue();
	*mStream << "reg " << r.getId() << " : ";

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

bool Visitor::visit(Instance &inst) {
	*mStream << "inst " << inst.getId() << " of ";
	return true;
}

void Visitor::leave(Instance &inst) {
	*mStream << " ";
	outputInfo(inst);
	*mStream << endl;
}

bool Visitor::visit(Memory &m) {
	*mStream << "mem " << m.getId() << " : (";
	outputInfo(m);
	*mStream << indent << endl;

	*mStream << "data-type => ";
	m.getDType()->accept(*this);
	*mStream << endl;
	*mStream << "depth => " << std::to_string(m.getDepth()) << endl;
	*mStream << "read-latency => " << std::to_string(m.getReadlatency()) << endl;
	*mStream << "write-latency => " << std::to_string(m.getWritelatency()) << endl;
	*mStream << "read-under-write => ";
	switch (m.getRuwflag()) {
	case Memory::RuwFlag::OLD: *mStream << "old" << endl; break;
	case Memory::RuwFlag::NEW: *mStream << "new" << endl; break;
	default: *mStream << "undefined" << endl; break;
	}

	for (auto r : m.getReaders())
		*mStream << "reader => " << r << endl;

	for (auto w : m.getWriters())
		*mStream << "writer => " << w << endl;

	for (auto rw : m.getReadWriters())
		*mStream << "readwriter => " << rw << endl;

	*mStream << ")" << dedent << endl;

	return false;
}

bool Visitor::visit(Node &n) {
	*mStream << "node " << n.getId() << " = ";
	return true;
}

void Visitor::leave(Node &n) {
	*mStream << " ";
	outputInfo(n);
	*mStream << endl;
}

bool Visitor::visit(Connect &c) {
	std::shared_ptr<Expression> from, to;

	from = c.getFrom();
	to = c.getTo();

	to->accept(*this);

	if (c.getPartial())
		*mStream << " <- ";
	else
		*mStream << " <= ";

	from->accept(*this);

	*mStream << endl;

	return false;
}

bool Visitor::visit(Invalid &) {
	return true;
}

void Visitor::leave(Invalid& in) {
	*mStream << " is invalid" << endl;
}

bool Visitor::visit(Conditional &c) {
	*mStream << "when ";
	c.getCondition()->accept(*this);
	*mStream << " :";
	outputInfo(c);
	*mStream << indent << endl;

	for (auto s : c.getIfStmts())
		s->accept(*this);

	*mStream << dedent << endl;

	auto elses = c.getElseStmts();
	if (elses.size() > 0) {
		*mStream << "else :";
		if (c.getElseInfo())
			*mStream << " @[" << c.getElseInfo()->getValue() << "]";
		*mStream << indent << endl;
		for (auto s : elses)
			s->accept(*this);
		*mStream << dedent << endl;
	}

	return false;
}

bool Visitor::visit(Stop &s) {
	*mStream << "stop(";
	s.getClock()->accept(*this);
	*mStream << ", ";
	s.getCondition()->accept(*this);
	*mStream << ", " << std::to_string(s.getCode());
	*mStream << ")";
	outputInfo(s);
	*mStream << endl;
	return false;
}

bool Visitor::visit(Printf &p) {
	*mStream << "printf(";
	p.getClock()->accept(*this);
	*mStream << ", ";
	p.getCondition()->accept(*this);
	*mStream << ", \"" << p.getFormat() << "\")" << endl;
	return false;
}

void Visitor::visit(Empty &e) {
	*mStream << "skip";
	outputInfo(e);
	*mStream << endl;
}

void Visitor::visit(Reference &r) {
	*mStream << r.getToString();
}

void Visitor::visit(Constant &c) {
	*mStream << c.getString();
}

bool Visitor::visit(SubField &f) {
	f.getOf()->accept(*this);
	*mStream << ".";
	f.getField()->accept(*this);

	return false;
}

bool Visitor::visit(SubIndex &i) {
	i.getOf()->accept(*this);
	*mStream << "[" << std::to_string(i.getIndex()) << "]";

	return false;
}

bool Visitor::visit(SubAccess &a) {
	a.getOf()->accept(*this);
	*mStream << "[";
	a.getExp()->accept(*this);
	*mStream << "]";
	return false;
}

bool Visitor::visit(Mux &m) {
	*mStream << "mux(";
	m.getSel()->accept(*this);
	*mStream << ", ";
	m.getA()->accept(*this);
	*mStream << ", ";
	m.getB()->accept(*this);
	*mStream << ")";

	return false;
}

bool Visitor::visit(CondValid &c) {
	*mStream << "mux(";
	c.getSel()->accept(*this);
	*mStream << ", ";
	c.getA()->accept(*this);
	*mStream << ")";

	return false;
}

bool Visitor::visit(PrimOp &op) {
	*mStream << op.operationName() << "(";

	auto ops = op.getOperands();
	auto params = op.getParameters();

	ops[0]->accept(*this);
	ops.erase(ops.begin());

	for (auto o : ops) {
		*mStream << ", ";
		o->accept(*this);
	}

	*mStream << ")";

	return false;
}

void Visitor::outputInfo(IRNode &n) {
	std::shared_ptr<Info> info = n.getInfo();

	if (info)
		*mStream << " @[" << info->getValue() << "]";
}


}
}
}
