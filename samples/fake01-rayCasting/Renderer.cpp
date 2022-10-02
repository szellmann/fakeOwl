// ======================================================================== //
// Copyright 2020-2020 Ingo Wald                                            //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#include <fstream>
#include "mesh.h"
#include "Renderer.h"
#include "deviceCode.h"
#include <owl/owl_ext.h>
#include <random>

namespace dvr {
  extern "C" char embedded_deviceCode[];

  bool  Renderer::heatMapEnabled = false;
  float Renderer::heatMapScale = 1e-5f;
  int   Renderer::spp = 1;
  
  OWLVarDecl rayGenVars[]
  = {
     { nullptr /* sentinel to mark end of list */ }
  };

  OWLVarDecl triangleGeomVars[]
  = {
     { "indexBuffer",  OWL_BUFPTR, OWL_OFFSETOF(TriangleGeom,indexBuffer)},
     { "vertexBuffer", OWL_BUFPTR, OWL_OFFSETOF(TriangleGeom,vertexBuffer)},
     { "slopes",  OWL_BUFPTR, OWL_OFFSETOF(TriangleGeom,slopes)},
     { nullptr /* sentinel to mark end of list */ }
  };

  OWLVarDecl launchParamsVars[]
  = {
     { "fbPointer",   OWL_RAW_POINTER, OWL_OFFSETOF(LaunchParams,fbPointer) },
     { "accumBuffer",   OWL_BUFPTR, OWL_OFFSETOF(LaunchParams,accumBuffer) },
     { "accumID",   OWL_INT, OWL_OFFSETOF(LaunchParams,accumID) },
#ifdef DUMP_FRAMES
    // to allow dumping rgba and depth for some unrelated compositing work....
     { "fbDepth",     OWL_BUFPTR,      OWL_OFFSETOF(LaunchParams,fbDepth) },
#endif
     { "world",    OWL_GROUP,  OWL_OFFSETOF(LaunchParams,world)},
     { "domain.lower", OWL_FLOAT3, OWL_OFFSETOF(LaunchParams,domain.lower) },
     { "domain.upper", OWL_FLOAT3, OWL_OFFSETOF(LaunchParams,domain.upper) },
     { "numParticles",    OWL_UINT,   OWL_OFFSETOF(LaunchParams,numParticles) },
     { "radius",    OWL_FLOAT,   OWL_OFFSETOF(LaunchParams,radius) },
     // render settings
     { "render.dt",            OWL_FLOAT, OWL_OFFSETOF(LaunchParams,render.dt) },
     { "render.spp",           OWL_INT,   OWL_OFFSETOF(LaunchParams,render.spp) },
     { "render.heatMapEnabled", OWL_INT, OWL_OFFSETOF(LaunchParams,render.heatMapEnabled) },
     { "render.heatMapScale", OWL_FLOAT, OWL_OFFSETOF(LaunchParams,render.heatMapScale) },
     // camera settings
     { "camera.org",    OWL_FLOAT3, OWL_OFFSETOF(LaunchParams,camera.org) },
     { "camera.dir_00", OWL_FLOAT3, OWL_OFFSETOF(LaunchParams,camera.dir_00) },
     { "camera.dir_du", OWL_FLOAT3, OWL_OFFSETOF(LaunchParams,camera.dir_du) },
     { "camera.dir_dv", OWL_FLOAT3, OWL_OFFSETOF(LaunchParams,camera.dir_dv) },
     // Model, if in rendering mode
     { "model.group", OWL_GROUP,  OWL_OFFSETOF(LaunchParams,model.group)},
     { "model.indexBuffer",  OWL_BUFPTR, OWL_OFFSETOF(LaunchParams,model.indexBuffer)},
     { "model.vertexBuffer", OWL_BUFPTR, OWL_OFFSETOF(LaunchParams,model.vertexBuffer)},
     { nullptr /* sentinel to mark end of list */ }
  };
  
