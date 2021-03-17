#include <sstream>

#include <dlfcn.h>

#include "Logging.h"
#include "Module.h"
#include "Program.h"

namespace fake
{
    Program::~Program()
    {
    }

    void Program::build()
    {
        FAKE_LOG_DBG << "Building program: " << name;
        std::stringstream str;
        str << entryPointPrefix() << name;
        std::string s = str.str();
        FAKE_LOG_DBG << "Qualified name of program: " << s;

        entryPointSym = dlsym(module->handle, s.c_str());

        fakeSetLaunchIndexSym = dlsym(module->handle, "fakeSetLaunchIndex");
        fakeSetLaunchDimsSym = dlsym(module->handle, "fakeSetLaunchDims");

        if (entryPointSym != nullptr)
            FAKE_LOG_DBG << "Success!";
        else
            FAKE_LOG(fake::logging::Level::Error) << "Could not create program: " << s;
    }

    bool Program::isValid() const
    {
        return entryPointSym && fakeSetLaunchIndexSym && fakeSetLaunchDimsSym;
    }
} // fake
