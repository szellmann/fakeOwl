
#pragma once

#include "Program.h"
#include "VarDecl.h"

namespace fake
{
    class Miss : public Program, public VarDecl
    {
    public:
        Miss(std::size_t sizeOfVarStruct, OWLVarDecl* vars, int numVars);

        std::string entryPointPrefix();

        int rayType = 0;

        // Symbol to prepare an intersection program call
        void* fakePrepareMissSym = nullptr;
    };
} // fake
