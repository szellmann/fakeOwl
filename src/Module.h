
#pragma once

#include <cstddef>
#include <vector>

#include <owl/owl_host.h>

#include "Miss.h"
#include "Program.h"
#include "Resource.h"

namespace fake
{
    class Module : public ContextResource
    {
    public:
        Module(const char* lib);
       ~Module();

        void buildPrograms();

        Program* createProgram(Program::Type pt, const char* name,
                               std::size_t sizeOfVarStruct = 0,
                               OWLVarDecl* vars = nullptr,
                               int numVars = 0);
        void releaseProgram(Program* program);

        void setMissProg(int rayType, Miss* missProg);
        bool haveMissProg(int rayType);
        Miss* getMissProg(int rayType);

        // Handle to the dlopen()'d shared library for this module
        void* handle = nullptr;

        // Symbol handle of the optixLaunchParams variable
        void* optixLaunchParamsSym = nullptr;

        // Symbol to prepare an intersection program call
        void* fakePrepareRayGenSym = nullptr;

        // Symbol to prepare an intersection program call
        void* fakePrepareIntersectionSym = nullptr;

        // Symbo to obtain result from intersection program call
        void* fakeGetIntersectionResultSym = nullptr;

        // Symbol to prepare a closest hit program call
        void* fakePrepareClosestHitSym = nullptr;

        // Symbol to prepare an intersection program call
        void* fakePrepareMissSym = nullptr;

        // Symbol to reset the state to the previous program
        void* fakeResetPreviousProgramStateSym = nullptr;

    private:

        std::vector<Miss*> miss;

        struct ProgramEntry
        {
            Program::Type type;
            ResourceHandle handle;
        };
        std::vector<ProgramEntry> programEntries;

    };

} // fake
