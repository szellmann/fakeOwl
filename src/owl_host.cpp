#include <cassert>

#include <owl/owl_host.h>

#include "Buffer.h"
#include "ClosestHit.h"
#include "Context.h"
#include "Geom.h"
#include "GeomType.h"
#include "Group.h"
#include "InstanceGroup.h"
#include "Intersect.h"
#include "Logging.h"
#include "Miss.h"
#include "Module.h"
#include "Params.h"
#include "Program.h"
#include "RayGen.h"
#include "Resource.h"
#include "Texture.h"

using namespace fake;

OWL_API OWLContext
owlContextCreate(int32_t *requestedDeviceIDs,
                 int      numDevices)
{
    Context* ctx = new Context(requestedDeviceIDs, numDevices);
    ctx->resourceHandle = RegisterManagedResource((ManagedResource)ctx);
    return (OWLContext)ctx;
}

OWL_API void
owlContextDestroy(OWLContext context)
{
    FAKE_LOG_DBG << "Destroying context";

    Context* ctx = (Context*)context;
    UnregisterManagedResource(ctx->resourceHandle);
    delete ctx;
}

OWL_API void
owlContextSetRayTypeCount(OWLContext context,
                          size_t numRayTypes)
{
    Context* ctx = (Context*)context;
    ctx->setRayTypeCount((int)numRayTypes);
}

OWL_API void
owlSetMaxInstancingDepth(OWLContext context,
                         int32_t maxInstanceDepth)
{
    Context* ctx = (Context*)context;
    ctx->setMaxInstancingDepth(maxInstanceDepth);
}

OWL_API OWLModule
owlModuleCreate(OWLContext  context,
                const char *ptxCode)
{
    Context* ctx = (Context*)context;
    return (OWLModule)ctx->createModule(ptxCode);
}

OWL_API void
owlRayGenLaunch2D(OWLRayGen rayGen,
                  int       dims_x,
                  int       dims_y)
{
    RayGen* rg = (RayGen*)rayGen;
    rg->launch(dims_x, dims_y, nullptr);
}

OWL_API void
owlLaunch2D(OWLRayGen rayGen,
            int       dims_x,
            int       dims_y,
            OWLParams params)
{
    RayGen* rg = (RayGen*)rayGen;
    rg->launch(dims_x, dims_y, (Params*)params);
}

OWL_API OWLRayGen
owlRayGenCreate(OWLContext  context,
                OWLModule   module,
                const char *programName,
                size_t      sizeOfVarStruct,
                OWLVarDecl *vars,
                int         numVars)
{
    (void)context;
    Module* mdl = (Module*)module;
    return (OWLRayGen)mdl->createProgram(Program::PtRayGen, programName,
                                         sizeOfVarStruct, vars, numVars);
}

OWL_API OWLMissProg
owlMissProgCreate(OWLContext  context,
                  OWLModule   module,
                  const char *programName,
                  size_t      sizeOfVarStruct,
                  OWLVarDecl *vars,
                  int         numVars)
{
    (void) context;
    Module* mdl = (Module*)module;
    return (OWLMissProg)mdl->createProgram(Program::PtMiss, programName,
                                           sizeOfVarStruct, vars, numVars);
}

OWL_API void
owlMissProgSet(OWLContext  context,
               int rayType,
               OWLMissProg missProgToUse)
{
    (void) context;
    Miss* miss = (Miss*)missProgToUse;
    Module* mdl = miss->module;
    mdl->setMissProg(rayType, (Miss*)missProgToUse);
}

OWL_API void
owlModuleRelease(OWLModule module)
{
    Module* mdl = (Module*)module;
    Context* ctx = mdl->getContext();
    ctx->releaseResource(mdl);
    module = nullptr;
}

OWL_API void
owlRayGenRelease(OWLRayGen rayGen)
{
    RayGen* rg = (RayGen*)rayGen;
    Module* mdl = rg->module;
    assert(mdl);
    mdl->releaseProgram(rg);
    rg = nullptr;
}

OWL_API void
owlBuildPrograms(OWLContext context)
{
    Context* ctx = (Context*)context;
    ctx->buildPrograms();
}

OWL_API void
owlBuildPipeline(OWLContext context)
{
}

OWL_API void
owlBuildSBT(OWLContext       context,
            OWLBuildSBTFlags flags)
{
    // We take this opportunity to optimize a bit..
    Context* ctx = (Context*)context;
    ctx->optimizeNodeGraph();
}

