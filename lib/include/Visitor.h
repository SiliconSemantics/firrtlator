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

#include "IR.h"

namespace Firrtlator {

class Visitor {
public:
	virtual ~Visitor();

	virtual bool visit(Circuit &) = 0;
	virtual void leave(Circuit &){};

	virtual bool visit(Module &) = 0;
	virtual void leave(Module &){};

	virtual bool visit(Port &) = 0;
	virtual void leave(Port &){};

	virtual bool visit(Parameter &) = 0;
	virtual void leave(Parameter &){};

	virtual void visit(TypeInt &) = 0;

	virtual void visit(TypeClock &) = 0;

	virtual bool visit(Field &) = 0;
	virtual void leave(Field &){};

	virtual bool visit(TypeBundle &) = 0;
	virtual void leave(TypeBundle &){};

	virtual bool visit(TypeVector &) = 0;
	virtual void leave(TypeVector &){};

	virtual bool visit(Wire &) = 0;
	virtual void leave(Wire &){};

	virtual bool visit(Reg &) = 0;
	virtual void leave(Reg &){};

	virtual bool visit(Instance &) = 0;
	virtual void leave(Instance &){};

	virtual bool visit(Memory &) = 0;
	virtual void leave(Memory &){};

	virtual bool visit(Node &) = 0;
	virtual void leave(Node &){};

	virtual bool visit(Connect &) = 0;
	virtual void leave(Connect &){};

	virtual bool visit(Invalid &) = 0;
	virtual void leave(Invalid &){};

	virtual bool visit(Conditional &) = 0;
	virtual void leave(Conditional &){};

	virtual bool visit(Stop &) = 0;
	virtual void leave(Stop &){};

	virtual bool visit(Printf &) = 0;
	virtual void leave(Printf &){};

	virtual void visit(Empty &) = 0;

	virtual void visit(Reference &) = 0;

	virtual void visit(Constant &) = 0;

	virtual bool visit(SubField &) = 0;
	virtual void leave(SubField &){};

	virtual bool visit(SubIndex &) = 0;
	virtual void leave(SubIndex &){};

	virtual bool visit(SubAccess &) = 0;
	virtual void leave(SubAccess &){};

	virtual bool visit(Mux &) = 0;
	virtual void leave(Mux &){};

	virtual bool visit(CondValid &) = 0;
	virtual void leave(CondValid &){};

	virtual bool visit(PrimOp &) = 0;
	virtual void leave(PrimOp &){};

	// TODO: all PrimOps and default calls base
};

}
