
#pragma once

#include <visionaray/texture/texture.h>
#include <visionaray/math/unorm.h>

namespace visionaray
{
    class Texture
    {
    public:
        Texture();
        Texture(OWLTexelFormat         texelFormat,
                uint32_t               size_x,
                uint32_t               size_y,
                const void            *texels,
                OWLTextureFilterMode   filterMode,
                OWLTextureAddressMode  addressMode,
                OWLTextureColorSpace   colorSpace,
                uint32_t               linePitchInBytes);
        Texture(OWLTexelFormat         texelFormat,
                uint32_t               size_x,
                uint32_t               size_y,
                uint32_t               size_z,
                const void            *texels,
                OWLTextureFilterMode   filterMode,
                OWLTextureAddressMode  addressMode,
                OWLTextureColorSpace   colorSpace,
                uint32_t               linePitchInBytes);
       ~Texture();

        void reset(OWLTexelFormat         texelFormat,
                   uint32_t               size_x,
                   uint32_t               size_y,
                   const void            *texels,
                   OWLTextureFilterMode   filterMode,
                   OWLTextureAddressMode  addressMode,
                   OWLTextureColorSpace   colorSpace,
                   uint32_t               linePitchInBytes);

        void reset(OWLTexelFormat         texelFormat,
                   uint32_t               size_x,
                   uint32_t               size_y,
                   uint32_t               size_z,
                   const void            *texels,
                   OWLTextureFilterMode   filterMode,
                   OWLTextureAddressMode  addressMode,
                   OWLTextureColorSpace   colorSpace,
                   uint32_t               linePitchInBytes);

        struct
        {
            // The texture data
            std::shared_ptr<texture<vector<4, unorm<8>>, 2>> data = nullptr;

            // An accessor to the texture data
            texture_ref<vector<4, unorm<8>>, 2> accessor;
        } asRGBA8_2D;

        struct
        {
            // The texture data
            std::shared_ptr<texture<vector<4, float>, 2>> data = nullptr;

            // An accessor to the texture data
            texture_ref<vector<4, float>, 2> accessor;
        } asRGBA32F_2D;

        struct
        {
            // The texture data
            std::shared_ptr<texture<unorm<8>, 3>> data = nullptr;

            // An accessor to the texture data
            texture_ref<unorm<8>, 3> accessor;
        } asR8_3D;

        struct
        {
            // The texture data
            std::shared_ptr<texture<float, 3>> data = nullptr;

            // An accessor to the texture data
            texture_ref<float, 3> accessor;
        } asR32F_3D;
    };
} // visionaray
