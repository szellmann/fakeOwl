#include <cstring>

#include <visionaray/detail/parallel_for.h>

#include "Context.h"
#include "Logging.h"
#include "Module.h"
#include "Params.h"
#include "RayGen.h"
#include "Trace.h"

void* optixLaunchParams;

namespace fake
{
    RayGen::RayGen(std::size_t sizeOfVarStruct, OWLVarDecl* vars, int numVars)
        : VarDecl(sizeOfVarStruct, vars, numVars)
    {
    }

    void RayGen::launch(int dims, Params* params)
    {
        using namespace visionaray;

        FAKE_LOG_DBG << "Launching ray gen program with params: " << name << " (dims=" << dims << ')';

        if (!isValid())
        {
            FAKE_LOG(fake::logging::Level::Error) << "Cannot launch ray gen program: " << name << " - forgot to call buildPrograms()?!";;
            return;
        }

        if (params != nullptr)
        {
            void* optixLaunchParams;
            optixLaunchParams = (void*)module->optixLaunchParamsSym;
            std::memcpy(optixLaunchParams, params->dataPtr, params->sizeOfVarStruct);
        }

        // Update all traversables to have a pointer to the current module!
        traversablesSetCurrentModule(module);

        context->backend.parallel_for(0, dims, 256,
            [=](int i)
            {
                void (*entryPoint)();
                entryPoint = (void (*)())entryPointSym;

                fake::ProgramState* (*fakePrepareRayGen)();
                fakePrepareRayGen = (fake::ProgramState* (*)())fakePrepareRayGenSym;

                void (*fakeResetPreviousProgramState)();
                fakeResetPreviousProgramState
                        = (void (*)())fakeResetPreviousProgramStateSym;

                void (*fakeSetLaunchIndex)(int, int, int);
                fakeSetLaunchIndex = (void (*)(int, int, int))fakeSetLaunchIndexSym;

                void (*fakeSetLaunchDims)(int, int, int);
                fakeSetLaunchDims = (void (*)(int, int, int))fakeSetLaunchDimsSym;


                fake::ProgramState* state = fakePrepareRayGen();
                state->sbtPointer = this->dataPtr;

                fakeSetLaunchDims(dims, 1, 1);

                fakeSetLaunchIndex(i, 0, 0);
                entryPoint();

                fakeResetPreviousProgramState();
            }
            );

        FAKE_LOG_DBG << "Success!";
    }

    void RayGen::launch(int w, int h, Params* params)
    {
        using namespace visionaray;

        FAKE_LOG_DBG << "Launching ray gen program with params: " << name << " (w=" << w << ",h=" << h << ')';

        if (!isValid())
        {
            FAKE_LOG(fake::logging::Level::Error) << "Cannot launch ray gen program: " << name << " - forgot to call buildPrograms()?!";;
            return;
        }

        if (params != nullptr)
        {
            //void* optixLaunchParams;
            optixLaunchParams = (void*)module->optixLaunchParamsSym;
            std::memcpy(optixLaunchParams, params->dataPtr, params->sizeOfVarStruct);
        }

        // Update all traversables to have a pointer to the current module!
        traversablesSetCurrentModule(module);

        context->backend.parallel_for(0, w, 16, 0, h, 16,
            [=](int x, int y)
            {
                void (*entryPoint)();
                entryPoint = (void (*)())entryPointSym;

                fake::ProgramState* (*fakePrepareRayGen)();
                fakePrepareRayGen = (fake::ProgramState* (*)())fakePrepareRayGenSym;

                void (*fakeResetPreviousProgramState)();
                fakeResetPreviousProgramState
                        = (void (*)())fakeResetPreviousProgramStateSym;

                void (*fakeSetLaunchIndex)(int, int, int);
                fakeSetLaunchIndex = (void (*)(int, int, int))fakeSetLaunchIndexSym;

                void (*fakeSetLaunchDims)(int, int, int);
                fakeSetLaunchDims = (void (*)(int, int, int))fakeSetLaunchDimsSym;


                fake::ProgramState* state = fakePrepareRayGen();
                state->sbtPointer = this->dataPtr;

                fakeSetLaunchDims(w, h, 1);

                fakeSetLaunchIndex(x, y, 0);
                entryPoint();
                fakeResetPreviousProgramState();
            }
            );

        FAKE_LOG_DBG << "Success!";
    }

    std::string RayGen::entryPointPrefix()
    {
        return "__raygen__";
    }
} // fake
