
#pragma once

#include <cstddef>
#include <memory>

#include <visionaray/math/aabb.h>
#include <visionaray/math/forward.h>
#include <visionaray/math/io.h>
#include <visionaray/math/matrix.h>
#include <visionaray/math/primitive.h>
#include <visionaray/bvh.h>
#include <visionaray/traverse.h>

#include <owl/common/math/box.h>
#include <owl/owl.h>

#include <fake/owl.h>

#include "Bounds.h"
#include "Geom.h"
#include "Group.h"
#include "Intersect.h"
#include "Logging.h"
#include "Trace.h"
#include "VarDecl.h"

namespace visionaray
{
    template <typename Cond>
    inline void update_if(fake::HitRecord& dst, const fake::HitRecord& src, const Cond& cond)
    {
        using Base = hit_record<fake::Ray, primitive<unsigned>>;

        update_if((Base&)dst, (const Base&)src, cond);
        if (cond)
        {
            if (src.geom != nullptr)
            {
                dst.geom = src.geom;
                dst.kind = src.kind;
            }

            if (src.instID != ~0U)
                dst.instID = src.instID;

            dst.objectRayOrigin = src.objectRayOrigin;
            dst.objectRayDirection = src.objectRayDirection;

            dst.objectToWorldTransform = src.objectToWorldTransform;
            dst.worldToObjectTransform = src.worldToObjectTransform;

            dst.hitKind = src.hitKind;
        }
    }


    //--- UserGeomPrimitive -------------------------------

    struct UserGeomPrimitive : primitive<unsigned>
    {
        // User geometry this primitive belongs to
        const fake::UserGeom* userGeom;
    };

    inline aabb get_bounds(UserGeomPrimitive prim)
    {
        fake::Bounds* boundsProg = prim.userGeom->getBoundsProg();
        void (*entryPoint)(const void*, owl::box3f&, const int);
        entryPoint = (void (*)(const void*, owl::box3f&, const int))boundsProg->entryPointSym;

        if (boundsProg == nullptr)
        {
            FAKE_LOG(fake::logging::Level::Error) << "No bounds prog assigned!";
            return {};
        }

        fake::VarDecl* vars = (fake::VarDecl*)prim.userGeom; // slicing
        owl::box3f primBounds;
        entryPoint(vars->dataPtr,
                   primBounds,
                   (int)prim.prim_id);

        return {{ primBounds.lower.x,  primBounds.lower.y, primBounds.lower.z },
                { primBounds.upper.x,  primBounds.upper.y, primBounds.upper.z }};
    }

    inline void split_primitive(aabb& L, aabb& R, float plane, int axis, UserGeomPrimitive const& prim)
    {
        VSNRAY_UNUSED(L);
        VSNRAY_UNUSED(R);
        VSNRAY_UNUSED(plane);
        VSNRAY_UNUSED(axis);
        VSNRAY_UNUSED(prim);
    
        // TODO: implement this to support SBVHs
    }

    static __forceinline__ fake::HitRecord intersect(fake::Ray r, const UserGeomPrimitive& prim)
    {
        fake::Intersect* intersectProg = prim.userGeom->getIntersectProg(r.payload->rayType);

        void (*entryPoint)();
        entryPoint = (void (*)())intersectProg->entryPointSym;

        fake::ProgramState* (*fakePrepareIntersection)();
        fakePrepareIntersection
                = (fake::ProgramState* (*)())intersectProg->fakePrepareIntersectionSym;

        fake::IntersectionResult* (*fakeGetIntersectionResult)();
        fakeGetIntersectionResult
                = (fake::IntersectionResult* (*)())intersectProg->fakeGetIntersectionResultSym;

        fake::ProgramState* state = fakePrepareIntersection();
        state->primID = prim.prim_id;
        state->instID = r.payload->instID;
        for (int i = 0; i < 12; ++i)
        {
            state->objectToWorldTransform[i] = r.payload->objectToWorldTransform.data()[i];
            state->worldToObjectTransform[i] = r.payload->worldToObjectTransform.data()[i];
        }
        state->worldRayOrigin[0] = r.payload->worldRayOrigin.x;
        state->worldRayOrigin[1] = r.payload->worldRayOrigin.y;
        state->worldRayOrigin[2] = r.payload->worldRayOrigin.z;
        state->worldRayDirection[0] = r.payload->worldRayDirection.x;
        state->worldRayDirection[1] = r.payload->worldRayDirection.y;
        state->worldRayDirection[2] = r.payload->worldRayDirection.z;
        state->objectRayOrigin[0] = r.ori.x;
        state->objectRayOrigin[1] = r.ori.y;
        state->objectRayOrigin[2] = r.ori.z;
        state->objectRayDirection[0] = r.dir.x;
        state->objectRayDirection[1] = r.dir.y;
        state->objectRayDirection[2] = r.dir.z;
        state->rayTmin = r.tmin;
        state->rayTmax = r.tmax;
        state->p0 = *r.payload->p0;
        state->p1 = *r.payload->p1;
        state->sbtPointer = prim.userGeom->dataPtr;

        entryPoint();

        fake::IntersectionResult* res = fakeGetIntersectionResult();

        fake::HitRecord hr;
        hr.hit = res->wasHit;
        if (hr.hit)
        {
            hr.t = res->hitT;
            hr.prim_id = prim.prim_id;
            hr.geom = prim.userGeom;
            hr.kind = OWL_GEOM_USER;
            hr.objectRayOrigin = r.ori;
            hr.objectRayDirection = r.dir;
            hr.objectToWorldTransform = r.payload->objectToWorldTransform;
            hr.worldToObjectTransform = r.payload->worldToObjectTransform;
        }
        return hr;
    }