OWL_API void 
owlBufferResize(OWLBuffer buffer,
                size_t    newItemCount)
{
    Buffer* buf = (Buffer*)buffer;
    buf->resize(newItemCount);
}

OWL_API void
owlBufferDestroy(OWLBuffer buffer)
{
    // Buffer* buf = (Buffer*)buffer;
}

OWL_API void 
owlBufferUpload(OWLBuffer buffer,
                const void *hostPtr,
                size_t offset,
                size_t numBytes)
{
    Buffer* buf = (Buffer*)buffer;
    buf->upload(hostPtr,offset,numBytes);
}

OWL_API const void *
owlBufferGetPointer(OWLBuffer buffer, int deviceID)
{
    (void)deviceID;
    Buffer* buf = (Buffer*)buffer;
    return buf->getPointer();
}

OWL_API OWLGeomType
owlGeomTypeCreate(OWLContext context,
                  OWLGeomKind kind,
                  size_t sizeOfVarStruct,
                  OWLVarDecl *vars,
                  int         numVars)
{
    Context* ctx = (Context*)context;
    return (OWLGeomType)ctx->createGeomType(kind, sizeOfVarStruct,
                                            vars, numVars);
}

OWL_API OWLTexture
owlTexture2DCreate(OWLContext context,
                   OWLTexelFormat texelFormat,
                   uint32_t size_x,
                   uint32_t size_y,
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
                                          texels,
                                          filterMode,
                                          addressMode,
                                          colorSpace,
                                          linePitchInBytes);
}

OWL_API CUtexObject
owlTextureGetObject(OWLTexture texture,
                    int deviceID)
{
    Texture* tex = (Texture*)texture;
    return tex->textureHandle;
}

OWL_API void
owlGeomTypeSetClosestHit(OWLGeomType type,
                         int rayType,
                         OWLModule module,
                         const char *progName)
{
    Module* mdl = (Module*)module;
    GeomType* gt = (GeomType*)type;
    gt->setClosestHitProg((ClosestHit*)mdl->createProgram(Program::PtClosestHit, progName), rayType);
}

OWL_API void
owlGeomTypeSetAnyHit(OWLGeomType type,
                     int rayType,
                     OWLModule module,
                     const char *progName)
{
    // Module* mdl = (Module*)module;
    // GeomType* gt = (GeomType*)type;
    // gt->anyHit = mdl->createProgram(Program::PtAnyHit, progName);
}

OWL_API void
owlGeomTypeSetIntersectProg(OWLGeomType type,
                            int rayType,
                            OWLModule module,
                            const char *progName)
{
    Module* mdl = (Module*)module;
    GeomType* gt = (GeomType*)type;
    gt->setIntersectProg((Intersect*)mdl->createProgram(Program::PtIntersect, progName), rayType);
}

OWL_API void
owlGeomTypeSetBoundsProg(OWLGeomType type,
                         OWLModule module,
                         const char *progName)
{
    Module* mdl = (Module*)module;
    GeomType* gt = (GeomType*)type;
    gt->setBoundsProg((Bounds*)mdl->createProgram(Program::PtBounds, progName));
}

OWL_API OWLParams
owlParamsCreate(OWLContext  context,
                size_t      sizeOfVarStruct,
                OWLVarDecl *vars,
                int         numVars)
{
    Context* ctx = (Context*)context;
    return (OWLParams)ctx->createParams(sizeOfVarStruct,
                                        vars, numVars);
}

OWL_API OWLGeom
owlGeomCreate(OWLContext  context,
              OWLGeomType type)
{
    Context* ctx = (Context*)context;
    return (OWLGeom)ctx->createGeom(type);
}

OWL_API void
owlGeomSetPrimCount(OWLGeom obj,
                    size_t  primCount)
{
    Geom* geom = (Geom*)obj;
    geom->setPrimCount(primCount);
}

OWL_API OWLGroup
owlUserGeomGroupCreate(OWLContext context,
                       size_t     numGeometries,
                       OWLGeom   *arrayOfChildGeoms)
{
    Context* ctx = (Context*)context;
    return (OWLGroup)ctx->createUserGeomGroup(numGeometries, arrayOfChildGeoms);
}

OWL_API OWLGroup
owlTrianglesGeomGroupCreate(OWLContext context,
                            size_t     numGeometries,
                            OWLGeom   *initValues)
{
    Context* ctx = (Context*)context;
    return (OWLGroup)ctx->createTrianglesGeomGroup(numGeometries, initValues);
}

