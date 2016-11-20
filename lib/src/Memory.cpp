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
#include "Util.h"

namespace Firrtlator {

Memory::Memory(std::string id) : Stmt(id) {}

void Memory::setDType(std::shared_ptr<Type> type) {
	throwAssert((type != nullptr), "Invalid memory type");
	throwAssert((mDType == nullptr), "Memory type already set");

	mDType = type;
}

void Memory::setDepth(int depth) {
	throwAssert((depth >= 0), "Invalid memory depth");
	throwAssert((mDepth == -1), "Memory depth already set");

	mDepth = depth;
}

void Memory::setReadLatency(int lat) {
	throwAssert((lat >= 0), "Invalid memory read latency");
	throwAssert((mReadlatency == -1), "Memory read latency already set");

	mDepth = lat;
}

void Memory::setWriteLatency(int lat) {
	throwAssert((lat >= 0), "Invalid memory write latency");
	throwAssert((mWritelatency == -1), "Memory write latency already set");

	mWritelatency = lat;
}

void Memory::setRuwFlag(RuwFlag flag) {
	throwAssert(!mRuwflagset, "Memory RUW flag already set");

	mRuwflagset = true;
	mRuwflag = flag;
}

void Memory::addReader(std::string r) {
	throwAssert((std::find(mReaders.begin(), mReaders.end(), (r))
	== mReaders.end()), "Reader already defined");

	if (!checkAndUpdateDeferedType())
		addReaderToType(r);

	mReaders.push_back(r);
}

void Memory::addWriter(std::string w) {
	throwAssert((std::find(mWriters.begin(), mWriters.end(), (w))
	== mWriters.end()), "writer already defined");

	if (!checkAndUpdateDeferedType())
		addWriterToType(w);

	mWriters.push_back(w);
}

void Memory::addReadWriter(std::string rw) {
	throwAssert((std::find(mReadWriters.begin(), mReadWriters.end(), (rw))
	== mReadWriters.end()), "ReadWriter already defined");

	if (!checkAndUpdateDeferedType())
		addReadWriterToType(rw);

	mReadWriters.push_back(rw);
}

std::shared_ptr<Type> Memory::getDType() {
	return mDType;
}

std::shared_ptr<TypeBundle> Memory::getType() {
	return mType;
}

std::vector<std::string> Memory::getReaders() {
	return mReaders;
}

std::vector<std::string> Memory::getWriters() {
	return mWriters;
}

std::vector<std::string> Memory::getReadWriters() {
	return mReadWriters;
}

int Memory::getDepth() {
	return mDepth;
}

int Memory::getReadlatency() {
	return mReadlatency;
}

int Memory::getWritelatency() {
	return mWritelatency;
}

Memory::RuwFlag Memory::getRuwflag() {
	return mRuwflag;
}

bool Memory::checkAndUpdateDeferedType() {
	// TODO: Add further defer reasons
	if (mDType == nullptr)
		return false;

	if (mType != nullptr)
		return true;

	mType = std::make_shared<TypeBundle>();

	for (auto r : mReaders)
		addReaderToType(r);
	for (auto w : mWriters)
		addWriterToType(w);
	for (auto rw : mReadWriters)
		addReadWriterToType(rw);

	return true;
}

void Memory::addReaderToType(std::string r) {
	std::shared_ptr<TypeBundle> bundle;
	bundle = std::make_shared<TypeBundle>();
	//bundle->addField(std::make_shared<Field>("data", TODO, true)); // Copy constructor
	//bundle->addField(std::make_shared<Field>("addr", TODO)); // Width inference
	bundle->addField(std::make_shared<Field>("en",
			std::make_shared<TypeInt>(false, 1)));
	bundle->addField(std::make_shared<Field>("clk",
			std::make_shared<TypeClock>()));
	mType->addField(std::make_shared<Field>(r, bundle));
}

void Memory::addWriterToType(std::string w) {
	std::shared_ptr<TypeBundle> bundle;
	bundle = std::make_shared<TypeBundle>();
	//bundle->addField(std::make_shared<Field>("data", TODO, true)); // Copy constructor
	//bundle->addField(std::make_shared<Field>("mask", TODO, true)); // Inference
	//bundle->addField(std::make_shared<Field>("addr", TODO)); // Width inference
	bundle->addField(std::make_shared<Field>("en",
			std::make_shared<TypeInt>(false, 1)));
	bundle->addField(std::make_shared<Field>("clk",
			std::make_shared<TypeClock>()));
	mType->addField(std::make_shared<Field>(w, bundle));
}

void Memory::addReadWriterToType(std::string rw) {
	std::shared_ptr<TypeBundle> bundle;
	bundle = std::make_shared<TypeBundle>();
	//bundle->addField(std::make_shared<Field>("data", TODO, true)); // Copy constructor
	//bundle->addField(std::make_shared<Field>("mask", TODO, true)); // Inference
	//bundle->addField(std::make_shared<Field>("addr", TODO)); // Width inference
	bundle->addField(std::make_shared<Field>("en",
			std::make_shared<TypeInt>(false, 1)));
	bundle->addField(std::make_shared<Field>("clk",
			std::make_shared<TypeClock>()));
	bundle->addField(std::make_shared<Field>("wmode",
			std::make_shared<TypeInt>(false, 1)));
	mType->addField(std::make_shared<Field>(rw, bundle));
}

void Memory::accept(Visitor& v) {

}

}
