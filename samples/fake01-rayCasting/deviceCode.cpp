#include <iostream>
#include <sstream>

#include "deviceCode.h"
#include "owl/common/math/random.h"

namespace dvr
{
    extern "C" __constant__ LaunchParams optixLaunchParams;


    typedef owl::common::LCG<4> Random;
    
    inline  __device__
    vec3f backGroundColor()
    {
        const vec2i pixelID = owl::getLaunchIndex();
        const float t = pixelID.y / (float)optixGetLaunchDimensions().y;
        const vec3f c = (1.0f - t)*vec3f(1.0f, 1.0f, 1.0f) + t * vec3f(0.5f, 0.7f, 1.0f);
        return c;
    }

    inline  __device__ vec4f over(const vec4f &A, const vec4f &B)
    {
        return A + (1.f-A.w)*B;
    }

    struct PRD
    {
        float sumWeightedValues;
        float sumWeights;
    };

    inline __device__
    vec4f sampleVolume(const vec3f &pos,
                       vec3f &gradient)
    {
        auto &lp = optixLaunchParams;
        PRD prd = { 0.f, 0.f };
        owl::Ray ray(pos,
                     vec3f(1.f),
                     0.f,
                     2e-10f);
        owl::traceRay(lp.world,ray,prd,
                      OPTIX_RAY_FLAG_DISABLE_ANYHIT);
        float val = prd.sumWeights > 0.f ? prd.sumWeightedValues/prd.sumWeights : 0.f;
        return {0.f,0.f,1.f,val};
    }

    inline __device__
    bool intersect(const Ray &ray,
                   const box3f &box,
                   float &t0,
                   float &t1)
    {
        vec3f lo = (box.lower - ray.origin) / ray.direction;
        vec3f hi = (box.upper - ray.origin) / ray.direction;
        
        vec3f nr = min(lo,hi);
        vec3f fr = max(lo,hi);

        t0 = max(ray.tmin,reduce_max(nr));
        t1 = min(ray.tmax,reduce_min(fr));

        return t0 < t1;
    }

    inline __device__
    float firstSampleT(const range1f &rayInterval,
                       const float dt,
                       const float ils_t0)
    {
        float numSegsf = floor((rayInterval.lower - dt*ils_t0)/dt);
        float t = dt * (ils_t0 + numSegsf);
        if (t < rayInterval.lower) t += dt;
        return t;
    }

    inline __device__
    vec4f integrateRay(const Ray &ray,
                       const float ils_t0,
                       float &z)
    {
        auto &lp = optixLaunchParams;
        const box3f bounds = lp.domain;//{ vec3f(.5f), vec3f(lp.volume.dims)-vec3f(.5f) };
        float t0, t1;
        vec3f color = 0.f;
        float alpha = 0.f;
        if (!intersect(ray,bounds,t0,t1)) {
            z = 1e10f;
            return vec4f(0.f);
        }

        const float dt = lp.render.dt;
        for (float t = firstSampleT({t0,t1},dt,ils_t0); t < t1 && alpha < .99f; t += dt) {
            vec3f gradient;
            vec4f sample = sampleVolume(ray.origin+t*ray.direction,gradient);
            sample.w *= 1.f;
            //color += dt * (1.f-alpha) * sample.w * vec3f(sample)
            //    * (.1f+.9f*fabsf(dot(gradient,ray.direction)));
            color += dt * (1.f-alpha) * sample.w * vec3f(sample);
                //* (.1f+.9f*fabsf(dot(gradient,ray.direction)));
            alpha += dt * (1.f-alpha)*sample.w;
        }

        z = t0;
        return vec4f(color,alpha);
    }

    inline  __device__ Ray generateRay(const vec2f screen)
    {
        auto &lp = optixLaunchParams;
        vec3f org = lp.camera.org;
        vec3f dir
          = lp.camera.dir_00
          + screen.u * lp.camera.dir_du
          + screen.v * lp.camera.dir_dv;
        dir = normalize(dir);
        if (fabs(dir.x) < 1e-5f) dir.x = 1e-5f;
        if (fabs(dir.y) < 1e-5f) dir.y = 1e-5f;
        if (fabs(dir.z) < 1e-5f) dir.z = 1e-5f;
        return Ray(org,dir,0.f,1e10f);
    }
 
    // ==================================================================
    // Triangle model
    // ==================================================================

    struct ModelPRD {
        float t_hit;
        vec3f gn;
        int primID;
    };

    OPTIX_CLOSEST_HIT_PROGRAM(ModelCH)()
    {
        ModelPRD& prd = owl::getPRD<ModelPRD>();
        const TriangleGeom& self = owl::getProgramData<TriangleGeom>();
        prd.t_hit = optixGetRayTmax();
        prd.primID = optixGetPrimitiveIndex();
        const vec3i index  = self.indexBuffer[prd.primID];
        const vec3f& v1     = self.vertexBuffer[index.x];
        const vec3f& v2     = self.vertexBuffer[index.y];
        const vec3f& v3     = self.vertexBuffer[index.z];
        prd.gn            = normalize(cross(v2 - v1, v3 - v1));
    }

    OPTIX_BOUNDS_PROGRAM(Particles)(const void  *geomData,
                                    box3f       &primBounds,
                                    const int    primID)
    {
        const ParticleGeom& self = *(const ParticleGeom*)geomData;
        vec3f particle = self.particles[primID];
        float radius = self.radius;
        vec3f min(particle-radius);
        vec3f max(particle+radius);
        //if (primID == 23)
        //    printf("(%f %f %f) (%f %f %f)\n",min.x,min.y,min.z,max.x,max.y,max.z);
        primBounds
            = box3f()
            .including(min)
            .including(max);
    }

