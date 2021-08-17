#pragma once

#include <stdexcept>

#include <owl/common/math/LinearSpace.h>
#include <owl/common/math/vec.h>

#include "optix.h"
#include "owl.h"

namespace fake
{
    static thread_local int launchIndex[3];
    static thread_local int launchDims[3];

    static thread_local ProgramState programStates[128];
    static thread_local unsigned stackPtr = 0;
    static thread_local IntersectionResult intersectionResult = {};
}

#ifdef FAKE_OWL_DEFINE_ENTRY_POINTS
extern "C" void fakeSetLaunchIndex(int x, int y, int z)
{
    fake::launchIndex[0] = x;
    fake::launchIndex[1] = y;
    fake::launchIndex[2] = z;
}

extern "C" void fakeSetLaunchDims(int x, int y, int z)
{
    fake::launchDims[0] = x;
    fake::launchDims[1] = y;
    fake::launchDims[2] = z;
}

extern "C" fake::ProgramState* fakePrepareRayGen()
{
    fake::stackPtr++;
    fake::programStates[fake::stackPtr].type = fake::PtRayGen;
    return &fake::programStates[fake::stackPtr];
}

extern "C" fake::ProgramState* fakePrepareIntersection()
{
    /* reset intersection */
    fake::intersectionResult = {};

    fake::stackPtr++;
    fake::programStates[fake::stackPtr].type = fake::PtIntersect;
    return &fake::programStates[fake::stackPtr];
}

extern "C" fake::ProgramState* fakePrepareClosestHit()
{
    fake::stackPtr++;
    fake::programStates[fake::stackPtr].type = fake::PtClosestHit;
    return &fake::programStates[fake::stackPtr];
}

extern "C" fake::ProgramState* fakePrepareMiss()
{
    fake::stackPtr++;
    fake::programStates[fake::stackPtr].type = fake::PtMiss;
    return &fake::programStates[fake::stackPtr];
}

extern "C" fake::IntersectionResult* fakeGetIntersectionResult()
{
    /* side effect: pop program stack */
    fake::stackPtr--;

    return &fake::intersectionResult;
}

extern "C" void fakeResetPreviousProgramState()
{
    fake::stackPtr--;
}
#endif


inline owl::vec2i optixGetLaunchIndex()
{
    return { fake::launchIndex[0], fake::launchIndex[1] };
}

inline owl::vec2i optixGetLaunchDimensions()
{
    return { fake::launchDims[0], fake::launchDims[1] };
}

inline unsigned optixGetPrimitiveIndex()
{
    return fake::programStates[fake::stackPtr].primID;
}

/* Returns the user-supplied instID */
inline unsigned optixGetInstanceId()
{
    return fake::programStates[fake::stackPtr].instID;
}

inline unsigned optixGetHitKind()
{
    return fake::programStates[fake::stackPtr].hitKind;
}

inline OptixPrimitiveType optixGetPrimitiveType()
{
    throw std::runtime_error("Not implemented yet");
}

inline bool optixIsFrontFaceHit(unsigned hitKind)
{
    return hitKind == 0xFE;
}

inline bool optixIsBackFaceHit(unsigned hitKind)
{
    return hitKind == 0xFF;
}

inline owl::vec2f optixGetTriangleBarycentrics()
{
    return {
        fake::programStates[fake::stackPtr].triangleBarycentricU,
        fake::programStates[fake::stackPtr].triangleBarycentricV
    };
}

inline void optixGetWorldToObjectTransformMatrix(float m[12])
{
    memcpy(m, fake::programStates[fake::stackPtr].worldToObjectTransform,
           sizeof(float) * 12);
}

inline void optixGetObjectToWorldTransformMatrix(float m[12])
{
    memcpy(m, fake::programStates[fake::stackPtr].objectToWorldTransform,
           sizeof(float) * 12);
}

inline owl::vec3f optixTransformNormalFromObjectToWorldSpace(owl::vec3f normal)
{
    float* o2w
      = fake::programStates[fake::stackPtr].objectToWorldTransform;
        owl::vec3f col1(o2w[0], o2w[1], o2w[2]);
        owl::vec3f col2(o2w[3], o2w[4], o2w[5]);
        owl::vec3f col3(o2w[6], o2w[7], o2w[8]);
        owl::linear3f l(col1, col2, col3);
    return owl::xfmNormal(l, normal);
}

