
#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include <owl/owl_host.h>

#include "Group.h"
#include "Visionaray.h"

namespace fake
{
    class InstanceGroup : public Group
    {
    public:
        InstanceGroup(std::size_t     numInstances,
                      const OWLGroup *initGroups,
                      const uint32_t *initInstanceIDs,
                      const float    *initTransforms,
                      OWLMatrixFormat matrixFormat);
       ~InstanceGroup();

        void setChild(std::size_t index, Group* child);

        void setTransform(std::size_t     index,
                          const float*    trans,
                          OWLMatrixFormat matrixFormat);

        void buildAccel();
        void refitAccel();

        const visionaray::GroupBVH& getAccel() const;

        // Try to patch this group's BVH into parent(s)
        void optimizeBottomUp();

        // Reset the old accessor, e.g. when the tranform changed
        void undoOptimization();

    private:
        visionaray::GroupBVH bvh;
        std::vector<Group*> instances;
        std::vector<float> transforms;
        std::vector<uint32_t> instIDs;
        std::vector<visionaray::GroupBVH> instanceBVHs;

        // Potentially store our own accessor here
        const void* ownAccessor = nullptr;
    };

} // fake