OWL_API OWLGroup
owlInstanceGroupCreate(OWLContext context,
                       size_t     numInstances,
                       const OWLGroup *initGroups,
                       const uint32_t *initInstanceIDs,
                       const float    *initTransforms,
                       OWLMatrixFormat matrixFormat)
{
    Context* ctx = (Context*)context;
    return (OWLGroup)ctx->createInstanceGroup(numInstances,
                                              initGroups,
                                              initInstanceIDs,
                                              initTransforms,
                                              matrixFormat);
}

OWL_API void owlTrianglesSetVertices(OWLGeom triangles,
                                     OWLBuffer vertices,
                                     size_t count,
                                     size_t stride,
                                     size_t offset)
{
    TrianglesGeom* geom = (TrianglesGeom*)triangles;
    Buffer* buf = (Buffer*)vertices;
    geom->setVertices(buf, count, stride, offset);
}

OWL_API void owlTrianglesSetIndices(OWLGeom triangles,
                                    OWLBuffer indices,
                                    size_t count,
                                    size_t stride,
                                    size_t offset)
{
    TrianglesGeom* geom = (TrianglesGeom*)triangles;
    Buffer* buf = (Buffer*)indices;
    geom->setIndices(buf, count, stride, offset);
}

OWL_API void
owlInstanceGroupSetChild(OWLGroup group,
                         int whichChild,
                         OWLGroup child)
{
    InstanceGroup* ig = (InstanceGroup*)group;
    ig->setChild(whichChild, (Group*)child);
}

OWL_API void
owlInstanceGroupSetTransform(OWLGroup group,
                             int whichChild,
                             const float *floats,
                             OWLMatrixFormat matrixFormat)
{
    InstanceGroup* ig = (InstanceGroup*)group;
    ig->setTransform(whichChild, floats, matrixFormat);
}

OWL_API void owlGroupBuildAccel(OWLGroup group)
{
    Group* grp = (Group*)group;
    grp->buildAccel();
}

OWL_API void owlGroupRefitAccel(OWLGroup group)
{
    Group* grp = (Group*)group;
    grp->refitAccel();
}

OWL_API void
owlRayGenSet1i(OWLRayGen   obj,
               const char *name,
               int         val)
{
    RayGen* rayGen = (RayGen*)obj;
    rayGen->set(name, val);
}

OWL_API void
owlRayGenSet2i(OWLRayGen   obj,
               const char *name,
               int         x,
               int         y)
{
    RayGen* rayGen = (RayGen*)obj;
    rayGen->set(name, x, y);
}

OWL_API void
owlRayGenSet3i(OWLRayGen   obj,
               const char *name,
               int         x,
               int         y,
               int         z)
{
    RayGen* rayGen = (RayGen*)obj;
    rayGen->set(name, x, y, z);
}

OWL_API void
owlRayGenSet4i(OWLRayGen   obj,
               const char *name,
               int         x,
               int         y,
               int         z,
               int         w)
{
    RayGen* rayGen = (RayGen*)obj;
    rayGen->set(name, x, y, z, w);
}

OWL_API void
owlRayGenSet1ul(OWLRayGen   obj,
                const char* name,
                uint64_t    val)
{
    RayGen* rayGen = (RayGen*)obj;
    rayGen->set(name, val);
}

OWL_API void
owlRayGenSet3f(OWLRayGen obj,
             const char *name,
             float       x,
             float       y,
             float       z)
{
    RayGen* rayGen = (RayGen*)obj;
    rayGen->set(name, x, y, z);
}

OWL_API void owlRayGenSetGroup(OWLRayGen   obj,
                               const char *name,
                               OWLGroup    val)
{
    RayGen* rayGen = (RayGen*)obj;
    rayGen->set(name, (Group*)val);
}

OWL_API void
owlRayGenSetBuffer(OWLRayGen     obj,
                   const char   *name,
                   OWLBuffer     val)
{
    RayGen* rayGen = (RayGen*)obj;
    rayGen->set(name, (Buffer*)val);
}

OWL_API void
owlMissProgSet3f(OWLMissProg obj,
                 const char *name,
                 float       x,
                 float       y,
                 float       z)
{
    Miss* miss = (Miss*)obj;
    miss->set(name, x, y, z);
}

OWL_API void
owlGeomSet1i(OWLGeom     obj,
             const char *name,
             int32_t     val)
{
    Geom* geom = (Geom*)obj;
    geom->set(name, val);
}