inline owl::vec3f optixGetWorldRayOrigin()
{
    return {
        fake::programStates[fake::stackPtr].worldRayOrigin[0],
        fake::programStates[fake::stackPtr].worldRayOrigin[1],
        fake::programStates[fake::stackPtr].worldRayOrigin[2]
    };
}

inline owl::vec3f optixGetWorldRayDirection()
{
    return {
        fake::programStates[fake::stackPtr].worldRayDirection[0],
        fake::programStates[fake::stackPtr].worldRayDirection[1],
        fake::programStates[fake::stackPtr].worldRayDirection[2]
    };
}

inline owl::vec3f optixGetObjectRayOrigin()
{
    return {
        fake::programStates[fake::stackPtr].objectRayOrigin[0],
        fake::programStates[fake::stackPtr].objectRayOrigin[1],
        fake::programStates[fake::stackPtr].objectRayOrigin[2]
    };
}

inline owl::vec3f optixGetObjectRayDirection()
{
    return {
        fake::programStates[fake::stackPtr].objectRayDirection[0],
        fake::programStates[fake::stackPtr].objectRayDirection[1],
        fake::programStates[fake::stackPtr].objectRayDirection[2]
    };
}

inline float optixGetRayTmin()
{
    return fake::programStates[fake::stackPtr].rayTmin;
}

inline float optixGetRayTmax()
{
    return fake::programStates[fake::stackPtr].rayTmax;
}

inline unsigned optixGetPayload_0()
{
    return fake::programStates[fake::stackPtr].p0;
}

inline unsigned optixGetPayload_1()
{
    return fake::programStates[fake::stackPtr].p1;
}

inline const void* optixGetSbtDataPointer()
{
    return fake::programStates[fake::stackPtr].sbtPointer;
}

inline const bool optixReportIntersection(float    hitT,
                                          unsigned hitKind)
{
    bool wasIntersection = true; /* TODO */
    fake::intersectionResult.wasHit = 1;
    fake::intersectionResult.hitT = hitT;
    fake::intersectionResult.hitKind = hitKind;
    return wasIntersection;
}

inline const bool optixReportIntersection(float    hitT,
                                          unsigned hitKind,
                                          unsigned a0)
{
    bool wasIntersection = true; /* TODO */
    fake::intersectionResult.wasHit = 1;
    fake::intersectionResult.hitT = hitT;
    fake::intersectionResult.hitKind = hitKind;
    fake::intersectionResult.a0 = a0;
    return wasIntersection;
}

inline const bool optixReportIntersection(float    hitT,
                                          unsigned hitKind,
                                          unsigned a0,
                                          unsigned a1)
{
    bool wasIntersection = true; /* TODO */
    fake::intersectionResult.wasHit = 1;
    fake::intersectionResult.hitT = hitT;
    fake::intersectionResult.hitKind = hitKind;
    fake::intersectionResult.a0 = a0;
    fake::intersectionResult.a1 = a1;
    return wasIntersection;
}

inline const bool optixReportIntersection(float    hitT,
                                          unsigned hitKind,
                                          unsigned a0,
                                          unsigned a1,
                                          unsigned a2)
{
    bool wasIntersection = true; /* TODO */
    fake::intersectionResult.wasHit = 1;
    fake::intersectionResult.hitT = hitT;
    fake::intersectionResult.hitKind = hitKind;
    fake::intersectionResult.a0 = a0;
    fake::intersectionResult.a1 = a1;
    fake::intersectionResult.a2 = a2;
    return wasIntersection;
}

inline const bool optixReportIntersection(float    hitT,
                                          unsigned hitKind,
                                          unsigned a0,
                                          unsigned a1,
                                          unsigned a2,
                                          unsigned a3)
{
    bool wasIntersection = true; /* TODO */
    fake::intersectionResult.wasHit = 1;
    fake::intersectionResult.hitT = hitT;
    fake::intersectionResult.hitKind = hitKind;
    fake::intersectionResult.a0 = a0;
    fake::intersectionResult.a1 = a1;
    fake::intersectionResult.a2 = a2;
    fake::intersectionResult.a3 = a3;
    return wasIntersection;
}

