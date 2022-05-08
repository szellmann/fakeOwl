#pragma once

#include <stdint.h>

#include <owl/owl.h>

using namespace owl;
using namespace owl::common;

typedef owl::interval<float> range1f;

namespace dvr {
  struct RayGen {
  };

  typedef vec3f Particle;
  struct ParticleGeom
  {
    OptixTraversableHandle world;
    box3f domain;
    Particle* particles;
    unsigned numParticles;
    float radius;
  };
#if 1
  struct TriangleGeom {
    vec3f *vertexBuffer;
    vec3i *indexBuffer;
    float *slopes;
  };
#endif
  struct SSPRD {
    int primID; // only for debugging/visualizing
    vec3f Ng; // only for debugging/visualizing
    float t;
#if SS_SINGLE_RAY
    bool backFace;
#endif
  };

  struct LaunchParams {
    uint32_t *fbPointer;
    float4   *accumBuffer;
    int       accumID;
#ifdef DUMP_FRAMES
    // to allow dumping rgba and depth for some unrelated compositing work....
    float    *fbDepth;
#endif
    OptixTraversableHandle world;
    box3f domain;
    Particle* particles;
    unsigned numParticles;
    float radius;
#if 1
    struct {
      vec3f *vertexBuffer;
      vec3f *indexBuffer;
      OptixTraversableHandle group;
    } model;
#endif
    struct {
      vec3f org;
      vec3f dir_00;
      vec3f dir_du;
      vec3f dir_dv;
    } camera;
    struct {
      float dt;
      int   heatMapEnabled;
      float heatMapScale;
      int   spp;
    } render;
  };

}
