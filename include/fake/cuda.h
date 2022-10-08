
#pragma once

#if !defined(__aarch64__)
#include <x86intrin.h>
#endif
#include <stddef.h>
#include <stdint.h>

#if !defined(__CUDACC__) && !defined(CUDARTAPI)

#define __CUDACC__ 1

#define __global__
#define __constant__
#define __host__
#define __device__

#define __forceinline__ __attribute__((always_inline))

// enums
typedef enum cudaError {
  cudaSuccess                =  0,
  cudaErrorInvalidValue      =  1,
  cudaErrorMemoryAllocation  =  2,
  cudaErrorInvalidPitchValue = 12,
} cudaError_t;

enum cudaChannelFormatKind
{
    cudaChannelFormatKindSigned   = 0,
    cudaChannelFormatKindUnsigned = 1,
    cudaChannelFormatKindFloat    = 2,
    cudaChannelFormatKindNone     = 3,
    // The rest is not supported, so for the moment
    // let's just catch that at compile time..
};

enum cudaMemcpyKind
{
    cudaMemcpyHostToHost     = 0,
    cudaMemcpyHostToDevice   = 1,
    cudaMemcpyDeviceToHost   = 2,
    cudaMemcpyDeviceToDevice = 3,
    cudaMemcpyDefault        = 4,
};

enum cudaResourceType {
    cudaResourceTypeArray          = 0x00,
    cudaResourceTypeMipmappedArray = 0x01,
    cudaResourceTypeLinear         = 0x02,
    cudaResourceTypePitch2D        = 0x03,
};

enum cudaResourceViewFormat {
    cudaResViewFormatNone          = 0x00,
    cudaResViewFormatUnsignedChar1 = 0x01,
    cudaResViewFormatUnsignedChar2 = 0x02,
    cudaResViewFormatUnsignedChar4 = 0x03,
    //...
    cudaResViewFormatFloat1        = 0x16,
    cudaResViewFormatFloat2        = 0x17,
    cudaResViewFormatFloat4        = 0x18,
    //...
};

enum cudaTextureAddressMode {
    cudaAddressModeWrap   = 0,
    cudaAddressModeClamp  = 1,
    cudaAddressModeMirror = 2,
    cudaAddressModeBorder = 3,
};

enum cudaTextureFilterMode {
    cudaFilterModePoint  = 0,
    cudaFilterModeLinear = 1,
};

enum cudaTextureReadMode {
    cudaReadModeElementType    = 0,
    cudaReadModeNormaizedFloat = 1,
};

inline uint64_t clock64()
{
#if defined(__aarch64__)
    uint64_t cnt;
    asm volatile("mrs %0, cntvct_el0" : "=r" (cnt));
    return cnt;
#else
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
#endif
}

#if 0
// TODO: need to define CUDA_ARCH in fake_owl_compile_and_embed
template <typename T> inline T min(const T& x, const T& y) { return x < y ? x : y; }
template <typename T> inline T max(const T& x, const T& y) { return x < y ? y : x; }
inline float __saturatef(const float &f) { return min(1.f,max(0.f,f)); }
#else
inline float __saturatef(const float &f)
{
    auto min = [](float x, float y) { return x < y ? x : y; };
    auto max = [](float x, float y) { return x < y ? y : x; };
    return min(1.f,max(0.f,f));
}
#endif

namespace fake
{
    static thread_local cudaError_t lastCudaError = cudaSuccess;
}

__host__
static cudaError_t cudaGetLastError(void)
{
    return fake::lastCudaError;
}

__host__
static const char* cudaGetErrorString(cudaError_t err)
{
    if (err == cudaSuccess) {
        return "success";
    } else if (err == cudaErrorInvalidValue) {
        return "invalid value";
    } else if (err == cudaErrorMemoryAllocation) {
        return "memory allocation";
    } else if (err == cudaErrorInvalidPitchValue) {
        return "invalid pitch value";
    }

    return "unknown error";
}