    //--- TrianglePrimitive -----------------------------

    struct TrianglePrimitive : basic_triangle<3, float>
    {
        // Geometric normal for front or back-face culling
        vec3f gn;

        // Triangles geometry this primitive belongs to
        const fake::TrianglesGeom* trianglesGeom;
    };

    static __forceinline__ fake::HitRecord intersect(fake::Ray r, const TrianglePrimitive& prim)
    {
        fake::HitRecord result;
        float ndotv = dot(prim.gn, -r.dir);
        bool cull = (r.payload->rayFlags & OPTIX_RAY_FLAG_CULL_BACK_FACING_TRIANGLES) && ndotv < 0.f;
        cull     |= (r.payload->rayFlags & OPTIX_RAY_FLAG_CULL_FRONT_FACING_TRIANGLES) && ndotv >= 0.f;

        if (!cull)
        {
            auto hr = intersect(r, (const basic_triangle<3, float>&)prim);

            if (hr.hit)
            {
                result.hit = hr.hit;
                result.t = hr.t;
                result.prim_id = hr.prim_id;
                result.geom_id = hr.geom_id;
                result.u = hr.u;
                result.v = hr.v;
                result.geom = prim.trianglesGeom;
                result.kind = OWL_GEOM_TRIANGLES;
                result.instID = r.payload->instID;
                result.objectToWorldTransform = r.payload->objectToWorldTransform;
                result.worldToObjectTransform = r.payload->worldToObjectTransform;
                result.objectRayOrigin = r.ori;
                result.objectRayDirection = r.dir;
                result.hitKind = ndotv >= 0.f ? 0xFE : 0xFF;
            }
        }
        return result;
    }

    inline void split_primitive(aabb& L, aabb& R, float plane, int axis, TrianglePrimitive const& prim)
    {
        return split_primitive(L, R, plane, axis, (const basic_triangle<3, float>&)prim);
    }


    //--- GroupBVH ----------------------------------------

    class GroupBVH
    {
    public:
        struct Reference : primitive<unsigned>
        {
            struct Instance
            {
                index_bvh<GroupBVH::Reference>::bvh_inst accessor;

                unsigned instID;

                mat4x3 objectToWorldTransform;
                mat4x3 worldToObjectTransform;
            };

            aabb bounds;

            struct
            {
                index_bvh<UserGeomPrimitive>::bvh_ref* accessor = nullptr;
            } asUserGeomGroup;

            struct
            {
                index_bvh<TrianglePrimitive>::bvh_ref* accessor = nullptr;
            } asTrianglesGeomGroup;

            struct
            {
                index_bvh<Instance>::bvh_ref* accessor = nullptr;
            } asInstanceGroup;
        };

        typedef Reference Accessor;

    public:
        GroupBVH();
        GroupBVH(fake::UserGeom** userGeoms, std::size_t numUserGeoms);
        GroupBVH(fake::TrianglesGeom** triGeoms, std::size_t numTrianglesGeoms);
        GroupBVH(GroupBVH* groupBVHs, unsigned* instIDs, mat4x3f* transforms, std::size_t numGroupBVHs);
       ~GroupBVH();

        void reset(fake::UserGeom** userGeoms, std::size_t numUserGeoms);
        void reset(fake::TrianglesGeom** triGeoms, std::size_t numTrianglesGeoms);
        void reset(GroupBVH* groupBVHs, unsigned* instIDs, mat4x3f* transforms, std::size_t numGroupBVHs);

        // A handle to retrieve an accessor to this BVH
        fake::TraversableHandle traversableHandle;

        void build();
        void refit();

        Reference::Instance inst(const mat4x3f& trans);

        Reference ref();

