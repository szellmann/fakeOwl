
#pragma once

#include <cstddef>
#include <cstdint>
#include <map>

#include <owl/owl_host.h>
#include <owl/common/math/AffineSpace.h>

#include <fake/owl.h>

#include "Group.h"

namespace fake
{
    struct DataTypeInfo
    {
        OWLDataType type;
        std::size_t sizeInBytes;
    };

    static std::map<OWLDataType, DataTypeInfo> dataTypeInfoMap = {
        { OWL_INVALID_TYPE,     { OWL_INVALID_TYPE,     0                               } },

        { OWL_BOOL,             { OWL_BOOL,             sizeof(bool)                    } },
        { OWL_BOOL2,            { OWL_BOOL2,            2 * sizeof(bool)                } },
        { OWL_BOOL3,            { OWL_BOOL3,            3 * sizeof(bool)                } },
        { OWL_BOOL4,            { OWL_BOOL4,            4 * sizeof(bool)                } },

        { OWL_UCHAR,            { OWL_UCHAR,            sizeof(uint8_t)                 } },
        { OWL_UCHAR2,           { OWL_UCHAR2,           2 * sizeof(uint8_t)             } },
        { OWL_UCHAR3,           { OWL_UCHAR3,           3 * sizeof(uint8_t)             } },
        { OWL_UCHAR4,           { OWL_UCHAR4,           4 * sizeof(uint8_t)             } },

        { OWL_CHAR,             { OWL_CHAR,             sizeof(int8_t)                  } },
        { OWL_CHAR2,            { OWL_CHAR2,            2 * sizeof(int8_t)              } },
        { OWL_CHAR3,            { OWL_CHAR3,            3 * sizeof(int8_t)              } },
        { OWL_CHAR4,            { OWL_CHAR4,            4 * sizeof(int8_t)              } },

        { OWL_USHORT,           { OWL_USHORT,           sizeof(uint16_t)                } },
        { OWL_USHORT2,          { OWL_USHORT2,          2 * sizeof(uint16_t)            } },
        { OWL_USHORT3,          { OWL_USHORT3,          3 * sizeof(uint16_t)            } },
        { OWL_USHORT4,          { OWL_USHORT4,          4 * sizeof(uint16_t)            } },

        { OWL_SHORT,            { OWL_SHORT,            sizeof(int16_t)                 } },
        { OWL_SHORT2,           { OWL_SHORT2,           2 * sizeof(int16_t)             } },
        { OWL_SHORT3,           { OWL_SHORT3,           3 * sizeof(int16_t)             } },
        { OWL_SHORT4,           { OWL_SHORT4,           4 * sizeof(int16_t)             } },

        { OWL_UINT,             { OWL_UINT,             sizeof(uint32_t)                } },
        { OWL_UINT2,            { OWL_UINT2,            2 * sizeof(uint32_t)            } },
        { OWL_UINT3,            { OWL_UINT3,            3 * sizeof(uint32_t)            } },
        { OWL_UINT4,            { OWL_UINT4,            4 * sizeof(uint32_t)            } },

        { OWL_INT,              { OWL_INT,              sizeof(int32_t)                 } },
        { OWL_INT2,             { OWL_INT2,             2 * sizeof(int32_t)             } },
        { OWL_INT3,             { OWL_INT3,             3 * sizeof(int32_t)             } },
        { OWL_INT4,             { OWL_INT4,             4 * sizeof(int32_t)             } },

        { OWL_ULONG,            { OWL_ULONG,            sizeof(uint64_t)                } },
        { OWL_ULONG2,           { OWL_ULONG2,           2 * sizeof(uint64_t)            } },
        { OWL_ULONG3,           { OWL_ULONG3,           3 * sizeof(uint64_t)            } },
        { OWL_ULONG4,           { OWL_ULONG4,           4 * sizeof(uint64_t)            } },

        { OWL_LONG,             { OWL_LONG,             sizeof(int64_t)                 } },
        { OWL_LONG2,            { OWL_LONG2,            2 * sizeof(int64_t)             } },
        { OWL_LONG3,            { OWL_LONG3,            3 * sizeof(int64_t)             } },
        { OWL_LONG4,            { OWL_LONG4,            4 * sizeof(int64_t)             } },

        { OWL_FLOAT,            { OWL_FLOAT,            sizeof(float)                   } },
        { OWL_FLOAT2,           { OWL_FLOAT2,           2 * sizeof(float)               } },
        { OWL_FLOAT3,           { OWL_FLOAT3,           3 * sizeof(float)               } },
        { OWL_FLOAT4,           { OWL_FLOAT4,           4 * sizeof(float)               } },

        { OWL_AFFINE3F,         { OWL_AFFINE3F,         sizeof(owl::affine3f)           } },

        { OWL_BUFFER_SIZE,      { OWL_BUFFER_SIZE,      sizeof(std::size_t)             } },
        { OWL_BUFFER,           { OWL_BUFFER,           0/* not imoplemented */         } },
        { OWL_BUFFER_POINTER,   { OWL_BUFFER_POINTER,   sizeof(void*)                   } },

        { OWL_TEXTURE,          { OWL_TEXTURE,          sizeof(TextureHandle)           } },

        { OWL_GROUP,            { OWL_GROUP,            sizeof(TraversableHandle)       } },
        };


    static DataTypeInfo GetDataTypeInfo(OWLDataType type)
    {
        if (type < OWL_USER_TYPE_BEGIN)
            return dataTypeInfoMap[type];

        // size is encoded in value
        std::size_t size = std::size_t(type) - std::size_t(OWL_USER_TYPE_BEGIN);
        return { type, size };
    }

} // fake
