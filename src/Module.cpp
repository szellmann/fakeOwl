#include <dlfcn.h>

#include "Bounds.h"
#include "ClosestHit.h"
#include "Intersect.h"
#include "Logging.h"
#include "Miss.h"
#include "Module.h"
#include "RayGen.h"

namespace fake
{
    Module::Module(const char* lib)
    {
        FAKE_LOG_DBG << "Creating module from library: " << lib;
        handle = dlopen(lib, RTLD_NOW);
        if (handle != nullptr)
            FAKE_LOG_DBG << "Success!";
        else
        {
            FAKE_LOG(fake::logging::Level::Error) << "Could not create module from library: " << lib;
            return;
        }

        optixLaunchParamsSym = dlsym(handle, "optixLaunchParams");
        if (optixLaunchParamsSym != nullptr)
            FAKE_LOG_DBG << "Module has optixLaunchParams";

        fakePrepareRayGenSym = dlsym(handle, "fakePrepareRayGen");
        if (fakePrepareRayGenSym == nullptr)
        {
            FAKE_LOG(fake::logging::Level::Error) << "Missing symbol for fakePrepareRayGen(): " << lib;
        }

        fakePrepareIntersectionSym = dlsym(handle, "fakePrepareIntersection");
        if (fakePrepareIntersectionSym == nullptr)
        {
            FAKE_LOG(fake::logging::Level::Error) << "Missing symbol for fakePrepareIntersection(): " << lib;
        }

        fakeGetIntersectionResultSym = dlsym(handle, "fakeGetIntersectionResult");
        if (fakeGetIntersectionResultSym == nullptr)
        {
            FAKE_LOG(fake::logging::Level::Error) << "Missing symbol for fakeGetIntersectionResult(): " << lib;
        }

        fakePrepareClosestHitSym = dlsym(handle, "fakePrepareClosestHit");
        if (fakePrepareClosestHitSym == nullptr)
        {
            FAKE_LOG(fake::logging::Level::Error) << "Missing symbol for fakePrepareClosestHit(): " << lib;
        }

        fakePrepareMissSym = dlsym(handle, "fakePrepareMiss");
        if (fakePrepareMissSym == nullptr)
        {
            FAKE_LOG(fake::logging::Level::Error) << "Missing symbol for fakePrepareMiss(): " << lib;
        }

        fakeResetPreviousProgramStateSym = dlsym(handle, "fakeResetPreviousProgramState");
        if (fakeResetPreviousProgramStateSym == nullptr)
        {
            FAKE_LOG(fake::logging::Level::Error) << "Missing symbol for fakeResetPreviousProgramState(): " << lib;
        }
    }

    Module::~Module()
    {
        FAKE_LOG_DBG << "Releasing module";

        // Destroy all programs associated with this module
        for (size_t i = 0; i < programEntries.size(); ++i)
        {
            Program* program = (Program*)GetManagedResource(programEntries[i].handle);
            FAKE_LOG_DBG << "Releasing program (type " << program->type << "): " << program->name;
            delete program;
            UnregisterManagedResource(programEntries[i].handle);
        }

        dlclose(handle);
    }

    void Module::buildPrograms()
    {
        for (size_t i = 0; i < programEntries.size(); ++i)
        {
            Program* program = (Program*)GetManagedResource(programEntries[i].handle);
            FAKE_LOG_DBG << "Building program (type " << program->type << "): " << program->name;
            program->build();
        }
    }

    Program* Module::createProgram(Program::Type pt, const char* name,
                                   std::size_t sizeOfVarStruct,
                                   OWLVarDecl* vars,
                                   int numVars)
    {
        FAKE_LOG_DBG << "Creating program (type " << pt << "): " << name;

        Program* program = nullptr;

        if (pt == Program::PtBounds)
            program = new Bounds;
        if (pt == Program::PtClosestHit)
        {
            program = new ClosestHit;
            ClosestHit* ch = (ClosestHit*)program;
            ch->fakePrepareClosestHitSym = fakePrepareClosestHitSym;
        }
        else if (pt == Program::PtMiss)
        {
            program = new Miss(sizeOfVarStruct, vars, numVars);
            Miss* missProg = (Miss*)program;
            missProg->fakePrepareMissSym = fakePrepareMissSym;

            // This is rayType order
            miss.push_back(missProg);
        }
        else if (pt == Program::PtIntersect)
        {
            program = new Intersect;
            Intersect* isect = (Intersect*)program;
            isect->fakePrepareIntersectionSym = fakePrepareIntersectionSym;
            isect->fakeGetIntersectionResultSym = fakeGetIntersectionResultSym;
        }
        else if (pt == Program::PtRayGen)
        {
            program = new RayGen(sizeOfVarStruct, vars, numVars);
            RayGen* rayGen = (RayGen*)program;
            rayGen->fakePrepareRayGenSym = fakePrepareRayGenSym;
        }

        if (program != nullptr)
        {
            program->resourceHandle = RegisterManagedResource((ManagedResource)program);
            program->setContext(context);
            program->module = this;
            program->type = pt;
            program->name = name;
            program->fakeResetPreviousProgramStateSym = fakeResetPreviousProgramStateSym;
            programEntries.push_back({pt, program->resourceHandle});

            FAKE_LOG_DBG << "Success!";
        }
        else
            FAKE_LOG(fake::logging::Level::Error) << "Unsuccessful!";

        return program;
    }

    void Module::releaseProgram(Program* program)
    {
        FAKE_LOG_DBG << "Releasing program (type " << program->type << "): " << program->name;

        programEntries.erase(std::remove_if(programEntries.begin(),
                                            programEntries.end(),
                                            [program](ProgramEntry ent)
                                            { return ent.type == program->type
                                                  && ent.handle == program->resourceHandle; }),
                             programEntries.end());
        UnregisterManagedResource(program->resourceHandle);
        delete program;
    }

    void Module::setMissProg(int rayType, Miss* missProg)
    {
        if (rayType >= miss.size())
        {
            // As miss progs are set in some of the original examples
            // before the ray type count was set (if ever), be
            // robust towards that (but issue a warning)
            FAKE_LOG(fake::logging::Level::Warning) << "rayType " << rayType
                                                    << " exceeds rayType count:"
                                                    << miss.size()
                                                    << " -- setting rayType count to "
                                                    << rayType+1;
            miss.resize(rayType+1);
        }

        miss[rayType] = missProg;
    }

    bool Module::haveMissProg(int rayType)
    {
        return rayType >= 0 && rayType < miss.size();
    }

    Miss* Module::getMissProg(int rayType)
    {
        if (rayType >= miss.size())
        {
            FAKE_LOG(fake::logging::Level::Error) << "rayType " << rayType
                                                  << " exceeds rayType count:"
                                                  << miss.size();
            return nullptr;
        }

        return miss[rayType];
    }
} // fake