OWL_API void
owlGeomSet1ui(OWLGeom     obj,
              const char *name,
              uint32_t    val)
{
    Geom* geom = (Geom*)obj;
    geom->set(name, val);
}

OWL_API void
owlGeomSet1f(OWLGeom     obj,
             const char *name,
             float       val)
{
    Geom* geom = (Geom*)obj;
    geom->set(name, val);
}

OWL_API void
owlGeomSet2f(OWLGeom     obj,
             const char *name,
             float       x,
             float       y)
{
    Geom* geom = (Geom*)obj;
    geom->set(name, x, y);
}

OWL_API void
owlGeomSet3f(OWLGeom     obj,
             const char *name,
             float       x,
             float       y,
             float       z)
{
    Geom* geom = (Geom*)obj;
    geom->set(name, x, y, z);
}

OWL_API void owlGeomSetTexture(OWLGeom     obj,
                               const char *name,
                               OWLTexture  val)
{
    Geom* geom = (Geom*)obj;
    geom->set(name, (Texture*)val);
}

OWL_API void owlGeomSetGroup(OWLGeom obj,
                             const char *name,
                             OWLGroup val)
{
    Geom* geom = (Geom*)obj;
    geom->set(name, (Group*)val);
}

OWL_API void
owlGeomSetBuffer(OWLGeom     obj,
                 const char *name,
                 OWLBuffer   val)
{
    Geom* geom = (Geom*)obj;
    geom->set(name, (Buffer*)val);
}

OWL_API void
owlGeomSetRaw(OWLGeom    obj,
              const char  *name,
              const void *val)
{
    Geom* geom = (Geom*)obj;
    geom->set(name, val);
}

OWL_API void
owlGeomSetPointer(OWLGeom    obj,
                  const char  *name,
                  const void *val)
{
    Geom* geom = (Geom*)obj;
    geom->set(name, val);
}

OWL_API void
owlParamsSet1b(OWLParams   obj,
               const char *name,
               bool        val)
{
    Params* params = (Params*)obj;
    params->set(name, val);
}

OWL_API void
owlParamsSet1i(OWLParams  obj,
              const char *name,
              int         val)
{
    Params* params = (Params*)obj;
    params->set(name, val);
}

OWL_API void
owlParamsSet2i(OWLParams   obj,
               const char *name,
               int         x,
               int         y)
{
    Params* params = (Params*)obj;
    params->set(name, x, y);
}

OWL_API void
owlParamsSet3i(OWLParams   obj,
               const char *name,
               int         x,
               int         y,
               int         z)
{
    Params* params = (Params*)obj;
    params->set(name, x, y, z);
}

OWL_API void
owlParamsSet4i(OWLParams   obj,
               const char *name,
               int         x,
               int         y,
               int         z,
               int         w)
{
    Params* params = (Params*)obj;
    params->set(name, x, y, z, w);
}

OWL_API void owlParamsSet2iv(OWLParams   obj,
                             const char *name,
                             const int  *val)
{
    Params* params = (Params*)obj;
    params->set(name, val[0], val[1]);
}

OWL_API void owlParamsSet3iv(OWLParams   obj,
                             const char *name,
                             const int  *val)
{
    Params* params = (Params*)obj;
    params->set(name, val[0], val[1], val[2]);
}

OWL_API void owlParamsSet4iv(OWLParams   obj,
                             const char *name,
                             const int  *val)
{
    Params* params = (Params*)obj;
    params->set(name, val[0], val[1], val[2], val[3]);
}

OWL_API void
owlParamsSet1ui(OWLParams   obj,
                const char *name,
                unsigned    val)
{
    Params* params = (Params*)obj;
    params->set(name, val);
}

OWL_API void
owlParamsSet2ui(OWLParams   obj,
                const char *name,
                unsigned    x,
                unsigned    y)
{
    Params* params = (Params*)obj;
    params->set(name, x, y);
}

OWL_API void
owlParamsSet3ui(OWLParams   obj,
                const char *name,
                unsigned    x,
                unsigned    y,
                unsigned    z)
{
    Params* params = (Params*)obj;
    params->set(name, x, y, z);
}

OWL_API void
owlParamsSet4ui(OWLParams   obj,
                const char *name,
                unsigned    x,
                unsigned    y,
                unsigned    z,
                unsigned    w)
{
    Params* params = (Params*)obj;
    params->set(name, x, y, z, w);
}

