
#include <owl/owl_host.h>

#include "Texture.h"
#include "VisionarayTexture.h"

namespace visionaray
{

    tex_filter_mode cast(OWLTextureFilterMode mode)
    {
        return mode == OWL_TEXTURE_NEAREST ? Nearest : Linear;
    }

    tex_address_mode cast(OWLTextureAddressMode mode)
    {
        if (mode == OWL_TEXTURE_WRAP)
            return Wrap;
        else if (mode == OWL_TEXTURE_CLAMP)
            return Clamp;
        else if (mode == OWL_TEXTURE_BORDER)
            return Border;
        else
            return Mirror;
    }

    tex_color_space cast(OWLTextureColorSpace colorSpace)
    {
        if (colorSpace == OWL_COLOR_SPACE_LINEAR)
            return RGB;
        else
            return sRGB;
    }

    Texture::Texture()
    {
    }

    Texture::Texture(OWLTexelFormat         texelFormat,
                     uint32_t               size_x,
                     uint32_t               size_y,
                     const void            *texels,
                     OWLTextureFilterMode   filterMode,
                     OWLTextureAddressMode  addressMode,
                     OWLTextureColorSpace   colorSpace,
                     uint32_t               linePitchInBytes)
    {
        reset(texelFormat, size_x, size_y, texels,
              filterMode, addressMode, colorSpace,
              linePitchInBytes);
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
    {
        reset(texelFormat, size_x, size_y, size_z, texels,
              filterMode, addressMode, colorSpace,
              linePitchInBytes);
    }

    Texture::~Texture()
    {
    }

    void Texture::reset(OWLTexelFormat         texelFormat,
                        uint32_t               size_x,
                        uint32_t               size_y,
                        const void            *texels,
                        OWLTextureFilterMode   filterMode,
                        OWLTextureAddressMode  addressMode,
                        OWLTextureColorSpace   colorSpace,
                        uint32_t               linePitchInBytes) // TODO: mind this!!
    {
        if (texelFormat == OWL_TEXEL_FORMAT_RGBA8)
        {
            asRGBA8_2D.data = std::make_shared<texture<vector<4, unorm<8>>, 2>>(size_x, size_y);
            asRGBA8_2D.data->set_address_mode(cast(addressMode));
            asRGBA8_2D.data->set_filter_mode(cast(filterMode));
            asRGBA8_2D.data->set_color_space(cast(colorSpace));
            asRGBA8_2D.data->reset((const vector<4, unorm<8>>*)texels);
            asRGBA8_2D.accessor = texture_ref<vector<4, unorm<8>>, 2>(*asRGBA8_2D.data);
        }
        else if (texelFormat == OWL_TEXEL_FORMAT_RGBA32F)
        {
            asRGBA32F_2D.data = std::make_shared<texture<vector<4, float>, 2>>(size_x, size_y);
            asRGBA32F_2D.data->set_address_mode(cast(addressMode));
            asRGBA32F_2D.data->set_filter_mode(cast(filterMode));
            asRGBA32F_2D.data->set_color_space(cast(colorSpace));
            asRGBA32F_2D.data->reset((const vector<4, float>*)texels);
            asRGBA32F_2D.accessor = texture_ref<vector<4, float>, 2>(*asRGBA32F_2D.data);
        }
    }

    void Texture::reset(OWLTexelFormat         texelFormat,
                        uint32_t               size_x,
                        uint32_t               size_y,
                        uint32_t               size_z,
                        const void            *texels,
                        OWLTextureFilterMode   filterMode,
                        OWLTextureAddressMode  addressMode,
                        OWLTextureColorSpace   colorSpace,
                        uint32_t               linePitchInBytes) // TODO: mind this!!
    {
        if (texelFormat == OWL_TEXEL_FORMAT_R8)
        {
            asR8_3D.data = std::make_shared<texture<unorm<8>, 3>>(size_x, size_y, size_z);
            asR8_3D.data->set_address_mode(cast(addressMode));
            asR8_3D.data->set_filter_mode(cast(filterMode));
            asR8_3D.data->set_color_space(cast(colorSpace));
            asR8_3D.data->reset((const unorm<8>*)texels);
            asR8_3D.accessor = texture_ref<unorm<8>, 3>(*asR8_3D.data);
        }
        else if (texelFormat == OWL_TEXEL_FORMAT_R32F)
        {
            asR32F_3D.data = std::make_shared<texture<float, 3>>(size_x, size_y, size_z);
            asR32F_3D.data->set_address_mode(cast(addressMode));
            asR32F_3D.data->set_filter_mode(cast(filterMode));
            asR32F_3D.data->set_color_space(cast(colorSpace));
            asR32F_3D.data->reset((const float*)texels);
            asR32F_3D.accessor = texture_ref<float, 3>(*asR32F_3D.data);
        }
    }
} // visionaray
