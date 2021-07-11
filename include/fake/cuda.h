
#pragma once

#include <x86intrin.h>

#if !defined(__CUDACC__) && !defined(CUDARTAPI)

#define FAKE_OWL_CUDA_TYPES 1

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
    char x;
} char1;

typedef struct {
    char x, y;
} char2;

typedef struct {
    char x, y, z;
} char3;

typedef struct {
    char x, y, z, w;
} char4;

typedef struct {
    short x;
} short1;

typedef struct {
    short x, y;
} short2;

typedef struct {
    short x, y, z;
} short3;

typedef struct {
    short x, y, z, w;
} short4;

typedef struct {
    int x;
} int1;

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
    long x;
} long1;

typedef struct {
    long x, y;
} long2;

typedef struct {
    long x, y, z;
} long3;

typedef struct {
    long x, y, z, w;
} long4;

typedef struct {
    long long x;
} longlong1;

typedef struct {
    long long x, y;
} longlong2;

typedef struct {
    long long x, y, z;
} longlong3;

typedef struct {
    long long x, y, z, w;
} longlong4;

typedef struct {
    unsigned char x;
} uchar1;

typedef struct {
    unsigned char x, y;
} uchar2;

typedef struct {
    unsigned char x, y, z;
} uchar3;

typedef struct {
    unsigned char x, y, z, w;
} uchar4;

typedef struct {
    unsigned short x;
} ushort1;

typedef struct {
    unsigned short x, y;
} ushort2;

typedef struct {
    unsigned short x, y, z;
} ushort3;

typedef struct {
    unsigned short x, y, z, w;
} ushort4;

typedef struct {
    unsigned x;
} uint1;

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
    unsigned long x;
} ulong1;

typedef struct {
    unsigned long x, y;
} ulong2;

typedef struct {
    unsigned long x, y, z;
} ulong3;

typedef struct {
    unsigned long x, y, z, w;
} ulong4;

typedef struct {
    unsigned long long x;
} ulonglong1;

typedef struct {
    unsigned long long x, y;
} ulonglong2;

typedef struct {
    unsigned long long x, y, z;
} ulonglong3;

typedef struct {
    unsigned long long x, y, z, w;
} ulonglong4;

typedef struct {
    float x;
} float1;

typedef struct {
    float x, y;
} float2;

typedef struct {
    float x, y, z;
} float3;

typedef struct {
    float x, y, z, w;
} float4;

typedef struct {
    double x;
} double1;

typedef struct {
    double x, y;
} double2;

typedef struct {
    double x, y, z;
} double3;

typedef struct {
    double x, y, z, w;
} double4;

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

inline uchar1 make_uchar1(unsigned char x)
{
    uchar1 u1;
    u1.x = x;
    return u1;
}

inline uchar2 make_uchar2(unsigned char x, unsigned char y)
{
    uchar2 u2;
    u2.x = x;
    u2.y = y;
    return u2;
}

inline uchar3 make_uchar3(unsigned char x, unsigned char y, unsigned char z)
{
    uchar3 u3;
    u3.x = x;
    u3.y = y;
    u3.z = z;
    return u3;
}

inline uchar4 make_uchar4(unsigned char x, unsigned char y, unsigned char z, unsigned char w)
{
    uchar4 u4;
    u4.x = x;
    u4.y = y;
    u4.z = z;
    u4.w = w;
    return u4;
}

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


