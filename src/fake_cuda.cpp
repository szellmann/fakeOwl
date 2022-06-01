
#include "VisionarayTexture.h" // keep on top!

#include <fake/cuda.h>

#include "Texture.h"

enum class CudaType {
    Char1,
    Char2,
    Char3,
    Char4,
    Uchar1,
    Uchar2,
    Uchar3,
    Uchar4,

    Short1,
    Short2,
    Short3,
    Short4,
    Ushort1,
    Ushort2,
    Ushort3,
    Ushort4,

    Int1,
    Int2,
    Int3,
    Int4,
    Uint1,
    Uint2,
    Uint3,
    Uint4,

    Long1,
    Long2,
    Long3,
    Long4,
    Ulong1,
    Ulong2,
    Ulong3,
    Ulong4,

    Float1,
    Float2,
    Float3,
    Float4,

    Unknown,
};

inline size_t numBytes(CudaType t) {
    switch (t) {
    case CudaType::Char1:   return 1;
    case CudaType::Char2:   return 2;
    case CudaType::Char3:   return 3;
    case CudaType::Char4:   return 4;
    case CudaType::Uchar1:  return 1;
    case CudaType::Uchar2:  return 2;
    case CudaType::Uchar3:  return 3;
    case CudaType::Uchar4:  return 4;

    case CudaType::Short1:  return 2;
    case CudaType::Short2:  return 4;
    case CudaType::Short3:  return 6;
    case CudaType::Short4:  return 8;
    case CudaType::Ushort1: return 2;
    case CudaType::Ushort2: return 4;
    case CudaType::Ushort3: return 6;
    case CudaType::Ushort4: return 8;

    case CudaType::Int1:    return  4;
    case CudaType::Int2:    return  8;
    case CudaType::Int3:    return 12;
    case CudaType::Int4:    return 16;
    case CudaType::Uint1:   return  4;
    case CudaType::Uint2:   return  8;
    case CudaType::Uint3:   return 12;
    case CudaType::Uint4:   return 16;

    case CudaType::Float1:  return  4;
    case CudaType::Float2:  return  8;
    case CudaType::Float3:  return 12;
    case CudaType::Float4:  return 16;

    default: return 0;
    }

    return 0;
}