OWL_API void owlParamsSet2uiv(OWLParams       obj,
                              const char     *name,
                              const unsigned *val)
{
    Params* params = (Params*)obj;
    params->set(name, val[0], val[1]);
}

OWL_API void owlParamsSet3uiv(OWLParams       obj,
                              const char     *name,
                              const unsigned *val)
{
    Params* params = (Params*)obj;
    params->set(name, val[0], val[1], val[2]);
}

OWL_API void owlParamsSet4uiv(OWLParams       obj,
                              const char     *name,
                              const unsigned *val)
{
    Params* params = (Params*)obj;
    params->set(name, val[0], val[1], val[2], val[3]);
}

OWL_API void
owlParamsSet1ul(OWLParams   obj,
                const char *name,
                uint64_t    val)
{
    Params* params = (Params*)obj;
    params->set(name, val);
}

OWL_API void
owlParamsSet2ul(OWLParams   obj,
                const char *name,
                uint64_t    x,
                uint64_t    y)
{
    Params* params = (Params*)obj;
    params->set(name, x, y);
}

OWL_API void
owlParamsSet3ul(OWLParams   obj,
                const char *name,
                uint64_t    x,
                uint64_t    y,
                uint64_t    z)
{
    Params* params = (Params*)obj;
    params->set(name, x, y, z);
}

OWL_API void
owlParamsSet4ul(OWLParams   obj,
                const char *name,
                uint64_t    x,
                uint64_t    y,
                uint64_t    z,
                uint64_t    w)
{
    Params* params = (Params*)obj;
    params->set(name, x, y, z, w);
}

OWL_API void owlParamsSet2ulv(OWLParams       obj,
                              const char     *name,
                              const uint64_t *val)
{
    Params* params = (Params*)obj;
    params->set(name, val[0], val[1]);
}

OWL_API void owlParamsSet3ulv(OWLParams       obj,
                              const char     *name,
                              const uint64_t *val)
{
    Params* params = (Params*)obj;
    params->set(name, val[0], val[1], val[2]);
}

OWL_API void owlParamsSet4ulv(OWLParams       obj,
                              const char     *name,
                              const uint64_t *val)
{
    Params* params = (Params*)obj;
    params->set(name, val[0], val[1], val[2], val[3]);
}

OWL_API void
owlParamsSet1f(OWLParams   obj,
               const char *name,
               float       val)
{
    Params* params = (Params*)obj;
    params->set(name, val);
}

OWL_API void
owlParamsSet2f(OWLParams   obj,
               const char *name,
               float       x,
               float       y)
{
    Params* params = (Params*)obj;
    params->set(name, x, y);
}

OWL_API void
owlParamsSet3f(OWLParams   obj,
               const char *name,
               float       x,
               float       y,
               float       z)
{
    Params* params = (Params*)obj;
    params->set(name, x, y, z);
}

OWL_API void
owlParamsSet4f(OWLParams   obj,
               const char *name,
               float       x,
               float       y,
               float       z,
               float       w)
{
    Params* params = (Params*)obj;
    params->set(name, x, y, z, w);
}

OWL_API void
owlParamsSet2fv(OWLParams    obj,
                const char  *name,
                const float *val)
{
    Params* params = (Params*)obj;
    params->set(name, val[0], val[1]);
}

OWL_API void
owlParamsSet3fv(OWLParams    obj,
                const char  *name,
                const float *val)
{
    Params* params = (Params*)obj;
    params->set(name, val[0], val[1], val[2]);
}

OWL_API void
owlParamsSet4fv(OWLParams    obj,
                const char  *name,
                const float *val)
{
    Params* params = (Params*)obj;
    params->set(name, val[0], val[1], val[2], val[3]);
}

OWL_API void
owlParamsSetTexture(OWLParams   obj,
                    const char *name,
                    OWLTexture  val)
{
    Params* params = (Params*)obj;
    params->set(name, (Texture*)val);
}

OWL_API void
owlParamsSetBuffer(OWLParams   obj,
                   const char *name,
                   OWLBuffer   val)
{
    Params* params = (Params*)obj;
    params->set(name, (Buffer*)val);
}

OWL_API void owlParamsSetGroup(OWLParams   obj,
                               const char *name,
                               OWLGroup    val)
{
    Params* params = (Params*)obj;
    params->set(name, (Group*)val);
}

OWL_API void
owlParamsSetPointer(OWLParams    obj,
                    const char  *name,
                    const void *val)
{
    Params* params = (Params*)obj;
    params->set(name, val);
}




