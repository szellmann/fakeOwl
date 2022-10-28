#include <algorithm>
#include <cstddef>

#include <owl/owl_host.h>

#include "Buffer.h"
#include "Context.h"
#include "DataType.h"
#include "Geom.h"
#include "GeomGroup.h"
#include "GeomType.h"
#include "InstanceGroup.h"
#include "Logging.h"
#include "Module.h"
#include "Params.h"
#include "Texture.h"

namespace fake
{
    Context::Context(int* deviceIDs, int numDevices)
    {
    }

    Context::~Context()
    {
        FAKE_LOG_DBG << "Releasing context";

        // Destroy all resources bound to this context
        for (size_t i = 0; i < resourceHandles.size(); ++i)
        {
            ManagedResource resource = GetManagedResource(resourceHandles[i]);
            FAKE_LOG_DBG << "Releasing resource handle " << i << " from context";
            delete resource;
            UnregisterManagedResource(resourceHandles[i]);
        }
    }

    void Context::buildPrograms()
    {
        for (size_t i = 0; i < resourceHandles.size(); ++i)
        {
            ContextResource* cr = (ContextResource*)GetManagedResource(resourceHandles[i]);
            if (Module* module = dynamic_cast<Module*>(cr))
            {
                FAKE_LOG_DBG << "Building programs for module " << i;
                module->buildPrograms();
            }
        }
    }

    void Context::setRayTypeCount(int cnt)
    {
        rayTypeCount = cnt;
    }

    int Context::getRayTypeCount() const
    {
        return rayTypeCount;
    }

    void Context::setMaxInstancingDepth(int mid)
    {
        maxInstancingDepth = mid;
    }

    int Context::getMaxInstancingDepth() const
    {
        return maxInstancingDepth;
    }

    void Context::optimizeNodeGraph()
    {
        FAKE_LOG_DBG << "Optimizing node graph";

        // Iterate over the leaves (geom groups), optimize bottom-up
        for (size_t i = 0; i < resourceHandles.size(); ++i)
        {
            ContextResource* cr = (ContextResource*)GetManagedResource(resourceHandles[i]);
            if (GeomGroup* gg = dynamic_cast<GeomGroup*>(cr))
            {
                for (size_t p = 0; p < gg->parents.size(); ++p)
                {
                    if (InstanceGroup* ig = dynamic_cast<InstanceGroup*>(gg->parents[p]))
                        ig->optimizeBottomUp();
                }
            }
        }
    }

    Module* Context::createModule(const char* lib)
    {
        Module* module = new Module(lib);
        module->resourceHandle = RegisterManagedResource((ManagedResource)module);
        module->setContext(this);
        resourceHandles.push_back(module->resourceHandle);
        return module;
    }

    Params* Context::createParams(std::size_t  sizeOfVarStruct,
                                  OWLVarDecl  *vars,
                                  int          numVars)
    {
        FAKE_LOG_DBG << "Creating params, sizeOfVarStruct: "
                      << sizeOfVarStruct<< ", numVars: " << numVars;
        Params* params = new Params(sizeOfVarStruct, vars, numVars);
        params->resourceHandle = RegisterManagedResource((ManagedResource)params);
        params->setContext(this);
        resourceHandles.push_back(params->resourceHandle);
        return params;
    }

    Texture* Context::createTexture(OWLTexelFormat         texelFormat,
                                    uint32_t               size_x,
                                    uint32_t               size_y,
                                    const void            *texels,
                                    OWLTextureFilterMode   filterMode,
                                    OWLTextureAddressMode  addressMode,
                                    OWLTextureColorSpace   colorSpace,
                                    uint32_t               linePitchInBytes)
    {
        FAKE_LOG_DBG << "Creating 2D texture. Size: " << size_x << " x " << size_y
                     << ", filter mode: " << (int)filterMode
                     << ", address mode: " << (int)addressMode
                     << ", color space: " << (int)colorSpace
                     << ", line pitch (bytes): " << linePitchInBytes;
        Texture* texture = new Texture(texelFormat, size_x, size_y, texels,
                                       filterMode, addressMode, colorSpace,
                                       linePitchInBytes);
        texture->resourceHandle = RegisterManagedResource((ManagedResource)texture);
        texture->setContext(this);
        resourceHandles.push_back(texture->resourceHandle);
        return texture;
    }

