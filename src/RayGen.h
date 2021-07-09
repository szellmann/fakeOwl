
#pragma once

#include "Program.h"
#include "VarDecl.h"

namespace fake
{
    class Params;

    class RayGen : public Program, public VarDecl
    {
    public:
        RayGen(std::size_t sizeOfVarStruct, OWLVarDecl* vars, int numVars);

        void launch(int dims, Params* params = nullptr);

        void launch(int w, int h, Params* params = nullptr);

        std::string entryPointPrefix();

        // Symbol to prepare an intersection program call
        void* fakePrepareRayGenSym = nullptr;

    };
} // fake
