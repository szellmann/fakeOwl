
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include <owl/owl_host.h>
#include <fake/cuda.h>

#include "Resource.h"

namespace visionaray
{
    class Texture;
} // visionaray

namespace fake
{
    struct Texture : ContextResource
    {
        struct Reference
        {
            TextureHandle handle;
            OWLTexelFormat texelFormat;
            const void* accessor;
        };

        Texture() = default;

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

        Reference ref();

        TextureHandle textureHandle;
        const void* accessor;

    private:
        std::shared_ptr<visionaray::Texture> vtex;
        OWLTexelFormat texelFormat;
    };
} // fake


namespace fake
{
    TextureHandle registerTexture(Texture::Reference tex);

    Texture::Reference& getTexture(TextureHandle handle);

    Texture::Reference* getTextureList();

    std::size_t getNumTextures();

} // fake
