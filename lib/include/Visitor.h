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

	virtual bool visit(std::shared_ptr<Circuit>){ return true; };
	virtual void leave(std::shared_ptr<Circuit>){};

	virtual bool visit(std::shared_ptr<Module>){ return true; };
	virtual void leave(std::shared_ptr<Module>){};

	virtual bool visit(std::shared_ptr<Port>){ return true; };
	virtual void leave(std::shared_ptr<Port>){};

	virtual bool visit(std::shared_ptr<Parameter>){ return true; };
	virtual void leave(std::shared_ptr<Parameter>){};

	virtual void visit(std::shared_ptr<TypeInt>){};

	virtual void visit(std::shared_ptr<TypeClock>){};

	virtual bool visit(std::shared_ptr<Field>){ return true; };
	virtual void leave(std::shared_ptr<Field>){};

	virtual bool visit(std::shared_ptr<TypeBundle>){ return true; };
	virtual void leave(std::shared_ptr<TypeBundle>){};

	virtual bool visit(std::shared_ptr<TypeVector>){ return true; };
	virtual void leave(std::shared_ptr<TypeVector>){};

	virtual bool visit(std::shared_ptr<StmtGroup>){ return true; };
	virtual void leave(std::shared_ptr<StmtGroup>){};

	virtual bool visit(std::shared_ptr<Wire>){ return true; };
	virtual void leave(std::shared_ptr<Wire>){};

	virtual bool visit(std::shared_ptr<Reg>){ return true; };
	virtual void leave(std::shared_ptr<Reg>){};

	virtual bool visit(std::shared_ptr<Instance>){ return true; };
	virtual void leave(std::shared_ptr<Instance>){};

	virtual bool visit(std::shared_ptr<Memory>){ return true; };
	virtual void leave(std::shared_ptr<Memory>){};

	virtual bool visit(std::shared_ptr<Node>){ return true; };
	virtual void leave(std::shared_ptr<Node>){};

	virtual bool visit(std::shared_ptr<Connect>){ return true; };
	virtual void leave(std::shared_ptr<Connect>){};

	virtual bool visit(std::shared_ptr<Invalid>){ return true; };
	virtual void leave(std::shared_ptr<Invalid>){};

	virtual bool visit(std::shared_ptr<Conditional>){ return true; };
	virtual void leave(std::shared_ptr<Conditional>){};

	virtual bool visit(std::shared_ptr<ConditionalElse>){ return true; };
	virtual void leave(std::shared_ptr<ConditionalElse>){};

	virtual bool visit(std::shared_ptr<Stop>){ return true; };
	virtual void leave(std::shared_ptr<Stop>){};

	virtual bool visit(std::shared_ptr<Printf>){ return true; };
	virtual void leave(std::shared_ptr<Printf>){};

	virtual void visit(std::shared_ptr<Empty>){};

	virtual void visit(std::shared_ptr<Reference>){};

	virtual void visit(std::shared_ptr<Constant>){};

	virtual bool visit(std::shared_ptr<SubField>){ return true; };
	virtual void leave(std::shared_ptr<SubField>){};

	virtual bool visit(std::shared_ptr<SubIndex>){ return true; };
	virtual void leave(std::shared_ptr<SubIndex>){};

	virtual bool visit(std::shared_ptr<SubAccess>){ return true; };
	virtual void leave(std::shared_ptr<SubAccess>){};

	virtual bool visit(std::shared_ptr<Mux>){ return true; };
	virtual void leave(std::shared_ptr<Mux>){};

	virtual bool visit(std::shared_ptr<CondValid>){ return true; };
	virtual void leave(std::shared_ptr<CondValid>){};

	virtual bool visit(std::shared_ptr<PrimOp>){ return true; };
	virtual void leave(std::shared_ptr<PrimOp>){};

	// TODO: all PrimOps and default calls base
};

}