    OPTIX_INTERSECT_PROGRAM(Particles)()
    {
        const ParticleGeom& self = owl::getProgramData<ParticleGeom>();
        PRD &prd = owl::getPRD<PRD>();

        vec3f pos = optixGetObjectRayOrigin();
        unsigned primID = optixGetPrimitiveIndex();
        vec3f particlePos = self.particles[primID];
        float radius = self.radius;
        if (length(particlePos-pos) <= radius && optixReportIntersection(optixGetRayTmax(),0))
        {
            auto& lp = optixLaunchParams;
            const vec2i threadIdx = owl::getLaunchIndex();
            int pixelID = threadIdx.x + owl::getLaunchDims().x*threadIdx.y;
            Random random(pixelID,lp.accumID);

            float val = random();
            float weight = 1.f-(length(particlePos-pos)/radius);
            prd.sumWeightedValues += val*weight;
            prd.sumWeights += weight;
        }
    }

    OPTIX_CLOSEST_HIT_PROGRAM(Particles)()
    {
        // const ParticleGeom& self = owl::getProgramData<ParticleGeom>();
        // PRD &prd = owl::getPRD<PRD>();
        // prd.particleID = optixGetPrimitiveIndex();
    }

    inline __device__ vec3f hue_to_rgb(float hue)
    {
        float s = saturate( hue ) * 6.0f;
        float r = saturate( fabsf(s - 3.f) - 1.0f );
        float g = saturate( 2.0f - fabsf(s - 2.0f) );
        float b = saturate( 2.0f - fabsf(s - 4.0f) );
        return vec3f(r, g, b); 
    }
      
    inline __device__ vec3f temperature_to_rgb(float t)
    {
        float K = 4.0f / 6.0f;
        float h = K - K * t;
        float v = .5f + 0.5f * t;    return v * hue_to_rgb(h);
    }
      
                                      
    inline __device__
    vec3f heatMap(float t)
    {
#if 1
        return temperature_to_rgb(t);
#else
        if (t < .25f) return lerp(vec3f(0.f,1.f,0.f),vec3f(0.f,1.f,1.f),(t-0.f)/.25f);
        if (t < .5f)  return lerp(vec3f(0.f,1.f,1.f),vec3f(0.f,0.f,1.f),(t-.25f)/.25f);
        if (t < .75f) return lerp(vec3f(0.f,0.f,1.f),vec3f(1.f,1.f,1.f),(t-.5f)/.25f);
        if (t < 1.f)  return lerp(vec3f(1.f,1.f,1.f),vec3f(1.f,0.f,0.f),(t-.75f)/.25f);
        return vec3f(1.f,0.f,0.f);
#endif
    }
  
    OPTIX_RAYGEN_PROGRAM(renderFrame)()
    {
        auto& lp = optixLaunchParams;
#if 1
        const int spp = lp.render.spp;
        const vec2i threadIdx = owl::getLaunchIndex();
        Ray ray = generateRay(vec2f(threadIdx)+vec2f(.5f));

        vec4f bgColor = vec4f(backGroundColor(),1.f);
        int pixelID = threadIdx.x + owl::getLaunchDims().x*threadIdx.y;
        Random random(pixelID,lp.accumID);

        uint64_t clock_begin = clock64();

        vec4f accumColor = 0.f;

        float z = 1e20f;
        for (int sampleID=0;sampleID<spp;sampleID++) {
            vec4f color = 0.f;
#if 0
          ModelPRD prd{-1.f,vec3f(-1),-1};
          owl::traceRay(lp.model.group,ray,prd,
                        OPTIX_RAY_FLAG_DISABLE_ANYHIT);
          if (prd.primID >= 0)
            color = vec4f(vec3f(.2f+.8f*max(0.f,dot(-ray.direction,prd.gn))),1.f);
#else
            PRD prd = { 0.f, 0.f };
            // owl::traceRay(lp.world,ray,prd,
            //               OPTIX_RAY_FLAG_DISABLE_ANYHIT);
            const float ils_t0 = random();
            color = integrateRay(ray,ils_t0,z);
#endif
            color = over(color,bgColor);
            accumColor += color;
        }
#if DUMP_FRAMES
        lp.fbPointer[pixelID] = make_rgba(accumColor);
        lp.fbDepth[pixelID]   = z;
        return;
#endif

        uint64_t clock_end = clock64();
        if (lp.render.heatMapEnabled > 0.f) {
            float t = (clock_end-clock_begin)*(lp.render.heatMapScale/spp);
            accumColor = over(vec4f(heatMap(t),.5f),accumColor);
        }

        if (lp.accumID > 0)
            accumColor += vec4f(lp.accumBuffer[pixelID]);
        lp.accumBuffer[pixelID] = accumColor;
        accumColor *= (1.f/(lp.accumID+1));
        
        bool crossHairs = (owl::getLaunchIndex().x == owl::getLaunchDims().x/2
                           ||
                           owl::getLaunchIndex().y == owl::getLaunchDims().y/2
                           );
        if (crossHairs) accumColor = vec4f(1.f) - accumColor;
        
        lp.fbPointer[pixelID] = make_rgba(vec3f(accumColor*(1.f/spp)));
#endif
        // const vec2i threadIdx = owl::getLaunchIndex();
        // int pixelID = threadIdx.x + owl::getLaunchDims().x*threadIdx.y;
        // lp.fbPointer[pixelID] = make_rgba(vec3f(1.f));
    }
}
