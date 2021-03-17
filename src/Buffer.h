
#pragma once

#include <cstddef>

#include <owl/owl_host.h>

#include "Resource.h"

namespace fake
{
    struct Buffer : ContextResource
    {
        enum Type { BtHostBuffer, };

        Buffer() = default;
        Buffer(OWLDataType dataType, std::size_t numElems);
        virtual ~Buffer();

        virtual void resize(std::size_t newNumElems) = 0;
        virtual void upload(const void* hostPtr, std::size_t offset, std::size_t numBytes) = 0;
        virtual const void* getPointer() = 0;

        void* dataPtr = nullptr;
        OWLDataType dataType = OWL_INVALID_TYPE;
        std::size_t numElems = 0;
    };


    // HostBuffer
    class HostBuffer : public Buffer
    {
    public:
        HostBuffer(OWLDataType dataType, std::size_t numElems, const void* init);
       ~HostBuffer();

        void resize(std::size_t newNumElems);
        void upload(const void* hostPtr, std::size_t offset, std::size_t numBytes);
        const void* getPointer();

    };

} // fake
