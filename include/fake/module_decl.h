#pragma once

#include <stdexcept>

#include <owl/common/math/LinearSpace.h>
#include <owl/common/math/vec.h>

#include "optix.h"
#include "owl.h"

#define FAKE_OWL_MODULE_DECL                                                    \
namespace fake                                                                  \
{                                                                               \
    static thread_local int launchIndex[3];                                     \
    static thread_local int launchDims[3];                                      \
                                                                                \
    static thread_local ProgramType previousProgram = PtRayGen;                 \
    static thread_local ProgramType currentProgram = PtRayGen;                  \
                                                                                \
    static thread_local ProgramState programStates[PtCount];                    \
    static thread_local IntersectionResult intersectionResult = {};             \
}                                                                               \
                                                                                \
extern "C" void fakeSetLaunchIndex(int x, int y, int z)                         \
{                                                                               \
    fake::launchIndex[0] = x;                                                   \
    fake::launchIndex[1] = y;                                                   \
    fake::launchIndex[2] = z;                                                   \
}                                                                               \
                                                                                \
extern "C" void fakeSetLaunchDims(int x, int y, int z)                          \
{                                                                               \
    fake::launchDims[0] = x;                                                    \
    fake::launchDims[1] = y;                                                    \
    fake::launchDims[2] = z;                                                    \
}                                                                               \
                                                                                \
extern "C" fake::ProgramState* fakePrepareRayGen()                              \
{                                                                               \
    fake::previousProgram = fake::currentProgram;                               \
    fake::currentProgram = fake::PtRayGen;                                      \
                                                                                \
    return &fake::programStates[fake::PtRayGen];                                \
}                                                                               \
                                                                                \
extern "C" fake::ProgramState* fakePrepareIntersection()                        \
{                                                                               \
    fake::previousProgram = fake::currentProgram;                               \
    fake::currentProgram = fake::PtIntersect;                                   \
                                                                                \
    /* reset intersection */                                                    \
    fake::intersectionResult = {};                                              \
                                                                                \
    return &fake::programStates[fake::PtIntersect];                             \
}                                                                               \
                                                                                \
extern "C" fake::ProgramState* fakePrepareClosestHit()                          \
{                                                                               \
    fake::previousProgram = fake::currentProgram;                               \
    fake::currentProgram = fake::PtClosestHit;                                  \
                                                                                \
    return &fake::programStates[fake::PtClosestHit];                            \
}                                                                               \
                                                                                \
extern "C" fake::ProgramState* fakePrepareMiss()                                \
{                                                                               \
    fake::previousProgram = fake::currentProgram;                               \
    fake::currentProgram = fake::PtMiss;                                        \
                                                                                \
    return &fake::programStates[fake::PtMiss];                                  \
}                                                                               \
                                                                                \
extern "C" fake::IntersectionResult* fakeGetIntersectionResult()                \
{                                                                               \
    fake::currentProgram = fake::previousProgram;                               \
                                                                                \
    return &fake::intersectionResult;                                           \
}                                                                               \
                                                                                \
extern "C" void fakeResetPreviousProgramState()                                 \
{                                                                               \
    fake::currentProgram = fake::previousProgram;                               \
}                                                                               \
                                                                                \
owl::vec2i optixGetLaunchIndex()                                                \
{                                                                               \
    return { fake::launchIndex[0], fake::launchIndex[1] };                      \
}                                                                               \
                                                                                \
owl::vec2i optixGetLaunchDimensions()                                           \
{                                                                               \
    return { fake::launchDims[0], fake::launchDims[1] };                        \
}                                                                               \
                                                                                \
unsigned optixGetPrimitiveIndex()                                               \
{                                                                               \
    return fake::programStates[fake::currentProgram].primID;                    \
}                                                                               \
                                                                                \
/* Returns the user-supplied instID */                                          \
unsigned optixGetInstanceId()                                                   \
{                                                                               \
    return fake::programStates[fake::currentProgram].instID;                    \
}                                                                               \
                                                                                \
unsigned optixGetHitKind()                                                      \
{                                                                               \
    return fake::programStates[fake::currentProgram].hitKind;                   \
}                                                                               \
                                                                                \
OptixPrimitiveType optixGetPrimitiveType()                                      \
{                                                                               \
    throw std::runtime_error("Not implemented yet");                            \
}                                                                               \
                                                                                \
bool optixIsFrontFaceHit(unsigned hitKind)                                      \
{                                                                               \
    return hitKind == 0xFE;                                                     \
}                                                                               \
                                                                                \
bool optixIsBackFaceHit(unsigned hitKind)                                       \
{                                                                               \
    return hitKind == 0xFF;                                                     \
}                                                                               \
                                                                                \
