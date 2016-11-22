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
namespace Frontend {

class FrontendBase {
public:
    virtual ~FrontendBase();
    virtual bool parseString(std::string::const_iterator begin,
                             std::string::const_iterator end) = 0;
    virtual std::shared_ptr<Circuit> getIR();
protected:
    std::shared_ptr<Circuit> mIR;
};

class FrontendFactory
{
public:
	virtual ~FrontendFactory() {}
    virtual std::shared_ptr<FrontendBase> create() = 0;

    virtual std::string getName() = 0;
    virtual std::string getDescription() = 0;
    virtual std::vector<std::string> getFiletypes() = 0;
};

class Registry {
public:
	static void registerFrontend(const std::string &name,
		FrontendFactory * factory);
	static std::shared_ptr<FrontendBase> create(const std::string &name) {
		throwAssert(getPassMap().find(name) != getPassMap().end(),
				"Cannot find frontend: " + name);
		return getPassMap()[name]->create();
	}

	static std::vector<Firrtlator::FrontendDescriptor> getDescriptors() {
		std::vector<Firrtlator::FrontendDescriptor> list;
		for (auto b : getPassMap()) {
			Firrtlator::FrontendDescriptor desc;
			desc.name = b.second->getName();
			desc.description = b.second->getDescription();
			desc.filetypes = b.second->getFiletypes();
			list.push_back(desc);
		}
		return list;
	}
private:
	typedef std::map<std::string, FrontendFactory* > PassMap;
    static PassMap &getPassMap() {
    	static PassMap map;
    	return map;
    }
};

}
}

#define REGISTER_FRONTEND(frontend) \
    class frontend##Factory : public ::Firrtlator::Frontend::FrontendFactory { \
    public: \
        frontend##Factory() \
        { \
            ::Firrtlator::Frontend::Registry::registerFrontend(frontend::name, \
				this); \
        } \
        virtual std::shared_ptr<::Firrtlator::Frontend::FrontendBase> create() { \
            return std::make_shared<frontend>(); \
        } \
        virtual std::string getName() { \
        	return frontend::name; \
        } \
        virtual std::string getDescription() { \
        	return frontend::description; \
        } \
        virtual std::vector<std::string> getFiletypes() { \
            return frontend::filetypes; \
        } \
    }; \
    static frontend##Factory global_##frontend##Factory;
