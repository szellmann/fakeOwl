#include <algorithm>

#include <visionaray/math/forward.h>
#include <visionaray/math/matrix.h>
#include <visionaray/math/vector.h>

#include "Visionaray.h" // keep on top!
#include "GeomGroup.h"
#include "InstanceGroup.h"
#include "Logging.h"

namespace fake
{
    InstanceGroup::InstanceGroup(std::size_t     numInstances,
                                 const OWLGroup *initGroups,
                                 const uint32_t *initInstanceIDs,
                                 const float    *initTransforms,
                                 OWLMatrixFormat matrixFormat)
        : bvh(new visionaray::GroupBVH)
        , instances(numInstances)
        , instIDs(numInstances)
        , transforms(numInstances * 12)
    {
        traversableHandle = bvh->traversableHandle;

        if (initGroups)
        {
            std::copy((Group**)initGroups, (Group**)initGroups + numInstances, instances.data());

            for (std::size_t i = 0; i < numInstances; ++i)
            {
                Group* grp = (Group*)instances[i];
                grp->parents.push_back(this);
            }
        }

        if (initInstanceIDs)
            std::copy(initInstanceIDs, initInstanceIDs + numInstances, instIDs.data());
        else
        {
            for (std::size_t i = 0; i < numInstances; ++i)
            {
                instIDs[i] = uint32_t(i);
            }
        }

        if (initTransforms)
            std::copy(initTransforms, initTransforms + numInstances * 12, (float*)transforms.data());
        else
        {
            // Init with identities
            visionaray::mat4x3 I = { visionaray::mat3::identity(), visionaray::vec3(0.f) };

            float* data = transforms.data();

            for (std::size_t i = 0; i < numInstances; ++i)
            {
                std::copy(I.data(), I.data() + 12, data);

                data += 12;
            }
        }
    }

    InstanceGroup::~InstanceGroup()
    {
    }

    void InstanceGroup::setChild(std::size_t index, Group* child)
    {
        if (index >= instances.size())
        {
            FAKE_LOG(fake::logging::Level::Error) << "Child index out of range";
            return;
        }

        instances[index] = child;

        child->parents.push_back(this);
    }

    void InstanceGroup::setTransform(std::size_t     index,
                                     const float*    trans,
                                     OWLMatrixFormat matrixFormat)
    {
        float* data = transforms.data() + index * 12;

        std::copy(trans, trans + 12, data);

        if (ownAccessor != nullptr)
        {
            // This node was optimized and we potentially have to undo this!
            visionaray::mat4x3 I{ visionaray::mat3::identity(), visionaray::vec3(0.f) };
            visionaray::mat4x3 m(trans);

            if (m != I)
                undoOptimization();
        }
    }

    void InstanceGroup::buildAccel()
    {
        instanceBVHs.resize(instances.size());

        for (std::size_t i = 0; i < instances.size(); ++i)
        {
            instanceBVHs[i] = instances[i]->getAccel();
        }

        bvh->reset(instanceBVHs.data(), instIDs.data(), (visionaray::mat4x3*)transforms.data(), instanceBVHs.size());
        bvh->build();
    }

    void InstanceGroup::refitAccel()
    {
        FAKE_LOG(fake::logging::Level::Warning) << "Refit not implemented yet, rebuilding instead!";
        buildAccel();
    }

    visionaray::GroupBVH::SP InstanceGroup::getAccel()
    {
        return bvh;
    }

    void InstanceGroup::optimizeBottomUp()
    {
        if (instances.size() == 1)
        {
            visionaray::mat4x3 I{ visionaray::mat3::identity(), visionaray::vec3(0.f) };
            visionaray::mat4x3 m(transforms.data());

            if (m == I)
            {
                fake::Traversable& traversable = fake::getTraversable(traversableHandle);
                fake::Traversable& childTraversable = fake::getTraversable(
                            instances[0]->getAccel()->traversableHandle);

                if (childTraversable.accessor != nullptr)
                {
                    ownAccessor = traversable.accessor;
                    traversable.accessor = childTraversable.accessor;
                    FAKE_LOG_DBG << "Patching child accessor into parent";

                    // Now try to optimize bottom-up
                    for (size_t p = 0; p < parents.size(); ++p)
                    {
                        if (InstanceGroup* ig = dynamic_cast<InstanceGroup*>(parents[p]))
                            ig->optimizeBottomUp();
                    }
                    return;
                }
                else
                    FAKE_LOG_DBG << "Cannot optimize, child's accessor is NULL";
            }
            else
                FAKE_LOG_DBG << "Cannot optimize, transform is not identity";
        }
    }

    void InstanceGroup::undoOptimization()
    {
        if (ownAccessor != nullptr)
        {
            FAKE_LOG_DBG << "Undo optimization";

            fake::Traversable& traversable = fake::getTraversable(traversableHandle);
            traversable.accessor = ownAccessor;

            for (std::size_t i = 0; i < instances.size(); ++i)
            {
                if (InstanceGroup* ig = dynamic_cast<InstanceGroup*>(instances[i]))
                    ig->undoOptimization();
            }
        }
    }

} // fake