owl::vec2f optixGetTriangleBarycentrics()                                       \
{                                                                               \
    return {                                                                    \
        fake::programStates[fake::currentProgram].triangleBarycentricU,         \
        fake::programStates[fake::currentProgram].triangleBarycentricV          \
    };                                                                          \
}                                                                               \
                                                                                \
void optixGetWorldToObjectTransformMatrix(float m[12])                          \
{                                                                               \
    memcpy(m, fake::programStates[fake::currentProgram].worldToObjectTransform, \
           sizeof(float) * 12);                                                 \
}                                                                               \
                                                                                \
void optixGetObjectToWorldTransformMatrix(float m[12])                          \
{                                                                               \
    memcpy(m, fake::programStates[fake::currentProgram].objectToWorldTransform, \
           sizeof(float) * 12);                                                 \
}                                                                               \
                                                                                \
owl::vec3f optixTransformNormalFromObjectToWorldSpace(owl::vec3f normal)        \
{                                                                               \
    float* o2w                                                                  \
      = fake::programStates[fake::currentProgram].objectToWorldTransform;       \
        owl::vec3f col1(o2w[0], o2w[1], o2w[2]);                                \
        owl::vec3f col2(o2w[3], o2w[4], o2w[5]);                                \
        owl::vec3f col3(o2w[6], o2w[7], o2w[8]);                                \
        owl::linear3f l(col1, col2, col3);                                      \
    return owl::xfmNormal(l, normal);                                           \
}                                                                               \
                                                                                \
owl::vec3f optixGetWorldRayOrigin()                                             \
{                                                                               \
    return {                                                                    \
        fake::programStates[fake::currentProgram].worldRayOrigin[0],            \
        fake::programStates[fake::currentProgram].worldRayOrigin[1],            \
        fake::programStates[fake::currentProgram].worldRayOrigin[2]             \
    };                                                                          \
}                                                                               \
                                                                                \
owl::vec3f optixGetWorldRayDirection()                                          \
{                                                                               \
    return {                                                                    \
        fake::programStates[fake::currentProgram].worldRayDirection[0],         \
        fake::programStates[fake::currentProgram].worldRayDirection[1],         \
        fake::programStates[fake::currentProgram].worldRayDirection[2]          \
    };                                                                          \
}                                                                               \
                                                                                \
owl::vec3f optixGetObjectRayOrigin()                                            \
{                                                                               \
    return {                                                                    \
        fake::programStates[fake::currentProgram].objectRayOrigin[0],           \
        fake::programStates[fake::currentProgram].objectRayOrigin[1],           \
        fake::programStates[fake::currentProgram].objectRayOrigin[2]            \
    };                                                                          \
}                                                                               \
                                                                                \
owl::vec3f optixGetObjectRayDirection()                                         \
{                                                                               \
    return {                                                                    \
        fake::programStates[fake::currentProgram].objectRayDirection[0],        \
        fake::programStates[fake::currentProgram].objectRayDirection[1],        \
        fake::programStates[fake::currentProgram].objectRayDirection[2]         \
    };                                                                          \
}                                                                               \
                                                                                \
float optixGetRayTmin()                                                         \
{                                                                               \
    return fake::programStates[fake::currentProgram].rayTmin;                   \
}                                                                               \
                                                                                \
float optixGetRayTmax()                                                         \
{                                                                               \
    return fake::programStates[fake::currentProgram].rayTmax;                   \
}                                                                               \
                                                                                \
unsigned optixGetPayload_0()                                                    \
{                                                                               \
    return fake::programStates[fake::currentProgram].p0;                        \
}                                                                               \
                                                                                \
unsigned optixGetPayload_1()                                                    \
{                                                                               \
    return fake::programStates[fake::currentProgram].p1;                        \
}                                                                               \
                                                                                \
const void* optixGetSbtDataPointer()                                            \
{                                                                               \
    return fake::programStates[fake::currentProgram].sbtPointer;                \
}                                                                               \
                                                                                \
const bool optixReportIntersection(float    hitT,                               \
                                   unsigned hitKind)                            \
{                                                                               \
    bool wasIntersection = true; /* TODO */                                     \
    fake::intersectionResult.wasHit = 1;                                        \
    fake::intersectionResult.hitT = hitT;                                       \
    fake::intersectionResult.hitKind = hitKind;                                 \
    return wasIntersection;                                                     \
}                                                                               \
                                                                                \
const bool optixReportIntersection(float    hitT,                               \
                                   unsigned hitKind,                            \
                                   unsigned a0)                                 \
{                                                                               \
    bool wasIntersection = true; /* TODO */                                     \
    fake::intersectionResult.wasHit = 1;                                        \
    fake::intersectionResult.hitT = hitT;                                       \
    fake::intersectionResult.hitKind = hitKind;                                 \
    fake::intersectionResult.a0 = a0;                                           \
    return wasIntersection;                                                     \
}                                                                               \