    Texture* Context::createTexture(OWLTexelFormat         texelFormat,
                                    uint32_t               size_x,
                                    uint32_t               size_y,
                                    uint32_t               size_z,
                                    const void            *texels,
                                    OWLTextureFilterMode   filterMode,
                                    OWLTextureAddressMode  addressMode,
                                    OWLTextureColorSpace   colorSpace,
                                    uint32_t               linePitchInBytes)
    {
        FAKE_LOG_DBG << "Creating 3D texture. Size: "
                     << size_x << " x " << size_y << " x " << size_z
                     << ", filter mode: " << (int)filterMode
                     << ", address mode: " << (int)addressMode
                     << ", color space: " << (int)colorSpace
                     << ", line pitch (bytes): " << linePitchInBytes;
        Texture* texture = new Texture(texelFormat, size_x, size_y, size_z, texels,
                                       filterMode, addressMode, colorSpace,
                                       linePitchInBytes);
        texture->resourceHandle = RegisterManagedResource((ManagedResource)texture);
        texture->setContext(this);
        resourceHandles.push_back(texture->resourceHandle);
        return texture;
    }

    GeomType* Context::createGeomType(OWLGeomKind  kind,
                                      std::size_t  sizeOfVarStruct,
                                      OWLVarDecl  *vars,
                                      int          numVars)
    {
        FAKE_LOG_DBG << "Creating geom type (kind: " << kind << "), sizeOfVarStruct: "
                      << sizeOfVarStruct<< ", numVars: " << numVars;
        GeomType* geomType = new GeomType(kind, sizeOfVarStruct, vars, numVars);
        geomType->resourceHandle = RegisterManagedResource((ManagedResource)geomType);
        geomType->setContext(this);
        resourceHandles.push_back(geomType->resourceHandle);
        return geomType;
    }

    Geom* Context::createGeom(OWLGeomType type)
    {
        FAKE_LOG_DBG << "Creating geom";
        GeomType* gt = (GeomType*)type;
        Geom* geom = gt->createGeom();
        geom->resourceHandle = RegisterManagedResource((ManagedResource)geom);
        geom->setContext(this);
        resourceHandles.push_back(geom->resourceHandle);
        return geom;
    }

    GeomGroup* Context::createUserGeomGroup(std::size_t numGeoms,
                                            OWLGeom*    arrayOfChildGeoms)
    {
        FAKE_LOG_DBG << "Creating user geometry group with " << numGeoms << " geoms";
        GeomGroup* gg = new GeomGroup(OWL_GEOMETRY_USER, numGeoms, arrayOfChildGeoms);
        gg->resourceHandle = RegisterManagedResource((ManagedResource)gg);
        gg->setContext(this);
        resourceHandles.push_back(gg->resourceHandle);
        return gg;
    }

    GeomGroup* Context::createTrianglesGeomGroup(std::size_t numGeoms,
                                                 OWLGeom*    initValues)
    {
        FAKE_LOG_DBG << "Creating triangles geometry group with " << numGeoms << " geoms";
        GeomGroup* gg = new GeomGroup(OWL_GEOMETRY_TRIANGLES, numGeoms, initValues);
        gg->resourceHandle = RegisterManagedResource((ManagedResource)gg);
        gg->setContext(this);
        resourceHandles.push_back(gg->resourceHandle);
        return gg;
    }

    InstanceGroup* Context::createInstanceGroup(std::size_t     numInstances,
                                                const OWLGroup *initGroups,
                                                const uint32_t *initInstanceIDs,
                                                const float    *initTransforms,
                                                OWLMatrixFormat matrixFormat)
    {
        FAKE_LOG_DBG << "Creating instance group with " << numInstances << " instances";
        InstanceGroup* ig = new InstanceGroup(numInstances,
                                              initGroups,
                                              initInstanceIDs,
                                              initTransforms,
                                              matrixFormat);
        ig->resourceHandle = RegisterManagedResource((ManagedResource)ig);
        ig->setContext(this);
        resourceHandles.push_back(ig->resourceHandle);
        return ig;
    }

    Buffer* Context::createBuffer(Buffer::Type bufType,
                                  OWLDataType  dataType,
                                  std::size_t  count,
                                  const void*  init)
    {
        Buffer* buffer = nullptr;

        DataTypeInfo info = GetDataTypeInfo(dataType);

        if (bufType == Buffer::BtHostBuffer)
        {
            FAKE_LOG_DBG << "Creating host buffer (data type "
                         << dataType << "), count: "
                         << count << ", size in bytes: "
                         << count * info.sizeInBytes;

            buffer = new HostBuffer(dataType, count, init);
        }

        if (buffer != nullptr)
        {
            buffer->resourceHandle = RegisterManagedResource((ManagedResource)buffer);
            buffer->setContext(this);
            resourceHandles.push_back(buffer->resourceHandle);
        }
        else
            FAKE_LOG(fake::logging::Level::Error) << "Unsupported or unknown buffer type";

        return buffer;
    }

    void Context::releaseResource(ManagedResource resource)
    {
        resourceHandles.erase(std::remove(resourceHandles.begin(),
                                          resourceHandles.end(),
                                          resource->resourceHandle),
                              resourceHandles.end());
        UnregisterManagedResource(resource->resourceHandle);
        delete resource;
    }

}