inline const bool optixReportIntersection(float    hitT,
                                          unsigned hitKind,
                                          unsigned a0,
                                          unsigned a1,
                                          unsigned a2,
                                          unsigned a3,
                                          unsigned a4)
{
    bool wasIntersection = true; /* TODO */
    fake::intersectionResult.wasHit = 1;
    fake::intersectionResult.hitT = hitT;
    fake::intersectionResult.hitKind = hitKind;
    fake::intersectionResult.a0 = a0;
    fake::intersectionResult.a1 = a1;
    fake::intersectionResult.a2 = a2;
    fake::intersectionResult.a3 = a3;
    fake::intersectionResult.a4 = a4;
    return wasIntersection;
}

inline const bool optixReportIntersection(float    hitT,
                                          unsigned hitKind,
                                          unsigned a0,
                                          unsigned a1,
                                          unsigned a2,
                                          unsigned a3,
                                          unsigned a4,
                                          unsigned a5)
{
    bool wasIntersection = true; /* TODO */
    fake::intersectionResult.wasHit = 1;
    fake::intersectionResult.hitT = hitT;
    fake::intersectionResult.hitKind = hitKind;
    fake::intersectionResult.a0 = a0;
    fake::intersectionResult.a1 = a1;
    fake::intersectionResult.a2 = a2;
    fake::intersectionResult.a3 = a3;
    fake::intersectionResult.a4 = a4;
    fake::intersectionResult.a5 = a5;
    return wasIntersection;
}

inline const bool optixReportIntersection(float    hitT,
                                          unsigned hitKind,
                                          unsigned a0,
                                          unsigned a1,
                                          unsigned a2,
                                          unsigned a3,
                                          unsigned a4,
                                          unsigned a5,
                                          unsigned a6)
{
    bool wasIntersection = true; /* TODO */
    fake::intersectionResult.wasHit = 1;
    fake::intersectionResult.hitT = hitT;
    fake::intersectionResult.hitKind = hitKind;
    fake::intersectionResult.a0 = a0;
    fake::intersectionResult.a1 = a1;
    fake::intersectionResult.a2 = a2;
    fake::intersectionResult.a3 = a3;
    fake::intersectionResult.a4 = a4;
    fake::intersectionResult.a6 = a6;
    return wasIntersection;
}

inline const bool optixReportIntersection(float    hitT,
                                          unsigned hitKind,
                                          unsigned a0,
                                          unsigned a1,
                                          unsigned a2,
                                          unsigned a3,
                                          unsigned a4,
                                          unsigned a5,
                                          unsigned a6,
                                          unsigned a7)
{
    bool wasIntersection = true; /* TODO */
    fake::intersectionResult.wasHit = 1;
    fake::intersectionResult.hitT = hitT;
    fake::intersectionResult.hitKind = hitKind;
    fake::intersectionResult.a0 = a0;
    fake::intersectionResult.a1 = a1;
    fake::intersectionResult.a2 = a2;
    fake::intersectionResult.a3 = a3;
    fake::intersectionResult.a4 = a4;
    fake::intersectionResult.a6 = a6;
    fake::intersectionResult.a7 = a7;
    return wasIntersection;
}

inline unsigned optixGetAttribute_0()
{
    return fake::intersectionResult.a0;
}

inline unsigned optixGetAttribute_1()
{
    return fake::intersectionResult.a1;
}

inline unsigned optixGetAttribute_2()
{
    return fake::intersectionResult.a2;
}

inline unsigned optixGetAttribute_3()
{
    return fake::intersectionResult.a3;
}

inline unsigned optixGetAttribute_4()
{
    return fake::intersectionResult.a4;
}

inline unsigned optixGetAttribute_5()
{
    return fake::intersectionResult.a5;
}

inline unsigned optixGetAttribute_6()
{
    return fake::intersectionResult.a6;
}

inline unsigned optixGetAttribute_7()
{
    return fake::intersectionResult.a7;
}

inline void optixTerminateRay()
{
    /* TODO */
}
