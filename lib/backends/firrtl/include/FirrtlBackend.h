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

#pragma once

#include "FirrtlatorBackend.h"
#include "Visitor.h"

namespace Firrtlator {
namespace Backend {
namespace Firrtl {

class Backend : public ::Firrtlator::Backend::BackendBase {
public:
	virtual void generate(std::shared_ptr<Circuit> ir);
};

class Visitor : public ::Firrtlator::Visitor {
public:
	virtual ~Visitor();
	virtual void visit(Circuit &);
	virtual void visit(Module &);
	virtual void visit(Port &);
	virtual void visit(Parameter &);
	virtual void visit(TypeInt &);
	virtual void visit(TypeClock &);
	virtual void visit(Field &);
	virtual void visit(TypeBundle &);
	virtual void visit(TypeVector &);
	virtual void visit(Wire &);
	virtual void visit(Reg &);
	virtual void visit(Instance &);
	virtual void visit(Memory &);
	virtual void visit(Node &);
	virtual void visit(Connect &);
	virtual void visit(Invalid &);
	virtual void visit(Conditional &);
	virtual void visit(Stop &);
	virtual void visit(Printf &);
	virtual void visit(Empty &);
	virtual void visit(Reference &);
	virtual void visit(Constant &);
	virtual void visit(SubField &);
	virtual void visit(SubIndex &);
	virtual void visit(SubAccess &);
	virtual void visit(Mux &);
	virtual void visit(CondValid &);
	virtual void visit(PrimOp &);
};

}
}
}
