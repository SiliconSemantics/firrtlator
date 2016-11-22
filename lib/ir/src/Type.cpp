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

#include "../backends/generic/include/StreamIndentation.h"
#include "IR.h"
#include "Visitor.h"

namespace Firrtlator {

Type::Type() : mBasetype(UNDEFINED) {}

Type::Type(Basetype type) : mBasetype(type) {}

TypeInt::TypeInt() : TypeInt(false) {}

TypeInt::TypeInt(bool sign, int width)
: Type(INT), mWidth(width), mSigned(sign) {}

void TypeInt::setWidth(int width) {
	mWidth = width;
}

int TypeInt::getWidth() {
	return mWidth;
}

bool TypeInt::getSigned() {
	return mSigned;
}

void TypeInt::setSigned(bool sign) {
	mSigned = sign;
}

void TypeInt::accept(Visitor& v) {
	v.visit(*this);
}

TypeClock::TypeClock() : Type(CLOCK) { }

void TypeClock::accept(Visitor& v) {
	v.visit(*this);
}

Field::Field() : Field("", nullptr) {}
Field::Field(std::string id, std::shared_ptr<Type> type, bool flip)
: IRNode(id), mFlip(flip), mType(type) {}

void Field::setType(std::shared_ptr<Type> t) {
	mType = t;
}

void Field::setFlip(bool flip) {
	mFlip = flip;
}

std::shared_ptr<Type> Field::getType() {
	return mType;
}

bool Field::getFlip() {
	return mFlip;
}

void Field::accept(Visitor& v) {

}

TypeBundle::TypeBundle() : Type(BUNDLE) {}
void TypeBundle::addField(std::shared_ptr<Field> field) {
	mFields.push_back(field);
}

std::vector<std::shared_ptr<Field> > TypeBundle::getFields() {
	return mFields;
}

void TypeBundle::accept(Visitor& v) {

}

TypeVector::TypeVector() : Type(VECTOR), mSize(0) { }

void TypeVector::setSize(int size) {
	mSize = size;
}

int TypeVector::getSize() {
	return mSize;
}

void TypeVector::accept(Visitor& v) {

}

}

