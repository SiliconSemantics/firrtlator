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

	virtual bool visit(std::shared_ptr<Circuit>) = 0;
	virtual void leave(std::shared_ptr<Circuit>){};

	virtual bool visit(std::shared_ptr<Module>) = 0;
	virtual void leave(std::shared_ptr<Module>){};

	virtual bool visit(std::shared_ptr<Port>) = 0;
	virtual void leave(std::shared_ptr<Port>){};

	virtual bool visit(std::shared_ptr<Parameter>) = 0;
	virtual void leave(std::shared_ptr<Parameter>){};

	virtual void visit(std::shared_ptr<TypeInt>) = 0;

	virtual void visit(std::shared_ptr<TypeClock>) = 0;

	virtual bool visit(std::shared_ptr<Field>) = 0;
	virtual void leave(std::shared_ptr<Field>){};

	virtual bool visit(std::shared_ptr<TypeBundle>) = 0;
	virtual void leave(std::shared_ptr<TypeBundle>){};

	virtual bool visit(std::shared_ptr<TypeVector>) = 0;
	virtual void leave(std::shared_ptr<TypeVector>){};

	virtual bool visit(std::shared_ptr<StmtGroup>) = 0;
	virtual void leave(std::shared_ptr<StmtGroup>){};

	virtual bool visit(std::shared_ptr<Wire>) = 0;
	virtual void leave(std::shared_ptr<Wire>){};

	virtual bool visit(std::shared_ptr<Reg>) = 0;
	virtual void leave(std::shared_ptr<Reg>){};

	virtual bool visit(std::shared_ptr<Instance>) = 0;
	virtual void leave(std::shared_ptr<Instance>){};

	virtual bool visit(std::shared_ptr<Memory>) = 0;
	virtual void leave(std::shared_ptr<Memory>){};

	virtual bool visit(std::shared_ptr<Node>) = 0;
	virtual void leave(std::shared_ptr<Node>){};

	virtual bool visit(std::shared_ptr<Connect>) = 0;
	virtual void leave(std::shared_ptr<Connect>){};

	virtual bool visit(std::shared_ptr<Invalid>) = 0;
	virtual void leave(std::shared_ptr<Invalid>){};

	virtual bool visit(std::shared_ptr<Conditional>) = 0;
	virtual void leave(std::shared_ptr<Conditional>){};

	virtual bool visit(std::shared_ptr<ConditionalElse>) = 0;
	virtual void leave(std::shared_ptr<ConditionalElse>){};

	virtual bool visit(std::shared_ptr<Stop>) = 0;
	virtual void leave(std::shared_ptr<Stop>){};

	virtual bool visit(std::shared_ptr<Printf>) = 0;
	virtual void leave(std::shared_ptr<Printf>){};

	virtual void visit(std::shared_ptr<Empty>) = 0;

	virtual void visit(std::shared_ptr<Reference>) = 0;

	virtual void visit(std::shared_ptr<Constant>) = 0;

	virtual bool visit(std::shared_ptr<SubField>) = 0;
	virtual void leave(std::shared_ptr<SubField>){};

	virtual bool visit(std::shared_ptr<SubIndex>) = 0;
	virtual void leave(std::shared_ptr<SubIndex>){};

	virtual bool visit(std::shared_ptr<SubAccess>) = 0;
	virtual void leave(std::shared_ptr<SubAccess>){};

	virtual bool visit(std::shared_ptr<Mux>) = 0;
	virtual void leave(std::shared_ptr<Mux>){};

	virtual bool visit(std::shared_ptr<CondValid>) = 0;
	virtual void leave(std::shared_ptr<CondValid>){};

	virtual bool visit(std::shared_ptr<PrimOp>) = 0;
	virtual void leave(std::shared_ptr<PrimOp>){};

	// TODO: all PrimOps and default calls base
};

}
