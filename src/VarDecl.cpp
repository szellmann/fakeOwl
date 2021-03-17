#include <algorithm>
#include <cstdint>
#include <cstring>

#include "Buffer.h"
#include "DataType.h"
#include "Logging.h"
#include "Texture.h"
#include "Trace.h"
#include "VarDecl.h"

namespace fake
{
    VarDecl::VarDecl(std::size_t sizeOfVarStruct, OWLVarDecl* vars, int numVars)
        : dataPtr(new uint8_t[sizeOfVarStruct])
        , sizeOfVarStruct(sizeOfVarStruct)
    {
        std::memset(dataPtr, 0, sizeOfVarStruct);

        if (vars == nullptr)
            return;

        if (numVars < 0)
        {
            numVars = 0;
            for (int i = 0; true; ++i)
            {
                if (vars[i].name == nullptr)
                    break;

                numVars++;
            }
        }

        inputVars.resize(numVars);
        std::copy(vars, vars + numVars, inputVars.data());
    }

    VarDecl::~VarDecl()
    {
        FAKE_LOG_DBG << "Releasing variable declaration";

        delete[] (uint8_t*)dataPtr;
    }

    void VarDecl::set(const char* name, bool b1)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        std::memcpy((uint8_t*)dataPtr + var.offset, &b1, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, bool b1, bool b2)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        bool bb[2] = { b1, b2 };
        std::memcpy((uint8_t*)dataPtr + var.offset, bb, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, bool b1, bool b2, bool b3)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        bool bbb[3] = { b1, b2, b3 };
        std::memcpy((uint8_t*)dataPtr + var.offset, bbb, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, bool b1, bool b2, bool b3, bool b4)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        bool bbbb[4] = { b1, b2, b3, b4 };
        std::memcpy((uint8_t*)dataPtr + var.offset, bbbb, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, int32_t i1)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        std::memcpy((uint8_t*)dataPtr + var.offset, &i1, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, int32_t i1, int32_t i2)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        int32_t ii[2] = { i1, i2 };
        std::memcpy((uint8_t*)dataPtr + var.offset, ii, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, int32_t i1, int32_t i2, int32_t i3)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        int32_t iii[3] = { i1, i2, i3 };
        std::memcpy((uint8_t*)dataPtr + var.offset, iii, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, int32_t i1, int32_t i2, int32_t i3, int32_t i4)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        int32_t iiii[4] = { i1, i2, i3, i4 };
        std::memcpy((uint8_t*)dataPtr + var.offset, iiii, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, uint32_t ui1)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        std::memcpy((uint8_t*)dataPtr + var.offset, &ui1, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, uint32_t ui1, uint32_t ui2)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        uint32_t uu[2] = { ui1, ui2 };
        std::memcpy((uint8_t*)dataPtr + var.offset, uu, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, uint32_t ui1, uint32_t ui2, uint32_t ui3)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        uint32_t uuu[3] = { ui1, ui2, ui3 };
        std::memcpy((uint8_t*)dataPtr + var.offset, uuu, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, uint32_t ui1, uint32_t ui2, uint32_t ui3, uint32_t ui4)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        uint32_t uuuu[4] = { ui1, ui2, ui3, ui4 };
        std::memcpy((uint8_t*)dataPtr + var.offset, uuuu, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, uint64_t ui1)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        std::memcpy((uint8_t*)dataPtr + var.offset, &ui1, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, uint64_t ui1, uint64_t ui2)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        uint64_t uu[2] = { ui1, ui2 };
        std::memcpy((uint8_t*)dataPtr + var.offset, uu, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, uint64_t ui1, uint64_t ui2, uint64_t ui3)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        uint64_t uuu[3] = { ui1, ui2, ui3 };
        std::memcpy((uint8_t*)dataPtr + var.offset, uuu, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, uint64_t ui1, uint64_t ui2, uint64_t ui3, uint64_t ui4)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        uint64_t uuuu[4] = { ui1, ui2, ui3, ui4 };
        std::memcpy((uint8_t*)dataPtr + var.offset, uuuu, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, float f1)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        std::memcpy((uint8_t*)dataPtr + var.offset, &f1, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, float f1, float f2)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        float ff[2] = { f1, f2 };
        std::memcpy((uint8_t*)dataPtr + var.offset, ff, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, float f1, float f2, float f3)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        float fff[3] = { f1, f2, f3 };
        std::memcpy((uint8_t*)dataPtr + var.offset, fff, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, float f1, float f2, float f3, float f4)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        float ffff[4] = { f1, f2, f3, f4 };
        std::memcpy((uint8_t*)dataPtr + var.offset, ffff, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, const void* pointer)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        if (var.type >= OWL_USER_TYPE_BEGIN)
            std::memcpy((uint8_t*)dataPtr + var.offset, pointer, GetDataTypeInfo(var.type).sizeInBytes);
        else // OWL_RAW_POINTER (a.k.a. OWL_ULONG)
            std::memcpy((uint8_t*)dataPtr + var.offset, &pointer, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, Buffer* buffer)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        if (buffer != nullptr)
            std::memcpy((uint8_t*)dataPtr + var.offset, &buffer->dataPtr, GetDataTypeInfo(var.type).sizeInBytes);
        else
            std::memset((uint8_t*)dataPtr + var.offset, 0, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, Group* group)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;

        if (group != nullptr)
            std::memcpy((uint8_t*)dataPtr + var.offset, &group->traversableHandle, GetDataTypeInfo(var.type).sizeInBytes);
        else
            std::memset((uint8_t*)dataPtr + var.offset, 0, GetDataTypeInfo(var.type).sizeInBytes);
    }

    void VarDecl::set(const char* name, Texture* tex)
    {
        OWLVarDecl var = findVariable(name);

        if (var.name == nullptr)
            return;
        if (tex != nullptr)
            std::memcpy((uint8_t*)dataPtr + var.offset, &tex->textureHandle, GetDataTypeInfo(var.type).sizeInBytes);
        else
            std::memset((uint8_t*)dataPtr + var.offset, 0, GetDataTypeInfo(var.type).sizeInBytes);
    }

    OWLVarDecl VarDecl::findVariable(const char* name)
    {
        OWLVarDecl var = { nullptr };

        for (std::size_t i = 0; i < inputVars.size(); ++i)
        {
            if (strcmp(name, inputVars[i].name) == 0)
            {
                var = inputVars[i];
                break;
            }
        }

        if (var.name == nullptr)
        {
            FAKE_LOG(fake::logging::Level::Error) << "Variable " << name << " not present in params";
        }

        return var;
    }

} // fake
