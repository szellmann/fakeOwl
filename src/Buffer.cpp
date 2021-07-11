#include <algorithm>
#include <cstdint>
#include <cstring>

#include "Buffer.h"
#include "DataType.h"

namespace fake
{
    Buffer::Buffer(OWLDataType dataType, std::size_t numElems)
        : dataPtr(new uint8_t[numElems * GetDataTypeInfo(dataType).sizeInBytes])
        , dataType(dataType)
        , numElems(numElems)
    {
    }

    Buffer::~Buffer()
    {
    }



    HostBuffer::HostBuffer(OWLDataType dataType, std::size_t numElems, const void* init)
        : Buffer(dataType, numElems)
    {
        if (init != nullptr)
        {
            std::memcpy(dataPtr, init, numElems * GetDataTypeInfo(dataType).sizeInBytes);
        }
    }

    HostBuffer::~HostBuffer()
    {
        delete[] (uint8_t*)dataPtr;
    }

    void HostBuffer::resize(std::size_t newNumElems)
    {
        std::size_t oldSizeInBytes = numElems * GetDataTypeInfo(dataType).sizeInBytes;
        std::size_t newSizeInBytes = newNumElems * GetDataTypeInfo(dataType).sizeInBytes;

        uint8_t* temp = new uint8_t[oldSizeInBytes];
        std::memcpy(temp, dataPtr, oldSizeInBytes);

        delete[] (uint8_t*)dataPtr;
        dataPtr = new uint8_t[newSizeInBytes];
        std::memcpy(dataPtr, temp, std::min(oldSizeInBytes, newSizeInBytes));

        delete[] temp;

        numElems = newNumElems;
    }

    void HostBuffer::upload(const void* hostPtr, std::size_t offset, std::size_t numBytes)
    {
        std::size_t sizeInBytes = numBytes == size_t(-1)
                ? numElems * GetDataTypeInfo(dataType).sizeInBytes : numBytes;
        std::memcpy(dataPtr, (uint8_t*)hostPtr + offset, sizeInBytes);
    }

    size_t HostBuffer::getSizeInBytes()
    {
        return numElems * GetDataTypeInfo(dataType).sizeInBytes;
    }

    const void* HostBuffer::getPointer()
    {
        return dataPtr;
    }

} // fake
