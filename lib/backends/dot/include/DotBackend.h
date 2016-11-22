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

#include <iostream>
#include <map>

namespace Firrtlator {
namespace Backend {
namespace Dot {

class Backend : public ::Firrtlator::Backend::BackendBase {
public:
	Backend(std::ostream &os);
	virtual void generate(std::shared_ptr<Circuit> ir);
	static std::string name;
	static std::string description;
	static std::vector<std::string> filetypes;
};

class Visitor : public ::Firrtlator::Visitor {
public:
	Visitor(std::ostream *os);

	virtual ~Visitor();
	virtual bool visit(Circuit &);
	virtual void leave(Circuit &);

	virtual bool visit(Module &);
	virtual void leave(Module &);

	virtual bool visit(Port &);
	virtual void leave(Port &);

	virtual bool visit(Parameter &);

	virtual void visit(TypeInt &);

	virtual void visit(TypeClock &);

	virtual bool visit(Field &);

	virtual bool visit(TypeBundle &);

	virtual bool visit(TypeVector &);

	virtual bool visit(StmtGroup &);

	virtual bool visit(Wire &);

	virtual bool visit(Reg &);

	virtual bool visit(Instance &);

	virtual bool visit(Memory &);

	virtual bool visit(Node &);

	virtual bool visit(Connect &);

	virtual bool visit(Invalid &);
	virtual void leave(Invalid &);

	virtual bool visit(Conditional &);

	virtual bool visit(ConditionalElse &);

	virtual bool visit(Stop &);

	virtual bool visit(Printf &);

	virtual void visit(Empty &);

	virtual void visit(Reference &);

	virtual void visit(Constant &);

	virtual bool visit(SubField &);

	virtual bool visit(SubIndex &);

	virtual bool visit(SubAccess &);

	virtual bool visit(Mux &);

	virtual bool visit(CondValid &);

	virtual bool visit(PrimOp &);
private:
	std::ostream *mStream;
	std::vector<std::string> mNodes;
	std::vector<std::tuple<IRNode*, IRNode*, std::string> > mEdges;

	std::map<IRNode*, int> mNodeDictionary;

	void addNode(IRNode* node, std::string text);
	void addEdge(IRNode* from, IRNode* to, std::string text);
};

}
}
}
