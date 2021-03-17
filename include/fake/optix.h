
#pragma once

#include <stdint.h>

#include <owl/common/math/vec.h>

#include "cuda.h"
#include "owl.h"

typedef fake::TraversableHandle OptixTraversableHandle;
typedef uint32_t OptixVisibilityMask;

struct OptixDeviceContext_t;
typedef struct OptixDeviceContext_t* OptixDeviceContext;

owl::vec2i optixGetLaunchIndex();
owl::vec2i optixGetLaunchDimensions();
unsigned optixGetPrimitiveIndex();
owl::vec2f optixGetTriangleBarycentrics();
owl::vec3f optixTransformNormalFromObjectToWorldSpace(owl::vec3f normal);
owl::vec3f optixGetWorldRayOrigin();
owl::vec3f optixGetWorldRayDirection();
owl::vec3f optixGetObjectRayOrigin();
owl::vec3f optixGetObjectRayDirection();
float optixGetRayTmin();
float optixGetRayTmax();
const void* optixGetSbtDataPointer();
unsigned optixGetPayload_0();
unsigned optixGetPayload_1();

typedef enum {
    OPTIX_PRIMITIVE_TYPE_CUSTOM,
    OPTIX_PRIMITIVE_TYPE_ROUND_QUADRATIC_BSPLINE,
    OPTIX_PRIMITIVE_TYPE_ROUND_CUBIC_BSPLINE,
    OPTIX_PRIMITIVE_TYPE_ROUND_LINEAR,
    OPTIX_PRIMITIVE_TYPE_TRIANGLE,
} OptixPrimitiveType;

typedef enum {
    OPTIX_RAY_FLAG_NONE = 0u,
    OPTIX_RAY_FLAG_DISABLE_ANYHIT = 1u << 0,
    OPTIX_RAY_FLAG_ENFORCE_ANYHIT = 1u << 1,
    OPTIX_RAY_FLAG_TERMINATE_ON_FIRST_HIT = 1u << 2,
    OPTIX_RAY_FLAG_DISABLE_CLOSESTHIT = 1u << 3,
    OPTIX_RAY_FLAG_CULL_BACK_FACING_TRIANGLES = 1u << 4,
    OPTIX_RAY_FLAG_CULL_FRONT_FACING_TRIANGLES = 1u << 5,
    OPTIX_RAY_FLAG_CULL_DISABLED_ANYHIT = 1u << 6,
    OPTIX_RAY_FLAG_CULL_ENFORCED_ANYHIT = 1u << 7,
} OptixRayFlags;

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
                unsigned int & p1);
