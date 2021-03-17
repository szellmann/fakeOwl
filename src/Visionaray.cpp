#include <algorithm>
#include <vector>

#include <visionaray/math/forward.h>
#include <visionaray/math/ray.h>
#include <visionaray/aligned_vector.h>

#include "Context.h"
#include "Trace.h"
#include "Visionaray.h"

namespace visionaray
{
    //--- Traversable -------------------------------------

    fake::TraversableHandle registerTraversable(const index_bvh<UserGeomPrimitive>::bvh_ref& ref)
    {
        fake::Traversable traversable;
        traversable.accessor = (const void*)&ref;
        return fake::registerTraversable(traversable);
    }

    fake::TraversableHandle registerTraversable(const GroupBVH::Reference& ref)
    {
        fake::Traversable traversable;
        traversable.accessor = (const void*)&ref;
        return fake::registerTraversable(traversable);
    }


    GroupBVH::GroupBVH()
    {
        traversableHandle = registerTraversable(accessor);
    }

    GroupBVH::GroupBVH(fake::UserGeom** userGeoms, std::size_t numUserGeoms)
    {
        reset(userGeoms, numUserGeoms);

        traversableHandle = registerTraversable(accessor);
    }

    GroupBVH::GroupBVH(fake::TrianglesGeom** triGeoms, std::size_t numTrianglesGeoms)
    {
        reset(triGeoms, numTrianglesGeoms);

        traversableHandle = registerTraversable(accessor);
    }

    GroupBVH::GroupBVH(GroupBVH* groupBVHs, unsigned* instIDs, mat4x3f* transforms, std::size_t numGroupBVHs)
    {
        reset(groupBVHs, instIDs, transforms, numGroupBVHs);

        traversableHandle = registerTraversable(accessor);
    }

    GroupBVH::~GroupBVH()
    {
    }

    void GroupBVH::reset(fake::UserGeom** userGeoms, std::size_t numUserGeoms)
    {
        asUserGeomGroup.data.reset(new index_bvh<UserGeomPrimitive>);

        asUserGeomGroup.accessor = asUserGeomGroup.data->ref();

        asUserGeomGroup.userGeoms = userGeoms;

        asUserGeomGroup.numUserGeoms = numUserGeoms;
    }

    void GroupBVH::reset(fake::TrianglesGeom** triGeoms, std::size_t numTrianglesGeoms)
    {
        asTrianglesGeomGroup.data.reset(new index_bvh<TrianglePrimitive>);

        asTrianglesGeomGroup.accessor = asTrianglesGeomGroup.data->ref();

        asTrianglesGeomGroup.trianglesGeoms = triGeoms;

        asTrianglesGeomGroup.numTrianglesGeoms = numTrianglesGeoms;
    }

    void GroupBVH::reset(GroupBVH* groupBVHs, unsigned* instIDs, mat4x3f* transforms, std::size_t numGroupBVHs)
    {
        asInstanceGroup.bottomLevelBVHs = groupBVHs;
        asInstanceGroup.numBottomLevelBVHs = numGroupBVHs;
        asInstanceGroup.data.reset(new index_bvh<Reference::Instance>);

        asInstanceGroup.instIDs.resize(numGroupBVHs);
        if (instIDs == nullptr)
        {
            for (std::size_t i = 0; i < numGroupBVHs; ++i)
            {
                asInstanceGroup.instIDs[i] = unsigned(i);
            }
        }
        else
            std::copy(instIDs, instIDs + numGroupBVHs, asInstanceGroup.instIDs.data());

        asInstanceGroup.transforms.resize(numGroupBVHs);
        if (transforms == nullptr)
        {
            for (std::size_t i = 0; i < numGroupBVHs; ++i)
            {
                asInstanceGroup.transforms[i] = { mat3f::identity(), vec3f(0.f) };
            }
        }
        else
            std::copy(transforms, transforms + numGroupBVHs, asInstanceGroup.transforms.data());

        asInstanceGroup.accessor = asInstanceGroup.data->ref();
    }

