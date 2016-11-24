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

#include <memory>
#include <string>
#include <fstream>

#include "IR.h"
#include "Firrtlator.h"

namespace Firrtlator {
namespace Pass {

class PassBase {
public:
    virtual ~PassBase();
    virtual void run(std::shared_ptr<Circuit>) = 0;
};

class PassFactory
{
public:
	virtual ~PassFactory() {}
    virtual std::shared_ptr<PassBase> create() = 0;

    virtual std::string getName() = 0;
    virtual std::string getDescription() = 0;
};

class Registry {
public:
	static void registerPass(const std::string &name,
		PassFactory * factory);
	static std::shared_ptr<PassBase> create(const std::string &name) {
		throwAssert(getPassMap().find(name) != getPassMap().end(),
				"Cannot find pass: " + name);
		return getPassMap()[name]->create();
	}

	static std::vector<Firrtlator::PassDescriptor> getDescriptors() {
		std::vector<Firrtlator::PassDescriptor> list;
		for (auto b : getPassMap()) {
			Firrtlator::PassDescriptor desc;
			desc.name = b.second->getName();
			desc.description = b.second->getDescription();
			list.push_back(desc);
		}
		return list;
	}
private:
	typedef std::map<std::string, PassFactory* > PassMap;
    static PassMap &getPassMap() {
    	static PassMap map;
    	return map;
    }
};

}
}

#define REGISTER_PASS(pass) \
    class pass##Factory : public ::Firrtlator::Pass::PassFactory { \
    public: \
        pass##Factory() \
        { \
            ::Firrtlator::Pass::Registry::registerPass(pass::name, \
				this); \
        } \
        virtual std::shared_ptr<::Firrtlator::Pass::PassBase> create() { \
            return std::make_shared<pass>(); \
        } \
        virtual std::string getName() { \
        	return pass::name; \
        } \
        virtual std::string getDescription() { \
        	return pass::description; \
        } \
    }; \
    static pass##Factory global_##pass##Factory;