inline CudaType toCudaType(cudaChannelFormatDesc d) {
    if (d.x==8 && d.y==0 && d.z==0 && d.w==0 && d.f == cudaChannelFormatKindSigned)
        return CudaType::Char1;
    else if (d.x==8 && d.y==8 && d.z==0 && d.w==0 && d.f == cudaChannelFormatKindSigned)
        return CudaType::Char2;
    else if (d.x==8 && d.y==8 && d.z==8 && d.w==0 && d.f == cudaChannelFormatKindSigned)
        return CudaType::Char3;
    else if (d.x==8 && d.y==8 && d.z==8 && d.w==8 && d.f == cudaChannelFormatKindSigned)
        return CudaType::Char4;
    else if (d.x==8 && d.y==0 && d.z==0 && d.w==0 && d.f == cudaChannelFormatKindUnsigned)
        return CudaType::Uchar1;
    else if (d.x==8 && d.y==8 && d.z==0 && d.w==0 && d.f == cudaChannelFormatKindUnsigned)
        return CudaType::Uchar2;
    else if (d.x==8 && d.y==8 && d.z==8 && d.w==0 && d.f == cudaChannelFormatKindUnsigned)
        return CudaType::Uchar3;
    else if (d.x==8 && d.y==8 && d.z==8 && d.w==8 && d.f == cudaChannelFormatKindUnsigned)
        return CudaType::Uchar4;

    else if (d.x==16 && d.y==0 && d.z==0 && d.w==0 && d.f == cudaChannelFormatKindSigned)
        return CudaType::Short1;
    else if (d.x==16 && d.y==16 && d.z==0 && d.w==0 && d.f == cudaChannelFormatKindSigned)
        return CudaType::Short2;
    else if (d.x==16 && d.y==16 && d.z==16 && d.w==0 && d.f == cudaChannelFormatKindSigned)
        return CudaType::Short3;
    else if (d.x==16 && d.y==16 && d.z==16 && d.w==16 && d.f == cudaChannelFormatKindSigned)
        return CudaType::Short4;
    else if (d.x==16 && d.y==0 && d.z==0 && d.w==0 && d.f == cudaChannelFormatKindUnsigned)
        return CudaType::Ushort1;
    else if (d.x==16 && d.y==16 && d.z==0 && d.w==0 && d.f == cudaChannelFormatKindUnsigned)
        return CudaType::Ushort2;
    else if (d.x==16 && d.y==16 && d.z==16 && d.w==0 && d.f == cudaChannelFormatKindUnsigned)
        return CudaType::Ushort3;
    else if (d.x==16 && d.y==16 && d.z==16 && d.w==16 && d.f == cudaChannelFormatKindUnsigned)
        return CudaType::Ushort4;

    else if (d.x==32 && d.y==0 && d.z==0 && d.w==0 && d.f == cudaChannelFormatKindSigned)
        return CudaType::Int1;
    else if (d.x==32 && d.y==32 && d.z==0 && d.w==0 && d.f == cudaChannelFormatKindSigned)
        return CudaType::Int2;
    else if (d.x==32 && d.y==32 && d.z==32 && d.w==0 && d.f == cudaChannelFormatKindSigned)
        return CudaType::Int3;
    else if (d.x==32 && d.y==32 && d.z==32 && d.w==32 && d.f == cudaChannelFormatKindSigned)
        return CudaType::Int4;
    else if (d.x==32 && d.y==0 && d.z==0 && d.w==0 && d.f == cudaChannelFormatKindUnsigned)
        return CudaType::Uint1;
    else if (d.x==32 && d.y==32 && d.z==0 && d.w==0 && d.f == cudaChannelFormatKindUnsigned)
        return CudaType::Uint2;
    else if (d.x==32 && d.y==32 && d.z==32 && d.w==0 && d.f == cudaChannelFormatKindUnsigned)
        return CudaType::Uint3;
    else if (d.x==32 && d.y==32 && d.z==32 && d.w==32 && d.f == cudaChannelFormatKindUnsigned)
        return CudaType::Uint4;

    else if (d.x==32 && d.y==0 && d.z==0 && d.w==0 && d.f == cudaChannelFormatKindFloat)
        return CudaType::Float1;
    else if (d.x==32 && d.y==32 && d.z==0 && d.w==0 && d.f == cudaChannelFormatKindFloat)
        return CudaType::Float2;
    else if (d.x==32 && d.y==32 && d.z==32 && d.w==0 && d.f == cudaChannelFormatKindFloat)
        return CudaType::Float3;
    else if (d.x==32 && d.y==32 && d.z==32 && d.w==32 && d.f == cudaChannelFormatKindFloat)
        return CudaType::Float4;

    return CudaType::Unknown;
}

struct cudaArray
{
    cudaArray(size_t w, size_t h, CudaType t)
        : width(w)
        , height(h)
        , type(t)
    {
        using namespace visionaray;

        unsigned dims = height <= 1 ? 1 : 2;

        // 1D
        if (t == CudaType::Float1 && dims == 1)
            ptr = (cudaArray_t *)new texture<float,1>(width);
        else if (t == CudaType::Float2 && dims == 1)
            ptr = (cudaArray_t *)new texture<vec2,1>(width);
        else if (t == CudaType::Float3 && dims == 1)
            ptr = (cudaArray_t *)new texture<vec3,1>(width);
        else if (t == CudaType::Float4 && dims == 1)
            ptr = (cudaArray_t *)new texture<vec4,1>(width);

        // 2D
        else if (t == CudaType::Float1 && dims == 2)
            ptr = (cudaArray_t *)new texture<float,2>(width,height);
        else if (t == CudaType::Float2 && dims == 2)
            ptr = (cudaArray_t *)new texture<vec2,2>(width,height);
        else if (t == CudaType::Float3 && dims == 2)
            ptr = (cudaArray_t *)new texture<vec3,2>(width,height);
        else if (t == CudaType::Float4 && dims == 2)
            ptr = (cudaArray_t *)new texture<vec4,2>(width,height);
        else {
            width = height = 0;
            type = CudaType::Unknown;
            ptr = nullptr;
        }
    }

