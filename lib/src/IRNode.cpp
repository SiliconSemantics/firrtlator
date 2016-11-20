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

#include <IR.h>

#include <iostream>

namespace Firrtlator {

IRNode::~IRNode() {}

IRNode::IRNode() : IRNode("") {}

IRNode::IRNode(std::string id) : mId(id) {}

std::string IRNode::getId() { return mId; }

void IRNode::setId(std::string id) { mId = id; }

std::shared_ptr<Info> IRNode::getInfo() { return mInfo; }

void IRNode::setInfo(std::shared_ptr<Info> info) {
	mInfo = info;
}

bool IRNode::isDeclaration() { return (mId.length() != 0); }

Info::Info(std::string value) : mValue(value) {}

void Info::setValue(std::string value) { mValue = value; }

std::string Info::getValue() { return mValue; }

std::ostream& operator<< (std::ostream &os, const Info &info)
{
	if (info.mValue.length() > 0) {
		os << " @[" << info.mValue << "] ";
	}
    return os;
}

}

