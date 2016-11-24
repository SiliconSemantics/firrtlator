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

#include "../../stripinfo/include/StripInfo.h"

#include "StreamIndentation.h"

#include <iostream>


namespace Firrtlator {
namespace Pass {
namespace StripInfo {

std::string Pass::name = "stripinfo";
std::string Pass::description = "Remove all info nodes";

REGISTER_PASS(Pass)

Pass::Pass() : PassBase() {

}

void Pass::run(std::shared_ptr<Circuit> ir) {
	Visitor v;

	ir->accept(v);
}

Visitor::Visitor() {

}

Visitor::~Visitor() {

}

bool Visitor::visit(std::shared_ptr<Circuit> c) {
	c->setInfo(nullptr);
	return true;
}

bool Visitor::visit(std::shared_ptr<Module> m) {
	m->setInfo(nullptr);
	return true;
}

bool Visitor::visit(std::shared_ptr<Port> p) {
	p->setInfo(nullptr);
	return false;
}

bool Visitor::visit(std::shared_ptr<Wire> w) {
	w->setInfo(nullptr);
	return false;
}

bool Visitor::visit(std::shared_ptr<Reg> r) {
	r->setInfo(nullptr);
	return false;
}

bool Visitor::visit(std::shared_ptr<Instance> i) {
	i->setInfo(nullptr);
	return false;
}

bool Visitor::visit(std::shared_ptr<Memory> m) {
	m->setInfo(nullptr);
	return false;
}
bool Visitor::visit(std::shared_ptr<Node> n) {
	n->setInfo(nullptr);
	return false;
}

bool Visitor::visit(std::shared_ptr<Connect> c) {
	c->setInfo(nullptr);
	return false;
}

bool Visitor::visit(std::shared_ptr<Invalid> i) {
	i->setInfo(nullptr);
	return false;
}

bool Visitor::visit(std::shared_ptr<Conditional> c) {
	c->setInfo(nullptr);
	return true;
}

bool Visitor::visit(std::shared_ptr<ConditionalElse> c) {
	c->setInfo(nullptr);
	return true;
}

bool Visitor::visit(std::shared_ptr<Stop> s) {
	s->setInfo(nullptr);
	return false;
}

bool Visitor::visit(std::shared_ptr<Printf> p) {
	p->setInfo(nullptr);
	return false;
}

void Visitor::visit(std::shared_ptr<Empty> e) {
	e->setInfo(nullptr);
}

}
}
}
