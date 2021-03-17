// This file is distributed under the MIT license.
// See the LICENSE file for details.

#pragma once

#include <cstdint>

namespace fake
{
    // Managed resource

    typedef uint32_t ResourceHandle;
    typedef struct
    {
        ResourceHandle resourceHandle;
    } ManagedResource_;
    typedef ManagedResource_* ManagedResource;

    ResourceHandle RegisterManagedResource(ManagedResource resource);

    void UnregisterManagedResource(ResourceHandle handle);

    ManagedResource GetManagedResource(ResourceHandle handle);


    // Context resource

    class Context;

    class ContextResource : public ManagedResource_
    {
    public:
        ContextResource();
        virtual ~ContextResource();

        Context* getContext() const;
        virtual void setContext(Context* ctx);
    protected:
        Context* context;
    };

} // fake
