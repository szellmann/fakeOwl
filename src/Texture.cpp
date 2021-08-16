#include <cstddef>

#include "VisionarayTexture.h" // keep on top!
#include "Texture.h"

namespace fake
{
    Texture::Texture(OWLTexelFormat         texelFormat,
                     uint32_t               size_x,
                     uint32_t               size_y,
                     const void            *texels,
                     OWLTextureFilterMode   filterMode,
                     OWLTextureAddressMode  addressMode,
                     OWLTextureColorSpace   colorSpace,
                     uint32_t               linePitchInBytes)
        : vtex(new visionaray::Texture)
        , texelFormat(texelFormat)
    {
        vtex->reset(texelFormat, size_x, size_y, texels,
                    filterMode, addressMode, colorSpace,
                    linePitchInBytes);

        if (vtex->asRGBA8_2D.data != nullptr)
        {
            accessor = &vtex->asRGBA8_2D.accessor;
        }
        else if (vtex->asRGBA32F_2D.data != nullptr)
        {
            accessor = &vtex->asRGBA32F_2D.accessor;
        }

        textureHandle = registerTexture(ref());
    }

    Texture::Texture(OWLTexelFormat         texelFormat,
                     uint32_t               size_x,
                     uint32_t               size_y,
                     uint32_t               size_z,
                     const void            *texels,
                     OWLTextureFilterMode   filterMode,
                     OWLTextureAddressMode  addressMode,
                     OWLTextureColorSpace   colorSpace,
                     uint32_t               linePitchInBytes)
        : vtex(new visionaray::Texture)
        , texelFormat(texelFormat)
    {
        vtex->reset(texelFormat, size_x, size_y, size_z, texels,
                    filterMode, addressMode, colorSpace,
                    linePitchInBytes);

        if (vtex->asR8_3D.data != nullptr)
        {
            accessor = &vtex->asR8_3D.accessor;
        }
        else if (vtex->asR32F_3D.data != nullptr)
        {
            accessor = &vtex->asR32F_3D.accessor;
        }

        textureHandle = registerTexture(ref());
    }

    Texture::~Texture()
    {
    }

    Texture::Reference Texture::ref()
    {
        return { textureHandle, texelFormat, accessor };
    }
} // fake


namespace fake
{
    static Texture::Reference textures[1<<16];
    static std::size_t numTextures = 0;

    TextureHandle registerTexture(Texture::Reference tex)
    {
        std::size_t i = numTextures++;
        textures[i] = tex;
        tex.handle = (TextureHandle)&textures[i];
        return tex.handle;
    }

    //--- CUDA texture interface --------------------------

    void sampleTexture2D(float4& result, cudaTextureObject_t obj, float tcx, float tcy)
    {
        Texture::Reference* texture = (Texture::Reference*)obj;

        if (texture == nullptr)
        {
            result = { 0.f, 0.f, 0.f, 0.f };
            return;
        }

        if (texture->texelFormat == OWL_TEXEL_FORMAT_RGBA8)
        {
            using Reference = visionaray::texture_ref<visionaray::vector<4, visionaray::unorm<8>>, 2>;

            Reference ref = *(Reference*)texture->accessor;

            visionaray::vec4 tx = visionaray::tex2D(ref, visionaray::vec2(tcx, tcy));

            result = { tx.x, tx.y, tx.z, tx.w };
        }
        else if (texture->texelFormat == OWL_TEXEL_FORMAT_RGBA32F)
        {
            using Reference = visionaray::texture_ref<visionaray::vector<4, float>, 2>;

            Reference ref = *(Reference*)texture->accessor;

            visionaray::vec4 tx = visionaray::tex2D(ref, visionaray::vec2(tcx, tcy));

            result = { tx.x, tx.y, tx.z, tx.w };
        }
    }

    void sampleTexture3D(float& result, cudaTextureObject_t obj, float tcx, float tcy, float tcz)
    {
        Texture::Reference* texture = (Texture::Reference*)obj;

        if (texture == nullptr)
        {
            result = 0.f;
            return;
        }

        if (texture->texelFormat == OWL_TEXEL_FORMAT_R8)
        {
            using Reference = visionaray::texture_ref<visionaray::unorm<8>, 3>;

            Reference ref = *(Reference*)texture->accessor;

            result = visionaray::tex3D(ref, visionaray::vec3(tcx, tcy, tcz));
        }
        else if (texture->texelFormat == OWL_TEXEL_FORMAT_R32F)
        {
            using Reference = visionaray::texture_ref<float, 3>;

            Reference ref = *(Reference*)texture->accessor;

            result = visionaray::tex3D(ref, visionaray::vec3(tcx, tcy, tcz));
        }
    }

} // fake