  Renderer::Renderer()
    : xfDomain({0.f,1.f})
  {
#if 1
    std::default_random_engine rd;
    std::uniform_real_distribution<float> pos(-1.0f, 1.0f);

    float radius = .312f;
    particles.resize(50);
    box3f domain;
    for (int i=0; i<particles.size(); ++i) {
        float x=pos(rd);
        float y=pos(rd);
        float z=pos(rd);
        particles[i] = {x,y,z};
        domain.extend(particles[i]-vec3f(radius));
        domain.extend(particles[i]+vec3f(radius));
    }
#else
    std::ifstream in("/Users/stefan/vowl/data/atm_2019_07_01_07_00.tab.out");
    float radius = 10000.312f;
    uint64_t size;
    in.read((char*)&size,sizeof(size));
    particles.resize(size);
    std::vector<vec4f> temp(particles.size());
    in.read((char*)temp.data(),temp.size()*sizeof(vec4f));
    box3f domain;
    for (int i=0; i<particles.size(); ++i) {
        particles[i] = vec3f(temp[i]);std::cout << particles[i] << '\n';
        domain.extend(particles[i]-vec3f(radius));
        domain.extend(particles[i]+vec3f(radius));
    }
#endif

    std::cout << domain << '\n';
    std::cout << particles.size() << '\n';

#if 0
      modelBounds = { vec3f(1e30f), vec3f(-1e30f) };

      using namespace cdf;
      Mesh::SP objMesh;
      try {
        objMesh = Mesh::load("/Users/stefan/obj/dragon.obj");
        // Construct bounds
        for (std::size_t i=0; i<objMesh->geoms.size(); ++i)
        {
          const Geometry::SP &geom = objMesh->geoms[i];
          for (const auto &v : geom->vertex) {
            modelBounds.extend(v);
          }
        }
        // Push onto geometry stack
        for (std::size_t i=0; i<objMesh->geoms.size(); ++i)
        {
          Geometry::SP &geom = objMesh->geoms[i];
          //if (i==0) {
          if (0) {
            // Add a ground plane to the first mesh
            vec3f v1(modelBounds.lower.x*10.f,modelBounds.lower.y,modelBounds.lower.z*10.f);
            vec3f v2(modelBounds.upper.x*10.f,modelBounds.lower.y,modelBounds.lower.z*10.f);
            vec3f v3(modelBounds.upper.x*10.f,modelBounds.lower.y,modelBounds.upper.z*10.f);
            vec3f v4(modelBounds.lower.x*10.f,modelBounds.lower.y,modelBounds.upper.z*10.f);
            vec3i idx1 = vec3i(0,1,2)+vec3i((int)geom->vertex.size());
            vec3i idx2 = vec3i(0,2,3)+vec3i((int)geom->vertex.size());
            geom->vertex.push_back(v1);
            geom->vertex.push_back(v2);
            geom->vertex.push_back(v3);
            geom->vertex.push_back(v4);
            geom->index.push_back(idx1);
            geom->index.push_back(idx2);
          }
        }
      } catch (...) { std::cerr << "Cannot load..\n"; }
#else
    modelBounds.extend(domain);
#endif

    owl = owlContextCreate(nullptr,1);
    module = owlModuleCreate(owl,embedded_deviceCode);
    rayGen = owlRayGenCreate(owl,module,"renderFrame",
                             sizeof(RayGen),rayGenVars,-1);
    lp = owlParamsCreate(owl,sizeof(LaunchParams),launchParamsVars,-1);

    // owlParamsSet3i(lp,"volume.dims",
    //                512,
    //                512,
    //                512);
    // owlParamsSet3f(lp,"render.gradientDelta",
    //                1.f/512,
    //                1.f/512,
    //                1.f/512);

#ifdef DUMP_FRAMES
    fbDepth = owlManagedMemoryBufferCreate(owl,OWL_FLOAT,1,nullptr);
    fbSize  = vec2i(1);
    owlParamsSetBuffer(lp,"fbDepth",fbDepth);
#endif

    particlesBuf = owlBufferCreateEXT(owl,
                                      OWL_USER_TYPE(Particle),
                                      0, nullptr);

    OWLVarDecl particleGeomVars[] = {
        { "world",        OWL_GROUP,  OWL_OFFSETOF(ParticleGeom,world)},
        { "domain.lower", OWL_FLOAT3, OWL_OFFSETOF(ParticleGeom,domain.lower) },
        { "domain.upper", OWL_FLOAT3, OWL_OFFSETOF(ParticleGeom,domain.upper) },
        { "particles",    OWL_BUFPTR, OWL_OFFSETOF(ParticleGeom,particles)},
        { "numParticles", OWL_UINT,   OWL_OFFSETOF(ParticleGeom,numParticles)},
        { "radius",       OWL_FLOAT,  OWL_OFFSETOF(ParticleGeom,radius)},
        { /* sentinel to mark end of list */ }
    };

#if 0

    triangleGeomType = owlGeomTypeCreate(owl,
                                         OWL_TRIANGLES,
                                         sizeof(TriangleGeom),
                                         triangleGeomVars, -1);
    owlGeomTypeSetClosestHit(triangleGeomType, 0, module, "ModelCH");

    for (std::size_t i = 0; i < 1; ++i) {//objMesh->geoms.size(); ++i) {
      const Geometry::SP &geom = objMesh->geoms[i];
      OWLBuffer indexBuffer = owlBufferCreateEXT(owl, OWL_INT3, geom->index.size(), geom->index.data());
      OWLBuffer vertexBuffer = owlBufferCreateEXT(owl, OWL_FLOAT3, geom->vertex.size(), geom->vertex.data());
      OWLGeom ogeom;
      ogeom = owlGeomCreate(owl, triangleGeomType);
      owlTrianglesSetIndices(ogeom, indexBuffer, geom->index.size(), sizeof(vec3i), 0);
      owlTrianglesSetVertices(ogeom, vertexBuffer, geom->vertex.size(), sizeof(vec3f), 0);

      modelGroup = owlTrianglesGeomGroupCreate(owl, 1, &ogeom);
      owlGroupBuildAccel(modelGroup);
      // owlParamsSetBuffer(lp,"model.vertexBuffer",vertexBuffer);
      // owlParamsSetBuffer(lp,"model.indexBuffer",indexBuffer);
      owlGeomSetBuffer(ogeom,"vertexBuffer",vertexBuffer);
      owlGeomSetBuffer(ogeom,"indexBuffer",indexBuffer);
      // owlInstanceGroupSetChild(modelGroup, modelIndex, gi);
      owlParamsSetGroup(lp,"model.group",modelGroup);
    }
#endif
    geomType = owlGeomTypeCreate(owl,
                                 OWL_GEOMETRY_USER,
                                 sizeof(ParticleGeom),
                                 particleGeomVars,-1);

    owlGeomTypeSetBoundsProg(geomType, module, "Particles");
    owlGeomTypeSetIntersectProg(geomType, 0, module, "Particles");
    owlGeomTypeSetClosestHit(geomType, 0, module, "Particles");

    geom = owlGeomCreate(owl, geomType);

    blasGroup = owlUserGeomGroupCreate(owl, 1, &geom);
#if 0
    tlasGroup = owlInstanceGroupCreate(owl, 1);
    owlInstanceGroupSetChild(tlasGroup, 0, modelGroup);
#else
    tlasGroup = owlInstanceGroupCreate(owl, 1);
    owlInstanceGroupSetChild(tlasGroup, 0, blasGroup);
#endif

    // TODO: restructure; just for testing
    owlBufferResize(particlesBuf, particles.size());
    owlBufferUpload(particlesBuf, particles.data());

    owlParamsSetGroup(lp, "world", tlasGroup);
    owlParamsSetBuffer(lp, "particles", particlesBuf);

    owlGeomSetGroup(geom, "world", tlasGroup);
    owlGeomSetBuffer(geom, "particles", particlesBuf);

    owlBuildPrograms(owl);
    owlBuildPipeline(owl);
    owlBuildSBT(owl);


    owlParamsSet3f(lp,"domain.lower",
                   domain.lower.x,
                   domain.lower.y,
                   domain.lower.z);
    owlParamsSet3f(lp,"domain.upper",
                   domain.upper.x,
                   domain.upper.y,
                   domain.upper.z);
    owlParamsSet1ui(lp, "numParticles", particles.size());
    owlParamsSet1f(lp, "radius", radius);

    owlGeomSetPrimCount(geom, particles.size());

    owlGeomSet3f(geom,"domain.lower",
                 domain.lower.x,
                 domain.lower.y,
                 domain.lower.z);
    owlGeomSet3f(geom,"domain.upper",
                 domain.upper.x,
                 domain.upper.y,
                 domain.upper.z);
    owlGeomSet1ui(geom, "numParticles", particles.size());
    owlGeomSet1f(geom, "radius", radius);


    // Rebuild (also restructure.. TODO)
    {
        owlGroupBuildAccel(blasGroup);
        owlGroupBuildAccel(tlasGroup);
        owlBuildSBT(owl);
    }
  }