__host__ __device__
static cudaError_t cudaDeviceSynchronize(void)
{
    // TODO
    return cudaSuccess;
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
#if defined(__cplusplus)
    dim3() = default;
    dim3(unsigned x) : x(x), y(1), z(1) {}
    dim3(unsigned x, unsigned y) : x(x), y(y), z(1) {}
    dim3(unsigned x, unsigned y, unsigned z) : x(x), y(y), z(z) {}
    operator uint3() const { return { x, y, z }; }
#endif
    unsigned x, y, z;
};

/* TODO: these are just dummies: */
static thread_local dim3 threadIdx;
static thread_local dim3 blockDim;
static thread_local dim3 blockIdx;
static thread_local dim3 gridDim;

/* Driver API typedefs */
struct CUarray_st;
typedef CUarray_st* CUarray;

struct CUctx_st;
typedef CUctx_st* CUcontext;

typedef int CUdevice_v1;
typedef CUdevice_v1 CUdevice;

typedef unsigned CUdeviceptr_v2;
typedef CUdeviceptr_v2 CUdeviceptr;

struct CUstream_st;
typedef CUstream_st* CUstream;

struct CUevent_st;
typedef CUevent_st* CUevent;

/* runtime API typedefs */
struct cudaGraphicsResource;
typedef cudaGraphicsResource* cudaGraphicsResource_t;

namespace fake
{
    typedef unsigned long long TextureHandle;
}

typedef fake::TextureHandle cudaTextureObject_t;
typedef fake::TextureHandle CUtexObject;

typedef struct cudaArray *cudaArray_t;
typedef const struct cudaArray *cudaArray_const_t;

typedef struct {
} cudaMipmappedArray_t;

struct cudaChannelFormatDesc {
    int x, y, z, w;
    enum cudaChannelFormatKind f;
};

struct cudaResourceDesc {
    enum cudaResourceType resType;
    union {
        struct {
            cudaArray_t array;
        } array;
        struct {
            cudaMipmappedArray_t mipmap;
        } mipmap;
        struct {
            void *devPtr;
            struct cudaChannelFormatDesc desc;
            size_t sizeInBytes;
        } linear;
        struct {
            void *devPtr;
            struct cudaChannelFormatDesc desc;
            size_t width;
            size_t height;
            size_t pitchInBytes;
        } pitch2D;
    } res;
};

struct cudaResourceViewDesc {
    enum cudaResourceViewFormat format;
    size_t                      width;
    size_t                      height;
    size_t                      depth;
    unsigned int                firstMipmapLevel;
    unsigned int                lastMipmapLevel;
    unsigned int                firstLayer;
    unsigned int                lastLayer;
};

struct cudaTextureDesc {
    enum cudaTextureAddressMode addressMode[3];
    enum cudaTextureFilterMode  filterMode;
    enum cudaTextureReadMode    readMode;
    int                         sRGB;
    float                       borderColor[4];
    int                         normalizedCoords;
    unsigned                    maxAnisotropy;
    enum cudaTextureFilterMode  mipmapFilterMode;
    float                       mipmapLevelBias;
    float                       minMipmapLevelClamp;
    float                       maxMipmapLevelClamp;
};

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

inline float1 make_float1(float x)
{
    float1 f1;
    f1.x = x;
    return f1;
}

