
#pragma once

#include "owl.h"

/*! creates a buffer that is visible only on the host */
OWL_API OWLBuffer
owlBufferCreateEXT(OWLContext  context,
                   OWLDataType type,
                   size_t      count,
                   const void *init);

OWL_API OWLTexture
owlTexture3DCreateEXT(OWLContext context,
                      OWLTexelFormat texelFormat,
                      /*! number of texels in x dimension */
                      uint32_t size_x,
                      /*! number of texels in x dimension */
                      uint32_t size_y,
                      /*! number of texels in x dimension */
                      uint32_t size_z,
                      const void *texels,
                      OWLTextureFilterMode filterMode OWL_IF_CPP(=OWL_TEXTURE_LINEAR),
                      OWLTextureAddressMode addressMode OWL_IF_CPP(=OWL_TEXTURE_CLAMP),
                      OWLTextureColorSpace colorSpace OWL_IF_CPP(=OWL_COLOR_SPACE_LINEAR),
                       /*! number of bytes between one line of texels and
                         the next; '0' means 'size_x * sizeof(texel)' */
                       uint32_t linePitchInBytes       OWL_IF_CPP(=0)
                       );

OWL_API void owlTextureReleaseEXT(OWLTexture texture);