  void Renderer::set_dt(float dt)
  {
    owlParamsSet1f(lp,"render.dt",dt);
  }
  
  void Renderer::setCamera(const vec3f &org,
                           const vec3f &dir_00,
                           const vec3f &dir_du,
                           const vec3f &dir_dv)
  {
    owlParamsSet3f(lp,"camera.org",   org.x,org.y,org.z);
    owlParamsSet3f(lp,"camera.dir_00",dir_00.x,dir_00.y,dir_00.z);
    owlParamsSet3f(lp,"camera.dir_du",dir_du.x,dir_du.y,dir_du.z);
    owlParamsSet3f(lp,"camera.dir_dv",dir_dv.x,dir_dv.y,dir_dv.z);
  }

  void Renderer::render(const vec2i &fbSize,
                        uint32_t *fbPointer)
  {
    if (fbSize != this->fbSize) {
#ifdef DUMP_FRAMES
      owlBufferResize(fbDepth,fbSize.x*fbSize.y);
#endif
      if (!accumBuffer)
        accumBuffer = owlBufferCreateEXT(owl,OWL_FLOAT4,1,nullptr);
      owlBufferResize(accumBuffer,fbSize.x*fbSize.y);
      owlParamsSetBuffer(lp,"accumBuffer",accumBuffer);
      this->fbSize = fbSize;
    }
    owlParamsSetPointer(lp,"fbPointer",fbPointer);

    owlParamsSet1i(lp,"accumID",accumID);
    accumID++;
    owlParamsSet1i(lp,"render.spp",max(spp,1));
    owlParamsSet1i(lp,"render.heatMapEnabled",heatMapEnabled);
    owlParamsSet1f(lp,"render.heatMapScale",heatMapScale);

    owlLaunch2D(rayGen,fbSize.x,fbSize.y,lp);
  }
                             
}
