
#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <fake/owl.h>

#include "Resource.h"

namespace visionaray
{
    class GroupBVH;
}

namespace fake
{
    struct Group : ContextResource
    {
        virtual ~Group();

        virtual void buildAccel() = 0;
        virtual void refitAccel() = 0;

        virtual std::shared_ptr<visionaray::GroupBVH> getAccel() = 0;

        TraversableHandle traversableHandle = { nullptr, nullptr, uint64_t(-1) };

        std::vector<Group*> parents;
    };

} // fake