    void GroupBVH::build()
    {
        if (asUserGeomGroup.data != nullptr) // User Geom BVH
        {
            std::size_t primCount = 0;

            for (std::size_t i = 0; i < asUserGeomGroup.numUserGeoms; ++i)
            {
                fake::UserGeom* userGeom = asUserGeomGroup.userGeoms[i];
                primCount += userGeom->getPrimCount();
            }

            aligned_vector<UserGeomPrimitive> prims(primCount);

            std::size_t primIndex = 0;
            for (std::size_t i = 0; i < asUserGeomGroup.numUserGeoms; ++i)
            {
                fake::UserGeom* userGeom = asUserGeomGroup.userGeoms[i];

                for (std::size_t j = 0; j < userGeom->getPrimCount(); ++j)
                {
                    prims[primIndex].userGeom = userGeom;
                    // Might be duplicate primIDs, but that's ok
                    prims[primIndex].prim_id = (unsigned)j;

                    ++primIndex;
                }
            }

            binned_sah_builder builder;
            // TODO: Support spatial splits / SBVH!
            builder.enable_spatial_splits(false);

            std::cout << "Building BVH...\n";
            *asUserGeomGroup.data = builder.build(index_bvh<UserGeomPrimitive>{}, prims.data(), prims.size());
            std::cout << "Done!\n";

            asUserGeomGroup.accessor = asUserGeomGroup.data->ref();
        }
        else if (asTrianglesGeomGroup.data != nullptr) // Triangles Geom BVH
        {
            std::size_t primCount = 0;

            for (std::size_t i = 0; i < asTrianglesGeomGroup.numTrianglesGeoms; ++i)
            {
                fake::TrianglesGeom* triGeom = asTrianglesGeomGroup.trianglesGeoms[i];
                primCount += triGeom->getIndexCount();
            }

            aligned_vector<TrianglePrimitive> prims(primCount);

            std::size_t primIndex = 0;
            for (std::size_t i = 0; i < asTrianglesGeomGroup.numTrianglesGeoms; ++i)
            {
                fake::TrianglesGeom* triGeom = asTrianglesGeomGroup.trianglesGeoms[i];

                for (std::size_t j = 0; j < triGeom->getIndexCount(); ++j)
                {
                    owl::vec3i index = triGeom->getIndex(j);
                    owl::vec3f v1 = triGeom->getVertex(index.x);
                    owl::vec3f v2 = triGeom->getVertex(index.y);
                    owl::vec3f v3 = triGeom->getVertex(index.z);

                    prims[primIndex].trianglesGeom = triGeom;
                    // Might be duplicate primIDs, but that's ok
                    prims[primIndex].prim_id = (unsigned)j;
                    prims[primIndex].v1 = { v1.x, v1.y, v1. z };
                    prims[primIndex].e1 = { v2.x-v1.x, v2.y-v1.y, v2.z-v1.z };
                    prims[primIndex].e2 = { v3.x-v1.x, v3.y-v1.y, v3.z-v1.z };
                    prims[primIndex].gn = normalize(cross(prims[primIndex].e1, prims[primIndex].e2));
                    ++primIndex;
                }
            }

            binned_sah_builder builder;
            // TODO: Support spatial splits / SBVH!
            builder.enable_spatial_splits(false);

            //std::cout << "Building BVH...\n";
            *asTrianglesGeomGroup.data = builder.build(index_bvh<TrianglePrimitive>{}, prims.data(), prims.size());
            auto bounds = asTrianglesGeomGroup.data->node(0).get_bounds();
            //std::cout << "Done!\n";

            asTrianglesGeomGroup.accessor = asTrianglesGeomGroup.data->ref();
        }
        else if (asInstanceGroup.data != nullptr) // Instance BVH
        {
            std::cout << "Building this\n";
            aligned_vector<Reference::Instance> prims(asInstanceGroup.numBottomLevelBVHs);

            for (std::size_t i = 0; i < asInstanceGroup.numBottomLevelBVHs; ++i)
            {
                // Set instance id
                asInstanceGroup.bottomLevelBVHs[i].instID = asInstanceGroup.instIDs[i];

                // Compute absolute transforms
                mat3 w2o3(worldToObjectTransform.col0,
                          worldToObjectTransform.col1,
                          worldToObjectTransform.col2);
                mat4x3 m = asInstanceGroup.transforms[i];
                mat3 m3(m.col0, m.col1, m.col2);
                w2o3 = w2o3 * m3;
                mat4x3 w2o = { w2o3, worldToObjectTransform.col3 + m.col3 };
                mat4x3 o2w = { inverse(w2o3), -w2o.col3 };
                asInstanceGroup.bottomLevelBVHs[i].objectToWorldTransform = o2w;
                asInstanceGroup.bottomLevelBVHs[i].worldToObjectTransform = w2o;

                // Create visionaray instance
                prims[i] = asInstanceGroup.bottomLevelBVHs[i].inst(asInstanceGroup.transforms[i]);
            }

            binned_sah_builder builder;
            // TODO: Support spatial splits / SBVH!
            builder.enable_spatial_splits(false);

            std::cout << "Building BVH...\n";
            *asInstanceGroup.data = builder.build(index_bvh<Reference::Instance>{}, prims.data(), prims.size());
            std::cout << "Done!\n";

            asInstanceGroup.accessor = asInstanceGroup.data->ref();
        }

        // One extra level to determine at runtime which type of BVH this really is
        binned_sah_builder builder;
        Reference reference = ref();
        topLevelBVH = builder.build(index_bvh<GroupBVH::Reference>{}, &reference, 1);

        accessor = reference;

        fake::Traversable& traversable = fake::getTraversable(traversableHandle);
        traversable.accessor = (const void*)&accessor;
    }

    void GroupBVH::refit()
    {
    }

    GroupBVH::Reference::Instance GroupBVH::inst(const mat4x3f& trans)
    {
        Reference::Instance result;
        result.accessor = topLevelBVH.inst(trans);
        result.instID = instID;
        result.objectToWorldTransform = objectToWorldTransform;
        result.worldToObjectTransform = worldToObjectTransform;
        return result;
    }

    GroupBVH::Reference GroupBVH::ref()
    {
        GroupBVH::Reference reference;

        if (asUserGeomGroup.data != nullptr)
        {
            reference.bounds = asUserGeomGroup.data->node(0).get_bounds();
            reference.asUserGeomGroup.accessor = &asUserGeomGroup.accessor;
        }
        if (asTrianglesGeomGroup.data != nullptr)
        {
            reference.bounds = asTrianglesGeomGroup.data->node(0).get_bounds();
            reference.asTrianglesGeomGroup.accessor = &asTrianglesGeomGroup.accessor;
        }
        else if (asInstanceGroup.data != nullptr)
        {
            reference.bounds = asInstanceGroup.data->node(0).get_bounds();
            reference.asInstanceGroup.accessor = &asInstanceGroup.accessor;
        }
        return reference;
    }


} // visionaray