    private:
        // The top level BVH, dispatches to child BVHs at run time
        index_bvh<GroupBVH::Reference> topLevelBVH;

        // An accessor to the BVH
        Accessor accessor;

        // This instances' ID
        unsigned instID = unsigned(-1);

        // The transform to get to the world
        mat4x3 objectToWorldTransform = { mat3::identity(), vec3(0.f) };

        // The transform to get here
        mat4x3 worldToObjectTransform = { mat3::identity(), vec3(0.f) };

        struct
        {
            // The BVH data
            std::shared_ptr<index_bvh<UserGeomPrimitive>> data = nullptr;

            // An accessor for the BVH data as user geometry
            index_bvh<UserGeomPrimitive>::bvh_ref accessor;

            // Pointer to the geom data
            fake::UserGeom** userGeoms = nullptr;

            // Number of user geoms
            std::size_t numUserGeoms = 0;
        } asUserGeomGroup;

        struct
        {
            // The BVH data
            std::shared_ptr<index_bvh<TrianglePrimitive>> data = nullptr;

            // An accessor for the BVH data as user geometry
            index_bvh<TrianglePrimitive>::bvh_ref accessor;

            // Pointer to the geom data
            fake::TrianglesGeom** trianglesGeoms = nullptr;

            // Number of triangles geoms
            std::size_t numTrianglesGeoms = 0;
        } asTrianglesGeomGroup;

        struct
        {
            // The BVH data
            std::shared_ptr<index_bvh<Reference::Instance>> data = nullptr;

            // An instance transform per child group
            std::vector<mat4x3f> transforms;

            // An instance ID per child group
            std::vector<unsigned> instIDs;

            // The blas's this BVH is created over
            GroupBVH* bottomLevelBVHs = nullptr;

            // The number of blas's
            std::size_t numBottomLevelBVHs = 0;

            // An accessor for the BVH data as user geometry
            index_bvh<Reference::Instance>::bvh_ref accessor;
        } asInstanceGroup;
    };

    inline aabb get_bounds(const GroupBVH::Reference& bvh)
    {
        return bvh.bounds;
    }

    inline void split_primitive(aabb& L, aabb& R, float plane, int axis, GroupBVH::Reference const& bvh)
    {
        VSNRAY_UNUSED(L);
        VSNRAY_UNUSED(R);
        VSNRAY_UNUSED(plane);
        VSNRAY_UNUSED(axis);
        VSNRAY_UNUSED(bvh);
    
        // TODO: implement this to support SBVHs
    }

    static __forceinline__ fake::HitRecord intersect(fake::Ray r, const GroupBVH::Reference::Instance& bvh);
    static __forceinline__ fake::HitRecord intersect(fake::Ray r, const GroupBVH::Reference& bvh)
    {
        if (bvh.asUserGeomGroup.accessor)
            return closest_hit(r, bvh.asUserGeomGroup.accessor, bvh.asUserGeomGroup.accessor + 1);
        else if (bvh.asTrianglesGeomGroup.accessor)
            return closest_hit(r, bvh.asTrianglesGeomGroup.accessor, bvh.asTrianglesGeomGroup.accessor + 1);
        else if (bvh.asInstanceGroup.accessor)
            return closest_hit(r, bvh.asInstanceGroup.accessor, bvh.asInstanceGroup.accessor + 1);

        return {};
    }

    inline aabb get_bounds(const GroupBVH::Reference::Instance& bvh)
    {
        return get_bounds(bvh.accessor);
    }

    inline void split_primitive(aabb& L, aabb& R, float plane, int axis, GroupBVH::Reference::Instance const& bvh)
    {
        VSNRAY_UNUSED(L);
        VSNRAY_UNUSED(R);
        VSNRAY_UNUSED(plane);
        VSNRAY_UNUSED(axis);
        VSNRAY_UNUSED(bvh);
    
        // TODO: implement this to support SBVHs
    }

    static __forceinline__ fake::HitRecord intersect(fake::Ray r, const GroupBVH::Reference::Instance& bvh)
    {
        unsigned prevInstID = r.payload->instID;
        mat4x3 prevO2w = r.payload->objectToWorldTransform;
        mat4x3 prevW2o = r.payload->worldToObjectTransform;

        r.payload->instID = bvh.instID;
        r.payload->objectToWorldTransform = bvh.objectToWorldTransform;
        r.payload->worldToObjectTransform = bvh.worldToObjectTransform;

        fake::HitRecord hr = closest_hit(r, &bvh.accessor, &bvh.accessor + 1);

        r.payload->instID = prevInstID;
        r.payload->objectToWorldTransform = prevO2w;
        r.payload->worldToObjectTransform = prevW2o;

        return hr;
    }

} // fake
