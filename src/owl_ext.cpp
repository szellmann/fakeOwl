
#include <owl/owl_ext.h>

#include "Context.h"
#include "RayGen.h"
#include "Texture.h"

using namespace fake;

OWL_API OWLBuffer
owlBufferCreateEXT(OWLContext  context,
                   OWLDataType type,
                   size_t      count,
                   const void *init)
{
    Context* ctx = (Context*)context;
    return (OWLBuffer)ctx->createBuffer(Buffer::BtHostBuffer, type, count, init);
}

OWL_API OWLTexture
owlTexture3DCreateEXT(OWLContext context,
                      OWLTexelFormat texelFormat,
                      uint32_t size_x,
                      uint32_t size_y,
                      uint32_t size_z,
                      const void *texels,
                      OWLTextureFilterMode filterMode,
                      OWLTextureAddressMode addressMode,
                      OWLTextureColorSpace colorSpace,
                      uint32_t linePitchInBytes)
{
    Context* ctx = (Context*)context;
    return (OWLTexture)ctx->createTexture(texelFormat,
                                          size_x,
                                          size_y,
                                          size_z,
                                          texels,
                                          filterMode,
                                          addressMode,
                                          colorSpace,
                                          linePitchInBytes);
}

OWL_API void
owlTextureReleaseEXT(OWLTexture texture)
{
    Texture* tex = (Texture*)texture;
    Context* ctx = tex->getContext();
    ctx->releaseResource(tex);
    texture = nullptr;
}

OWL_API void
owlLaunch1DEXT(OWLRayGen rayGen, int dims,
               OWLParams params)
{
    RayGen* rg = (RayGen*)rayGen;
    rg->launch(dims, (Params*)params);
}

