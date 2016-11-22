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
#include "Firrtlator.h"

namespace Firrtlator {
namespace Backend {

class BackendBase {
public:
    virtual ~BackendBase() {};
    BackendBase(std::ostream &os) : mStream(&os) {}
    virtual void generate(std::shared_ptr<Circuit> ir) = 0;
protected:
	std::ostream *mStream;
};

class BackendFactory
{
public:
	virtual ~BackendFactory() {}
    virtual std::shared_ptr<BackendBase> create(std::ostream &os) = 0;

    virtual std::string getName() = 0;
    virtual std::string getDescription() = 0;
    virtual std::vector<std::string> getFiletypes() = 0;
};

class Registry {
public:
	static void registerBackend(const std::string &name,
		BackendFactory * factory);
	static std::shared_ptr<BackendBase> create(const std::string &name, std::ostream &os) {
		throwAssert(getBackendMap().find(name) != getBackendMap().end(),
				"Cannot find backend: " + name);
		return getBackendMap()[name]->create(os);
	}

	static std::vector<Firrtlator::BackendDescriptor> getDescriptors() {
		std::vector<Firrtlator::BackendDescriptor> list;
		for (auto b : getBackendMap()) {
			Firrtlator::BackendDescriptor desc;
			desc.name = b.second->getName();
			desc.description = b.second->getDescription();
			desc.filetypes = b.second->getFiletypes();
			list.push_back(desc);
		}
		return list;
	}
private:
	typedef std::map<std::string, BackendFactory* > BackendMap;
    static BackendMap &getBackendMap() {
    	static BackendMap map;
    	return map;
    }
};

}
}

#define REGISTER_BACKEND(backend) \
    class backend##Factory : public ::Firrtlator::Backend::BackendFactory { \
    public: \
        backend##Factory() \
        { \
            ::Firrtlator::Backend::Registry::registerBackend(backend::name, \
				this); \
        } \
        virtual std::shared_ptr<::Firrtlator::Backend::BackendBase> create(std::ostream &os) { \
            return std::make_shared<backend>(os); \
        } \
        virtual std::string getName() { \
        	return backend::name; \
        } \
        virtual std::string getDescription() { \
        	return backend::description; \
        } \
        virtual std::vector<std::string> getFiletypes() { \
            return backend::filetypes; \
        } \
    }; \
    static backend##Factory global_##backend##Factory;
