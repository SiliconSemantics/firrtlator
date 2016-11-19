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

#include <iostream>
#include "../../generic/include/StreamIndentation.h"

namespace Firrtlator {
namespace Backend {
namespace Firrtl {

void Backend::generate(std::shared_ptr<Circuit> ir) {
	std::cout << "Test" << endl;
	std::cout << *ir;
}


Visitor::~Visitor() {

}

void Visitor::visit(Circuit &) { }
void Visitor::visit(Module &) { }
void Visitor::visit(Port &) { }
void Visitor::visit(Parameter &) { }
void Visitor::visit(TypeInt &) { }
void Visitor::visit(TypeClock &) { }
void Visitor::visit(Field &) { }
void Visitor::visit(TypeBundle &) { }
void Visitor::visit(TypeVector &) { }
void Visitor::visit(Wire &) { }
void Visitor::visit(Reg &) { }
void Visitor::visit(Instance &) { }
void Visitor::visit(Memory &) { }
void Visitor::visit(Node &) { }
void Visitor::visit(Connect &) { }
void Visitor::visit(Invalid &) { }
void Visitor::visit(Conditional &) { }
void Visitor::visit(Stop &) { }
void Visitor::visit(Printf &) { }
void Visitor::visit(Empty &) { }
void Visitor::visit(Reference &) { }
void Visitor::visit(Constant &) { }
void Visitor::visit(SubField &) { }
void Visitor::visit(SubIndex &) { }
void Visitor::visit(SubAccess &) { }
void Visitor::visit(Mux &) { }
void Visitor::visit(CondValid &) { }
void Visitor::visit(PrimOp &) { }

}
}
}