    const char* data() const
    {
        using namespace visionaray;

        unsigned dims = height <= 1 ? 1 : 2;

        // 1D
        if (type == CudaType::Float1 && dims == 1)
            return (const char *)((texture<float,1>*)ptr)->data();
        else if (type == CudaType::Float2 && dims == 1)
            return (const char *)((texture<vec2,1>*)ptr)->data();
        else if (type == CudaType::Float3 && dims == 1)
            return (const char *)((texture<vec3,1>*)ptr)->data();
        else if (type == CudaType::Float4 && dims == 1)
            return (const char *)((texture<vec4,1>*)ptr)->data();

        // 2D
        else if (type == CudaType::Float1 && dims == 2)
            return (const char *)((texture<float,2>*)ptr)->data();
        else if (type == CudaType::Float2 && dims == 2)
            return (const char *)((texture<vec2,2>*)ptr)->data();
        else if (type == CudaType::Float3 && dims == 2)
            return (const char *)((texture<vec3,2>*)ptr)->data();
        else if (type == CudaType::Float4 && dims == 2)
            return (const char *)((texture<vec4,2>*)ptr)->data();

        else
            return nullptr;
    }

    size_t width = 0;
    size_t height = 0;
    CudaType type = CudaType::Unknown;
    void *ptr = nullptr;
    void *accessor = nullptr;
};

inline visionaray::tex_address_mode mapAddressMode(cudaTextureAddressMode mode)
{
    if (mode==cudaAddressModeWrap)
        return visionaray::Wrap;
    else if (mode==cudaAddressModeMirror)
        return visionaray::Mirror;
    else if (mode==cudaAddressModeClamp)
        return visionaray::Clamp;
    else
        return visionaray::Border;
}

inline visionaray::tex_filter_mode mapFilterMode(cudaTextureFilterMode mode)
{
    if (mode==cudaFilterModePoint)
        return visionaray::Nearest;
    else
        return visionaray::Linear;
}

template <typename T>
void prepareTexture(visionaray::texture<T,1>& tex, const cudaTextureDesc& desc)
{
    tex.set_address_mode(0,mapAddressMode(desc.addressMode[0]));
    tex.set_filter_mode(mapFilterMode(desc.filterMode));
    tex.set_normalized_coords(desc.normalizedCoords);
    tex.set_color_space(desc.sRGB ? visionaray::sRGB : visionaray::RGB);
}

// API

cudaError_t cudaDestroyTextureObject(cudaTextureObject_t texObject)
{
}

cudaError_t cudaMallocArray(cudaArray_t* array,
                            const cudaChannelFormatDesc* desc,
                            size_t width,
                            size_t height,
                            unsigned flags)
{
    *array = new cudaArray(width,height,toCudaType(*desc));

    if ((*array)->width==0 || (*array)->height==0 || (*array)->type==CudaType::Unknown || (*array)->ptr==nullptr)
        return cudaErrorInvalidValue;

    return cudaSuccess;
}

cudaError_t cudaMemcpy2DToArray(cudaArray_t dst,
                                size_t wOffset, /* in bytes */
                                size_t hOffset, /* in rows */
                                const void* src,
                                size_t spitch,
                                size_t width,   /* in bytes */
                                size_t height,  /* in rows */
                                cudaMemcpyKind kind)
{
    if (0/*spitch exceeds max value*/)
        return cudaErrorInvalidPitchValue;

    if (width > spitch)
        return cudaErrorInvalidValue;

    if (wOffset + width > dst->width * numBytes(dst->type))
        return cudaErrorInvalidValue;

    // TODO: invalid copy direction..

    for (size_t row=hOffset; row!=hOffset+height; ++row) {
        size_t index = row * spitch + wOffset;
        memcpy((char *)dst->data()+index,((const char*)src)+index,width);
    }

    return cudaSuccess;
}

cudaError_t cudaCreateTextureObject(cudaTextureObject_t* pTexObject,
                                    const cudaResourceDesc* pResDesc,
                                    const cudaTextureDesc* pTexDesc,
                                    const cudaResourceViewDesc* pResViewDesc)
{
    using namespace visionaray;

    cudaArray* arr = pResDesc->res.array.array;

    if (1) {
        texture<vec4,1>* tex = (texture<vec4,1>*)arr->ptr;

        prepareTexture(*tex,*pTexDesc);

        arr->accessor = new texture_ref<vec4,1>(*tex);

        fake::Texture::Reference ref = { OWL_TEXEL_FORMAT_RGBA32F, arr->accessor };

        *pTexObject = registerTexture(ref);
    }

    return cudaSuccess;
}