inline float2 make_float2(float x, float y)
{
    float2 f2;
    f2.x = x;
    f2.y = y;
    return f2;
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

// ------------------------------------------------------------------
// CUDA texture functions
// ------------------------------------------------------------------

namespace fake
{
    template <typename T>
    struct ChannelFormatDesc {};

    template <> struct ChannelFormatDesc<char1> { 
        static cudaChannelFormatDesc get() { return {8,0,0,0,cudaChannelFormatKindSigned}; }
    };

    template <> struct ChannelFormatDesc<char2> { 
        static cudaChannelFormatDesc get() { return {8,8,0,0,cudaChannelFormatKindSigned}; }
    };

    template <> struct ChannelFormatDesc<char3> { 
        static cudaChannelFormatDesc get() { return {8,8,8,0,cudaChannelFormatKindSigned}; }
    };

    template <> struct ChannelFormatDesc<char4> { 
        static cudaChannelFormatDesc get() { return {8,8,8,8,cudaChannelFormatKindSigned}; }
    };

    template <> struct ChannelFormatDesc<uchar1> { 
        static cudaChannelFormatDesc get() { return {8,0,0,0,cudaChannelFormatKindUnsigned}; }
    };

    template <> struct ChannelFormatDesc<uchar2> { 
        static cudaChannelFormatDesc get() { return {8,8,0,0,cudaChannelFormatKindUnsigned}; }
    };

    template <> struct ChannelFormatDesc<uchar3> { 
        static cudaChannelFormatDesc get() { return {8,8,8,0,cudaChannelFormatKindUnsigned}; }
    };

    template <> struct ChannelFormatDesc<uchar4> { 
        static cudaChannelFormatDesc get() { return {8,8,8,8,cudaChannelFormatKindUnsigned}; }
    };

    template <> struct ChannelFormatDesc<short1> { 
        static cudaChannelFormatDesc get() { return {16,0,0,0,cudaChannelFormatKindSigned}; }
    };

    template <> struct ChannelFormatDesc<short2> { 
        static cudaChannelFormatDesc get() { return {16,16,0,0,cudaChannelFormatKindSigned}; }
    };

    template <> struct ChannelFormatDesc<short3> { 
        static cudaChannelFormatDesc get() { return {16,16,16,0,cudaChannelFormatKindSigned}; }
    };

    template <> struct ChannelFormatDesc<short4> { 
        static cudaChannelFormatDesc get() { return {16,16,16,16,cudaChannelFormatKindSigned}; }
    };

    template <> struct ChannelFormatDesc<ushort1> { 
        static cudaChannelFormatDesc get() { return {16,0,0,0,cudaChannelFormatKindUnsigned}; }
    };

    template <> struct ChannelFormatDesc<ushort2> { 
        static cudaChannelFormatDesc get() { return {16,16,0,0,cudaChannelFormatKindUnsigned}; }
    };

    template <> struct ChannelFormatDesc<ushort3> { 
        static cudaChannelFormatDesc get() { return {16,16,16,0,cudaChannelFormatKindUnsigned}; }
    };

    template <> struct ChannelFormatDesc<ushort4> { 
        static cudaChannelFormatDesc get() { return {16,16,16,16,cudaChannelFormatKindUnsigned}; }
    };

    template <> struct ChannelFormatDesc<int1> { 
        static cudaChannelFormatDesc get() { return {32,0,0,0,cudaChannelFormatKindSigned}; }
    };

    template <> struct ChannelFormatDesc<int2> { 
        static cudaChannelFormatDesc get() { return {32,32,0,0,cudaChannelFormatKindSigned}; }
    };

    template <> struct ChannelFormatDesc<int3> { 
        static cudaChannelFormatDesc get() { return {32,32,32,0,cudaChannelFormatKindSigned}; }
    };

    template <> struct ChannelFormatDesc<int4> { 
        static cudaChannelFormatDesc get() { return {32,32,32,32,cudaChannelFormatKindSigned}; }
    };

    template <> struct ChannelFormatDesc<uint1> { 
        static cudaChannelFormatDesc get() { return {32,0,0,0,cudaChannelFormatKindUnsigned}; }
    };

    template <> struct ChannelFormatDesc<uint2> { 
        static cudaChannelFormatDesc get() { return {32,32,0,0,cudaChannelFormatKindUnsigned}; }
    };

    template <> struct ChannelFormatDesc<uint3> { 
        static cudaChannelFormatDesc get() { return {32,32,32,0,cudaChannelFormatKindUnsigned}; }
    };

    template <> struct ChannelFormatDesc<uint4> { 
        static cudaChannelFormatDesc get() { return {32,32,32,32,cudaChannelFormatKindUnsigned}; }
    };

    template <> struct ChannelFormatDesc<long> { 
        static cudaChannelFormatDesc get() { return {0,0,0,0,cudaChannelFormatKindSigned}; }
    };

    template <> struct ChannelFormatDesc<long1> { 
        static cudaChannelFormatDesc get() { return {0,0,0,0,cudaChannelFormatKindSigned}; }
    };

    template <> struct ChannelFormatDesc<long2> { 
        static cudaChannelFormatDesc get() { return {0,0,0,0,cudaChannelFormatKindSigned}; }
    };

    template <> struct ChannelFormatDesc<long3> { 
        static cudaChannelFormatDesc get() { return {0,0,0,0,cudaChannelFormatKindSigned}; }
    };

    template <> struct ChannelFormatDesc<long4> { 
        static cudaChannelFormatDesc get() { return {0,0,0,0,cudaChannelFormatKindSigned}; }
    };

    template <> struct ChannelFormatDesc<ulong1> {
        static cudaChannelFormatDesc get() { return {0,0,0,0,cudaChannelFormatKindUnsigned}; }
    };

    template <> struct ChannelFormatDesc<ulong2> {
        static cudaChannelFormatDesc get() { return {0,0,0,0,cudaChannelFormatKindUnsigned}; }
    };

    template <> struct ChannelFormatDesc<ulong3> {
        static cudaChannelFormatDesc get() { return {0,0,0,0,cudaChannelFormatKindUnsigned}; }
    };

    template <> struct ChannelFormatDesc<ulong4> {
        static cudaChannelFormatDesc get() { return {0,0,0,0,cudaChannelFormatKindUnsigned}; }
    };

    template <> struct ChannelFormatDesc<float1> { 
        static cudaChannelFormatDesc get() { return {32,0,0,0,cudaChannelFormatKindFloat}; }
    };

    template <> struct ChannelFormatDesc<float2> { 
        static cudaChannelFormatDesc get() { return {32,32,0,0,cudaChannelFormatKindFloat}; }
    };

    template <> struct ChannelFormatDesc<float3> { 
        static cudaChannelFormatDesc get() { return {32,32,32,0,cudaChannelFormatKindFloat}; }
    };

    template <> struct ChannelFormatDesc<float4> { 
        static cudaChannelFormatDesc get() { return {32,32,32,32,cudaChannelFormatKindFloat}; }
    };

    void sampleTexture1D(float4& result, TextureHandle obj, float tcx);

    void sampleTexture2D(float4& result, TextureHandle obj, float tcx, float tcy);

    void sampleTexture3D(float& result, TextureHandle obj, float tcx, float tcy, float tcz);
    void sampleTexture3D(float4& result, TextureHandle obj, float tcx, float tcy, float tcz);
} // fake

template <typename T>
cudaChannelFormatDesc cudaCreateChannelDesc()
{
    return fake::ChannelFormatDesc<T>::get();
}

cudaError_t cudaDestroyTextureObject(cudaTextureObject_t texObject);

cudaError_t cudaMallocArray(cudaArray_t* array,
                            const cudaChannelFormatDesc* desc,
                            size_t width,
                            size_t height = 0,
                            unsigned flags = 0);

cudaError_t cudaMemcpy2DToArray(cudaArray_t dst,
                                size_t wOffset,
                                size_t hOffset,
                                const void* src,
                                size_t spitch,
                                size_t width,
                                size_t height,
                                cudaMemcpyKind kind);

cudaError_t cudaCreateTextureObject(cudaTextureObject_t* pTexObject,
                                    const cudaResourceDesc* pResDesc,
                                    const cudaTextureDesc* pTexDesc,
                                    const cudaResourceViewDesc* pResViewDesc);

template <typename T>
inline T tex1D(cudaTextureObject_t obj, float tcx)
{
    T res;
    fake::sampleTexture1D(res, obj, tcx);
    return res;
}

template <typename T>
inline void tex1D(T* res, cudaTextureObject_t obj, float tcx)
{
    fake::sampleTexture1D(*res, obj, tcx);
}

template <typename T>
inline T tex2D(cudaTextureObject_t obj, float tcx, float tcy)
{
    T res;
    fake::sampleTexture2D(res, obj, tcx, tcy);
    return res;
}

template <typename T>
inline void tex2D(T* res, cudaTextureObject_t obj, float tcx, float tcy)
{
    fake::sampleTexture2D(*res, obj, tcx, tcy);
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


