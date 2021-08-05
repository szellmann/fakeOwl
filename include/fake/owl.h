
#pragma once

#include <cstdint>

namespace fake
{
    typedef enum
    {
        PtBounds,
        PtAnyHit,
        PtClosestHit,
        PtMiss,
        PtIntersect,
        PtRayGen,
        PtCount = 6, // Keep last!
    } ProgramType;

    /* state variables for intersection programs */
    typedef struct
    {
        unsigned primID;
        unsigned instID;
        float triangleBarycentricU;
        float triangleBarycentricV;
        unsigned hitKind;
        float objectToWorldTransform[12];
        float worldToObjectTransform[12];
        float worldRayOrigin[3];
        float worldRayDirection[3];
        float objectRayOrigin[3];
        float objectRayDirection[3];
        float rayTmin;
        float rayTmax;
        unsigned p0;
        unsigned p1;
        void* sbtPointer;
    } ProgramState;

    typedef struct
    {
        unsigned wasHit;
        float hitT;
        unsigned hitKind;
        unsigned a0;
        unsigned a1;
        unsigned a2;
        unsigned a3;
        unsigned a4;
        unsigned a5;
        unsigned a6;
        unsigned a7;
    } IntersectionResult;

    typedef struct
    {
        void* currentModule;
        void* traversables;
        uint64_t index;
    } TraversableHandle;
} // fake

#define OPTIX_BOUNDS_PROGRAM(programName) \
  extern "C" __global__ \
  void __boundsFunc__##programName

