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

bool Visitor::visit(Circuit &c) {
	*mStream << "(circuit) id=" << c.getId();
	outputInfo(c);
	*mStream << indent << endl;
	return true;
}

void Visitor::leave(Circuit &c) {
	*mStream << dedent;
}

bool Visitor::visit(Module &m) {
	*mStream << "(module) id=" << m.getId();
	outputInfo(m);
	*mStream << indent << endl;
	return true;
}

void Visitor::leave(Module &m) {
	*mStream << dedent;
}

bool Visitor::visit(Port &p) {
	*mStream << "(port) id=" << p.getId() << ", dir=";
	if (p.getDirection() == Port::INPUT)
		*mStream << "input";
	else
		*mStream << "output";
	outputInfo(p);
	*mStream << indent << endl;
	return true;
}

void Visitor::leave(Port &p) {
	*mStream << dedent;
}

bool Visitor::visit(Parameter &) {
	return true;
}

void Visitor::visit(TypeInt &t) {
	*mStream << "(type int) signed=" << (t.getSigned() ? "true" : "false");
	*mStream << ", width=" << t.getWidth() << endl;
}

void Visitor::visit(TypeClock &t) {
	*mStream << "(type clock)" << endl;
}

bool Visitor::visit(Field &f) {
	*mStream << "(field) id=" << f.getId();
	*mStream << ", flipped=" << (f.getFlip() ? "true" : "false");
	*mStream << indent;
	return true;
}

void Visitor::leave(Field &) {
	*mStream << dedent;
}

bool Visitor::visit(TypeBundle &b) {
	*mStream << "(type bundle)" << indent << endl;
	return true;
}

void Visitor::leave(TypeBundle &b) {
	*mStream << dedent;
}

bool Visitor::visit(TypeVector &v) {
	*mStream << "(type vector) size=" << v.getSize();
	*mStream << indent << endl;
	return true;
}

void Visitor::leave(TypeVector &b) {
	*mStream << dedent;
}

bool Visitor::visit(StmtGroup &) {
	*mStream << "(stmt group)" << indent << endl;
	return true;
}

void Visitor::leave(StmtGroup &) {
	*mStream << dedent;
}


bool Visitor::visit(Wire &w) {
	*mStream << "(wire) id=" << w.getId() << indent << endl;;
	return true;
}

void Visitor::leave(Wire &) {
	*mStream << dedent;
}

bool Visitor::visit(Reg &r) {
	*mStream << "(reg) id=" << r.getId();
	outputInfo(r);
	*mStream << indent << endl;
	*mStream << "[type] ";
	r.getType()->accept(*this);
	*mStream << "[clock] ";
	r.getClock()->accept(*this);
	if (r.getResetTrigger()) {
		*mStream << "[reset trigger] ";
		r.getResetTrigger()->accept(*this);
		*mStream << "[reset value] ";
		r.getResetValue()->accept(*this);
	}
	*mStream << dedent;
	return false;
}

bool Visitor::visit(Instance &i) {
	*mStream << "(inst) id=" << i.getId();
	*mStream << indent << endl;
	return true;
}

void Visitor::leave(Instance &) {
	*mStream << dedent;
}

bool Visitor::visit(Memory &m) {
	*mStream << "(memory)";
	return false;
}

bool Visitor::visit(Node &n) {
	*mStream << "(node) id=" << n.getId();
	*mStream << indent << endl;
	return true;
}

void Visitor::leave(Node &n) {
	*mStream << dedent;
}

bool Visitor::visit(Connect &c) {
	*mStream << "(connect) partial=" << (c.getPartial() ? "true" : "false");
	*mStream << indent << endl << "[to]";
	c.getTo()->accept(*this);
	*mStream << "[from]";
	c.getFrom()->accept(*this);
	*mStream << dedent;
	return false;
}

bool Visitor::visit(Invalid &i) {
	*mStream << "(invalid)" << indent << endl;
	return true;
}

void Visitor::leave(Invalid& in) {
	*mStream << dedent;
}

bool Visitor::visit(Conditional &c) {
	*mStream << "(when)" << indent << endl;
	*mStream << "[cond]";

	return true;
}

void Visitor::leave(Conditional&) {
	*mStream << dedent;
}

bool Visitor::visit(ConditionalElse &e) {
	*mStream << "(else)" << indent << endl;
	return true;
}

void Visitor::leave(ConditionalElse&) {
	*mStream << dedent;
}

bool Visitor::visit(Stop &s) {
	*mStream << "(stop) code=" << s.getCode() << indent << endl;
	*mStream << "[clk]";
	s.getClock()->accept(*this);
	*mStream << "[cond]";
	s.getCondition()->accept(*this);
	*mStream << dedent;
	return false;
}

bool Visitor::visit(Printf &p) {
	*mStream << "(printf) format=\"" << p.getFormat() << "\"" << endl;
	*mStream << indent << "[clock]";
	p.getClock()->accept(*this);
	*mStream << "[condition]";
	p.getCondition()->accept(*this);

	for (auto a : p.getArguments())
		a->accept(*this);

	*mStream << dedent;
	return false;
}

void Visitor::visit(Empty &) {
	*mStream << "(skip)" << endl;
}

void Visitor::visit(Reference &r) {
	*mStream << "(ref) to=" << r.getToString() << endl;
}

void Visitor::visit(Constant &c) {
	*mStream << "(const) value=" << c.getValue() << endl;
	*mStream << indent;
	c.getType()->accept(*this);
	*mStream << dedent;
}

bool Visitor::visit(SubField &s) {
	*mStream << "(subfield)" << indent << endl;
	*mStream << "[field]";
	s.getField()->accept(*this);
	*mStream << "[of]";
	s.getOf()->accept(*this);
	*mStream << dedent;
	return false;
}

bool Visitor::visit(SubIndex &s) {
	*mStream << "(subindex) index=" << s.getIndex() << indent << endl;
	*mStream << "[of]";
	s.getOf()->accept(*this);
	*mStream << dedent;
	return false;
}

bool Visitor::visit(SubAccess &s) {
	*mStream << "(subaccess)" << indent << endl;
	*mStream << "[expr]";
	s.getExp()->accept(*this);
	*mStream << "[of]";
	s.getOf()->accept(*this);
	*mStream << dedent;
	return false;
}

bool Visitor::visit(Mux &m) {
	*mStream << "(mux)" << indent << endl;
	*mStream << "[sel]";
	m.getSel()->accept(*this);
	*mStream << "[a]";
	m.getA()->accept(*this);
	*mStream << "[b]";
	m.getB()->accept(*this);
	*mStream << dedent;
	return false;
}

bool Visitor::visit(CondValid &c) {
	*mStream << "(condvalid)" << indent << endl;
	*mStream << "[sel]";
	c.getSel()->accept(*this);
	*mStream << "[a]";
	c.getA()->accept(*this);
	*mStream << dedent;
	return false;
}

bool Visitor::visit(PrimOp &op) {
	*mStream << "(" << op.operationName() << ")" << indent << endl;
	return true;
}

void Visitor::leave(PrimOp &op) {
	*mStream << dedent;
}
void Visitor::outputInfo(IRNode &n) {
	if (n.getInfo()) {
		*mStream << ", info=\"" << n.getInfo()->getValue() << "\"";
	}
}

}
}
}
