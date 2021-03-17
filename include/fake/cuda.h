
#pragma once

#include <x86intrin.h>

#if !defined(__CUDACC__) && !defined(CUDARTAPI)

#define VOWL_CUDA_TYPES 1

#define __global__
#define __constant__
#define __host__
#define __device__

#define __forceinline__ __attribute__((always_inline))

inline uint64_t clock64()
{
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

typedef struct {
    int x, y;
} int2;

typedef struct {
    int x, y, z;
} int3;

typedef struct {
    int x, y, z, w;
} int4;

typedef struct {
    unsigned x, y;
} uint2;

typedef struct {
    unsigned x, y, z;
} uint3;

typedef struct {
    unsigned x, y, z, w;
} uint4;

typedef struct {
    float x, y;
} float2;

typedef struct {
    float x, y, z;
} float3;

typedef struct {
    float x, y, z, w;
} float4;

struct dim3 {
    dim3() = default;
    dim3(unsigned x) : x(x), y(1), z(1) {}
    dim3(unsigned x, unsigned y) : x(x), y(y), z(1) {}
    dim3(unsigned x, unsigned y, unsigned z) : x(x), y(y), z(z) {}
    operator uint3() const { return { x, y, z }; }
    unsigned x, y, z;
};

typedef struct {
} CUstream;

struct cudaGraphicsResource;
typedef cudaGraphicsResource* cudaGraphicsResource_t;

namespace fake
{
    typedef struct {
        void* textures;
        uint64_t index;
        uint32_t texelFormat;
    } TextureHandle;
}

typedef fake::TextureHandle cudaTextureObject_t;
typedef fake::TextureHandle CUtexObject;

inline float3 make_float3(float x, float y, float z)
{
    float3 f3;
    f3.x = x;
    f3.y = y;
    f3.z = z;
    return f3;
}

inline float4 make_float4(float x, float y, float z, float w)
{
    float4 f4;
    f4.x = x;
    f4.y = y;
    f4.z = z;
    f4.w = w;
    return f4;
}

inline int atomicAdd(int* addr, int val)
{
    return __atomic_fetch_add(addr, val, __ATOMIC_RELAXED);
}

namespace fake
{
    void sampleTexture2D(float4& result, TextureHandle obj, float tcx, float tcy);

    void sampleTexture3D(float& result, TextureHandle obj, float tcx, float tcy, float tcz);
    void sampleTexture3D(float4& result, TextureHandle obj, float tcx, float tcy, float tcz);
} // fake

template <typename T>
inline T tex2D(cudaTextureObject_t obj, float tcx, float tcy)
{
    T res;
    fake::sampleTexture2D(res, obj, tcx, tcy);
    return res;
}

template <typename T>
inline T tex3D(cudaTextureObject_t obj, float tcx, float tcy, float tcz)
{
    T res;
    fake::sampleTexture3D(res, obj, tcx, tcy, tcz);
    return res;
}

#else
#include <cuda.h>
#include <cuda_runtime.h>
#include <vector_types.h>
#endif


