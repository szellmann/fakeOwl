#include <cassert>
#include <cstddef>
#include <cstdint>

#include <fake/optix.h>
#include <fake/owl.h>

#include "ClosestHit.h"
#include "Miss.h"
#include "Module.h"
#include "Trace.h"
#include "Visionaray.h"

namespace fake
{
    static Traversable traversables[1<<24];
    static std::size_t numTraversables = 0;

    TraversableHandle registerTraversable(Traversable traversable)
    {
        traversable.handle = { nullptr, &traversables[0], (uint64_t)numTraversables };
        traversables[numTraversables++] = traversable;
        return traversable.handle;
    }

    Traversable& getTraversable(TraversableHandle handle)
    {
        return traversables[handle.index];
    }

    void traversablesSetCurrentModule(Module* module)
    {
        for (std::size_t i = 0; i < numTraversables; ++i)
        {
            traversables[i].handle.currentModule = (void*)module;
        }
    }

    Traversable* getTraversableList()
    {
        return &traversables[0];
    }

    std::size_t getNumTraversables()
    {
        return numTraversables;
    }
}

// TODO: at least on macOS, looking up the address of this function
// from the shared lib is extremely slow (tlv_get_addr(), tl=thread local).
// Maybe inline this, anyway?
void optixTrace(OptixTraversableHandle handle,
                float3 rayOrigin,
                float3 rayDirection,
                float tmin,
                float tmax,
                float rayTime,
                OptixVisibilityMask visibilityMask,
                unsigned int rayFlags,
                unsigned int SBToffset,
                unsigned int SBTstride,
                unsigned int missSBTIndex,
                unsigned int & p0,
                unsigned int & p1)
{
    using namespace visionaray;

    fake::Ray r;
    r.ori = vec3(rayOrigin.x, rayOrigin.y, rayOrigin.z);
    r.dir = vec3(rayDirection.x, rayDirection.y, rayDirection.z);
    r.tmin = tmin;
    r.tmax = tmax;

    fake::Payload pl;
    pl.p0 = &p0;
    pl.p1 = &p1;
    pl.rayType = SBToffset;
    pl.visibilityMask = visibilityMask;
    pl.rayFlags = rayFlags;
    pl.worldToObjectTransform = { mat3::identity(), vec3(0.f) };
    pl.objectToWorldTransform = { mat3::identity(), vec3(0.f) };
    pl.worldRayOrigin = vec3(rayOrigin.x, rayOrigin.y, rayOrigin.z);
    pl.worldRayDirection = vec3(rayDirection.x, rayDirection.y, rayDirection.z);

    r.payload = &pl;

    fake::Traversable* traversables = (fake::Traversable*)handle.traversables;
    fake::Traversable traversable = traversables[handle.index];

    fake::HitRecord hr;

    GroupBVH::Reference bvh = *(GroupBVH::Reference*)traversable.accessor;
    hr = intersect(r, bvh);

    if (hr.hit)
    {
        fake::Geom* geom = (fake::Geom*)hr.geom;

        fake::ClosestHit* chProg = geom->getClosestHitProg(r.payload->rayType);

        if (chProg != nullptr)
        {
            void (*entryPoint)();
            entryPoint = (void (*)())chProg->entryPointSym;

            fake::ProgramState* (*fakePrepareClosestHit)();
            fakePrepareClosestHit
                    = (fake::ProgramState* (*)())chProg->fakePrepareClosestHitSym;

            void (*fakeResetPreviousProgramState)();
            fakeResetPreviousProgramState
                    = (void (*)())chProg->fakeResetPreviousProgramStateSym;

            fake::ProgramState* state = fakePrepareClosestHit();
            state->primID = hr.prim_id;
            state->instID = hr.instID;
            state->triangleBarycentricU = hr.u;
            state->triangleBarycentricV = hr.v;
            state->hitKind = hr.hitKind;
            for (int i = 0; i < 12; ++i)
            {
                state->objectToWorldTransform[i] = r.payload->objectToWorldTransform.data()[i];
                state->worldToObjectTransform[i] = r.payload->worldToObjectTransform.data()[i];
            }
            state->worldRayOrigin[0] = rayOrigin.x;
            state->worldRayOrigin[1] = rayOrigin.y;
            state->worldRayOrigin[2] = rayOrigin.z;
            state->worldRayDirection[0] = rayDirection.x;
            state->worldRayDirection[1] = rayDirection.y;
            state->worldRayDirection[2] = rayDirection.z;
            state->objectRayOrigin[0] = hr.objectRayOrigin.x;
            state->objectRayOrigin[1] = hr.objectRayOrigin.y;
            state->objectRayOrigin[2] = hr.objectRayOrigin.z;
            state->objectRayDirection[0] = hr.objectRayDirection.x;
            state->objectRayDirection[1] = hr.objectRayDirection.y;
            state->objectRayDirection[2] = hr.objectRayDirection.z;
            state->rayTmin = tmin;
            state->rayTmax = hr.t; // returns hitT in rayTmax
            state->p0 = *r.payload->p0;
            state->p1 = *r.payload->p1;
            state->sbtPointer = geom->dataPtr;

            entryPoint();

            fakeResetPreviousProgramState();
        }
    }
    else
    {
        if (traversable.handle.currentModule != nullptr)
        {
            fake::Module* module = (fake::Module*)traversable.handle.currentModule;

            if (module->haveMissProg(0/*TODO: rayType*/))
            {
                fake::Miss* missProg = module->getMissProg(0/*TODO*/);

                void (*entryPoint)();
                entryPoint = (void (*)())missProg->entryPointSym;

                fake::ProgramState* (*fakePrepareMiss)();
                fakePrepareMiss
                        = (fake::ProgramState* (*)())missProg->fakePrepareMissSym;

                void (*fakeResetPreviousProgramState)();
                fakeResetPreviousProgramState
                    = (void (*)())missProg->fakeResetPreviousProgramStateSym;

                fake::ProgramState* state = fakePrepareMiss();
                state->worldRayOrigin[0] = rayOrigin.x;
                state->worldRayOrigin[1] = rayOrigin.y;
                state->worldRayOrigin[2] = rayOrigin.z;
                state->worldRayDirection[0] = rayDirection.x;
                state->worldRayDirection[1] = rayDirection.y;
                state->worldRayDirection[2] = rayDirection.z;
                state->objectRayOrigin[0] = hr.objectRayOrigin.x;
                state->objectRayOrigin[1] = hr.objectRayOrigin.y;
                state->objectRayOrigin[2] = hr.objectRayOrigin.z;
                state->objectRayDirection[0] = hr.objectRayDirection.x;
                state->objectRayDirection[1] = hr.objectRayDirection.y;
                state->objectRayDirection[2] = hr.objectRayDirection.z;
                state->rayTmin = tmin;
                state->rayTmax = tmax;
                state->p0 = *r.payload->p0;
                state->p1 = *r.payload->p1;
                state->sbtPointer = missProg->dataPtr;

                entryPoint();

                fakeResetPreviousProgramState();
            }
        }
    }
}


