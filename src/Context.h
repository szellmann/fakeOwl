
#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

#include <owl/owl_host.h>

#include <visionaray/detail/thread_pool.h>

#include "Buffer.h"
#include "Resource.h"

namespace fake
{
    class Geom;
    class GeomGroup;
    class GeomType;
    class InstanceGroup;
    class Module;
    class Params;
    class Texture;

    class Context : public ManagedResource_
    {
    public:
        Context(int* deviceIDs, int numDevices);
       ~Context();

        void buildPrograms();

        visionaray::thread_pool& getThreadPool();

        void setRayTypeCount(int cnt);

        int getRayTypeCount() const;

        void setMaxInstancingDepth(int mid);

        int getMaxInstancingDepth() const;

        void optimizeNodeGraph();

        Module* createModule(const char* lib);

        Params* createParams(std::size_t  sizeOfVarStruct,
                             OWLVarDecl  *vars,
                             int          numVars);

        GeomType* createGeomType(OWLGeomKind  kind,
                                 std::size_t  sizeOfVarStruct,
                                 OWLVarDecl  *vars,
                                 int          numVars);

        Texture* createTexture(OWLTexelFormat         texelFormat,
                               uint32_t               size_x,
                               uint32_t               size_y,
                               const void            *texels,
                               OWLTextureFilterMode   filterMode,
                               OWLTextureAddressMode  addressMode,
                               OWLTextureColorSpace   colorSpace,
                               uint32_t               linePitchInBytes);

        Texture* createTexture(OWLTexelFormat         texelFormat,
                               uint32_t               size_x,
                               uint32_t               size_y,
                               uint32_t               size_z,
                               const void            *texels,
                               OWLTextureFilterMode   filterMode,
                               OWLTextureAddressMode  addressMode,
                               OWLTextureColorSpace   colorSpace,
                               uint32_t               linePitchInBytes);

        Geom* createGeom(OWLGeomType geom);

        GeomGroup* createUserGeomGroup(std::size_t numGeoms,
                                       OWLGeom*    arrayOfChildGeoms);

        GeomGroup* createTrianglesGeomGroup(std::size_t numGeoms,
                                            OWLGeom*    initValues);

        InstanceGroup* createInstanceGroup(std::size_t     numInstances,
                                           const OWLGroup *initGroups,
                                           const uint32_t *initInstanceIDs,
                                           const float    *initTransforms,
                                           OWLMatrixFormat matrixFormat);

        Buffer* createBuffer(Buffer::Type bufType,
                             OWLDataType  dataType,
                             std::size_t  count,
                             const void*  init);

        void releaseResource(ManagedResource resource);

    private:
        std::vector<ResourceHandle> resourceHandles;

        visionaray::thread_pool threadPool;

        int rayTypeCount = 1;

        int maxInstancingDepth = 1;

    };
} // fake
